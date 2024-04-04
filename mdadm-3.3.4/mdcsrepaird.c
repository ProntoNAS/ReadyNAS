/*
 * mdcsrepaird - daemon for repair of checksum-mismatching block
 * for RAID-1/5/6/10
 *
 * Copyright (C) 2014 NETGEAR
 * Copyright (C) 2014,2015 Hiro Sugawara
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    Author: Hiro Sugawara
 */

#include "mdadm.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <alloca.h>
#include <time.h>
#include <ctype.h>
#include <sys/socket.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <linux/dm-ioctl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <pthread.h>

#define	RATE_DELAY	5
#define	GC_INTERVAL	10

const char Name[] = "mdcsrepaird";
static bool dopt, lopt, nopt, ropt, vopt, wopt, xopt;
static char *repairer;

#define __err(ec, ...)	do {__warn(__VA_ARGS__); exit((ec));} while(0);
#define __errx(ec, ...)	do {__warnx(__VA_ARGS__); exit((ec));} while(0);

static void __warn(const char *fmt, ...)
{
	va_list ap;

	if (!lopt || dopt) {
		va_start(ap, fmt);
		vwarn(fmt, ap);
		va_end(ap);
	}
	if (lopt) {
		char *msg;

		va_start(ap, fmt);
		vasprintf(&msg, fmt, ap);
		syslog(LOG_WARNING, "%s: %s", msg, strerror(errno));
		free(msg);
		va_end(ap);
	}
}

static void __warnx(const char *fmt, ...)
{
	va_list ap;

	if (!lopt || dopt) {
		va_start(ap, fmt);
		vwarnx(fmt, ap);
		va_end(ap);
	}
	if (lopt) {
		va_start(ap, fmt);
		vsyslog(LOG_WARNING, fmt, ap);
		va_end(ap);
	}
}

/*
 * Determines if the given set of dev/pos/len combination has not been
 * found recently and thus it is to be processed (rate limiting).
 * "gonly" makes do garbage-collection only/
 */
static bool newrepair(const char *device,
			const char *position, const char *length, bool gconly)
{
	char mykey[256];
	time_t now = time(NULL);
	struct ratelink *rlp, *prev = NULL, *match = NULL;
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	static struct ratelink {
		char *key;
		time_t expire;
		struct ratelink *next;
	} *ratelink;

	if (!gconly && snprintf(mykey, sizeof mykey, "%s %s@%s",
			device, position, length) >= (int)sizeof mykey)
		err(1, "%s: mykey overflowed.", __func__);

	pthread_mutex_lock(&mutex);
	rlp = ratelink;
	while (rlp) {
		if (rlp->expire <= now) {
			do {
				struct ratelink *next = rlp->next;
				free(rlp->key);
				free(rlp);
				rlp = next;
			} while (rlp);
			if (prev)
				prev->next = NULL;
			else
				ratelink = NULL;
			break;
		}
		if (!gconly && !match && !strcmp(rlp->key, mykey))
			match = rlp;
		prev = rlp;
		rlp = rlp->next;
	}
	pthread_mutex_unlock(&mutex);

	if (gconly || match)
		return false;

	if (!(rlp = malloc(sizeof *rlp)) || !(rlp->key = strdup(mykey)))
		err(1, "%s: malloc failed.", __func__);
	rlp->expire = now + RATE_DELAY;
	rlp->next = ratelink;
	ratelink = rlp;
	return true;
}

/*
 * Garbage collector thread.
 */
static void *garbage_collector(void *p)
{
	while (true) {
		newrepair(NULL, NULL, NULL, true);
		while (wait(NULL) >= 0)
			;
		sleep(GC_INTERVAL);
	}
	return NULL;
}

static char *find_inum(const char *point, const char *inum)
{
	FILE *fp;
	char *path = NULL, *cmd = NULL;

	/* Use the much faster BTRFS-specific implementation */
	/* if (asprintf(&cmd, "find %s -xdev -inum %s", point, inum) <= 0) { */
	if (asprintf(&cmd, "btrfs inspect inode %s %s", inum, point) <= 0) {
		__warn("failed to asprintf find ...");
		return NULL;
	}

	if ((fp = popen(cmd, "r"))) {
		char buf[PATH_MAX], *p;
		size_t n;

		p = fgets(buf, sizeof buf, fp);
		pclose(fp);
		if (p) {
			n = strlen(p);
			if (p[n - 1] == '\n')
				p[n - 1] = '\0';
			if (p[0])
				path = strdup(p);
		}
	}
	free(cmd);
	return path;
}

