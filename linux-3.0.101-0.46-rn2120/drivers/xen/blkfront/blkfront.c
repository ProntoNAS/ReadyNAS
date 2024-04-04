/******************************************************************************
 * blkfront.c
 * 
 * XenLinux virtual block-device driver.
 * 
 * Copyright (c) 2003-2004, Keir Fraser & Steve Hand
 * Modifications by Mark A. Williamson are (c) Intel Research Cambridge
 * Copyright (c) 2004, Christian Limpach
 * Copyright (c) 2004, Andrew Warfield
 * Copyright (c) 2005, Christopher Clark
 * Copyright (c) 2005, XenSource Ltd
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation; or, when distributed
 * separately from the Linux kernel or incorporated into other
 * software packages, subject to the following license:
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this source file (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <linux/version.h>
#include "block.h"
#include <linux/cdrom.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/log2.h>
#include <linux/scatterlist.h>
#include <linux/vmalloc.h>
#include <scsi/scsi.h>
#include <xen/evtchn.h>
#include <xen/xenbus.h>
#include <xen/interface/grant_table.h>
#include <xen/interface/io/protocols.h>
#include <xen/gnttab.h>
#include <asm/hypervisor.h>
#include <asm/maddr.h>

#ifdef HAVE_XEN_PLATFORM_COMPAT_H
#include <xen/platform-compat.h>
#endif

struct blk_resume_entry {
	struct list_head list;
	struct blk_shadow copy;
};

#define BLKIF_STATE_DISCONNECTED 0
#define BLKIF_STATE_CONNECTED    1
#define BLKIF_STATE_SUSPENDED    2


static void connect(struct blkfront_info *);
static void blkfront_closing(struct blkfront_info *);
static int blkfront_remove(struct xenbus_device *);
static int talk_to_backend(struct xenbus_device *, struct blkfront_info *);
static int setup_blkring(struct xenbus_device *, struct blkfront_info *);

static void kick_pending_request_queues(struct blkfront_info *);

static irqreturn_t blkif_int(int irq, void *dev_id);
static void blkif_restart_queue(struct work_struct *arg);
static int blkif_recover(struct blkfront_info *, unsigned int old_ring_size,
			 unsigned int new_ring_size);
static void blkif_completion(struct blk_shadow *);
static void blkif_free(struct blkfront_info *, int);


/**
 * Entry point to this code when a new device is created.  Allocate the basic
 * structures and the ring buffer for communication with the backend, and
 * inform the backend of the appropriate details for those.  Switch to
 * Initialised state.
 */
static int blkfront_probe(struct xenbus_device *dev,
			  const struct xenbus_device_id *id)
{
	int err, vdevice;
	struct blkfront_info *info;
	enum xenbus_state backend_state;

#ifndef CONFIG_XEN /* For HVM guests, do not take over CDROM devices. */
	char *type;

	type = xenbus_read(XBT_NIL, dev->nodename, "device-type", NULL);
	if (IS_ERR(type)) {
		xenbus_dev_fatal(dev, PTR_ERR(type), "reading dev type");
		return PTR_ERR(type);
	}
	if (!strncmp(type, "cdrom", 5)) {
		/*
		 * We are handed a cdrom device in a hvm guest; let the
		 * native cdrom driver handle this device.
		 */
		kfree(type);
		pr_notice("blkfront: ignoring CDROM %s\n", dev->nodename);
		return -ENXIO;
	}
	kfree(type);
#endif

	/* FIXME: Use dynamic device id if this is not set. */
	err = xenbus_scanf(XBT_NIL, dev->nodename,
			   "virtual-device", "%i", &vdevice);
	if (err != 1) {
		/* go looking in the extended area instead */
		err = xenbus_scanf(XBT_NIL, dev->nodename, "virtual-device-ext",
			"%i", &vdevice);
		if (err != 1) {
			xenbus_dev_fatal(dev, err, "reading virtual-device");
			return err;
		}
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		xenbus_dev_fatal(dev, -ENOMEM, "allocating info structure");
		return -ENOMEM;
	}

	spin_lock_init(&info->io_lock);
	mutex_init(&info->mutex);
	info->xbdev = dev;
	info->vdevice = vdevice;
	info->connected = BLKIF_STATE_DISCONNECTED;
	INIT_WORK(&info->work, blkif_restart_queue);
	INIT_LIST_HEAD(&info->resume_list);

	/* Front end dir is a number, which is used as the id. */
	info->handle = simple_strtoul(strrchr(dev->nodename,'/')+1, NULL, 0);
	dev_set_drvdata(&dev->dev, info);

	backend_state = xenbus_read_driver_state(dev->otherend);
	/*
	 * XenbusStateInitWait would be the correct state to enter here,
	 * but (at least) blkback considers this a fatal error.
	 */
	xenbus_switch_state(dev, XenbusStateInitialising);
	if (backend_state != XenbusStateInitWait)
		return 0;

	err = talk_to_backend(dev, info);
	if (err) {
		kfree(info);
		dev_set_drvdata(&dev->dev, NULL);
		return err;
	}

	return 0;
}


/**
 * We are reconnecting to the backend, due to a suspend/resume, or a backend
 * driver restart.  We tear down our blkif structure and recreate it, but
 * leave the device-layer structures intact so that this is transparent to the
 * rest of the kernel.
 */
