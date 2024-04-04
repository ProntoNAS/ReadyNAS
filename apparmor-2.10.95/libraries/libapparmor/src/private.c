/*
 * Copyright 2014 Canonical Ltd.
 *
 * The libapparmor library is licensed under the terms of the GNU
 * Lesser General Public License, version 2.1. Please see the file
 * COPYING.LGPL.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "private.h"

/**
 * Allow libapparmor to build on older systems where secure_getenv() is still
 * named __secure_getenv(). This snippet was taken from the glibc wiki
 * (https://sourceware.org/glibc/wiki/Tips_and_Tricks/secure_getenv).
 */
#ifndef HAVE_SECURE_GETENV
 #ifdef HAVE___SECURE_GETENV
  #define secure_getenv __secure_getenv
 #else
  #error neither secure_getenv nor __secure_getenv is available
 #endif
#endif

struct ignored_suffix_t {
	const char * text;
	int len;
	int silent;
};

static struct ignored_suffix_t ignored_suffixes[] = {
	/* Debian packging files, which are in flux during install
           should be silently ignored. */
	{ ".dpkg-new", 9, 1 },
	{ ".dpkg-old", 9, 1 },
	{ ".dpkg-dist", 10, 1 },
	{ ".dpkg-bak", 9, 1 },
	/* RPM packaging files have traditionally not been silently
           ignored */
	{ ".rpmnew", 7, 0 },
	{ ".rpmsave", 8, 0 },
	/* patch file backups/conflicts */
	{ ".orig", 5, 0 },
	{ ".rej", 4, 0 },
	/* Backup files should be mentioned */
	{ "~", 1, 0 },
	{ NULL, 0, 0 }
};

#define DEBUG_ENV_VAR	"LIBAPPARMOR_DEBUG"

void print_error(bool honor_env_var, const char *ident, const char *fmt, ...)
{
	va_list args;
	int openlog_options = 0;

	if (honor_env_var && secure_getenv(DEBUG_ENV_VAR))
		openlog_options |= LOG_PERROR;

	openlog(ident, openlog_options, LOG_ERR);
	va_start(args, fmt);
	vsyslog(LOG_ERR, fmt, args);
	va_end(args);
	closelog();
}

void print_debug(const char *fmt, ...)
{
	va_list args;

	if (!secure_getenv(DEBUG_ENV_VAR))
		return;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

void atomic_inc(unsigned int *v)
{
	__sync_add_and_fetch(v, 1);
}

bool atomic_dec_and_test(unsigned int *v)
{
	return __sync_sub_and_fetch(v, 1) == 0;
}

int _aa_is_blacklisted(const char *name)
{
	size_t name_len = strlen(name);
	struct ignored_suffix_t *suffix;

	/* skip dot files and files with no name */
	if (!name_len || *name == '.' || !strcmp(name, "README"))
		return 1;

	/* skip blacklisted suffixes */
	for (suffix = ignored_suffixes; suffix->text; suffix++) {
		char *found;
		if ( (found = strstr((char *) name, suffix->text)) &&
		     found - name + suffix->len == name_len ) {
			if (!suffix->silent)
				return -1;
			return 1;
		}
	}

	return 0;
}

/* automaticly free allocated variables tagged with autofree on fn exit */
void _aa_autofree(void *p)
{
	void **_p = (void**)p;
	free(*_p);
}

void _aa_autoclose(int *fd)
{
	if (*fd != -1) {
		/* if close was interrupted retry */
		while(close(*fd) == -1 && errno == EINTR);
		*fd = -1;
	}
}

void _aa_autofclose(FILE **f)
{
	if (*f) {
		fclose(*f);
		*f = NULL;
	}
}

int _aa_asprintf(char **strp, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = vasprintf(strp, fmt, args);
	va_end(args);

	if (rc == -1)
		*strp = NULL;

	return rc;
}

static int dot_or_dot_dot_filter(const struct dirent *ent)
{
	if (strcmp(ent->d_name, ".") == 0 ||
	    strcmp(ent->d_name, "..") == 0)
		return 0;

	return 1;
}

/**
 * _aa_dirat_for_each: iterate over a directory calling cb for each entry
 * @dirfd: already opened directory
 * @name: name of the directory (NOT NULL)
 * @data: data pointer to pass to the callback fn (MAY BE NULL)
 * @cb: the callback to pass entry too (NOT NULL)
 *
 * Iterate over the entries in a directory calling cb for each entry.
 * The directory to iterate is determined by a combination of @dirfd and
 * @name.
 *
 * See the openat section of the open(2) man page for details on valid @dirfd
 * and @name combinations. This function does accept AT_FDCWD as @dirfd if
 * @name should be considered relative to the current working directory.
 *
 * Pass "." for @name if @dirfd is the directory to iterate over.
 *
 * The cb function is called with the DIR in use and the name of the
 * file in that directory.  If the file is to be opened it should
 * use the openat, fstatat, and related fns.
 *
 * Returns: 0 on success, else -1 and errno is set to the error code
 */
int _aa_dirat_for_each(int dirfd, const char *name, void *data,
		       int (* cb)(int, const char *, struct stat *, void *))
{
	autofree struct dirent **namelist = NULL;
	autoclose int cb_dirfd = -1;
	int i, num_dirs, rc;

	if (!cb || !name) {
		errno = EINVAL;
		return -1;
	}

	cb_dirfd = openat(dirfd, name, O_RDONLY | O_CLOEXEC | O_DIRECTORY);
	if (cb_dirfd == -1) {
		PDEBUG("could not open directory '%s': %m\n", name);
		return -1;
	}

	num_dirs = scandirat(cb_dirfd, ".", &namelist,
			     dot_or_dot_dot_filter, NULL);
	if (num_dirs == -1) {
		PDEBUG("scandirat of directory '%s' failed: %m\n", name);
		return -1;
	}

	for (rc = 0, i = 0; i < num_dirs; i++) {
		/* Must cycle through all dirs so that each one is autofreed */
		autofree struct dirent *dir = namelist[i];
		struct stat my_stat;

		if (rc)
			continue;

		if (fstatat(cb_dirfd, dir->d_name, &my_stat, 0)) {
			PDEBUG("stat failed for '%s': %m\n", dir->d_name);
			rc = -1;
			continue;
		}

		if (cb(cb_dirfd, dir->d_name, &my_stat, data)) {
			PDEBUG("dir_for_each callback failed for '%s'\n",
			       dir->d_name);
			rc = -1;
			continue;
		}
	}

	return rc;
}
