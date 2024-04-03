/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* Definitions of various structures. We have to pre-declare some because of
mutually recursive definitions in the auths, directors, routers, and transports
blocks. */

struct address_item;
struct auth_info;
struct driver_info;
struct director_info;
struct smtp_inblock;
struct smtp_outblock;
struct transport_info;
struct router_info;

/* Structure for remembering macros for the configuration file */

typedef struct macro_item {
  struct macro_item *next;
  BOOL  command_line;
  char *replacement;
  char name[1];
} macro_item;

/* Entries in lists of main and driver options are in this form. */

typedef struct {
  char *name;
  int   type;
  void *value;
} optionlist;

/* Block for holding a uid and gid, possibly unset, and an initgroups flag. */

typedef struct ugid_block {
  uid_t uid;
  gid_t gid;
  BOOL  uid_set;
  BOOL  gid_set;
  BOOL  initgroups;
} ugid_block;

/* Structure for holding information about a host for use mainly by
routers, but also used when checking lists of hosts and when transporting.
Looking up host addresses is done using this structure. */

typedef struct host_item {
  struct host_item *next;
  char *name;
  char *address;
  /* Used when routing */
  int   mx;                      /* MX value if found via MX records */
  int   sort_key;                /* MX*100 plus random "fraction" */
  int   status;                  /* Usable, unusable, or unknown */
  int   why;                     /* Why host is unusable */
  int   last_try;                /* Time of last try if known */
  BOOL  update_waiting;          /* Turned off if wait db not to be updated */
  /* Used when checking */
  char *ident_string;            /* RFC 1413 ident string */
} host_item;

/* Chain of rewrite rules, read from the rewrite config, or parsed from the
rewrite_headers field of a transport. */

typedef struct rewrite_rule {
  struct rewrite_rule *next;
  int flags;
  char *key;
  char *replacement;
} rewrite_rule;

/* Directors, routers, and transports have similar data blocks. Each driver
that is compiled into the code is represented by a xxx_info block; the active
drivers are represented by a chain of xxx_instance blocks. To make it possible
to use the same code for reading the configuration files for all three, the
layout of the start of the blocks is kept the same, and represented by the
generic structures driver_info and driver_instance. The specific structures
have the right types for the main entry point functions; we use void * in the
generic structure to match all types. */

typedef struct driver_instance {
  struct driver_instance *next;
  char *name;                    /* Instance name */
  struct driver_info *info;      /* Points to info for this driver */
  void *options_block;           /* Pointer to private options */
  char *driver_name;             /* All start with this generic option */
} driver_instance;

typedef struct driver_info {
  char *driver_name;             /* Name of driver */
  void *code;                    /* Main entry point of driver */
  void (*init)(                  /* Initialization entry point */
    struct driver_instance *);
  void (*tidyup)(                /* Tidyup entry point */
    struct driver_instance *);
  optionlist *options;           /* Table of private options names */
  int  *options_count;           /* -> Number of entries in table */
  void *options_block;           /* Points to default private block */
  int   options_len;             /* Length of same in bytes */
} driver_info;


/* Structure for holding information about the configured transports. Some
of the generally accessible options are set from the configuration file; others
are set by transport initialization, since they can only be set for certain
transports. They need to be generally accessible, however, as they are used by
the main transport code. */

