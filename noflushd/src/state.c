/*
 * <state.c>
 * Central noflushd state machine, and random accumulated crap.
 * 
 * Copyright (C) 2000, 2001 Daniel Kobras <kobras@linux.de>
 * 
 * except the sync functions which are
 * 
 * Copyright (C) 2000 Pavel Machek <pavel@suse.cz> and Daniel Kobras
 * 
 * Pavel Machek's work was sponsored by SuSE.
 * 
 * $Id: state.c,v 1.3 2011-02-12 02:17:05 jmaggard Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */             

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "state.h"
#include "bug.h"
#include "diskhelper.h"
#include "disk_stat.h"
#include "disk_info.h"
#include "intr_stat.h"
#include "part_info.h"
#include "timeout.h"
#include "kupdate.h"
#include "noflushd.h"

#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <mntent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/ioctl.h>

#include <dirent.h>
#include <libreadynas_lgpl/readynas.h>

int vm_setting=0;

static part_info_t part;
static char *dev_prefix;
static int plen;
static int log_status(disk_info_t whichdisk);

/* Fixup name from /proc/partitions (i.e. from part_info_get_name())
 * to yield a valid absolute file name. The returned string may be
 * used until the next call to devname_get() only.
 */
char *devname_get(char *name)
{
	char *suffix;

	if (!dev_prefix)
		BUG("Device name requested without prefix.");
	
	dev_prefix=realloc(dev_prefix, strlen(name)+plen+1);
	suffix = dev_prefix+plen;
	
	strcpy(suffix, name);

	return dev_prefix;
}
	
/* Check for oldstyle or devfs entries in /proc/partitions and set
 * dev_prefix accordingly, so that dev_prefix + pname yields a valid
 * fs name to the device node.
 */
static void set_path_fixup(void)
{
	char *dev;
	FILE *mnt;
	struct mntent *mntent;
	struct stat st;
	
	/* Try to determine devfs mountpoint. */
	mnt = fopen("/proc/mounts", "r");
	while ((mntent = getmntent(mnt)))
		if (!strcmp(mntent->mnt_type, "devfs"))
			break;
	fclose(mnt);

	if (mntent) {
		dev_prefix = malloc(strlen(mntent->mnt_dir + 2));
		strcpy(dev_prefix, mntent->mnt_dir);
		strcat(dev_prefix, "/");
		plen = strlen(dev_prefix);
		DEBUG("Detected devfs at %s", dev_prefix);
		return;
	}
	
	if (part_info_is_devfs()) {
		ERR("Your kernel is configured with devfs, but devfs is not\n"
		    "mounted anywhere. This means noflushd cannot work.\n"
		    "Please consult the noflushd README for details");
		exit(1);
	}
	
	/* Looks like oldstyle. Try default name. */
	dev_prefix = strdup("/dev/");
	plen = strlen(dev_prefix);

	part_info_reset(part);
	do {
		if (!part_info_disk_next(part)) {
			ERR("No valid disks found");
			exit(1);
		}
	} while (IS_META(part_info_get_major(part),
	                 part_info_get_minor(part)));
		
	dev = devname_get(part_info_get_name(part));
	
	/* Check: If we can access a block dev via the constructed name,
	 * our guess was probably correct.
	 */
	DEBUG("Probing for valid blkdev at %s", dev);
	if (!stat(dev, &st) && S_ISBLK(st.st_mode)) {
		DEBUG("Detected oldstyle dev at %s", dev_prefix);
		return;
	}
	
	BUG("Unable to determine device dir at %s", dev);
}

/* Initialize the static globals for devname handling. Ought to be done
 * before daemonizing to get error reporting on the console rather than
 * syslog. These functions are probably misplaced in this file, but then
 * all other places I thought of were worse.
 */
void devname_init(void)
{
	part = part_info_init();
	if (!part) {
		ERR("Error reading /proc/partitions");
		exit(1);
	}
	
	set_path_fixup();
}

