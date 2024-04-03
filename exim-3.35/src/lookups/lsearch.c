/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "lsearch.h"



/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description */

void *
lsearch_open(char *filename, char **errmsg)
{
FILE *f = fopen(filename, "r");
if (f == NULL)
  {
  int save_errno = errno;
  *errmsg = string_open_failed(errno, "%s for linear search", filename);
  errno = save_errno;
  return NULL;
  }
return f;
}



/*************************************************
*             Check entry point                  *
*************************************************/

BOOL
lsearch_check(void *handle, char *filename, int modemask, uid_t *owners,
  gid_t *owngroups, char **errmsg)
{
return search_check_file(fileno((FILE *)handle), filename, modemask,
  owners, owngroups, "lsearch", errmsg) == 0;
}



/*************************************************
*              Find entry point                  *
*************************************************/

/* See local README for interface description */

int
lsearch_find(void *handle, char *filename, char *keystring, int length,
  char **result, char **errmsg)
{
FILE *f = (FILE *)handle;
char buffer[4096];

filename = filename;  /* Keep picky compilers happy */
errmsg = errmsg;

rewind(f);
while (fgets(buffer, sizeof(buffer), f) != NULL)
  {
  int ptr, size;
  int p = (int)strlen(buffer);
  char *yield;
  char *s = buffer;

  while (p > 0 && isspace((uschar)buffer[p-1])) p--;
  buffer[p] = 0;

  if (buffer[0] == 0 || buffer[0] == '#' || isspace((uschar)buffer[0]))
    continue;
  while (*s != 0 && *s != ':' && !isspace((uschar)*s)) s++;

  if (s-buffer != length || strncmpic(buffer, keystring, length) != 0)
    continue;

  while (isspace((uschar)*s)) s++;
  if (*s == ':')
    {
    s++;
    while (isspace((uschar)*s)) s++;
    }

  size = 100;
  ptr = 0;
  yield = store_get(size);
  if (*s != 0)
    yield = string_cat(yield, &size, &ptr, s, (int)strlen(s));

  while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
    p = (int)strlen(buffer);
    while (p > 0 && isspace((uschar)buffer[p-1])) p--;
    buffer[p] = 0;
    if (buffer[0] == 0 || buffer[0] == '#') continue;
    if (!isspace((uschar)buffer[0])) break;
    s = buffer;
    while (isspace((uschar)*s)) s++;
    *(--s) = ' ';
    yield = string_cat(yield, &size, &ptr, s, (int)strlen(s));
    }

  yield[ptr] = 0;
  *result = yield;
  return OK;
  }

return FAIL;
}



/*************************************************
*              Close entry point                 *
*************************************************/

/* See local README for interface description */

void
lsearch_close(void *handle)
{
fclose((FILE *)handle);
}

/* End of lookups/lsearch.c */
