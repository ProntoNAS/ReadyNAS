/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * raidard.c is based on
 * iproxy-server.c                                            July 2001
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

#define _GNU_SOURCE

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/wait.h>

#include <zlib.h>

#include "util.h"
#include "raidard.h"
#include "packet.h"
#include "interfaces.h"
#include "interface.h"

static unsigned short raidard_port = RAIDARD_PORT;
static uint32 raidard_sid = RAIDARD_SID;
static unsigned short dest_port = WEB_PORT;
uint32 mcast_ttl = TTL_MULTICAST;
static char *xcast_ip = ADDR_BROADCAST;
unsigned short raidard_mode = MODE_BROADCAST | MODE_SERVER;
int raidard_verbose;
int systemd = 0;
int ifaceCount = 0;
int quitting;
int netlink_fd;
int status_fd = -1;

#define DISK_BUF_SZ 16384
static char disk_buf[DISK_BUF_SZ];
static time_t last_disk_update = 0;

#define PIDFILE "/var/run/raidard.pid"
#define BOOT_INFO "/proc/sys/dev/boot/info"
#define SELECT_TIMEOUT 600
#if defined(__arm__)
#define ARCH "arm"
#elif defined(__x86_64__) || defined (__i386__)
#define ARCH "x86"
#endif

struct sys_info {
	char model[24];
	char serial[16];
	char os_version[36];
	char hostname[32];
	char os_status[60];
	uint32_t flags;
	bool readynasd_online;
	time_t next_readynasd_check;
};
struct sys_info info;

static char *get_boot_info(const char *tag, char *buf, int size)
{
	FILE *boot_info;
	char *val = NULL;
	char str_buf[1024];
	char *string;
	char *word;
	char *saveptr;

	boot_info = fopen(BOOT_INFO, "r");
	if (!boot_info)
		return NULL;
	while (fgets(str_buf, sizeof(str_buf), boot_info) != NULL)
	{
		string = str_buf;
		word = strtok_r(string, ":\t\n", &saveptr);
		if (!word || strcmp(word, tag) != 0)
			continue;
		string = NULL;
		word = strtok_r(string, ":\t\n", &saveptr);
		if (!word)
			break;
		snprintf(buf, size, "%s", word);
		val = buf;
		break;
	}
	fclose(boot_info);

	return val;
}

static void get_model(char *buf, int size)
{
	char *p;

	p = get_boot_info("model", buf, size);
	if (!p)
		snprintf(buf, size, "%s", "None");
}

static void get_serial(char *buf, int size)
{
	char *p;

	p = get_boot_info("serial", buf, size);
	if (!p)
		snprintf(buf, size, "%s", "N0NE000200000");
}

static void get_os_version(char *buf, int sz)
{
	FILE *fp;

	buf[0] = '\0';
	fp = fopen("/etc/os_version", "r");
	if (fp)
	{
		char ver[32];
		if (fscanf(fp, "%*[^!]!!version=%31[^,]", ver) == 1)
			snprintf(buf, sz, "fw=%s", ver);
		fclose(fp);
	}
}

static void get_sys_info(void)
{
	get_model(info.model, sizeof(info.model));
	get_serial(info.serial, sizeof(info.serial));
	get_os_version(info.os_version, sizeof(info.os_version));
}

static void reread(int sig)
{
	extern int iface_fd[MAX_INTERFACES + 1];
	extern int iface_count;
	int i;
	for (i = 0; i < iface_count; i++)
	{
		if (iface_fd[i] < 0)
			continue;
		close(iface_fd[i]);
	}
	interfaces_get();
	interface_open_fd(ADDR_ANY, xcast_ip, raidard_port);
	get_sys_info();
	signal(sig, (void (*)(int))reread);
}

static int
_compress(unsigned char *in, size_t inlen, unsigned char *out, size_t *outlen, int level)
{
	int ret;
	z_stream strm;
	size_t len = *outlen;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return 1;
	strm.avail_in = inlen;
	strm.next_in = in;
	strm.avail_out = len;
	strm.next_out = out;
	ret = (deflate(&strm, Z_FINISH) != Z_STREAM_END);
	*outlen = len - strm.avail_out;
	deflateEnd(&strm);
	return ret;
}

