/*
 * libdpkg - Debian packaging suite library routines
 * file.h - file handling routines
 *
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

#ifndef LIBDPKG_FILE_H
#define LIBDPKG_FILE_H

#include <stdbool.h>

#include <dpkg/macros.h>

DPKG_BEGIN_DECLS

/*
 * Copy file ownership and permissions from one file to another.
 */
void file_copy_perms(const char *src, const char *dst);

bool file_is_locked(int lockfd, const char *filename);
void file_lock(int *lockfd, const char *filename,
               const char *emsg, const char *emsg_eagain);
void file_unlock(void);

DPKG_END_DECLS

#endif /* LIBDPKG_FILE_H */