static int blkfront_resume(struct xenbus_device *dev)
{
	struct blkfront_info *info = dev_get_drvdata(&dev->dev);
	enum xenbus_state backend_state;

	DPRINTK("blkfront_resume: %s\n", dev->nodename);

	blkif_free(info, info->connected == BLKIF_STATE_CONNECTED);

	backend_state = xenbus_read_driver_state(dev->otherend);
	/* See respective comment in blkfront_probe(). */
	xenbus_switch_state(dev, XenbusStateInitialising);
	if (backend_state != XenbusStateInitWait)
		return 0;

	return talk_to_backend(dev, info);
}


static void shadow_init(struct blk_shadow *shadow, unsigned int ring_size)
{
	unsigned int i = 0;

	WARN_ON(!ring_size);
	while (++i < ring_size)
		shadow[i - 1].req.id = i;
	shadow[i - 1].req.id = 0x0fffffff;
}

/* Common code used when first setting up, and when resuming. */
static int talk_to_backend(struct xenbus_device *dev,
			   struct blkfront_info *info)
{
	unsigned int ring_size, ring_order;
	unsigned int old_ring_size = RING_SIZE(&info->ring);
	const char *what = NULL;
	struct xenbus_transaction xbt;
	int err;

	if (dev->state >= XenbusStateInitialised)
		return 0;

	err = xenbus_scanf(XBT_NIL, dev->otherend,
			   "max-ring-pages", "%u", &ring_size);
	if (err != 1)
		ring_size = 0;
	else if (!ring_size)
		pr_warn("blkfront: %s: zero max-ring-pages\n", dev->nodename);
	err = xenbus_scanf(XBT_NIL, dev->otherend,
			   "max-ring-page-order", "%u", &ring_order);
	if (err != 1)
		ring_order = ring_size ? ilog2(ring_size) : 0;
	else if (!ring_size)
		/* nothing */;
	else if ((ring_size - 1) >> ring_order)
		pr_warn("blkfront: %s: max-ring-pages (%#x) inconsistent with"
			" max-ring-page-order (%u)\n",
			dev->nodename, ring_size, ring_order);
	else
		ring_order = ilog2(ring_size);

	if (ring_order > BLK_MAX_RING_PAGE_ORDER)
		ring_order = BLK_MAX_RING_PAGE_ORDER;
	/*
	 * While for larger rings not all pages are actually used, be on the
	 * safe side and set up a full power of two to please as many backends
	 * as possible.
	 */
	info->ring_size = ring_size = 1U << ring_order;

	/* Create shared ring, alloc event channel. */
	err = setup_blkring(dev, info);
	if (err)
		goto out;

again:
	err = xenbus_transaction_start(&xbt);
	if (err) {
		xenbus_dev_fatal(dev, err, "starting transaction");
		goto destroy_blkring;
	}

	if (ring_size == 1) {
		what = "ring-ref";
		err = xenbus_printf(xbt, dev->nodename, what, "%u",
				    info->ring_refs[0]);
		if (err)
			goto abort_transaction;
	} else {
		unsigned int i;
		char buf[16];

		what = "ring-page-order";
		err = xenbus_printf(xbt, dev->nodename, what, "%u",
				    ring_order);
		if (err)
			goto abort_transaction;
		what = "num-ring-pages";
		err = xenbus_printf(xbt, dev->nodename, what, "%u", ring_size);
		if (err)
			goto abort_transaction;
		what = buf;
		for (i = 0; i < ring_size; i++) {
			snprintf(buf, sizeof(buf), "ring-ref%u", i);
			err = xenbus_printf(xbt, dev->nodename, what, "%u",
					    info->ring_refs[i]);
			if (err)
				goto abort_transaction;
		}
 	}

	what = "event-channel";
	err = xenbus_printf(xbt, dev->nodename, what, "%u",
			    irq_to_evtchn_port(info->irq));
	if (err)
		goto abort_transaction;
	what = "protocol";
	err = xenbus_printf(xbt, dev->nodename, what, "%s",
			    XEN_IO_PROTO_ABI_NATIVE);
	if (err)
		goto abort_transaction;

	err = xenbus_transaction_end(xbt, 0);
	if (err) {
		if (err == -EAGAIN)
			goto again;
		xenbus_dev_fatal(dev, err, "completing transaction");
		goto destroy_blkring;
	}

	xenbus_switch_state(dev, XenbusStateInitialised);

	ring_size = RING_SIZE(&info->ring);
	switch (info->connected) {
	case BLKIF_STATE_DISCONNECTED:
		shadow_init(info->shadow, ring_size);
		break;
	case BLKIF_STATE_SUSPENDED:
		err = blkif_recover(info, old_ring_size, ring_size);
		if (err)
			goto out;
		break;
	}

	pr_info("blkfront: %s: ring-pages=%u nr_ents=%u\n",
		dev->nodename, info->ring_size, ring_size);

	return 0;

 abort_transaction:
	xenbus_transaction_end(xbt, 1);
	if (what)
		xenbus_dev_fatal(dev, err, "writing %s", what);
 destroy_blkring:
	blkif_free(info, 0);
 out:
	return err;
}


static int setup_blkring(struct xenbus_device *dev,
			 struct blkfront_info *info)
{
	blkif_sring_t *sring;
	int err;
	unsigned int nr;

