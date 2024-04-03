/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* All the global variables are defined together in this one module, so that
they are easy to find. */

#include "exim.h"


/* The OSF1 linker puts out a worrying warning if any sections contain no
executable code. It says

Warning: Linking some objects which contain exception information sections
        and some which do not. This may cause fatal runtime exception handling
        problems.

As this may cause people to worry needlessly, include a dummy function here
to stop the message from appearing. Make it reference itself to stop picky
compilers complaining that it is unused, and put in a dummy argument to stop
even pickier compilers complaining about infinite loops. */

static void dummy(int x) { dummy(x-1); }


/* For comments on these variables, see globals.h. I'm too idle to
duplicate them here... */

#ifdef EXIM_PERL
char  *opt_perl_startup       = NULL;
BOOL   opt_perl_at_start      = FALSE;
BOOL   opt_perl_started       = FALSE;
#endif

#ifdef HAVE_AUTH
BOOL   auth_always_advertise  = TRUE;
char  *auth_hosts             = NULL;
#ifdef SUPPORT_TLS
char  *auth_over_tls_hosts    = NULL;
#endif
char  *host_auth_accept_relay = NULL;
pcre  *regex_AUTH             = NULL;
BOOL   smtp_authenticated     = FALSE;

/* Generic options for auths, all of which live inside auth_instance
data blocks and hence have the opt_public flag set. */

optionlist optionlist_auths[] = {
  { "driver",        opt_stringptr | opt_public,
                 (void *)(offsetof(auth_instance, driver_name)) },
  { "public_name",   opt_stringptr | opt_public,
                 (void *)(offsetof(auth_instance, public_name)) },
  { "server_mail_auth_condition", opt_stringptr | opt_public,
                 (void *)(offsetof(auth_instance, mail_auth_condition)) },
  { "server_set_id", opt_stringptr | opt_public,
                 (void *)(offsetof(auth_instance, set_id)) }
};

int optionlist_auths_size = sizeof(optionlist_auths)/sizeof(optionlist);
#endif

#ifdef LOOKUP_LDAP
char  *ldap_default_servers   = NULL;
#endif

#ifdef LOOKUP_MYSQL
char  *mysql_servers          = NULL;
#endif

#ifdef LOOKUP_PGSQL
char *pgsql_servers	      = NULL;
#endif

#ifdef SUPPORT_MOVE_FROZEN_MESSAGES
BOOL   move_frozen_messages   = FALSE;
#endif

/* This variable is outside the #ifdef because it keeps the code less cluttered
in several places (e.g. during logging) if we can always refer to it. */

BOOL   tls_active             = -1;

#ifdef SUPPORT_TLS
pcre  *regex_STARTTLS         = NULL;
char  *tls_advertise_hosts    = NULL;
char  *tls_certificate        = NULL;
char  *tls_cipher             = NULL;
char  *tls_dhparam            = NULL;
char  *tls_hosts              = NULL;
char  *tls_host_accept_relay  = NULL;
BOOL   tls_log_cipher         = TRUE;
BOOL   tls_log_peerdn         = FALSE;
BOOL   tls_offered            = FALSE;
char  *tls_peerdn             = NULL;
char  *tls_privatekey         = NULL;
char  *tls_verify_certificates= NULL;
char  *tls_verify_ciphers     = NULL;
char  *tls_verify_hosts       = NULL;
#endif


#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is
left here just in case. Nothing is documented. */
BOOL   dsn                    = FALSE;
char  *dsn_envid              = NULL;
int    dsn_ret                = 0;
pcre  *regex_DSN              = NULL;
BOOL   smtp_use_dsn           = FALSE;
#endif


/* Input-reading functions for messages, so we can use special ones for
incoming TCP/IP. */

int (*accept_getc)(FILE *)    = getc;
int (*accept_ungetc)(int, FILE *) = ungetc;
int (*accept_feof)(FILE *)    = feof;
int (*accept_ferror)(FILE *)  = ferror;

/* General global variables */