static char *find_path_btrfs_subvol(const char *point, const char *inum,
					dev_t adev)
{
	char *path = NULL, *subvol = NULL, line[PATH_MAX];
	FILE *pp;

	if (snprintf(line, sizeof line,
			"btrfs subvol list %s | awk '{ print $NF }'",
			point) <= 0) {
		__warn("failed to asprintf cmd subvol list ...");
		return NULL;
	}

	if (!(pp = popen(line, "r"))) {
		__warn("failed to asprintf cmd subvol list ...");
		return NULL;
	}

	while (fgets(line, sizeof line, pp)) {
		size_t n = strlen(line);
		struct stat st;

		if (line[n - 1] == '\n')
			line[n - 1] = '\0';
		if (asprintf(&subvol, "%s/%s", point, line) <= 0) {
			__warn("failed to asprintf path ...");
			break;
		}
		if (!stat(subvol, &st) &&
			st.st_dev == adev &&
			(path = find_inum(subvol, inum)))
			break;
		free(subvol);
	}
	pclose(pp);
	return path;
}

static void free_volume_group(char **volgroup)
{
	char **vg = volgroup;

	if (vg) {
		while (*vg)
			free(*vg++);
		free(volgroup);
	}
}

/*
 * Return a list of /dev/xxx belonging to the same volume group as dev does.
 * Each volume group is like...
Label: none  uuid: d5b5f744-cc17-4376-9643-eb3698f99f61
	Total devices 2 FS bytes used 401.67MB
	devid    2 size 298.50MB used 297.50MB path /dev/md91
	devid    1 size 298.50MB used 250.75MB path /dev/md90
 */
static char **get_volume_group_btrfs(const char *dev)
{
	char **volgroup = NULL;
	char buf[1024];
	FILE *pp = popen("btrfs filesystem show", "r");

	if (!pp) {
		warn("failed to open 'btrfs filesystem show'");
		return NULL;
	}
	while (!feof(pp)) {
		int n = 0;

		while (fgets(buf, sizeof buf, pp)) {
			/* Trim trailing whitespaces. */
			char *p = buf + strlen(buf) - 1;
			while (p >= buf && isspace(*p))
				*p-- = '\0';

			/* Delimiter line? */
			if (!buf[0] || !strncmp(buf, "Label: ", 7))
				break;
			for (p = buf; isspace(*p); p++)
				;
			if (strncmp(p, "devid ", 6))
				continue;

			/* Extrac device name and add it to the list. */
			for (p = buf + strlen(p) - 1;
				p >= buf &&!isspace(*p); p--)
				;
			volgroup = realloc(volgroup,
						(++n + 1) * sizeof *volgroup);
			volgroup[n - 1] = strdup(++p);
			volgroup[n] = NULL;
		}

		if (volgroup) {
			char **vg;

			/* Look for the desired dev. */
			for (vg = volgroup; *vg && strcmp(*vg, dev); vg++)
				;
			if (*vg)
				break;
			else {/* Not found. Release the list. */
				free_volume_group(volgroup);
				volgroup = NULL;
			}
		}
	}
	pclose(pp);

	return volgroup;
}

/*
 * Finds file path for given inode number under the mount/subvol point.
 * Caller must free the returned string.
 */
