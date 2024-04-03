/*
 * Copyright (C) 2004-2005 Kay Sievers <kay.sievers@vrfy.org>
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License as published by the
 *	Free Software Foundation version 2 of the License.
 * 
 *	This program is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	General Public License for more details.
 * 
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include "udev.h"


int log_priority(const char *priority)
{
	char *endptr;
	int prio;

	prio = strtol(priority, &endptr, 10);
	if (endptr[0] == '\0')
		return prio;
	if (strncasecmp(priority, "err", 3) == 0)
		return LOG_ERR;
	if (strcasecmp(priority, "info") == 0)
		return LOG_INFO;
	if (strcasecmp(priority, "debug") == 0)
		return LOG_DEBUG;
	if (string_is_true(priority))
		return LOG_ERR;

	return 0;
}

struct name_entry *name_list_add(struct list_head *name_list, const char *name, int sort)
{
	struct name_entry *name_loop;
	struct name_entry *name_new;

	/* avoid duplicate entries */
	list_for_each_entry(name_loop, name_list, node) {
		if (strcmp(name_loop->name, name) == 0) {
			dbg("'%s' is already in the list\n", name);
			return name_loop;
		}
	}

	if (sort)
		list_for_each_entry(name_loop, name_list, node) {
			if (strcmp(name_loop->name, name) > 0)
				break;
		}

	name_new = malloc(sizeof(struct name_entry));
	if (name_new == NULL)
		return NULL;

	strlcpy(name_new->name, name, sizeof(name_new->name));
	dbg("adding '%s'\n", name_new->name);
	list_add_tail(&name_new->node, &name_loop->node);

	return name_new;
}

struct name_entry *name_list_key_add(struct list_head *name_list, const char *key, const char *value)
{
	struct name_entry *name_loop;
	struct name_entry *name_new;

	list_for_each_entry(name_loop, name_list, node) {
		if (strncmp(name_loop->name, key, strlen(key)) == 0) {
			dbg("key already present '%s', replace it\n", name_loop->name);
			snprintf(name_loop->name, sizeof(name_loop->name), "%s=%s", key, value);
			name_loop->name[sizeof(name_loop->name)-1] = '\0';
			return name_loop;
		}
	}

	name_new = malloc(sizeof(struct name_entry));
	if (name_new == NULL)
		return NULL;

	snprintf(name_new->name, sizeof(name_new->name), "%s=%s", key, value);
	name_new->name[sizeof(name_new->name)-1] = '\0';
	dbg("adding '%s'\n", name_new->name);
	list_add_tail(&name_new->node, &name_loop->node);

	return name_new;
}

int name_list_key_remove(struct list_head *name_list, const char *key)
{
	struct name_entry *name_loop;
	struct name_entry *name_tmp;
	size_t keylen = strlen(key);
	int retval = 0;

	list_for_each_entry_safe(name_loop, name_tmp, name_list, node) {
		if (strncmp(name_loop->name, key, keylen) != 0)
			continue;
		if (name_loop->name[keylen] != '=')
			continue;
		list_del(&name_loop->node);
		free(name_loop);
		retval = 1;
		break;
	}
	return retval;
}

void name_list_cleanup(struct list_head *name_list)
{
	struct name_entry *name_loop;
	struct name_entry *name_tmp;

	list_for_each_entry_safe(name_loop, name_tmp, name_list, node) {
		list_del(&name_loop->node);
		free(name_loop);
	}
}

/* calls function for every file found in specified directory */
int add_matching_files(struct list_head *name_list, const char *dirname, const char *suffix)
{
	struct dirent *ent;
	DIR *dir;
	char filename[PATH_SIZE];

	dbg("open directory '%s'\n", dirname);
	dir = opendir(dirname);
	if (dir == NULL) {
		err("unable to open '%s': %s\n", dirname, strerror(errno));
		return -1;
	}

	while (1) {
		ent = readdir(dir);
		if (ent == NULL || ent->d_name[0] == '\0')
			break;

		if ((ent->d_name[0] == '.') || (ent->d_name[0] == COMMENT_CHARACTER))
			continue;

		/* look for file matching with specified suffix */
		if (suffix != NULL) {
			const char *ext;

			ext = strrchr(ent->d_name, '.');
			if (ext == NULL)
				continue;
			if (strcmp(ext, suffix) != 0)
				continue;
		}
		dbg("put file '%s/%s' into list\n", dirname, ent->d_name);

		snprintf(filename, sizeof(filename), "%s/%s", dirname, ent->d_name);
		filename[sizeof(filename)-1] = '\0';
		name_list_add(name_list, filename, 1);
	}

	closedir(dir);
	return 0;
}

uid_t lookup_user(const char *user)
{
	struct passwd *pw;
	uid_t uid = 0;

	errno = 0;
	pw = getpwnam(user);
	if (pw == NULL) {
		if (errno == 0 || errno == ENOENT || errno == ESRCH)
			err("specified user '%s' unknown\n", user);
		else
			err("error resolving user '%s': %s\n", user, strerror(errno));
	} else
		uid = pw->pw_uid;

	return uid;
}

extern gid_t lookup_group(const char *group)
{
	struct group *gr;
	gid_t gid = 0;

	errno = 0;
	gr = getgrnam(group);
	if (gr == NULL) {
		if (errno == 0 || errno == ENOENT || errno == ESRCH)
			err("specified group '%s' unknown\n", group);
		else
			err("error resolving group '%s': %s\n", group, strerror(errno));
	} else
		gid = gr->gr_gid;

	return gid;
}