static void
process_status_event(int fd, uint32_t *flags)
{
	char *buf;
	char key[64], val[64];
	ssize_t len;

	buf = malloc(DISK_BUF_SZ);
	if (!buf)
		return;
	len = read(fd, buf, DISK_BUF_SZ-1);
	buf[len-1] = '\n';
	buf[len] = '\0';

	if (raidard_verbose > 1)
		printf("received status update: %s\n", buf);

	if (strncmp(buf, "disk!!", 6) == 0 ||
	    strncmp(buf, "volume!!", 8) == 0)
	{
		strncpy(disk_buf, buf, DISK_BUF_SZ-1);
		disk_buf[DISK_BUF_SZ-1] = '\0';
		last_disk_update = time(NULL);
	}
	else if (sscanf(buf, "%63[^=]=%63s", key, val) == 2)
	{
		if (strcmp(key, "READYCLOUD_CONNECTIVITY") == 0)
		{
			if (atoi(val))
				*flags |= FLAG_ONLINE;
			else
				*flags &= ~FLAG_ONLINE;
		}
	}
	free(buf);
}

static void
process_netlink_event(int s)
{
#ifdef __linux__
	int len;
	char buf[4096];
	struct nlmsghdr *nlh;
	int changed = 0;

	nlh = (struct nlmsghdr*)buf;

	len = recv(s, nlh, sizeof(buf), 0);
	if (len <= 0)
		return;
	while ((NLMSG_OK(nlh, len)) && (nlh->nlmsg_type != NLMSG_DONE))
	{
		if (nlh->nlmsg_type == RTM_NEWADDR ||
		    nlh->nlmsg_type == RTM_DELADDR)
		{
			changed = 1;
		}
		nlh = NLMSG_NEXT(nlh, len);
	}
	if (changed)
		reread(0);
#endif
}

static int
disk_is_down(const char *disk, int len)
{
	FILE *fp = fopen("/run/spindown", "r");
	char buf[16];
	char key[16];
	int val = 0;
	
	if (!fp)
		return 0;
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (sscanf(buf, "%15[^:]:%d", key, &val) != 2)
			continue;
		if (strcmp(key, disk) == 0)
		{
			fclose(fp);
			return val;
		}
	}
	fclose(fp);

	return 0;
}

static int
get_disk_gb(const char *disk)
{
	FILE *fp;
	char path[PATH_MAX];
	char buf[21];
	long long sectors;
	int ret;

	snprintf(path, sizeof(path), "/sys/block/%s/size", disk);
	fp = fopen(path, "r");
	if (!fp)
		return -EINVAL;
	ret = fread(buf, 1, sizeof(buf), fp);
	fclose(fp);
	if (ret <= 0)
		return -EINVAL;

	/* According to IDEMA standards:
	**   The LBA count shall be calculated using the following formula:
	**   LBA count = (97696368) + (1953504 * (Desired Capacity in Gbytes - 50.0))
	** So we should calculate capacity using the reverse of that formula */
	/* Update: SFF-8447 describes a different method for high-capacity (> 8TB) disks */
	sectors = strtoll(buf, NULL, 0);
	if (sectors > 15628053168)
		ret = (((sectors * 512) / 1000000000000) * 1000);
	else
		ret = ((sectors - 97696368) / 1953504) + 50;

	return ret;
}

static int
filter(const struct dirent *d)
{
	int ctl, chan;
	char model[32];

	return(d->d_name[0] != '.' &&
	       sscanf(d->d_name, "%d:%d:%31[^:]", &ctl, &chan, model) == 3 &&
	       ctl == 0);
}

static int get_disk_info(char *buf, int size)
{
	struct dirent **namelist;
	struct dirent *de;
	int spindown = !access("/run/spindown", R_OK);
	int i, n;
	int off = 0;

	n = scandir("/dev/disk/internal", &namelist, filter, versionsort);
	if (n < 0)
		return 0;
	for (i = 0; i < n && off < size; i++)
	{
		int ctl, chan, gb, down;
		char model[32];
		char capacity[16];
		char path[256];
		char link[16];
		ssize_t len;

		de = namelist[i];
		if (sscanf(de->d_name, "%d:%d:%31[^:]", &ctl, &chan, model) != 3)
		{
			free(de);
			continue;
		}
		chan++;
		snprintf(path, sizeof(path), "/dev/disk/internal/%s", de->d_name);
		len = readlink(path, link, sizeof(link)-1);
		if (len < 9)
		{
			free(de);
			continue;
		}
		link[len] = '\0';
		memcpy(link+1, "/dev", 4);
		gb = get_disk_gb(link+6);
		if ((gb / 1000) > 0 && (gb % 1000) == 0)
			snprintf(capacity, sizeof(capacity), "%d%s", gb / 1000, "TB");
		else
			snprintf(capacity, sizeof(capacity), "%d%s", gb, "GB");
		down = spindown ? disk_is_down(link+1, len-1) : 0;

		off += snprintf(buf + off, size - off, "disk!!%d!!status=ok::descr=Channel %d: %s %s%s\n",
				chan, chan, model, capacity, down ? " [Sleeping]" : "");
		free(de);
	}
	if (off >= size)
		buf[size-2] = '\n';
	free(namelist);

	return off;
}