static char *find_path(const char *point, const char *inum, const char *fs,
			dev_t adev)
{
	char *path = NULL;
	struct stat st;
	char buf[PATH_MAX];
	FILE *fp;

	errno = 0;
	if (!inum || !strtoull(inum, NULL, 10) || errno)
		return NULL;

	if (stat(point, &st)) {
		__warn("failed to stat %s.", point);
		return NULL;
	}

	if (S_ISBLK(st.st_mode)) {/* Look for mount point by device. */
		char **volgroup = NULL;

		if (!strcmp(fs, "btrfs"))
			volgroup = get_volume_group_btrfs(point);
		if (!(fp = fopen("/proc/mounts", "r"))) {
			__warn("failed to open /proc/mounts.");
			return NULL;
		}
		while (fgets(buf, sizeof buf, fp)) {
			char *dev = NULL, *mnt = NULL,
				*root = NULL, *thisfs = NULL, *dev2;

			if (sscanf(buf, "%ms %ms %ms",
					&dev, &mnt, &thisfs) != 3) {
				__warn("/proc/mounts could not be parsed.");
				free(dev);
				free(mnt);
				free(thisfs);
				break;
			}

			if (strcmp(thisfs, fs)) {
				free(dev);
				free(mnt);
				free(thisfs);
				continue;
			}

			free(thisfs);

			/* /proc/partitions may list symlink to mounted devs */
			if (!(dev2 = realpath(dev, NULL)))
				err(1, "failed to resolve partitions dev");
			free(dev);
			if (!strcmp(dev2, point))
				root = mnt;
			else if (volgroup) {
				char **vg = volgroup;
				while (*vg)
					if (!strcmp(dev2, *vg++)) {
						root = mnt;
						break;
					}
			}
			free(dev2);
			if (root) {
				struct stat st;
				if ((!adev ||
					(!stat(mnt, &st) &&
						st.st_dev == adev)) &&
					(path = find_inum(mnt, inum))) {
					free(mnt);
					break;
				}
				if (!strcmp("btrfs", fs) &&
					adev &&
					(path = find_path_btrfs_subvol(mnt,
								inum, adev))) {
					free(mnt);
					break;
				}
			}
			free(mnt);
		}
		fclose(fp);
		free_volume_group(volgroup);
	} else {
		path = find_inum(point, inum);
	}

	return path;
}

/* ***UNTESTED**
 * Resolve dm (device mapper) layer dev to underlying device.
 * offset will be updated.
 * Only "linear" mapping is supported.
 * offset/length crossing a mapping boundary is an error.
 */
static char *map_dm(const char *dev, char *offset, const char *length)
{
	FILE *fp;
	char *cmd;

	if (asprintf(&cmd, "dmsetup table %s", dev) <=0 ) {
		__warn("failed to asprintf dmsetup ...");
		return NULL;
	}
	if (!(fp = popen(cmd, "r"))) {
		__warn("failed to execute '%s'", cmd);
		free(cmd);
		return NULL;
	}
	free(cmd);

	__u64 pos = strtoull(offset, NULL, 10);
	__u64 len = strtoull(length, NULL, 10);
	__u64 start, size, dmoffset;
	char dmtype[DM_MAX_TYPE_NAME];
	int dmmajor, dmminor;
	char *p = NULL;

	while (fscanf(fp, "%llu %llu %s %d:%d %llu",
			&start, &size,
			dmtype, &dmmajor, &dmminor, &dmoffset) == 6) {
		if (strcmp(dmtype, "linear"))
			continue;
		if (start <= pos && pos + len <= start + size) {
			sprintf(offset, "%llu", dmoffset + (pos - start));
			if (asprintf(&p, "/dev/block/%d:%d", dmmajor, dmminor) <= 0) {
				__warn("failed to asprintf /dev/block/ ...");
				p = NULL;
			}
			break;
		}
	}
	pclose(fp);

	if (!p) {
		__warnx("failed to resolve dm-device %s", dev);
		return NULL;
	}

	char *p2 = realpath(p, NULL);
	free(p);
	return p2;
}

/*
 * Map MAJOR:MINOR to /dev/xxx.
 * If MAJOR is zero, MINOR is a fake device number that we cannot handle.
 * If intra-file position, we cannot handle.
 * If in debug mode and inode is non-NULL, possible file path is returned.
 * Process LVM (device mapper) layer to convert offset.
 * Caller must free the returned string and *pathp.
 */