BOOL   accept_8bitmime        = FALSE;
BOOL   accept_call_bombout    = FALSE;
int    accept_linecount       = 0;
int    accept_messagecount    = 0;
int    accept_timeout         = 0;
address_item *addr_duplicate  = NULL;
char  *address_file           = NULL;
char  *address_pipe           = NULL;
BOOL   address_test_mode      = FALSE;
gid_t *admin_groups           = NULL;
BOOL   admin_user             = FALSE;
BOOL   allow_mx_to_ip         = FALSE;
BOOL   allow_unqualified_recipient = TRUE;    /* For local messages */
BOOL   allow_unqualified_sender = TRUE;       /* Reset for SMTP */
BOOL   always_bcc             = FALSE;
char  *authenticated_id       = NULL;
char  *authenticated_sender   = NULL;
auth_instance *auths          = NULL;

auth_instance auth_defaults   = {
   NULL,                      /* chain pointer */
   NULL,                      /* name */
   NULL,                      /* info */
   NULL,                      /* private options block pointer */
   NULL,                      /* driver_name */
   NULL,                      /* public_name */
   NULL,                      /* set_id */
   NULL,                      /* server_mail_auth_condition */
   FALSE,                     /* client */
   FALSE                      /* server */
};

char  *auth_defer_msg         = "reason not recorded";
int    auto_thaw              = 0;

int    batch_max              = -1;
char  *bi_command             = NULL;
char  *big_buffer             = NULL;
int    big_buffer_size        = BIG_BUFFER_SIZE;
int    body_linecount         = 0;
int    bsmtp_transaction_linecount = 0;

BOOL   check_dns_names        = TRUE;
char  *check_dns_names_pattern= "(?i)^(?>(?(1)\\.|())[^\\W_](?>[a-z0-9-]*[^\\W_])?)+$";
int    check_log_inodes       = 0;
int    check_log_space        = 0;
int    check_spool_inodes     = 0;
int    check_spool_space      = 0;
int    clmacro_count          = 0;
char  *clmacros[MAX_CLMACROS];
BOOL   config_changed         = FALSE;
FILE  *config_file            = NULL;
char  *config_filename        = CONFIGURE_FILE
			"\0<-----------Space to patch configure_filename->";
int    config_lineno          = 0;
char  *continue_hostname      = NULL;
BOOL   continue_more          = FALSE;
int    continue_sequence      = 1;
char  *continue_transport     = NULL;

BOOL   daemon_listen          = FALSE;
int    debug_fd               = -1;
FILE  *debug_file             = NULL;
int    debug_level            = -1;
pid_t  debug_pid              = 0;
int    debug_trace_memory     = 0;
int    delay_warning[DELAY_WARNING_SIZE] = { DELAY_WARNING_SIZE, 1, 24*60*60 };
char  *delay_warning_condition= "${if match{$h_precedence:}{(?i)bulk|list|junk}{no}{yes}}";
BOOL   delivery_date_remove   = TRUE;
char  *deliver_in_buffer      = NULL;
char  *deliver_out_buffer     = NULL;
int    deliver_datafile       = -1;
char  *deliver_domain         = NULL;
char  *deliver_domain_orig    = NULL;
char  *deliver_domain_parent  = NULL;
BOOL   deliver_firsttime      = FALSE;
BOOL   deliver_force          = FALSE;
BOOL   deliver_freeze         = FALSE;
int    deliver_frozen_at      = 0;
char  *deliver_home           = NULL;
char  *deliver_host           = NULL;
char  *deliver_host_address   = NULL;
int    deliver_load_max       = -1;
char  *deliver_localpart      = NULL;
char  *deliver_localpart_orig = NULL;
char  *deliver_localpart_parent = NULL;
char  *deliver_localpart_prefix = NULL;
char  *deliver_localpart_suffix = NULL;
BOOL   deliver_force_thaw     = FALSE;
BOOL   deliver_manual_thaw    = FALSE;
int    deliver_queue_load_max = -1;
address_item *deliver_recipients = NULL;
char  *deliver_selectstring   = NULL;
BOOL   deliver_selectstring_regex = FALSE;
char  *deliver_selectstring_sender = NULL;
BOOL   deliver_selectstring_sender_regex = FALSE;
director_instance *directors  = NULL;

