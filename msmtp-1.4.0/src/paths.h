/*
 * paths.h
 *
 * This file is part of msmtp, an SMTP client.
 *
 * Copyright (C) 2004, 2005
 * Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   msmtp is released under the GPL with the additional exemption that
 *   compiling, linking, and/or using OpenSSL is allowed.
 */

#ifndef PATHS_H
#define PATHS_H


/* All 'errstr' arguments must point to buffers that are at least
 * 'errstr_bufsize' characters long.
 * If a function with an 'errstr' argument returns a value != PATHS_EOK,
 * 'errstr' will contain an error message (maybe just "").
 * If such a function returns PATHS_EOK, 'errstr' will not be changed.
 */
extern const size_t errstr_bufsize;
#define PATHS_EOK		0	/* no error */
#define PATHS_ENOHOME		1	/* Cannot get user's directory */
#define PATHS_ECANTCREAT	2	/* Cannot create file */
#define PATHS_EINSECURE		3	/* file security check failed */
#define PATHS_EUNKNOWN		4	/* unknown error */

/*
 * get_prgname()
 *
 * Get the program name from an argv[0]-like string.
 * Returns a pointer to a static buffer.
 */
char *get_prgname(char *argv0);

/*
 * get_default_system_conffile()
 *
 * Get the default filename of the system configuration file.
 * It consists of a system configuration directory (OS dependent; SYSCONFDIR
 * on UNIX) plus 'base'. (Example: /etc/msmtprc; base is "msmtprc")
 * Returns a pointer to an  allocated string.
 * Cannot fail (uses a safe fallback as a last resort).
 */
#ifdef SYSCONFDIR
char *get_default_system_conffile(const char *base);
#endif

/*
 * get_default_user_conffile()
 *
 * Get the default filename of the user configuration file.
 * It consists of a user directory (OS dependent; $HOME on UNIX) 
 * plus 'base'. (Example: $HOME/.msmtprc; base is ".msmtprc")
 * Returns a pointer to an  allocated string in 'dst'.
 * Used error codes: PATHS_ENOHOME
 */
int get_default_user_conffile(const char *base, char **dst, char *errstr);

/*
 * expand_tilde()
 *
 * If the first character of 'filename' is '~', it will be replaced by
 * the user's home directory, and the resulting string will be returned
 * in 'new_filename'.
 * If the first character of 'filename' is not '~', the returned string 
 * will simply be a copy of 'filename'.
 * Used error codes: PATHS_ENOHOME
 */
int expand_tilde(const char *filename, char **new_filename, char *errstr);

/*
 * tempfile()
 *
 * Create a temporary file, only accessible by the current user (if applicable
 * given the platform; on UNIX this means mode 0600). The file will be created
 * in $TMPDIR or, if this variable is unset, in a system specific directory for
 * temporary files. It will be automatically deleted when closed.
 * 'base' is a suggestion for the file name prefix. It must only contain ASCII
 * characters that are safe for filenames on all systems. This function may
 * ignore this suggestion.
 * The resulting stream will be returned in 'f' ('f' will be NULL on errors).
 * Used error codes: PATHS_ECANTCREAT
 */
int tempfile(const char *base, FILE **f, char *errstr);

/*
 * check_secure()
 *
 * Checks whether the given file
 * - is a regular file
 * - is owned by the current user
 * - has permissions no more than 0600
 * The file name is not included in error strings, so that the caller can use
 * "%s: %s", pathname, errstr as an error string.
 * Used error codes: PATHS_EINSECURE, PATHS_EUNKNOWN
 */
int check_secure(const char *pathname, char *errstr);

#endif
