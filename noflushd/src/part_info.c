/*
 * <part_info.c>
 *
 * Copyright (C) 2001 Daniel Kobras
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
 * $Id: part_info.c,v 1.1.1.1 2010-09-21 01:09:51 jmaggard Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "part_info.h"
#include "state.h"
#include "util.h"
#include "diskhelper.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>	/* for major()/minor() macros */
#include <sys/types.h>

struct part_info_s {
	FILE *fp;	/* filp of /proc/partitions */
	int pos_major;	/* number of major entry in /proc/partitions */
	int pos_minor;	/* number of minor entry in /proc/partitions */
	int pos_name;	/* number of name entry in /proc/partitions */
	int major;	/* dev major of current entry */
	int minor;	/* dev minor of current entry */
	char *line;	/* next line to parse */
	char *name;	/* name of current entry */
};

/* The following functions determine whether a device name looks like a
 * name for a full disk (as opposed to a partition on a disk). They return
 * 1 for disks, and 0 for partitions.
 */
static int (*is_disk)(char *);

static int devfs_is_disk(char *name)
{
	int len;
	
	if (!name)
		return 0;
	
	len = strlen(name);
	
	if (len < 5)
		return 0;

	if (strcmp(name+len-5, "/disc"))
		return 0;

	return 1;
}

static int oldstyle_is_disk(char *name)
{
	int len;
	
	if (!name)
		return 0;

	len = strlen(name);
	
	if (len < 1)
		return 0;
	

	if (isdigit(name[strlen(name)-1]))
		return 0;

	return 1;
}

/* Check whether a device name looks devfs-like or oldstyle.
 */
static int name_is_devfs(char *name)
{
	return strchr(name, (int) '/') ? 1 : 0;
}

/* Check whether device names in /proc/partitions are devfs or oldstyle.
 * Set virtual function is_disk() accordingly.
 * Note: Calling check_devfs() will peek through /proc/partitions for a
 * valid device entry and thus clobber filepos in part->fp, and part->line!
 */
static void check_devfs(part_info_t part)
{

	char *major, *minor;
	char *name = NULL;
	
	goto start;
	
	while ((part->line=get_line(part->fp))) {

		release_entry(major);
		release_entry(minor);
		
	start:
		minor = NULL;
		
		if (!(major = get_entry(part->line, part->pos_major)) ||
		    !atoi(major) ||
		    !(minor = get_entry(part->line, part->pos_minor)))
			continue;
		
		/* XXX Hack!  Some meta devices like ataraid nowadays use
		 * a devfs-ish '/' in the name field.  Luckily they are
		 * accompanied by an ordinary IDE device, and we can simply
		 * ignore the meta devices.
		 */
		if (IS_META(atoi(major), atoi(minor)))
			continue;
		
		if ((name = get_entry(part->line, part->pos_name)))
			break;
	}

	if (!name)
		BUG("No valid entries found in /proc/partitions");
	
	if (name_is_devfs(name)) {
		DEBUG("Kernel talks devfs");
		is_disk = devfs_is_disk;
	} else {
		DEBUG("Kernel talks oldstyle dev");
		is_disk = oldstyle_is_disk;
	}
	
	release_entry(major);
	release_entry(minor);
	release_entry(name);
}	

/* Call to mark current line as treated. Fetches next line. */
static char *eat_line(part_info_t part)
{
	if (part->line)
		release_line(part->line);

	return (part->line=get_line(part->fp));
}

