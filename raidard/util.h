/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * util.h                                                     July 2001
 * Andrew Tridgell                                     tridge@samba.org
 * Horms                                             horms@vergenet.net
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


#ifndef _RAIDARD_UTIL_H
#define _RAIDARD_UTIL_H

#include <netinet/in.h>


/****************************************************************************
open a socket of the specified type, port and address for incoming data
****************************************************************************/
int open_socket_in_addr(int type, const struct in_addr *addr, int port);



/****************************************************************************
  create an outgoing socket. 
  **************************************************************************/
int open_socket_out_addr(int type, const struct in_addr *addr, int port, 
                int broadcast);

/****************************************************************************
  create an outgoing socket. 
  **************************************************************************/
int open_socket_out(int type, const char *dst, int port, int broadcast);

size_t write_all(int fd, void *buf, size_t size);

#endif /* _RAIDARD_UTIL_H */