typedef struct transport_instance {
  struct transport_instance *next;
  char *name;                    /* Instance name */
  struct transport_info *info;   /* Info for this driver */
  void *options_block;           /* Pointer to private options */
  char *driver_name;             /* Must be first */
  int  (*setup)(                 /* Setup entry point for local transports */
    struct transport_instance *,
    struct address_item *,
    char **);
                                 /**************************************/
  int   local_batch;             /* )                                  */
  int   local_smtp;              /* )                                  */
  int   batch_max;               /* )                                  */
  BOOL  uid_set;                 /* )                                  */
  BOOL  gid_set;                 /* )                                  */
  uid_t uid;                     /* )                                  */
  gid_t gid;                     /* ) For those transports that change */
  char *expand_uid;              /* )   state - local transports.      */
  char *expand_gid;              /* )                                  */
  char *home_dir;                /* )                                  */
  char *current_dir;             /* )                                  */
  char *warn_message;            /* ) Text for the message             */
  BOOL  deliver_as_creator;      /* )                                  */
  BOOL  initgroups;              /* )                                  */
  BOOL  bsmtp_helo;              /* )                                  */
                                 /**************************************/
  BOOL  multi_domain;            /* Applies to remote transports only  */
  BOOL  overrides_hosts;         /* Ditto                              */
                                 /**************************************/
  struct transport_instance *shadow;  /* Shadow, to use on success. */
  char *shadow_condition;        /* Condition for running it */
  char *filter_command;          /* For on-the-fly-filtering */
  char *add_headers;             /* Add these headers */
  char *remove_headers;          /* Remove these headers */
  char *return_path;             /* Overriding (rewriting) return path */
  char *debug_string;            /* Debugging output */
  char *headers_rewrite;         /* Rules for rewriting headers */
  rewrite_rule *rewrite_rules;   /* Parsed rewriting rules */
  int   rewrite_existflags;      /* Bits showing which headers are rewritten */
  BOOL  body_only;               /* Deliver only the body */
  BOOL  delivery_date_add;       /* Add Delivery-Date header */
  BOOL  envelope_to_add;         /* Add Envelope-To header */
  BOOL  headers_only;            /* Deliver only the headers */
  BOOL  return_path_add;         /* Add Return-Path header */
  BOOL  return_output;           /* TRUE if output should always be returned */
  BOOL  return_fail_output;      /* ditto, but only on failure */
  BOOL  log_output;              /* Similarly for logging */
  BOOL  log_fail_output;
  BOOL  log_defer_output;
  BOOL  retry_use_local_part;    /* Defaults true for local, false for remote */
  int   max_addresses;           /* Max amalgamation for remote delivery */
  int   message_size_limit;      /* Biggest message this transport handles */
} transport_instance;


/* Structure for holding information about a type of transport. */

typedef struct transport_info {
  char *driver_name;             /* Driver name */
  BOOL (*code)(
    transport_instance *,
    struct address_item *);
  void (*init)(
    struct transport_instance *);
  void (*tidyup)(
    struct transport_instance *);
  optionlist *options;           /* Table of private options names */
  int  *options_count;           /* -> Number of entries in table */
  void *options_block;           /* Points to default private block */
  int   options_len;             /* Length of same in bytes */
  BOOL  local;                   /* TRUE for local transports */
  void (*closedown)(             /* For closing down a passed channel */
    struct transport_instance *);
} transport_info;


/* Structure for holding information about the configured directors. */

typedef struct director_instance {
  struct director_instance *next;
  char *name;
  struct director_info *info;
  void *options_block;           /* Pointer to private options */
  char *driver_name;             /* Must be first */

  /********************************************************************/
  /* This set of variables must be exactly the same for directors and */
  /* routers, so that common code can be used. Some of the others are */
  /* similarly named and have the same function, but don't share code */
  /* so they aren't in this block.                                    */
  /********************************************************************/
  char *domains;                 /* Specific domains */
  char *local_parts;             /* Specific local parts */
  char *errors_to;               /* Errors address */
  char *extra_headers;           /* Additional headers */
  char *remove_headers;          /* Those to remove */
  char *debug_string;            /* Debugging output */
  char *fallback_hosts;          /* For remote transports */
  host_item *fallback_hostlist;
  BOOL  more;                    /* If FALSE, do no more if this one fails */
  BOOL  verify_only;             /* Skip this director if not verifying */
  BOOL  verify_sender;           /* Use this director when verifying a sender */
  BOOL  verify_recipient;        /* Use this director when verifying a recipient*/
  char *expand_uid;              /* Expanded uid string */
  char *expand_gid;              /* Expanded gid string */
  uid_t uid;                     /* Fixed uid value */
  gid_t gid;                     /* Fixed gid value */
  BOOL  uid_set;                 /* Flag to indicate uid is set */
  BOOL  gid_set;                 /* Flag to indicate gid is set */
  BOOL  initgroups;              /* True if initgroups is required */
  /********************************************************************/

  transport_instance *transport; /* Assigned transport */
  char *expand_transport;        /* Dynamic transport string */
  char *current_directory;       /* For use during delivery */
  char *home_directory;          /* Ditto */
  char *condition;               /* General condition */
  char *prefix;                  /* Address prefix */
  char *suffix;                  /* Address suffix */
  char *senders;                 /* Specific senders */
  char *require_files;           /* Required (or not) for director to run */
  char *new_director;            /* Name of director for generated address */
  struct director_instance *new; /* Actual director for generated address */
  BOOL  unseen;                  /* If TRUE, carry on even after success */
  BOOL  prefix_optional;         /* Just what it says */
  BOOL  suffix_optional;         /* Ditto */
  BOOL  fail_verify_sender;      /* Fail verify if sender match this director */
  BOOL  fail_verify_recipient;   /* Fail verify if recipient match this director */
  BOOL  expn;                    /* Use this director when processing EXPN */
  BOOL  one_continue;            /* Continue verify if generated just one child */
} director_instance;


