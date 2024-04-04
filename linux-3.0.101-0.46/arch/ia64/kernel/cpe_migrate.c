/*
 *  CPE Migration driver
 *
 *  Copyright (c) 2008-2010 Silicon Graphics, Inc.  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * File:	cpe_migrate.c
 * Purpose:	Migrate data from physical pages with excessive correctable
 *		errors to new physical pages.  Keep the old pages on a discard
 *		list.
 *
 * Copyright (C) 2008-2010 SGI - Silicon Graphics Inc.
 * Copyright (C) 2008-2010 Russ Anderson <rja@sgi.com>
 */

#include <linux/sysdev.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/vmalloc.h>
#include <linux/migrate.h>
#include <linux/page-isolation.h>
#include <linux/memcontrol.h>
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/sn/sn_cpuid.h>
#include <asm/mca.h>

#define BADRAM_BASENAME	"badram"
struct kobject *badram_kobj;

struct cpe_info {
	struct list_head list;
	u64 	paddr;
	u16	node;
	u16	count;
};
LIST_HEAD(ce_list);

static int cpe_polling_enabled = 1;
static int record_to_process;
unsigned int migrate_threshold = 1;
static int mstat_cannot_isolate;
static int mstat_failed_to_discard;
static int mstat_already_marked;
static int mstat_already_on_list;
static int mstat_added_to_queue;
static int mstat_removed_from_queue;
static int mstat_currently_on_queue;
static int mstat_entries_processed;
static int mstat_cpe_list_lock;
static int record_bad_paddr = 1;
static DEFINE_SPINLOCK(cpe_history_lock);
int cpe_poll_threshold = 1;
int cmc_poll_threshold = 1;

/* IRQ handler notifies this wait queue on receipt of an IRQ */
DECLARE_WAIT_QUEUE_HEAD(cpe_activate_IRQ_wq);
static DECLARE_COMPLETION(kthread_cpe_migrated_exited);
int cpe_active;
DEFINE_SPINLOCK(cpe_migrate_lock);

static void
get_physical_address(void *buffer, u64 *paddr, u16 *node)
{
	sal_log_record_header_t *rh;
	sal_log_mem_dev_err_info_t *mdei;
	ia64_err_rec_t *err_rec;
	sal_log_platform_err_info_t *plat_err;
	efi_guid_t guid;

	err_rec = buffer;
	rh = &err_rec->sal_elog_header;
	*paddr = 0;
	*node = 0;

	/*
	 * Make sure it is a corrected error.
	 */
	if (rh->severity != sal_log_severity_corrected)
		return;

	plat_err = (sal_log_platform_err_info_t *)&err_rec->proc_err;

	guid = plat_err->mem_dev_err.header.guid;
	if (efi_guidcmp(guid, SAL_PLAT_MEM_DEV_ERR_SECT_GUID) == 0) {
		/*
		 * Memory cpe
		 */
		mdei = &plat_err->mem_dev_err;
		if (mdei->valid.oem_data) {
			if (mdei->valid.physical_addr)
				*paddr = mdei->physical_addr;

			if (mdei->valid.node) {
				if (ia64_platform_is("sn2"))
					*node = nasid_to_cnodeid(mdei->node);
				else
					*node = mdei->node;
			}
		}
	}
}

static struct page *
alloc_migrate_page(struct page *ignored, unsigned long node, int **x)
{

	return alloc_pages_node(node, GFP_HIGHUSER_MOVABLE, 0);
}

static int
validate_paddr_page(u64 paddr)
{
	struct page *page;

	if (!paddr)
		return -EINVAL;

	if (!ia64_phys_addr_valid(paddr))
		return -EINVAL;

	if (!pfn_valid(paddr >> PAGE_SHIFT))
		return -EINVAL;

	page = phys_to_page(paddr);
	if (PageMemError(page))
		mstat_already_marked++;
	return 0;
}

