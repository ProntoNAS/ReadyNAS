/*
 * libdpkg - Debian packaging suite library routines
 * string.c - string handling routines
 *
 * Copyright © 1995 Ian Jackson <ian@chiark.greenend.org.uk>
 * Copyright © 2008, 2009 Guillem Jover <guillem@debian.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <compat.h>

#include <string.h>

#include <dpkg/string.h>

char *
str_escape_fmt(char *dst, const char *src)
{
	char *d = dst;
	const char *s = src;

	while (*s) {
		if (*s == '%')
			*d++ = '%';
		*d++ = *s++;
	}

	*d = '\0';

	return d;
}

/* Check and strip possible surrounding quotes in string. */
char *
str_strip_quotes(char *str)
{
	if (str[0] == '"' || str[0] == '\'') {
		size_t str_len = strlen(str);

		if (str[0] != str[str_len - 1])
			return NULL;

		/* Remove surrounding quotes. */
		str[str_len - 1] = '\0';
		str++;
	}

	return str;
}

