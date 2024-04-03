/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "dbmdb.h"


/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description */

void *
dbmdb_open(char *filename, char **errmsg)
{
EXIM_DB *yield;
EXIM_DBOPEN(filename, O_RDONLY, 0, &yield);
if (yield == NULL)
  {
  int save_errno = errno;
  *errmsg = string_open_failed(errno, "%s as a %s file", filename, EXIM_DBTYPE);
  errno = save_errno;
  }
return yield;
}



/*************************************************
*             Check entry point                  *
*************************************************/

/* This needs to know more about the underlying files than is good for it!
We need to know what the real file names are in order to check the owners and
modes. If USE_DB is set, we know it is Berkeley DB, which uses an unmodified
file name. If USE_TDB is set, we know it is tdb, which does the same.
Otherwise, for safety, we have to check for x.db or x.dir and x.pag. */

BOOL
dbmdb_check(void *handle, char *filename, int modemask, uid_t *owners,
  gid_t *owngroups, char **errmsg)
{
int rc;
handle = handle;    /* Keep picky compilers happy */

#if defined(USE_DB) || defined(USE_TDB)
rc = search_check_file(-1, filename, modemask, owners, owngroups,
  "dbm", errmsg);
#else
  {
  char filebuffer[256];
  sprintf(filebuffer, "%.250s.db", filename);
  rc = search_check_file(-1, filebuffer, modemask, owners, owngroups,
    "dbm", errmsg);
  if (rc < 0)        /* stat() failed */
    {
    sprintf(filebuffer, "%.250s.dir", filename);
    rc = search_check_file(-1, filebuffer, modemask, owners, owngroups,
      "dbm", errmsg);
    if (rc == 0)     /* x.dir was OK */
      {
      sprintf(filebuffer, "%.250s.pag", filename);
      rc = search_check_file(-1, filebuffer, modemask, owners, owngroups,
        "dbm", errmsg);
      }
    }
  }
#endif

return rc == 0;
}



/*************************************************
*              Find entry point                  *
*************************************************/

/* See local README for interface description. This function adds 1 to
the keylength in order to include the terminating zero. */

int
dbmdb_find(void *handle, char *filename, char *keystring, int length,
  char **result, char **errmsg)
{
EXIM_DB *d = (EXIM_DB *)handle;
EXIM_DATUM key, data;

filename = filename;    /* Keep picky compilers happy */
errmsg = errmsg;

EXIM_DATUM_INIT(key);               /* Some DBM libraries require datums to */
EXIM_DATUM_INIT(data);              /* be cleared before use. */
EXIM_DATUM_DATA(key) = keystring;
EXIM_DATUM_SIZE(key) = length + 1;

if (EXIM_DBGET(d, key, data))
  {
  *result = string_copyn(EXIM_DATUM_DATA(data), EXIM_DATUM_SIZE(data));
  EXIM_DATUM_FREE(data);            /* Some DBM libraries need a free() call */
  return OK;
  }
return FAIL;
}



/*************************************************
*      Find entry point - no zero on key         *
*************************************************/

/* See local README for interface description */

int
dbmnz_find(void *handle, char *filename, char *keystring, int length,
  char **result, char **errmsg)
{
return dbmdb_find(handle, filename, keystring, length-1, result, errmsg);
}



/*************************************************
*              Close entry point                 *
*************************************************/

/* See local README for interface description */

void
dbmdb_close(void *handle)
{
EXIM_DBCLOSE((EXIM_DB *)handle);
}

/* End of lookups/dbmdb.c */
