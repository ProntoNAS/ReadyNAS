/*
 * Copyright (C) 2014,2015 NETGEAR, Inc.  All rights reserved.
 * Copyright (C) 2014,2015 Hiro Sugawara  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include "ctree.h"
#include "disk-io.h"
#include "cslink.h"

char *__btrfs_proc_mdcsrepair_debug;

static struct {
	void *func;
	u8 inverted;	/* Keep this u8 or results are unpredictable. */
	char *name;
} csfunc2algo[] = {
	{(void *)btrfs_csum_final, ~0, "crc32c_le"},
	{(void *)btrfs_csum_data, 0, "crc32c_le"},
	{NULL, 0, NULL},
};

/* Current actual message length ~ 300 (5/28/2014) */
#define MAX_XML	(512 - sizeof(struct list_head))

static struct mdcs_msg {
	struct list_head list;
	char buffer[];
} mdcs_msg_q;
static spinlock_t mdcs_lock;
static DECLARE_WAIT_QUEUE_HEAD(mdcs_waiter);
static atomic_t mdcs_q_len = ATOMIC_INIT(0);
#define MDCS_Q_MAX	200

static struct mdcs_msg *retrieve_q(void)
{
	struct mdcs_msg *m = NULL;

	spin_lock(&mdcs_lock);
	if (!list_empty(&mdcs_msg_q.list)) {
		m = list_first_entry(&mdcs_msg_q.list, struct mdcs_msg, list);
		list_del(&m->list);
	}
	spin_unlock(&mdcs_lock);
	if (m)
		atomic_dec(&mdcs_q_len);

	return m;
}

static void append_q(struct mdcs_msg *msg)
{
	spin_lock(&mdcs_lock);
	list_add_tail(&msg->list, &mdcs_msg_q.list);
	spin_unlock(&mdcs_lock);
	atomic_inc(&mdcs_q_len);
	wake_up(&mdcs_waiter);
}

/*
 * Do not change this function name as it is used to validate the XML
 * message by the user land code.
 */
void mdcsrepair_procfs(const char *filesystem, const char *place,
		u64 inum,
		const u8 *expected, const u8 *computed, size_t cslen,
		const void *csfunc,
		u64 dpos, size_t len,
		dev_t dev, dev_t adev, u64 fpos, u64 flags)
{
	char expbuf[cslen * 2 + 1], compbuf[cslen * 2 + 1];
	char *p, *name = NULL;
	int i, remainder;
	unsigned int inverted = 0;
	struct mdcs_msg *msg;

	if (dpos == (u64)~0) {
		pr_err("%s: Invalid dpos (-1)\n", __func__);
		return;
	}

	if ((i = atomic_read(&mdcs_q_len)) > MDCS_Q_MAX) {
		pr_warn("%s: Too many (%d) checksum failures in queue.",
			__func__, i);
		return;
	}

	if (!dev)
		return;

	for (i = 0; i < ARRAY_SIZE(csfunc2algo); i++)
		if (csfunc2algo[i].func == csfunc) {
			name = csfunc2algo[i].name;
			inverted = csfunc2algo[i].inverted;
			break;
		}

	if (!name) {
		pr_err("%s: checksum algorithm not found for %p\n",
			__func__, csfunc);
		return;
	}

	for (i = 0, p = expbuf; i < cslen; i++) {
		sprintf(p, "%02x", expected[i] ^ inverted);
		p +=2;
	}
	*p = '\0';
	for (i = 0, p = compbuf; i < cslen; i++) {
		sprintf(p, "%02x", computed[i] ^ inverted);
		p +=2;
	}
	*p = '\0';

	if (!(msg = kmalloc(MAX_XML + sizeof(*msg), GFP_ATOMIC))) {
		pr_err("%s: get_free_page failed.", __func__);
		return;
	}

	remainder = MAX_XML - (strlen("/></>") + strlen(__func__));
	if (snprintf(msg->buffer, remainder,
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?><%s>"
		"<data offset=\"%llu\" length=\"%lu\" position=\"%s\"/>"
		"<checksum expected=\"%s\" computed=\"%s\" "
			"length=\"%lu\" algorithm=\"%s\"/>"
		"<device major=\"%u\" minor=\"%u\" filesystem=\"%s\"/>"
		"<place function=\"%s\"/>"
		"<file inode=\"%llu\"",
		__func__,
		dpos, (unsigned long)len,
		(flags & CSL_INTRAFILEPOS) ? "file" : "disk",
		expbuf, compbuf, (unsigned long)cslen, name,
		MAJOR(dev), MINOR(dev),
		filesystem,
		place ? place : "UNKOWN",
		inum
		) >= remainder) {
		pr_err("%s: Buffer overflowed\n", __func__);
		goto nlmsg_failure2;
	}
	remainder -= strlen(msg->buffer);

	if (fpos != (u64)~0) {
		int len;
		p = msg->buffer + strlen(msg->buffer);
		if ((len = snprintf(p, remainder, " offset=\"%llu\"", fpos)) >=
				remainder) {
			pr_err("%s: Buffer overflowed\n", __func__);
			goto nlmsg_failure2;
		}
		remainder -= len;
	}
	strncat(msg->buffer, "/>", remainder);

	if (adev) {
		int len;
		p = msg->buffer + strlen(msg->buffer);
		if ((len = snprintf(p, remainder,
			"<anondevice major=\"%u\" minor=\"%d\"/>",
			MAJOR(adev), MINOR(adev))) >= remainder) {
			pr_err("%s: Buffer overflowed\n", __func__);
			goto nlmsg_failure2;
		}
		remainder -= len;
	}

	p = msg->buffer + strlen(msg->buffer);
	sprintf(p, "</%s>", __func__);

	append_q(msg);
	return;

nlmsg_failure2:
	kfree(msg);
}

