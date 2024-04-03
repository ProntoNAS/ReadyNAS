/*
 * <disk_stat.c>
 *
 * Copyright (C) 2000-2003 Daniel Kobras
 *           (C) 2003 Michael Buesch <fsdeveloper@yahoo.de> (Linux 2.5-style
 *                    support)
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
 * $Id: disk_stat.c,v 1.5 2010-03-06 01:42:04 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "disk_stat.h"
#include "disk_info.h"
#include "bug.h"
#include "diskhelper.h"
#include "noflushd.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

struct iostat {
	unsigned int	reads;
	unsigned int	writes;
	unsigned int	rio;
	unsigned int	wio;
};

typedef struct disk_io_s *disk_io_t;

struct disk_io_s {
	disk_io_t	next;
	int		major;	/* FIXME: Major/index are only needed for */
	int		index;	/* newstyle stat. Put into union. */
	struct iostat	oldio;
	struct iostat	newio;
};

typedef struct disk_stat_ops_s {
	disk_io_t (*get_entry)(disk_stat_t, int major, int minor);
	void (*update_io)(disk_stat_t);
} *disk_stat_ops_t;

struct disk_stat_s {
	disk_io_t dio_head;
	disk_io_t dio_old[4];
	FILE *stat;
	struct disk_stat_ops_s ds_ops;
};

/*
 * Legacy section for oldstyle /proc/stat, using 4 columns of
 * disk_* entries.
 *
 */

/*
 * The disk stats in /proc/stat use a rather clumsy mapping of device
 * major/minor -> number of column. We just care about the usual IDE 
 * and SCSI entries even though the kernel supports more device types.
 * Additionally, some majors indicate meta-devices built on top of 
 * other block devices, so care must be taken not to account a single
 * disk more than once. A return value < 0 indicates that we have found
 * such a meta-device.
 */

static int old_entry(int major, int minor)
{
	int entry;

	/* Devices that show up in /proc/partitions but have no
	 * physical counterpart. 
	 */
	if (IS_META(major, minor))
		return -1;

	switch (major) {
		case IDE0_MAJOR:
			entry=0;
			if (minor)
				entry++;
			break;
		case IDE1_MAJOR:
			entry=2;
			if (minor)
				entry++;
			break;
		case SCSI_DISK0_MAJOR:
			entry=minor>>4;
			break;
		default:
			entry=4;
	}

	return entry;
}

static disk_io_t get_entry_old(disk_stat_t ds, int major, int minor)
{
	int entry;
	
	entry = old_entry(major, minor);

	if (entry > 3 || entry < 0) {
		DEBUG("Warning! get_entry_old() called on bad disk (%d, %d).",
		      major, minor);
		
		return NULL;
	}

	return ds->dio_old[entry];
}

static disk_io_t alloc_entry_old(disk_stat_t ds, int entry)
{
	if (entry > 3) {
		ERR("Cannot account entry %d", entry);
		return NULL;
	}

	if (entry < 0)
		return NULL;

	if (ds->dio_old[entry]) {
		DEBUG("/proc/stat entry for %d has conflicts.", entry);
		return ds->dio_old[entry];
	}
	
	ds->dio_old[entry] = malloc(sizeof(struct disk_io_s));
	if (!ds->dio_old[entry])
		BUG("Not enough memory for disk_info.");
	
	ds->dio_old[entry]->next = ds->dio_head;
	ds->dio_head = ds->dio_old[entry];

	DEBUG("Allocated stat entry %d", entry);

	return ds->dio_old[entry];
}

/*
 * Central monitoring routine. Accounting is based on the information
 * available in /proc/stat. Unfortunately /proc/stat logic is pretty
 * broken, e.g. on a system with /dev/hda and /dev/sda you're screwed as
 * both disks get accounted in the same column. An interface to Stephen
 * Tweedie's sard might be a good thing to do in the future.
 */