director_instance director_defaults = {
   NULL,                      /* chain pointer */
   NULL,                      /* name */
   NULL,                      /* info */
   NULL,                      /* private options block pointer */
   NULL,                      /* driver name */
   NULL,                      /* domains */
   NULL,                      /* local_parts */
   NULL,                      /* errors_to */
   NULL,                      /* extra_headers */
   NULL,                      /* remove_headers */
   NULL,                      /* debug_string */
   NULL,                      /* fallback_hosts */
   NULL,                      /* fallback_hostlist */
   TRUE,                      /* more */
   FALSE,                     /* verify_only */
   TRUE,                      /* verify_sender */
   TRUE,                      /* verify_recipient */
   NULL,                      /* expand_uid */
   NULL,                      /* expand_gid */
   (uid_t)(-1),               /* uid */
   (gid_t)(-1),               /* gid */
   FALSE,                     /* uid_set */
   FALSE,                     /* gid_set */
   FALSE,                     /* initgroups */
   NULL,                      /* transport */
   NULL,                      /* expanded_transport */
   NULL,                      /* current_directory */
   NULL,                      /* home_directory */
   NULL,                      /* condition */
   NULL,                      /* prefix */
   NULL,                      /* suffix */
   NULL,                      /* senders */
   NULL,                      /* require_files */
   NULL,                      /* new_director */
   NULL,                      /* new (director) */
   FALSE,                     /* unseen */
   FALSE,                     /* prefix_optional */
   FALSE,                     /* suffix_optional */
   FALSE,                     /* fail_verify_sender */
   FALSE,                     /* fail_verify_recipient */
   TRUE,                      /* expn */
   TRUE                       /* one_continue */
};

char  *dns_again_means_nonexist = NULL;
int    dns_retrans            = 0;
int    dns_retry              = 0;
char  *domain_data            = NULL;
BOOL   dont_deliver           = FALSE;
BOOL   dot_ends               = TRUE;
BOOL   drop_cr                = FALSE;

BOOL   enable_dollar_recipients = FALSE;
BOOL   envelope_to_remove     = TRUE;
char  *errmsg_file            = NULL;
char  *errmsg_text            = NULL;
char  *errmsg_recipient       = NULL;
int    errno_quota            = ERRNO_QUOTA;
char  *errors_copy            = NULL;
int    error_handling         = ERRORS_SENDER;
char  *errors_address         = "postmaster";
char  *errors_reply_to        = NULL;
int    errors_sender_rc       = EXIT_FAILURE;

#ifdef EXIM_GID
gid_t  exim_gid               = EXIM_GID;
BOOL   exim_gid_set           = TRUE;
#else
gid_t  exim_gid               = 0;
BOOL   exim_gid_set           = FALSE;
#endif

char  *exim_path              = BIN_DIRECTORY "/exim"
			"\0<---------------Space to patch exim_path->";

#ifdef EXIM_UID
uid_t  exim_uid               = EXIM_UID;
BOOL   exim_uid_set           = TRUE;
#else
uid_t  exim_uid               = 0;
BOOL   exim_uid_set           = FALSE;
#endif

BOOL   expand_forbid_exists   = FALSE;
BOOL   expand_forbid_lookup   = FALSE;
BOOL   expand_forbid_perl     = FALSE;
int    expand_nlength[EXPAND_MAXN+1];
int    expand_nmax            = -1;
char  *expand_nstring[EXPAND_MAXN+1];
BOOL   expand_string_forcedfail = FALSE;
char  *expand_string_message;
BOOL   extract_addresses_remove_arguments = TRUE;

char  *filter_fmsg            = NULL;
int    filter_n[FILTER_VARIABLE_COUNT];
BOOL   filter_running         = FALSE;
int    filter_sn[FILTER_VARIABLE_COUNT];
char  *filter_test            = NULL;
char  *filter_thisaddress     = NULL;
int    finduser_retries       = 0;
BOOL   forbid_domain_literals = FALSE;
BOOL   freeze_tell_mailmaster = FALSE;

char  *gecos_name             = NULL;
char  *gecos_pattern          = NULL;
rewrite_rule *global_rewrite_rules = NULL;

BOOL   have_seteuid           = HAVE_SETEUID || HAVE_SETRESUID;
BOOL   header_changed         = FALSE;
header_line *header_last      = NULL;
header_line *header_list      = NULL;
header_name *header_names     = NULL;