static int sync_part(char *name)
{
	int fd, ret=0;

	DEBUG("Syncing %s", name);
	fd = open(name, O_WRONLY);
	if (fd==-1)
		return 0;
	if (!fsync(fd))
		ret=1;
	close(fd);
	return ret;
}

/* Sync disk in current part setting. Returns number of successful
 * sync attempts. */
static int sync_current_disk(void)
{
	char *name;
	int ret=0;
	int major = part_info_get_major(part);

	do {
		if( part_info_get_major(part) != major )
			break;
		name = devname_get(part_info_get_name(part));
		ret += sync_part(name);
	} while (part_info_part_next(part));

	return ret;
}

static void sync_disk(disk_info_t di)
{
	int succ;

	part_info_reset(part);

	while ((succ=part_info_disk_next(part)))
		if (part_info_get_major(part) == di->major &&
		    part_info_get_minor(part) == di->minor)
			break;
	
	if (!succ) {
		DEBUG("(%d, %d): No such disk.", di->major, di->minor);
		return;
	}
	
	if (!sync_current_disk())
		disk_info_mark_ro(di);
}
		
static void sync_spinning_disks(disk_info_t head)
{
	int succ;
	disk_info_t di;
	FILE *mounts;
	struct mntent *mntent;
	
	part_info_reset(part);
	
	mounts = setmntent("/proc/mounts", "r");
	if (mounts) {
		while ((mntent = getmntent(mounts))) {
			if ((strcmp(mntent->mnt_type, "ext3") == 0) ||
			    (strcmp(mntent->mnt_type, "ext4") == 0))
				sync_part(mntent->mnt_fsname);
		}
		endmntent(mounts);
		sleep(1); // Give the RAID devs a chance to sync
	}

	while ((succ=part_info_disk_next(part))) {
		int major = part_info_get_major(part);
		int minor = part_info_get_minor(part);
		
		for (di=head; di; di=di->next) {
			if (di->major == major && di->minor==minor)
				break;
		}

		if (di && (di->state==DISK_STATE_STOPPED || !di->is_rw))
			continue;
		
		sync_current_disk();
	}
}

static int get_min_timeout(disk_info_t di)
{
	int to, min=0;
	
	for (; di; di=di->next) {
		to = timeout_get(di->timeouts);
		if (!NFD_TO_IS_REGULAR(to))
			continue;
		if (to < min || min == 0)
			min = to;
	}
	
	return min ? min : 60;
}
		
static void advance_timeouts(disk_info_t di)
{
	while (advance_timeout) {
		advance_timeout--;
		timeout_advance_default();
		for (; di; di=di->next) {
			int delta = timeout_advance(&di->timeouts);
			if (NFD_TO_DO_SKIP(timeout_get(di->timeouts))) {
				di->state = DISK_STATE_IGNORED;
				di->time_left = 0;
				DEBUG("Timeout update. Ignoring disk %s",
				      di->name);
			} else {
				/* Don't touch state of already spun down
				 * disks regardless of new timeout. */
				if (di->state == DISK_STATE_STOPPED)
					continue;
				di->state = DISK_STATE_SPINNING;
				di->time_left += delta;
				DEBUG("Timeout update. Disk %s, timeout %d, "
				      "left %d", 
				      di->name, timeout_get(di->timeouts),
				      di->time_left);
			}
		}
		
	}
}

#if 1 /*NETGEAR - JM: Read ahead on file we know we'll be needing, so they don't
		      have to wake up the disks */
