/**********************************************************************
 * interfaces.h                                               July 2001
 * Horms                                             horms@vergenet.net
 * Andrew Tridgell                                     tridge@samba.org
 * Headers for interfaces.c which has been taken verbatim from
 * Samba 2.2.1a
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
#ifndef _RAIDARD_INTERFACES_H
#define _RAIDARD_INTERFACES_H

#include <netinet/in.h>
#include <stdint.h>
#ifdef __linux__
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#endif

struct iface_struct {
	struct sockaddr sa;
	struct sockaddr broadaddr;
	char mac_addr[18];
//	char host_name[32];
};

int get_interfaces(struct iface_struct *ifaces);

char *get_ip_str(const struct sockaddr *sa, char *ip);

int open_netlink_sock(void);

#endif /* _RAIDARD_INTERFACES_H */
