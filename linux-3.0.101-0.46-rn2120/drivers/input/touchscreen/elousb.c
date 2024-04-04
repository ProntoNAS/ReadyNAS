/*
 *  Copyright (c) 1999-2001 Vojtech Pavlik
 *
 *  Elo USB touchscreen support
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@suse.cz>, or by paper mail:
 * Vojtech Pavlik, Simunkova 1594, Prague 8, 182 00 Czech Republic
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

/*
 * Version Information
 */
#define DRIVER_VERSION "v1.1"
#define DRIVER_AUTHOR "Vojtech Pavlik <vojtech@suse.cz>"
#define DRIVER_DESC "Elo USB touchscreen driver"
#define DRIVER_LICENSE "GPL"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

/* timing constants */
#define PERIODIC_READ_INTERVAL (HZ)
#define SMARTSET_CMD_TIMEOUT 2000 /* msec */

/* Elo SmartSet commands */
#define USB_FLUSH_SMARTSET_RESPONSES	0x02 /* Flush all pending smartset responses */
#define USB_SEND_SMARTSET_COMMAND	0x05 /* Send a smartset command */
#define USB_GET_SMARTSET_RESPONSE	0x06 /* Get a smartset response */
#define ELO_DIAG			0x64 /* Diagnostics command */
#define ELO_SMARTSET_PACKET_SIZE	8

struct workqueue_struct *elo_wq;
static bool use_fw_quirk = true;
module_param(use_fw_quirk, bool, true);
MODULE_PARM_DESC(use_fw_quirk, "Do periodic pokes for broken M firmwares (default = true)");

struct elousb {
	char name[128];
	char phys[64];
	struct usb_device *usbdev;
	struct input_dev *dev;
	struct urb *irq;

	unsigned char *data;
	dma_addr_t data_dma;
	struct timer_list timer;
	struct work_struct workaround_work;
};

static void elousb_irq(struct urb *urb)
{
	struct elousb *elo = urb->context;
	unsigned char *data = elo->data;
	struct input_dev *dev = elo->dev;
	int status;

	switch (urb->status) {
		case 0:            /* success */
			break;
		case -ECONNRESET:    /* unlink */
		case -ENOENT:
		case -ESHUTDOWN:
			return;
			/* -EPIPE:  should clear the halt */
		default:        /* error */
			goto resubmit;
	}

	if (data[0] != 'T')    /* Mandatory ELO packet marker */
		return;


	input_report_abs(dev, ABS_X, ((u32)data[3] << 8) | data[2]);
	input_report_abs(dev, ABS_Y, ((u32)data[5] << 8) | data[4]);

	input_report_abs(dev, ABS_PRESSURE,
			(data[1] & 0x80) ? (((u32)data[7] << 8) | data[6]): 0);

	if (data[1] & 0x03) {
		input_report_key(dev, BTN_TOUCH, 1);
		input_sync(dev);
	}

	if (data[1] & 0x04)
		input_report_key(dev, BTN_TOUCH, 0);

	input_sync(dev);

	if (elo->timer.function)
		mod_timer(&elo->timer, jiffies + PERIODIC_READ_INTERVAL);

resubmit:
	status = usb_submit_urb (urb, GFP_ATOMIC);
	if (status)
		err ("can't resubmit intr, %s-%s/input0, status %d",
				elo->usbdev->bus->bus_name,
				elo->usbdev->devpath, status);
}

/* timer function, schedule the device query to be run in process context */
static void elousb_timer(unsigned long data)
{
	struct elousb *elo = (struct elousb *) data;

	if (!work_pending(&elo->workaround_work))
		queue_work(elo_wq, &elo->workaround_work);
}

static int flush_smartset_responses(struct usb_device *dev)
{
	return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
				USB_FLUSH_SMARTSET_RESPONSES,
				USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
				0, 0, NULL, 0, USB_CTRL_SET_TIMEOUT);
}

static int smartset_send_get(struct usb_device *dev, __u8 command, void *data)
{
	int pipe;

	if (command != USB_SEND_SMARTSET_COMMAND && command != USB_GET_SMARTSET_RESPONSE)
		return -1;

	pipe = (command == USB_GET_SMARTSET_RESPONSE ? usb_rcvctrlpipe(dev, 0) : usb_sndctrlpipe(dev, 0));
	return usb_control_msg(dev, pipe, command,
				(command == USB_GET_SMARTSET_RESPONSE ? USB_DIR_IN : USB_DIR_OUT)
				| USB_TYPE_VENDOR | USB_RECIP_DEVICE,
				0, 0, data, ELO_SMARTSET_PACKET_SIZE, SMARTSET_CMD_TIMEOUT);
}

