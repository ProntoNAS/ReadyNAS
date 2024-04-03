/*
 * <disk_stat.h>
 * Gather statistical data for disk accesses.
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: disk_stat.h,v 1.3 2008-06-09 20:36:55 ahu Exp $
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

#ifndef _NFD_DISK_STAT_H
#define _NFD_DISK_STAT_H

typedef struct disk_stat_s *disk_stat_t;

typedef enum disk_stat_flags_e {
	DISK_STAT_INVALID=0,	/* No accounting info for disk */
	DISK_STAT_VALID=1,	/* There is accounting info for disk */
	DISK_STAT_READS=2,	/* There were reads from disk */
	DISK_STAT_WRITES=4	/* There were writes to disk */
				/* read/write refer to the period between the
				 * last two calls to disk_stat_uptodate() */
} disk_stat_flags;

/* Initialise disk stat accounting. Returns handle to further disk stat calls,
 * or NULL on error. */
disk_stat_t disk_stat_init(void);
/* Update stats on all disks. */
void disk_stat_update(disk_stat_t);
/* Query stats on disk (major, minor) for latest update call. Return value is
 * or'ed of various disk_stat_flags, see above. */
disk_stat_flags disk_stat_check(disk_stat_t ds, int major, int minor);
/* Check XRAID status to see if we should skip spinning down disks */
#ifdef __sparc__
int check_xraid(void);
#endif

#endif
