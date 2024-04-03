/*
 * <util.h>
 * Convenience functions for noflushd.
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: util.h,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
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

#ifndef _NFD_UTIL_H
#define _NFD_UTIL_H

/* Read from file pointer fp until next newline or EOF. Storage is
 * dynamically allocated. */
char *get_line(FILE *fp);
/* Release dynamic storage associated with line. */
void release_line(char *line);
/* Advance to next line in file. */
void next_line(FILE *fp);
/* Get num-th space delimited entry in string str. While _get_entry modifies
 * str, get_entry creates a private copy that must be freed via release_entry.
 * Beware: It's an extremely bad idea to _get_entry on a str in ascending
 * order - the str will be shortened to the first entry looked up.
 */
char *_get_entry(char *str, int num);
char *get_entry(char *str, int num);
/* Release dynamic storage associated with entry. */
void release_entry(char *str);
/* Get rest of line, starting with n-th entry. While get_remains() returns a
 * private copy that must be released afterwards, _get_remains() returns a
 * pointer into str. get_remains() strips the final newline.
 */
char *_get_remains(char *str, int num);
char *get_remains(char *str, int num);
void release_remains(char *str);

#endif
