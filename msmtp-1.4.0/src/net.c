/*
 * net.c
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#ifdef __MINGW32__
#include <winsock2.h>
#ifdef HAVE_GETADDRINFO
#include <ws2tcpip.h>
#endif
#elif defined (DJGPP)
#include <unistd.h>
#include <tcp.h>
#include <netdb.h>
#include <errno.h>
extern int errno;
#else /* UNIX */
#include <unistd.h>
#include <fcntl.h>
#ifndef HAVE_GETADDRINFO
#include <netinet/in.h>
extern int h_errno;
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
extern int errno;
#endif /* UNIX */

#include "net.h"


/*
 * [Windows only] wsa_strerror() 
 *
 * This function translates WSA error codes to strings.
 * It should translate all codes that could be caused by the Windows socket 
 * functions used in this file:
 * WSAStartup, getaddrinfo() or gethostbyname(), socket(), connect(), 
 * recv(), send()
 */

#ifdef __MINGW32__
const char *wsa_strerror(int error_code)
{
    switch (error_code)
    {
	case WSA_NOT_ENOUGH_MEMORY:
	    return "not enough memory";
	    
	case WSAEINVAL:
	    return "invalid argument";
	    
	case WSATYPE_NOT_FOUND:
    	    return "class type not found";
	    
	case WSAENETDOWN:
    	    return "the network subsystem has failed";
	    
	case WSAHOST_NOT_FOUND:
    	    return "host not found (authoritative)";
	    
	case WSATRY_AGAIN:
    	    return "host not found (nonauthoritative) or server failure";
	    
	case WSANO_RECOVERY:
    	    return "nonrecoverable error";
	    
	case WSANO_DATA:
    	    return "valid name, but no data record of requested type";

	case WSAEAFNOSUPPORT:
	    return "address family not supported";
	    
	case WSAEMFILE:
	    return "no socket descriptors available";

	case WSAENOBUFS:
	    return "no buffer space available";

	case WSAEPROTONOSUPPORT:
	    return "protocol not supported";

	case WSAEPROTOTYPE:
	    return "wrong protocol type for this socket";

	case WSAESOCKTNOSUPPORT:
	    return "socket type is not supported in this address family";
	    
	case WSAEADDRNOTAVAIL:
	    return "remote address is not valid";

	case WSAECONNREFUSED:
	    return "connection refused";

	case WSAENETUNREACH:
	    return "network unreachable";

	case WSAETIMEDOUT:
	    return "timeout";
	    
	case WSAENOTCONN:
	    return "socket not connected";

	case WSAESHUTDOWN:
	    return "the socket was shut down";

	case WSAEHOSTUNREACH:
	    return "host unreachable";

	case WSAECONNRESET:
	    return "connection reset by peer";
	    
	case WSASYSNOTREADY:
    	    return "the underlying network subsystem is not ready";
	    
	case WSAVERNOTSUPPORTED:
    	    return "the requested version is not available";
	    
	case WSAEINPROGRESS:
    	    return "a blocking operation is in progress";
	    
	case WSAEPROCLIM:
    	    return "limit on the number of tasks has been reached";
	    
	case WSAEFAULT:
    	    return "invalid request";

	default:
    	    return "unknown error";
    }
}
#endif /* __MINGW32__ */


/*
 * net_lib_init()
 *
 * see net.h
 */

int net_lib_init(char *errstr)
{
#ifdef __MINGW32__
   
    WORD wVersionRequested;
    WSADATA wsaData;
    int error_code;
    
    wVersionRequested = MAKEWORD(2, 0);
    if ((error_code = WSAStartup(wVersionRequested, &wsaData)) != 0)
    {
	snprintf(errstr, errstr_bufsize, "%s", wsa_strerror(error_code));
	return NET_ELIBFAILED;
    }
    else
    {
	return NET_EOK;
    }
    
#else /* noone else needs this... */
    
    return NET_EOK;
    
#endif
}


/*
 * net_close_socket()
 *
 * [This function is needed because Windows cannot just close() a socket].
 *
 * see net.h
 */

void net_close_socket(int fd)
{
#ifdef __MINGW32__
    (void)closesocket(fd);
#else
    (void)close(fd);
#endif
}


/*
 * net_connect()
 *
 * connect() with timeout.
 *
 * This function is equivalent to connect(), except that a connection attempt
 * times out after 'timeout' seconds instead of the OS dependant default value.
 * A 'timeout' <= 0 will be ignored.
 */

