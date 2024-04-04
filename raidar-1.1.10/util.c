/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * util.c                                                     July 2001
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


#include "raidard.h"

/****************************************************************************
open a socket of the specified type, port and address for incoming data
****************************************************************************/
int open_socket_in_addr(int type, const struct in_addr *addr, int port)
{
	struct sockaddr_in sock;
	int res;
	int one = 1;

	if (!addr)
		return -1;

	memset((char *)&sock,'\0',sizeof(sock));

#ifdef HAVE_SOCK_SIN_LEN
	sock.sin_len = sizeof(sock);
#endif
	sock.sin_port = htons(port);
	sock.sin_family = AF_INET;
	sock.sin_addr = *addr;
	res = socket(AF_INET, type, 0);
	if (res == -1) {
		perror("socket");
		return -1;
	}

	if (setsockopt(res, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one))) {
		perror("setsockopt(SO_REUSEADDR)");
	}

	if (bind(res, (struct sockaddr * )&sock, sizeof(sock)) == -1) {
		perror("bind");
		close(res);
		return -1;
	}

	return res;
}



/****************************************************************************
  create an outgoing socket. 
  **************************************************************************/
int open_socket_out_addr(int type, const struct in_addr *addr, int port, 
                int broadcast)
{
	struct sockaddr_in sock_out;
	int res;
	int one=1;

	res = socket(PF_INET, type, 0);
	if (res == -1) {
		perror("socket");
		return -1;
	}

	memset((char *)&sock_out,'\0',sizeof(sock_out));
	sock_out.sin_addr = *addr;
	sock_out.sin_port = htons(port);
	sock_out.sin_family = AF_INET;

        if(broadcast==SO_BROADCAST){
	  setsockopt(res,SOL_SOCKET,SO_BROADCAST,(char *)&one,sizeof(one));
        }

	if (connect(res,(struct sockaddr *)&sock_out,sizeof(sock_out)) != 0) {
                close(res);
		perror("connect");
		return -1;
	}

	return res;
}

/****************************************************************************
  create an outgoing socket. 
  **************************************************************************/
int open_socket_out(int type, const char *dst, int port, int broadcast)
{
	struct in_addr addr;

	if (inet_pton(AF_INET, dst, &addr) <= 0) {
		perror(dst);
	}

	return(open_socket_out_addr(type, &addr, port, broadcast));
}

size_t write_all(int fd, void *buf, size_t size)
{
        ssize_t n=0;

	while (size) {
		n = write(fd, buf, size);
		if (n <= 0) {
			perror("write");
			return(n);
		}
		size -= n;
		buf += n;
	}

        return(n);
}