void read_ahead_for_cron() {
	int fd, i;
	off_t len;
	const char * const files[] = {  "/etc/cron.d/check_nonredundant_raid",
					"/etc/cron.d/check_smart_errors",
					"/etc/cron.d/exim",
					"/etc/cron.d/frontview",
					"/etc/cron.d/frontview-backup",
					"/etc/cron.d/ntpdate",
					"/etc/cron.d/poweroff",
					"/etc/cron.daily/check_disk_usage",
					"/etc/cron.daily/clean_recycle_bins",
					"/etc/cron.daily/logtruncate",
					"/etc/cron.daily/ntpdate",
					"/etc/cron.weekly/backup_idmap",
					"/etc/cron.weekly/get_smart",
					"/etc/cron.weekly/quotacheck",
					"/etc/cron.weekly/schedule_update_check",
					"/etc/pam.d/cron",
					"/etc/pam.d/sudo",
					"/usr/share/perl/5.8.8/unicore/To/Lower.pl", // Next 3 for freakin' SqueezeCenter...
					"/usr/share/perl/5.8.8/unicore/lib/gc_sc/SpacePer.pl",
					"/usr/share/perl/5.8.8/unicore/lib/gc_sc/LinearB.pl",
					"/frontview/bin/check_nonredundant_raid",
					"/frontview/bin/check_smart_errors",
					"/frontview/bin/functions",
					"/lib/libpthread.so.0",
					"/lib/libm.so.6",
					"/lib/librt.so.1",
#ifdef __sparc__
					"/lib/libpthread-0.10.so",
					"/lib/libm-2.3.2.so",
					"/lib/librt-2.3.2.so",
#else
					"/etc/acpi/powerbtn",
#endif
					"/usr/bin/killall",
					"/usr/bin/empty_exim",
					"/usr/bin/nice",
					"/usr/bin/test",
					"/usr/bin/perl",
					"/usr/bin/awk",
					"/bin/run-parts",
					"/bin/cat",
					"/bin/grep",
					"/bin/sleep",
					"/bin/bash",
					"/bin/rm",
					"/var/run/monitor_enclosure.pid",
					0
				      };

	DEBUG("Reading ahead to avoid future spinups.");
	for( i=0; files[i]; i++ ) {
		DEBUG("Reading %s...", files[i]);
		fd = open(files[i], O_RDONLY);
		if( fd != -1 ) {
			len = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);
			readahead(fd, 0, len);
			close(fd);
		}
	}
}

int param_get(const char *path, int *val) {
	FILE *file;
	int ret = -1;

	file = fopen(path, "r");
	if (file) {
		ret = fscanf(file, "%d", val);
		fclose(file);
	}

	return ret;
}

int param_set(const char *path, int val) {
	FILE *file;
	int ret = -1;

	file = fopen(path, "w");
	if (file) {
		ret = fprintf(file, "%d", val);
		fclose(file);
	}

	return ret;
}

