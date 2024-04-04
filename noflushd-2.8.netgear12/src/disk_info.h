/*
 * <disk_info.h>
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: disk_info.h,v 1.7 2004/01/12 22:52:38 nold Exp $
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

#include <sys/time.h>

#ifndef _NFD_DISK_INFO_H
#define _NFD_DISK_INFO_H

typedef struct disk_info_s *disk_info_t;

#include "timeout.h"
#include "state.h"
#include "spindown.h"

struct disk_info_s {
	disk_info_t next;
	disk_state_t state;
	int	major;
	int	minor;
	int     time_left;
	tolist_t timeouts;
	int	is_rw;
	char    *name;		/* Name as given by user */
	int	channel;
	time_t  spundown_at;
	spindown_t spindown;
};
	

disk_info_t disk_info_init(void);
disk_info_t disk_info_get_byname(disk_info_t head, char *name);
void disk_info_setup(disk_info_t di, tolist_t tohead);
void disk_info_mark_ro(disk_info_t);
char *disk_info_get_devname(disk_info_t);
void disk_info_release_devname(char *);

#define disk_info_foreach(head, di) \
	for (di=head; di; di=di->next)

#define disk_info_is_rw(di) \
	(di->is_rw != 0)

#endif
