/*
 * <util.c>
 * Convenience functions for noflushd.
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: util.c,v 1.1.1.1 2010-09-21 01:09:51 jmaggard Exp $
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bug.h"
#include "util.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>



char *get_line(FILE *fp)
{
	char *c, *buf = NULL;
	const size_t inc = 1024;
	size_t	size = 0;

	if (!fp || feof(fp))
		return NULL;
	
	do {
		size += inc;
		buf = (char *) realloc(buf, size);
		if (!buf)
			goto _out;
		
		c = buf+(size-inc);
		if (!fgets(c, inc, fp) && size==inc) {
			free(buf);
			buf=NULL;
			goto _out;
		}
		size = strlen(buf);
	} while (!feof(fp) && buf[size-1] != '\n');
		
_out:
	return buf;
}

void release_line(char *line)
{
	free(line);
}

/* There's obvious room for optimisations here. */
void next_line(FILE *fp)
{
	release_line(get_line(fp));
}
static char *skip_space(char *str)
{
	while (*str && isspace((int) *str))
		str++;
	
	return str;
}

static char *skip_nospace(char *str)
{
	while (*str && !isspace((int) *str))
		str++;

	return str;
}

static char *__get_entry(char *str, int num)
{
	goto _in;
	
	while (*str && num--) {
		str = skip_nospace(str);
_in:
		str = skip_space(str);
	};

	return *str ? str : NULL;
}
	
char *_get_entry(char *str, int num)
{
	char *end;

	if (!(str = __get_entry(str, num)))
		return NULL;
	end = skip_nospace(str);
	*end = '\0';
	return str;
}

char *get_entry(char *str, int num)
{
	char *end, *newstr;
	size_t len;
	
	if (!(str = __get_entry(str, num)))
		return NULL;
	end = skip_nospace(str);
	len = end-str;
	newstr = malloc(len+1);
	if (!newstr)
		return NULL;
	strncpy(newstr, str, len);
	newstr[len] = '\0';
	return newstr;
}

void release_entry(char *str)
{
	free(str);
}

char *_get_remains(char *str, int num)
{
	return __get_entry(str, num);
}

char *get_remains(char *str, int num)
{
	char *start;
	int len;
	
	if (!(start = __get_entry(str, num)))
		return NULL;
	start = strdup(start);
	len = strlen(start);
	if (len > 0 && start[len-1]=='\n')
		start[len-1]='\0';

	return start;
}
	
void release_remains(char *str)
{
	free(str);
}