static char *map2md(dev_t dev, const char *fs,
			dev_t adev,
			bool intrafile,
			const char *inode, char *offset, const char *length,
			char **pathp)
{
	char *devname = NULL;
	int major = major(dev);
	int minor = minor(dev);

	if (!major(dev)) {
		errno = ENOENT;
		__warn("failed to map %d:%d to a device.", major, minor);
		goto fail2;
	}

	if (intrafile) {
		__warnx("intra-file position not supported.");
		goto fail;
	}

	char *p = NULL;
	if (asprintf(&p, "/dev/block/%d:%d", major, minor) <= 0) {
		__warn("failed to asprintf /dev/block/ ...");
		goto fail2;
	}
	
	devname = realpath(p, NULL);
	if (!devname) {
		__warn("failed to map %d:%d to a device.", major, minor);
		goto fail;
	}
	free(p);

	/* If offset is intra-file position, map it to on-disk position. */
	if (inode && *inode && atoll(inode)) {
		char *path = find_path(devname, inode, fs, adev);
		if (!path) {
			errno = ENOENT;
			__warn("cannot find file path for %s:%s",
							devname, inode);
			goto fail2;
		}
		if (pathp)
			*pathp = path;
		else
			free(path);
	}

	/* If device is a dm-device, resolve to base device. */
	if (asprintf(&p, "/sys/dev/block/%d:%d/dm/name", major, minor) <= 0) {
		__warn("failed to asprintf /dev/block/ ...");
		goto fail2;
	}
	if (!access(p, R_OK)) {
		char *p2 = devname;
		devname = map_dm(p2, offset, length);
		free(p2);
	}
	free(p);

	return devname;

fail:
	free(p);
fail2:
	free(devname);
	return NULL;
}

static dev_t *devIDs;

static void deviceID_list(const char *arg)
{
	static int num_devIDs = 0;
	int major, minor;
	dev_t dev;

	if (sscanf(arg, "%d:%d", &major, &minor) != 2)
		errx(1, "Malformed deivce ID for -D %s", arg);
	dev = makedev(major, minor);
	devIDs = realloc(devIDs, (++num_devIDs + 1) * sizeof *devIDs);
	devIDs[num_devIDs - 1] = dev;
	devIDs[num_devIDs] = 0;
}

static bool deviceID_filter(dev_t dev)
{
	dev_t *devID = devIDs;

	if (!devID)
		return true;

	while (*devID)
		if (*devID++ == dev)
			return true;
	return false;
}

enum {
	DATA_OFFSET = 0,
	DATA_LENGTH,
	CS_EXPECTED,
	CS_COMPUTED,
	CS_LENGTH,
	CS_ALGORITHM,
	DEV_MAJOR,
	DEV_MINOR,
	DEV_FILESYSTEM,
	FILE_INODE,
	FILE_OFFSET,
	ADEV_MAJOR,
	ADEV_MINOR,

	/* Optional */
	DATA_POSITION,
	PL_FUNC,
	FL_NOEXEC,
	ARG_SIZE
};

static char **repargv;
static int repargc = 1;

#define _strcmp(a,b)	strcmp((const char *)(a), (b))

