/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* All the global variables are defined together in this one header, so that
they are easy to find. First put any specials that are required for some
operating systems. */

#ifdef NEED_H_ERRNO
extern int h_errno;
#endif

/* Now things that are present only when configured. */

#ifdef EXIM_PERL
extern char  *opt_perl_startup;       /* Startup code for Perl interpreter */
extern BOOL   opt_perl_at_start;      /* Start Perl interpreter at start */
extern BOOL   opt_perl_started;       /* Set once interpreter started */
#endif

#ifdef HAVE_AUTH
extern BOOL   auth_always_advertise;  /* If FALSE, advertise only when needed */
extern char  *auth_hosts;             /* These must authenticate */
#ifdef SUPPORT_TLS
extern char  *auth_over_tls_hosts;    /* These must use TLS for AUTH */
#endif
extern char  *host_auth_accept_relay; /* Relay permitted if authenticated */
extern pcre  *regex_AUTH;             /* For recognizing AUTH settings */
extern BOOL   smtp_authenticated;     /* Sending client has authenticated */
#endif

#ifdef LOOKUP_LDAP
extern char  *ldap_default_servers;   /* List of default servers */
#endif

#ifdef LOOKUP_MYSQL
extern char  *mysql_servers;          /* List of servers and connect info */
#endif

#ifdef LOOKUP_PGSQL
extern char  *pgsql_servers;          /* List of servers and connect info */
#endif

#ifdef SUPPORT_MOVE_FROZEN_MESSAGES
extern BOOL   move_frozen_messages;   /* Get them out of the normal directory */
#endif

/* This variable is outside the #ifdef because it keeps the code less cluttered
in several places (e.g. during logging) if we can always refer to it. */

extern int    tls_active;             /* fd/socket when in a TLS session */

#ifdef SUPPORT_TLS
extern pcre  *regex_STARTTLS;         /* For recognizing STARTTLS settings */
extern char  *tls_advertise_hosts;    /* host for which TLS is advertised */
extern char  *tls_certificate;        /* Certificate file */
extern char  *tls_cipher;             /* Cipher used */
extern char  *tls_dhparam;            /* DH param file */
extern char  *tls_hosts;              /* hosts which must use TLS */
extern char  *tls_host_accept_relay;  /* hosts which may relay if using TLS */
extern BOOL   tls_log_cipher;         /* Request cipher logging */
extern BOOL   tls_log_peerdn;         /* Request peerdn logging */
extern BOOL   tls_offered;            /* Server offered TLS */
extern char  *tls_peerdn;             /* DN from peer */
extern char  *tls_privatekey;         /* Private key file */
extern char  *tls_verify_certificates;/* Path for certificates to check */
extern char  *tls_verify_ciphers;     /* Ciphers to allow */
extern char  *tls_verify_hosts;       /* host list for client verification */
#endif


#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is
left here just in case. Nothing is documented. */
extern BOOL   dsn;                    /* FALSE if DSN not to be used */
extern char  *dsn_envid;              /* DSN envid string */
extern int    dsn_ret;                /* DSN ret type*/
extern pcre  *regex_DSN;              /* For recognizing DSN settings */
extern BOOL   smtp_use_dsn;           /* Global for passed connections */
#endif


/* Input-reading functions for messages, so we can use special ones for
incoming TCP/IP. */

extern int (*accept_getc)(FILE *);
extern int (*accept_ungetc)(int, FILE *);
extern int (*accept_feof)(FILE *);
extern int (*accept_ferror)(FILE *);

/* General global variables */

