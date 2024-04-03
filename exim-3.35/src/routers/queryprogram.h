/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* Private structure for the private options. */

typedef struct {
  char *command;
  int timeout;
  uid_t uid;
  gid_t gid;
  BOOL uid_set;
  BOOL gid_set;
  char *current_directory;
} queryprogram_router_options_block;

/* Data for reading the private options. */

extern optionlist queryprogram_router_options[];
extern int queryprogram_router_options_count;

/* Block containing default values. */

extern queryprogram_router_options_block queryprogram_router_option_defaults;

/* The main and initialization entry points for the router */

extern int queryprogram_router_entry(router_instance *, address_item *,
  address_item **, address_item **, BOOL, char *);

extern void queryprogram_router_init(router_instance *);

/* End of routers/queryprogram.h */
