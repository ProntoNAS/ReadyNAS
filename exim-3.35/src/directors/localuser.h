/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  char *pw_dir;
} localuser_director_options_block;

/* Data for reading the private options. */

extern optionlist localuser_director_options[];
extern int localuser_director_options_count;

/* Block containing default values. */

extern localuser_director_options_block localuser_director_option_defaults;

/* The main and initialization entry points for the director */

extern int localuser_director_entry(director_instance *, address_item *,
  address_item **, address_item **, address_item **, address_item **, BOOL);

extern void localuser_director_init(director_instance *);

/* End of directors/localuser.h */