extern BOOL   accept_8bitmime;        /* Allow *BITMIME incoming */
extern BOOL   accept_call_bombout;    /* Flag for crashing log */
extern int    accept_linecount;       /* Mainly for BSMTP errors */
extern int    accept_messagecount;    /* Mainly for BSMTP errors */
extern int    accept_timeout;         /* For non-SMTP acceptance */
extern address_item *addr_duplicate;  /* Duplicate address list */
extern char  *address_file;           /* Name of file when delivering to one */
extern char  *address_pipe;           /* Pipe command when delivering to one */
extern BOOL   address_test_mode;      /* True for -bt */
extern gid_t *admin_groups;           /* List of admin groups */
extern BOOL   admin_user;             /* True if caller can do admin */
extern BOOL   allow_mx_to_ip;         /* Allow MX records to -> ip address */
extern BOOL   allow_unqualified_recipient; /* As it says */
extern BOOL   allow_unqualified_sender; /* Ditto */
extern BOOL   always_bcc;             /* Use bcc in all adding cases */
extern char  *authenticated_id;       /* ID that was authenticated */
extern char  *authenticated_sender;   /* From AUTH on MAIL */
extern auth_info auths_available[];   /* Vector of available auth mechanisms */
extern auth_instance *auths;          /* Chain of instantiated auths */
extern auth_instance auth_defaults;   /* Default values */
extern char  *auth_defer_msg;         /* Error message */
extern int    auto_thaw;              /* Auto-thaw interval */

extern int    batch_max;              /* Max SMTP batching from -oB */
extern char  *bi_command;             /* Command for -bi option */
extern char  *big_buffer;             /* Used for various temp things */
extern int    big_buffer_size;        /* Current size (can expand) */
extern int    body_linecount;         /* Line count in body */
extern int    bsmtp_transaction_linecount; /* Start of last transaction */

extern BOOL   check_dns_names;        /* True for preliminary syntax check */
extern char  *check_dns_names_pattern;/* Regex for checking */
extern int    check_log_inodes;       /* Minimum for message acceptance */
extern int    check_log_space;        /* Minimum for message acceptance */
extern int    check_spool_inodes;     /* Minimum for message acceptance */
extern int    check_spool_space;      /* Minimum for message acceptance */
extern int    clmacro_count;          /* Number of command line macros */
extern char  *clmacros[];             /* Copy of them, for re-exec */
extern BOOL   config_changed;         /* True if -C used */
extern FILE  *config_file;            /* Configuration file */
extern char  *config_filename;        /* Configuration file name */
extern int    config_lineno;          /* Line number in config file */
extern char  *continue_hostname;      /* Host for continued delivery */
extern BOOL   continue_more;          /* Flag more addresses waiting */
extern int    continue_sequence;      /* Sequence num for continued delivery */
extern char  *continue_transport;     /* Transport for continued delivery */

extern BOOL   daemon_listen;          /* True if listening required */
extern int    debug_fd;               /* The fd for debug_file */
extern FILE  *debug_file;             /* Where to write debugging info */
extern int    debug_level;            /* Just what its name implies */
extern pid_t  debug_pid;              /* Pid to add to debug output */
extern int    debug_trace_memory;     /* For bad problems */
extern int    delay_warning[];        /* Times between warnings */
extern char  *delay_warning_condition; /* Condition string for warnings */
extern BOOL   delivery_date_remove;   /* Remove delivery-date headers */
extern char  *deliver_in_buffer;      /* Buffers for copying file */
extern char  *deliver_out_buffer;
extern int    deliver_datafile;       /* FD for data part of message */
extern char  *deliver_domain;         /* The local domain for delivery */
extern char  *deliver_domain_orig;    /* The original local domain for delivery */
extern char  *deliver_domain_parent;  /* The parent domain for delivery */
extern BOOL   deliver_firsttime;      /* True for first delivery attempt */
extern BOOL   deliver_force;          /* TRUE if delivery was forced */
extern BOOL   deliver_freeze;         /* TRUE if delivery is frozen */
extern int    deliver_frozen_at;      /* Time of freezing */
extern char  *deliver_home;           /* Home directory for pipes */
extern char  *deliver_host;           /* (First) host for routed local deliveries */
                                      /* Remote host for filter */
