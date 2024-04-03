/*
 * <intr_stat.h>
 * Gather statistical data for interrupts.
 * 
 * (C) 2001 Laurent Pelecq <laurent.pelecq@soleil.org>
 * 
 * $Id: intr_stat.h,v 1.1.1.1 2010-09-21 01:09:51 jmaggard Exp $
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

#ifndef _INTR_STAT_H
#define _INTR_STAT_H

int intr_stat_init(void);
void intr_set_timeout(int);
void intr_stat_release(void);
int intr_stat_register(int);
int intr_stat_register_byids(const char *);
int intr_stat_check_idleness(void);

#endif
