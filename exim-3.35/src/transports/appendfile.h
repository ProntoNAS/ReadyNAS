/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  char *filename;
  char *dirname;
  char *prefix;
  char *suffix;
  char *create_file_string;
  char *quota;
  char *quota_size_regex;
  char *quota_warn_threshold;
  char *maildir_tag;
  char *mailstore_prefix;
  char *mailstore_suffix;
  char *check_string;
  char *escape_string;
  char *file_format;
  int   quota_value;
  int   quota_warn_threshold_value;
  int   quota_filecount;
  int   mode;
  int   dirmode;
  int   lockfile_mode;
  int   lockfile_timeout;
  int   lock_fcntl_timeout;
  int   lock_retries;
  int   lock_interval;
  int   maildir_retries;
  int   create_file;
  int   options;
  BOOL  allow_fifo;
  BOOL  allow_symlink;
  BOOL  check_group;
  BOOL  check_owner;
  BOOL  create_directory;
  BOOL  notify_comsat;
  BOOL  require_lockfile;
  BOOL  use_lockfile;
  BOOL  set_use_lockfile;
  BOOL  use_fcntl;
  BOOL  set_use_fcntl;
  BOOL  use_mbx_lock;
  BOOL  set_use_mbx_lock;
  BOOL  use_crlf;
  BOOL  from_hack;
  BOOL  set_from_hack;
  BOOL  file_must_exist;
  BOOL  mode_fail_narrower;
  BOOL  maildir_format;
  BOOL  mailstore_format;
  BOOL  mbx_format;
  BOOL  quota_warn_threshold_is_percent;
  BOOL  quota_is_inclusive;
} appendfile_transport_options_block;

/* Restricted creation options */

enum { create_anywhere, create_belowhome, create_inhome };

/* Data for reading the private options. */

extern optionlist appendfile_transport_options[];
extern int appendfile_transport_options_count;

/* Block containing default values. */

extern appendfile_transport_options_block appendfile_transport_option_defaults;

/* The main and init entry points for the transport */

extern BOOL appendfile_transport_entry(transport_instance *, address_item *);
extern void appendfile_transport_init(transport_instance *);

/* End of transports/appendfile.h */