static void workaround_fn(struct work_struct *work)
{
	struct elousb *elo = (struct elousb *) container_of(work, struct elousb, workaround_work);
	struct usb_device *dev = elo->usbdev;
	unsigned char *buffer;
	int io_ret;

	if (!(buffer = kzalloc(ELO_SMARTSET_PACKET_SIZE, GFP_KERNEL)))
		goto fail;

	if ((io_ret = flush_smartset_responses(dev)) < 0) {
		err("initial FLUSH_SMARTSET_RESPONSES failed, error %d", io_ret);
		goto fail;
	}

	/* send Diagnostics command */
	*buffer = ELO_DIAG;
	if ((io_ret = smartset_send_get(dev, USB_SEND_SMARTSET_COMMAND, buffer)) < 0) {
		err("send Diagnostics Command failed, error %d", io_ret);
		goto fail;
	}

	/* get the result */
	if ((io_ret = smartset_send_get(dev, USB_GET_SMARTSET_RESPONSE, buffer)) < 0) {
		err("get Diagnostics Command response failed, error %d", io_ret);
		goto fail;
	}

	/* read the ack */
	if (*buffer != 'A') {
		if ((io_ret = smartset_send_get(dev, USB_GET_SMARTSET_RESPONSE, buffer)) < 0) {
			err("get acknowledge response failed, error %d", io_ret);
			goto fail;
		}
	}

fail:
	if ((io_ret = flush_smartset_responses(dev)) < 0)
		err("final FLUSH_SMARTSET_RESPONSES failed, error %d", io_ret);
	kfree(buffer);
	mod_timer(&elo->timer, jiffies + PERIODIC_READ_INTERVAL);
	return;
}

static int elousb_open(struct input_dev *dev)
{
	struct elousb *elo = input_get_drvdata(dev);

	elo->irq->dev = elo->usbdev;
	if (usb_submit_urb(elo->irq, GFP_KERNEL))
		return -EIO;

	return 0;
}

static void elousb_close(struct input_dev *dev)
{
	struct elousb *elo = input_get_drvdata(dev);

	usb_kill_urb(elo->irq);
}

/* not all Elo devices need the periodic HID descriptor reads, only
 * firmware version M needs this */
static int has_broken_firmware(struct usb_device *dev) {
        struct usb_device *hub = dev->parent;
        struct usb_device *child = NULL;
        u16 fw_lvl = le16_to_cpu(dev->descriptor.bcdDevice);
        u16 child_vid, child_pid;
        int i;
                    
        if (!use_fw_quirk)
                return 0;
        if (!hub)
                return 0;
        if (fw_lvl != 0x10d)
                return 0;
        printk(KERN_INFO "elousb: touchscreen has FW Level: 0x10d\n");
         /*iterate sibling devices of the touch controller*/
        for (i=0; i<hub->maxchild; i++) {
                child = hub->children[i];
                if (!child)
                        continue;
                child_vid = le16_to_cpu(child->descriptor.idVendor);
                child_pid = le16_to_cpu(child->descriptor.idProduct);
                printk(KERN_INFO "elousb: sibling device %04x:%04x on port %d\n",
                        child_vid, child_pid, i+1);
                /* If one of the devices below is present attached as a sibling of 
                    the touch controller then  this is a newer IBM 4820 monitor that 
                    Does Not need the IBM-requested workaround if fw level is
                    0x010d - aka 'M'.
                    No other HW can have this combination.
                */
                if (child_vid==0x04b3) {
                        switch (child_pid) {
                        case 0x4676: /*4820 21x Video*/
                        case 0x4677: /*4820 51x Video*/
                        case 0x4678: /*4820 2Lx Video*/
                        case 0x4679: /*4820 5Lx Video*/
                                return 0;
                         }
                }
        }
        return 1;
}