	for (nr = 0; nr < info->ring_size; nr++) {
		info->ring_refs[nr] = GRANT_INVALID_REF;
		info->ring_pages[nr] = alloc_page(GFP_NOIO | __GFP_HIGH
						 | __GFP_HIGHMEM);
		if (!info->ring_pages[nr])
			break;
	}

	sring = nr == info->ring_size
		? vmap(info->ring_pages, nr, VM_MAP, PAGE_KERNEL)
		: NULL;
	if (!sring) {
		while (nr--)
			__free_page(info->ring_pages[nr]);
		xenbus_dev_fatal(dev, -ENOMEM, "allocating shared ring");
		return -ENOMEM;
	}
	SHARED_RING_INIT(sring);
	FRONT_RING_INIT(&info->ring, sring,
			(unsigned long)info->ring_size << PAGE_SHIFT);

	sg_init_table(info->sg, BLKIF_MAX_SEGMENTS_PER_REQUEST);

	err = xenbus_multi_grant_ring(dev, nr, info->ring_pages,
				      info->ring_refs);
	if (err < 0)
		goto fail;

	err = bind_listening_port_to_irqhandler(
		dev->otherend_id, blkif_int, IRQF_SAMPLE_RANDOM, "blkif", info);
	if (err <= 0) {
		xenbus_dev_fatal(dev, err,
				 "bind_listening_port_to_irqhandler");
		goto fail;
	}
	info->irq = err;

	return 0;
fail:
	blkif_free(info, 0);
	return err;
}


/**
 * Callback received when the backend's state changes.
 */
static void backend_changed(struct xenbus_device *dev,
			    enum xenbus_state backend_state)
{
	struct blkfront_info *info = dev_get_drvdata(&dev->dev);
	struct block_device *bd;

	DPRINTK("blkfront:backend_changed.\n");

	switch (backend_state) {
	case XenbusStateInitialising:
	case XenbusStateInitialised:
	case XenbusStateReconfiguring:
	case XenbusStateReconfigured:
	case XenbusStateUnknown:
		break;

	case XenbusStateInitWait:
		if (talk_to_backend(dev, info)) {
			dev_set_drvdata(&dev->dev, NULL);
			kfree(info);
		}
		break;

	case XenbusStateConnected:
		connect(info);
		break;

	case XenbusStateClosed:
		if (dev->state == XenbusStateClosed)
			break;
		/* Missed the backend's Closing state -- fallthrough */
	case XenbusStateClosing:
		mutex_lock(&info->mutex);
		if (dev->state == XenbusStateClosing) {
			mutex_unlock(&info->mutex);
			break;
		}

		bd = info->gd ? bdget_disk(info->gd, 0) : NULL;

		mutex_unlock(&info->mutex);

		if (bd == NULL) {
			xenbus_frontend_closed(dev);
			break;
		}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17)
		down(&bd->bd_sem);
#else
		mutex_lock(&bd->bd_mutex);
#endif
		if (bd->bd_openers) {
			xenbus_dev_error(dev, -EBUSY,
					 "Device in use; refusing to close");
			xenbus_switch_state(dev, XenbusStateClosing);
		} else
			blkfront_closing(info);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17)
		up(&bd->bd_sem);
#else
		mutex_unlock(&bd->bd_mutex);
#endif
		bdput(bd);
		break;
	}
}


/* ** Connection ** */

static void blkfront_setup_discard(struct blkfront_info *info)
{
	unsigned int discard_granularity;
	unsigned int discard_alignment;
	int discard_secure;

	info->feature_discard = 1;
	if (!xenbus_gather(XBT_NIL, info->xbdev->otherend,
			   "discard-granularity", "%u", &discard_granularity,
			   "discard-alignment", "%u", &discard_alignment,
			   NULL)) {
		info->discard_granularity = discard_granularity;
		info->discard_alignment = discard_alignment;
	}
	if (xenbus_scanf(XBT_NIL, info->xbdev->otherend,
			 "discard-secure", "%d", &discard_secure) != 1)
		discard_secure = 0;
	info->feature_secdiscard = !!discard_secure;
}

/*
 * Invoked when the backend is finally 'ready' (and has told produced
 * the details about the physical device - #sectors, size, etc).
 */
