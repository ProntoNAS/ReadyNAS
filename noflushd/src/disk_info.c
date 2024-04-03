/*
 * <disk_info.c>
 *
 * Copyright (C) 2000 Daniel Kobras
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
 * 
 * $Id: disk_info.c,v 1.6 2008-11-20 03:35:47 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "diskhelper.h"
#include "disk_info.h"
#include "part_info.h"
#include "state.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>	/* for major()/minor() macros */
#include <sys/types.h>
#include <libgen.h>

#define APPEND

#ifdef APPEND
static disk_info_t prepend_entry(disk_info_t head)
{
	disk_info_t di;

	di = malloc(sizeof(struct disk_info_s));
	if (!di)
		return NULL;

	if( head )
		head->next=di;
	di->next = NULL;
	return di;
}		
#else
static disk_info_t prepend_entry(disk_info_t head)
{
	disk_info_t di;

	di = malloc(sizeof(struct disk_info_s));
	if (!di)
		return NULL;

	di->next=head;
	return di;
}		
#endif

static disk_info_t get_disk_entry(disk_info_t head, int major, int minor)
{
	for (;head; head=head->next)
		if (head->major==major && head->minor==minor)
			break;

	return head;
}

/*
 * Returns 1 if disk is a read/write mounted medium, 0 for read-only.
 * This check isn't too smart and currently only detects IDE CD-Roms.
 * If in doubt, we return 1 indication r/w which is the safe answer.
 */

static int check_rw(disk_info_t di, char *name)
{
	int dfd = -1, fd, ret = 1;
	char buf[6], cdrom[] = "cdrom";
	char *pos;
	
	/* Currently we only know about IDE cdroms. */
	if (di->major != IDE0_MAJOR && di->major != IDE1_MAJOR)
		goto _out;
	
	/* Kernel reports media type below /proc/ide/hd.../ even with
	 * devfs installed. So we rely on devfs names instead.
	 */
	if ((pos=strrchr(name, (int) '/'))) {
		if (!strcmp(pos+1, "cd"))
			ret = 0;
		goto _out;
	}
		
	fd = open(".", O_RDONLY);
	if (chdir("/proc/ide") ||
	    chdir(name) ||
	    (fd = open("media", O_RDONLY)) == -1)
		goto _out;
	
	/* Careful here. The kernel (currently) returns "cdrom  ",
	 * not "cdrom"! 
	 */
	if (read(fd, buf, 6) == 6 && !strncmp(buf, cdrom, 5))
		ret = 0;

	close(fd);

_out:
	if (dfd != -1)
		fchdir(dfd);
	else
		chdir("/");

	return ret;

}
	
/*
 * Parse /proc/partitions to determine which kinds of disks are installed
 * in the system.
 */