/* Structure for holding information about a type of director. */

typedef struct director_info {
  char *driver_name;
  int (*code)(
    director_instance *,
    struct address_item *,
    struct address_item **,
    struct address_item **,
    struct address_item **,
    struct address_item **,
    BOOL);
  void (*init)(
    struct director_instance *);
  void (*tidyup)(
    struct director_instance *);
  optionlist *options;           /* Table of private options names */
  int  *options_count;           /* -> Number of entries in table */
  void *options_block;           /* Points to default private block */
  int   options_len;             /* Length of same in bytes */
} director_info;


/* Structure for holding information about the configured routers. */

typedef struct router_instance {
  struct router_instance *next;
  char *name;
  struct router_info *info;
  void *options_block;           /* Pointer to private options */
  char *driver_name;             /* Must be first */

  /********************************************************************/
  /* This set of variables must be exactly the same for directors and */
  /* routers, so that common code can be used. Some of the others are */
  /* similarly named and have the same function, but don't share code */
  /* so they aren't in this block.                                    */
  /********************************************************************/
  char *domains;                 /* Specific domains */
  char *local_parts;             /* Specific local parts */
  char *errors_to;               /* Errors address */
  char *extra_headers;           /* Additional headers */
  char *remove_headers;          /* Removed headers */
  char *debug_string;            /* Debugging output */
  char *fallback_hosts;          /* For remote transports */
  host_item *fallback_hostlist;
  BOOL  more;                    /* If FALSE, do no more if this one fails */
  BOOL  verify_only;             /* Skip this router if not verifying */
  BOOL  verify_sender;           /* Use this router when verifying a sender */
  BOOL  verify_recipient;        /* Use this router when verifying a recipient*/
  char *expand_uid;              /* Expanded uid string */
  char *expand_gid;              /* Expanded gid string */
  uid_t uid;                     /* Fixed uid value */
  gid_t gid;                     /* Fixed gid value */
  BOOL  uid_set;                 /* Flag to indicate uid is set */
  BOOL  gid_set;                 /* Flag to indicate gid is set */
  BOOL  initgroups;              /* True if initgroups is required */
  /********************************************************************/

  transport_instance *transport; /* Assigned transport */
  char *expand_transport;        /* Dynamic transport string */
  char *condition;               /* General condition */
  char *senders;                 /* Specific senders */
  char *require_files;           /* Files required (or not) for router to run */
  BOOL  unseen;                  /* If TRUE carry on, even after success */
  BOOL  fail_verify_sender;      /* Fail verify if sender match this router */
  BOOL  fail_verify_recipient;   /* Fail verify if recipient match this router */
  BOOL  pass_on_timeout;         /* Treat timeout DEFERs as fails */
  char *self;                    /* Text option for handling self reference */
  int   self_code;               /* Encoded version of same */
  BOOL  self_rewrite;            /* TRUE to rewrite headers if making local */
  BOOL  sender_rewrite;          /* TRUE to rewrite verified sender */
  char *translate_ip_address;    /* IP address translation fudgery */
  char *ignore_target_hosts;     /* Target hosts to ignore */
} router_instance;


/* Structure for holding information about a type of router. */

typedef struct router_info {
  char *driver_name;
  int (*code)(
    router_instance *,
    struct address_item *,
    struct address_item **,
    struct address_item **,
    BOOL,
    char *);
  void (*init)(
    struct router_instance *);
  void (*tidyup)(
    struct router_instance *);
  optionlist *options;           /* Table of private options names */
  int  *options_count;           /* -> Number of entries in table */
  void *options_block;           /* Points to default private block */
  int   options_len;             /* Length of same in bytes */
} router_info;


/* Structure for holding information about a lookup type. */

