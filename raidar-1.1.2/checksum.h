/**********************************************************************
 * checksum.h                                               August 2001
 * Horms                                             horms@vergenet.net
 *                                                    horms@valinux.com
 *
 * Code to cheksum data
 *
 * iproxy
 * UDP - TCP proxy
 * Copyright (C) 2001  Andrew Tridgell <tridge@samba.org>
 *                     Horms <horms@vergenet.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 **********************************************************************/


#ifndef _RAIDARD_CHECKSUM_H
#define _RAIDARD_CHECKSUM_H

#include "int.h"


/**********************************************************************
 * raidard_checksum
 * Do a rolling 8 bit chekcsum
 * pre: buf: buffer to checksum
 *      size: Number of bytes in buffer to checksum
 * post: none
 * return: Rolling 8 bit checksum
 **********************************************************************/

uint32 raidard_checksum(void *buf, size_t size);


/**********************************************************************
 * raidard_checksum_skip
 * Do a rolling checksum as per raidard_checksum but sckip the first word.
 * This is so the checksum itself can be inserted into the first work
 * without effecting the result of this call
 * pre: _buf: Buffer to checksum
 *            Must be an array so sisof(*_buf) will work
 * post: none
 * return: Checksum of _buf, skipping the first word
 **********************************************************************/
#define raidard_checksum_skip(_buf) \
(raidard_checksum((void *)((char *)_buf + sizeof(uint32)), \
                sizeof(*_buf) - sizeof(uint32)))


#endif /* _RAIDARD_CHECKSUM_H */
