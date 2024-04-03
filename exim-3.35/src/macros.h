/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* These aren't macros, but they're the sort of general definition that fits in
this file and we need to have them defined early. Some operating systems
(naughtily, imo) include a definition for "uchar" in the standard header files,
so we use "uschar". Solaris has u_char in sys/types.h. This is just a typing
convenience, of course. */

typedef int BOOL;
typedef unsigned char uschar;

/* These macros save typing for the casting that is needed to cope with the
mess that is "char" in ISO/ANSI C. Having now been bitten enough times, I'm
working towards declaring everything explicitly unsigned - this transformation
is by no means complete, as it is just getting done as I go along. At the end
of the transformation, some of the casts that have been put in will strictly be
unnecessary, but they do no harm. I will try to have a tidy in due course. */

#define CS   (char *)
#define CSS  (char **)
#define US   (unsigned char *)
#define USS  (unsigned char **)

/* These two macros make it possible to obtain the result of macro-expanding
a string as a text string. This is sometimes useful for debugging output. */

#define mac_string(s) # s
#define mac_expanded_string(s) mac_string(s)


/* The address_item structure has a word full of 1-bit flags. These macros
manipulate them. */

#define setflag(addr,flag)    addr->flags |= (flag)
#define clearflag(addr,flag)  addr->flags &= ~(flag)

#define testflag(addr,flag)       ((addr->flags & (flag)) != 0)
#define testflagsall(addr,flag)   ((addr->flags & (flag)) == (flag))

#define copyflag(addrnew,addrold,flag) \
  addrnew->flags = (addrnew->flags & ~(flag)) | (addrold->flags & (flag))

#define orflag(addrnew,addrold,flag) \
  addrnew->flags |= addrold->flags & (flag)


/* We need a special return code for "no recipients and failed to send an error
message". ANSI C defines only EXIT_FAILURE and EXIT_SUCCESS. On the assumption
that these are always 1 and 0 on Unix systems ... */

#define EXIT_NORECIPIENTS 2


/* Character-handling macros. It seems that the set of standard functions in
ctype.h aren't actually all that useful. One reason for this is that email is
international, so the concept of using a locale to vary what they do is not
helpful. Another problem is that in different operating systems, the libraries
yield different results, even in the default locale. For example, Linux yields
TRUE for iscntrl() for all characters > 127, whereas many other systems yield
FALSE. For these reasons we define our own set of macros for a number of
character testing functions. Because it is so easy to define variables as char
rather than unsigned char, ensure that all these tests treat their arguments as
unsigned. */

#define mac_iscntrl(c) \
  ((uschar)(c) < 32 || (uschar)(c) == 127)

#define mac_iscntrl_or_special(c) \
  ((uschar)(c) < 32 || strchr(" ()<>@,;:\\\".[]\177", (uschar)(c)) != NULL)

#define mac_isgraph(c) \
  ((uschar)(c) > 32 && (uschar)(c) != 127)

#define mac_isprint(c) \
  (((uschar)(c) >= 32 && (uschar)(c) <= 126) || c == '\t' || \
  ((uschar)(c) > 127 && print_topbitchars))


/* When adding things to log lines, they come in pairs, as strings. This macro
packages up two calls to string_cat(). */

#define mac_cat2(s,size,ptr,s1,s2) \
  string_cat(string_cat(s,size,ptr,s1,(int)strlen(s1)),size,ptr,s2,(int)strlen(s2))


/* When built with TLS support, the act of flushing SMTP output becomes
a no-op once an SSL session is in progress. */

#ifdef SUPPORT_TLS
#define mac_smtp_fflush() if (!tls_active) fflush(smtp_out);
#else
#define mac_smtp_fflush() fflush(smtp_out);
#endif


/* Define which ends of pipes are for reading and writing, as some systems
don't make both fd's two-way. */

#define pipe_read  0
#define pipe_write 1

/* Define symbols for identifying the two store pools. */

#define POOL_MAIN 0
#define POOL_TEMP 1

/* Simplify taking the larger of two values */

#define mac_max(a, b) ((a) > (b) ? (a) : (b))

/* A macro to simplify testing bits in lookup types */

#define mac_islookup(a,b) ((lookup_list[a].type & (b)) != 0)

/* Some generalities */

#define DEBUG(x)      if ((x) <= debug_level)
#define HDEBUG(x)     if (host_checking || (x) <= debug_level)