int adjust_vm_settings() {
	FILE *mounts;
	struct mntent *mntent;
	int ret = 0;
	static int dirty_background_ratio = 0, dirty_background_bytes = 0;
	static int dirty_ratio = 0, dirty_bytes = 0;
	static int dirty_expire_centisecs = 0;

	if (vm_setting) { // 1 == prepare for spindown
		if (!dirty_background_bytes && !dirty_background_ratio) {
			param_get("/proc/sys/vm/dirty_background_bytes", &dirty_background_bytes);
			if (dirty_background_bytes < 1) {
				param_get("/proc/sys/vm/dirty_background_ratio", &dirty_background_ratio);
				if (dirty_background_ratio < 1)
					dirty_background_ratio = 10;
				DEBUG("Saving original dirty_background_ratio setting [%d]", dirty_background_ratio);
			}
			else
				DEBUG("Saving original dirty_background_bytes setting [%d]", dirty_background_bytes);
		}
		if (!dirty_bytes && !dirty_ratio) {
			param_get("/proc/sys/vm/dirty_bytes", &dirty_bytes);
			if (dirty_bytes < 1) {
				param_get("/proc/sys/vm/dirty_ratio", &dirty_ratio);
				if (dirty_ratio < 1)
					dirty_ratio = 20;
				DEBUG("Saving original dirty_ratio setting [%d]", dirty_ratio);
			}
			else
				DEBUG("Saving original dirty_bytes setting [%d]", dirty_bytes);
		}
		if (!dirty_expire_centisecs) {
			if (param_get("/proc/sys/vm/dirty_expire_centisecs", &dirty_expire_centisecs) < 1)
				dirty_expire_centisecs = 3000;
			DEBUG("Saving original dirty_expire_centisecs setting [%d]", dirty_expire_centisecs);
		}
		param_set("/proc/sys/vm/dirty_background_ratio", 80);
		param_set("/proc/sys/vm/dirty_ratio", 80);
		param_set("/proc/sys/vm/dirty_expire_centisecs", 60480000);
	}
	else {
		if (dirty_background_bytes)
			param_set("/proc/sys/vm/dirty_background_bytes", dirty_background_bytes);
		else
			param_set("/proc/sys/vm/dirty_background_ratio", dirty_background_ratio);
		if (dirty_bytes)
			param_set("/proc/sys/vm/dirty_bytes", dirty_bytes);
		else
			param_set("/proc/sys/vm/dirty_ratio", dirty_ratio);
		param_set("/proc/sys/vm/dirty_expire_centisecs", dirty_expire_centisecs);
	}

	mounts = setmntent("/proc/mounts", "r");
	if (!mounts)
		return -1;
	while ((mntent = getmntent(mounts))) {
		if ((strcmp(mntent->mnt_type, "ext3") == 0) ||
		    (strcmp(mntent->mnt_type, "ext4") == 0)) {
		    	if (strncmp(mntent->mnt_dir, "/USB", 4) == 0)
				continue;
			DEBUG("%s is mounted %s.  Remounting...", mntent->mnt_fsname, mntent->mnt_type);
			if (vm_setting)
				ret = mount(mntent->mnt_fsname, mntent->mnt_dir, mntent->mnt_type,
				            MS_NOATIME | MS_REMOUNT, "commit=604800"); // Change commit interval to one week
			else
				ret = mount(mntent->mnt_fsname, mntent->mnt_dir, mntent->mnt_type,
				            MS_NOATIME | MS_REMOUNT, "commit=0"); // Change commit interval to default
		}
	}
	endmntent(mounts);

	return ret;
}

static int check_for_sync(void)
{
	FILE *sync_action;
	DIR *devs;
	struct dirent *ent;
	char path_buf[PATH_MAX];
	char *action;
	int is_syncing = 0;

	devs = opendir("/sys/devices/virtual/block");
	if (!devs) {
		ERR("Unable to open sysfs");
		return 0;
	}

	/* More space efficient than using scandir()... */ 
	while ((ent = readdir(devs)) && !is_syncing) {
		if (strncmp(ent->d_name, "md", 2) != 0)
			continue;
		snprintf(path_buf, sizeof(path_buf), "/sys/devices/virtual/block/%s/md/sync_action", ent->d_name);
		sync_action = fopen(path_buf, "r");
		if (!sync_action)
			continue;
		if (fscanf(sync_action, "%as", &action) == 1)
		{
			
			if (strcmp(action, "idle") != 0)
				is_syncing = 1;
			free(action);
		}
		fclose(sync_action);
	}
	closedir(devs);

	return is_syncing;
}
#endif

/* Checks whether it's time to spin down a disk. If yes, it does so after
 * syncing the disk.
 * Returns 1 iff disk is properly spun down.
 */
static int try_spindown(disk_stat_t ds, disk_info_t di)
{
	static short int done=0;
	
	DEBUG("Disk %s, Time left %d.", di->name, di->time_left);

	if (di->time_left > 0) {
		/* Spindown time not reached. */
		done = 0;
		return 0;
	}

#ifdef __sparc__
	if ( check_xraid() ) {
		DEBUG("Disks want to spin down, but canceling because XRAID is busy.");
		return -2;
	}
#endif
	if (!done) {
		if (!vm_setting) {
			read_ahead_for_cron();
			vm_setting = 1;
			adjust_vm_settings();
		}
		readynas_lcd_sharp();
		readynas_lcd_disp_line(2, "Spinning down...");
		INFO("Spinning down disks.");
	}
	/* Wait for the reads to register in proc */
	disk_stat_update(ds);
	disk_stat_check(ds, di->major, di->minor);

	DEBUG("Spinning down %s.", di->name);

	/* Syncing can last a while, in which time new data might have been
	 * produced. So we sync twice, assuming that the second sync is
	 * quite fast.
	 */
	if (di->is_rw) {
		if (!done)
			done=1;
		sync_disk(di);
		sync_disk(di);
	}
	
	disk_stat_update(ds);
	
	/* Cancel spindown if there was other activity than our sync on the
	 * disk.
	 */
	if ((disk_stat_check(ds, di->major, di->minor) & ~DISK_STAT_WRITES) 
	    != DISK_STAT_VALID) {
		INFO("Spindown of %s cancelled.", di->name);
		di->time_left=timeout_get(di->timeouts);
		return -1;
	}
	
	return spindown(di);
}

