/*
 * Copyright (C) 2001-2004 Sistina Software, Inc. All rights reserved.  
 * Copyright (C) 2004-2005 Red Hat, Inc. All rights reserved.
 *
 * This file is part of the device-mapper userspace tools.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "lib.h"
#include "libdm-file.h"

#include <sys/file.h>
#include <fcntl.h>
#include <dirent.h>

static int _create_dir_recursive(const char *dir)
{
	char *orig, *s;
	int rc, r = 0;

	log_verbose("Creating directory \"%s\"", dir);
	/* Create parent directories */
	orig = s = dm_strdup(dir);
	while ((s = strchr(s, '/')) != NULL) {
		*s = '\0';
		if (*orig) {
			rc = mkdir(orig, 0777);
			if (rc < 0 && errno != EEXIST) {
				log_error("%s: mkdir failed: %s", orig,
					  strerror(errno));
				goto out;
			}
		}
		*s++ = '/';
	}

	/* Create final directory */
	rc = mkdir(dir, 0777);
	if (rc < 0 && errno != EEXIST) {
		log_error("%s: mkdir failed: %s", orig,
			  strerror(errno));
		goto out;
	}

	r = 1;
out:
	dm_free(orig);
	return r;
}

int create_dir(const char *dir)
{
	struct stat info;

	if (!*dir)
		return 1;

	if (stat(dir, &info) < 0)
		return _create_dir_recursive(dir);

	if (S_ISDIR(info.st_mode))
		return 1;

	log_error("Directory \"%s\" not found", dir);
	return 0;
}

