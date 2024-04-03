/*
 * <spindown.h>
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: spindown.h,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
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
#include "bug.h"

#ifndef _NFD_SPINDOWN_H
#define _NFD_SPINDOWN_H

typedef int(*spindown_t)(int);

spindown_t spindown_setup(int major, int minor);
int spindown(disk_info_t di);

#endif
