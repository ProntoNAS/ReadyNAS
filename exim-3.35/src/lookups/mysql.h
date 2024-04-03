/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the mysql lookup functions */

extern void *mysql_open(char *, char **);
extern int   mysql_find(void *, char *, char *, int, char **, char **);
extern void  mysql_tidy(void);
extern char *mysql_quote(char *);

/* End of lookups/mysql.h */