#define FALSE         0
#define TRUE          1

/* The maximum permitted number of command-line (-D) macro definitions. We
need a limit only to make it easier to generate argument vectors for re-exec
of Exim. */

#define MAX_CLMACROS 10

/* The number of integer variables available in filter files. If this is
changed, then the tables in expand.c for accessing them must be changed too. */

#define FILTER_VARIABLE_COUNT 10

/* The size of the vector holding delay warning times */

#define DELAY_WARNING_SIZE 12

/* The size of the buffer holding the processing information string. */

#define PROCESS_INFO_SIZE 256

/* The size of buffer to get for constructing log entries. Make it big
enough to hold all the headers from a normal kind of message. */

#define LOG_BUFFER_SIZE 8192

/* The initial size of a big buffer for use in various places. It gets put
into big_buffer_size and in some circumstances increased. It should be at least
as long as the maximum path length. */

#if defined PATH_MAX && PATH_MAX > 1024
#define BIG_BUFFER_SIZE PATH_MAX
#elif defined MAXPATHLEN && MAXPATHLEN > 1024
#define BIG_BUFFER_SIZE MAXPATHLEN
#else
#define BIG_BUFFER_SIZE 1024
#endif

/* A limit to the length of an address. RFC 2821 limits the local part to 64
and the domain to 255, so this should be adequate, taking into account quotings
etc. */

#define ADDRESS_MAXLENGTH 512

/* The length of message identification strings. This is the id used internally
by exim. The external version for use in Received: strings has a leading 'E'
added to ensure it starts with a letter. */

#define MESSAGE_ID_LENGTH 16

/* The offset to the start of the data in the data file - this allows for
the name of the data file to be present in the first line. */

#define DATA_START_OFFSET (MESSAGE_ID_LENGTH+3)

/* The length of the base names of spool files, which consist of an internal
message id with a trailing "-H" or "-D" added. */

#define SPOOL_NAME_LENGTH (MESSAGE_ID_LENGTH+2)

/* The maximum number of message ids to store in a waiting database
record. */

#define WAIT_NAME_MAX 50

/* Fixed option values for all PCRE functions */

#define PCRE_COPT 0   /* compile */
#define PCRE_EOPT 0   /* exec */

/* Macros for trivial functions */

#define mac_ismsgid(s) \
  (pcre_exec(regex_ismsgid,NULL,s,(int)strlen(s),0,PCRE_EOPT,NULL,0) >= 0)

/* Macros for calling the memory allocation routines with
tracing information for debugging. */

#define store_extend(addr,old,new) \
  store_extend_3(addr, old, new, __FILE__, __LINE__)

#define store_free(addr)    store_free_3(addr, __FILE__, __LINE__)
#define store_get(size)     store_get_3(size, __FILE__, __LINE__)
#define store_malloc(size)  store_malloc_3(size, __FILE__, __LINE__)
#define store_mark(flag)    store_mark_3(flag, __FILE__, __LINE__)
#define store_release(addr) store_release_3(addr, __FILE__, __LINE__)
#define store_reset(addr)   store_reset_3(addr, __FILE__, __LINE__)

/* Options for dns_next_rr */

enum { RESET_NEXT, RESET_ANSWERS, RESET_ADDITIONAL };

/* Argument values for the time-of-day function */

enum { tod_log, tod_full, tod_bsdin, tod_mbx };

/* For identifying which kind of driver deferred */

enum {
  DTYPE_NONE,
  DTYPE_DIRECTOR,
  DTYPE_ROUTER,
  DTYPE_TRANSPORT
};

/* Error numbers for generating error messages */

enum {
  ERRMESS_BADARGADDRESS,    /* Bad address via argument list */
  ERRMESS_BADADDRESS,       /* Bad address read via -t */
  ERRMESS_NOADDRESS,        /* Message has no addresses */
  ERRMESS_IGADDRESS,        /* All -t addresses ignored */
  ERRMESS_BADNOADDRESS,     /* Bad address via -t, leaving none */
  ERRMESS_IOERR,            /* I/O error while reading a message */
  ERRMESS_VLONGHEADER,      /* Excessively long message header */
  ERRMESS_TOOBIG,           /* Message too big */
  ERRMESS_TOOMANYRECIP      /* Too many recipients */
};

/* Error handling styles - set by option, and apply only when receiving
a local message. */

enum {
  ERRORS_SENDER,            /* Return to sender (default) */
  ERRORS_STDERR             /* Write on stderr */
};

