/*
 * <pidfile.h>
 *
 * Copyright (C) 1999, 2000 Daniel Kobras
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * $Id: pidfile.h,v 1.1 2000/10/17 18:15:43 nold Exp $
 *
 */

#ifndef _NFD_PIDFILE_H
#define _NFD_PIDFILE_H

#define NOFLUSHD_PID_FILE "/var/run/noflushd.pid"

typedef struct pidfile_s *pidfile_t;

/*
 * Save our pid to pidfile. Returns NULL if file exists and
 * another noflushd is running with indicated pid, or on any
 * error.
 */
pidfile_t pidfile_get(char *pidfile);

/*
 * Remove pidfile.
 */
void pidfile_drop(pidfile_t);

#endif
