/*
 * Copyright (C) 2014 NETGEAR, Inc.  All rights reserved.
 * Copyright (C) 2014 Hiro Sugawara  All rights reserved.
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
#include <linux/kthread.h>
#include <linux/list.h>
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

enum {
        MDCSREPAIR_NL_C_UNSPEC,
        MDCSREPAIR_NL_C_REQUEST,
        __MDCSREPAIR_NL_C_MAX,
};
#define MDCSREPAIR_NL_C_MAX (__MDCSREPAIR_NL_C_MAX - 1)

enum {
        MDCSREPAIR_NL_A_UNSPEC,
        MDCSREPAIR_NL_A_XML,
        __MDCSREPAIR_NL_A_MAX,
};
#define MDCSREPAIR_NL_A_MAX (__MDCSREPAIR_NL_A_MAX - 1)

static struct genl_family mdcs_family = {
	.id = GENL_ID_GENERATE,
	.hdrsize = 0,
	.name = "VFS_MDCSREPAIR",
	.version = 1,
	.maxattr = MDCSREPAIR_NL_A_MAX,
};

/* Current actual message length ~ 300 (5/28/2014) */
#define MAX_XML	(512 - sizeof(struct list_head))

static struct mdcs_msg {
	struct list_head list;
	char buffer[];
} mdcs_msg_q;
static spinlock_t mdcs_lock;
static DECLARE_WAIT_QUEUE_HEAD(mdcs_waiter);
static struct task_struct *kthread;
static atomic_t mdcs_q_len = ATOMIC_INIT(0);
#define MDCS_Q_MAX	200

static void append_q(struct mdcs_msg *msg)
{
	spin_lock(&mdcs_lock);
	list_add_tail(&msg->list, &mdcs_msg_q.list);
	spin_unlock(&mdcs_lock);
	atomic_inc(&mdcs_q_len);
	wake_up(&mdcs_waiter);
}

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

/*
 * btrfs_readpage_end_io() calls mdcs_netlink() in a worker thread with
 * in_atomic_preempt_off() turned on. nal_put() calls schedule() and it
 * fails due to atomic context violation (although it succesfully puts
 * the argument). This thread on behalf of such and all threads calls
 * nla_put() to complete netlink message passing.
 */
static int mdcsrepair_kthread(void *arg)
{
	while (1) {
		struct nlmsghdr *nlh;
		struct sk_buff *skb;
		static atomic_t seq;
		struct mdcs_msg *m;

		/* wait_event() warns if there is no wakeup for long. */
		wait_event_timeout(mdcs_waiter,
				!list_empty(&mdcs_msg_q.list), HZ);
tryagain:
		if (!(m = retrieve_q()))
			continue;
		/* Do not free skb.
		 * skb will be "consumed" in genlmsg_multicast().
		 */
		if (!(skb = genlmsg_new(NLMSG_GOODSIZE, GFP_NOFS))) {
			pr_err("%s: genlmsg_new failed.", __func__);
			goto nlmsg_failure;
		}
		if (!(nlh = genlmsg_put(skb, 0, atomic_add_return(1, &seq),
					&mdcs_family, 0,
					MDCSREPAIR_NL_C_REQUEST))) {
			pr_err("%s: genlmsg_put failed.", __func__);
			goto nlmsg_failure;
		}
		if (nla_put_string(skb, MDCSREPAIR_NL_A_XML, m->buffer)) {
			pr_err("%s: nla_put_string failed.", __func__);
			goto nlmsg_failure;
		}
		genlmsg_end(skb, nlh);
		genlmsg_multicast(skb, 0, mdcs_family.id, GFP_NOFS);
		skb = NULL;
nlmsg_failure:
		if (skb)
			kfree_skb(skb);
		kfree(m);
		goto tryagain;
	}

	return 0;
}

/*
 * Do not change this function name as it is used to validate the XML
 * message by the user land code.
 */