static bool systemd_is_active(const char *cmd, const char *unit)
{
	pid_t pid = fork();
	int ret = -1, status = 1;

	if (pid < 0)
		return false;
	if (pid)
		ret = waitpid(pid, &status, 0);
	else
		exit(!execl("/bin/systemctl", "/bin/systemctl", "-q", cmd, unit, NULL));
	if (ret < 0)
		return false;

	return !status;
}

static const char *build_raidar_response(struct sys_info *info, char *sysInfo, size_t len)
{
	FILE *fp;
	const char *boot = "";
	uint32_t flags = info->flags;
	char *p;
	char model[1024];
	int n;

	gethostname(info->hostname, sizeof(info->hostname));
	p = strchr(info->hostname, '.');
	if (p)
		*p = '\0';

	if ((fp = fopen("/.os_status", "r")) != NULL ||
	    (fp = fopen("/run/.os_status", "r")) != NULL)
	{
		if (fgets(info->os_status, sizeof(info->os_status), fp) != NULL)
		{
			if (strncmp(info->os_status, "Support:", 8) == 0 ||
			    strncmp(info->os_status, "TELNET", 6) == 0)
				flags |= FLAG_SUPPORT;
			boot = "\t1";
		}
		else
			snprintf(info->os_status, sizeof(info->os_status), "%21s", info->os_version);
		fclose(fp);
	}
	else
		snprintf(info->os_status, sizeof(info->os_status), "%21s", info->os_version);

	if (flags & FLAG_INITRD)
	{
		struct stat st;
		if (stat("/", &st) == 0 && st.st_dev > 2)
		{
			info->flags &= ~FLAG_INITRD;
			flags &= ~FLAG_INITRD;
		}
		else
		{
			/* Don't confuse people while we're in initrd. */
			flags &= ~FLAG_NEW;
			if (flags & FLAG_SUPPORT)
				flags &= ~FLAG_BOOTING;
		}
	}
	if ((flags & FLAG_BOOTING) && !(flags & FLAG_INITRD))
	{
		if (systemd_is_active("is-active", "graphical.target"))
		{
			info->flags &= ~FLAG_BOOTING;
			flags &= ~FLAG_BOOTING;
		}
	}
	if (!(flags & (FLAG_BOOTING | FLAG_INITRD | FLAG_SUPPORT)) &&
	    (!info->readynasd_online || time(NULL) >= info->next_readynasd_check))
	{
		info->readynasd_online = systemd_is_active("is-active", "readynasd");
		if (info->readynasd_online == false)
		{
			snprintf(info->os_status, sizeof(info->os_status), "Management service is offline");
			boot = "\t1";
		}
		info->next_readynasd_check = time(NULL) + 120;
	}

	if ((flags & FLAG_NEW) && access("/var/log/frontview/.managed", F_OK) == 0)
	{
		info->flags &= ~FLAG_NEW;
		flags &= ~FLAG_NEW;
	}

	if (!last_disk_update || !disk_buf[0])
		get_disk_info(disk_buf, sizeof(disk_buf));

	n = snprintf(model, sizeof(model), "model!!0!!sn=%s::descr=%s::arch=%s::flags=%X::%s",
					info->serial, info->model, ARCH, flags, info->os_version);
	snprintf(sysInfo, len,  "%.*s%s", (int)len-n, disk_buf, model);
	sysInfo[len-n-1] = '\n';
	return boot;
}