extern char  *deliver_host_address;   /* Address for remote delivery filter */
extern int    deliver_load_max;       /* No deliveries if load > this */
extern char  *deliver_localpart;      /* The local part for delivery */
extern char  *deliver_localpart_orig; /* The original local part for delivery */
extern char  *deliver_localpart_parent; /* The parent local part for delivery */
extern char  *deliver_localpart_prefix; /* The stripped prefix, if any */
extern char  *deliver_localpart_suffix; /* The stripped suffix, if any */
extern BOOL   deliver_force_thaw;     /* TRUE to force thaw in queue run */
extern BOOL   deliver_manual_thaw;    /* TRUE if manually thawed */
extern int    deliver_queue_load_max; /* Different value for queue running */
extern address_item *deliver_recipients; /* Current set of addresses */
extern char  *deliver_selectstring;   /* For selecting by recipient */
extern BOOL   deliver_selectstring_regex; /* String is regex */
extern char  *deliver_selectstring_sender; /* For selecting by sender */
extern BOOL   deliver_selectstring_sender_regex; /* String is regex */
extern director_info directors_available[]; /* Vector of available directors */
extern director_instance *directors;  /* Chain of instantiated directors */
extern director_instance director_defaults; /* Default values */
extern char  *dns_again_means_nonexist; /* Domains that are badly set up */
extern int    dns_retrans;            /* Retransmission time setting */
extern int    dns_retry;              /* Number of retries */
extern char  *domain_data;            /* From domain lookup */
extern BOOL   dont_deliver;           /* TRUE for -N option */
extern BOOL   dot_ends;               /* TRUE if "." ends non-SMTP input */
extern BOOL   drop_cr;                /* For broken local MUAs */

extern BOOL   enable_dollar_recipients; /* Make $recipients available */
extern int    envelope_to_remove;     /* Remove envelope_to_headers */
extern char  *errmsg_file;            /* Template file */
extern char  *errmsg_text;            /* One-liner */
extern char  *errmsg_recipient;       /* When writing an errmsg */
extern int    errno_quota;            /* Quota errno in this OS */
extern int    error_handling;         /* Error handling style */
extern char  *errors_address;         /* Normally "postmaster" */
extern char  *errors_copy;            /* For taking copies of errors */
extern char  *errors_reply_to;        /* Reply-to for error messages */
extern int    errors_sender_rc;       /* Return after message to sender*/
extern gid_t  exim_gid;               /* To be used with exim_uid */
extern BOOL   exim_gid_set;           /* TRUE if exim_gid set */
extern char  *exim_path;              /* Path to exec exim */
extern uid_t  exim_uid;               /* Non-root uid for exim */
extern BOOL   exim_uid_set;           /* TRUE if exim_uid set */
extern BOOL   expand_forbid_exists;   /* Lock out exists checking */
extern BOOL   expand_forbid_lookup;   /* Lock out lookups */
extern BOOL   expand_forbid_perl;     /* Lock out Perl calls */
extern int    expand_nlength[];       /* Lengths of numbered strings */
extern int    expand_nmax;            /* Max numerical value */
extern char  *expand_nstring[];       /* Numbered strings */
extern BOOL   expand_string_forcedfail; /* TRUE if failure was "expected" */
extern char  *expand_string_message;  /* Error info for failing expansion */
extern BOOL   extract_addresses_remove_arguments; /* Controls -t behaviour */

extern char  *filter_fmsg;            /* Message for freeze or fail */
extern int    filter_n[FILTER_VARIABLE_COUNT]; /* filter variables */
extern BOOL   filter_running;         /* TRUE while running a filter */
extern int    filter_sn[FILTER_VARIABLE_COUNT]; /* variables set by system filter */
extern char  *filter_test;            /* Run as a filter tester on this file */
extern char  *filter_thisaddress;     /* For address looping */
extern int    finduser_retries;       /* Retry count for getpwnam() */
extern BOOL   forbid_domain_literals; /* As it says */
extern BOOL   freeze_tell_mailmaster; /* Message on each freeze */

extern char  *gecos_name;             /* To be expanded when pattern matches */
extern char  *gecos_pattern;          /* Pattern to match */
extern rewrite_rule *global_rewrite_rules;  /* Chain of rewriting rules */