/* Returns new noflushd state depending on number of rw disks spinning/stopped.
 */
static nfd_state_t check_io(disk_info_t di, disk_stat_t ds, int interval)
{
	int rw_spinning=0, rw_stopped=0;
	int irq_is_idle;
	disk_stat_flags io_flags;
	int new_access=0;
	int spindown_ret;
	disk_info_t di_w=di;
	time_t now, delta;
	short spundown = 0;

	disk_stat_update(ds);
	/* Racy, but just about as close as we can reasonably get. */
	irq_is_idle = intr_stat_check_idleness();

	
	for (; di; di=di->next) {
		if( spundown )
			usleep(500000);
		io_flags=disk_stat_check(ds, di->major, di->minor);
		switch (di->state) {
		case DISK_STATE_IGNORED:
			break;
		case DISK_STATE_SPINNING:
			if (io_flags==DISK_STAT_INVALID) {
				INFO("No stats for %s. Ignoring.", di->name);
				di->state = DISK_STATE_IGNORED;
				break;
			}
			if (io_flags & DISK_STAT_READS) {
				//DEBUG("New access detected: DISK_STATE_SPINNING && DISK_STAT_READS");
				new_access=1;
			} else {
				/* time_left is int, so potentially diving a
				 * little bit into negative should be okay. */
				if (di->time_left > 0)
					di->time_left-=interval;
				if (irq_is_idle) {
					spindown_ret = try_spindown(ds, di);
					if(spindown_ret > 0) {
						di->state=DISK_STATE_STOPPED;
						spundown=1;
					}
					else if(spindown_ret < 0) {
						//DEBUG("New access detected: spindown_ret < 0");
						new_access=1;
					}
				}
			}
			break;
		case DISK_STATE_STOPPED:
			if (io_flags==DISK_STAT_INVALID) {
				ERR("No stats for stopped disk %s", di->name);
				di->state=DISK_STATE_SPINNING;
				break;
			}
			if (io_flags & (DISK_STAT_READS|DISK_STAT_WRITES)) {
				//DEBUG("New access detected: DISK_STATE_STOPPED && DISK_STAT_READS|DISK_STAT_WRITES");
				new_access=1;
			}
			break;
		case DISK_STATE_UNINITIALISED:
		default:
			BUG("Illegal disk state %d on %s (%d, %d)", 
				di->state, di->name, di->major, di->minor);
		}

		/* Ignored disks are treated as spinning. */
		if (di->state==DISK_STATE_STOPPED)
			rw_stopped+=di->is_rw;
		else
			rw_spinning+=di->is_rw;
	}
	
	if (new_access) {
		spundown = 0;
		for (di=di_w; di; di=di->next) {
			switch (di->state) {
			case DISK_STATE_SPINNING:
				di->time_left=timeout_get(di->timeouts);
				break;
			case DISK_STATE_STOPPED:
				di->time_left=timeout_get(di->timeouts);
				di->state=DISK_STATE_SPINNING;

				now = time(NULL);
				if (now > di->spundown_at)
					delta = now - di->spundown_at;
				else
					delta = di->spundown_at-now;

				if (!spundown) {
					readynas_lcd_sharp();
					readynas_lcd_disp_line(2, "Spinning up...");
					INFO("Disks spinning up after %ld minutes.", delta/60);
					spundown = 1;
				}
				DEBUG("Spinning up %s after %ld minutes.",
				     di->name, delta/60);

				sync_disk(di);
				rw_stopped-=di->is_rw;
				rw_spinning+=di->is_rw;
				break;
			default:
				BUG("Illegal disk state %d on %s (%d, %d)", 
					di->state, di->name, di->major, di->minor);
			}
		}
		if (vm_setting) {
			vm_setting = 0;
			adjust_vm_settings();
		}
	}
	else if (rw_spinning > 0) {
		now = 0;
		for (di=di_w; di; di=di->next) {
			if (di->time_left > now)
				now = di->time_left;
		}
		if (((now <= interval+5) || (now < 30)) && !vm_setting) {
			read_ahead_for_cron();
			vm_setting = 1;
			adjust_vm_settings();
			sync_spinning_disks(di_w);
			sync_spinning_disks(di_w);
		}
	}

	DEBUG("rw disks: %d stopped, %d spinning", rw_stopped, rw_spinning);

	if (!rw_stopped)
		return NFD_STATE_SPINNING;
	if (!rw_spinning)
		return NFD_STATE_STOPPED;

	return NFD_STATE_PARTIAL;
}