int net_connect(int fd, const struct sockaddr *serv_addr, socklen_t addrlen,
	int timeout)
{
#ifdef __MINGW32__
    /* TODO: I don't know how to do this on Win32. Please send a patch. */
    return connect(fd, serv_addr, addrlen);
#elif defined DJGPP
    /* TODO: I don't know how to do this with Watt32. Please send a patch. */
    return connect(fd, serv_addr, addrlen);
#else /* UNIX */
    
    int flags;
    struct timeval tv;
    fd_set rset;
    fd_set wset;
    int err;
    socklen_t optlen;
    
    if (timeout <= 0)
    {
	return connect(fd, serv_addr, addrlen);
    }
    else
    {
	/* make socket non-blocking */
	flags = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
	    return -1;
	}
	
	/* start connect */
	if (connect(fd, serv_addr, addrlen) < 0)
	{
	    if (errno != EINPROGRESS)
	    {
		return -1;
	    }
	    
    	    tv.tv_sec = timeout;
	    tv.tv_usec = 0;
	    FD_ZERO(&rset);
	    FD_ZERO(&wset);
	    FD_SET(fd, &rset);
	    FD_SET(fd, &wset);
	    
    	    /* wait for connect() to finish */
	    if ((err = select(fd + 1, &rset, &wset, NULL, &tv)) <= 0)
	    {
		/* errno is already set if err < 0 */
		if (err == 0)
		{
		    errno = ETIMEDOUT;
		}
		return -1;
	    }
	    
	    /* test for success, set errno */
	    optlen = sizeof(int);
	    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &optlen) < 0)
	    {
		return -1;
	    }
	    if (err != 0)
	    {
		errno = err;
		return -1;
	    }		
	}

	/* restore blocking mode */
	if (fcntl(fd, F_SETFL, flags) == -1)
	{
	    return -1;
	}
	
	return 0;
    }
#endif /* UNIX */
}


/*
 * open_socket() 
 *
 * see net.h
 */

int net_open_socket(const char *hostname, int port, int timeout, int *ret_fd, 
	char *errstr)
{    
#ifdef HAVE_GETADDRINFO
    
    int fd;
    char port_string[11];
    struct addrinfo hints;
    struct addrinfo *res0;
    struct addrinfo *res;
    int error_code;
    int cause;
    
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    if (snprintf(port_string, 11, "%d", port) >= 11)
    {
	/* cannot happen unless port numbers have more than 32 bit */
	snprintf(errstr, errstr_bufsize, 
		"Port number %d is too high. "
		"Increase the length of port_string", port);
	return NET_EHOSTNOTFOUND;
    }
    error_code = getaddrinfo(hostname, port_string, &hints, &res0);
    if (error_code)
    {
	snprintf(errstr, errstr_bufsize, "cannot locate host %s: %s",
		hostname,
#ifdef __MINGW32__
		wsa_strerror(error_code)
#else
		gai_strerror(error_code)
#endif
		);
	return NET_EHOSTNOTFOUND;
    }

    fd = -1;
    cause = 0;
    for (res = res0; res; res = res->ai_next)
    {
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd < 0)
	{
	    cause = 1;
	    continue;
	}
	if (net_connect(fd, res->ai_addr, res->ai_addrlen, timeout) < 0)
	{
	    cause = 2;
	    net_close_socket(fd);
	    fd = -1;
	    continue;
	}
	break;
    }
    freeaddrinfo(res0);
    if (fd < 0)
    {
	if (cause == 1)
	{
	    snprintf(errstr, errstr_bufsize, "cannot create socket: %s", 
#ifdef __MINGW32__
		    wsa_strerror(WSAGetLastError())
#else
		    strerror(errno)
#endif
		    );
	    return NET_ESOCKET; 
	}
	else if (cause == 2)
	{
	    snprintf(errstr, errstr_bufsize, 
		    "cannot connect to %s, port %d: %s", hostname, port,
#ifdef __MINGW32__
		    wsa_strerror(WSAGetLastError())
#else
		    strerror(errno)
#endif
		    );
	    return NET_ECONNECT;
	}
	else /* cause == 0, can't happen */
	{
	    snprintf(errstr, errstr_bufsize, "cannot locate host %s: "
		    "getaddrinfo() returned crap", hostname);
	    return NET_EHOSTNOTFOUND;
	}
    }
    
    *ret_fd = fd;
    return NET_EOK;