extern int isolate_lru_page(struct page *);
static int
ia64_mca_cpe_move_page(u64 paddr, u32 node)
{
	LIST_HEAD(pagelist);
	struct page *page;
	unsigned long flags;
	int ret;

	ret = validate_paddr_page(paddr);
	if (ret < 0)
		return ret;

	/*
	 * convert physical address to page number
	 */
	page = phys_to_page(paddr);

	spin_lock_irqsave(&cpe_migrate_lock, flags);
	migrate_prep();
	ret = isolate_lru_page(page);
	if (ret) {
		mstat_cannot_isolate++;
		spin_unlock_irqrestore(&cpe_migrate_lock, flags);
		return ret;
	}

	list_add(&page->lru, &pagelist);
	SetPageMemError(page);		/* Mark the page as bad */
	ret = migrate_pages(&pagelist, alloc_migrate_page, node, false, true);
	if (ret == 0) {
		total_badpages++;
		if (record_bad_paddr)
			list_add_tail(&page->lru, &badpagelist);
	} else {
		mstat_failed_to_discard++;
		/*
		 * The page failed to migrate and is not on the bad page list.
		 * Clearing the error bit will allow another attempt to migrate
		 * if it gets another correctable error.
		 */
		ClearPageMemError(page);
	}
	spin_unlock_irqrestore(&cpe_migrate_lock, flags);

	return 0;
}

DEFINE_SPINLOCK(cpe_list_lock);

/*
 * cpe_process_queue
 *	Pulls the physical address off the list and calls the migration code.
 *	Will process all the addresses on the list.
 */
void
cpe_process_queue(void)
{
	int i, ret;
	struct cpe_info *entry, *entry2;

	while (!list_empty(&ce_list)) {
		u64     paddr = 0;
		u16     node = 0;

		if (!spin_trylock(&cpe_list_lock)) {
			for (i = 0; i < 10000; i++) {
				if (spin_trylock(&cpe_list_lock))
					break;
				else
					cpu_relax();
			}
			if (i >= 10000)
				/*
				 * Couldn't get the lock, give up.
				 */
				return;
		}
		list_for_each_entry_safe(entry, entry2, &ce_list, list) {
			if (entry->count >= migrate_threshold) {
				/*
				 * There is a valid entry that needs processing.
				 */
				paddr = entry->paddr;
				node = entry->node;
				/*
				 * Remove entry from the list
				 */
				list_del(&entry->list);
				mstat_currently_on_queue--;
				break;
			}
		}
		spin_unlock(&cpe_list_lock);
		kfree(entry);
		mstat_removed_from_queue++;

		if (paddr) {
			mstat_entries_processed++;
			ret = ia64_mca_cpe_move_page(paddr, node);
			paddr = 0;
		}
	}

	record_to_process = 0;
	return;
}

/*
 * kthread_cpe_migrate
 *	kthread_cpe_migrate is created at module load time and lives
 *	until the module is removed.  When not active, it will sleep.
 */
static int
kthread_cpe_migrate(void *ignore)
{
	while (cpe_active) {
		/*
		 * wait for work
		 */
		(void)wait_event_interruptible(cpe_activate_IRQ_wq,
						(record_to_process ||
						!cpe_active));
		cpe_process_queue();		/* process work */
	}
	complete(&kthread_cpe_migrated_exited);
	return 0;
}

/*
 * cpe_add_migrate_list
 *	Add a physical address to the list of addresses to migrate
 *	and schedule the back end worker task.  This is called
 *	in interrupt context so cannot directly call the migration
 *	code.
 *
 *  Inputs
 *	rec	The CPE record
 *  Outputs
 *	1 on Success, -1 on failure
 */
