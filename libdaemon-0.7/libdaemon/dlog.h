#ifndef foodaemonloghfoo
#define foodaemonloghfoo

/* $Id: dlog.h 71 2004-12-18 23:29:50Z lennart $ */

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

#include <syslog.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file
 *
 * Contains a robust API for logging messages
 */

/** Specifies where to send the log messages to
 */
enum daemon_log_flags {
    DAEMON_LOG_SYSLOG = 1,   /**< Log messages are written to syslog */
    DAEMON_LOG_STDERR = 2,   /**< Log messages are written to STDERR */
    DAEMON_LOG_STDOUT = 4,   /**< Log messages are written to STDOUT */
    DAEMON_LOG_AUTO = 8      /**< If this is set a daemon_fork() will
                                  change this to DAEMON_LOG_SYSLOG in
                                  the daemon process. */
};

/** This variable is used to specify the log target(s) to use. Defaults to DAEMON_LOG_STDERR|DAEMON_LOG_AUTO */
extern enum daemon_log_flags daemon_log_use;

/** Specifies the syslog identification, use daemon_ident_from_argv0()
 * to set this to a sensible value or generate your own. */
extern char* daemon_log_ident;

/** Log a message using printf format strings using the specified syslog priority
 * @param prio The syslog priority (PRIO_xxx constants)
 * @param t,... The text message to log
 */
void daemon_log(int prio, const char* t, ...);

/** Return a sensible syslog identification for daemon_log_ident
 * generated from argv[0]. This will return a pointer to the file name
 * of argv[0], i.e. strrchr(argv[0], '\')+1
 * @param argv0 argv[0] as passed to main()
 * @return The identification string
 */
char *daemon_ident_from_argv0(char *argv0);

#ifdef __cplusplus
}
#endif

#endif
