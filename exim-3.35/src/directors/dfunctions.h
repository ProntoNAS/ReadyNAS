/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Header for the functions that are shared by the directors */

/* The start of each director's private function block must be the same if they
use any of these shared functions. This is described by the structure defined
here, but we use a macro to do it so the specific blocks can easily incorporate
the same fields in the same order. Not all directors that use this block make
use of every single field in it. */

#define COMMON_DIRECTOR_FIELDS \
  transport_instance *directory_transport; \
  char *expand_directory_transport; \
  transport_instance *directory2_transport; \
  char *expand_directory2_transport; \
  transport_instance *file_transport; \
  char *expand_file_transport; \
  transport_instance *pipe_transport; \
  char *expand_pipe_transport; \
  transport_instance *reply_transport; \
  char *expand_reply_transport; \
  char *file; \
  char *queries; \
  char *query; \
  int   modemask; \
  uid_t *owners; \
  gid_t *owngroups; \
  int   partial_match; \
  int   search_type; \
  BOOL  check_ancestor; \
  BOOL  check_local_user; \
  BOOL  expand; \
  BOOL  forbid_file; \
  BOOL  forbid_pipe; \
  BOOL  forbid_reply; \
  BOOL  include_domain; \
  BOOL  hide_child_in_errmsg; \
  BOOL  one_time; \
  BOOL  optional

typedef struct {
  COMMON_DIRECTOR_FIELDS;
} common_director_options_block;

/* Function prototypes */

extern int  df_extracted(int, director_instance *, address_item *, char *,
              BOOL, BOOL, char *);

extern void df_generated(director_instance *, address_item **, address_item *,
              address_item *, char *, header_line *, char *, ugid_block *,
              struct passwd *, char *);

extern int  df_lookup(director_instance *, address_item *, uschar **,
              uschar **);

/* End of dfunctions.h */