static int
cpe_add_migrate_list(u64 paddr, u16 node)
{
	int ret;
	struct cpe_info *entry;

	ret = validate_paddr_page(paddr);
	if (ret < 0)
		return -EINVAL;

	entry = kmalloc(sizeof(struct cpe_info), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;
	entry->node = node;
	entry->paddr = paddr;
	entry->count = 1;

	if (!spin_trylock(&cpe_list_lock)) {
		/*
		 * Someone else has the lock.  To avoid spinning in interrupt
		 * handler context, bail.
		 */
		kfree(entry);
		mstat_cpe_list_lock++;
		return 1;
	}

	/*
	 * New entry, add to the list.
	 */
	list_add_tail(&entry->list, &ce_list);
	mstat_currently_on_queue++;
	spin_unlock(&cpe_list_lock);
	mstat_added_to_queue++;
	if (migrate_threshold <= 1) {
		record_to_process = 1;
		wake_up_interruptible(&cpe_activate_IRQ_wq);
	}

	return 1;
}

/*
 * cpe_setup_migrate
 *	Get the physical address out of the CPE record, add it
 *	to the list of addresses to migrate,
 *
 *  Inputs
 *	rec	The CPE record
 *  Outputs
 *	1 on Success, -EINVAL on failure
 */
static int
cpe_setup_migrate(void *rec)
{
	u64 paddr;
	u16 node;

	if (!rec)
		return -EINVAL;

	get_physical_address(rec, &paddr, &node);
	cpe_add_migrate_list(paddr, node);
	return 1;
}
/*
 * =============================================================================
 */

/*
 * free_one_bad_page
 *	Free one page from the list of bad pages.
 */
static int
free_one_bad_page(unsigned long paddr)
{
	LIST_HEAD(pagelist);
	struct page *page, *page2, *target;

	/*
	 * Verify page address
	 */
	target = phys_to_page(paddr);
	list_for_each_entry_safe(page, page2, &badpagelist, lru) {
		if (page != target)
			continue;

		ClearPageMemError(page);        /* Mark the page as good */
		total_badpages--;
		list_move_tail(&page->lru, &pagelist);
		putback_lru_pages(&pagelist);
		break;
	}
	return 0;
}

/*
 * free_all_bad_pages
 *	Free all of the pages on the bad pages list.
 */
static int
free_all_bad_pages(void)
{
	struct page *page, *page2;

	list_for_each_entry_safe(page, page2, &badpagelist, lru) {
		ClearPageMemError(page);        /* Mark the page as good */
		total_badpages--;
	}
	putback_lru_pages(&badpagelist);
	/*
	 * Clear the stat counters
	 */
	mstat_cannot_isolate = 0;
	mstat_failed_to_discard = 0;
	mstat_already_marked = 0;
	mstat_already_on_list = 0;
	mstat_cpe_list_lock = 0;
	mstat_added_to_queue = 0;
	mstat_removed_from_queue = 0;
	mstat_currently_on_queue = 0;
	mstat_entries_processed = 0;
	return 0;
}

#define OPT_LEN	16

static ssize_t
badpage_store(struct kobject *kobj,
	      struct kobj_attribute *attr, const char *buf, size_t count)
{
	char optstr[OPT_LEN];
	unsigned long opt;
	int len = OPT_LEN;
	int err;

	if (count < len)
		len = count;

	strlcpy(optstr, buf, len);

	err = strict_strtoul(optstr, 16, &opt);
	if (err)
		return err;

	if (opt == 0)
		free_all_bad_pages();
	else
		free_one_bad_page(opt);

	return count;
}

/*
 * badpage_show
 *	Display the number, size, and addresses of all the pages on the
 *	bad page list.
 *
 *	Note that sysfs provides buf of PAGE_SIZE length.  bufend tracks
 *	the remaining space in buf to avoid overflowing.
 */
static ssize_t
summary_info_show(struct kobject *kobj,
	     struct kobj_attribute *attr, char *buf)
{
	int cnt = 0;
	char *bufend = buf + PAGE_SIZE;

	cnt = snprintf(buf, bufend - (buf + cnt),
			"Memory marked bad:        %d kB\n"
			"Pages marked bad:         %d\n"
			"Unable to isolate on LRU: %d\n"
			"Unable to migrate:        %d\n"
			"Already marked bad:       %d\n"
			"Already on list:          %d\n"
			"Times list lock was busy: %d\n"
			"Added to the queue:       %d\n"
			"Removed from the queue:   %d\n"
			"Currently on the queue:   %d\n"
			"Entries processed:        %d\n",
			total_badpages << (PAGE_SHIFT - 10), total_badpages,
			mstat_cannot_isolate, mstat_failed_to_discard,
			mstat_already_marked, mstat_already_on_list,
			mstat_cpe_list_lock, mstat_added_to_queue,
			mstat_removed_from_queue,
			mstat_currently_on_queue,
			mstat_entries_processed
			);
	return cnt;
}

static struct kobj_attribute summary_info_attr = {
	.attr    = {
		.name = "summary_info",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = summary_info_show,
	.store = badpage_store,
};

/*
 * badpage_show
 *	Display all the pages on the bad page list.
 *
 *	Note that sysfs provides buf of PAGE_SIZE length.  bufend tracks
 *	the remaining space in buf to avoid overflowing.
 */
static ssize_t
badpage_show(struct kobject *kobj,
	     struct kobj_attribute *attr, char *buf)
{
	struct page *page, *page2;
	int i = 0, cnt = 0;
	char *bufend = buf + PAGE_SIZE;

	if (total_badpages == 0) { /* No bad pages, nothing else to print */
		cnt = snprintf(buf, bufend - (buf + cnt),
			"No bad pages.\n");
		return cnt;
	}
	/*
	 * Print the physical addresses of the bad pages.
	 */
	list_for_each_entry_safe(page, page2, &badpagelist, lru) {
		if (bufend - (buf + cnt) < 20)
			break;		/* Avoid overflowing the buffer */
		cnt += snprintf(buf + cnt, bufend - (buf + cnt),
				" 0x%011lx", page_to_phys(page));
		if (!(++i % 5))
			cnt += snprintf(buf + cnt, bufend - (buf + cnt), "\n");
	}
	cnt += snprintf(buf + cnt, bufend - (buf + cnt), "\n");

	return cnt;
}

static struct kobj_attribute badram_attr = {
	.attr    = {
		.name = "bad_pages",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = badpage_show,
	.store = badpage_store,
};


static ssize_t
migrate_store(struct kobject *kobj,
	      struct kobj_attribute *attr, const char *buf, size_t count)
{
	char optstr[OPT_LEN];
	unsigned long opt;
	int len = OPT_LEN;
	int err;
	unsigned long flags;

	if (count < len)
		len = count;

	strlcpy(optstr, buf, len);

	err = strict_strtoul(optstr, 16, &opt);
	if (err)
		return err;

	spin_lock_irqsave(&cpe_history_lock, flags);
	cpe_add_migrate_list(opt, nasid_to_cnodeid(get_node_number(opt)));
	spin_unlock_irqrestore(&cpe_history_lock, flags);

	return count;
}


static struct kobj_attribute migrate_page_attr = {
	.attr    = {
		.name = "migrate_page",
		.mode = S_IWUSR | S_IRUGO,
	},
	.store = migrate_store,
};


static ssize_t
cpe_poll_threshold_store(struct kobject *kobj,
	      struct kobj_attribute *attr, const char *buf, size_t count)
{
	char optstr[OPT_LEN];
	unsigned long opt;
	int len = OPT_LEN;
	int err;

	if (count < len)
		len = count;

	strlcpy(optstr, buf, len);

	err = strict_strtoul(optstr, 16, &opt);
	if (err)
		return err;
	cpe_poll_threshold = opt;

	return count;
}

static ssize_t
cpe_poll_threshold_show(struct kobject *kobj,
	     struct kobj_attribute *attr, char *buf)
{
	int cnt = 0;
	char *bufend = buf + PAGE_SIZE;

	cnt = snprintf(buf, bufend - (buf + cnt), "%d\n", cpe_poll_threshold);
	return cnt;
}

static struct kobj_attribute cpe_poll_threshold_attr = {
	.attr    = {
		.name = "cpe_polling_threshold",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = cpe_poll_threshold_show,
	.store = cpe_poll_threshold_store,
};


static ssize_t
cmc_poll_threshold_store(struct kobject *kobj,
	      struct kobj_attribute *attr, const char *buf, size_t count)
{
	char optstr[OPT_LEN];
	unsigned long opt;
	int len = OPT_LEN;
	int err;

	if (count < len)
		len = count;

	strlcpy(optstr, buf, len);

	err = strict_strtoul(optstr, 16, &opt);
	if (err)
		return err;
	cmc_poll_threshold = opt;

	return count;
}

static ssize_t
cmc_poll_threshold_show(struct kobject *kobj,
	     struct kobj_attribute *attr, char *buf)
{
	int cnt = 0;
	char *bufend = buf + PAGE_SIZE;

	cnt = snprintf(buf, bufend - (buf + cnt), "%d\n", cmc_poll_threshold);
	return cnt;
}

static struct kobj_attribute cmc_poll_threshold_attr = {
	.attr    = {
		.name = "cmc_polling_threshold",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = cmc_poll_threshold_show,
	.store = cmc_poll_threshold_store,
};

static ssize_t
migrate_threshold_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	char optstr[OPT_LEN];
	unsigned long opt;
	int len = OPT_LEN;
	int err;

	if (count < len)
		len = count;

	strlcpy(optstr, buf, len);

	err = strict_strtoul(optstr, 16, &opt);
	if (err)
		return err;
	migrate_threshold = opt;

	return count;
}

static ssize_t
migrate_threshold_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	int cnt = 0;
	char *bufend = buf + PAGE_SIZE;

	cnt = snprintf(buf, bufend - (buf + cnt), "%d\n", migrate_threshold);
	return cnt;
}

static struct kobj_attribute migrate_threshold_attr = {
	.attr    = {
		.name = "migrate_threshold",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = migrate_threshold_show,
	.store = migrate_threshold_store,
};


static int __init
cpe_migrate_external_handler_init(void)
{
	int error;
	struct task_struct *kthread;

	if (!badram_kobj)
		badram_kobj = kobject_create_and_add(BADRAM_BASENAME, firmware_kobj);
	if (!badram_kobj) {
		printk(KERN_WARNING "kobject_create_and_add %s failed \n", BADRAM_BASENAME);
		return -EINVAL;
	}

	error = sysfs_create_file(badram_kobj, &badram_attr.attr);
	if (error)
		return -EINVAL;

	error = sysfs_create_file(badram_kobj, &summary_info_attr.attr);
	if (error)
		return -EINVAL;

	error = sysfs_create_file(badram_kobj, &migrate_page_attr.attr);
	if (error)
		return -EINVAL;

	error = sysfs_create_file(badram_kobj, &cpe_poll_threshold_attr.attr);
	if (error)
		return -EINVAL;

	error = sysfs_create_file(badram_kobj, &cmc_poll_threshold_attr.attr);
	if (error)
		return -EINVAL;

	error = sysfs_create_file(badram_kobj, &migrate_threshold_attr.attr);
	if (error)
		return -EINVAL;

	/*
	 * set up the kthread
	 */
	cpe_active = 1;
	kthread = kthread_run(kthread_cpe_migrate, NULL, "cpe_migrate");
	if (IS_ERR(kthread)) {
		complete(&kthread_cpe_migrated_exited);
		return -EFAULT;
	}

	/*
	 * register external ce handler
	 */
	if (ia64_reg_CE_extension(cpe_setup_migrate)) {
		printk(KERN_ERR "ia64_reg_CE_extension failed.\n");
		return -EFAULT;
	}
	cpe_poll_enabled = cpe_polling_enabled;

	printk(KERN_INFO "Registered badram Driver\n");
	return 0;
}

static void __exit
cpe_migrate_external_handler_exit(void)
{
	/* unregister external mca handlers */
	ia64_unreg_CE_extension();

	/* Stop kthread */
	cpe_active = 0;			/* tell kthread_cpe_migrate to exit */
	wake_up_interruptible(&cpe_activate_IRQ_wq);
	wait_for_completion(&kthread_cpe_migrated_exited);

	sysfs_remove_file(kernel_kobj, &migrate_threshold_attr.attr);
	sysfs_remove_file(kernel_kobj, &cpe_poll_threshold_attr.attr);
	sysfs_remove_file(kernel_kobj, &cmc_poll_threshold_attr.attr);
	sysfs_remove_file(kernel_kobj, &badram_attr.attr);
	sysfs_remove_file(kernel_kobj, &summary_info_attr.attr);
	kobject_put(badram_kobj);
}

module_init(cpe_migrate_external_handler_init);
module_exit(cpe_migrate_external_handler_exit);

module_param(cpe_polling_enabled, int, 0644);
MODULE_PARM_DESC(cpe_polling_enabled,
		"Enable polling with migration");

MODULE_AUTHOR("Russ Anderson <rja@sgi.com>");
MODULE_DESCRIPTION("ia64 Corrected Error page migration driver");
MODULE_LICENSE("GPL");