/* These are the headers that Exim notices, and may use in various ways. */

header_name header_names_normal[] = {
  { "Bcc",         3 },       /* This data must be in the correct */
  { "Cc",          2 },       /* order as defined by the enum for */
  { "Date",        4 },       /* the hn_xxx offsets. */
  { "From",        4 },
  { "Message-Id", 10 },
  { "Sender",      6 },
  { "To",          2 },
  { "Reply-To",    8 },
  { "Subject",     7 }
};

/* If any resent- header exists, this set of names is used instead. */

header_name header_names_resent[] = {
  { "Resent-Bcc",        10 }, /* This data must be in the correct */
  { "Resent-Cc",          9 }, /* order as defined by the enum for */
  { "Resent-Date",       11 }, /* the hn_xxx offsets. */
  { "Resent-From",       11 },
  { "Resent-Message-Id", 17 },
  { "Resent-Sender",     13 },
  { "Resent-To",          9 },
  { "Reply-To",           8 }, /* There aren't resent-versions of these */
  { "Subject",            7 }

/*****
Exim used to have these two lines: they are replaced by the above because
RFC 2822 has disallowed Resent-Reply-To: and there never was a Resent-Subject:
(I was deluded).

  { "Resent-Reply-To",   15 },
  { "Resent-Subject",    14 }
*****/

};

BOOL   headers_check_syntax   = FALSE;
BOOL   headers_checks_fail    = TRUE;
BOOL   headers_sender_verify  = FALSE;
BOOL   headers_sender_verify_errmsg = FALSE;
char  *helo_accept_junk_hosts = NULL;
BOOL   helo_strict_syntax     = FALSE;
char  *helo_verify            = NULL;
char  *hold_domains           = NULL;
char  *host_accept_relay      = NULL;
BOOL   host_checking          = FALSE;
BOOL   host_find_failed_syntax= FALSE;
char  *host_lookup            = NULL;
BOOL   host_lookup_failed     = FALSE;
char  *host_lookup_msg        = "";
int    host_number            = 0;
char  *host_number_string     = NULL;
char  *host_reject            = NULL;
char  *host_reject_recipients = NULL;
char  *hosts_treat_as_local   = NULL;

BOOL   ignore_errmsg_errors   = FALSE;
int    ignore_errmsg_errors_after = 0;
BOOL   ignore_fromline_local  = FALSE;
char  *ignore_fromline_hosts  = NULL;
char  *interface_address      = NULL;
#if HAVE_IPV6
BOOL   ipv4_address_lookup    = FALSE;
#else
BOOL   ipv4_address_lookup    = TRUE;
#endif

int    journal_fd             = -1;

int    keep_malformed         = 4*24*60*60;    /* 4 days */
BOOL   kill_ip_options        = TRUE;

int    load_average           = -2;
char  *local_domains          = NULL;
BOOL   local_domains_include_host = FALSE;
BOOL   local_domains_include_host_literals = FALSE;
BOOL   local_error_message    = FALSE;
BOOL   local_from_check     = TRUE;
char  *local_from_prefix    = NULL;
char  *local_from_suffix    = NULL;
char  *local_interfaces       = NULL;
ip_address_item *local_interface_data = NULL;
char  *local_part_data        = NULL;
BOOL   locally_caseless       = TRUE;
BOOL   log_all_parents        = FALSE;
BOOL   log_arguments          = FALSE;
char  *log_buffer             = NULL;
char  *log_file_path          = LOG_FILE_PATH
			  "\0<--------------Space to patch log_file_path->";
BOOL   log_incoming_port      = FALSE;
BOOL   log_ip_options         = TRUE;
int    log_level              = 5;
int    log_queue_run_level    = 0;
BOOL   log_received_sender    = FALSE;
BOOL   log_received_recipients = FALSE;
BOOL   log_refused_recipients = FALSE;
BOOL   log_rewrites           = FALSE;
BOOL   log_sender_on_delivery = FALSE;
BOOL   log_smtp_confirmation  = FALSE;
BOOL   log_smtp_connections   = FALSE;
BOOL   log_smtp_syntax_errors = FALSE;
FILE  *log_stderr             = NULL;
BOOL   log_subject            = FALSE;
char  *login_sender_address   = NULL;
char  *lookup_key             = NULL;
int    lookup_open_max        = 25;
char  *lookup_value           = NULL;

