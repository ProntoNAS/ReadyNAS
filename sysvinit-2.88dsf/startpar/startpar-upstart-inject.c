/* Copyright 2011 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ****************************************************************
 */

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "startpar.h"

int main(int argc, char **argv)
{
	int fd, len;
	ssize_t written = 0;
	struct sockaddr_un saddr;
	socklen_t addrlen;
	char *buf;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s jobname instance upstart_events\n",
		        argv[0]);
		exit(1);
	}

	fd = socket(AF_LOCAL, SOCK_SEQPACKET|SOCK_NONBLOCK, 0);
	if (!fd)
		exit(1);

	saddr.sun_family = AF_LOCAL;
	strcpy(saddr.sun_path, UPSTART_SOCKET_PATH);
	addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(saddr.sun_path);
	/* translate leading '@' to abstract namespace */
	if (saddr.sun_path[0] == '@')
		saddr.sun_path[0] = '\0';

	/* If the socket isn't there yet, we can assume startpar hasn't
	 * started, and will catch this job itself once it does. */
	if (connect(fd, (struct sockaddr *)&saddr, addrlen) < 0)
		exit(0);

	/* FIXME: should be able to unambiguously parse the arguments instead
	 * of relying on whitespace. */
	if (!strcmp(argv[3],"started") || !strcmp(argv[3],"stopped"))
	{
		len = asprintf(&buf, "%s %s %s",argv[3],argv[1],argv[2]);
		buf[strlen(argv[3])] = '\0';
		buf[strlen(argv[3])+strlen(argv[1])+1] = '\0';
	}
	else
		/* Unhandled upstart event. */
		exit(0);
	if (len < 1)
		exit(1);
	do {
		ssize_t s = send(fd, buf+written, len-written, MSG_NOSIGNAL);
		if (s < 0 && errno != EINTR)
		{
			close(fd);
			exit(1);
		}
		if (s > 0)
			written += s;
	} while (written < len);
	close(fd);
	exit(0);
}
