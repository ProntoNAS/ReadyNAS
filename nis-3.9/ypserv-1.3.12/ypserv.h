#ifndef __YPSERV_H__
#define __YPSERV_H__

#include <rpc/rpc.h>

/* Struct for ypserv.conf options */
typedef struct conffile
{
  struct in_addr netmask;
  struct in_addr network;
  char *map;
  int security;
  int mangle;
  int field;
  struct conffile *next;
} conffile_t;

extern char *progname;      	/* ypserv.c */
extern char *path_ypdb;		/* ypserv.c */
extern int debug_flag;		/* ypserv.c || ypxfrd.c */
extern int dns_flag;		/* ypserv.c || ypxfrd.c */
extern volatile int children;		/* server.c || ypxfrd.c */
extern int forked;		/* server.c || ypxfrd.c */
extern int xfr_check_port;	/* server.c || ypxfrd.c */

/*
** returns 1, if secure, else 0 or -1, if domain is not valid
*/
extern int is_valid_host(struct svc_req *rqstp, const char *map,
			 const char *domain);
/*
** returns 1, if secure, else 0
*/
extern int is_valid_domain(const char *domain);

/*
** load/reload the YPMAPDIR/securenets file
*/
extern void load_securenets(void);
extern void load_config(void);
extern conffile_t *load_ypserv_conf(const char *);

extern void ypserv_svc_run(void);

/*
** Defines for ypserv.conf
*/

/* Security field: */
#define SEC_NONE 0
#define SEC_DENY 1
#define SEC_PORT 2
#define SEC_DES  3

/* Mangle field: */
#define MANGLE_NO 0
#define MANGLE_YES 1

#endif