void nfd_daemon(disk_info_t head, disk_stat_t stat)
{
	long left = 0, sync_left, interval;	
	time_t t_new, t_old;
	nfd_state_t nfd_state=NFD_STATE_UNINITIALISED;

	sync_left = 0;
	t_old = time(NULL);

	disk_info_t firstdisk=head;
	nfd_state_t old_nfd_state=nfd_state;

	read_ahead_for_cron();
	mount("/dev/md0", "/", "ext3", MS_NOATIME | MS_REMOUNT, "barrier=1");

	for (;;) {
		advance_timeouts(head);

		/* No need to check I/O stats if a RAID is syncing */
		if (check_for_sync()) {
			DEBUG("RAID activity running; skipping I/O check.");
			nfd_state = NFD_STATE_SPINNING;
			t_old = time(NULL);
			goto skipped;
		}

		/* This is nasty but the only way to eliminate a race
		 * between checking i/o stats and spindown. */
		kupdate_stop(kupdate);
		
		t_new = time(NULL);
		
		if (t_new > t_old)
			interval=t_new-t_old;
		else
			interval=t_old-t_new;
		
		t_old=t_new;
		
		DEBUG("Check interval %ld", interval);
		nfd_state=check_io(head, stat, interval);

		if(old_nfd_state != nfd_state) {
			/* JM: We need to log disk states in ramdisk for Frontview to check
			 INFO("Old/new state: %d/%d\n", old_nfd_state, nfd_state);
			*/
			if (!(log_status(firstdisk)))
				DEBUG("Logging disk spindown status failed!");
			old_nfd_state = nfd_state;
		}
		
		switch (nfd_state) {
		case NFD_STATE_SPINNING:
			kupdate_start(kupdate);
skipped:
			/* All disks spinning - poll 60 times minimum before
			 * spindown. */
			left = get_min_timeout(head)/60;
			sync_left = 0;
			break;
		/* XXX: The syncing code recognizes hotplugged disks now, but
		 *      we do not yet put them on the disk_info list. Don't
		 *      optimize the stopped case therefore, as one of these
		 *      alien disks might be out there. This will be handled
		 *      by rebuiling the disk_info list someday, but for now
		 *      we settle with the easy solution.
		 */
//JM		case NFD_STATE_STOPPED:
		case NFD_STATE_PARTIAL:
			/* We emulate kupdate - use its wakeup interval
			 * for sync calls, but keep (at most) default polling 
			 * interval for i/o checks. */
			if (sync_left <= 0) {
				sync_spinning_disks(head);
				sync_left = kupdate_get_interval(kupdate);
				/* Tricky case: Someone else has shut down
				 * kupdate/pdflush from under us. We're not
				 * turning it back on in this case, but we're
				 * still running our own syncing routine at a
				 * default 5 second interval. I'm not quite
				 * sure that this is the behaviour users
				 * expect here, though.
				 */
				if (sync_left <= 0)
					sync_left = 5;
			}
			left = get_min_timeout(head)/60;
			if (sync_left < left)
				left = sync_left;
			sync_left -= left;
			break;
		/* Temporarily disabled. See above. */
		case NFD_STATE_STOPPED:
			/* Poll for spinup every 5 seconds. */
			left = 5;
			sync_left = 0;
			break;
		case NFD_STATE_UNINITIALISED:
		default:
			BUG("Illegal state");
		
		}

		DEBUG("State %d, sleeping %ld seconds", nfd_state, left);
		while (!advance_timeout && (left=sleep(left)));
	}
}			