static void update_io_old(disk_stat_t ds)
{
	FILE *stat;
	int i, succ = 0;
	unsigned int rio[4], wio[4], rblk[4], wblk[4];

	stat=ds->stat;
	fflush(stat);
	rewind(stat);
	
	while ((succ < 4) && stat && !feof(stat)) {
		fpos_t pos;
		fgetpos(stat, &pos);
		if (fscanf(stat, "disk_rio %u %u %u %u\n",
			   &rio[0], &rio[1], &rio[2], &rio[3]) == 4) {
			succ++;
			continue;
		}
		fsetpos(stat, &pos);	/* Must rewind to beginning of line */
		if (fscanf(stat, "disk_wio %u %u %u %u\n",
		           &wio[0], &wio[1], &wio[2], &wio[3]) == 4) {
			succ++;
			continue;
		}
		fsetpos(stat, &pos);	/* Must rewind to beginning of line */
		if (fscanf(stat, "disk_rblk %u %u %u %u\n",
			   &rblk[0], &rblk[1], &rblk[2], &rblk[3]) == 4) {
			succ++;
			continue;
		}
		fsetpos(stat, &pos);	/* Must rewind to beginning of line */
		if (fscanf(stat, "disk_wblk %u %u %u %u\n",
			   &wblk[0], &wblk[1], &wblk[2], &wblk[3]) == 4) {
			succ++;
			continue;
		}

		next_line(stat);
	}

	if (succ < 4) {
		/* Trying to fail gracefully here proved to be a bad move. */
		BUG("Error reading /proc/stat. (%d)", succ);
	}

	for (i=0; i < 4; i++) {
		if (!ds->dio_old[i] && !alloc_entry_old(ds, i))
			continue;
		ds->dio_old[i]->newio.rio = rio[i];
		ds->dio_old[i]->newio.wio = wio[i];
		ds->dio_old[i]->newio.reads = rblk[i];
		ds->dio_old[i]->newio.writes = wblk[i];
	}
	
}

/* Return true (1) if /proc/stat is oldstyle, false (0) otherwise.
 */

static int check_oldstyle(disk_stat_t ds)
{
	FILE *stat;
	int succ = 0;
	unsigned int dummy[4];

	if (!ds->stat) {
		stat=fopen("/proc/stat", "r");
		if (!stat)
			return 0;
		ds->stat=stat;
	} else {
		stat=ds->stat;
		fflush(stat);
		rewind(stat);
	}

	while ((succ < 4) && stat && !feof(stat)) {
		fpos_t pos;
		fgetpos(stat, &pos);
		if (fscanf(stat, "disk_rio %u %u %u %u\n",
			   &dummy[0], &dummy[1], &dummy[2], &dummy[3]) == 4) {
			succ++;
			continue;
		}
		fsetpos(stat, &pos);	/* Must rewind to beginning of line */
		if (fscanf(stat, "disk_wio %u %u %u %u\n",
			   &dummy[0], &dummy[1], &dummy[2], &dummy[3]) == 4) {
			succ++;
			continue;
		}
		fsetpos(stat, &pos);	/* Must rewind to beginning of line */
		if (fscanf(stat, "disk_rblk %u %u %u %u\n",
			   &dummy[0], &dummy[1], &dummy[2], &dummy[3]) == 4) {
			succ++;
			continue;
		}
		fsetpos(stat, &pos);	/* Must rewind to beginning of line */
		if (fscanf(stat, "disk_wblk %u %u %u %u\n",
			   &dummy[0], &dummy[1], &dummy[2], &dummy[3]) == 4) {
			succ++;
			continue;
		}

		next_line(stat);
	}
	
	return (succ==4);
}

/*
 * Newstyle /proc/stat handlers.
 */

/*
 * minor2index() corresponds to the kernel's disk_index() function
 * defined in include/linux/genhd.h. It maps a (major,minor) pair to
 * (major,index) as used in /proc/stat. Earlier versions added 2 to
 * the index for disks on ide1. This is dropped now, however, and we
 * provide compatibility via a kludge in update_io_new().
 */

static int minor2index(int major, int minor)
{
	int index;
	
	if (IDE_DISK_MAJOR(major))
		index = (minor & 0x0040) >> 6;
	else if (SCSI_DISK_MAJOR(major))
		index = (minor & 0x00f0) >> 4;
	else
		return 0;

	return index;
}

/* 
 * __get_entry_new takes (major, disk index) tuple as used in /proc/stat,
 * while the exported get_entry_new method needs the disk's
 * major/minor code.
 */

