/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the aliasfile director */

/* Structure for the private options. The first block of them must be the
common director fields for all directors that use shared functions. */

typedef struct {
  COMMON_DIRECTOR_FIELDS;
  char *syntax_errors_to;
  char *syntax_errors_text;
  BOOL  forbid_include;
  BOOL  forbid_special;
  BOOL  qualify_preserve_domain;
  BOOL  freeze_missing_include;
  BOOL  rewrite;
  BOOL  skip_syntax_errors;
} aliasfile_director_options_block;

/* Data for reading the private options. */

extern optionlist aliasfile_director_options[];
extern int aliasfile_director_options_count;

/* Block containing default values. */

extern aliasfile_director_options_block aliasfile_director_option_defaults;

/* The main and initialization entry points */

extern int aliasfile_director_entry(director_instance *, address_item *,
  address_item **, address_item **, address_item **, address_item **, BOOL);
extern void aliasfile_director_init(director_instance *);

/* End of directors/aliasfile.h */