typedef struct lookup_info {
  char *name;                    /* e.g. "lsearch" */
  int type;                      /* query/singlekey/abs-file */
  void *(*open)(                 /* open function */
    char *,                      /* file name for those that have one */
    char **);                    /* for error message */
  BOOL (*check)(                 /* file checking function */
    void *,                      /* handle */
    char *,                      /* file name */
    int,                         /* modemask for file checking */
    uid_t *,                     /* owners for file checking */
    gid_t *,                     /* owngroups for file checking */
    char **);                    /* for error messages */
  int (*find)(                   /* find function */
    void *,                      /* handle */
    char *,                      /* file name or NULL */
    char *,                      /* key or query */
    int,                         /* length of key or query */
    char **,                     /* for returning answer */
    char **);                    /* for error message */
  void (*close)(                 /* close function */
    void *);                     /* handle */
  void (*tidy)(void);            /* tidy function */
  char *(*quote)(                /* quoting function */
    char *);                     /* string to quote */
} lookup_info;


/* Structure for holding information about the configured authentication
mechanisms */

typedef struct auth_instance {
  struct auth_instance *next;
  char *name;                    /* Exim instance name */
  struct auth_info *info;        /* Pointer to driver info block */
  void *options_block;           /* Pointer to private options */
  char *driver_name;             /* Must be first */
  char *public_name;             /* Advertised name */
  char *set_id;                  /* String to set as authenticated id */
  char *mail_auth_condition;     /* Condition for AUTH on MAIL command */
  BOOL  client;                  /* TRUE if client option(s) set */
  BOOL  server;                  /* TRUE if server options(s) set */
} auth_instance;


/* Structure for holding information about an authentication mechanism */

typedef struct auth_info {
  char *driver_name;             /* e.g. "condition" */
  int (*servercode)(             /* server function */
    auth_instance *,             /* the instance data */
    char *);                     /* rest of AUTH command */
  void (*init)(                  /* initialization function */
    struct auth_instance *);
  int (*clientcode)(             /* client function */
    struct auth_instance *,
    struct smtp_inblock *,       /* socket and input buffer */
    struct smtp_outblock *,      /* socket and output buffer */
    int,                         /* command timeout */
    char *,                      /* buffer for reading response */
    int);                        /* sizeof buffer */
  optionlist *options;           /* Table of private options names */
  int  *options_count;           /* -> Number of entries in table */
  void *options_block;           /* Points to default private block */
  int   options_len;             /* Length of same in bytes */
} auth_info;


/* Structure for holding a single IP address; used for the chain of
addresses and ports for the local host. Make the char string large enough
to hold an IPv6 address. */

typedef struct ip_address_item {
  struct ip_address_item *next;
  char address[46];
} ip_address_item;


/* Information about a soft delivery failure, for use when calculating
retry information. It's separate from the address block, because there
can be a chain of them for SMTP deliveries where multiple IP addresses
can be tried. */

typedef struct retry_item {
  struct retry_item *next;       /* for chaining */
  char *destination;             /* string identifying destination */
  int   basic_errno;             /* error code for this destination */
  int   more_errno;              /* additional error information */
  char *message;                 /* local error message */
  int   flags;                   /* see below */
} retry_item;

/* Retry data flags */

#define rf_delete   0x0001       /* retry info is to be deleted */
#define rf_host     0x0002       /* retry info is for a remote host */
#define rf_message  0x0004       /* retry info is for a host+message */

/* Information about a constructed message that is to be sent using the
autoreply transport. This is pointed to from the address block. */

typedef struct reply_item {
  char *from;                    /* ) */
  char *reply_to;                /* ) */
  char *to;                      /* ) */
  char *cc;                      /* ) specific header fields */
  char *bcc;                     /* ) */
  char *subject;                 /* ) */
  char *headers;                 /* misc other headers, concatenated */
  char *text;                    /* text string body */
  char *file;                    /* file body */
  BOOL  file_expand;             /* expand the body */
  BOOL  forbid_exists;           /* things not allowed in expansion */
  BOOL  forbid_lookup;
  BOOL  forbid_perl;
  char *logfile;                 /* file to keep a log in */
  char *oncelog;                 /* file to keep records in for once only */
  time_t once_repeat;            /* time to repeat "once only" */
  BOOL  return_message;          /* send back the original message */
} reply_item;


/* Structure for maintaining a chain of header lines. */

typedef struct header_line {
  struct header_line *next;
  int type;
  int slen;
  char *text;
} header_line;


/*Structure for holding information about an envelope address */