/* Returns from the directing, routing, transport and authentication functions
(not all apply to all of them). Some other functions also use these convenient
values, and some additional values are used only by non-driver functions. */

enum {
  OK,                       /* Success */
  DEFER,                    /* Defer delivery */
  DECLINE,                  /* Declined to handle the address, pass to next
                               driver unless no_more is set */
  PASS,                     /* Pass to next driver, even if no_more is set */
  FORCEFAIL,                /* Failed, and don't ever pass to next driver */
  FAIL_CONDITION,           /* Failed a condition test in a director */
  ISLOCAL,                  /* Remote address turned out to be local */
  FAIL,                     /* Transport failed */
  ERROR,                    /* Soft failed with internal or config error */
  PANIC,                    /* Hard failed with internal error */
  BAD64,                    /* Bad base64 data (auth) */
  UNEXPECTED,               /* Unexpected initial auth data */
  FAIL_SEND                 /* send() failed in authenticator */
};

/* Returns from DNS lookup functions. */

enum { DNS_SUCCEED, DNS_NOMATCH, DNS_AGAIN, DNS_FAIL };

/* Ending states when reading a message. The order is important. The test
for having to swallow the rest of an SMTP message is whether the value is
>= END_NOTENDED. */

#define END_NOTSTARTED 0    /* Message not started */
#define END_DOT        1    /* Message ended with '.' */
#define END_EOF        2    /* Message ended with EOF (error for SMTP) */
#define END_NOTENDED   3    /* Message reading not yet ended */
#define END_SIZE       4    /* Reading ended because message too big */
#define END_WERROR     5    /* Write error while reading the message */

/* Private error numbers for delivery failures, set negative so as not
to conflict with system errno values. */

#define ERRNO_UNKNOWNERROR    (-1)
#define ERRNO_USERSLASH       (-2)
#define ERRNO_EXISTRACE       (-3)
#define ERRNO_NOTREGULAR      (-4)
#define ERRNO_NOTDIRECTORY    (-5)
#define ERRNO_BADUGID         (-6)
#define ERRNO_BADMODE         (-7)
#define ERRNO_INODECHANGED    (-8)
#define ERRNO_LOCKFAILED      (-9)
#define ERRNO_BADADDRESS2    (-10)
#define ERRNO_BADFORWARD     (-11)
#define ERRNO_FORBIDPIPE     (-12)
#define ERRNO_FORBIDFILE     (-13)
#define ERRNO_FORBIDREPLY    (-14)
#define ERRNO_MISSINGPIPE    (-15)
#define ERRNO_MISSINGFILE    (-16)
#define ERRNO_MISSINGREPLY   (-17)
#define ERRNO_BADALIAS       (-18)
#define ERRNO_SMTPCLOSED     (-19)
#define ERRNO_SMTPFORMAT     (-20)
#define ERRNO_SPOOLFORMAT    (-21)
#define ERRNO_NOTABSOLUTE    (-22)
#define ERRNO_EXIMQUOTA      (-23)   /* Exim-imposed quota */
#define ERRNO_HELD           (-24)
#define ERRNO_FILTER_FAIL    (-25)   /* Delivery filter process failure */
#define ERRNO_CHHEADER_FAIL  (-26)   /* Delivery add/remove header failure */
#define ERRNO_WRITEINCOMPLETE (-27)  /* Delivery write incomplete error */
#define ERRNO_EXPANDFAIL     (-28)   /* Some expansion failed */
#define ERRNO_GIDFAIL        (-29)   /* Failed to get gid */
#define ERRNO_UIDFAIL        (-30)   /* Failed to get uid */
#define ERRNO_BADTRANSPORT   (-31)   /* Unset or non-existent transport */
#define ERRNO_MBXLENGTH      (-32)   /* MBX length mismatch */
#define ERRNO_UNKNOWNHOST    (-33)   /* Lookup failed in smtp transport */
#define ERRNO_FORMATUNKNOWN  (-34)   /* Can't match format in appendfile */
#define ERRNO_BADCREATE      (-35)   /* Creation outside home in appendfile */
#define ERRNO_LISTDEFER      (-36)   /* Can't check a list; lookup defer */
#define ERRNO_DNSDEFER       (-37)   /* DNS lookup defer */
#define ERRNO_TLSFAILURE     (-38)   /* Failed to start TLS session */
#define ERRNO_TLSREQUIRED    (-39)   /* Mandatory TLS session not started */