static disk_io_t __get_entry_new(disk_stat_t ds, int major, int index)
{
	disk_io_t di;

	/* On typical systems the list will contain only one or two entries
	 * so no need to do fancy optimisations here. */
	for (di=ds->dio_head; di; di=di->next)
		if (di->major == major && di->index == index)
			break;

	return di;
}

static disk_io_t get_entry_new(disk_stat_t ds, int major, int minor)
{
	return __get_entry_new(ds, major, minor2index(major, minor));
}

static disk_io_t alloc_entry_new(disk_stat_t ds, int major, int index)
{
	disk_io_t di;

	/* Filter out meta devices */
	if (IS_META(major, 0))
		return NULL;
	
	di = __get_entry_new(ds, major, index);
	if (di) 
		BUG("BUG! Double allocation of (%d, %d).", major, index);
	
	di = malloc(sizeof(struct disk_io_s));
	if (!di) 
		BUG("Not enough memory for disk_info.");

	di->major = major;
	di->index = index;

	di->next = ds->dio_head;
	ds->dio_head = di;

	DEBUG("Allocated stat entry for (%d, %d)", major, index);

	return di;
}

/* Some disk indices differ between kernel revisions. Grmpf. */
static int index_fixup(int major, int index)
{
	/* 2.4.8-ac-whatever removed the unconditional +2 */
	if (major == IDE1_MAJOR && index > 1)
		index -= 2;

	return index;
}

static void update_io_new(disk_stat_t ds)
{
	FILE *stat;
	char s;
	unsigned int major, index;
	unsigned int sum, rio, rblk, wio, wblk;
	disk_io_t di;
	
	stat=ds->stat;
	fflush(stat);
	rewind(stat);
	
	while (!feof(stat)) {
		if (fscanf(stat, "disk_io:%c", &s) != 1) {
			next_line(stat);
			continue;
		}
		
		while (fscanf(stat, "(%u,%u):(%u,%u,%u,%u,%u) ",
			      &major, &index, 
			      &sum, &rio, &rblk, &wio, &wblk) == 7) { 
			
			index = index_fixup(major, index);
			
			if (!(di = __get_entry_new(ds, major, index)))
				di = alloc_entry_new(ds, major, index);
			if (di) {
				di->newio.rio = rio;
				di->newio.wio = wio;
				di->newio.reads = rblk;
				di->newio.writes = wblk;
			} else {
				DEBUG("No entry for (%d,%d)", major, index);
			}
		}

		next_line(stat);
	}
}

static int check_newstyle(disk_stat_t ds)
{
	FILE *stat;
	int succ = 0, fd = -1;
	unsigned int dummy;
	
	
	if (!ds->stat) {
		stat=fopen("/proc/stat", "r");
		if (!stat)
			return 0;
		ds->stat=stat;
	} else {
		stat=ds->stat;
		fflush(stat);
		rewind(stat);
	}
	
	while (!feof(stat) && !succ) {
		if (fscanf(stat, "disk_io: (%u,%u):(%u,%u,%u,%u,%u) ",
		           &dummy, &dummy, 
			   &dummy, &dummy, &dummy, &dummy, &dummy) == 7)
			succ++;
		else
			next_line(stat);
	}

	if (fd != -1)
		fclose(stat);
	
	return succ;
}

/*
 * On kernel versions 2.5 and up, gather disk statistics from /proc/diskstat.
 */

static int check_newstyle_25(disk_stat_t ds)
{
	FILE *stat;
	int succ = 0;
	unsigned int dummy;
	char ch_dummy[10];

	if (!ds->stat) {
		stat = fopen("/proc/diskstats", "r");
		if (!stat)
			return 0;
		ds->stat = stat;
	} else {
		stat = ds->stat;
		fflush(stat);
		rewind(stat);
	}

	while (!feof(stat) && !succ) {
		if (fscanf(stat, "%u %u %9s %u %u %u %u %u %u %u %u %u %u %u",
			   &dummy, &dummy, ch_dummy,
			   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
			   &dummy, &dummy,
			   &dummy, &dummy, &dummy) == 14)
			succ++;
		else
			next_line(stat);
	}

	return succ;
}

