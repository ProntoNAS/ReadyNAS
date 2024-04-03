/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Structure for the private options. The first block of them must be the
common director fields for all directors that use shared functions. */

typedef struct {
  COMMON_DIRECTOR_FIELDS;
  char *new_address;
  BOOL  panic_expansion_fail;
  BOOL  qualify_preserve_domain;
  BOOL  rewrite;
} smartuser_director_options_block;

/* Data for reading the private options. */

extern optionlist smartuser_director_options[];
extern int smartuser_director_options_count;

/* Block containing default values. */

extern smartuser_director_options_block smartuser_director_option_defaults;

/* The main and initialization entry points for the director */

extern int smartuser_director_entry(director_instance *, address_item *,
  address_item **, address_item **, address_item **, address_item **, BOOL);

extern void smartuser_director_init(director_instance *);

/* End of directors/smartuser.h */
