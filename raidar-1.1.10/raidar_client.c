/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * iproxy.c                                                   July 2001
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

#define _USE_BSD
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <libgen.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

#include <zlib.h>

#include "util.h"
#include "raidard.h"
#include "packet.h"
#include "interface.h"
#include "connection.h"

#define KEEPALIVE_TIME (time_t)1

static uint16 iproxy_port = RAIDARD_PORT;
static uint32 iproxy_cid = 0;
uint32 mcast_ttl = TTL_MULTICAST;
static char *xcast_ip = ADDR_BROADCAST;
uint32 raidard_mode = MODE_BROADCAST | MODE_CLIENT;
int raidard_verbose;
int list_only = 0;
int systemd = 0;

static struct connection con;

static void trim(char *s)
{
	int len = strlen(s);
	while (isspace(s[--len]))
		s[len] = '\0';
}

static int show_sys_info(const char *buf, char **hosts)
{
	char mac[18];
	char hn[32];
	static char last_hn[32];
	char ip[32];
	static char last_ip[32];
	char info[8192];
	char status[96];
	char model[32] = "UNKNOWN";
	char serial[32] = "";
	char flags[64] = "";
	char *p;
	int n;
	int i;
	int ret;

	ret = sscanf(buf, "%17[^\t]\t%31[^\t]\t%31[^\t]\t%8191[^\t]\t%95[^\t]\t",
		     mac, hn, ip, info, status);
	if (ret != 5)
		return -1;

	/* Filter for specified hosts */
	if (hosts && hosts[0])
	{
		for (i = 0; hosts[i]; i++)
		{
			if (strcasecmp(hn, hosts[i]) == 0)
				break;
		}
		if (!hosts[i])
			return 0;
	}

	/* Skip duplicates */
	if (!strcmp(last_hn, hn) && !strcmp(last_ip, ip))
		return 1;
	strcpy(last_hn, hn);
	strcpy(last_ip, ip);

	if (list_only)
	{
		printf("%-32.32s %s\n", hn, ip);
		return 0;
	}

	trim(status);
	trim(info);

	p = strstr(info, "model!!0!!");
	if (p)
	{
		char *begin, *end;
		begin = strstr(p, "descr=");
		if (begin)
		{
			begin += 6;
			end = strstr(begin, "::");
			if (end)
				snprintf(model, sizeof(model), "%.*s",
					 (int)(end - begin), begin);
		}
		begin = strstr(p, "sn=");
		if (begin)
		{
			begin += 3;
			end = strstr(begin, "::");
			if (end)
				snprintf(serial, sizeof(serial), "\tSerial: %.*s\n",
					 (int)(end - begin), begin);
		}
		begin = strstr(p, "flags=");
		if (begin)
		{
			unsigned long val;
			begin += 6;
			val = strtoul(begin, NULL, 16);
			snprintf(flags, sizeof(flags), "\tFlags: 0x%04lX\n", val);
		}
	}

	if ((p = strstr(status, "!!version=")))
	{
		p += 10;
		memmove(status, p, sizeof(status) + status - p);
		p = strchr(status, ',');
		if (p)
			*p = '\0';
	}
	else if ((p = strstr(status, "fw=")))
	{
		p += 3;
		memmove(status, p, sizeof(status) + status - p);
	}

	printf("================================\n"
	       "%s\n"
	       "================================\n"
	       "\tModel: %s\n"
	       "%s"
	       "%s"
	       "\tMAC: %s\n"
	       "\tIP: %s\n"
	       "\tStatus: %s\n",
	       hn, model, serial, flags, mac, ip, status);

	/* volume!!1!!status=ok::descr=data: RAID 5, 1.2TB (97%) of 1.3TB used */
	n = 0;
	p = info;
	while ((p = strstr(p, "volume!!")))
	{
		char *vol;
		const char *state = ")";
		p += 8;
		if (!n)
			printf("\tVolumes:\n");
		n++;
		printf("\t\t%d", atoi(p));
		p = strstr(p, "status=");
		if (p)
		{
			if (strncmp(p+7, "dead", 4) == 0)
				state = "*)";
			else if (strncmp(p+7, "resync", 4) == 0)
				state = ">";
		}
		printf("%s ", state);
		p = strstr(p, "descr=");
		if (!p)
			break;
		vol = p + 6;
		p = strchr(vol, '\n');
		if (!p)
			break;
		*p = '\0';
		p++;
		printf("%s\n", vol);
		p[-1] = '\n';
	}

	/* disk!!3!!status=ok::descr=Channel 3: TOSHIBA_MK1002TSKB 1TB */
	n = 0;
	p = info;
	while ((p = strstr(p, "disk!!")))
	{
		char *disk;
		const char *state = ")";
		p += 6;
		if (!n)
			printf("\tDisks:\n");
		n++;
		printf("\t\t%d", atoi(p));
		p = strstr(p, "status=");
		if (!p)
			break;
		if (strncmp(p+7, "dead", 4) == 0)
			state = "*)";
		else if (strncmp(p+7, "resync", 4) == 0)
			state = ">";
		printf("%s ", state);
		p = strstr(p, "descr=");
		if (!p)
			break;
		disk = p + 6;
		p = strchr(disk, '\n');
		if (!p)
			break;
		*p = '\0';
		p++;
		printf("%s\n", disk);
		p[-1] = '\n';
	}

	return 0;
}