macro_item *macros            = NULL;
char  **mailer_argv;
int    max_username_length    = 0;
int    message_age            = 0;
char  *message_body           = NULL;
char  *message_body_end       = NULL;
int    message_body_size      = 0;
int    message_body_visible   = 500;
int    message_ended          = END_NOTSTARTED;
char  *message_filter         = NULL;

char  *message_filter_directory_transport = NULL;
char  *message_filter_directory2_transport = NULL;
char  *message_filter_file_transport = NULL;
char  *message_filter_pipe_transport = NULL;
char  *message_filter_reply_transport = NULL;

gid_t  message_filter_gid     = 0;
BOOL   message_filter_gid_set = FALSE;
uid_t  message_filter_uid     = 0;
BOOL   message_filter_uid_set = FALSE;
char  *message_id;
char  *message_id_text        = NULL;
char   message_id_option[MESSAGE_ID_LENGTH + 3];
char  *message_id_external;
int    message_linecount      = 0;
FILE  *message_log            = NULL;
char  *message_precedence     = NULL;
int    message_size           = 0;
int    message_size_limit     = 0;
BOOL   message_size_limit_count_recipients = FALSE;
char   message_subdir[2]      = "\0";
char  *message_reference      = NULL;

uid_t *never_users            = NULL;
gid_t  nobody_gid             = 0;
BOOL   nobody_gid_set         = FALSE;
uid_t  nobody_uid             = 0;
BOOL   nobody_uid_set         = FALSE;

uid_t  original_euid;
gid_t  originator_gid;
char  *originator_login       = NULL;
char  *originator_name        = NULL;
uid_t  originator_uid;

BOOL   parse_allow_group      = FALSE;
BOOL   parse_found_group      = FALSE;
char  *percent_hack_domains   = NULL;
char  *pid_file_path          = PID_FILE_PATH
			  "\0<--------------Space to patch pid_file_path->";
BOOL   preserve_message_logs  = FALSE;
char  *primary_hostname       = NULL;
char  *primary_hostname_lc    = NULL;
BOOL   print_topbitchars      = FALSE;
char   process_info[PROCESS_INFO_SIZE];
BOOL   prod_requires_admin    = TRUE;
char  *prohibition_message    = NULL;
char  *prohibition_reason     = NULL;

char  *qualify_domain_recipient = NULL;
char  *qualify_domain_sender  = NULL;
BOOL   queue_2stage           = FALSE;
int    queue_interval         = -1;
BOOL   queue_list_requires_admin = TRUE;
BOOL   queue_only             = FALSE;
char  *queue_only_file        = NULL;
int    queue_only_load        = -1;
BOOL   queue_remote           = FALSE;
char  *queue_remote_domains   = NULL;
BOOL   queue_run_force        = FALSE;
BOOL   queue_run_in_order     = FALSE;
BOOL   queue_run_local        = FALSE;
int    queue_run_max          = 5;
pid_t  queue_run_pid          = (pid_t)0;
int    queue_run_pipe         = -1;
BOOL   queue_running          = FALSE;
BOOL   queue_smtp             = FALSE;
char  *queue_smtp_domains     = NULL;

unsigned int random_seed      = 0;
char  *raw_sender             = NULL;
char **raw_recipients         = NULL;
char  *rbl_domain             = NULL;
char  *rbl_domains            = NULL;
char  *rbl_hosts              = "*";
header_line *rbl_header       = NULL;
BOOL   rbl_host               = FALSE;
BOOL   rbl_log_rcpt_count     = FALSE;
BOOL   rbl_log_headers        = FALSE;
char  *rbl_msg_buffer         = NULL;
BOOL   rbl_reject_recipients  = TRUE;
BOOL   rbl_warn_header        = TRUE;

gid_t  real_gid;
uid_t  real_uid;
BOOL   really_exim            = TRUE;
int    received_count         = 0;
char  *received_for           = NULL;

/* This is the default text for Received headers generated by Exim. The
date will be automatically added on the end. */

