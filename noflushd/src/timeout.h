/*
 * <timeout.h>
 * 
 * (C) 2000 Daniel Kobras <kobras@linux.de>
 * 
 * $Id: timeout.h,v 1.1.1.1 2006-09-14 22:54:14 jmaggard Exp $
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

#ifndef _NFD_TIMEOUT_H
#define _NFD_TIMEOUT_H

typedef struct tolist_s *tolist_t;

/* Parse list of comma-separated timeout entries and convert into tolist.
 * Head of tolist is returned. Timeouts may be given as positive numbers
 * or 0 and -1, indicating 'skip disk' and 'use default timeout' respectively.
 * Alternatively to 0 and -1, the words "skip" and "default" may be used.
 * If the flag is_default is non-zero, the internal list of default timeouts
 * is set. -1 and "default" are not allowed in this case.
 */
tolist_t timeout_parse(tolist_t head, char *args, int is_default);
/* Get current timeout in timeout list. */
int timeout_get(tolist_t head);
/* Advance to next default timeout in list. */
void timeout_advance_default(void);
/* Advance to next timeout in list. Returns difference between new and old
 * timeout, or new timeout if previous timeout was set to special value
 * 'skip'. */
int timeout_advance(tolist_t *head);

#define NFD_TO_SKIP		0
#define NFD_TO_USE_DEFAULT	-1	/* Not externally visible. */
#define NFD_TO_IS_REGULAR(x)	((x) > 0)
#define NFD_TO_IS_VALID(x)	(!((x) < 0))
#define NFD_TO_DO_SKIP(x)	((x) == 0)

#endif
