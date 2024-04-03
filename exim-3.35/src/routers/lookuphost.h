/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  BOOL check_secondary_mx;
  BOOL gethostbyname;
  BOOL qualify_single;
  BOOL search_parents;
  BOOL rewrite_headers;
  char *widen_domains;
  char *mx_domains;
} lookuphost_router_options_block;

/* Data for reading the private options. */

extern optionlist lookuphost_router_options[];
extern int lookuphost_router_options_count;

/* Block containing default values. */

extern lookuphost_router_options_block lookuphost_router_option_defaults;

/* The main and initialization entry points for the router */

extern int lookuphost_router_entry(router_instance *, address_item *,
  address_item **, address_item **, BOOL, char *);

extern void lookuphost_router_init(router_instance *);

/* End of routers/lookuphost.h */
