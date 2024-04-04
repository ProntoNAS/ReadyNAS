/*
 *  SCSI Spinup specific structures and functions.
 *
 *  Copyright (c) 2009 Marvell,  All rights reserved.
 *  Copyright (c) 2014 NETGEAR   All rights reserved.
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/dmi.h>
#include <linux/moduleparam.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_spinup.h>

#include "scsi_priv.h"
#include "scsi_logging.h"

/* structure: spinup_config=<spinup_max>,<spinup_timout>
   example: spinup_config=2,6 (max two disks spinning up, 6 seconds apart) */
static char *cmdline = NULL;

#ifdef DEBUG_SPIN_UP_QUEUE
#define __DPRINTK(...)	printk(__VA_ARGS__)
#else
#define __DPRINTK(...)	do {} while(0)
#endif

/* Required to get the configuration string from the Kernel Command Line */
int spinup_cmdline_config(char *s);
__setup("spinup_config=", spinup_cmdline_config);

int spinup_cmdline_config(char *s)
{
	cmdline = s;
	return 1;
}

static unsigned int spinup_enabled = 0;
static unsigned int spinup_max = 2;
static unsigned int spinup_timeout = 6;
static spinlock_t spinup_lock;
static int scsi_spinup_ok(void);

module_param_named(staggered_spinup, spinup_enabled, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(staggered_spinup, "0=disable, 1=enable staggered spin up");

static atomic_t spinup_now;
struct spinup_node {
	struct list_head list;
	struct scsi_device *sdev;
};
static struct list_head spinup_list;

static int param_set_spinup_max(const char *val, const struct kernel_param *kp)
{
	int new_max = param_set_int(val, kp);

	if (new_max <= 0)
		return -EINVAL;

	atomic_add(new_max - spinup_max, &spinup_now);
	spinup_max = new_max;
	return 0;
}

static struct kernel_param_ops params_ops_spinup_max = {
	.set = param_set_spinup_max,
	.get = param_get_uint,
};
module_param_cb(spinup_max, &params_ops_spinup_max, &spinup_max,
		S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(spinup_max, "maximum number of spin-ups");
module_param_named(spinup_timeout, spinup_timeout, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(spinup_timeout, "delay in staggered spin-ups in seconds");

static void spinup_times_out(struct scsi_device *sdev);

void standby_add_timer(struct scsi_device *sdev, int timeout,
		    void (*complete)(struct scsi_device *))
{
	/*
	 * If the clock was already running for this device, then
	 * first delete the timer.  The timer handling code gets rather
	 * confused if we don't do this.
	 */
	if (sdev->standby_timeout.function)
		del_timer(&sdev->standby_timeout);

	sdev->standby_timeout.data = (unsigned long)sdev;
	sdev->standby_timeout_secs = timeout;

	sdev->standby_timeout.expires = timeout + jiffies ;
	sdev->standby_timeout.function = (void (*)(unsigned long)) complete;

	SCSI_LOG_ERROR_RECOVERY(5, printk("%s: scmd: %p, time:"
					  " %d, (%p)\n", __FUNCTION__,
					  sdev, timeout, complete));

	add_timer(&sdev->standby_timeout);
}

int standby_delete_timer(struct scsi_device *sdev)
{
	int rtn = del_timer(&sdev->standby_timeout);

	SCSI_LOG_ERROR_RECOVERY(5, printk("%s: scmd: %p,"
					 " rtn: %d\n", __FUNCTION__,
					 sdev, rtn));

	sdev->standby_timeout.data = (unsigned long)NULL;
	sdev->standby_timeout.function = NULL;

	return rtn;
}


void standby_times_out(struct scsi_device *sdev)
{
	unsigned long flags = 0;

	__DPRINTK("\nDisk [%d] timeout done, going to sleep...\n",sdev->id);
	spin_unlock_irqrestore(sdev->host->host_lock, flags);
	sdev->sdev_power_state = SDEV_PW_STANDBY_TIMEOUT_PASSED;
	spin_lock_irqsave(sdev->host->host_lock, flags);
	standby_delete_timer(sdev);
}

static void spinup_add_timer(struct scsi_device *sdev, int timeout,
		    void (*complete)(struct scsi_device *))
{
	/*
	 * If the clock was already running for this device, then
	 * first delete the timer.  The timer handling code gets rather
	 * confused if we don't do this.
	 */
	if (sdev->spinup_timeout.function)
		del_timer(&sdev->spinup_timeout);


	sdev->spinup_timeout.data = (unsigned long)sdev;

	sdev->spinup_timeout.expires = jiffies + msecs_to_jiffies (timeout * 1000);
	sdev->spinup_timeout.function = (void (*)(unsigned long)) complete;

	SCSI_LOG_ERROR_RECOVERY(5, printk("%s: scmd: %p, time:"
					  " %d, (%p)\n", __FUNCTION__,
					  sdev, timeout, complete));
	add_timer(&sdev->spinup_timeout);
}

int spinup_delete_timer(struct scsi_device *sdev)
{
	int rtn;

	rtn = del_timer(&sdev->spinup_timeout);

	SCSI_LOG_ERROR_RECOVERY(5, printk("%s: scmd: %p,"
					 " rtn: %d\n", __FUNCTION__,
					 sdev, rtn));

	sdev->spinup_timeout.data = (unsigned long)NULL;
	sdev->spinup_timeout.function = NULL;

	return rtn;
}

static void scsi_spinup_device_dequeue_next(void)
{
	struct list_head *ptr;
	struct spinup_node *entry;

	spin_lock(&spinup_lock);
	if (!list_empty(&spinup_list)) {
		ptr = spinup_list.next;
		entry = list_entry(ptr, struct spinup_node, list);
		__DPRINTK("\nNext Disk is entry: [%d] power state [%d]\n",
				entry->sdev->id, entry->sdev->sdev_power_state);

		if (scsi_spinup_ok()) {
			entry->sdev->sdev_power_state = SDEV_PW_SPINNING_UP;
			spinup_add_timer(entry->sdev,
				scsi_spinup_get_timeout(), spinup_times_out);
			scsi_internal_device_unblock(entry->sdev, SDEV_RUNNING);
			pr_debug("Timeout - Disk [%d] spinning up...\n",
				entry->sdev->id);
			list_del(ptr);
			kfree(entry);
		}
	}
#ifdef DEBUG_SPIN_UP_QUEUE
	printk("\n");
	list_for_each(ptr, &spinup_list) {
		entry = list_entry(ptr, struct spinup_node, list);
		printk("[%d] ->", entry->sdev->id);
	}
	printk("[EOD]\n");
#endif
	spin_unlock(&spinup_lock);
}

static void spinup_times_out(struct scsi_device *sdev)
{
	scsi_spinup_complete();
	spinup_delete_timer(sdev);
	scsi_spinup_device_dequeue_next();
}

int scsi_spinup_enabled(void)
{
	return spinup_enabled;
}

int scsi_spinup_get_timeout(void)
{
	return spinup_timeout;
}

/* __setup kernel line parsing and setting up the spinup feature */
int __init scsi_spinup_init(void)
{
	spin_lock_init(&spinup_lock);
	atomic_set(&spinup_now, spinup_max);
	INIT_LIST_HEAD(&spinup_list);
	return 0;
}

static int scsi_spinup_device_queue(struct scsi_device *sdev)
{
	struct spinup_node *new;
	unsigned long flags;

	new = kmalloc(sizeof(struct spinup_node), GFP_NOWAIT);
	if (!new)
		return 1;
	new->sdev = sdev;
	spin_lock_irqsave(&spinup_lock, flags);
	list_add_tail(&new->list, &spinup_list);

#ifdef DEBUG_SPIN_UP_QUEUE
	struct list_head *ptr;
	struct spinup_node *entry;

	printk("\n");
	list_for_each(ptr, &spinup_list) {
		entry = list_entry(ptr, struct spinup_node, list);
		printk("[%d] ->",entry->sdev->id);
	}
	printk("[EOD]\n");
#endif
	spin_unlock_irqrestore(&spinup_lock, flags);

	return 0;
}

int scsi_spinup_device(struct scsi_cmnd *cmd)
{
	struct scsi_device *sdev = cmd->device;

	__DPRINTK("\nDisk [%d] scsi_spinup_device...\n", sdev->id);
	switch (sdev->sdev_power_state) {
	case SDEV_PW_STANDBY:
	case SDEV_PW_STANDBY_TIMEOUT_PASSED:
		/* disk will wait here to his turn to spinup */
		__DPRINTK("\nDisk [%d] waiting to spinup...\n", sdev->id);
		if (!scsi_spinup_ok()) {
			if (scsi_spinup_device_queue(sdev) == 0) {
				pr_debug("Disk [%d] queued up for spinup!\n", sdev->id);
				sdev->sdev_power_state = SDEV_PW_WAIT_FOR_SPIN_UP;
			}
			return 1;
		}
		sdev->sdev_power_state = SDEV_PW_SPINNING_UP;
		/* starting timer for the spinup process */
		pr_debug("Disk [%d] spinning up...\n", sdev->id);
		spinup_add_timer(sdev, scsi_spinup_get_timeout(), spinup_times_out);
		break;
	case SDEV_PW_STANDBY_TIMEOUT_WAIT:
		standby_add_timer(sdev, sdev->standby_timeout_secs, standby_times_out);
		break;
	default:
		break;
	}
	return 0;
}

static int scsi_spinup_ok(void)
{
	if (atomic_dec_if_positive(&spinup_now) >= 0) {
		pr_debug("Down we go! [%d]\n", atomic_read(&spinup_now));
		return 1;
	}
	return 0;
}

int scsi_spinup_complete(void)
{
	atomic_inc(&spinup_now);
	__DPRINTK("\nUP we go!! [%d] \n",(int) atomic_read(&spinup_now) );
	return 0;
}
