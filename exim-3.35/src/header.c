/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "exim.h"


/*************************************************
*            Add new header on end of chain      *
*************************************************/

/* header_last points to the last header during message reception and delivery;
otherwise it is NULL. We add new headers only when header_last is not NULL.
The function may get called sometimes when it is NULL (e.g. during address
verification where rewriting options exist). Headers are always added in the
main storage pool.

Arguments:
  type      Exim header type character
  format    sprintf format
  ...       arguments for the format

Returns:    nothing
*/

void
header_add(int type, char *format, ...)
{
int pool_reset = store_pool;
header_line *new;
va_list ap;
char buffer[8192];

if (header_last == NULL) return;

va_start(ap, format);
if (!string_vformat(buffer, sizeof(buffer), format, ap))
  log_write(0, LOG_PANIC_DIE, "string too long in header_add: %.100s ...",
    buffer);
va_end(ap);

store_pool = POOL_MAIN;
new = store_get(sizeof(header_line));
new->text = string_copy(buffer);
store_pool = pool_reset;

new->slen = (int)strlen(buffer);
new->next = NULL;
new->type = type;

header_last->next = new;
header_last = new;
}


/*************************************************
*          Check the name of a header            *
*************************************************/

/* This function is necessary because the colon is allowed to be separated from
the name by white space. If it were not, a simple strncmpic would do.

Arguments:
  h         points to the header line
  name      points to the header name to be checked, excluding the :
  len       length of the name

Returns:    TRUE if the header starts with the name, followed by optional
            white space and a colon
*/

BOOL
header_checkname(header_line *h, char *name, int len)
{
char *text = h->text;
while (len-- > 0) if (tolower(*text++) != tolower(*name++)) return FALSE;
while (isspace((uschar)*text)) text++;
return (*text == ':');
}

/* End of header.c */
