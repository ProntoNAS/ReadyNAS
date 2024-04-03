#ifndef foodexechfoo
#define foodexechfoo

/* $Id: exec.h 4 2003-08-10 19:56:53Z lennart $ */

/*
 * This file is part of libdaemon.
 *
 * libdaemon is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * libdaemon is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdaemon; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** \file
 *
 * Contains a robust API for running sub processes with STDOUT and
 * STDERR redirected to syslog
 */

/** If this variable is defined to 1 iff daemon_exec() is supported.*/
#define DAEMON_EXEC_AVAILABLE 1

/** Run the specified executable with the specified arguments in the
 * specified directory and return the return value of the program in
 * the specified pointer. The calling process is blocked until the
 * child finishes and all child output (either STDOUT or STDIN) has
 * been written to syslog.
 * 
 * @param dir Working directory for the process.
 * @param ret A pointer to an integer to write the return value of the program to.
 * @param prog The path to the executable
 * @param ... The arguments to be passed to the program, followed by a (char *) NULL
 * @return Nonzero on failure, zero on success
 */
int daemon_exec(const char *dir, int *ret, const char *prog, ...);

#ifdef __cplusplus
}
#endif

#endif