#define UNINITIALIZED -1
#define NO_SPECIAL_SETTINGS 0
#define SLOW_FAN_1300 0x000001

#define SPUN_DOWN 0x1F1111
#define SPINNING  0xFFFFFF

int log_status(disk_info_t whichdisk)
{
	FILE *log;
	int dimmer = SPUN_DOWN;
	int pid;
#ifdef __sparc__
	FILE *led;
	static int sys_flags = UNINITIALIZED;
	int minpwm = 64;
#endif
//#ifdef __arm__
#if 0 // Disable this, as it appears to hang the system
	char filename[16];
	char channel[64];
	int chan = -1;
#endif
	if (param_get("/var/run/monitor_enclosure.pid", &pid) > 0)
		kill(pid, SIGUSR2);

	log=fopen("/ramfs/spindown","w");
	if(!log)
		return 0;
	for (; whichdisk; whichdisk=whichdisk->next) {
		if (whichdisk->state==DISK_STATE_STOPPED) {
			fprintf(log,"%s:1\n", whichdisk->name);
		}
		else {
			fprintf(log,"%s:0\n", whichdisk->name);
			dimmer = SPINNING;
		}
//#ifdef __arm__
#if 0 // Disable this, as it appears to hang the system
		sprintf(filename, "/sys/block/%s", whichdisk->name+5);
		memset(channel, 0, sizeof(channel));
		if( readlink(filename, channel, sizeof(channel)) != -1 ) {
			chan = (atoi(strstr(channel, "host")+4));
			sprintf(channel, "/sys/class/scsi_host/host%d/link_power_management_policy", chan);
			led=fopen(channel, "w");
			if(led) {
				fprintf(led,"%s\n", (whichdisk->state==DISK_STATE_STOPPED) ? "medium_power" : "max_performance");
				fclose(led);
			}
		}
#endif
	}
	fclose(log);

#ifdef __sparc__
	param_set("/proc/sys/dev/led/set_pwm", dimmer);

	if (sys_flags == UNINITIALIZED) {
		DEBUG("Checking system type...");
		sys_flags = NO_SPECIAL_SETTINGS;
		char buffer[512];
		char model[16];
		FILE *boot_info = fopen("/proc/sys/dev/boot/info", "r");
		if (boot_info) {
			fgets(buffer, sizeof(buffer)-1, boot_info);
			while (!feof(boot_info)) {
				if (sscanf(buffer, "model: %15[^\n]", model) == 1) {
					DEBUG("Found model: '%s'", model);
					if ((strcmp(model, "ReadyNAS NV+") == 0) ||
					    (strcmp(model, "ReadyNAS Duo") == 0))
					sys_flags |= SLOW_FAN_1300;
				}
				else if (sscanf(buffer, "minpwm: %d[^\n]", &minpwm) == 1)
				{
					DEBUG("Found minpwm: %d", minpwm);
					break;
				}
				fgets(buffer, sizeof(buffer)-1, boot_info);
			}
			fclose(boot_info);
			DEBUG("sys_flags: %X", sys_flags);
		}
	}

	if (sys_flags & SLOW_FAN_1300) {
		if (dimmer == SPUN_DOWN) {
			param_set(RPM_OVERRIDE_FILE, 1300);
			param_set("/proc/sys/dev/hwmon/fan0_speed_control", minpwm);
		}
		else {
			unlink(RPM_OVERRIDE_FILE);
		}
	}
#endif
	return 1;
}

