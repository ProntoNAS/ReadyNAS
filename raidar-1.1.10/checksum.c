/**********************************************************************
 * checksum.c                                               August 2001
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


#include <sys/types.h>

#include "int.h"


/**********************************************************************
 * raidard_checksum
 * Do a rolling 8 bit chekcsum
 * pre: buf: buffer to checksum
 *      size: Number of bytes in buffer to checksum
 * post: none
 * return: Rolling 8 bit checksum
 **********************************************************************/

uint32 raidard_checksum(void *buf, size_t size){
        size_t i;
        uint32 sum=0;
        unsigned char *c_buf;

        c_buf=(unsigned char *)buf;

        for(i=0 ; i<size ; i++,c_buf++){
                sum += *c_buf;
        }

        return(sum);
}
