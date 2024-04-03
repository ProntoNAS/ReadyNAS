/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the forwardfile director */

/* Private structure for the private options. */

typedef struct {
  COMMON_DIRECTOR_FIELDS;
  char *data;
  char *file_dir;
  char *pw_dir;
  char *syntax_errors_to;
  char *syntax_errors_text;
  BOOL  check_group;
  BOOL  filter;
  BOOL  allow_system_actions;
  BOOL  forbid_filter_exists;
  BOOL  forbid_filter_logwrite;
  BOOL  forbid_filter_lookup;
  BOOL  forbid_filter_perl;
  BOOL  forbid_include;
  BOOL  ignore_eacces;
  BOOL  ignore_enotdir;
  BOOL  qualify_preserve_domain;
  BOOL  use_seteuid;
  BOOL  freeze_missing_include;
  BOOL  rewrite;
  BOOL  skip_syntax_errors;
} forwardfile_director_options_block;

/* Data for reading the private options. */

extern optionlist forwardfile_director_options[];
extern int forwardfile_director_options_count;

/* Block containing default values. */

extern forwardfile_director_options_block forwardfile_director_option_defaults;

/* The main and initialization entry points for the director */

extern int forwardfile_director_entry(director_instance *, address_item *,
  address_item **, address_item **, address_item **, address_item **, BOOL);

extern void forwardfile_director_init(director_instance *);

/* End of directors/forwardfile.h */
