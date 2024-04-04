/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * interface.h                                                July 2001
 * Horms                                             horms@vergenet.net
 * Andrew Tridgell                                     tridge@samba.org
 *
 * Associate each network interface with a UDP socket
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


#ifndef _RAIDARD_INTERFACE_H
#define _RAIDARD_INTERFACE_H

#include <stdint.h>

#define MAX_INTERFACES 16

/**********************************************************************
 * interface_open_fd
 * Open a file descriptor bound to each network interface, to send 
 * mulicast or broadcast packets.
 * pre: Private state needs to have been initilaised using interfaces_get()
 *      raidard_mode should be set to MODE_MULTICAST or MODE_BROADCAST
 *      bind_addr: Address to bind to
 *      mcast_addr: Address of multi-cast group. Only used if
 *                  raidard_mode is MODE_MULTICAST
 *      port: Port to bind to
 * post: If raidard_mode is MODE_UNICAST
 *           Bind to bind_addr
 *       Else
 *         A UDP file descriptor is bound to each interface
 *         If raidard_mode is MODE_MULTICAST
 *           set the socket option SO_BROADCAST on each file descriptor
 *         Else (raidard_mode is MODE_MULTICAST)
 *           join the multicast group mcast_addr
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int interface_open_fd(const char *bind_addr, const char *mcast_addr,
                uint32_t port);


/**********************************************************************
 * interfaces_get
 * Find all the network interfaces on the current host
 * pre: none
 * post: 
 *       If raidard_mode is not  MODE_UNICAST
 *         Zero iface_count
 *       Else
 *         The private variables iface_count and iface are seeded
 *         with information on all the network interfaces for the 
 *         current host.
 * return: Number of interfaces found
 *         <= 0 on error, as returned by get_interfaces()
 **********************************************************************/

int interfaces_get();



/**********************************************************************
 * interface_sendto_all
 * Write a buffer to the file desciptor associated with each interface
 * pre: msg: Data to send
 *      len: Length of msg in bytes
 *      flags: flags
 *      to: pointer to destination of message
 *      tolen: size of what to is poiting to
 *      Note: All parameters are passed verbatim to sendto(2). Please
 *            see the manpage for that libc function for the more
 *            information on the semantics of these arguments.
 * post: message is sent to file descriptor associated with each 
 *       network interface.
 *       Note: if raidard_mode is MODE_UNICAST then the message is just
 *             sent to a socket bound to IN_ADDR_ANY and the host's routing
 *             should send this out exactly one interface.
 * return: 1 on success
 *         -1 on error
 **********************************************************************/

int interface_sendto_all(const void *msg, size_t len, int flags, 
                const struct sockaddr *to, socklen_t tolen);


/**********************************************************************
 * interface_add_to_read_template
 * Add file descriptors assoiated with each interface to
 * a read template.
 * pre: read_template: Read template to add file descriptors to
 * post: The file descriptors associate with each interface are
 *        added to read_template;
 * return: Number of file descriptors added.
 *         -1 on error.
 **********************************************************************/

int interface_add_to_read_template(fd_set *read_template);


/**********************************************************************
 * interface_set_fd_start
 * Reset internal state about set fds
 * You must call this before the first time you call
 * interface_set_fd_next after the read_template being refered
 * changes.
 * pre: none
 * post: Internal state about set fds is reset
 * return: none
 **********************************************************************/

void interface_set_fd_start(void);


/**********************************************************************
 * interface_set_fd_next
 * Find the next connection whose associated tcp socket is in the
 * read_tepmlate
 * pre: read_template: read_template to reference
 * post: internal status of what connection we are up to is updated.
 * return: Next connection whose TCP socket is set in the read_template
 *         -1 if no connection is found
 **********************************************************************/

int interface_set_fd_next(fd_set *read_template);


/**********************************************************************
 * interfacee_set_fd_end
 * Reset internal state about set fds
 * You should call this after the last time you call
 * interfacee_set_fd_next after the read_template being refered
 * changes.
 *
 * Actually it does nothing, but is here for completeness
 *
 * pre: none
 * post: none
 * return: none
 **********************************************************************/

void interface_set_fd_end(void);

#endif /* _RAIDARD_INTERFACE_H */
