/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the nis and nis0 lookups */

extern void *nis_open(char *, char **);
extern int   nis_find(void *, char *, char *, int, char **, char **);
extern int   nis0_find(void *, char *, char *, int, char **, char **);

/* End of lookups/nis.h */