typedef struct recipient_item {
  char *address;                 /* the recipient address */
  int   flags;                   /* whose names start rf_ */
  int   pno;                     /* parent number when it's a "fixed" alias */
  #ifdef SUPPORT_DSN
  char *orcpt;                   /* DSN orcpt field */
  #endif
} recipient_item;


/* The address_item structure contains many fields which are used at various
times while delivering a message. Some are used only for remote deliveries;
some only for local. */

typedef struct address_item {
  struct address_item *next;     /* for chaining addresses */
  struct address_item *parent;   /* parent address */
  struct address_item *shadow_parent; /* parent when shadow delivering */
  struct address_item *first;    /* point to first after group delivery */
  unsigned int flags;            /* a row of bits, defined below */
  char *onetime_parent;          /* saved original parent for onetime */
  char *orig;                    /* as read from the spool */
  char *unique;                  /* used for disambiguating .forwarded */
  char *local_part;              /* local part of envelope address */
  char *prefix;                  /* stripped prefix of local part */
  char *suffix;                  /* stripped suffix of local part */
  char *domain;                  /* domain of envelope address */
  char *route_domain;            /* domain to be used by routers */
  char *errors_address;          /* where to send errors (NULL => sender) */
  char *self_hostname;           /* after routing->directors */
  header_line *extra_headers;    /* additional headers */
  char *remove_headers;          /* list of those to remove */
  char *domain_data;             /* from "domains" lookup */
  char *local_part_data;         /* from "local_part" lookup */
  director_instance *start_director; /* generated address starts here */
  director_instance *director;   /* the director that directed */
  router_instance *router;       /* OR the router that routed */
  transport_instance *transport; /* transport to use */
  host_item *host_list;          /* host data for the transport */
  host_item *transported;        /* host that took delivery or failed hard */
  host_item *fallback_hosts;     /* to try if delivery defers */
  int  child_count;              /* number of child addresses */
  uid_t uid;                     /* uid for local transporting */
  gid_t gid;                     /* gid for local transporting */
  int  mode;                     /* mode for local transporting to a file */
  char *home_dir;                /* home directory for local transporting */
  char *current_dir;             /* current directory for local transporting */
  char *route_option;            /* routing option string */
  char **pipe_expandn;           /* numeric expansions for pipe from filter */
  int  transport_return;         /* result of delivery attempt */
  int  basic_errno;              /* status after failure */
  int  more_errno;               /* additional error information */
  char *message;                 /* local error message */
  char *shadow_message;          /* info about shadow transporting */
  int  special_action;           /* used when when deferred or failed; also */
                                 /* contains = or - when successful SMTP delivered */
  retry_item *retries;           /* chain of retry information */
  char *local_retry_key;         /* key for local transport retries */
  char *return_filename;         /* ) some transports write text into a file */
  int   return_file;             /* ) for return to the sender */
  reply_item *reply;             /* and some generate reply messages */

  #ifdef SUPPORT_TLS
  char *cipher;                  /* Cipher used for transport */
  char *peerdn;                  /* DN of server's certificate */
  #endif

  /* These are the remains of an attempt to put in some DSN support that
  was abandoned. */

  #ifdef SUPPORT_DSN
  char *dsn_orcpt;               /* DSN orcpt value */
  int  dsn_flags;                /* DSN flags */
  #endif
} address_item;

/* Bits for the flags field above */

#define af_allow_file          0x00000001 /* allow file in generated address */
#define af_allow_pipe          0x00000002 /* allow pipe in generated address */
#define af_allow_reply         0x00000004 /* allow autoreply in generated address */
#define af_dr_retry_exists     0x00000008 /* director or router retry record exists */
#define af_expand_pipe         0x00000010 /* expand pipe arguments */
#define af_forced_local        0x00000020 /* by a self option on a router */
#define af_gid_set             0x00000040 /* gid field is set */
#define af_ignore_error        0x00000080 /* ignore delivery error */
#define af_initgroups          0x00000100 /* use initgroups() for local transporting */
#define af_local               0x00000200 /* local/remote flag */
#define af_local_host_removed  0x00000400 /* local host was backup */
#define af_local_part_quoted   0x00000800 /* local part was quoted */
#define af_lt_retry_exists     0x00001000 /* local transport retry exists */
#define af_pfr                 0x00002000 /* pipe or file or reply delivery */
#define af_retry_skipped       0x00004000 /* true if retry caused some skipping */
#define af_retry_timedout      0x00008000 /* true if retry timed out */
#define af_rewrite_headers     0x00010000 /* set TRUE by routers that want it done */
#define af_uid_set             0x00020000 /* uid field is set */
#define af_hide_child          0x00040000 /* hide child in bounce/defer msgs */