extern BOOL   have_seteuid;           /* TRUE if system has seteuid */
extern BOOL   header_changed;         /* TRUE if header data has changed */
extern header_line *header_last;      /* Last header in chain */
extern header_line *header_list;      /* Chain of header lines */
extern header_name *header_names;     /* Adjusted for presence of resent- */
extern header_name header_names_normal[];  /* to one of */
extern header_name header_names_resent[];  /* these */
extern BOOL   headers_check_syntax;   /* Check From, To, etc. */
extern BOOL   headers_checks_fail;    /* If FALSE, just warn */
extern BOOL   headers_sender_verify;  /* TRUE to verify sender from headers */
extern BOOL   headers_sender_verify_errmsg; /* ditto, only if errmsg */
extern char  *helo_accept_junk_hosts; /* Allowed to use junk args */
extern BOOL   helo_strict_syntax;     /* Allows _ in name if false */
extern char  *helo_verify;            /* Check HELO argument */
extern char  *hold_domains;           /* Hold up deliveries to these */
extern char  *host_accept_relay;      /* Accept relay only from these hosts */
extern BOOL   host_checking;          /* Set when checking a host */
extern BOOL   host_find_failed_syntax;/* DNS syntax check failure */
extern char  *host_lookup;            /* For which IP addresses are always looked up */
extern BOOL   host_lookup_failed;     /* TRUE if lookup failed */
extern char  *host_lookup_msg;        /* Text for why it failed */
extern int    host_number;            /* For sharing spools */
extern char  *host_number_string;     /* For expanding */
extern char  *host_reject;            /* Reject from these hosts */
extern char  *host_reject_recipients; /* Reject RCPT from these hosts */
extern char  *hosts_treat_as_local;   /* For routing */

extern BOOL   ignore_errmsg_errors;   /* As it says */
extern int    ignore_errmsg_errors_after; /* Or keep them for this time. */
extern BOOL   ignore_fromline_local;  /* Local SMTP ignore fromline */
extern char  *ignore_fromline_hosts;  /* Hosts permitted to send "From " */
extern char  *interface_address;      /* Interface on incoming call */
extern BOOL   ipv4_address_lookup;    /* If TRUE, don't look for AAAA & A6 */

extern int    journal_fd;             /* Fd for journal file */

extern int    keep_malformed;         /* Time to keep malformed messages */
extern BOOL   kill_ip_options;        /* For flattening source routing */

extern int    load_average;           /* Most recently read load average */
extern char  *local_domains;          /* List of local domains */
extern BOOL   local_domains_include_host; /* Always include local host */
extern BOOL   local_domains_include_host_literals; /* Plus literals */
extern BOOL   local_error_message;    /* True if handling one of these */
extern BOOL   local_from_check;       /* For adding Sender: */
extern char  *local_from_prefix;      /* Permitted prefixes */
extern char  *local_from_suffix;      /* Permitted suffixes */
extern char  *local_interfaces;       /* For forcing specific interfaces */
extern ip_address_item *local_interface_data; /* List of local interface addresses */
extern char  *local_part_data;        /* From local part lookup */
extern BOOL   locally_caseless;       /* Local local parts are caseless */
extern BOOL   log_all_parents;        /* As it says */
extern BOOL   log_arguments;          /* Logs Exim's args */
extern char  *log_buffer;             /* For constructing log entries */
extern char  *log_file_path;          /* If unset, use default */
extern BOOL   log_incoming_port;      /* For logging incoming port numbers */
extern BOOL   log_ip_options;         /* For logging IP source routing */
extern int    log_level;              /* Level of logging info */
extern int    log_queue_run_level;    /* Log level for start/end */
extern BOOL   log_received_sender;    /* Before rewriting */
extern BOOL   log_received_recipients; /* As it says (before rewriting) */
extern BOOL   log_refused_recipients; /* Sic */
extern BOOL   log_rewrites;           /* Sic */
extern BOOL   log_sender_on_delivery; /* Sic */
extern BOOL   log_smtp_confirmation;  /* Log response after "." */
extern BOOL   log_smtp_connections;   /* Verbose connection logging */
extern BOOL   log_smtp_syntax_errors; /* As it says (for incoming smtp) */
extern FILE  *log_stderr;             /* Copy of stderr for log use, or NULL */
extern BOOL   log_subject;            /* As it says */
extern char  *login_sender_address;   /* The actual sender address */
extern char  *lookup_key;             /* For query expansion */
extern lookup_info lookup_list[];     /* Vector of available lookups */
extern int    lookup_open_max;        /* Max lookup files to cache */
extern char  *lookup_value;           /* Value looked up from file */

