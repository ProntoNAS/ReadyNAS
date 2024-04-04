/*
 * <disk_info.c>
 *
 * Copyright (C) 2000 Daniel Kobras
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * $Id: disk_info.c,v 1.16 2010/07/25 00:44:52 nold Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "diskhelper.h"
#include "disk_info.h"
#include "part_info.h"
#include "state.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>	/* for major()/minor() macros */
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>

static disk_info_t append_entry(disk_info_t *head)
{
	disk_info_t di, curr;

	di = malloc(sizeof(struct disk_info_s));
	if (!di)
		return NULL;
	di->next = NULL;

	if (!(*head))
	{
		*head = di;
		return di;
	}

	for (curr=*head; curr; curr=curr->next)
	{
		if (curr->next)
			continue;
		curr->next = di;
		break;
	}

	return di;
}		

static disk_info_t prepend_entry(disk_info_t head)
{
	disk_info_t di;

	di = malloc(sizeof(struct disk_info_s));
	if (!di)
		return NULL;

	di->next=head;
	return di;
}		

static disk_info_t get_disk_entry(disk_info_t head, int major, int minor)
{
	for (;head; head=head->next)
		if (head->major==major && head->minor==minor)
			break;

	return head;
}

static int is_ssd(const char *dev)
{
	FILE *fp;
	char buf[PATH_MAX];
	int val;

	snprintf(buf, sizeof(buf), "/sys/block/%s/queue/rotational", dev);
	fp = fopen(buf, "r");
	if (!fp)
		return 0;
	if (fscanf(fp, "%d", &val) != 1)
		return 0;
	fclose(fp);
	return !val;
}

static int is_internal_disk(const char *dev, int *channel)
{
	DIR *dp;
	struct dirent entry, de;
	struct dirent *result = &de;
	char buf[PATH_MAX], path[PATH_MAX];
	int ret = 0;

	dp = opendir("/dev/disk/internal");
	if (!dp)
		return 0;
	while (readdir_r(dp, &entry, &result) == 0) {
		ssize_t len;
		if (!result)
			break;
		if (strncmp(result->d_name, "0:", 2) != 0)
			continue;
		snprintf(path, sizeof(path), "%s/%s", "/dev/disk/internal", result->d_name);
		len = readlink(path, buf, sizeof(buf)-1);
		if (len == -1)
			continue;
		buf[len] = '\0';
		if (strcmp(dev, basename(buf)) == 0) {
			ret = 1;
			if (channel)
				*channel = atoi(result->d_name + 2) + 1;
			break;
		}
	}
	closedir(dp);

	if (ret == 1)
		INFO("Enabling spindown for disk %d [%s,%s]",
			atoi(result->d_name+2)+1, dev, result->d_name);

	return ret;
}

/*
 * Parse /proc/partitions to determine which kinds of disks are installed
 * in the system.
 */

disk_info_t disk_info_init(void)
{
	part_info_t part;
	disk_info_t di = NULL, head = NULL;
	int major, minor;
	char *name = NULL;
	int channel;

	part = part_info_init();
	if (!part) {
		ERR("Unable to open /proc/partitions");
		return NULL;
	}
	
	while (part_info_disk_next(part)) {
		major = part_info_get_major(part);
		minor = part_info_get_minor(part);

		if (is_ssd(part_info_get_name(part)))
		{
			INFO("Not enabling spindown for SSD [%s]", part_info_get_name(part));
			continue;
		}

		if (!is_internal_disk(part_info_get_name(part), &channel))
			continue;

		name = strdup(devname_get(part_info_get_name(part)));

		di = append_entry(&head);
		DEBUG("Added entry for %s (%d, %d)", name, major, minor);

		di->major = major;
		di->minor = minor;
		di->timeouts = NULL;
		di->name = strdup(name);	
		di->is_rw = 1;
		di->state = DISK_STATE_IGNORED;
		di->channel = channel;
	}

	part_info_release(part);
	free(name);
	return head;
}

/* 
 * Return pointer to disk info structure associated with device <name>.
 */

disk_info_t disk_info_get_byname(disk_info_t head, char *name)
{
	int ret;
	struct stat statbuf;
	disk_info_t di;

	ret = stat(name, &statbuf);
	if (ret == -1) {
		ERR("Unable to stat disk");
		return NULL;
	}
	if (!S_ISBLK(statbuf.st_mode)) {
		ERR("%s is not a valid block device", name);
		return NULL;
	}
	di = get_disk_entry(head, major(statbuf.st_rdev), 
	                          minor(statbuf.st_rdev));
	if (!di) {
		ERR("No partition entry for device (%d,%d)",
			major(statbuf.st_rdev), minor(statbuf.st_rdev));
		/* Some people tend to get the command line syntax wrong.
		 * Try to be gentle and give a hint for names that look
		 * like /dev/hda1 (instead of /dev/hda). Catches devfs
		 * errors as well btw - those have .../part(.[0-9]) instead
		 * of .../disc. */
		if (strlen(name) && isdigit(name[strlen(name)-1]))
			ERR("Are you trying to spin down a partition "
			    "instead of the whole disk?");
		return NULL;
	}

	free(di->name);
	di->name=strdup(name);

	return di;
}

void disk_info_mark_ro(disk_info_t di)
{
	if (di)
		di->is_rw=0;
}

/*
 * Initialize a disk drive (based on its device node <name>) for idle
 * monitoring with timeout as in <tohead> list.
 */

void disk_info_setup(disk_info_t di, tolist_t tohead)
{
	int timeout;
	
	if (!di)
		return;

	di->spindown = spindown_setup(di);	
	if (!di->spindown) {
		ERR("Don't know how to spindown %s", di->name);
		return;
	}
	
	di->timeouts = tohead;
	timeout = timeout_get(tohead);
	if (!NFD_TO_IS_VALID(timeout))
		ERR("No valid timeout found for %s", di->name);
	else if (NFD_TO_IS_REGULAR(timeout))
		di->state = DISK_STATE_SPINNING;

	DEBUG("Added disk %s, current timeout %d, state %d", 
		di->name, timeout, di->state);
}

