/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the dnsdb lookup */

extern void *dnsdb_open(char *, char **);
extern int   dnsdb_find(void *, char *, char *, int, char **, char **);

/* End of lookups/dnsdb.h */