extern macro_item *macros;            /* Configuration macros */
extern int    max_username_length;    /* For systems with broken getpwnam() */
extern char  **mailer_argv;           /* Arg list for sending message */
extern int    message_age;            /* In seconds */
extern char  *message_body;           /* Start of message body for filter */
extern char  *message_body_end;       /* End of message body for filter */
extern int    message_body_size;      /* Sic */
extern int    message_body_visible;   /* Amount visible in message_body */
extern int    message_ended;          /* State of message reading and how ended */
extern char  *message_filter;         /* Name of system filter file */

/* Special transports for message filters (cf address_xxx, which are done
differently) */

extern char  *message_filter_directory_transport;
extern char  *message_filter_directory2_transport;
extern char  *message_filter_file_transport;
extern char  *message_filter_pipe_transport;
extern char  *message_filter_reply_transport;

extern gid_t  message_filter_gid;     /* Gid for running system filter */
extern BOOL   message_filter_gid_set; /* TRUE if gid set */
extern uid_t  message_filter_uid;     /* Uid for running system filter */
extern BOOL   message_filter_uid_set; /* TRUE if uid set */
extern char   message_id_option[];    /* -E<message-id> for use as option */
extern char  *message_id_external;    /* External form of following */
extern char  *message_id;             /* Internal id of message being handled */
extern char  *message_id_text;        /* Expanded to form message_id */
extern int    message_linecount;      /* As it says */
extern FILE  *message_log;            /* Open message log while delivering */
extern char  *message_precedence;     /* From Precedence: header */
extern int    message_size;           /* Size of message */
extern int    message_size_limit;     /* As it says */
extern BOOL   message_size_limit_count_recipients; /* As it says */
extern char   message_subdir[];       /* Subdirectory for messages */
extern char  *message_reference;      /* Reference for error messages */

extern uid_t *never_users;            /* List of uids never to be used */
extern gid_t  nobody_gid;             /* Safe, unprivileged, gid */
extern uid_t  nobody_uid;             /* Safe, unprivileged, uid */
extern BOOL   nobody_gid_set;         /* TRUE if nobody_gid set */
extern BOOL   nobody_uid_set;         /* TRUE if nobody_uid set */

extern optionlist optionlist_auths[];
extern int    optionlist_auths_size;
extern optionlist optionlist_directors[];  /* These option lists are made */
extern int    optionlist_directors_size;   /* global so that readconf can */
extern optionlist optionlist_routers[];    /* see them for printing out   */
extern int    optionlist_routers_size;     /* the options.                */
extern optionlist optionlist_transports[];
extern int    optionlist_transports_size;

extern uid_t  original_euid;          /* Original effective uid */
extern gid_t  originator_gid;         /* Gid of whoever wrote spool file */
extern char  *originator_login;       /* Login of same */
extern char  *originator_name;        /* Full name of same */
extern uid_t  originator_uid;         /* Uid of ditto */

extern BOOL   parse_allow_group;      /* Allow group syntax */
extern BOOL   parse_found_group;      /* In the middle of a group */
extern char  *percent_hack_domains;   /* Local domains for which '% operates */
extern char  *pid_file_path;          /* For writing daemon pids */
extern BOOL   preserve_message_logs;  /* Save msglog files */
extern char  *primary_hostname;       /* Primary name of this computer */
extern char  *primary_hostname_lc;    /* ... lower cased */
extern BOOL   print_topbitchars;      /* Topbit chars are printing chars */
extern char   process_info[];         /* For SIGUSR1 output */
extern BOOL   prod_requires_admin;    /* TRUE if prodding requires admin */
extern char  *prohibition_message;    /* Tailorable message */
extern char  *prohibition_reason;     /* Set while expanding message */

extern char  *qualify_domain_recipient; /* Domain to qualify recipients with */
extern char  *qualify_domain_sender;  /* Domain to qualify senders with */
extern BOOL   queue_2stage;           /* Run queue in 2-stage manner */
extern BOOL   queue_list_requires_admin; /* TRUE if -bp requires admin */
extern BOOL   queue_run_force;        /* TRUE to force during queue run */
extern BOOL   queue_run_local;        /* Local deliveries only in queue run */
extern BOOL   queue_running;          /* TRUE for queue running process and */
                                      /*   immediate children */