part_info_t part_info_init(void)
{
	char *entry;
	int pos;
	part_info_t part;

	part = malloc(sizeof(struct part_info_s));
	if (!part) {
		ERR("Could not alloc part struct");
		return NULL;
	}
	
	part->major = part->minor = 0;
	part->name = NULL;

	part->fp = fopen("/proc/partitions", "r");
	if (!part->fp) {
		ERR("Unable to open /proc/partitions");
		return NULL;
	}

	if (!(part->line=get_line(part->fp))) {
		ERR("Unable to read from /proc/partitions");
		part_info_release(part);
		return NULL;
	}

	/* Set up mapping between name tag and number of entry. */

	part->pos_major = part->pos_minor = part->pos_name = -1;
	
	for (pos=0; (entry=get_entry(part->line, pos)); pos++) {
		if (!strcmp(entry, "major"))
			part->pos_major=pos;
		else if (!strcmp(entry, "minor"))
			part->pos_minor=pos;
		else if (!strcmp(entry, "name"))
			part->pos_name=pos;
		release_entry(entry);
	}

	eat_line(part);
	
	if (part->pos_major==-1 || part->pos_minor==-1 || part->pos_name==-1) {
		ERR("Unknown format of /proc/partitions (%d, %d, %d)",
			part->pos_major, part->pos_minor, part->pos_name);
		part_info_release(part);
		return NULL;
	}

	check_devfs(part);
	part_info_reset(part);	/* check_devfs() clobbers filepos and line. */
	
	return part;
}

void part_info_release(part_info_t part)
{
	if (!part)
		return;

	if (part->line)
		release_line(part->line);
	if (part->name)
		free(part->name);
	if (part->fp)
		fclose(part->fp);
}

void part_info_reset(part_info_t part)
{
	if (!part || !part->fp)
		BUG("part_info_reset called on uninitialised part");

	fflush(part->fp);
	rewind(part->fp);

	/* Skip first line (tag line). */
	part->line = get_line(part->fp);
	eat_line(part);
}

/* part_info_next() handles both looking for partitions and whole disks.
 * Two bits in the flag parameter toggle the media type we are looking for,
 * and the behaviour upon unsuccessful lookups. The defines are overkill but
 * hopefully help clarify what's going on.
 */

#define NFD_PI_GET_PART		0x00	/* Looking for partition */
#define NFD_PI_GET_DISK		0x01	/* Looking for whole disk */
#define NFD_PI_MEDIA_MASK	0x0f
#define NFD_PI_FAIL_GO_ON	0x00	/* Go on searching if wrong media */
#define NFD_PI_FAIL_RETURN	0x10	/* Stop searching if wrong media */

static int part_info_next(part_info_t part, int flag)
{
	char *s_major, *s_minor, *name;
	int major, minor;

	if (!part || !part->fp)
		BUG("part_info_disk_next called on uninitialised part");

	if (!part->line)
		return 0;	/* EOF reached. */

	do {
		
		s_major=get_entry(part->line, part->pos_major);
		s_minor=get_entry(part->line, part->pos_minor);
		name=get_entry(part->line, part->pos_name);
		
		if (!s_major || !s_minor || !name) {
			release_entry(s_major);
			release_entry(s_minor);
			release_entry(name);
			continue;
		}

		major = atoi(s_major);
		minor = atoi(s_minor);
		release_entry(s_major);
		release_entry(s_minor);
		
		if (!major) {
			release_entry(name);
			continue;
		}

		if (!!is_disk(name) == !!(flag & NFD_PI_MEDIA_MASK))
			goto found;
		
		release_entry(name);
		
		if (flag & NFD_PI_FAIL_RETURN)
			return 0;
		
	} while (eat_line(part)); 

	/* End of file reached -> signal there are no more disks. */
	return 0;

found:
	if (part->name)
		free(part->name);

	part->name = strdup(name);
	part->major = major;
	part->minor = minor;

	release_entry(name);

	eat_line(part);
	return 1;
}

int part_info_disk_next(part_info_t part)
{
	return part_info_next(part, NFD_PI_GET_DISK | NFD_PI_FAIL_GO_ON);
}

int part_info_part_next(part_info_t part)
{
	return part_info_next(part, NFD_PI_GET_PART | NFD_PI_FAIL_RETURN);
}

char *part_info_get_name(part_info_t part)
{
	return part->name;
}

int part_info_get_major(part_info_t part)
{
	return part->major;
}
	
int part_info_get_minor(part_info_t part)
{
	return part->minor;
}
	
int part_info_is_devfs(void)
{
	return (is_disk == devfs_is_disk);
}