char  *received_header_text   =
    "Received: "
    "${if def:sender_rcvhost {from $sender_rcvhost\n\t}"
    "{${if def:sender_ident {from $sender_ident }}"
    "${if def:sender_helo_name {(helo=$sender_helo_name)\n\t}}}}"
    "by ${primary_hostname} "
    "${if def:received_protocol {with $received_protocol}} "
    #ifdef SUPPORT_TLS
    "${if def:tls_cipher {($tls_cipher)\n\t}}"
    #endif
    "(Exim $version_number #$compile_number)\n\t"
    "id ${message_id}"
    "${if def:received_for {\n\tfor $received_for}}"
    "\0<---------------Space to patch received_header_text->";

int    received_headers_max   = 30;
char  *received_protocol      = NULL;
int    received_time          = 0;
BOOL   receiver_try_verify    = FALSE;
char  *receiver_unqualified_hosts = NULL;
BOOL   receiver_verify        = FALSE;
char  *receiver_verify_addresses = NULL;
char  *receiver_verify_hosts  = "*";
char  *receiver_verify_senders = NULL;
int    recipients_count       = 0;
recipient_item *recipients_list = NULL;
int    recipients_list_max    = 0;
int    recipients_max         = 0;
BOOL   recipients_max_reject  = FALSE;
char  *recipients_reject_except = NULL;
char  *recipients_reject_except_senders = NULL;
BOOL   refuse_all_rcpts       = FALSE;
BOOL   refuse_ip_options      = TRUE;
char **regcomp_error_pointer  = NULL;
pcre  *regex_check_dns_names  = NULL;
pcre  *regex_From             = NULL;
pcre  *regex_PIPELINING       = NULL;
pcre  *regex_SIZE             = NULL;
pcre  *regex_ismsgid          = NULL;
char   *relay_domains         = NULL;
BOOL   relay_domains_include_local_mx = FALSE;
BOOL   relay_need_either      = FALSE;
int    remote_max_parallel    = 1;
char  *remote_sort            = NULL;
int    retry_data_expire      = 7*24*60*60;
int    retry_interval_max     = 24*60*60;
int    retry_maximum_timeout  = 0;        /* set from retry config */
retry_config *retries         = NULL;
char  *return_path            = NULL;
BOOL   return_path_remove     = TRUE;
int    return_size_limit      = 100*1024;
int    rewrite_existflags     = 0;
char  *rfc1413_hosts          = "*";
int    rfc1413_query_timeout  = 30;
BOOL   rfc821_domains         = FALSE;
uid_t  root_uid               = ROOT_UID;
char  *route_option           = NULL;

router_instance *routers  = NULL;
router_instance router_defaults = {
   NULL,                      /* chain pointer */
   NULL,                      /* name */
   NULL,                      /* info */
   NULL,                      /* private options block pointer */
   NULL,                      /* driver name */
   NULL,                      /* domains */
   NULL,                      /* local_parts */
   NULL,                      /* errors_to */
   NULL,                      /* extra_headers */
   NULL,                      /* remove_headers */
   NULL,                      /* debug_string */
   NULL,                      /* fallback_hosts */
   NULL,                      /* fallback_hostlist */
   TRUE,                      /* more */
   FALSE,                     /* verify_only */
   TRUE,                      /* verify_sender */
   TRUE,                      /* verify_recipient */
   NULL,                      /* expand_uid */
   NULL,                      /* expand_gid */
   (uid_t)(-1),               /* uid */
   (gid_t)(-1),               /* gid */
   FALSE,                     /* uid_set */
   FALSE,                     /* gid_set */
   FALSE,                     /* initgroups */
   NULL,                      /* transport instance */
   NULL,                      /* expanded_transport */
   NULL,                      /* condition */
   NULL,                      /* senders */
   NULL,                      /* require_files */
   FALSE,                     /* unseen */
   FALSE,                     /* fail_verify_sender */
   FALSE,                     /* fail_verify_recipient */
   FALSE,                     /* pass_on_timeout */
   "freeze",                  /* self */
   self_freeze,               /* self_code */
   FALSE,                     /* self_rewrite */
   TRUE,                      /* sender_rewrite */
   NULL,                      /* translate_ip_address */
   NULL                       /* ignore_target_hosts */
};