extern pid_t  queue_run_pid;          /* PID of the queue running process or 0 */
extern int    queue_run_pipe;         /* Pipe for synchronizing */
extern int    queue_interval;         /* Queue running interval */
extern BOOL   queue_only;             /* TRUE to disable immediate delivery */
extern int    queue_only_load;        /* max load before auto-queue */
extern char  *queue_only_file;        /* queue if file exists/not-exists */
extern BOOL   queue_remote;           /* Queue all remote deliveries (-odqr) */
extern char  *queue_remote_domains;   /* Queue these remote domains domains */
extern BOOL   queue_run_in_order;     /* As opposed to random */
extern int    queue_run_max;          /* Max queue runners */
extern BOOL   queue_smtp;             /* Disable immediate STMP (-odqs)*/
extern char  *queue_smtp_domains;     /* Ditto, for these domains */

extern unsigned int random_seed;      /* Seed for random numbers */
extern char  *raw_sender;             /* Before rewriting */
extern char **raw_recipients;         /* Before rewriting */
extern char  *rbl_domain;             /* RBL domain that caused rejection */
extern char  *rbl_domains;            /* Realtime Block List domains */
extern char  *rbl_hosts;              /* Hosts for which RBL is used */
extern header_line *rbl_header;       /* RBL header to add */
extern BOOL   rbl_host;               /* True if RBL host but not rejected */
extern BOOL   rbl_log_rcpt_count;
extern BOOL   rbl_log_headers;
extern char  *rbl_msg_buffer;         /* Pointer to TXT data */
extern BOOL   rbl_reject_recipients;  /* TRUE to reject recipients if found */
extern BOOL   rbl_warn_header;        /* Add header if warning */
extern gid_t  real_gid;               /* Real gid */
extern uid_t  real_uid;               /* Real user running program */
extern BOOL   really_exim;            /* FALSE in utilities */
extern int    received_count;         /* Count of Received: headers */
extern char  *received_for;           /* For "for" field */
extern char  *received_header_text;   /* Definition of Received: header */
extern int    received_headers_max;   /* Max count of Received: headers */
extern char  *received_protocol;      /* Name of incoming protocol */
extern int    received_time;          /* Time the message was received */
extern BOOL   receiver_try_verify;    /* Accept if soft error while verifying */
extern char  *receiver_unqualified_hosts; /* Permitted unqualified receivers */
extern BOOL   receiver_verify;        /* Verify receivers when received */
extern char  *receiver_verify_addresses; /* ... but only these addresses */
extern char  *receiver_verify_hosts;  /* Verify for these hosts */
extern char  *receiver_verify_senders;/* Verify only from these senders */
extern int    recipients_count;       /* Number of recipients */
extern recipient_item *recipients_list; /* List of recipient addresses */
extern int    recipients_list_max;    /* Maximum number fitting in list */
extern int    recipients_max;         /* Max permitted */
extern int    recipients_max_reject;  /* If TRUE, reject whole message */
extern char  *recipients_reject_except; /* Exceptions for policy rejections */
extern char  *recipients_reject_except_senders; /* Exceptions for policy rejections */
extern BOOL   refuse_all_rcpts;       /* Flag for SMTP verifying */
extern BOOL   refuse_ip_options;      /* Refuse source routing */
extern pcre  *regex_check_dns_names;  /* For DNS name checking */
extern pcre  *regex_From;             /* For recognizing "From_" lines */
extern pcre  *regex_PIPELINING;       /* For recognizing PIPELINING */
extern pcre  *regex_SIZE;             /* For recognizing SIZE settings */
extern pcre  *regex_ismsgid;          /* Compiled r.e. for message it */
extern char  *relay_domains;          /* Official relay domains */
extern BOOL   relay_domains_include_local_mx; /* Include all MXed to us */
extern BOOL   relay_need_either;      /* True for OR host, sender */
extern int    remote_max_parallel;    /* Maximum parallel delivery */
extern char  *remote_sort;            /* Remote domain sorting order */
extern retry_config *retries;         /* Chain of retry config information */
extern int    retry_data_expire;      /* When to expire retry data */
extern int    retry_interval_max;     /* Absolute maximum */
extern int    retry_maximum_timeout;  /* The maximum timeout */
extern char  *return_path;            /* Return path for a message */
extern BOOL   return_path_remove;     /* Remove return-path headers */
extern int    return_size_limit;      /* Limit messages returned to sender */
extern int    rewrite_existflags;     /* Indicate which headers have rewrites */
extern char  *rfc1413_hosts;          /* RFC hosts */
extern int    rfc1413_query_timeout;  /* Timeout on RFC 1413 calls */
extern BOOL   rfc821_domains;         /* If set, syntax is 821, not 822 */
extern uid_t  root_uid;               /* The uid for root */
extern char  *route_option;           /* Option string for some routers */
extern router_info routers_available[]; /* Vector of available routers */
extern router_instance *routers;      /* Chain of instantiated routers */
extern router_instance router_defaults; /* Default values */
extern BOOL   running_in_test_harness;/* TRUE when running_status is patched */
extern char  *running_status;         /* Flag string for testing */

