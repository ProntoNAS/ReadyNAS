/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * interface.c                                                July 2001
 * Horms                                             horms@vergenet.net
 * Andrew Tridgell                                     tridge@samba.org
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



#define _USE_BSD
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdint.h>

#include "interfaces.h"
#include "interface.h"
#include "raidard.h"
#include "util.h"


/**********************************************************************
 * Private data about interfaces
 **********************************************************************/

struct iface_struct iface[MAX_INTERFACES];
int iface_fd[MAX_INTERFACES+1];
int iface_count=0;




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


int interface_open_fd(const char *bind_addr, const char *mcast_addr, uint32_t port)
{
	int i;
	int one=1;
	int fd_out;
	socklen_t namelen;
	struct ip_mreq mreq;
	struct ipv6_mreq mreq6;
	struct sockaddr_in6 *sock6;
	struct in_addr bind_addr_bin;
	struct in_addr mcast_addr_bin = {};
	struct sockaddr_in name;
	struct sockaddr_in *sock;
	extern uint32_t mcast_ttl;
	extern unsigned short raidard_mode;

	/* Open outgoing UDP socket */
	if(inet_pton(AF_INET, bind_addr, &bind_addr_bin) <= 0) {
		perror("inet_pton bind_addr");
		return(-1);
	}

	if (raidard_mode & MODE_MULTICAST) {
		if(inet_pton(AF_INET, mcast_addr, &mcast_addr_bin) <= 0) {
			perror("inet_pton mcast_addr");
			return(-1);
		}
	}


	fd_out = open_socket_in_addr(SOCK_DGRAM, &bind_addr_bin, port);
	if (fd_out == -1) {
		fprintf(stderr, "Can't open socket\n");
		return(-1);
	}

	if (raidard_mode & MODE_UNICAST)
		goto interface_open_fd_end;

	namelen=sizeof(name);
	if(getsockname(fd_out, (struct sockaddr *)&name, &namelen)){
		perror("do_proxy: getsockname");
		close(fd_out);
		return(-1);
	}


	for (i = 0; i < iface_count; i++) {

		switch(iface[i].sa.sa_family) {
		case AF_INET:
			sock = (struct sockaddr_in *)&(iface[i].sa);
			sock->sin_port = name.sin_port;
			iface_fd[i] = socket(AF_INET, SOCK_DGRAM, 0);
			setsockopt(iface_fd[i],SOL_SOCKET,SO_REUSEADDR,(char *)&one,sizeof(one));
			if (bind(iface_fd[i], &(iface[i].sa), sizeof(struct sockaddr_in)) ==-1){
				perror("bind");
			}
			if (raidard_mode & MODE_BROADCAST){
				if (setsockopt(iface_fd[i], SOL_SOCKET, SO_BROADCAST,
						(char *)&one, sizeof(one))) {
					perror("do_proxy: setsockopt SO_BROADCAST");
					close(fd_out);
					return(-1);
				}
			}
			else if (raidard_mode & MODE_MULTICAST) {
				if (setsockopt(iface_fd[i], IPPROTO_IP, IP_MULTICAST_TTL,
						(char *)&mcast_ttl, sizeof(mcast_ttl))) {
					perror("do_proxy: setsockopt IP_MULTICAST_TTL");
					close(fd_out);
					return(-1);
				}

				/*
				 * Join the multicast group for replies
				 */
				mreq.imr_multiaddr.s_addr = mcast_addr_bin.s_addr;
				mreq.imr_interface.s_addr = sock->sin_addr.s_addr;

				if (setsockopt(fd_out, IPPROTO_IP, IP_ADD_MEMBERSHIP,
						(void *)&mreq, sizeof(mreq)) < 0) {
					perror("main: setsockopt IP_ADD_MEMBERSHIP");
					close(fd_out);
					return(-1);
				}

				if (setsockopt(iface_fd[i], IPPROTO_IP, IP_ADD_MEMBERSHIP,
						(void *)&mreq, sizeof(mreq)) < 0) {
					perror("main: setsockopt IP_ADD_MEMBERSHIP 2");
					close(fd_out);
					return(-1);
				}
			}
			break;
		case AF_INET6:
			sock6 = (struct sockaddr_in6 *)&(iface[i].sa);
			sock6->sin6_port = name.sin_port;
			iface_fd[i] = socket(AF_INET6, SOCK_DGRAM, 0);
			setsockopt(iface_fd[i], SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
			if (bind(iface_fd[i], &(iface[i].sa), sizeof(struct sockaddr_in6)) ==-1){
				perror("bind6:");
			}
			///* Specify the multicast group
			if (inet_pton(AF_INET6, ADDR_MULTICAST6, &mreq6.ipv6mr_multiaddr)){
				perror("inet_pton MULTICAST6:");
				close(fd_out);
				return(-1);
			}
			///* Accept multicast from any interface
			mreq6.ipv6mr_interface = 0;
			///* Join the multicast address
			if (setsockopt(iface_fd[i], IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
					&mreq6, sizeof(mreq6)) != 0) {
				perror("setsockopt() for Multicast6 failed");
			}
			break;
		default:
			break;
		}

		if (iface_fd[i] < 0) {
			fprintf(stderr, "Can't open broadcast socket\n");
			close(fd_out);
			return(1);
		}
	}
interface_open_fd_end:
	iface_fd[iface_count++] = fd_out;
	return(0);
}



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

int interfaces_get()
{
        int i;
        char ip[INET6_ADDRSTRLEN] = {0};
        extern int raidard_verbose;
        extern int systemd;
	extern unsigned short raidard_mode;

        if(raidard_mode&MODE_UNICAST) {
                iface_count=0;
                return(iface_count);
        }

        iface_count = get_interfaces(iface);

        if (iface_count <= 0) return(iface_count);

        if(raidard_verbose || systemd) {
		printf("got %d interfaces:\n", iface_count);
		for (i=0;i< iface_count;i++) {
			printf("IP=%s ", get_ip_str(&(iface[i].sa), ip));
			printf("Mac=%s ", iface[i].mac_addr);
			printf("BroadAddress=%s\n", get_ip_str(&(iface[i].broadaddr), ip));
		}
		fflush(stdout);
        }

	return(iface_count);
}



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
                 const struct sockaddr *to, socklen_t tolen)
{
        int top;
        int i;

        extern int raidard_verbose;
        extern unsigned short raidard_mode;

        /* Don't write to the last file descriptor. This file descripor
         * is not associated with an iterface. Rather it is a template
         * file descriptor, typically  bound to INADDR_ANY
         */
        if(raidard_mode&MODE_UNICAST)
                top=iface_count;
        else
                top=iface_count-1;
	switch(to->sa_family) {
        case AF_INET:
                 inet_pton(AF_INET, ADDR_BROADCAST, &(((struct sockaddr_in*)to)->sin_addr));
                 break;
        case AF_INET6:
                inet_pton(AF_INET6, ADDR_MULTICAST6, &(((struct sockaddr_in6*)to)->sin6_addr));
                break;
        }

        for(i=0;i<top;i++){
                if(iface_fd[i]<0){
                        continue;
                }

                if(raidard_verbose > 3)
                        printf("interface_sendto_all %d\n", i);
                if(sendto(iface_fd[i], msg, len, flags, to, tolen)<=0){
                        /* Ignore EINVAL for now. It seems to
                         * crop up if a packet is sent to an
                         * interface that can't deal with it because
                         * of routing or interface options */
                        if(errno == EINVAL)
                                break;
                        perror("Sendto: sendto");
                        return(-1);
                }
       /* 	if(sendto(iface_fd[i], msg, len, flags, &(iface[i].broadaddr), sizeof(struct sockaddr_in))<=0){
			if(errno == EINVAL)
                        break;
                        perror("Sendto: sendto");
                        return(-1);
                }*/
	}

        return(1);
}


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

int interface_add_to_read_template(fd_set *read_template)
{
        int i;
        int open=0;

        for(i=0;i<iface_count;i++){
                if(iface_fd[i]<0)
                        continue;
                open++;
                FD_SET(iface_fd[i], read_template);
        }

        return(open);
}


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

static int __interface_next_fd_marker=0;

void interface_set_fd_start(void)
{
        __interface_next_fd_marker=0;
}


/**********************************************************************
 * interface_set_fd_next
 * Find the next connection whose associated tcp socket is in the
 * read_tepmlate
 * pre: read_template: read_template to reference
 * post: internal status of what connection we are up to is updated.
 * return: Next connection whose TCP socket is set in the read_template
 *         -1 if no connection is found
 **********************************************************************/

int interface_set_fd_next(fd_set *read_template)
{
        int i;
        for(i=__interface_next_fd_marker;i<iface_count;i++){
                if(iface_fd[i]>=0 &&
                                (FD_ISSET(iface_fd[i], read_template))){
                        __interface_next_fd_marker=i+1;
                        return(iface_fd[i]);
                }
        }

        return(-1);
}


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

void interface_set_fd_end(void){ ; }
