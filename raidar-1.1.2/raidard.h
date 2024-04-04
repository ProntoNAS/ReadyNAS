/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * raidard.h is based on                                           
 * iproxy.h                                                   July 2001
 * Horms                                             horms@vergenet.net
 * Andrew Tridgell                                     tridge@samba.org
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


#ifndef _RAIDARD_H
#define _RAIDARD_H

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "int.h"

#define FLAG_NEW	0x01
#define FLAG_BOOTING	0x02
#define FLAG_INITRD	0x04
#define FLAG_SUPPORT	0x08
#define FLAG_ONLINE	0x10

#define RAIDARD_PORT 0x5641 /* VA in hex */
#define RAIDARD_SID 0x42
#define RAIDARD_CID 0x7
#define WEB_PORT 80

#define ADDR_LOCALHOST "127.0.0.1"
#define ADDR_BROADCAST "255.255.255.255"
#define ADDR_MULTICAST6 "FF7E:230::1234"
#define ADDR_ANY "0.0.0.0"
#define ADDR_MULTICAST "224.7.0.1" /* We need a propper mulicast group */
#define ADDR_UNICAST NULL

#define TTL_MULTICAST 1

#define MODE_BROADCAST 0x01
#define MODE_MULTICAST 0x02
#define MODE_UNICAST   0x04
#define MODE_CLIENT    0x10
#define MODE_SERVER    0x20

#endif /* _RAIDARD_H */
