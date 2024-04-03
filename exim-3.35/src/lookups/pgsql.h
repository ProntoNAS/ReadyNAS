/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the pgsql lookup functions */

extern void *pgsql_open(char *, char **);
extern int   pgsql_find(void *, char *, char *, int, char **, char **);
extern void  pgsql_tidy(void);
extern char *pgsql_quote(char *);

/* End of lookups/pgsql.h */