static int
_uncompress(unsigned char *in, size_t inlen, unsigned char *out, const size_t outlen)
{
	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return 1;
	strm.avail_in = inlen;
	strm.next_in = in;
	strm.avail_out = outlen;
	strm.next_out = out;

	ret = inflate(&strm, Z_FINISH);

	/* clean up and return */
	inflateEnd(&strm);
	return (ret != Z_STREAM_END);
}

static void do_search(char **hosts)
{
	ssize_t n;
	int fd_in = 0;
	fd_set read_template;
	struct in_addr addr;
	struct packet packet;
	struct sockaddr_in iaddr;
	int count = 0;

	iproxy_cid = getpid();         /* Bad Horms */

	/* We can just pass the mcast_ip as this
	** argument isn't used unless we are in multicast mode
	**/
	if (interface_open_fd(ADDR_ANY, xcast_ip, 0))
	{
		fprintf(stderr, "do_search: interface_open_fd\n");
		exit(-1);
	}

	if (inet_pton(AF_INET, xcast_ip, &addr) <= 0)
	{
		perror("inet_pton");
		exit(-1);
	}

	bzero(&iaddr, sizeof(iaddr));
	iaddr.sin_port = htons(iproxy_port);
	iaddr.sin_family = AF_INET;
	iaddr.sin_addr = addr;

	connection_initialise(&con, fd_in, &iaddr, &addr, iproxy_cid,
			      RAIDARD_LIST_SID);

	/* this initial packet is used to prime the connection */
	packet_send_primer(&iaddr, iproxy_cid, RAIDARD_LIST_SID);

	while (1)
	{
		struct timeval tv;
		int fd;

		FD_ZERO(&read_template);
		if (!interface_add_to_read_template(&read_template))
			break;

		tv.tv_sec = KEEPALIVE_TIME;
		tv.tv_usec = 0;

		n = select(FD_SETSIZE, &read_template, NULL, NULL, &tv);

		if (n == 0)
		{
			/* timeout - quit */
			break;
		}

		if (n < 0)
		{
			if (errno != EINTR)
			{
				perror("do_search: select");
				exit(-1);
			}
			continue; /* Ignore EINTR */
		}
		else if (FD_ISSET(fd_in, &read_template))
		{
			if (connection_read_tcp(&con))
				exit(0);
		}

		interface_set_fd_start();
		while ((fd = interface_set_fd_next(&read_template)) >= 0)
		{
			bzero(&packet, sizeof(packet));
			if ((n = read(fd, &packet, sizeof(packet))) <= 0)
			{
				break;
			}
			if (raidard_verbose > 0)
			{
				printf("received %d byte reply\n", (int)n);
			}

			if (packet.version != htonl(RAIDARD_PROTO_VERSION))
			{
				printf("packet for unknown raidar version\n");
				continue;
			}

			if (ntohl(packet.length) != n ||
			    ntohl(packet.length) < PACKET_HDR_LEN)
			{
				printf("length missmatch\n");
				continue;
			}

			if (ntohl(packet.cid) != iproxy_cid)
			{
				printf("cid missmatch %d!=%d\n",
				       ntohl(packet.cid),
				       iproxy_cid);
				continue;
			}

			if (ntohl(packet.sid) == RAIDARD_LIST_SID)
			{
				int ret;
				char *buf;
				if (ntohl(packet.length) == PACKET_HDR_LEN)
					continue;
				if (ntohl(packet.flag) & RAIDAR_FLAG_COMPRESS_RESP)
				{
					buf = malloc(16384);
					if (!buf)
						continue;
					ret = _uncompress((unsigned char *)packet.buf,
							ntohl(packet.length) - PACKET_HDR_LEN,
							(unsigned char *)buf, 16384);
					if (ret != 0)
					{
						free(buf);
						continue;
					}
				}
				else
					buf = packet.buf;
				ret = show_sys_info(buf, hosts);
				if (ret)
				{
					if (ret < 0)
						printf("error parsing response\n");
				}
				if (raidard_verbose > 1)
				{
					printf("----------\n%s\n----------\n", buf);
					fflush(stdout);
				}
				if (ntohl(packet.flag) & 0x40000000)
					free(buf);
				count++;
			}
		}
		interface_set_fd_end();
	}

	printf("\nFound %d total systems\n", count);

	exit(0);
}

static void usage(const char *name)
{
	printf("%s [options] [HOSTNAME ...]\n"
	       "    -h            help\n"
	       "    -l            list names and addresses only\n"
	       "    -v            increase verbosity\n", name);
	exit(0);
}

int main(int argc, char *argv[])
{
	int c;

	while ((c = getopt(argc, argv, "lvh")) != -1)
	{
		switch (c) {
		case 'l':
			list_only = 1;
			break;
		case 'v':
			raidard_verbose++;
			break;
		case 'h':
		default:
			usage(argv[0]);
			break;
		}
	}

	if (interfaces_get() < 0)
		exit(1);

	do_search(argv+optind);

	return 0;
}