extern BOOL   search_find_defer;      /* Set TRUE if lookup defers */
extern int    security_level;         /* For the set(e)uid stuff */
extern char  *security_type;          /* Text name for above, set by config */
extern char  *self_hostname;          /* Self host after routing->directors */
extern char  *sender_address;         /* Envelope sender */
extern BOOL   sender_address_forced;  /* Set by -f */
extern char  *sender_address_relay;   /* Only non-nearby relay from these addresses */
extern char  *sender_address_relay_hosts; /* ... if from these hosts */
extern char *sender_address_unrewritten; /* Set of verify rewrites */
extern char  *sender_fullhost;        /* Sender host name + address */
extern char  *sender_helo_name;       /* Host name from HELO/EHLO */
extern char  *sender_host_address;    /* IP address of sender, as chars */
extern char **sender_host_aliases;    /* Points to list of alias names */
extern char  *sender_host_authenticated; /* Name of authentication mechanism */
extern char  *sender_host_name;       /* Host name from lookup, or "?" */
extern BOOL   sender_host_notsocket;  /* Set for -bs and -bS */
extern int    sender_host_port;       /* Port number of sender */
extern BOOL   sender_host_unknown;    /* TRUE for -bs and -bS except inetd */
extern char  *sender_ident;           /* Sender identity via RFC 1413 */
extern BOOL   sender_local;           /* TRUE for local senders */
extern char  *sender_rcvhost;         /* Host data for Received: */
extern char  *sender_reject;          /* Spam filter list */
extern char  *sender_reject_recipients; /* Ditto, reject RCPT TO */
extern BOOL   sender_set_untrusted;   /* Sender set by untrusted caller */
extern BOOL   sender_try_verify;      /* Accept if soft error while verifying */
extern char  *sender_unqualified_hosts; /* Permitted unqualified senders */
extern BOOL   sender_verify;          /* TRUE when verifying senders */
extern BOOL   sender_verify_batch;    /* TRUE if batch SMTP included */
extern char  *sender_verify_callback_domains; /* For SMTP callbacks */
extern char  *sender_verify_callback_error;   /* ditto - holds error */
extern int    sender_verify_callback_timeout; /* ditto */
extern BOOL   sender_verify_fixup;    /* Fix broken senders from headers */
extern char  *sender_verify_hosts;    /* Verification hosts */
extern char  *sender_verify_hosts_callback; /* For SMTP callback */
extern BOOL   sender_verify_reject;   /* Reject if verify fails */
extern int    sender_verify_max_retry_rate; /* Max retry rate */
extern volatile BOOL sigalrm_seen;    /* Flag for sigalrm_handler */
extern char **sighup_argv;            /* Args for re-execing after SIGHUP */
extern int    smtp_accept_count;      /* Count of connections */
extern BOOL   smtp_accept_keepalive;  /* Set keepalive on incoming */
extern int    smtp_accept_max;        /* Max SMTP connections */
extern int    smtp_accept_max_per_host; /* Max SMTP cons from one host/addr */
extern int    smtp_accept_queue;      /* Queue after so many connections */
extern int    smtp_accept_queue_per_connection; /* Queue after so many msgs */
extern int    smtp_accept_reserve;    /* Reserve these SMTP connections */
extern char  *smtp_banner;            /* Banner string (to be expanded) */
extern BOOL   smtp_batched_input;     /* TRUE if SMTP batch (no interaction) */
extern BOOL   smtp_check_spool_space; /* TRUE to check SMTP SIZE value */
extern int    smtp_connect_backlog;   /* Max backlog permitted */
extern char  *smtp_etrn_command;      /* Command to run */
extern char  *smtp_etrn_hosts;        /* Accept ETRN only from these hosts */
extern BOOL   smtp_etrn_serialize;    /* Only one at once */
extern char  *smtp_expn_hosts;        /* Accept EXPN only from these hosts */
extern FILE  *smtp_in;                /* Incoming SMTP input file */
extern BOOL   smtp_input;             /* TRUE if input is via SMTP */
extern int    smtp_load_reserve;      /* Only from reserved if load > this */
extern FILE  *smtp_out;               /* Incoming SMTP output file */
extern int    smtp_port;              /* Alternate SMTP port number */
extern char  *smtp_read_error;        /* Message for SMTP input error */
extern int    smtp_receive_timeout;   /* Applies to each received line */
extern char  *smtp_reserve_hosts;     /* Hosts for reserved slots */
extern BOOL   smtp_use_pipelining;    /* Global for passed connections */
extern BOOL   smtp_use_size;          /* Global for passed connections */
extern BOOL   smtp_verify;            /* TRUE if VRFY permitted */
extern BOOL   split_spool_directory;  /* TRUE to use multiple subdirs */
extern char  *spool_directory;        /* Name of spool directory */
extern char  *stderr_filename;        /* File for use with -df */
extern int    store_pool;             /* Current pool number */
extern BOOL   strip_excess_angle_brackets; /* Surrounding route-addrs */
extern BOOL   strip_trailing_dot;     /* Remove dots at ends of domains */
extern BOOL   syslog_timestamp;       /* TRUE for timestamps on syslogs */