BOOL   running_in_test_harness = FALSE;
char  *running_status         = ">>>running<<<";

BOOL   search_find_defer      = FALSE;
int    security_level         = 0;
char  *security_type          = NULL;
char  *self_hostname          = NULL;
char  *sender_address         = NULL;
BOOL   sender_address_forced  = FALSE;
char  *sender_address_relay   = NULL;
char  *sender_address_relay_hosts = "*";
char  *sender_address_unrewritten = NULL;
char  *sender_fullhost        = NULL;
char  *sender_helo_name       = NULL;
char **sender_host_aliases    = NULL;
char  *sender_host_address    = NULL;
char  *sender_host_authenticated = NULL;
char  *sender_host_name       = NULL;
int    sender_host_port       = 0;
BOOL   sender_host_notsocket  = FALSE;
BOOL   sender_host_unknown    = FALSE;
char  *sender_ident           = NULL;
BOOL   sender_local           = FALSE;
char  *sender_rcvhost         = NULL;
char  *sender_reject          = NULL;
char  *sender_reject_recipients = NULL;
BOOL   sender_set_untrusted   = FALSE;
BOOL   sender_try_verify      = FALSE;
char  *sender_unqualified_hosts = NULL;
BOOL   sender_verify          = FALSE;
BOOL   sender_verify_batch    = FALSE;
char  *sender_verify_callback_domains = NULL;
char  *sender_verify_callback_error = NULL;
int    sender_verify_callback_timeout = 30;
BOOL   sender_verify_fixup    = FALSE;
char  *sender_verify_hosts    = "*";
char  *sender_verify_hosts_callback = NULL;
BOOL   sender_verify_reject   = TRUE;
int    sender_verify_max_retry_rate = 12;
volatile BOOL sigalrm_seen    = FALSE;
char **sighup_argv            = NULL;
int    smtp_accept_count      = 0;
BOOL   smtp_accept_keepalive  = TRUE;
int    smtp_accept_max        = 20;
int    smtp_accept_max_per_host = 0;
int    smtp_accept_queue      = 0;
int    smtp_accept_queue_per_connection = 10;
int    smtp_accept_reserve    = 0;
char  *smtp_banner            = "${primary_hostname} ESMTP "
                            "Exim ${version_number} #${compile_number} "
                            "${tod_full}"
			    "\0<---------------Space to patch smtp_banner->";
BOOL   smtp_batched_input     = FALSE;
BOOL   smtp_check_spool_space = TRUE;
int    smtp_connect_backlog   = 5;
FILE  *smtp_in                = NULL;
BOOL   smtp_input             = FALSE;
int    smtp_load_reserve      = -1;
FILE  *smtp_out               = NULL;
int    smtp_port              = -1;
int    smtp_receive_timeout   = 5*60;
char  *smtp_etrn_command      = NULL;
char  *smtp_etrn_hosts        = NULL;
BOOL   smtp_etrn_serialize    = TRUE;
char  *smtp_expn_hosts        = NULL;
char  *smtp_read_error        = "";
char  *smtp_reserve_hosts     = NULL;
BOOL   smtp_use_pipelining    = FALSE;
BOOL   smtp_use_size          = FALSE;
BOOL   smtp_verify            = FALSE;
BOOL   split_spool_directory  = FALSE;
char  *spool_directory        = SPOOL_DIRECTORY
			  "\0<--------------Space to patch spool_directory->";
char  *stderr_filename        = STDERR_FILE
                          "\0<--------------Space to patch stderr_filename->";

int    store_pool             = POOL_MAIN;
BOOL   strip_excess_angle_brackets = FALSE;
BOOL   strip_trailing_dot     = FALSE;
BOOL   syslog_timestamp       = TRUE;


int    timeout_frozen_after   = 0;
BOOL   timestamps_utc         = FALSE;

transport_instance *transports = NULL;

transport_instance *transport_message_filter_directory = NULL;
transport_instance *transport_message_filter_directory2 = NULL;
transport_instance *transport_message_filter_file = NULL;
transport_instance *transport_message_filter_pipe = NULL;
transport_instance *transport_message_filter_reply = NULL;


