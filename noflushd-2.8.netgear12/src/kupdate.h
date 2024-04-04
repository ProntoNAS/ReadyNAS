/*
 * <kupdate.h>
 * Interaction with kupdate kernel daemon.
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: kupdate.h,v 1.8 2004/03/01 15:19:35 nold Exp $
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

#include <stdio.h>
#include <sys/types.h>

#ifndef _NFD_KUPDATE_H
#define _NFD_KUPDATE_H

typedef struct kupdate_s *kupdate_t;
typedef void (*kupdate_startstop_t)(kupdate_t);
typedef long (*kupdate_fixupinterval_t)(long);

struct kupdate_s {
	kupdate_startstop_t	start;
	kupdate_startstop_t	stop;
	kupdate_fixupinterval_t	fixup_interval;
	int			stopped;
	long			interval;
	pid_t			pid;
	FILE			*f;
	FILE			*stat;
};

/* Initialise kupdate handle. */
kupdate_t kupdate_init(void);
/* Query kupdate wakeup interval. */
long kupdate_get_interval(kupdate_t);

/* Wrappers to start/stop kupdate daemon. */
static inline void kupdate_start(kupdate_t k)
{
	if (!k->stopped)
		return;
	k->start(k);
}

static inline void kupdate_stop(kupdate_t k)
{
	if (k->stopped)
		return;
	k->stop(k);
}

#endif