extern int    timeout_frozen_after;   /* Max time to keep frozen messages */
extern BOOL   timestamps_utc;         /* Use UTC for all times */

extern transport_instance *transport_message_filter_directory;
extern transport_instance *transport_message_filter_directory2;
extern transport_instance *transport_message_filter_file;
extern transport_instance *transport_message_filter_pipe;
extern transport_instance *transport_message_filter_reply;

extern int    transport_count;        /* Count of bytes transported */
extern char **transport_filter_argv;  /* For on-the-fly filtering */
extern transport_info transports_available[]; /* Vector of available transports */
extern transport_instance *transports; /* Chain of instantiated transports */
extern transport_instance transport_defaults; /* Default values */

extern int    transport_write_timeout;/* Set to time out individual writes */

extern tree_node *tree_dns_fails;     /* Tree of DNS lookup failures */
extern tree_node *tree_duplicates;    /* Tree of duplicate addresses */
extern tree_node *tree_nonrecipients; /* Tree of nonrecipient addresses */
extern tree_node *tree_unusable;      /* Tree of unusable addresses */

extern BOOL   trusted_caller;         /* Caller is trusted */
extern gid_t *trusted_groups;         /* List of trusted groups */
extern uid_t *trusted_users;          /* List of trusted users */
extern char  *timezone_string;        /* Required timezone setting */

extern char  *unknown_login;          /* To use when login id unknown */
extern char  *unknown_username;       /* Ditto */
extern BOOL   untrusted_set_sender;   /* Let untrusted users set sender */
extern char  *uucp_from_pattern;      /* For recognizing "From " lines */
extern char  *uucp_from_sender;       /* For building the sender */

extern char  *warnmsg_delay;          /* String form of delay time */
extern char  *warnmsg_file;           /* Template for warning messages */
extern char  *warnmsg_recipients;     /* Recipients of warning message */

extern char  *verify_address_parse_error; /* Sic */
extern char  *verify_forced_errmsg;   /* Message from :defer: or :fail: */
extern BOOL   verify_only;            /* TRUE for -bv */
extern char  *version_copyright;      /* Copyright notice */
extern char  *version_date;           /* Date of compilation */
extern char  *version_cnumber;        /* Compile number */
extern char  *version_string;         /* Version string */

extern int    warning_count;          /* Delay warnings sent for this msg */

/* End of globals.h */
