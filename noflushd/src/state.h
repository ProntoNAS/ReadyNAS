/*
 * <state.h>
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: state.h,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
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

#include "disk_info.h"
#include "disk_stat.h"

#ifndef _NFD_STATE_H
#define _NFD_STATE_H

typedef enum nfd_states {
	NFD_STATE_UNINITIALISED,
	NFD_STATE_SPINNING,
	NFD_STATE_PARTIAL,
	NFD_STATE_STOPPED
} nfd_state_t;

typedef enum disk_states {
	DISK_STATE_UNINITIALISED,
	DISK_STATE_IGNORED,
	DISK_STATE_SPINNING,
	DISK_STATE_STOPPED
} disk_state_t;

void devname_init(void);
char *devname_get(char *);

void nfd_daemon(disk_info_t head, disk_stat_t stat);

#endif
