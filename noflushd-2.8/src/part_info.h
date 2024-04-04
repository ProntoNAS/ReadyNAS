/*
 * <part_info.h>
 * 
 * (C) 2001 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: part_info.h,v 1.1 2001/07/23 21:35:23 nold Exp $
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

#ifndef _NFD_PART_INFO_H
#define _NFD_PART_INFO_H

/* The part_info set is used to query information on all disks and
 * partitions currently available in the system.
 */
typedef struct part_info_s *part_info_t;

/* Initialize and destroy resource infrastructure. */
part_info_t part_info_init(void);
void part_info_release(part_info_t);

/* Reset resources to begin new query. */
void part_info_reset(part_info_t);

/* Advance to next disk and next partition in current disk respectively.
 * Returns zero if no more disks/partitions are available, non-zero
 * otherwise.
 */
int part_info_disk_next(part_info_t);
int part_info_part_next(part_info_t);

/* Query name and device major/minor of current disk/partition entry. These
 * values change with each call to the *next() and *reset() methods.
 */
char *part_info_get_name(part_info_t);
int part_info_get_major(part_info_t);
int part_info_get_minor(part_info_t);

/* Returns true if device names in /proc/partitions follow the devfs
 * naming scheme. May only be used after part_info_init has been called
 * at least once.
 */
int part_info_is_devfs(void);

#endif