#else /* !HAVE_GETADDRINFO */

    int fd;
    struct sockaddr_in sock;
    struct hostent *remote_host;
    
    if (!(remote_host = gethostbyname(hostname)))
    {
	snprintf(errstr, errstr_bufsize, "cannot locate host %s: %s", hostname,
#ifdef __MINGW32__
		wsa_strerror(WSAGetLastError())
#else
		hstrerror(h_errno)
#endif
		);
	return NET_EHOSTNOTFOUND;
    }

    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
	snprintf(errstr, errstr_bufsize, "cannot create socket: %s", 
#ifdef __MINGW32__
		wsa_strerror(WSAGetLastError())
#else
		strerror(errno)
#endif
		);
	return NET_ESOCKET;
    }
    
    sock.sin_family = AF_INET;
    sock.sin_port = htons((unsigned short int)port);
    memcpy(&sock.sin_addr, remote_host->h_addr_list[0], 
	    (size_t)remote_host->h_length);

    if (net_connect(fd, (struct sockaddr *)(&sock), sizeof(sock), timeout) < 0)
    {
	snprintf(errstr, errstr_bufsize, "cannot connect to %s, port %d: %s",
		hostname, port,
#ifdef __MINGW32__
		wsa_strerror(WSAGetLastError())
#else
		strerror(errno)
#endif
		);
	return NET_ECONNECT;
    }

    *ret_fd = fd;
    return NET_EOK;

#endif /* HAVE_GETADDRINFO */
}


/*
 * net_readbuf_init()
 *
 * see net.h
 */

void net_readbuf_init(net_readbuf_t *readbuf)
{
    readbuf->count = 0;
}


/*
 * net_readbuf_read()
 *
 * Wraps read() to provide buffering for net_gets().
 */

ssize_t net_readbuf_read(int fd, net_readbuf_t *readbuf, char *ptr, 
	char *errstr)
{
    if (readbuf->count <= 0)
    {
#ifdef __MINGW32__
	readbuf->count = recv(fd, readbuf->buf, sizeof(readbuf->buf), 0);
#else /* ! __MINGW32__ */
	do
	{
	    readbuf->count = recv(fd, readbuf->buf, sizeof(readbuf->buf), 0);
	}
	while (readbuf->count < 0 && errno == EINTR);
#endif /* ! __MINGW32__ */
	if (readbuf->count < 0)
	{
	    snprintf(errstr, errstr_bufsize, "network read error: %s",
#ifdef __MINGW32__
		wsa_strerror(WSAGetLastError())
#else /* ! __MINGW32__ */
		strerror(errno)
#endif /* ! __MINGW32__ */
		);
	    return -1;
	}
	else if (readbuf->count == 0)
	{
	    return 0;
	}
	readbuf->ptr = readbuf->buf;
    }
    readbuf->count--;
    *ptr = *((readbuf->ptr)++);
    return 1;
}


/*
 * net_gets()
 *
 * see net.h
 */

int net_gets(int fd, net_readbuf_t *readbuf, 
	char *str, size_t size, size_t *len, char *errstr)
{
    char c;
    size_t i;
    ssize_t ret;

    i = 0;
    while (i + 1 < size)
    {
	if ((ret = net_readbuf_read(fd, readbuf, &c, errstr)) == 1)
	{
	    str[i++] = c;
	    if (c == '\n')
	    {
		break;
	    }
	}
	else if (ret == 0)
	{
	    break;
	}
	else
	{
	    return NET_EIO;
	}
    }
    str[i] = '\0';
    *len = i;
    return NET_EOK;
}


/*
 * net_puts()
 *
 * see net.h
 */

int net_puts(int fd, const char *s, size_t len, char *errstr)
{
#ifdef __MINGW32__
    int ret;
#else
    ssize_t ret;
#endif

#ifdef __MINGW32__
    ret = send(fd, s, len, 0);
#else /* ! __MINGW32__ */
    do
    {
	ret = send(fd, s, len, 0);
    }
    while (ret < 0 && errno == EINTR);
#endif /* ! __MINGW32__ */
    if (ret < 0)
    {
	snprintf(errstr, errstr_bufsize, "network write error: %s", 
#ifdef __MINGW32__
		wsa_strerror(WSAGetLastError())
#else
		strerror(errno)
#endif
		);
	return NET_EIO;
    }
    else if ((size_t)ret == len)
    {
	return NET_EOK;
    }
    else /* 0 <= error_code < len */
    {
	snprintf(errstr, errstr_bufsize, "network write error");
	return NET_EIO;
    }
}


/*
 * net_lib_deinit()
 *
 * see net.h
 */

void net_lib_deinit(void)
{
#ifdef __MINGW32__
    (void)WSACleanup();
#endif
}
