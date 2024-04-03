/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the testdb lookup */

extern void *testdb_open(char *, char **);
extern int   testdb_find(void *, char *, char *, int, char **, char **);

/* End of lookups/testdb.h */
