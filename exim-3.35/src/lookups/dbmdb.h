/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the dbm lookup. Use dbmdb in the code to avoid name
clashes with external library names. */

extern void *dbmdb_open(char *, char **);
extern BOOL  dbmdb_check(void *, char *, int, uid_t *, gid_t *, char **);
extern int   dbmdb_find(void *, char *, char *, int, char **, char **);
extern int   dbmnz_find(void *, char *, char *, int, char **, char **);
extern void  dbmdb_close(void *);

/* End of lookups/dbmdb.h */
