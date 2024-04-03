/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "testdb.h"


/* These are not real lookup functions; they are just a way of testing the
rest of Exim by providing an easy way of specifying particular yields from
the find function. */


/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description. */

void *
testdb_open(char *filename, char **errmsg)
{
filename = filename;   /* Keep picky compilers happy */
errmsg = errmsg;
return (void *)(1);    /* Just return something non-null */
}



/*************************************************
*               Find entry point                 *
*************************************************/

/* See local README for interface description. */

int
testdb_find(void *handle, char *filename, char *query, int length,
  char **result, char **errmsg)
{
handle = handle;          /* Keep picky compilers happy */
filename = filename;
length = length;

if (strcmp(query, "fail") == 0)
  {
  *errmsg = "testdb lookup forced FAIL";
  return FAIL;
  }
if (strcmp(query, "defer") == 0)
  {
  *errmsg = "testdb lookup forced DEFER";
  return DEFER;
  }
*result = string_copy(query);
return OK;
}

/* End of lookups/testdb.c */
