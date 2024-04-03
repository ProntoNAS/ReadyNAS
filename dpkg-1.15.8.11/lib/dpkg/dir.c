/*
 * libdpkg - Debian packaging suite library routines
 * dir.c - directory handling functions
 *
 * Copyright © 2010 Guillem Jover <guillem@debian.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <compat.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <dpkg/dpkg.h>
#include <dpkg/i18n.h>
#include <dpkg/varbuf.h>
#include <dpkg/dir.h>

void
dir_sync(DIR *dir, const char *path)
{
	int fd;

	fd = dirfd(dir);
	if (fd < 0)
		ohshite(_("unable to get file descriptor for directory '%s'"),
		        path);

	if (fsync(fd))
		ohshite(_("unable to sync directory '%s'"), path);
}

void
dir_sync_path(const char *path)
{
	DIR *dir;

	dir = opendir(path);
	if (!dir)
		ohshite(_("unable to open directory '%s'"), path);

	dir_sync(dir, path);

	closedir(dir);
}

void
dir_sync_path_parent(const char *path)
{
	char *dirname, *slash;

	dirname = m_strdup(path);

	slash = strrchr(dirname, '/');
	if (slash != NULL) {
		*slash = '\0';
		dir_sync_path(dirname);
	}

	free(dirname);
}

/* FIXME: Ideally we'd use openat() here, to avoid the path mangling, but
 * it's not widely available yet, so this will do for now. */
static void
dir_file_sync(const char *dir, const char *filename)
{
	struct varbuf path = VARBUF_INIT;
	int fd;

	varbufprintf(&path, "%s/%s", dir, filename);

	fd = open(path.buf, O_WRONLY);
	if (fd < 0)
		ohshite(_("unable to open file '%s'"), path.buf);
	if (fsync(fd))
		ohshite(_("unable to sync file '%s'"), path.buf);
	if (close(fd))
		ohshite(_("unable to close file '%s'"), path.buf);

	varbuf_destroy(&path);
}

void
dir_sync_contents(const char *path)
{
	DIR *dir;
	struct dirent *dent;

	dir = opendir(path);
	if (!dir)
		ohshite(_("unable to open directory '%s'"), path);

	while ((dent = readdir(dir)) != NULL) {
		if (strcmp(dent->d_name, ".") == 0 ||
		    strcmp(dent->d_name, "..") == 0)
			continue;

		dir_file_sync(path, dent->d_name);
	}

	dir_sync(dir, path);

	closedir(dir);
}