static void connect(struct blkfront_info *info)
{
	unsigned long long sectors;
	unsigned long sector_size;
	unsigned int binfo;
	int err, barrier, flush, discard;

	switch (info->connected) {
	case BLKIF_STATE_CONNECTED:
		/*
		 * Potentially, the back-end may be signalling
		 * a capacity change; update the capacity.
		 */
		err = xenbus_scanf(XBT_NIL, info->xbdev->otherend,
				   "sectors", "%Lu", &sectors);
		if (err != 1)
			return;
		err = xenbus_scanf(XBT_NIL, info->xbdev->otherend,
				   "sector-size", "%lu", &sector_size);
		if (err != 1)
			sector_size = 0;
		if (sector_size)
			blk_queue_logical_block_size(info->gd->queue,
						     sector_size);
		pr_info("Setting capacity to %Lu\n", sectors);
		set_capacity(info->gd, sectors);
		revalidate_disk(info->gd);

		/* fall through */
	case BLKIF_STATE_SUSPENDED:
		return;
	}

	DPRINTK("blkfront.c:connect:%s.\n", info->xbdev->otherend);

	err = xenbus_gather(XBT_NIL, info->xbdev->otherend,
			    "sectors", "%Lu", &sectors,
			    "info", "%u", &binfo,
			    "sector-size", "%lu", &sector_size,
			    NULL);
	if (err) {
		xenbus_dev_fatal(info->xbdev, err,
				 "reading backend fields at %s",
				 info->xbdev->otherend);
		return;
	}

	info->feature_flush = 0;
	info->flush_op = 0;

	err = xenbus_scanf(XBT_NIL, info->xbdev->otherend,
			   "feature-barrier", "%d", &barrier);
	/*
	 * If there's no "feature-barrier" defined, then it means
	 * we're dealing with a very old backend which writes
	 * synchronously; nothing to do.
	 *
	 * If there are barriers, then we use flush.
	 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
	if (err > 0 && barrier) {
		info->feature_flush = REQ_FLUSH | REQ_FUA;
		info->flush_op = BLKIF_OP_WRITE_BARRIER;
	}
	/*
	 * And if there is "feature-flush-cache" use that above
	 * barriers.
	 */
	err = xenbus_scanf(XBT_NIL, info->xbdev->otherend,
			   "feature-flush-cache", "%d", &flush);
	if (err > 0 && flush) {
		info->feature_flush = REQ_FLUSH;
		info->flush_op = BLKIF_OP_FLUSH_DISKCACHE;
	}
#else
	if (err <= 0)
		info->feature_flush = QUEUE_ORDERED_DRAIN;
	else if (barrier)
		info->feature_flush = QUEUE_ORDERED_TAG;
	else
		info->feature_flush = QUEUE_ORDERED_NONE;
#endif

	err = xenbus_scanf(XBT_NIL, info->xbdev->otherend,
			   "feature-discard", "%d", &discard);

	if (err > 0 && discard)
		blkfront_setup_discard(info);

	err = xlvbd_add(sectors, info->vdevice, binfo, sector_size, info);
	if (err) {
		xenbus_dev_fatal(info->xbdev, err, "xlvbd_add at %s",
				 info->xbdev->otherend);
		return;
	}

	err = xlvbd_sysfs_addif(info);
	if (err) {
		xenbus_dev_fatal(info->xbdev, err, "xlvbd_sysfs_addif at %s",
				 info->xbdev->otherend);
		return;
	}

	(void)xenbus_switch_state(info->xbdev, XenbusStateConnected);

	/* Kick pending requests. */
	spin_lock_irq(&info->io_lock);
	info->connected = BLKIF_STATE_CONNECTED;
	kick_pending_request_queues(info);
	spin_unlock_irq(&info->io_lock);

	add_disk(info->gd);

	info->is_ready = 1;

	register_vcd(info);
}

/**
 * Handle the change of state of the backend to Closing.  We must delete our
 * device-layer structures now, to ensure that writes are flushed through to
 * the backend.  Once is this done, we can switch to Closed in
 * acknowledgement.
 */
static void blkfront_closing(struct blkfront_info *info)
{
	unsigned long flags;

	DPRINTK("blkfront_closing: %d removed\n", info->vdevice);

	if (info->rq == NULL)
		goto out;

	spin_lock_irqsave(&info->io_lock, flags);
	/* No more blkif_request(). */
	blk_stop_queue(info->rq);
	/* No more gnttab callback work. */
	gnttab_cancel_free_callback(&info->callback);
	spin_unlock_irqrestore(&info->io_lock, flags);

	/* Flush gnttab callback work. Must be done with no locks held. */
	flush_work_sync(&info->work);

	xlvbd_sysfs_delif(info);

	unregister_vcd(info);

	xlvbd_del(info);

 out:
	if (info->xbdev)
		xenbus_frontend_closed(info->xbdev);
}


static int blkfront_remove(struct xenbus_device *dev)
{
	struct blkfront_info *info = dev_get_drvdata(&dev->dev);
	struct block_device *bd;
	struct gendisk *disk;

	DPRINTK("blkfront_remove: %s removed\n", dev->nodename);

	blkif_free(info, 0);

	mutex_lock(&info->mutex);

	disk = info->gd;
	bd = disk ? bdget_disk(disk, 0) : NULL;

	info->xbdev = NULL;
	mutex_unlock(&info->mutex);

	if (!bd) {
		kfree(info);
		return 0;
	}

	/*
	 * The xbdev was removed before we reached the Closed
	 * state. See if it's safe to remove the disk. If the bdev
	 * isn't closed yet, we let release take care of it.
	 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17)
	down(&bd->bd_sem);
#else
	mutex_lock(&bd->bd_mutex);
#endif
	info = disk->private_data;

	dev_warn(disk_to_dev(disk),
		 "%s was hot-unplugged, %d stale handles\n",
		 dev->nodename, bd->bd_openers);

	if (info && !bd->bd_openers) {
		blkfront_closing(info);
		disk->private_data = NULL;
		kfree(info);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17)
	up(&bd->bd_sem);
#else
	mutex_unlock(&bd->bd_mutex);
#endif
	bdput(bd);

	return 0;
}


static inline int GET_ID_FROM_FREELIST(
	struct blkfront_info *info)
{
	unsigned long free = info->shadow_free;
	BUG_ON(free >= RING_SIZE(&info->ring));
	info->shadow_free = info->shadow[free].req.id;
	info->shadow[free].req.id = 0x0fffffee; /* debug */
	return free;
}