/* Tables of normal and resent- header names consist of items of this type */

typedef struct {
  char *name;
  int   len;
} header_name;

/* Chain of information about errors (e.g. bad addresses) */

typedef struct error_block {
  struct error_block *next;
  char *text1;
  char *text2;
} error_block;

/* Chain of file names when processing the queue */

typedef struct queue_filename {
  struct queue_filename *next;
  char dir_char;
  char text[1];
} queue_filename;

/* Chain of items of retry information, read from the retry config. */

typedef struct retry_rule {
  struct retry_rule *next;
  int rule;
  int timeout;
  int p1;
  int p2;
} retry_rule;

typedef struct retry_config {
  struct retry_config *next;
  char *destination;
  int  basic_errno;
  int  more_errno;
  retry_rule *rules;
} retry_config;

/* Structure for each node in a tree, of which there are various kinds */

typedef struct tree_node {
  struct tree_node *left;         /* pointer to left child */
  struct tree_node *right;        /* pointer to right child */
  union
    {
    void  *ptr;                   /* pointer to data */
    int val;                      /* or integer data */
    } data;
  char balance;                   /* balancing factor */
  char name[1];                   /* node name - variable length */
} tree_node;

/* Structure for holding the handle and the cached last lookup for searches.
This block is pointed to by the tree entry for the file. The file can get
closed if too many are opened at once. There is a LRU chain for deciding which
to close. */

typedef struct search_cache {
  void *handle;                   /* lookup handle, or NULL if closed */
  int search_type;                /* search type */
  tree_node *up;                  /* LRU up pointer */
  tree_node *down;                /* LRU down pointer */
  char *key;                      /* the key of the last lookup */
  char *data;                     /* the data of the last lookup */
} search_cache;

/* Structure for holding a partially decoded DNS record; the name has been
uncompressed, but the data pointer is into the raw data. */

typedef struct {
  char  name[DNS_MAXNAME];        /* domain name */
  int   type;                     /* record type */
  int   size;                     /* size of data */
  uschar *data;                   /* pointer to data */
} dns_record;

/* Structure for holding the result of a DNS query. */

typedef struct {
  int   answerlen;                /* length of the answer */
  uschar answer[MAXPACKET];       /* the answer itself */
} dns_answer;

/* Structure for holding the intermediate data while scanning a DNS answer
block. */

typedef struct {
  int   rrcount;                  /* count of RRs in the answer */
  uschar *aptr;                   /* pointer in the answer while scanning */
  dns_record srr;                 /* data from current record in scan */
} dns_scan;

/* Structure for holding a chain of IP addresses that are extracted from
an A, AAAA, or A6 record. For the first two, there is only ever one address,
but the chaining feature of A6 allows for several addresses to be realized from
a single initial A6 record. The structure defines the address field of length
1. In use, a suitable sized block is obtained to hold the complete textual
address. */

typedef struct dns_address {
  struct dns_address *next;
  uschar address[1];
} dns_address;

/* Structure used for holding intermediate data during MD5 computations. */

typedef struct md5 {
  unsigned int length;
  unsigned int abcd[4];
  }
md5;

/* Structure used to hold incoming packets of SMTP responses for a specific
socket. The packets which may contain multiple lines (and in some cases,
multiple responses). */

typedef struct smtp_inblock {
  int sock;                       /* the socket */
  int buffersize;                 /* the size of the buffer */
  uschar *ptr;                    /* current position in the buffer */
  uschar *ptrend;                 /* end of data in the buffer */
  uschar *buffer;                 /* the buffer itself */
} smtp_inblock;

/* Structure used to hold buffered outgoing packets of SMTP commands for a
specific socket. The packets which may contain multiple lines when pipelining
is in use. */

typedef struct smtp_outblock {
  int sock;                       /* the socket */
  int cmd_count;                  /* count of buffered commands */
  int buffersize;                 /* the size of the buffer */
  uschar *ptr;                    /* current position in the buffer */
  uschar *buffer;                 /* the buffer itself */
} smtp_outblock;

/* End of structs.h */
