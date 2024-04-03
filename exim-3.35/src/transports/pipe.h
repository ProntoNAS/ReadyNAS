/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Private structure for the private options. */

typedef struct {
  char *cmd;
  char *allow_commands;
  char *environment;
  char *path;
  char *prefix;
  char *suffix;
  char *temp_errors;
  char *check_string;
  char *escape_string;
  int   umask;
  int   max_output;
  int   timeout;
  int   options;
  BOOL  freeze_exec_fail;
  BOOL  from_hack;
  BOOL  ignore_status;
  BOOL  restrict_to_path;
  BOOL  use_shell;
  BOOL  use_crlf;
} pipe_transport_options_block;

/* Data for reading the private options. */

extern optionlist pipe_transport_options[];
extern int pipe_transport_options_count;

/* Block containing default values. */

extern pipe_transport_options_block pipe_transport_option_defaults;

/* The main and init entry points for the transport */

extern BOOL pipe_transport_entry(transport_instance *, address_item *);
extern void pipe_transport_init(transport_instance *);

/* End of transports/pipe.h */