void mdcsrepair_netlink(const char *filesystem, const char *place,
		const u8 *expected, const u8 *computed, size_t cslen,
		const void *csfunc,
		u64 dpos, size_t len,
		dev_t dev, dev_t adev, u64 inum, u64 fpos, u64 flags)
{
	char expbuf[cslen * 2 + 1], compbuf[cslen * 2 + 1];
	char *p, *name = NULL;
	int i, remainder;
	unsigned int inverted = 0;
	struct mdcs_msg *msg;

	if (__btrfs_proc_mdcsrepair_debug && __btrfs_proc_mdcsrepair_debug[0] == ':') {
		char *end;
		u64 restrict_inum;
		restrict_inum = simple_strtoull(__btrfs_proc_mdcsrepair_debug+1, &end, 10);
		if (restrict_inum && *end == '\0' && inum != restrict_inum)
			return;
	}

	if ((i = atomic_read(&mdcs_q_len)) > MDCS_Q_MAX) {
		pr_warn("%s: Too many (%d) checksum failures in queue.",
			__func__, i);
		if (!__btrfs_proc_mdcsrepair_debug)
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
		dpos, len,
		(flags & CSL_INTRAFILEPOS) ? "file" : "disk",
		expbuf, compbuf, cslen, name,
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
static struct proc_dir_entry *btrfs_proc_root, *btrfs_proc_mdcsrepair;

static int btrfs_proc_mdcsrepair_read(char *page, char **start,
			off_t ppos, int count, int *peof, void *data)
{
	int len = 0;
	if (__btrfs_proc_mdcsrepair_debug && *__btrfs_proc_mdcsrepair_debug) {
		len = min((int)strlen(__btrfs_proc_mdcsrepair_debug),
					count - 1);
		memcpy(page, __btrfs_proc_mdcsrepair_debug, len);
	}
	page[len] = '\n';
	return len + 1;
}
		
static int btrfs_proc_mdcsrepair_write(struct file *file,
			const char __user *buffer,
			unsigned long count, void *data)
{
	ssize_t rv;

	if (__btrfs_proc_mdcsrepair_debug)
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

static int __devinit btrfs_mdcs_init(void)
{
	struct task_struct *kth;

	spin_lock_init(&mdcs_lock);
	INIT_LIST_HEAD(&mdcs_msg_q.list);
	atomic_set(&mdcs_q_len, 0);

	if (genl_register_family(&mdcs_family))
		return -EIO;
	if (!(btrfs_proc_root = proc_mkdir("fs/btrfs", NULL)))
		return -EIO;
	if (!(btrfs_proc_mdcsrepair =
		create_proc_entry("mdcsrepair",
				S_IFREG | S_IRUGO | S_IWUSR,
				btrfs_proc_root))) {
		remove_proc_entry("fs/btrfs", NULL);
		return -EIO;
	}
	btrfs_proc_mdcsrepair->read_proc = btrfs_proc_mdcsrepair_read;
	btrfs_proc_mdcsrepair->write_proc = btrfs_proc_mdcsrepair_write;
	kth = kthread_run(mdcsrepair_kthread, NULL, "mdcsrepair");
	if (IS_ERR(kth)) {
		pr_err("%s: failed to create a kernel thread", __func__);
		remove_proc_entry("fs/btrfs/mdcsrepair", NULL);
		genl_unregister_family(&mdcs_family);
		return -EIO;
	}
	kthread = kth;
	return 0;
}

static void __devexit btrfs_mdcs_exit(void)
{
	struct mdcs_msg *m;
	struct task_struct *kth = kthread;

	kthread = NULL;
	kthread_stop(kth);
	while ((m = retrieve_q()))
		kfree(m);
	remove_proc_entry("fs/btrfs/mdcsrepair", NULL);
	kfree(__btrfs_proc_mdcsrepair_debug);
	__btrfs_proc_mdcsrepair_debug = NULL;
	genl_unregister_family(&mdcs_family);
}

module_init(btrfs_mdcs_init);
module_exit(btrfs_mdcs_exit);

MODULE_AUTHOR("<hiro.sugawara@netgear.com>");
MODULE_DESCRIPTION("BTRFS checksum corrector");
MODULE_LICENSE("GPL");