disk_info_t disk_info_init(void)
{
	part_info_t part;
	disk_info_t head=NULL;
	disk_info_t di=NULL;
	int major, minor;
	char *name;
	char buffer[512];
#ifdef __sparc__
	int skip_major = -1;
	int raid_running, start_at, parity, fake = 0;
	char *last_word;
	FILE *xraid = fopen("/proc/xraid/configuration", "r");

	if( xraid ) {
		fgets(buffer, sizeof(buffer)-1, xraid);
		while (!feof(xraid)) {
			if( sscanf(buffer, "RAID_INFO::disks_total=%d,raid_disks=%d,parity_disk=%d", &raid_running, &start_at, &parity) == 3 ) {
				switch( parity )
				{
					case 0:
						open("/dev/hdc", O_RDONLY);
						break;
					case 1:
						open("/dev/hde", O_RDONLY);
						break;
					case 2:
						open("/dev/hdg", O_RDONLY);
						break;
					case 3:
						open("/dev/hdi", O_RDONLY);
						break;
				}
			}
			else if( sscanf(buffer, "RUN_PARAMETERS::raid_running=%d,last_word=%a[^,],interface_start_at=%d,fake=%d", &raid_running, &last_word, &start_at, &fake) == 4 )
				break;
			fgets(buffer, sizeof(buffer)-1, xraid);
		}
		fclose(xraid);
		if( last_word )
			free(last_word);
		if( fake == 1 )
			skip_major = 22;
		else if( fake == 2 )
			skip_major = 33;
		else if( fake == 3 )
			skip_major = 34;
		else if( fake == 4 )
			skip_major = 56;
	}
#endif

	part = part_info_init();
	if (!part) {
		ERR("Unable to open /proc/partitions");
		return NULL;
	}
	
	while (part_info_disk_next(part)) {
		major = part_info_get_major(part);
		minor = part_info_get_minor(part);

		/* Ignore meta-devices, hdc, and scsi (usb). */
		if (IS_META(major, minor))
			continue;
#ifdef __sparc__
		if( major == skip_major ) {
			DEBUG("Skipping fake XRAID disk major %d", major);
			continue;
		}
#else // Skip SCSI devices on the USB bus
		char link[512];
		int len;
		sprintf(buffer, "/sys/block/%s", basename(devname_get(part_info_get_name(part))));
		len = readlink(buffer, link, sizeof(link));
		link[len] = '\0';
		if( strstr(link, "usb") )
			continue;
#endif

		name = strdup(devname_get(part_info_get_name(part)));

		di = prepend_entry(di);
		DEBUG("Added entry for %s (%d, %d)", name, major, minor);
#if 1 // Reverse order from default noflushd
		if( !head )
			head = di;
#endif

		di->major=major;
		di->minor=minor;
		di->timeouts=NULL;
		di->name=strdup(name);	
		di->is_rw=check_rw(di, name);	
		di->state=DISK_STATE_IGNORED;
	}
	
	part_info_release(part);

#if 1 // Return the first instead of last disk as head
	return head;
#else
	return di;
#endif
}

/* 
 * Return pointer to disk info structure associated with device <name>.
 */

disk_info_t disk_info_get_byname(disk_info_t head, char *name)
{
	int ret;
	struct stat statbuf;
	disk_info_t di;

	ret = stat(name, &statbuf);
	if (ret == -1) {
		ERR("Unable to stat disk");
		return NULL;
	}
	if (!S_ISBLK(statbuf.st_mode)) {
		ERR("%s is not a valid block device", name);
		return NULL;
	}
	di = get_disk_entry(head, major(statbuf.st_rdev), 
	                          minor(statbuf.st_rdev));
	if (!di) {
		ERR("No partition entry for device (%d,%d)",
			major(statbuf.st_rdev), minor(statbuf.st_rdev));
		/* Some people tend to get the command line syntax wrong.
		 * Try to be gentle and give a hint for names that look
		 * like /dev/hda1 (instead of /dev/hda). Catches devfs
		 * errors as well btw - those have .../part(.[0-9]) instead
		 * of .../disc. */
		if (strlen(name) && isdigit(name[strlen(name)-1]))
			ERR("Are you trying to spin down a partition "
			    "instead of the whole disk?");
		return NULL;
	}

	free(di->name);
	di->name=strdup(name);

	return di;
}

void disk_info_mark_ro(disk_info_t di)
{
	if (di)
		di->is_rw=0;
}

/*
 * Initialize a disk drive (based on its device node <name>) for idle
 * monitoring with timeout as in <tohead> list.
 */

void disk_info_setup(disk_info_t di, tolist_t tohead)
{
	int timeout;
	
	if (!di)
		return;

	di->spindown = spindown_setup(di->major, di->minor);	
	if (!di->spindown) {
		ERR("Don't know how to spindown %s", di->name);
		return;
	}
	
	di->timeouts = tohead;
	timeout = timeout_get(tohead);
	if (!NFD_TO_IS_VALID(timeout))
		ERR("No valid timeout found for %s", di->name);
	else if (NFD_TO_IS_REGULAR(timeout))
		di->state = DISK_STATE_SPINNING;

	DEBUG("Added disk %s, current timeout %d, state %d", 
		di->name, timeout, di->state);
}