static int parseXML(const char *content)
{
	/* List of required elements. */
	unsigned int remain = (1<<DATA_OFFSET)	| (1<<DATA_LENGTH) |
				(1<<CS_EXPECTED)| (1<<CS_COMPUTED) |
				(1<<CS_ALGORITHM) |
				(1<<DEV_MAJOR)	| (1<<DEV_MINOR |
				(1<<DEV_FILESYSTEM) |
				(1<<FILE_INODE));
	char *path = NULL;
	char *args[ARG_SIZE];
	xmlDocPtr doc;
	int ret = 0;

	if (xopt) {
		printf("%s%s", content,
			(content[strlen(content) - 1] == '\n') ? "" : "\n");
	}

	doc = xmlReadMemory(content, strlen(content), "noname.xml", NULL, 0);
	if (!doc) {
		int err = errno;

		__warnx("malformed XML: %s", content);
		errno = err;
		return -1;
	}
	memset(args, 0, sizeof args);

	if (_strcmp(doc->children->name, "mdcsrepair") &&
		_strcmp(doc->children->name, "mdcsrepair_procfs")) {
		errno = EINVAL;
		ret = -1;
		goto exit;
	}

	struct _xmlNode *cslink = doc->children->children;

#define SETARG(p, n, x)	\
	(!_strcmp((p)->name, (n)) &&	\
		((args[(x)] = (char *)(p)->children->content),	\
		(remain &= ~(1 << (x))), 1))

	while (cslink) {
		if (!_strcmp(cslink->name, "data")) {
			struct _xmlAttr *data = cslink->properties;
			while (data) {
				SETARG(data, "offset", DATA_OFFSET) ||
				SETARG(data, "length", DATA_LENGTH) ||
				SETARG(data, "position", DATA_POSITION);
				data = data->next;
			}
		} if (!_strcmp(cslink->name, "checksum")) {
			struct _xmlAttr *cs = cslink->properties;
			while (cs) {
				SETARG(cs, "expected", CS_EXPECTED) ||
				SETARG(cs, "computed", CS_COMPUTED) ||
				SETARG(cs, "length", CS_LENGTH) ||
				SETARG(cs, "algorithm", CS_ALGORITHM);
				cs = cs->next;
			}
		} else if (!_strcmp(cslink->name, "device")) {
			struct _xmlAttr *device = cslink->properties;
			while (device) {
				SETARG(device, "major", DEV_MAJOR) ||
				SETARG(device, "minor", DEV_MINOR) ||
				SETARG(device, "filesystem", DEV_FILESYSTEM);
				device = device->next;
			}
		} else if (!_strcmp(cslink->name, "anondevice")) {
			struct _xmlAttr *adevice = cslink->properties;
			while (adevice) {
				SETARG(adevice, "major", ADEV_MAJOR) ||
				SETARG(adevice, "minor", ADEV_MINOR);
				adevice = adevice->next;
			}
		} else if (!_strcmp(cslink->name, "file")) {
			struct _xmlAttr *file = cslink->properties;
			while (file) {
				SETARG(file, "inode", FILE_INODE) ||
				SETARG(file, "offset", FILE_OFFSET);
				file = file->next;
			}
		} else if (!_strcmp(cslink->name, "place")) {
			struct _xmlAttr *place = cslink->properties;
			while (place) {
				SETARG(place, "function", PL_FUNC);
				place = place->next;
			}
		} else if (!_strcmp(cslink->name, "flag")) {
			struct _xmlAttr *flag = cslink->properties;
			while (flag) {
				SETARG(flag, "noexec", FL_NOEXEC);
				flag = flag->next;
			}
		}
		cslink = cslink->next;
	}

	if (remain) {
		__warnx("missing arg in %s", content);
		errno = EINVAL;
		ret = -1;
		goto exit;
	}

	/* Get deivce file name. */
	char offset[24];
	char *devname = NULL;
	dev_t adev = 0, dev = 0;

	if (args[ADEV_MAJOR] && args[ADEV_MINOR])
		adev = makedev(atoi(args[ADEV_MAJOR]), atoi(args[ADEV_MINOR]));
	if (args[DEV_MAJOR] && args[DEV_MINOR])
		dev = makedev(atoi(args[DEV_MAJOR]), atoi(args[DEV_MINOR]));

	if (!(adev && deviceID_filter(adev)) && !(dev && deviceID_filter(dev)))
		goto exit;

	strncpy(offset, args[DATA_OFFSET], sizeof offset);
	devname = map2md(dev,
			args[DEV_FILESYSTEM], adev,
			args[DATA_POSITION] &&
				!strcmp(args[DATA_POSITION], "file"),
			args[FILE_INODE],
			offset, args[DATA_LENGTH], &path);
	if (!devname) {
		errno = ENOENT;
		ret = -1;
		goto exit;
	}

	/* We spawn repairer only if checksums mismatch and if the same
	   defect was not reported too recently. */
	if ((dopt || strcasecmp(args[CS_EXPECTED], args[CS_COMPUTED])) &&
		(!ropt && newrepair(devname, offset,
					args[DATA_LENGTH], false))) {

		if (dopt) {
			printf("[%s]", args[PL_FUNC]);
			if (path)
				printf(" %s:%s", args[FILE_INODE], path);
			if (args[FILE_OFFSET])
				printf(" @ %s", args[FILE_OFFSET]);
			printf("\n%s %s @ %s %s %s B:%s G:%s\n",
				devname,
				args[DATA_LENGTH], offset,
				args[CS_ALGORITHM], args[CS_LENGTH],
				args[CS_COMPUTED], args[CS_EXPECTED]);
			fflush(stdout);
		}
		if (nopt)
			goto exit;
		if (!fork()) {
			int i, len = 0;
			char *p;

			repargv = realloc(repargv,
					(repargc + 7) * sizeof *repargv);
			repargv[repargc++] = devname;
			repargv[repargc++] = offset;
			repargv[repargc++] = args[DATA_LENGTH];
			repargv[repargc++] = args[CS_COMPUTED];
			repargv[repargc++] = args[CS_EXPECTED];
			if (path)
				repargv[repargc++] = path;
			repargv[repargc] = NULL;

			for (i = 0; i < repargc; i++)
				len += strlen(repargv[i]) + 1;
			p = alloca(len);
			strcpy(p, repargv[0]);
			for (i = 1; i < repargc; i++) {
				strcat(p, " ");
				strcat(p, repargv[i]);
			}
			__warnx("%s", p);
			execvp(repairer, repargv);
		} else if (wopt) {
			int status;

			if (wait(&status) >= 0 && WIFEXITED(status)) {
				errno = EINVAL;
				ret = -1;
			}
		}
	}
exit:
	xmlFreeDoc(doc);
	free(devname);
	free(path);
	return ret;
}

