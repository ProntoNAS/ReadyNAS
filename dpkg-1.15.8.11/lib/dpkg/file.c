/*
 * libdpkg - Debian packaging suite library routines
 * file.c - file handling functions
 *
 * Copyright © 1994, 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 * Copyright © 2008 Guillem Jover <guillem@debian.org>
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <dpkg/dpkg.h>
#include <dpkg/i18n.h>
#include <dpkg/file.h>

void
file_copy_perms(const char *src, const char *dst)
{
	struct stat stab;

	if (stat(src, &stab) == -1) {
		if (errno == ENOENT)
			return;
		ohshite(_("unable to stat source file '%.250s'"), src);
	}

	if (chown(dst, stab.st_uid, stab.st_gid) == -1)
		ohshite(_("unable to change ownership of target file '%.250s'"),
		        dst);

	if (chmod(dst, (stab.st_mode & 07777)) == -1)
		ohshite(_("unable to set mode of target file '%.250s'"), dst);
}

static void
file_lock_setup(struct flock *fl, short type)
{
	fl->l_type = type;
	fl->l_whence = SEEK_SET;
	fl->l_start = 0;
	fl->l_len = 0;
	fl->l_pid = 0;
}

static void
file_unlock_cleanup(int argc, void **argv)
{
	int lockfd = *(int*)argv[0];
	struct flock fl;

	assert(lockfd >= 0);

	file_lock_setup(&fl, F_UNLCK);

	if (fcntl(lockfd, F_SETLK, &fl) == -1)
		ohshite(_("unable to unlock dpkg status database"));
}

void
file_unlock(void)
{
	pop_cleanup(ehflag_normaltidy); /* Calls file_unlock_cleanup. */
}

/**
 * Check if a file has a lock acquired.
 *
 * @param lockfd The file descriptor for the lock.
 * @param filename The file name associated to the file descriptor.
 */
bool
file_is_locked(int lockfd, const char *filename)
{
	struct flock fl;

	file_lock_setup(&fl, F_WRLCK);

	if (fcntl(lockfd, F_GETLK, &fl) == -1)
		ohshit(_("unable to check file '%s' lock status"), filename);

	if (fl.l_type == F_WRLCK && fl.l_pid != getpid())
		return true;
	else
		return false;
}

/* lockfd must be allocated statically as its addresses is passed to
 * a cleanup handler. */
void
file_lock(int *lockfd, const char *filename,
          const char *emsg, const char *emsg_eagain)
{
	struct flock fl;

	setcloexec(*lockfd, filename);

	file_lock_setup(&fl, F_WRLCK);

	if (fcntl(*lockfd, emsg_eagain ? F_SETLK : F_SETLKW, &fl) == -1) {
		if (emsg_eagain && (errno == EACCES || errno == EAGAIN))
			ohshit(emsg_eagain);
		ohshite(emsg);
	}

	push_cleanup(file_unlock_cleanup, ~0, NULL, 0, 1, lockfd);
}

