/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the nisplus lookup */

extern void *nisplus_open(char *, char **);
extern int   nisplus_find(void *, char *, char *, int, char **, char **);
extern char *nisplus_quote(char *);

/* End of lookups/nisplus.h */