/* These must be last, so all retry deferments can easily be identified */

#define ERRNO_RETRY_BASE     (-41)   /* Base to test against */
#define ERRNO_DRETRY         (-41)   /* Not time for directing */
#define ERRNO_RRETRY         (-42)   /* Not time for routing */
#define ERRNO_LRETRY         (-43)   /* Not time for local delivery */
#define ERRNO_HRETRY         (-44)   /* Not time for any remote host */
#define ERRNO_LOCAL_ONLY     (-45)   /* Local-only delivery */

/* Special actions to take after failure or deferment. */

enum {
  SPECIAL_NONE,             /* No special action */
  SPECIAL_FREEZE,           /* Freeze message */
  SPECIAL_FAIL,             /* Fail the delivery */
  SPECIAL_WARN              /* Send a warning message */
};


/* Values for identifying particular headers; printing characters are
used so they can easily be seen in the spool file. */

#define htype_other    ' '   /* Unspecified header */
#define htype_from     'F'
#define htype_to       'T'
#define htype_cc       'C'
#define htype_bcc      'B'
#define htype_id       'I'   /* for message-id */
#define htype_replyto  'R'
#define htype_received 'P'   /* P for Postmark */
#define htype_sender   'S'
#define htype_old      '*'   /* Replaced header */

/* Offsets into the tables of names for headers (normal & resent) */

enum { hn_bcc, hn_cc, hn_date, hn_from, hn_msgid, hn_sender, hn_to,
  hn_replyto, hn_subject };

/* Types of item in options lists. These are the bottom bits of the "type"
field, which is an int. The opt_void value is used for entries in tables that
point to special types of value that are accessed only indirectly (e.g. the
rewrite data that is built out of a string option.) */

enum { opt_void, opt_stringptr, opt_transportptr, opt_rewrite, opt_int,
  opt_octint, opt_mkint, opt_fixed, opt_time, opt_timelist, opt_bool,
  opt_bool_verify, opt_bool_set, opt_uid, opt_gid, opt_uidlist, opt_gidlist,
  opt_expand_uid, opt_expand_gid, opt_searchtype, opt_local_batch };

/* There's a high-ish bit which is used to flag duplicate options, kept
for compatibility, which shouldn't be output. Also used for hidden options
that are automatically maintained from others. Another high bit is used to
flag driver options that although private (so as to be settable only on some
drivers), are stored in the instance block so as to be accessible from outside.
A third high bit is set when an option is read, so as to be able to give an
error if any option is set twice. Finally, there's a bit which is set when an
option is set with the "hide" prefix, to prevent -bP from showing it to
non-admin callers. */

#define opt_hidden  0x100
#define opt_public  0x200
#define opt_set     0x400
#define opt_secure  0x800
#define opt_mask    0x0ff

/* Verify types when directing and routing */

enum { v_none, v_sender, v_recipient, v_expn };

/* Option flags for verify_address() */

#define vopt_is_recipient   0x01
#define vopt_local          0x02
#define vopt_expn           0x04

/* Options for lookup functions */

#define lookup_querystyle      1    /* query-style lookup */
#define lookup_absfile         2    /* requires absolute file name */

/* Status values for host_item blocks. Require hstatus_unusable and
hstatus_unusable_expired to be last. */

enum { hstatus_unknown, hstatus_usable, hstatus_unusable,
       hstatus_unusable_expired };

/* Reasons why a host is unusable (for clearer log messages) */

enum { hwhy_unknown, hwhy_retry, hwhy_failed, hwhy_deferred, hwhy_ignored };

/* Domain lookup types for routers */

enum { lk_pass, lk_byname, lk_bydns, lk_bydns_a, lk_bydns_mx };

/* Values for the self_code fields */

enum { self_freeze, self_defer, self_send, self_reroute, self_local,
  self_pass, self_forcefail };

/* Flags for rewrite rules */

#define rewrite_sender       0x0001
#define rewrite_from         0x0002
#define rewrite_to           0x0004
#define rewrite_cc           0x0008
#define rewrite_bcc          0x0010
#define rewrite_replyto      0x0020
#define rewrite_all_headers  0x003F  /* all header flags */

#define rewrite_envfrom      0x0040
#define rewrite_envto        0x0080
#define rewrite_all_envelope 0x00C0  /* all envelope flags */

