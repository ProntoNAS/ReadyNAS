/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the ldap lookups */

extern void *eldap_open(char *, char **);
extern int   eldap_find(void *, char *, char *, int, char **, char **);
extern int   eldapdn_find(void *, char *, char *, int, char **, char **);
extern int   eldapm_find(void *, char *, char *, int, char **, char **);
extern void  eldap_tidy(void);
extern char *eldap_quote(char *);

#define LDAP_LIB_OPENLDAP2

#ifdef LDAP_LIB_OPENLDAP2
#define LDAP_LIB_NETSCAPE
#endif

/* End of lookups/ldap.h */
