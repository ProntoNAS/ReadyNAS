/*
 *  acpid.h - ACPI daemon
 *
 *  Copyright (C) 1999-2000 Andrew Henroid
 *  Copyright (C) 2001 Sun Microsystems (thockin@sun.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ACPID_H__
#define ACPID_H__

#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ACPI_PROCDIR 		"/proc/acpi"
#define ACPI_EVENTFILE		ACPI_PROCDIR "/event"
#define ACPI_CONFDIR		"/etc/acpi/events"
#define ACPI_LOGFILE		"/var/log/acpid"
#define ACPI_SOCKETFILE		"/var/run/acpid.socket"
#define ACPI_SOCKETMODE		0666
#define ACPI_MAX_ERRS		5

#define PACKAGE 		"acpid"

/*
 * acpid.c
 */
extern int acpid_debug;
extern int acpid_log(const char *fmt, ...);

/*
 * event.c
 */
extern int acpid_read_conf(const char *confdir);
extern int acpid_add_client(int client, const char *origin);
extern int acpid_cleanup_rules(void);
extern int acpid_handle_event(const char *event);

#endif /* ACPID_H__ */
