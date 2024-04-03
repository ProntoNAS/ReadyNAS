/*
** yp_msg.c  Message reporting for ypserv and utils
**
** Copyright (c) 1996, 1997, 1999 Thorsten Kukuk, Germany
**
** This file is part of the NYS YP Server.
**
** The NYS YP Server is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** The NYS YP Server is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** You should have received a copy of the GNU General Public
** License along with the NYS YP Server; see the file COPYING.  If
** not, write to the Free Software Foundation, Inc., 675 Mass Ave,
** Cambridge, MA 02139, USA.
**
** Author: Thorsten Kukuk <kukuk@suse.de>
**
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include "yp_msg.h"

/*
** Reports a message to stderr, if in debug mode, else to syslog
*/

void 
yp_msg (char *fmt,...)
{
  va_list ap;
#ifndef HAVE_VSYSLOG
  char msg[512];
#endif

  va_start (ap, fmt);
  if (debug_flag)
    vfprintf (stderr, fmt, ap);
  else
    {
#ifndef HAVE_VSYSLOG
      vsprintf (msg, fmt, ap);
      syslog (LOG_NOTICE, msg);
#else
      vsyslog (LOG_NOTICE, fmt, ap);
#endif
    }
  va_end (ap);
}