transport_instance transport_defaults = {
   NULL,                     /* chain pointer */
   NULL,                     /* name */
   NULL,                     /* info */
   NULL,                     /* private options block pointer */
   NULL,                     /* driver name */
   NULL,                     /* setup entry point */
   local_batch_off,          /* local_batch */
   local_smtp_off,           /* local_smtp */
   100,                      /* batch_max */
   FALSE,                    /* uid_set */
   FALSE,                    /* gid_set */
   (uid_t)(-1),              /* uid */
   (gid_t)(-1),              /* gid */
   NULL,                     /* expand_uid */
   NULL,                     /* expand_gid */
   NULL,                     /* home_dir */
   NULL,                     /* current_dir */
   NULL,                     /* warn_message */
   FALSE,                    /* deliver_as_creator */
   FALSE,                    /* initgroups */
   FALSE,                    /* bsmtp_helo */
   TRUE,                     /* multi-domain */
   FALSE,                    /* overrides_hosts */
   NULL,                     /* shadow */
   NULL,                     /* shadow_condition */
   NULL,                     /* filter_command */
   NULL,                     /* add_headers */
   NULL,                     /* remove_headers */
   NULL,                     /* return_path */
   NULL,                     /* debug_string */
   NULL,                     /* headers_rewrite */
   NULL,                     /* rewrite_rules */
   0,                        /* rewrite_existflags */
   FALSE,                    /* body_only */
   FALSE,                    /* delivery_date_add */
   FALSE,                    /* envelope_to_add */
   FALSE,                    /* headers_only */
   FALSE,                    /* return_path_add */
   FALSE,                    /* return_output */
   FALSE,                    /* return_fail_output */
   FALSE,                    /* log_output */
   FALSE,                    /* log_fail_output */
   FALSE,                    /* log_defer_output */
   2,                        /* BOOL, but set neither 1 nor 0 so can detect */
   100,                      /* max_addresses */
   0                         /* message_size_limit */
};

int    transport_count;
char **transport_filter_argv  = NULL;
int    transport_write_timeout= 0;

tree_node *tree_dns_fails     = NULL;
tree_node *tree_duplicates    = NULL;
tree_node *tree_nonrecipients = NULL;
tree_node *tree_unusable      = NULL;

BOOL   trusted_caller         = FALSE;
gid_t *trusted_groups         = NULL;
uid_t *trusted_users          = NULL;
char  *timezone_string        = TIMEZONE_DEFAULT;

char  *unknown_login          = NULL;
char  *unknown_username       = NULL;
BOOL   untrusted_set_sender   = FALSE;

/* A regex for matching a "From_" line in an incoming message, in the form

   From ph10 Fri Jan  5 12:35 GMT 1996

which the "mail" commands send to the MTA (undocumented, of course), or in
the form

   From ph10 Fri, 7 Jan 97 14:00:00 GMT

which is apparently used by some UUCPs, despite it not being in RFC 976.
Because of variations in time formats, just match up to the minutes. That
should be sufficient. Examples have been seen of time fields like 12:1:03,
so just require one digit for hours and minutes. The weekday is also absent
in some forms. */

char  *uucp_from_pattern      =
  "^From\\s+(\\S+)\\s+(?:[a-zA-Z]{3},?\\s+)?"    /* Common start */
  "(?:"                                          /* Non-extracting bracket */
  "[a-zA-Z]{3}\\s+\\d?\\d|"                      /* First form */
  "\\d?\\d\\s+[a-zA-Z]{3}\\s+\\d\\d(?:\\d\\d)?"  /* Second form */
  ")"                                            /* End alternation */
  "\\s+\\d\\d?:\\d\\d?";                         /* Start of time */

char  *uucp_from_sender       = "$1";

char  *warnmsg_delay          = NULL;
char  *warnmsg_file           = NULL;
char  *warnmsg_recipients     = NULL;

char  *verify_address_parse_error = NULL;
char  *verify_forced_errmsg   = NULL;
BOOL   verify_only            = FALSE;
char  *version_copyright      = "Copyright (c) University of Cambridge 2001";
char  *version_date           = "?";
char  *version_cnumber        = "????";
char  *version_string         = "?";

int    warning_count          = 0;

/* End of globals.c */