static void update_io_25(disk_stat_t ds)
{
	FILE *stat;
	int ret;
	unsigned int major, minor;
	unsigned int dummy, rio, rblk, wio, wblk;
	char ch_dummy[10], *line;
	disk_io_t di;

	stat = ds->stat;
	fflush(stat),
	rewind(stat);

	while (!feof(stat)) {
		/* Make sure we only parse a single line at once. */
		if (!(line = get_line(stat)))
			break;
		
		ret = sscanf(line,
		           "%u %u %9s %u %u %u %u %u %u %u %u %u %u %u",
		           &major, &minor, ch_dummy,
		           &rio, &dummy, &rblk, &dummy, &wio, &dummy, &wblk,
		           &dummy, &dummy, &dummy, &dummy);
		
		release_line(line);
		
		if (ret != 14)
			continue;
#ifdef __sparc__
		if( major != 22 && major != 33 && major != 34 &&
		    major != 56 && major != 57 && major != 88 &&
		    major != 89 && major != 90 )
			continue;
#else
		if( major != 8 )
			continue;
#endif

		if (!(di = __get_entry_new(ds, major, minor)))
			di = alloc_entry_new(ds, major, minor);
		if (di) {
			di->newio.rio = rio;
			di->newio.wio = wio;
			di->newio.reads = rblk;
			di->newio.writes = wblk;
		} else {
			DEBUG("No entry for (%d,%d)", major, minor);
		}
	}
}

/*
 * Try to determine proper /proc/stat handlers and set up generic functions.
 */

disk_stat_t disk_stat_init(void)
{
	disk_stat_t ds;

	ds=calloc(1, sizeof(struct disk_stat_s));
	if (!ds)
		return NULL;

	if (check_newstyle_25(ds)) {
		DEBUG("Kernel talks linux 2.5-style stat");
		ds->ds_ops.update_io = update_io_25;
		ds->ds_ops.get_entry = __get_entry_new; /* Indeed. :) */
		return ds;
	}
	if (check_newstyle(ds)) {
		DEBUG("Kernel talks newstyle stat");
		ds->ds_ops.update_io = update_io_new;
		ds->ds_ops.get_entry = get_entry_new;
		return ds;
	} 
	if (check_oldstyle(ds)) {
		DEBUG("Kernel talks oldstyle stat");
		ds->ds_ops.update_io = update_io_old;
		ds->ds_ops.get_entry = get_entry_old;
		return ds;
	}
	
	return NULL;
}

void disk_stat_update(disk_stat_t ds)
{
	ds->ds_ops.update_io(ds);
}

disk_stat_flags disk_stat_check(disk_stat_t ds, int major, int minor)
{
	disk_io_t di;
	disk_stat_flags flags;
	
	di = ds->ds_ops.get_entry(ds, major, minor);

	if (!di)
		return DISK_STAT_INVALID;

	flags = DISK_STAT_VALID;

	if (di->oldio.rio != di->newio.rio || 
	    di->oldio.reads != di->newio.reads)
		flags |= DISK_STAT_READS;

	if (di->oldio.wio != di->newio.wio ||
	    di->oldio.writes != di->newio.writes)
		flags |= DISK_STAT_WRITES;

	memcpy(&di->oldio, &di->newio, sizeof(struct iostat));

	return flags;
}

#ifdef __sparc__
int check_xraid(void)
{
	FILE *xraid;
	char buffer[501];
	char s;
	int busy;

	if( !(xraid = fopen("/proc/xraid/configuration", "r")) ) {
		return 0;
	}

	fgets(buffer, 500, xraid);
	while (!feof(xraid)) {
		if (sscanf(buffer, "RAID_REBUILD:%c", &s) == 1) {
			sscanf(buffer, "RAID_REBUILD::sync=%d,", &busy);
			if(busy) {
				fclose(xraid);
				return 1;
			}
		} else if(sscanf(buffer, "RAID_P_CHECK:%c", &s) == 1) {
			sscanf(buffer, "RAID_P_CHECK::chck=%d,", &busy);
			if(busy) {
				fclose(xraid);
				return 2;
			}
		}
		fgets(buffer, 500, xraid);
	}
	fclose(xraid);
	return 0;
}
#endif

