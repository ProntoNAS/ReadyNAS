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
 * $Id: state.c,v 1.30 2010/07/25 00:59:26 nold Exp $
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

#include <dirent.h>

static part_info_t part;
static char *dev_prefix;
static int plen;
static int log_status(disk_info_t whichdisk);

const char *run_on_spinup = "/etc/noflushd/spinup.sh";
const char *run_on_spindown = "/etc/noflushd/spindown.sh";

extern int quitting;

/* Fixup name from /proc/partitions (i.e. from part_info_get_name())
 * to yield a valid absolute file name. The returned string may be
 * used until the next call to devname_get() only.
 */
char *devname_get(char *name)
{
	char *suffix;

	if (!dev_prefix)
		BUG("Device name requested without prefix.");
	
	dev_prefix = realloc(dev_prefix, strlen(name)+plen+1);
	if (!dev_prefix)
		BUG("Unable to allocate memory!");
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
	
	/* Try default name. */
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

/* In the days of udev, closing a block device that had been open for writing
 * triggers an udev event to re-read the partition table (presumably). The
 * read comes in asynchronously after the close(). In other words, the old
 * implementation of sync_part() below synched a device, closed the fds, and
 * thus triggered a read that started in parallel when we tried to issue the
 * spindown, causing an immediate spinup. This quick&dirty hack caches the
 * writable fd between calls to sync_part() so we can avoid the close()
 * without leaking fds. There are certainly saner places to cache this info,
 * but I was lazy.
 */
typedef struct fdcache fdcache_t;
struct fdcache {
	fdcache_t *next;
	char *name;
	int fd;
};

static fdcache_t *head = NULL;

static void fdcache_add(char *name, int fd)
{
	fdcache_t *n;

	n = malloc(sizeof(fdcache_t));
	if (!n)
		return;

	n->name = strdup(name);
	if (!n->name) {
		free(n);
		return;
	}

	n->fd = fd;

	if (head)
		n->next = head->next;
	else
		head = n;

	head->next = n;
	head = n;

	DEBUG("Add entry for %s (fd %d)", name, fd);
}

static int fdcache_lookup(char *name)
{
	fdcache_t *n;

	if (!head)
		return -1;

	n = head;

	do {
		if (!strcmp(n->name, name)) {
			/* Optimize the most likely next lookup. */
			head = n->next;
			DEBUG2("Found cached fd for %s (fd %d)", name, n->fd);
			return n->fd;
		}
		n = n->next;
	} while (n != head);

	return -1;
}

static void fdcache_close(void)
{
	fdcache_t *n, *next;

	if (!head)
		return;

	n = head;

	do {
		close(n->fd);
		next = n->next;
		free(n);
		n = next;
	} while (n != head);
}

static int fdcache_open(char *name, int mode) {
	int fd;

	fd = fdcache_lookup(name);
	if (fd != -1)
		return fd;

	fd = open(name, mode);
	if (fd == -1)
		return -1;

	fdcache_add(name, fd);

	return fd;
}	

static int sync_part(char *name)
{
	int fd, ret=0;

	DEBUG2("Syncing %s", name);
	fd = fdcache_open(name, O_WRONLY);
	if (fd==-1)
		return 0;
	if (!fsync(fd))
		ret=1;
	/* Closing a writable fd of a block device causes a re-read via udev.
	 * This could interfere with a spindown request. Therefore, we use
	 * the fdcache and skip the close(). */
	/* close(fd); */
	return ret;
}

/* Sync disk in current part setting. Returns number of successful
 * sync attempts. */
static int sync_current_disk(void)
{
	char *name;
	int ret=0;

	do {
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
	
	if (!sync_current_disk()) {
		INFO("%s: sync failed, marking read-only", di->name);
		disk_info_mark_ro(di);
	}
}
		
static void sync_spinning_disks(disk_info_t head)
{
	int succ;
	disk_info_t di;
	
	part_info_reset(part);
	
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

#if 1
/*NETGEAR - JM: Read ahead on file we know we'll be needing, so they don't
		      have to wake up the disks */
static void read_ahead(void) {
	int fd, i;
	off_t len;
	const char * const files[] = {  "/bin/mkdir",
					"/bin/bash",
					"/bin/dash",
					"/bin/df",
					"/bin/ls",
					"/bin/rm",
					"/bin/rmdir",
					"/bin/sleep",
					"/etc/hosts",
					"/etc/resolv.conf",
					"/usr/bin/env",
					"/usr/bin/stat",
					"/usr/bin/mawk",
					"/usr/bin/msmtpq",
					"/usr/bin/rnutil",
					"/var/lib/connman/settings",
					"/bin/run-parts",
					"/etc/pam.d/cron",
					"/etc/pam.d/common-session-noninteractive",
					"/frontview/bin/logtruncate",
					"/opt/readycloud/bin/radar.sh",
					run_on_spinup,
					run_on_spindown,
#ifdef __x86_64__
					/* /etc/cron.hourly cron need the following */
					"/lib/x86_64-linux-gnu/security/pam_unix.so",
					"/lib/x86_64-linux-gnu/security/pam_deny.so",
					"/lib/x86_64-linux-gnu/security/pam_permit.so",
					"/lib/x86_64-linux-gnu/security/pam_env.so",
					"/lib/x86_64-linux-gnu/security/pam_limits.so",
					"/lib/x86_64-linux-gnu/security/pam_mkhomedir.so",
#endif
					0
				      };

	DEBUG("Reading ahead to avoid future spinups.");
	for (i = 0; files[i]; i++) {
		fd = open(files[i], O_RDONLY);
		if( fd != -1 ) {
			len = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);
			if (readahead(fd, 0, len))
				ERR("Readahead %s failed.", files[i]);
			else
				DEBUG("Reading %s...", files[i]);
			close(fd);
		}
	}
}
#endif

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

int adjust_vm_settings(int spindown) {
	FILE *mounts;
	struct mntent *mntent;
	char last_dev[16] = "";
	int ret = 0;
	static int dirty_background_ratio = 0, dirty_background_bytes = 0;
	static int dirty_ratio = 0, dirty_bytes = 0;
	static int dirty_expire_centisecs = 0;
	static int spundown = 0;

	if (spindown == spundown)
		return 0;

	if (spindown) { // 1 == prepare for spindown
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
	spundown = spindown;

	mounts = setmntent("/proc/mounts", "r");
	if (!mounts)
		return -1;
	last_dev[15] = '\0';
	while ((mntent = getmntent(mounts))) {
		if ((strcmp(mntent->mnt_type, "btrfs") == 0) ||
		    (strcmp(mntent->mnt_type, "ext4") == 0) ||
		    (strcmp(mntent->mnt_type, "ext3") == 0)) {
			/* bind mount? */
			if (strcmp(mntent->mnt_fsname, last_dev) == 0)
				continue;
			/* skip usb/esata device */
			if (strncmp(mntent->mnt_dir, "/media", 6) == 0)
				continue;
			DEBUG("%s is mounted %s.  Remounting...", mntent->mnt_fsname, mntent->mnt_type);
			strncpy(last_dev, mntent->mnt_fsname, sizeof(last_dev)-1);
			ret = mount(mntent->mnt_fsname, mntent->mnt_dir, mntent->mnt_type, MS_NOATIME | MS_REMOUNT,
			            spindown ? "commit=604800" : "commit=0"); // Change commit interval to one week
		}
	}
	endmntent(mounts);

	if (block_dump)
		param_set("/proc/sys/vm/block_dump", spindown);

	return ret;
}

#ifdef __i386__
#define SYS_BLOCK "/sys/devices/virtual/block"
#else
#define SYS_BLOCK "/sys/block"
#endif

static int check_for_sync(void)
{
	FILE *sync_action;
	DIR *devs;
	struct dirent *ent;
	char path_buf[PATH_MAX];
	char action[8];
	int is_syncing = 0;

	devs = opendir(SYS_BLOCK);
	if (!devs) {
		ERR("Unable to open sysfs");
		return 0;
	}

	/* More space efficient than using scandir()... */ 
	while ((ent = readdir(devs)) && !is_syncing) {
		if (strncmp(ent->d_name, "md", 2) != 0)
			continue;
		snprintf(path_buf, sizeof(path_buf), SYS_BLOCK "/%s/md/sync_action", ent->d_name);
		sync_action = fopen(path_buf, "r");
		if (!sync_action)
			continue;
		if (fscanf(sync_action, "%7s", action) == 1)
		{
			if (strcmp(action, "idle") != 0)
				is_syncing = 1;
		}
		fclose(sync_action);
	}
	closedir(devs);

	return is_syncing;
}

/* Checks whether it's time to spin down a disk. If yes, it does so after
 * syncing the disk.
 * Returns 1 if disk is properly spun down.
 */
static int try_spindown(disk_stat_t ds, disk_info_t di)
{
	static int done = 0;

	DEBUG("Disk %s, Time left %d.", di->name, di->time_left);

	if (di->time_left > 0)
		/* Spindown time not reached. */
		return 0;
	
	/* read ahead to avoid future spinups */
	if (!done)
	{
		read_ahead();
		done = 1;
	}
	/* Wait for the reads to register in proc */
	disk_stat_update(ds);
	disk_stat_check(ds, di->major, di->minor);

	INFO("Spinning down disk %d (%s).", di->channel, di->name);
	if (access(run_on_spindown, X_OK) == 0)
		system(run_on_spindown);

	/* Syncing can last a while, in which time new data might have been
	 * produced. So we sync twice, assuming that the second sync is
	 * quite fast.
	 */
	if (di->is_rw) {
		sync_disk(di);
		sync_disk(di);
	}

	adjust_vm_settings(1);
	usleep(1800000);
	disk_stat_update(ds);
	
	/* Cancel spindown if there was other activity than our sync on the
	 * disk.
	 */
	if ((disk_stat_check(ds, di->major, di->minor) & ~DISK_STAT_WRITES) 
	    != DISK_STAT_VALID) {
		INFO("Spindown of disk %d (%s) cancelled.", di->channel, di->name);
		di->time_left=timeout_get(di->timeouts);
		return 0;
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

	disk_stat_update(ds);
	/* Racy, but just about as close as we can reasonably get. */
	irq_is_idle = intr_stat_check_idleness();
	
	for (; di; di=di->next) {
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
				di->time_left=timeout_get(di->timeouts);
			} else {
				/* time_left is int, so potentially diving a
				 * little bit into negative should be okay. */
				if (di->time_left > 0)
					di->time_left-=interval;
				if (irq_is_idle && try_spindown(ds, di)) 
					di->state=DISK_STATE_STOPPED;
			}
			break;
		case DISK_STATE_STOPPED:
			if (io_flags==DISK_STAT_INVALID) {
				ERR("No stats for stopped disk %s", di->name);
				di->state=DISK_STATE_SPINNING;
				break;
			}
			//DEBUG("io_flags 0x%x", io_flags);
			if (io_flags & (DISK_STAT_READS|DISK_STAT_WRITES)) {
				time_t now, delta;

				di->time_left=timeout_get(di->timeouts);
				di->state=DISK_STATE_SPINNING;
				
				now = time(NULL);
				if (now > di->spundown_at)
					delta = now - di->spundown_at;
				else
					delta = di->spundown_at-now;
				
				INFO("Spinning up disk %d (%s) after %ld:%02ld:%02ld.",
				     di->channel, di->name, delta/3600, delta/60%60, delta%60);
				if (access(run_on_spinup, X_OK) == 0)
					system(run_on_spinup);

				sync_disk(di);
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
	
	DEBUG2("rw disks: %d stopped, %d spinning", rw_stopped, rw_spinning);

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

	read_ahead();

	while (!quitting) {
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
		
		DEBUG2("Check interval %ld", interval);
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
			adjust_vm_settings(0);
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
		case NFD_STATE_STOPPED:
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
#if 0
		/* Temporarily disabled. See above. */
		case NFD_STATE_STOPPED:
			/* Poll for spinup every 5 seconds. */
			left = 5;
			sync_left = 0;
			break;
#endif
		case NFD_STATE_UNINITIALISED:
		default:
			BUG("Illegal state");
		
		}

		DEBUG2("State %d, sleeping %ld seconds", nfd_state, left);
		while (!advance_timeout && (left=sleep(left)));
	}
	check_io(head, stat, interval);
}

#define UNINITIALIZED -1
#define NO_SPECIAL_SETTINGS 0
#define SLOW_FAN_1300 0x000001

#define SPUN_DOWN 0x1F1111
#define SPINNING  0xFFFFFF

int log_status(disk_info_t whichdisk)
{
	FILE *log;
	int pid;

	if (param_get("/var/run/monitor_enclosure.pid", &pid) > 0)
		kill(pid, SIGUSR2);

	log=fopen("/run/spindown","w");
	if(!log)
		return 0;
	for (; whichdisk; whichdisk=whichdisk->next) {
		if (whichdisk->state==DISK_STATE_STOPPED) {
			fprintf(log,"%s:1\n", whichdisk->name);
		}
		else {
			fprintf(log,"%s:0\n", whichdisk->name);
		}
	}
	fclose(log);

	return 1;
}