#define rewrite_all      (rewrite_all_headers | rewrite_all_envelope)

#define rewrite_smtp         0x0100  /* rewrite at SMTP time */
#define rewrite_qualify      0x0200  /* qualify if necessary */
#define rewrite_repeat       0x0400  /* repeat rewrite rule */
#define rewrite_X            0x0800  /* extra for relay checking */

#define rewrite_whole        0x1000  /* option bit for headers */
#define rewrite_quit         0x2000  /* "no more" option */

/* Special return values from parse_extract_addresses */

#define EXTRACTED_INCLUDEFAIL  (+1)    /* :include: failed */
#define EXTRACTED_OK            0
#define EXTRACTED_DEFER        (-1)    /* :defer: */
#define EXTRACTED_UNKNOWN      (-2)    /* :unknown: */
#define EXTRACTED_FAIL         (-3)    /* :fail: */
#define EXTRACTED_ERROR        (-4)    /* some problem */

/* Flags for log_write() */

#define LOG_MAIN        1    /* Write to the main log */
#define LOG_PANIC       2    /* Write to the panic log */
#define LOG_PANIC_DIE   6    /* Write to the panic log and then die */
#define LOG_PROCESS     8    /* Write to the process log */
#define LOG_REJECT     16    /* Write to the reject log, with headers */
#define LOG_SENDER     32    /* Add raw sender to the message */
#define LOG_RECIPIENTS 64    /* Add raw recipients to the message */
#define LOG_CONFIG    128    /* Add "Exim configuration error:\n" */
#define LOG_CONFIG2   256    /* Add "Exim configuration error for" */

/* Returns from host_find_by{name,dns}() */

enum {
  HOST_FIND_FAILED,     /* failed to find the host */
  HOST_FIND_AGAIN,      /* could not resolve at this time */
  HOST_FOUND,           /* found host */
  HOST_FOUND_LOCAL,     /* found, but MX points to local host */
  HOST_IGNORED          /* found but ignored - used internally only */
};

/* Actions applied to specific messages. */

enum { MSG_DELIVER, MSG_FREEZE, MSG_REMOVE, MSG_THAW, MSG_ADD_RECIPIENT,
       MSG_MARK_ALL_DELIVERED, MSG_MARK_DELIVERED, MSG_EDIT_SENDER,
       MSG_EDIT_BODY, MSG_SHOW_BODY, MSG_SHOW_HEADER, MSG_SHOW_LOG };

/* Options for local batched SMTP deliveries. Must be in the same order
as for non-SMTP local batching below. */

enum {
  local_smtp_off,       /* not doing batched SMTP */
  local_smtp_one,       /* each address separate */
  local_smtp_domain,    /* batch identical domains */
  local_smtp_all        /* batch all addresses */
};

/* Options for non-SMTP local batched deliveries - must be in the same
order as for SMTP local batching above. */

enum {
  local_batch_off,      /* not doing batched delivery */
  local_batch_one,      /* each address separate */
  local_batch_domain,   /* batch identical domains */
  local_batch_all       /* batch all addresses */
};

/* Returns from the spool_read_header() function */

enum {
  spool_read_OK,        /* success */
  spool_read_notopen,   /* open failed */
  spool_read_enverror,  /* error in the envelope */
  spool_read_hdrerror   /* error in the headers */
};

/* Options for transport_write_message */

#define topt_add_return_path    0x001
#define topt_add_delivery_date  0x002
#define topt_add_envelope_to    0x004
#define topt_use_crlf           0x008  /* Terminate lines with CRLF */
#define topt_end_dot            0x010  /* Send terminting dot line */
#define topt_no_headers         0x020  /* Omit headers */
#define topt_no_body            0x040  /* Omit body */
#define topt_escape_headers     0x080  /* Apply escape check to headers */

/* Flags for recipient_block */

#define rf_onetime              0x01  /* A one-time alias */
#define rf_notify_never         0x02  /* NOTIFY= settings */
#define rf_notify_success       0x04
#define rf_notify_failure       0x08
#define rf_notify_delay         0x10

#define rf_dsnflags  (rf_notify_never | rf_notify_success | \
                      rf_notify_failure | rf_notify_delay)

/* DSN RET types */

#define dsn_ret_full            1
#define dsn_ret_hdrs            2

/* Situations for spool_write_header() */

enum { SW_RECEIVING, SW_DELIVERING, SW_MODIFYING };

/* End of macros.h */
