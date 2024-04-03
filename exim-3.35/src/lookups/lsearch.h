/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the lsearch lookup */

extern void *lsearch_open(char *, char **);
extern BOOL  lsearch_check(void *, char *, int, uid_t *, gid_t *, char **);
extern int   lsearch_find(void *, char *, char *, int, char **, char **);
extern void  lsearch_close(void *);

/* End of lookups/lsearch.h */
