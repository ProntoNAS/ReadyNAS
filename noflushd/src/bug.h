/*
 * <bug.h>
 * Debugging aids.
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: bug.h,v 1.1.1.1 2010-09-21 01:09:51 jmaggard Exp $
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
 */             

#ifndef _NFD_BUG_H
#define _NFD_BUG_H

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>

#include "noflushd.h"
#include "kupdate.h"

/* FIXME: ... is a gcc extension. Get rid of that! */
/* Note: The whitespace between __LINE__ and the following comma is subtle!
 * If you leave it out, cpp will eat not only the comma but also the __LINE__
 * macro if ##x expands to nothing. Same goes for all the other ##x args 
 * below.
 */
#define BUG(msg,x...)	do { \
				if (released) \
					syslog(LOG_DEBUG, "BUG: " msg , ##x); \
				else \
					fprintf(stderr, "BUG at %s line %d: " \
					        msg "\n", __FILE__, __LINE__ , \
					        ##x); \
				kupdate->start(kupdate); \
				raise(11); \
			} while (0)

#define DEBUG(msg,x...)	do { if (verbose > 1 && !released) \
				fprintf(stderr, msg "\n" , ##x); \
			} while (0)
#define INFO(msg,x...)	do { if (verbose) { \
				if (released) \
					syslog(LOG_NOTICE, msg , ##x); \
			     	else \
					printf(msg "\n" , ##x); \
			     } \
			} while (0)
#define ERR(msg,x...)	do { if (released) \
				syslog(LOG_ERR, msg ": %m" , ##x); \
			     else \
				fprintf(stderr, "Error: " msg "\n" , ##x); \
			} while (0)
#endif