/*
 * For debugging, "src/file.c:function" enables a fake checksum failure
 * to send netlink message whenever the function is called.
 * A null string for src/file.c or function works as a wild card; thus
 * ":" catches every checksum calculation. A null string or NULL pointer
 * ("") disables debugging.
 */
static struct proc_dir_entry *btrfs_proc_root,
				*btrfs_proc_mdcsrepair, *btrfs_proc_mdcsevent;

static int btrfs_proc_mdcsrepair_read(struct seq_file *f, void *v)
{
	if (__btrfs_proc_mdcsrepair_debug && *__btrfs_proc_mdcsrepair_debug)
		seq_printf(f, "%s", __btrfs_proc_mdcsrepair_debug);
	seq_putc(f, '\n');
	return 0;
}

static ssize_t btrfs_proc_mdcsrepair_write(struct file *file,
			const char __user *buffer,
			size_t count, loff_t *ppos)
{
	ssize_t rv;

	kfree(__btrfs_proc_mdcsrepair_debug);
	if (!(__btrfs_proc_mdcsrepair_debug = kmalloc(count + 1, GFP_KERNEL)))
		return -ENOMEM;
	if ((rv = copy_from_user(__btrfs_proc_mdcsrepair_debug,
				buffer, count))) {
		kfree(__btrfs_proc_mdcsrepair_debug);
		__btrfs_proc_mdcsrepair_debug = NULL;
		return rv;
	}
	if (__btrfs_proc_mdcsrepair_debug[count - 1] == '\n')
		__btrfs_proc_mdcsrepair_debug[count - 1] = '\0';
	else
		__btrfs_proc_mdcsrepair_debug[count] = '\0';
	if (!__btrfs_proc_mdcsrepair_debug[0]) {
		kfree(__btrfs_proc_mdcsrepair_debug);
		__btrfs_proc_mdcsrepair_debug = NULL;
	}
	return count;
}

static int btrfs_proc_mdcsrepair_open(struct inode *inode, struct file *file)
{
	return single_open(file, btrfs_proc_mdcsrepair_read, NULL);
}

static const struct file_operations btrfs_proc_mdcsrepair_fops = {
	.owner	= THIS_MODULE,
	.llseek	= seq_lseek,
	.read	= seq_read,
	.write	= btrfs_proc_mdcsrepair_write,
	.open	= btrfs_proc_mdcsrepair_open,
	.release= single_release,
};

static int btrfs_proc_mdcsevent_read(struct seq_file *f, void *v)
{
	struct mdcs_msg *m = NULL;
	int err = 0;

	do {
		/* wait for repair requests to come */
		err = wait_event_interruptible(mdcs_waiter,
				!list_empty(&mdcs_msg_q.list));
	} while (!err && !(m = retrieve_q()));

	if (m) {
		seq_printf(f, "%s", m->buffer);
		kfree(m);
	}
	return err;
}

static int btrfs_proc_mdcsevent_open(struct inode *inode, struct file *file)
{
	return single_open(file, btrfs_proc_mdcsevent_read, NULL);
}

static const struct file_operations btrfs_proc_mdcsevent_fops = {
	.owner	= THIS_MODULE,
	.llseek	= seq_lseek,
	.read	= seq_read,
	.open	= btrfs_proc_mdcsevent_open,
	.release= single_release,
};

static int __init btrfs_mdcs_init(void)
{
	spin_lock_init(&mdcs_lock);
	INIT_LIST_HEAD(&mdcs_msg_q.list);
	atomic_set(&mdcs_q_len, 0);

	if (!(btrfs_proc_root = proc_mkdir("fs/btrfs", NULL))) {
		return -EIO;
	}
	if (!(btrfs_proc_mdcsrepair =
		proc_create("mdcsrepair",
			S_IFREG | S_IRUGO | S_IWUSR, btrfs_proc_root,
			&btrfs_proc_mdcsrepair_fops))) {
		remove_proc_entry("fs/btrfs", NULL);
		return -EIO;
	}

	if (!(btrfs_proc_mdcsevent =
		proc_create("mdcsevent",
			S_IFREG | S_IRUSR, btrfs_proc_root,
			&btrfs_proc_mdcsevent_fops))) {
		remove_proc_entry("fs/btrfs/mdcsrepair", NULL);
		remove_proc_entry("fs/btrfs", NULL);
		return -EIO;
	}

	return 0;
}

static void __exit btrfs_mdcs_exit(void)
{
	struct mdcs_msg *m;

	while ((m = retrieve_q()))
		kfree(m);
	remove_proc_entry("fs/btrfs/mdcsevent", NULL);
	remove_proc_entry("fs/btrfs/mdcsrepair", NULL);
	kfree(__btrfs_proc_mdcsrepair_debug);
	__btrfs_proc_mdcsrepair_debug = NULL;
}

module_init(btrfs_mdcs_init);
module_exit(btrfs_mdcs_exit);

MODULE_AUTHOR("<hiro.sugawara@netgear.com>");
MODULE_DESCRIPTION("BTRFS checksum corrector");
MODULE_LICENSE("GPL");