static void help(int ec, FILE *fp, const char *argv0)
{
	fprintf(fp,
"Daemon to accept checksum-based data correction requests\n"
"Usage:\n"
"%s [opt]\n"
"    -D <M:n>   filter device ID (repeatable)\n"
"    -d    debug on\n"
"    -h    help message\n"
"    -l    log warnings to syslog\n"
"    -n    no repair\n"
"    -o <opts>  data repairer program options\n"
"    -p <prog>  data repairer program name\n"
"    -r    no rate limiting\n"
"    -v    verbose mode on\n"
"    -w    exit on error including mdcsrepair exit status for one-time operation\n"
"    -x    show XML string\n",
		basename(argv0));
	exit(ec);
}


int main(int argc, char *argv[])
{
	pthread_t thid;
	pthread_attr_t thattr;
	int opt;

	LIBXML_TEST_VERSION

	repargv = calloc(2, sizeof *repargv);
	repairer = strdup(argv[0]);
	repairer[strlen(repairer) - 1] = '\0';	/* Remove last 'd'. */

	while ((opt = getopt (argc, argv, "?D:dhlno:p:rvwx")) != -1) {
		switch (opt) {
		case '?':
		case 'h':
			help(0, stdout, argv[0]);
			break;
		case 'D':
			deviceID_list(optarg);
			break;
		case 'd':
			dopt = true;
			break;
		case 'l':
			lopt = true;
			break;
		case 'n':
			nopt = 1;
			break;
		case 'o':
			while ((repargv[repargc] = strtok(optarg, " "))) {
				optarg = NULL;
				repargv = realloc(repargv,
					(++repargc + 1) * sizeof *repargv);
				repargv[repargc] = NULL;
			}
			break;
		case 'p':
			repairer = optarg;
			break;
		case 'r':
			ropt = true;
			break;
		case 'v':
			vopt = true;
			break;
		case 'w':
			wopt = true;
			break;
		case 'x':
			xopt = true;
			break;
		default:
			help(1, stderr, argv[0]);
			break;
		}
	}

	repargv[0] = basename(repairer);
	if (lopt) {
		repargv[repargc] = "-l";
		repargv = realloc(repargv, (++repargc + 1) * sizeof *repargv);
		repargv[repargc] = NULL;
	}

	pthread_attr_init(&thattr);
	if (!ropt && pthread_create(&thid, &thattr, garbage_collector, NULL))
		err(1, "failed to create a garbage collector thread.");

	/* Loop to read from a PROCFS entry. */
	while (true) {
		char buffer[4096];
		const char *pfspath = "/proc/fs/btrfs/mdcsevent";
		FILE *fp = fopen(pfspath, "r");

		if (!fp)
			err(1, "cannot open %s.", pfspath);

		fgets(buffer, sizeof buffer, fp);
		fclose(fp);

		if (parseXML(buffer) && wopt)
			exit(1);
	}

	return 0;
}

