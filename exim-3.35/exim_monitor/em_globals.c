/*************************************************
*                Exim Monitor                    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "em_hdr.h"

/* This source module contains all the global variables used in
the exim monitor, including those that are used by the standard
Exim modules that are included in Eximon. For comments on their
usage, see em_hdr.h and globals.h. */


/* The first set are unique to Eximon */

Display *X_display;
XtAppContext X_appcon;

XtActionsRec actionTable[] = {
  { "dialogAction",  (XtActionProc)dialogAction}};

int actionTableSize = sizeof(actionTable)/sizeof(XtActionsRec);

XtTranslations queue_trans;
XtTranslations text_trans;

Widget dialog_ref_widget;
Widget toplevel_widget;
Widget log_widget = NULL;
Widget queue_widget;
Widget unhide_widget = NULL;


FILE  *DEBUG = NULL;
FILE  *LOG;

int    action_output = TRUE;
int    action_queue_update = TRUE;
char   actioned_message[24];
char  *action_required;
char  *alternate_config = NULL;

int    body_max = 20000;

int    eximon_initialized = FALSE;

int    log_buffer_size = 10240;
int    log_depth = 150;
char  *log_display_buffer;
char  *log_file = NULL;
char  *log_font = NULL;
ino_t  log_inode;
long int log_position;
int    log_width = 600;

char  *menu_event = "Shift<Btn1Down>";
int    menu_is_up = FALSE;
int    min_height = 162;
int    min_width  = 103;

pipe_item *pipe_chain = NULL;

char  *qualify_domain = NULL;
int    queue_depth = 200;
char  *queue_font = NULL;
int    queue_max_addresses = 10;
skip_item *queue_skip = NULL;
char  *queue_stripchart_name = NULL;
int    queue_update = 60;
int    queue_width = 600;

pcre  *yyyymmdd_regex;

char  *size_stripchart = NULL;
char  *size_stripchart_name = NULL;
int    spool_is_split = FALSE;
int    start_small = FALSE;
int    stripchart_height = 90;
int    stripchart_number = 1;
pcre **stripchart_regex;
char **stripchart_title;
int   *stripchart_total;
int    stripchart_update = 60;
int    stripchart_width = 80;
int    stripchart_varstart = 1;

int    text_depth = 200;
int    tick_queue_accumulator = 999999;

char  *window_title = "exim monitor";


/***********************************************************/
/***********************************************************/


/* These ones are used by Exim modules included in Eximon. Not all are
actually relevant to the operation of Eximon. If SPOOL_DIRECTORY is not
defined (Exim was compiled with it unset), just define it empty. The script
that fires up the monitor fishes the value out by using -bP anyway. */

#ifndef SPOOL_DIRECTORY
#define SPOOL_DIRECTORY ""
#endif

char  *authenticated_id       = NULL;
char  *authenticated_sender   = NULL;

char  *big_buffer             = NULL;
int    big_buffer_size        = BIG_BUFFER_SIZE;
int    body_linecount         = 0;

off_t  data_start_offset      = MESSAGE_ID_LENGTH + 3;
FILE  *debug_file             = NULL;
int    debug_level            = 0;
int    debug_trace_memory     = 0;
int    deliver_datafile       = -1;
BOOL   deliver_firsttime      = FALSE;
BOOL   deliver_freeze         = FALSE;
int    deliver_frozen_at      = 0;
BOOL   deliver_manual_thaw    = FALSE;
BOOL   dont_deliver           = FALSE;
char  *dsn_envid              = 0;
int    dsn_ret                = 0;

char  *exim_path              = BIN_DIRECTORY "/exim"
			"\0<---------------Space to patch exim_path->";

header_line *header_last      = NULL;
header_line *header_list      = NULL;
header_name *header_names     = NULL;

/* These are never used, but some compilers insist on there being some
data, as otherwise they complain about not knowing the size. */

header_name header_names_normal[] = {{ "dummy", 5 }};
header_name header_names_resent[] = {{ "dummy", 5 }};

BOOL   host_lookup_failed     = FALSE;
char  *interface_address      = NULL;

BOOL   local_error_message    = FALSE;
int    message_age            = 0;
char  *message_id;
char   message_id_option[MESSAGE_ID_LENGTH + 3];
char  *message_id_external;

int    message_body_size      = 0;
int    message_linecount      = 0;
char  *message_precedence     = NULL;
int    message_size           = 0;
char   message_subdir[2]      = "\0";

gid_t  originator_gid;
char  *originator_login;
uid_t  originator_uid;

BOOL   print_topbitchars      = FALSE;

BOOL   queue_running          = FALSE;

int    received_count         = 0;
char  *received_protocol      = NULL;
int    received_time          = 0;
int    recipients_count       = 0;
recipient_item *recipients_list = NULL;
int    recipients_list_max    = 0;

char  *sender_address         = NULL;
char  *sender_fullhost        = NULL;
char  *sender_helo_name       = NULL;
char  *sender_host_address    = NULL;
char  *sender_host_authenticated = NULL;
char  *sender_host_name       = NULL;
int    sender_host_port       = 0;
char  *sender_ident           = NULL;
BOOL   sender_local           = FALSE;
BOOL   sender_set_untrusted   = FALSE;
volatile BOOL sigalrm_seen;

BOOL   split_spool_directory  = FALSE;
char  *spool_directory        = SPOOL_DIRECTORY;
int    store_pool             = POOL_MAIN;

char  *tls_cipher             = NULL;
char  *tls_peerdn             = NULL;

tree_node *tree_duplicates    = NULL;
tree_node *tree_nonrecipients = NULL;
tree_node *tree_unusable      = NULL;

BOOL   user_null_sender       = FALSE;

char  *version_date           = "?";
char  *version_string         = "?";

int    warning_count          = 0;

/* End of em_globals.c */