static int elousb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	struct hid_descriptor *hdesc;
	struct elousb *elo;
	struct input_dev *input_dev;
	int pipe, i;
	unsigned int rsize = 0;
	int error = -ENOMEM;
	char *rdesc;

	interface = intf->cur_altsetting;

	if (interface->desc.bNumEndpoints != 1)
		return -ENODEV;

	endpoint = &interface->endpoint[0].desc;
	if (!(endpoint->bEndpointAddress & USB_DIR_IN))
		return -ENODEV;
	if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) != USB_ENDPOINT_XFER_INT)
		return -ENODEV;

	if (usb_get_extra_descriptor(interface, HID_DT_HID, &hdesc) &&
			(!interface->desc.bNumEndpoints ||
			 usb_get_extra_descriptor(&interface->endpoint[0], HID_DT_HID, &hdesc))) {
		err("HID class descriptor not present");
		return -ENODEV;
	}

	for (i = 0; i < hdesc->bNumDescriptors; i++)
		if (hdesc->desc[i].bDescriptorType == HID_DT_REPORT)
			rsize = le16_to_cpu(hdesc->desc[i].wDescriptorLength);

	if (!rsize || rsize > HID_MAX_DESCRIPTOR_SIZE) {
		err("weird size of report descriptor (%u)", rsize);
		return -ENODEV;
	}


	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);

	elo = kzalloc(sizeof(struct elousb), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!elo || !input_dev)
		goto fail1;

	elo->data = usb_alloc_coherent(dev, 8, GFP_ATOMIC, &elo->data_dma);
	if (!elo->data)
		goto fail1;

	elo->irq = usb_alloc_urb(0, GFP_KERNEL);
	if (!elo->irq)
		goto fail2;

	if (!(rdesc = kmalloc(rsize, GFP_KERNEL)))
		goto fail3;

	elo->usbdev = dev;
	elo->dev = input_dev;

	if ((error = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
					HID_REQ_SET_IDLE, USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0,
					interface->desc.bInterfaceNumber,
					NULL, 0, USB_CTRL_SET_TIMEOUT)) < 0) {
		err("setting HID idle timeout failed, error %d", error);
		error = -ENODEV;
		goto fail4;
	}

	if ((error = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
					USB_REQ_GET_DESCRIPTOR, USB_RECIP_INTERFACE | USB_DIR_IN,
					HID_DT_REPORT << 8, interface->desc.bInterfaceNumber,
					rdesc, rsize, USB_CTRL_GET_TIMEOUT)) < rsize) {
		err("reading HID report descriptor failed, error %d", error);
		error = -ENODEV;
		goto fail4;
	}

	if (dev->manufacturer)
		strlcpy(elo->name, dev->manufacturer, sizeof(elo->name));

	if (dev->product) {
		if (dev->manufacturer)
			strlcat(elo->name, " ", sizeof(elo->name));
		strlcat(elo->name, dev->product, sizeof(elo->name));
	}

	if (!strlen(elo->name))
		snprintf(elo->name, sizeof(elo->name),
				"Elo touchscreen %04x:%04x",
				le16_to_cpu(dev->descriptor.idVendor),
				le16_to_cpu(dev->descriptor.idProduct));

	usb_make_path(dev, elo->phys, sizeof(elo->phys));
	strlcat(elo->phys, "/input0", sizeof(elo->phys));

	input_dev->name = elo->name;
	input_dev->phys = elo->phys;
	usb_to_input_id(dev, &input_dev->id);
	input_dev->dev.parent = &intf->dev;

	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_ABS);
	set_bit(BTN_TOUCH, input_dev->keybit);
	input_dev->absbit[0] = BIT(ABS_X) | BIT(ABS_Y);
	set_bit(ABS_PRESSURE, input_dev->absbit);

	input_set_abs_params(input_dev, ABS_X, 0, 4000, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, 3840, 0, 0);
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 256, 0, 0);

	input_set_drvdata(input_dev, elo);

	input_dev->open = elousb_open;
	input_dev->close = elousb_close;

	usb_fill_int_urb(elo->irq, dev, pipe, elo->data, 8,
			elousb_irq, elo, endpoint->bInterval);
	elo->irq->transfer_dma = elo->data_dma;
	elo->irq->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	if (has_broken_firmware(dev)) {
		printk(KERN_INFO "elousb: broken firmware found, installing workaround\n");
		INIT_WORK(&elo->workaround_work, &workaround_fn);
		init_timer(&elo->timer);
		elo->timer.data = (unsigned long) elo;
		elo->timer.function = elousb_timer;
		elo->timer.expires = jiffies + PERIODIC_READ_INTERVAL;
		add_timer(&elo->timer);
	}

	if (input_register_device(elo->dev)) {
		printk(KERN_ERR "elousb: failed to register input device\n");
		goto fail4;
	}

	usb_set_intfdata(intf, elo);
	return 0;

fail4:
	kfree(rdesc);
fail3:
	usb_free_urb(elo->irq);
fail2:
	usb_free_coherent(dev, 8, elo->data, elo->data_dma);
fail1:
	input_free_device(input_dev);
	kfree(elo);
	return -ENOMEM;
}

static void elousb_disconnect(struct usb_interface *intf)
{
	struct elousb *elo = usb_get_intfdata (intf);

	usb_set_intfdata(intf, NULL);
	if (elo) {
		usb_kill_urb(elo->irq);
		input_unregister_device(elo->dev);
		usb_free_urb(elo->irq);
		usb_free_coherent(interface_to_usbdev(intf), 8, elo->data,
				  elo->data_dma);
		if (elo->timer.function) {
			del_timer_sync(&elo->timer);
			flush_work(&elo->workaround_work);
		}
		kfree(elo);
	}
}

static struct usb_device_id elousb_id_table [] = {
	{ USB_DEVICE(0x04e7, 0x0009) }, /* CarrolTouch 4000U */
	{ USB_DEVICE(0x04e7, 0x0030) }, /* CarrolTouch 4500U */
	{ }    /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, elousb_id_table);

static struct usb_driver elousb_driver = {
	.name        = "elousb",
	.probe        = elousb_probe,
	.disconnect    = elousb_disconnect,
	.id_table    = elousb_id_table,
};

static int __init elousb_init(void)
{
	int retval;
	if (!(elo_wq = create_singlethread_workqueue("elousb")))
		return -ENOMEM;
	retval = usb_register(&elousb_driver);
	if (retval == 0)
		printk(KERN_INFO KBUILD_MODNAME ": " DRIVER_VERSION ":" DRIVER_DESC);
	return retval;
}

static void __exit elousb_exit(void)
{
	usb_deregister(&elousb_driver);
	destroy_workqueue(elo_wq);
}

module_init(elousb_init);
module_exit(elousb_exit);