static inline int ADD_ID_TO_FREELIST(
	struct blkfront_info *info, unsigned long id)
{
	if (info->shadow[id].req.id != id)
		return -EINVAL;
	if (!info->shadow[id].request)
		return -ENXIO;
	info->shadow[id].req.id  = info->shadow_free;
	info->shadow[id].request = NULL;
	info->shadow_free = id;
	return 0;
}

static const char *op_name(unsigned int op)
{
	static const char *const names[] = {
		[BLKIF_OP_READ] = "read",
		[BLKIF_OP_WRITE] = "write",
		[BLKIF_OP_WRITE_BARRIER] = "barrier",
		[BLKIF_OP_FLUSH_DISKCACHE] = "flush",
		[BLKIF_OP_PACKET] = "packet",
		[BLKIF_OP_DISCARD] = "discard",
	};

	if (op >= ARRAY_SIZE(names))
		return "unknown";

	return names[op] ?: "reserved";
}

static inline void flush_requests(struct blkfront_info *info)
{
	int notify;

	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&info->ring, notify);

	if (notify)
		notify_remote_via_irq(info->irq);
}

static void kick_pending_request_queues(struct blkfront_info *info)
{
	bool queued = false;

	/* Recover stage 3: Re-queue pending requests. */
	while (!list_empty(&info->resume_list) && !RING_FULL(&info->ring)) {
		/* Grab a request slot and copy shadow state into it. */
		struct blk_resume_entry *ent =
			list_first_entry(&info->resume_list,
					 struct blk_resume_entry, list);
		blkif_request_t *req =
			RING_GET_REQUEST(&info->ring, info->ring.req_prod_pvt);
		unsigned int i;

		*req = ent->copy.req;

		/* We get a new request id, and must reset the shadow state. */
		req->id = GET_ID_FROM_FREELIST(info);
		info->shadow[req->id] = ent->copy;
		info->shadow[req->id].req.id = req->id;

		/* Rewrite any grant references invalidated by susp/resume. */
		for (i = 0; i < req->nr_segments; i++)
			gnttab_grant_foreign_access_ref(req->seg[i].gref,
				info->xbdev->otherend_id,
				pfn_to_mfn(ent->copy.frame[i]),
				rq_data_dir(ent->copy.request) ?
				GTF_readonly : 0);

		info->ring.req_prod_pvt++;
		queued = true;

		__list_del_entry(&ent->list);
		kfree(ent);
	}

	/* Send off requeued requests */
	if (queued)
		flush_requests(info);

	if (!RING_FULL(&info->ring)) {
		/* Re-enable calldowns. */
		blk_start_queue(info->rq);
		/* Kick things off immediately. */
		do_blkif_request(info->rq);
	}
}

static void blkif_restart_queue(struct work_struct *arg)
{
	struct blkfront_info *info = container_of(arg, struct blkfront_info, work);
	spin_lock_irq(&info->io_lock);
	if (info->connected == BLKIF_STATE_CONNECTED)
		kick_pending_request_queues(info);
	spin_unlock_irq(&info->io_lock);
}

