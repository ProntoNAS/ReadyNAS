/*
 * <noflushd.h>
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: noflushd.h,v 1.3 2000/10/03 17:00:59 belbo Exp $
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

#include "kupdate.h"

#ifndef _NFD_NOFLUSHD_H
#define _NFD_NOFLUSHD_H

/* Globals needed by signal handlers */
extern kupdate_t kupdate;
extern volatile int advance_timeout;

extern int verbose;	/* Verbosity level */
extern int released;	/* True if running detached */
extern int block_dump;	/* True if running detached */
#endif