static void do_proxy_server(void)
{
	struct packet packet;
	size_t n;
	struct sockaddr_in saddr;
	struct timeval tv;
	socklen_t socklen;
	fd_set read_template;
	int status;
	int fd;
	FILE *fp;
	extern struct iface_struct iface[MAX_INTERFACES];
	extern int iface_count;
	int ret;
	info.flags = FLAG_NEW | FLAG_BOOTING | FLAG_INITRD;
	info.readynasd_online = false;
	info.next_readynasd_check = 0;

	get_sys_info();
	while (!quitting)
	{
		FD_ZERO(&read_template);

		interface_add_to_read_template(&read_template);
		if (netlink_fd >= 0)
			FD_SET(netlink_fd, &read_template);
		if (status_fd >= 0)
			FD_SET(status_fd, &read_template);

		tv.tv_sec = SELECT_TIMEOUT;
		tv.tv_usec = 0;

		status = select(FD_SETSIZE, &read_template, NULL, NULL, &tv);
		if (status < 0)
		{
			if (errno != EINTR)
			{
				perror("do_proxy: select");
				exit(-1);
			}
			continue; /* Ignore EINTR */
		}
		else if (status == 0)
		{
			continue; /* Skip to start of loop on timeout */
		}

		interface_set_fd_start();
		while ((fd = interface_set_fd_next(&read_template)) >= 0)
		{
			/* Read packet from UDP socket */
			memset(&packet, '\0', sizeof(packet));
			socklen = sizeof(saddr);
			n = recvfrom(fd, &packet, sizeof(packet),
				     0, (struct sockaddr*)&saddr, &socklen);
			if (ntohs(saddr.sin_port) <= 1024 || ntohs(saddr.sin_port) == RAIDARD_PORT)
			{
				printf("ignoring packet from invalid source port [%s:%d]\n", inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
				continue;
			}
			if (raidard_verbose)
				printf("received request of size %ld from sid=0x%x "
				       "cid=0x%x ip=%s:%d\n", n, ntohl(packet.sid),
				       ntohl(packet.cid), inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
			if (packet.version != htonl(RAIDARD_PROTO_VERSION))
			{
				if (raidard_verbose)
					printf("[%d] packet for unknown raidard_sid "
					       "version\n", getpid());
				continue;
			}

			if (htonl(packet.checksum) != packet_checksum(&packet))
			{
				/*printf("checksum mismatch 0x%x!=0x%x\n",
					ntohl(packet.checksum),
					packet_checksum(&packet));*/
				continue;
			}

			if (ntohl(packet.length) != n)
				continue;
			if (ntohl(packet.sid) != RAIDARD_LIST_SID &&
			    ntohl(packet.sid) != raidard_sid)
				continue;

			switch (ntohl(packet.flag) & 0xF) {
			case RAIDARD_LOCATE_FLAG:
				ret = system("/frontview/bin/blink all");
				if (ret != 0)
					fprintf(stderr, "error: failed to execute blink command\n");
				break;
			case RAIDARD_FORMAT_FLAG:
				if ((fp = fopen("/.failure_used_disks", "w")) != NULL)
				{
					fputs(packet.buf, fp);
					fclose(fp);
				}
				break;
			case RAIDARD_STOPTIMER_FLAG:
				if ((fp = fopen("/.os_stop_timer", "w")) != NULL)
				{
					fputs(packet.buf, fp);
					fclose(fp);
				}
				break;
			default:
				break;
			}

			if (ntohl(packet.sid) == RAIDARD_LIST_SID)
			{
				char *sysInfo;
				char ip[INET6_ADDRSTRLEN] = "";
				const char *boot;
				int sysInfoSz = ntohl(packet.flag) & RAIDAR_FLAG_COMPRESS_REQ ? 16384 : 1900;
				int i;

				sysInfo = malloc(sysInfoSz);
				if (!sysInfo)
					continue;

				boot = build_raidar_response(&info, sysInfo, sysInfoSz);

				for (i = 0; i < iface_count - 1; i++)
				{
					get_ip_str(&(iface[i].sa), ip);
					if (ntohl(packet.flag) & RAIDAR_FLAG_COMPRESS_REQ)
					{
						size_t outlen = sizeof(packet.buf);
						uint32_t flags = ntohl(packet.flag) | RAIDAR_FLAG_COMPRESS_RESP;
						char *buf;
						int ret;
						n = asprintf(&buf, "%s\t%s\t%s\t%s\t%s\t%u%s",
								iface[i].mac_addr, info.hostname, ip, sysInfo,
								info.os_status, raidard_sid, boot);
						if (n < 0)
							continue;
						ret = _compress((unsigned char *)buf, n,
								(unsigned char *)packet.buf, &outlen,
								Z_DEFAULT_COMPRESSION);
						if (ret != 0)
						{
							free(buf);
							continue;
						}
						packet.flag = htonl(flags);
						packet.length = htonl(outlen + PACKET_HDR_LEN);
						if (raidard_verbose > 1)
							printf("sending (compressed):\n%.*s\n", (int)n, buf);
						free(buf);
					}
					else
					{
						n = snprintf(packet.buf, sizeof(packet.buf), "%s\t%s\t%s\t%s\t%s\t%u%s",
							     iface[i].mac_addr, info.hostname, ip, sysInfo,
							     info.os_status, raidard_sid, boot);
						packet.length = htonl(n + PACKET_HDR_LEN);
						if (raidard_verbose > 1)
							printf("sending:\n%.*s\n", (int)n, packet.buf);
					}

					packet_send((struct sockaddr*)&saddr, &packet);
				}
				free(sysInfo);
			}
		}
		interface_set_fd_end();

		if (netlink_fd >= 0 && FD_ISSET(netlink_fd, &read_template))
		{
			process_netlink_event(netlink_fd);
		}

		if (status_fd >= 0 && FD_ISSET(status_fd, &read_template))
		{
			process_status_event(status_fd, &info.flags);
		}
	}
}

static void usage(int exit_status)
{
	fprintf(exit_status ? stderr : stdout, "\n"
		"raidard       [options]\n"
		"    -h            help\n"
		"    -p PORT       listen port number (UDP)\n"
		"    -d PORT       dest port (TCP)\n"
		"    -s SID        server id number\n"
		"    -I IP         set IP to send packets to\n"
		"    -T TTL        ttl for multicast packets\n"
		"    -b            Use broadcast (default)\n"
		"    -m            Use multicast\n"
		"    -u            Use unicast\n"
		"    -v            increase verbosity\n"
		"    -f            factory test flag\n"
		"    -D            debug mode (run in foreground)\n"
		);
	exit(exit_status);
}

static void quit(int sig)
{
	if (systemd)
		printf("quitting on signal %d...\n", sig);
	unlink(PIDFILE);
	quitting = 1;
}


int main(int argc, char *argv[])
{
	int c;
	char *p;
	int addr_set = 0;
	int daemonize = 1;
	FILE *pidfile;

	while ((c = getopt(argc, argv, "s:I:t:bmup:d:fvhSD")) != -1)
	{
		switch (c) {
		case 's':
			raidard_sid = atoi(optarg);
			break;
		case 'I':
			xcast_ip = optarg;
			addr_set++;
			break;
		case 't':
			mcast_ttl = atoi(optarg);
			break;
		case 'b':
			raidard_mode = MODE_BROADCAST | MODE_SERVER;
			if (!addr_set)
				xcast_ip = ADDR_BROADCAST;
			break;
		case 'm':
			raidard_mode = MODE_MULTICAST | MODE_SERVER;
			if (!addr_set)
				xcast_ip = ADDR_MULTICAST;
			break;
		case 'u':
			raidard_mode = MODE_UNICAST | MODE_SERVER;
			if (!addr_set)
				xcast_ip = ADDR_ANY;
			break;
		case 'p':
			raidard_port = atoi(optarg);
			break;
		case 'd':
			if ((p = strchr(optarg, ':')))
			{
				*p = 0;
				dest_port = atoi(p + 1);
			}
			else
			{
				dest_port = atoi(optarg);
			}
			break;
		case 'v':
			raidard_verbose++;
			break;
		case 'h':
			usage(0);
			break;
		case 'S':
			systemd = 1;
		case 'D':
			daemonize = 0;
			break;
		default:
			usage(-1);
			break;
		}
	}

	if (daemonize)
	{
		int ret = daemon(0, 0);
		if (ret != 0)
			fprintf(stderr, "main: failed to daemonize\n");
	}
	/*if(connection_table_init(xcast_ip))
	{
		fprintf(stderr, "main: connection_table_init\n");
		return(-1);
	}
	signal(SIGUSR1, (void(*)(int))dumper);*/
	signal(SIGHUP, (void (*)(int))reread);
	signal(SIGTERM, (void (*)(int))quit);
	signal(SIGINT, (void (*)(int))quit);

	/* Write out a pid file */
	pidfile = fopen(PIDFILE, "w");
	if (pidfile)
	{
		fprintf(pidfile, "%d\n", getpid());
		fclose(pidfile);
	}

	ifaceCount = interfaces_get();
	if (ifaceCount < 0)
	{
		exit(1);
	}

	netlink_fd = open_netlink_sock();

	if (access("/run/readynasd/raidar.fifo", R_OK) == 0 ||
	    mkfifo("/run/readynasd/raidar.fifo", 0600) == 0)
		status_fd = open("/run/readynasd/raidar.fifo", O_RDWR);

	/* We can just pass the mcast_ip as this
	 * argument isn't used unless we are in multicast mode
	 */
	if (interface_open_fd(ADDR_ANY, xcast_ip, raidard_port))
	{
		fprintf(stderr, "do_proxy: interface_open_fd");
		exit(-1);
	}

	do_proxy_server();
	return 0;
}