static void blkif_restart_queue_callback(void *arg)
{
	struct blkfront_info *info = (struct blkfront_info *)arg;
	schedule_work(&info->work);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
int blkif_open(struct inode *inode, struct file *filep)
{
	struct block_device *bd = inode->i_bdev;
#else
int blkif_open(struct block_device *bd, fmode_t mode)
{
#endif
	struct blkfront_info *info = bd->bd_disk->private_data;
	int err = 0;

	if (!info)
		/* xbdev gone */
		err = -ERESTARTSYS;
	else {
		mutex_lock(&info->mutex);

		if (!info->gd)
			/* xbdev is closed */
			err = -ERESTARTSYS;

		mutex_unlock(&info->mutex);
	}

	return err;
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
int blkif_release(struct inode *inode, struct file *filep)
{
	struct gendisk *disk = inode->i_bdev->bd_disk;
#else
int blkif_release(struct gendisk *disk, fmode_t mode)
{
#endif
	struct blkfront_info *info = disk->private_data;
	struct xenbus_device *xbdev;
	struct block_device *bd = bdget_disk(disk, 0);

	bdput(bd);
	if (bd->bd_openers)
		return 0;

	/*
	 * Check if we have been instructed to close. We will have
	 * deferred this request, because the bdev was still open.
	 */
	mutex_lock(&info->mutex);
	xbdev = info->xbdev;

	if (xbdev && xbdev->state == XenbusStateClosing) {
		/* pending switch to state closed */
		dev_info(disk_to_dev(disk), "releasing disk\n");
		blkfront_closing(info);
 	}

	mutex_unlock(&info->mutex);

	if (!xbdev) {
		/* sudden device removal */
		dev_info(disk_to_dev(disk), "releasing disk\n");
		blkfront_closing(info);
		disk->private_data = NULL;
		kfree(info);
	}

	return 0;
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
int blkif_ioctl(struct inode *inode, struct file *filep,
		unsigned command, unsigned long argument)
{
	struct block_device *bd = inode->i_bdev;
#else
int blkif_ioctl(struct block_device *bd, fmode_t mode,
		unsigned command, unsigned long argument)
{
#endif
	struct blkfront_info *info = bd->bd_disk->private_data;
	int i;

	DPRINTK_IOCTL("command: 0x%x, argument: 0x%lx, dev: 0x%04x\n",
		      command, (long)argument, inode->i_rdev);

	switch (command) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
	case HDIO_GETGEO: {
		struct hd_geometry geo;
		int ret;

                if (!argument)
                        return -EINVAL;

		geo.start = get_start_sect(bd);
		ret = blkif_getgeo(bd, &geo);
		if (ret)
			return ret;

		if (copy_to_user((struct hd_geometry __user *)argument, &geo,
				 sizeof(geo)))
                        return -EFAULT;

                return 0;
	}
#endif
	case CDROMMULTISESSION:
		DPRINTK("FIXME: support multisession CDs later\n");
		for (i = 0; i < sizeof(struct cdrom_multisession); i++)
			if (put_user(0, (char __user *)(argument + i)))
				return -EFAULT;
		return 0;

	case CDROM_GET_CAPABILITY:
		if (info->gd && (info->gd->flags & GENHD_FL_CD))
			return 0;
		return -EINVAL;

	default:
		if (info->mi && info->gd && info->rq) {
			switch (info->mi->major) {
			case SCSI_DISK0_MAJOR:
			case SCSI_DISK1_MAJOR ... SCSI_DISK7_MAJOR:
		        case SCSI_DISK8_MAJOR ... SCSI_DISK15_MAJOR:
		        case SCSI_CDROM_MAJOR:
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
				return scsi_cmd_ioctl(filep, info->gd, command,
						      (void __user *)argument);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
				return scsi_cmd_ioctl(filep, info->rq,
						      info->gd, command,
						      (void __user *)argument);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0) \
   && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0) \
       || LINUX_VERSION_CODE < KERNEL_VERSION(3,0,18))
				return scsi_cmd_ioctl(info->rq, info->gd,
						      mode, command,
						      (void __user *)argument);
#else
				return scsi_cmd_blk_ioctl(bd, mode, command,
							  (void __user *)argument);
#endif
			}
		}

		return -EINVAL; /* same return as native Linux */
	}

	return 0;
}


int blkif_getgeo(struct block_device *bd, struct hd_geometry *hg)
{
	/* We don't have real geometry info, but let's at least return
	   values consistent with the size of the device */
	sector_t nsect = get_capacity(bd->bd_disk);
	sector_t cylinders = nsect;

	hg->heads = 0xff;
	hg->sectors = 0x3f;
	sector_div(cylinders, hg->heads * hg->sectors);
	hg->cylinders = cylinders;
	if ((sector_t)(hg->cylinders + 1) * hg->heads * hg->sectors < nsect)
		hg->cylinders = 0xffff;
	return 0;
}


/*
 * Generate a Xen blkfront IO request from a blk layer request.  Reads
 * and writes are handled as expected.
 *
 * @req: a request struct
 */
static int blkif_queue_request(struct request *req)
{
	struct blkfront_info *info = req->rq_disk->private_data;
	unsigned long buffer_mfn;
	blkif_request_t *ring_req;
	unsigned long id;
	unsigned int fsect, lsect;
	int i, ref;
	grant_ref_t gref_head;
	struct scatterlist *sg;

	if (unlikely(info->connected != BLKIF_STATE_CONNECTED))
		return 1;

	if (gnttab_alloc_grant_references(
		BLKIF_MAX_SEGMENTS_PER_REQUEST, &gref_head) < 0) {
		gnttab_request_free_callback(
			&info->callback,
			blkif_restart_queue_callback,
			info,
			BLKIF_MAX_SEGMENTS_PER_REQUEST);
		return 1;
	}

	/* Fill out a communications ring structure. */
	ring_req = RING_GET_REQUEST(&info->ring, info->ring.req_prod_pvt);
	id = GET_ID_FROM_FREELIST(info);
	info->shadow[id].request = req;

	ring_req->id = id;
	ring_req->sector_number = (blkif_sector_t)blk_rq_pos(req);
	ring_req->handle = info->handle;

	ring_req->operation = rq_data_dir(req) ?
		BLKIF_OP_WRITE : BLKIF_OP_READ;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
	if (req->cmd_flags & (REQ_FLUSH | REQ_FUA))
#else
	if (req->cmd_flags & REQ_HARDBARRIER)
#endif
		ring_req->operation = info->flush_op;
	if (req->cmd_type == REQ_TYPE_BLOCK_PC)
		ring_req->operation = BLKIF_OP_PACKET;

	if (unlikely(req->cmd_flags & (REQ_DISCARD | REQ_SECURE))) {
		struct blkif_request_discard *discard = (void *)ring_req;

		/* id, sector_number and handle are set above. */
		discard->operation = BLKIF_OP_DISCARD;
		discard->flag = 0;
		discard->nr_sectors = blk_rq_sectors(req);
		if ((req->cmd_flags & REQ_SECURE) && info->feature_secdiscard)
			discard->flag = BLKIF_DISCARD_SECURE;
	} else {
		ring_req->nr_segments = blk_rq_map_sg(req->q, req, info->sg);
		BUG_ON(ring_req->nr_segments > BLKIF_MAX_SEGMENTS_PER_REQUEST);
		for_each_sg(info->sg, sg, ring_req->nr_segments, i) {
			buffer_mfn = page_to_phys(sg_page(sg)) >> PAGE_SHIFT;
			fsect = sg->offset >> 9;
			lsect = fsect + (sg->length >> 9) - 1;
			/* install a grant reference. */
			ref = gnttab_claim_grant_reference(&gref_head);
			BUG_ON(ref == -ENOSPC);

			gnttab_grant_foreign_access_ref(
				ref,
				info->xbdev->otherend_id,
				buffer_mfn,
				rq_data_dir(req) ? GTF_readonly : 0 );

			info->shadow[id].frame[i] = mfn_to_pfn(buffer_mfn);
			ring_req->seg[i] =
				(struct blkif_request_segment) {
					.gref       = ref,
					.first_sect = fsect,
					.last_sect  = lsect };
		}
	}

	info->ring.req_prod_pvt++;

	/* Keep a private copy so we can reissue requests when recovering. */
	info->shadow[id].req = *ring_req;

	gnttab_free_grant_references(gref_head);

	return 0;
}

/*
 * do_blkif_request
 *  read a block; request is in a request queue
 */
void do_blkif_request(struct request_queue *rq)
{
	struct blkfront_info *info = NULL;
	struct request *req;
	int queued;

	DPRINTK("Entered do_blkif_request\n");

	queued = 0;

	while ((req = blk_peek_request(rq)) != NULL) {
		info = req->rq_disk->private_data;

		if (RING_FULL(&info->ring))
			goto wait;

		blk_start_request(req);

		if ((req->cmd_type != REQ_TYPE_FS &&
		     (req->cmd_type != REQ_TYPE_BLOCK_PC || req->cmd_len)) ||
		    ((req->cmd_flags & (REQ_FLUSH | REQ_FUA)) &&
		     !info->flush_op)) {
			req->errors = (DID_ERROR << 16) |
				      (DRIVER_INVALID << 24);
			__blk_end_request_all(req, -EIO);
			continue;
		}

		DPRINTK("do_blk_req %p: cmd %p, sec %llx, "
			"(%u/%u) buffer:%p [%s]\n",
			req, req->cmd, (long long)blk_rq_pos(req),
			blk_rq_cur_sectors(req), blk_rq_sectors(req),
			req->buffer, rq_data_dir(req) ? "write" : "read");

		if (blkif_queue_request(req)) {
			blk_requeue_request(rq, req);
		wait:
			/* Avoid pointless unplugs. */
			blk_stop_queue(rq);
			break;
		}

		queued++;
	}

	if (queued != 0)
		flush_requests(info);
}


static irqreturn_t blkif_int(int irq, void *dev_id)
{
	struct request *req;
	blkif_response_t *bret;
	RING_IDX i, rp;
	unsigned long flags;
	struct blkfront_info *info = (struct blkfront_info *)dev_id;

	spin_lock_irqsave(&info->io_lock, flags);

	if (unlikely(info->connected != BLKIF_STATE_CONNECTED)) {
		spin_unlock_irqrestore(&info->io_lock, flags);
		return IRQ_HANDLED;
	}

 again:
	rp = info->ring.sring->rsp_prod;
	rmb(); /* Ensure we see queued responses up to 'rp'. */

	for (i = info->ring.rsp_cons; i != rp; i++) {
		unsigned long id;
		int ret;

		bret = RING_GET_RESPONSE(&info->ring, i);
		if (unlikely(bret->id >= RING_SIZE(&info->ring))) {
			/*
			 * The backend has messed up and given us an id that
			 * we would never have given to it (we stamp it up to
			 * RING_SIZE() - see GET_ID_FROM_FREELIST()).
			 */
			pr_warning("%s: response to %s has incorrect id (%#Lx)\n",
				   info->gd->disk_name,
				   op_name(bret->operation),
				   (unsigned long long)bret->id);
			continue;
		}
		id   = bret->id;
		req  = info->shadow[id].request;

		blkif_completion(&info->shadow[id]);

		ret = ADD_ID_TO_FREELIST(info, id);
		if (unlikely(ret)) {
			pr_warning("%s: id %#lx (response to %s) couldn't be recycled (%d)!\n",
				   info->gd->disk_name, id,
				   op_name(bret->operation), ret);
			continue;
		}

		ret = bret->status == BLKIF_RSP_OKAY ? 0 : -EIO;
		switch (bret->operation) {
			const char *kind;

		case BLKIF_OP_FLUSH_DISKCACHE:
		case BLKIF_OP_WRITE_BARRIER:
			kind = "";
			if (unlikely(bret->status == BLKIF_RSP_EOPNOTSUPP))
				ret = -EOPNOTSUPP;
			if (unlikely(bret->status == BLKIF_RSP_ERROR &&
				     info->shadow[id].req.nr_segments == 0)) {
				kind = "empty ";
				ret = -EOPNOTSUPP;
			}
			if (unlikely(ret)) {
				if (ret == -EOPNOTSUPP) {
					pr_warn("blkfront: %s: %s%s op failed\n",
					        info->gd->disk_name, kind,
						op_name(bret->operation));
					ret = 0;
				}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
				info->feature_flush = 0;
#else
				info->feature_flush = QUEUE_ORDERED_NONE;
#endif
			        xlvbd_flush(info);
			}
			/* fall through */
		case BLKIF_OP_READ:
		case BLKIF_OP_WRITE:
		case BLKIF_OP_PACKET:
			if (unlikely(bret->status != BLKIF_RSP_OKAY))
				DPRINTK("Bad return from blkdev %s request: %d\n",
					op_name(bret->operation),
					bret->status);

			__blk_end_request_all(req, ret);
			break;
		case BLKIF_OP_DISCARD:
			if (unlikely(bret->status == BLKIF_RSP_EOPNOTSUPP)) {
				struct request_queue *rq = info->rq;

				pr_warn("blkfront: %s: discard op failed\n",
					info->gd->disk_name);
				ret = -EOPNOTSUPP;
				info->feature_discard = 0;
				info->feature_secdiscard = 0;
				queue_flag_clear(QUEUE_FLAG_DISCARD, rq);
				queue_flag_clear(QUEUE_FLAG_SECDISCARD, rq);
			}
			__blk_end_request_all(req, ret);
			break;
		default:
			BUG();
		}
	}

	info->ring.rsp_cons = i;

	if (i != info->ring.req_prod_pvt) {
		int more_to_do;
		RING_FINAL_CHECK_FOR_RESPONSES(&info->ring, more_to_do);
		if (more_to_do)
			goto again;
	} else
		info->ring.sring->rsp_event = i + 1;

	kick_pending_request_queues(info);

	spin_unlock_irqrestore(&info->io_lock, flags);

	return IRQ_HANDLED;
}

static void blkif_free(struct blkfront_info *info, int suspend)
{
	/* Prevent new requests being issued until we fix things up. */
	spin_lock_irq(&info->io_lock);
	info->connected = suspend ?
		BLKIF_STATE_SUSPENDED : BLKIF_STATE_DISCONNECTED;
	/* No more blkif_request(). */
	if (info->rq)
		blk_stop_queue(info->rq);
	/* No more gnttab callback work. */
	gnttab_cancel_free_callback(&info->callback);
	spin_unlock_irq(&info->io_lock);

	/* Flush gnttab callback work. Must be done with no locks held. */
	flush_work_sync(&info->work);

	/* Free resources associated with old device channel. */
	vunmap(info->ring.sring);
	info->ring.sring = NULL;
	gnttab_multi_end_foreign_access(info->ring_size,
					info->ring_refs, info->ring_pages);
	if (info->irq)
		unbind_from_irqhandler(info->irq, info);
	info->irq = 0;
}

static void blkif_completion(struct blk_shadow *s)
{
	int i;

	if (s->req.operation == BLKIF_OP_DISCARD)
		return;
	for (i = 0; i < s->req.nr_segments; i++)
		gnttab_end_foreign_access(s->req.seg[i].gref, 0UL);
}

static int blkif_recover(struct blkfront_info *info,
			 unsigned int old_ring_size,
			 unsigned int ring_size)
{
	unsigned int i;
	struct blk_resume_entry *ent;
	LIST_HEAD(list);

	/* Stage 1: Make a safe copy of the shadow state. */
	for (i = 0; i < old_ring_size; i++) {
		/* Not in use? */
		if (!info->shadow[i].request)
			continue;
		ent = kmalloc(sizeof(*ent),
			      GFP_NOIO | __GFP_NOFAIL | __GFP_HIGH);
		if (!ent)
			break;
		ent->copy = info->shadow[i];
		list_add_tail(&ent->list, &list);
	}
	if (i < old_ring_size) {
		while (!list_empty(&list)) {
			ent = list_first_entry(&list, struct blk_resume_entry,
					       list);
			__list_del_entry(&ent->list);
			kfree(ent);
		}
		return -ENOMEM;
	}
	list_splice_tail(&list, &info->resume_list);

	/* Stage 2: Set up free list. */
	memset(&info->shadow, 0, sizeof(info->shadow));
	shadow_init(info->shadow, ring_size);
	info->shadow_free = info->ring.req_prod_pvt;

	(void)xenbus_switch_state(info->xbdev, XenbusStateConnected);

	spin_lock_irq(&info->io_lock);

	/* Now safe for us to use the shared ring */
	info->connected = BLKIF_STATE_CONNECTED;

	/* Kick any other new requests queued since we resumed */
	kick_pending_request_queues(info);

	spin_unlock_irq(&info->io_lock);

	return 0;
}

int blkfront_is_ready(struct xenbus_device *dev)
{
	struct blkfront_info *info = dev_get_drvdata(&dev->dev);

	return info->is_ready && info->xbdev;
}


/* ** Driver Registration ** */


static const struct xenbus_device_id blkfront_ids[] = {
	{ "vbd" },
	{ "" }
};
MODULE_ALIAS("xen:vbd");

static DEFINE_XENBUS_DRIVER(blkfront, ,
	.probe = blkfront_probe,
	.remove = blkfront_remove,
	.resume = blkfront_resume,
	.otherend_changed = backend_changed,
	.is_ready = blkfront_is_ready,
);


static int __init xlblk_init(void)
{
	if (!is_running_on_xen())
		return -ENODEV;

	return xenbus_register_frontend(&blkfront_driver);
}
module_init(xlblk_init);


static void __exit xlblk_exit(void)
{
	xenbus_unregister_driver(&blkfront_driver);
	xlbd_release_major_info();
}
module_exit(xlblk_exit);

MODULE_LICENSE("Dual BSD/GPL");
