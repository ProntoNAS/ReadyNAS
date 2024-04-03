/*
 * net.h
 *
 * This file is part of msmtp, an SMTP client.
 *
 * Copyright (C) 2000, 2003, 2004, 2005
 * Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   msmtp is released under the GPL with the additional exemption that
 *   compiling, linking, and/or using OpenSSL is allowed.
 */

#ifndef NET_H
#define NET_H


/* All 'errstr' arguments must point to buffers that are at least
 * 'errstr_bufsize' characters long.
 * If a function with an 'errstr' argument returns a value != NET_EOK,
 * 'errstr' will contain an error message (maybe just "").
 * If such a function returns NET_EOK, 'errstr' will not be changed.
 */
extern const size_t errstr_bufsize;
#define NET_EOK			0	/* no error */
#define NET_ELIBFAILED		1	/* The underlying library failed */
#define NET_EHOSTNOTFOUND	2	/* Host not found */
#define NET_ESOCKET		3	/* Cannot create socket */
#define NET_ECONNECT		4	/* Cannot connect */
#define NET_EIO			5	/* Input/output error */

/* This structure is used as a read buffer for net_gets. Do not access it
 * directly. Use net_readbuf_init() to initialize it. */
typedef struct
{
    ssize_t count;
    char *ptr;
    char buf[4096];
} net_readbuf_t;

/*
 * net_lib_init()
 *
 * Initializes the networking libraries.
 * Used error codes: NET_ELIBFAILED
 */
int net_lib_init(char *errstr);

/*
 * net_open_socket() 
 *
 * Opens a TCP socket to 'hostname':'port'.
 * 'hostname' may be a host name or an IPv4 address in dot notation.
 * If HAVE_GETADDRINFO is defined, it may also be an an IPv6 address in colon
 * notation. 'timeout' is measured in secondes. If it is <= 0, no timeout will
 * be set, which means that the OS dependent default timeout value will be used.
 * The file descriptor is returned in 'fd'.
 *
 * Used error codes: NET_EHOSTNOTFOUND, NET_ESOCKET, NET_ECONNECT
 */
int net_open_socket(const char *hostname, int port, int timeout, int *fd, 
	char *errstr);

/*
 * net_readbuf_init()
 *
 * Initialize a net_readbuf_t for first use.
 */
void net_readbuf_init(net_readbuf_t *readbuf);

/*
 * net_gets()
 * 
 * Reads in at most one less than 'size' characters from 'fd' and stores them
 * into the buffer pointed to by 'str'. Reading stops after an EOF or a newline.
 * If a newline is read, it is stored into the buffer. A '\0' is stored after 
 * the last character in the buffer. The length of the resulting string (the
 * number of characters excluding the terminating '\0') will be stored in 'len'.
 * 'readbuf' will be used as an input buffer and must of course be the same for
 * all read operations on 'fd'.
 * Used error codes: NET_EIO
 */
int net_gets(int fd, net_readbuf_t *readbuf,
	char *str, size_t size, size_t *len, char *errstr);

/*
 * net_puts()
 *
 * Writes 'len' characters from the string 's' to 'fd'.
 * Used error codes: NET_EIO
 */
int net_puts(int fd, const char *s, size_t len, char *errstr);

/*
 * net_close_socket()
 *
 * Closes a socket.
 */
void net_close_socket(int fd);

/*
 * net_lib_deinit()
 *
 * Deinit networking library
 */
void net_lib_deinit(void);

#endif
