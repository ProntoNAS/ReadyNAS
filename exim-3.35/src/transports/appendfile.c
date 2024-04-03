/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "appendfile.h"



/* Encodings for mailbox formats, and their names. MBX format is actually
supported only if SUPPORT_MBX is set. */

enum { mbf_unix, mbf_mbx, mbf_smail, mbf_maildir, mbf_mailstore };

static char *mailbox_formats[] = {
  "unix", "mbx", "smail", "maildir", "mailstore" };



/* Options specific to the appendfile transport. They must be in alphabetic
order (note that "_" comes before the lower case letters). Some of them are
stored in the publicly visible instance block - these are flagged with the
opt_public flag. */

optionlist appendfile_transport_options[] = {
  { "*expand_group",     opt_stringptr | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, expand_gid)) },
  { "*expand_user",      opt_stringptr | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, expand_uid)) },
  { "*set_from_hack",    opt_bool | opt_hidden,
      (void *)(offsetof(appendfile_transport_options_block, set_from_hack)) },
  { "*set_group",         opt_bool | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, gid_set)) },
  { "*set_use_fcntl_lock",opt_bool | opt_hidden,
      (void *)(offsetof(appendfile_transport_options_block, set_use_fcntl)) },
  { "*set_use_lockfile", opt_bool | opt_hidden,
      (void *)(offsetof(appendfile_transport_options_block, set_use_lockfile)) },
#ifdef SUPPORT_MBX
  { "*set_use_mbx_lock", opt_bool | opt_hidden,
      (void *)(offsetof(appendfile_transport_options_block, set_use_mbx_lock)) },
#endif
  { "*set_user",         opt_bool | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, uid_set)) },
  { "allow_fifo",        opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, allow_fifo)) },
  { "allow_symlink",     opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, allow_symlink)) },
  { "batch",             opt_local_batch | opt_public,
      (void *)(offsetof(transport_instance, local_batch)) },
  { "batch_max",         opt_int | opt_public,
      (void *)(offsetof(transport_instance, batch_max)) },
  { "bsmtp",             opt_local_batch | opt_public,
      (void *)(offsetof(transport_instance, local_smtp)) },
  { "bsmtp_helo",        opt_bool | opt_public,
      (void *)(offsetof(transport_instance, bsmtp_helo)) },
  { "check_group",       opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, check_group)) },
  { "check_owner",       opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, check_owner)) },
  { "check_string",      opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, check_string)) },
  { "create_directory",  opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, create_directory)) },
  { "create_file",       opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, create_file_string)) },
  { "current_directory", opt_stringptr | opt_public,
      (void *)(offsetof(transport_instance, current_dir)) },
  { "directory",         opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, dirname)) },
  { "directory_mode",    opt_octint,
      (void *)(offsetof(appendfile_transport_options_block, dirmode)) },
  { "escape_string",     opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, escape_string)) },
  { "file",              opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, filename)) },
  { "file_format",       opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, file_format)) },
  { "file_must_exist",   opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, file_must_exist)) },
  { "from_hack",         opt_bool_set | opt_hidden,
      (void *)(offsetof(appendfile_transport_options_block, from_hack)) },
  { "group",             opt_expand_gid | opt_public,
      (void *)(offsetof(transport_instance, gid)) },
  { "lock_fcntl_timeout", opt_time,
      (void *)(offsetof(appendfile_transport_options_block, lock_fcntl_timeout)) },
  { "lock_interval",     opt_time,
      (void *)(offsetof(appendfile_transport_options_block, lock_interval)) },
  { "lock_retries",      opt_int,
      (void *)(offsetof(appendfile_transport_options_block, lock_retries)) },
  { "lockfile_mode",     opt_octint,
      (void *)(offsetof(appendfile_transport_options_block, lockfile_mode)) },
  { "lockfile_timeout",  opt_time,
      (void *)(offsetof(appendfile_transport_options_block, lockfile_timeout)) },
#ifdef SUPPORT_MAILDIR
  { "maildir_format",    opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, maildir_format )) } ,
  { "maildir_retries",   opt_int,
      (void *)(offsetof(appendfile_transport_options_block, maildir_retries)) },
  { "maildir_tag",       opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, maildir_tag)) },
#endif
#ifdef SUPPORT_MAILSTORE
  { "mailstore_format",  opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, mailstore_format )) },
  { "mailstore_prefix",  opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, mailstore_prefix )) },
  { "mailstore_suffix",  opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, mailstore_suffix )) },
#endif
#ifdef SUPPORT_MBX
  { "mbx_format",        opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, mbx_format )) } ,
#endif
  { "mode",              opt_octint,
      (void *)(offsetof(appendfile_transport_options_block, mode)) },
  { "mode_fail_narrower",opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, mode_fail_narrower)) },
  { "notify_comsat",     opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, notify_comsat)) },
  { "prefix",            opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, prefix)) },
  { "quota",             opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, quota)) },
  { "quota_filecount",   opt_int,
      (void *)(offsetof(appendfile_transport_options_block, quota_filecount)) },
  { "quota_is_inclusive", opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, quota_is_inclusive)) },
  {"quota_size_regex",   opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, quota_size_regex)) },
  { "quota_warn_message", opt_stringptr | opt_public,
      (void *)(offsetof(transport_instance, warn_message)) },
  { "quota_warn_threshold", opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, quota_warn_threshold)) },
  { "require_lockfile",  opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, require_lockfile)) },
  { "retry_use_local_part", opt_bool | opt_public,
      (void *)offsetof(transport_instance, retry_use_local_part) },
  { "suffix",            opt_stringptr,
      (void *)(offsetof(appendfile_transport_options_block, suffix)) },
  { "use_crlf",          opt_bool,
      (void *)(offsetof(appendfile_transport_options_block, use_crlf)) },
  { "use_fcntl_lock",    opt_bool_set,
      (void *)(offsetof(appendfile_transport_options_block, use_fcntl)) },
  { "use_lockfile",      opt_bool_set,
      (void *)(offsetof(appendfile_transport_options_block, use_lockfile)) },
#ifdef SUPPORT_MBX
  { "use_mbx_lock",      opt_bool_set,
      (void *)(offsetof(appendfile_transport_options_block, use_mbx_lock)) },
#endif
  { "user",              opt_expand_uid | opt_public,
      (void *)(offsetof(transport_instance, uid)) },
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int appendfile_transport_options_count =
  sizeof(appendfile_transport_options)/sizeof(optionlist);

/* Default private options block for the appendfile transport. */

appendfile_transport_options_block appendfile_transport_option_defaults = {
  NULL,           /* file name */
  NULL,           /* dir name */
  "From ${if def:return_path{$return_path}{MAILER-DAEMON}} ${tod_bsdinbox}\n",
                  /* prefix */
  "\n",           /* suffix */
  "anywhere",     /* create_file_string (string value for create_file) */
  NULL,           /* quota */
  NULL,           /* quota_size_regex */
  NULL,           /* quota_warn_threshold */
  NULL,           /* maildir_tag */
  NULL,           /* mailstore_prefix */
  NULL,           /* mailstore_suffix */
  "From ",        /* check_string */
  ">From ",       /* escape_string */
  NULL,           /* file_format */
  0,              /* quota_value */
  0,              /* quota_warn_threshold_value */
  0,              /* quota_filecount */
  0600,           /* mode */
  0700,           /* dirmode */
  0600,           /* lockfile_mode */
  30*60,          /* lockfile_timeout */
  0,              /* lock_fcntl_timeout */
  10,             /* lock_retries */
   3,             /* lock_interval */
  10,             /* maildir_retries */
  create_anywhere,/* create_file */
  0,              /* options */
  FALSE,          /* allow_fifo */
  FALSE,          /* allow_symlink */
  FALSE,          /* check_group */
  TRUE,           /* check_owner */
  TRUE,           /* create_directory */
  FALSE,          /* notify_comsat */
  TRUE,           /* require_lockfile */
  TRUE,           /* use_lockfile */
  FALSE,          /* set_use_lockfile */
  TRUE,           /* use_fcntl */
  FALSE,          /* set_use_fcntl */
  FALSE,          /* use_mbx_lock */
  FALSE,          /* set_use_mbx_lock */
  FALSE,          /* use_crlf */
  TRUE,           /* from_hack */
  FALSE,          /* set_from_hack */
  FALSE,          /* file_must_exist */
  TRUE,           /* mode_fail_narrower */
  FALSE,          /* maildir_format */
  FALSE,          /* mailstore_format */
  FALSE,          /* mbx_format */
  FALSE,          /* quota_warn_threshold_is_percent */
  TRUE            /* quota_is_inclusive */
};



/*************************************************
*              Setup entry point                 *
*************************************************/

/* Called for each delivery in the privileged state, just before the uid/gid
are changed and the main entry point is called. We use this function to
expand any quota settings, so that it can access files that may not be readable
by the user.

Arguments:
  tblock     points to the transport instance
  addrlist   addresses about to be delivered
  errmsg     where to put an error message

Returns:     OK, FAIL, or DEFER
*/

static int
appendfile_transport_setup(transport_instance *tblock, address_item *addrlist,
  char **errmsg)
{
appendfile_transport_options_block *ob =
  (appendfile_transport_options_block *)(tblock->options_block);
char *q = ob->quota;
int *v = &(ob->quota_value);
int i;

addrlist = addrlist;    /* Keep picky compilers happy */

/* Loop for quota and quota_warn_threshold */

for (i = 0; i < 2; i++)
  {
  if (q == NULL) *v = 0; else
    {
    double d;
    char *rest;
    char *s = expand_string(q);

    if (s == NULL)
      {
      *errmsg = string_sprintf("Expansion of \"%s\" (quota setting "
        "for %s transport) failed: %s", q, tblock->name,
        expand_string_message);
      return search_find_defer? DEFER : FAIL;
      }

    d = strtod(s, &rest);

    /* Handle following characters K, M, %, the latter being permitted
    for quota_warn_threshold only. */

    if (tolower(*rest) == 'k') { d *= 1024.0; rest++; }
    else if (tolower(*rest) == 'm') { d *= 1024.0*1024.0; rest++; }
    else if (*rest == '%' && i == 1)
      {
      if (ob->quota_value <= 0)
        {
        /* We used to object to a percentage setting with no quota set,
        but there was a request just to ignore, so that is what is now done. */

        /****
        *errmsg = string_sprintf("quota_warn_threshold cannot be specified as "
          "a percentage (%s) unless quota is also set for %s transport",
            s, tblock->name);
        return FAIL;
        ****/

        d = 0;
        }
      if ((int)d < 0 || (int)d > 100)
        {
        *errmsg = string_sprintf("Invalid quota_warn_threshold percentage (%d)"
          " for %s transport", (int)d, tblock->name);
        return FAIL;
        }
      ob->quota_warn_threshold_is_percent = TRUE;
      rest++;
      }

    while (isspace((uschar)*rest)) rest++;

    if (*rest != 0)
      {
      *errmsg = string_sprintf("Malformed quota setting \"%s\" for "
        "%s transport", s, tblock->name);
      return FAIL;
      }

    *v = (int)d;
    }

  q = ob->quota_warn_threshold;
  v = &(ob->quota_warn_threshold_value);
  }

return OK;
}



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
appendfile_transport_init(transport_instance *tblock)
{
appendfile_transport_options_block *ob =
  (appendfile_transport_options_block *)(tblock->options_block);

/* Set up the setup entry point, to be called in the privileged state */

tblock->setup = appendfile_transport_setup;

/* Retry_use_local_part defaults TRUE if unset */

if (tblock->retry_use_local_part == 2) tblock->retry_use_local_part = TRUE;

/* Lock_retries must be greater than zero */

if (ob->lock_retries == 0) ob->lock_retries = 1;

/* Only one of a file name or directory name must be given. */

if (ob->filename != NULL && ob->dirname != NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
  "only one of \"file\" or \"directory\" can be specified", tblock->name);

/* If a file name was specified, it must be an absolute path. Can check here
only if there are no expansions. Quota_filecount must not be given. */

if (ob->filename != NULL)
  {
  if (ob->filename[0] != '/' && ob->filename[0] != '$')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
    "the file option must specify an absolute path", tblock->name);
  if (ob->quota_filecount > 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
    "quota_filecount must not be set without \"directory\"", tblock->name);
  }

/* The default locking depends on whether MBX is set or not. Change the
built-in default if none of the lock options has been explicitly set. At least
one form of locking is required in all cases, but mbx locking and fcntl locking
are mutually exclusive. */

#ifdef SUPPORT_MBX

if (ob->mbx_format)
  {
  if(!ob->set_use_lockfile && !ob->set_use_fcntl && !ob->set_use_mbx_lock)
    {
    ob->use_lockfile = ob->use_fcntl = FALSE;
    ob->use_mbx_lock = TRUE;
    }
  else if (ob->use_mbx_lock)
    {
    if (!ob->set_use_lockfile) ob->use_lockfile = FALSE;
    if (!ob->set_use_fcntl) ob->use_fcntl = FALSE;
    }
  }

if (ob->use_fcntl && ob->use_mbx_lock)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
    "use_fcntl_lock and use_mbx_lock are mutually exclusive", tblock->name);

#endif

if (!ob->use_fcntl && (!ob->use_lockfile || !ob->require_lockfile) &&
    !ob->use_mbx_lock)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
    "no locking configured", tblock->name);

/* If a directory name was specified, it must be an absolute path. Can check
here only if there are no expansions. Only one of maildir or mailstore may be
specified, and if quota_filecount is given, so must quota be. */

if (ob->dirname != NULL)
  {
  if (ob->dirname[0] != '/' && ob->dirname[0] != '$')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
      "the directory option must specify an absolute path", tblock->name);
  if (ob->maildir_format && ob->mailstore_format)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
      "only one of maildir and mailstore may be specified", tblock->name);
  if (ob->quota_filecount > 0 && ob->quota == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s transport:\n  "
      "quota must be set if quota_filecount is set", tblock->name);
  }

/* If a fixed uid field is set, then a gid field must also be set. */

if (tblock->uid_set && !tblock->gid_set && tblock->expand_gid == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "user set without group for the %s transport", tblock->name);

/* If "create_file" is set, check that a valid option is given, and set the
integer variable. */

if (ob->create_file_string != NULL)
  {
  int value = 0;
  if (strcmp(ob->create_file_string, "anywhere") == 0) value = create_anywhere;
  else if (strcmp(ob->create_file_string, "belowhome") == 0) value =
    create_belowhome;
  else if (strcmp(ob->create_file_string, "inhome") == 0)
    value = create_inhome;
  else
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "invalid value given for \"file_create\" for the %s transport: %s",
        tblock->name, ob->create_file_string);
  ob->create_file = value;
  }

/* If quota_warn_threshold is set, set up default for warn_message. It may
not be used if the actual threshold for a given delivery ends up as zero,
of if it's given as a percentage and there's no quota setting. */

if (ob->quota_warn_threshold != NULL)
  {
  if (tblock->warn_message == NULL) tblock->warn_message =
    "To: $local_part@$domain\n"
    "Subject: Your mailbox\n\n"
    "This message is automatically created by mail delivery software (Exim).\n\n"
    "The size of your mailbox has exceeded a warning threshold that is\n"
    "set by the system administrator.\n";
  }

/* If batch SMTP is set, ensure the generic local batch option matches. They
take the same set of values. Also force the check and escape strings, and
arrange that headers are also escaped. */

if (tblock->local_smtp != local_smtp_off)
  {
  tblock->local_batch = tblock->local_smtp;
  ob->check_string = ".";
  ob->escape_string = "..";
  ob->options |= topt_escape_headers;
  }

/* Otherwise, if the obsolete option from_hack has been explicitly unset,
translate its setting into the newer check_string and escape_string options. */

else if (ob->set_from_hack && !ob->from_hack)
  {
  ob->check_string = NULL;
  ob->escape_string = NULL;
  }

/* Set up the bitwise options for transport_write_message from the various
driver options. Only one of body_only and headers_only can be set. */

ob->options |=
  (tblock->body_only? topt_no_headers : 0) |
  (tblock->headers_only? topt_no_body : 0) |
  (tblock->return_path_add? topt_add_return_path : 0) |
  (tblock->delivery_date_add? topt_add_delivery_date : 0) |
  (tblock->envelope_to_add? topt_add_envelope_to : 0) |
  ((ob->use_crlf || ob->mbx_format)? topt_use_crlf : 0);
}



/*************************************************
*                  Notify comsat                 *
*************************************************/

/* The comsat daemon is the thing that provides asynchronous notification of
the arrival of local messages, if requested by the user by "biff y". It is a
BSD thing that uses a TCP/IP protocol for communication. A message consisting
of the text "user@offset" must be sent, where offset is the place in the
mailbox where new mail starts. There is no scope for telling it which file to
look at, which makes it a less than useful if mail is being delivered into a
non-standard place such as the user's home directory. In fact, it doesn't seem
to pay much attention to the offset.

Arguments:
  user       user name
  offset     offset in mailbox

Returns:     nothing
*/

static void
notify_comsat(char *user, int offset)
{
struct servent *sp;
host_item host;
host_item *h;
char buffer[256];

DEBUG(2) debug_printf("notify_comsat called\n");

sprintf(buffer, "%.200s@%d\n", user, offset);

if ((sp = getservbyname("biff", "udp")) == NULL)
  {
  DEBUG(2) debug_printf("biff/udp is an unknown service");
  return;
  }

host.name = "localhost";
host.next = NULL;


/* This code is all set up to look up "localhost" and use all its addresses
until one succeeds. However, it appears that at least on some systems, comsat
doesn't listen on the ::1 address. So for the moment, just force the address to
be 127.0.0.1. At some future stage, when IPv6 really is superseding IPv4, this
can be changed. */

/******
if (host_find_byname(&host, NULL, NULL, FALSE) == HOST_FIND_FAILED)
  {
  DEBUG(2) debug_printf("\"localhost\" unknown\n");
  return;
  }
******/

host.address = "127.0.0.1";


for (h = &host; h != NULL; h = h->next)
  {
  struct sockaddr *s_ptr;
  struct sockaddr_in s_in4;
  int host_af = (strchr(h->address, ':') != NULL)? AF_INET6 : AF_INET;
  int sock, s_len, rc;

  #if HAVE_IPV6
  struct sockaddr_in6 s_in6;

  /* For an IPv6 address, use an IPv6 sockaddr structure. */

  if (host_af == AF_INET6)
    {
    s_ptr = (struct sockaddr *)&s_in6;
    s_len = sizeof(s_in6);
    }
  else
  #endif

  /* For an IPv4 address, use an IPv4 sockaddr structure,
  even on an IPv6 system. */

    {
    s_ptr = (struct sockaddr *)&s_in4;
    s_len = sizeof(s_in4);
    }

  DEBUG(2) debug_printf("calling comsat on %s\n", h->address);

  sock = socket(host_af, SOCK_DGRAM, 0);
  if (sock < 0)
    {
    DEBUG(2) debug_printf("failed to create comsat socket: %s\n",
      strerror(errno));
    continue;
    }

  #if HAVE_IPV6
  if (host_af == AF_INET6)
    {
    memset(&s_in6, 0, sizeof(s_in6));
    s_in6.sin6_family = AF_INET6;
    s_in6.sin6_port = sp->s_port;
    if (inet_pton(host_af, h->address, &s_in6.sin6_addr) != 1)
      log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", h->address);
    }
  else
  #endif

  /* Set up a remote IPv4 address */

    {
    memset(&s_in4, 0, sizeof(s_in4));
    s_in4.sin_family = AF_INET;
    s_in4.sin_port = sp->s_port;
    s_in4.sin_addr.s_addr = (S_ADDR_TYPE)inet_addr(h->address);
    }

  rc = sendto(sock, buffer, (int)strlen(buffer) + 1, 0, s_ptr, s_len);
  close(sock);

  if (rc >= 0) break;
  DEBUG(2) debug_printf("send to comsat failed for %s: %s\n", strerror(errno),
    h->address);
  }
}



/*************************************************
*     Check the format of a file                 *
*************************************************/

/* This function is called when file_format is set, to check that an existing
file has the right format. The format string contains text/transport pairs. The
string matching is literal. we just read big_buffer_size bytes, because this is
all about the first few bytes of a file.

Arguments:
  cfd          the open file
  tblock       the transport block
  addr         the address block - for inserting error data

Returns:       pointer to the required transport, or NULL
*/

transport_instance *
check_file_format(int cfd, transport_instance *tblock, address_item *addr)
{
char *format =
  ((appendfile_transport_options_block *)(tblock->options_block))->file_format;
char data[256];
int len = read(cfd, data, sizeof(data));
int sep = 0;
char *s;

DEBUG(9) debug_printf("checking file format\n");

/* An empty file matches the current transport */

if (len == 0) return tblock;

/* Search the formats for a match */

while ((s = string_nextinlist(&format,&sep,big_buffer,big_buffer_size))!= NULL)
  {
  int slen = (int)strlen(s);
  BOOL match = len >= slen && strncmp(data, s, slen) == 0;
  char *tp = string_nextinlist(&format, &sep, big_buffer, big_buffer_size);
  if (match)
    {
    transport_instance *tt;
    for (tt = transports; tt != NULL; tt = tt->next)
      if (strcmp(tp, tt->name) == 0)
        {
        DEBUG(9) debug_printf("file format -> %s transport\n", tt->name);
        return tt;
        }
    addr->basic_errno = ERRNO_BADTRANSPORT;
    addr->message = string_sprintf("%s transport (for %.*s format) not found",
      tp, slen, data);
    return NULL;
    }
  }

/* Failed to find a match */

addr->basic_errno = ERRNO_FORMATUNKNOWN;
addr->message = "mailbox file format unrecognized";
return NULL;
}




/*************************************************
*       Check directory's files for quota        *
*************************************************/

/* This function is called if quota is set for one of the delivery modes that
delivers into a specific directory. It scans the directory and stats all the
files in order to get a total size and count. This is an expensive thing to do,
but some people are prepared to bear the cost. Alternatively, if size_regex is
set, it is used as a regex to try to extract the size from the file name, a
strategy that some people use on maildir files on systems where the users have
no shell access.

Note: Any problems can be written to debugging output, but cannot be written to
the log, because we are running as an unprivileged user here.

Arguments:
  dirname       the name of the directory
  countptr      where to add the file count (because this function recurses)
  regex         a compiled regex to get the size from a name

Returns:        the sum of the sizes of the stattable files
                zero if the directory cannot be opened
*/

static int
check_dir_size(char *dirname, int *countptr, pcre *regex)
{
DIR *dir;
int sum = 0;
int count = *countptr;
struct dirent *ent;
struct stat statbuf;

dir = opendir(dirname);
if (dir == NULL) return 0;

while ((ent = readdir(dir)) != NULL)
  {
  char *name = ent->d_name;
  char buffer[1024];

  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

  count++;

  if (!string_format(buffer, sizeof(buffer), "%s/%s", dirname, name))
    {
    DEBUG(9) debug_printf("check_dir_size: name too long: dir=%s name=%s",
      dirname, name);
    continue;
    }

  /* If there's a regex, try to find the size using it */

  if (regex != NULL)
    {
    int ovector[6];
    if (pcre_exec(regex,NULL, name, (int)strlen(name), 0, 0, ovector,6) >= 2)
      {
      int size;
      strncpy(buffer, name + ovector[2], ovector[3] - ovector[2]);
      size = atoi(buffer);
      sum += size;
      DEBUG(9)
        debug_printf("check_dir_size: size from %s is %d\n", name, size);
      continue;
      }
    }

  /* No regex or no match for the regex */

  if (stat(buffer, &statbuf) < 0)
    {
    DEBUG(9) debug_printf("check_dir_size: stat error %d for %s: %s\n",
      errno, buffer, strerror(errno));
    continue;
    }

  if ((statbuf.st_mode & S_IFMT) == S_IFREG)
    sum += statbuf.st_size;
  else if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
    sum += check_dir_size(buffer, &count, regex);
  }

closedir(dir);
DEBUG(9) debug_printf("check_dir_size: dir=%s sum=%d count=%d\n", dirname, sum,
  count);
*countptr = count;
return sum;
}




/*************************************************
*         Apply a lock to a file descriptor      *
*************************************************/

/* This function applies a lock to a file descriptor, using a blocking or
non-blocking lock, depending on the timeout value.

Arguments:
  fd          the file descriptor
  type        F_WRLCK or F_RDLCK
  timeout     non-zero to use blocking locking

Returns:      yield of the fcntl() call, with errno preserved;
              sigalrm_seen set if there has been a timeout
*/

static int
apply_lock(int fd, int type, int timeout)
{
int yield;
struct flock lock_data;
lock_data.l_type = type;
lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;

sigalrm_seen = FALSE;

if (timeout > 0)
  {
  int save_errno;
  os_non_restarting_signal(SIGALRM, sigalrm_handler);
  alarm(timeout);
  yield = fcntl(fd, F_SETLKW, &lock_data);
  save_errno = errno;
  alarm(0);
  signal(SIGALRM, SIG_IGN);
  errno = save_errno;
  }
else yield = fcntl(fd, F_SETLK, &lock_data);

return yield;
}





#ifdef SUPPORT_MBX
/*************************************************
*         Copy message into MBX mailbox          *
*************************************************/

/* This function is called when a message intended for a MBX mailbox has been
written to a temporary file. We can now get the size of the message and then
copy it in MBX format to the mailbox.

Arguments:
  to_fd        fd to write to (the real mailbox)
  from_fd      fd to read from (the temporary file)
  saved_size   current size of mailbox

Returns:       OK if all went well, DEFER otherwise, with errno preserved
*/

/* Values taken from c-client */

#define MBX_HDRSIZE            2048
#define MBX_NUSERFLAGS           30

static int
copy_mbx_message(int to_fd, int from_fd, int saved_size)
{
int used, size;
struct stat statbuf;

/* If the current mailbox size is zero, write a header block */

if (saved_size == 0)
  {
  int i;
  char *s;
  memset (deliver_out_buffer, '\0', MBX_HDRSIZE);
  sprintf(s = deliver_out_buffer, "*mbx*\015\012%08lx00000000\015\012",
    time(NULL));
  for (i = 0; i < MBX_NUSERFLAGS; i++)
    sprintf (s += (int)strlen(s), "\015\012");
  if (!transport_write_block (to_fd, deliver_out_buffer, MBX_HDRSIZE))
    return DEFER;
  }

DEBUG(9) debug_printf("copying MBX message from temporary file\n");

/* Now construct the message's header from the time and the RFC822 file
size, including CRLFs, which is the size of the input (temporary) file. */

if (fstat(from_fd, &statbuf) < 0) return DEFER;
size = statbuf.st_size;

sprintf (deliver_out_buffer, "%s,%lu;%08lx%04x-%08x\015\012",
  tod_stamp(tod_mbx), (long unsigned int)size, 0L, 0, 0);
used = (int)strlen(deliver_out_buffer);

/* Rewind the temporary file, and copy it over in chunks. */

lseek(from_fd, 0 , SEEK_SET);

while (size > 0)
  {
  int len = read(from_fd, deliver_out_buffer + used,
    DELIVER_OUT_BUFFER_SIZE - used);
  if (len <= 0)
    {
    if (len == 0) errno = ERRNO_MBXLENGTH;
    return DEFER;
    }
  if (!transport_write_block(to_fd, deliver_out_buffer, used + len))
    return DEFER;
  size -= len;
  used = 0;
  }

return OK;
}
#endif



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for general interface details. This transport always
returns FALSE, indicating that the status that has been placed in the first
address should be copied to any other addresses in a batch.

Appendfile delivery is tricky and has led to various security problems in other
mailers. The logic used here is therefore laid out in some detail. When this
function is called, we are running in a subprocess which has had its gid and
uid set to the appropriate values. Therefore, we cannot write directly to the
exim logs. Any errors must be handled by setting appropriate return codes.
Note that the default setting for addr->transport_return is DEFER, so it need
not be set unless some other value is required.

The code below calls geteuid() rather than getuid() to get the current uid
because in weird configurations not running setuid root there may be a
difference. In the standard configuration, where setuid() has been used in the
delivery process, there will be no difference between the uid and the euid.

(1)  If the af_pfr flag is set and the local_part field starts with '/' then
     this is a delivery to a file after .forward or alias expansion. Otherwise,
     there must be a configured file name or directory name.

The following items apply in the case when a file name (as opposed to a
directory name) is given:

(2f) If the file name to be expanded contains a reference to $local_part,
     check that the user name doesn't contain a / because that would cause
     a reference to another directory. **** This test is no longer applied. It
     is a pretty weak test, and can cause trouble if $local_part appears in the
     expanded string, but isn't in fact used as part of the file name (e.g. it
     is a lookup key). ****

(3f) Expand the file name.

(4f) If the file name is /dev/null, return success (optimization).

(5f) If the file_format options is set, open the file for reading, and check
     that the bytes at the start of the file match one of the given strings.
     If the check indicates a transport other than the current one should be
     used, pass control to that other transport. Otherwise continue. An empty
     or non-existent file matches the current transport. The file is closed
     after the check.

(6f) If a lock file is required, create it (see extensive separate comments
     below about the algorithm for doing this). It is important to do this
     before opening the mailbox if NFS is in use.

(7f) Stat the file, using lstat() rather than stat(), in order to pick up
     details of any symbolic link.

(8f) If the file already exists:

     Check the owner and group if necessary. Complain and defer if they are
     wrong.

     If it is a symbolic link AND the allow_symlink option is set (NOT the
     default), go back to (5f) but this time use stat() instead of lstat().

     If it's not a regular file (or FIFO when permitted), complain to local
     administrator and defer delivery with a local error code that causes
     subsequent delivery to be prevented until manually enabled.

     Check permissions. If the required permissions are *less* than the
     existing ones, or supplied by the address (often by the user via filter),
     chmod the file. Otherwise, complain and defer.

     Save the inode number.

     Open with O_RDRW + O_APPEND, thus failing if the file has vanished.

     If open fails because the file does not exist, go to (7f); on any other
     failure except EWOULDBLOCK, complain & defer. For EWOULDBLOCK (NFS
     failure), just defer.

     Check the inode number hasn't changed - I realize this isn't perfect (an
     inode can be reused) but it's cheap and will catch some of the races.

     Check it's still a regular file (or FIFO if permitted).

     Check that the owner and permissions haven't changed.

     If file_format is set, check that the file still matches the format for
     the current transport. If not, defer delivery.

(10f)If file does not exist initially:

     Open with O_WRONLY + O_EXCL + O_CREAT with configured mode, unless we know
     this is via a symbolic link (only possible if allow_symlinks is set), in
     which case don't use O_EXCL, as it dosn't work.

     If open fails because the file already exists, go to (7f). To avoid
     looping for ever in a situation where the file is continuously being
     created and deleted, all of this happens inside a loop that operates
     lock_retries times and includes the fcntl locking. If the loop completes
     without the file getting opened, complain and defer with a code that
     freezes delivery attempts.

     If open fails for any other reason, defer for subsequent delivery except
     when this is a file delivery resulting from an alias or forward expansion
     and the error is EPERM or ENOENT or EACCES, in which case FAIL as this is
     most likely a user rather than a configuration error.

(11f)We now have the file checked and open for writing. If so configured, lock
     it using fcntl or MBX locking rules. If this fails, close the file and
     goto (7f), up to lock_retries times, after sleeping for a while. If it
     still fails, give up and defer delivery.

(12f)Save the access time (for subsequent restoration) and the size of the
     file, for comsat and for re-setting if delivery fails in the middle -
     e.g. for quota exceeded.

The following items apply in the case when a directory name is given:

(2d) Create a new file in the directory using a temporary name, by opening for
     writing and with O_CREAT. If maildir format is being used, the file
     is created in a temporary subdirectory with a prescribed name. If
     mailstore format is being used, the envelope file is first created with a
     temporary name, then the data file.

The following items apply in all cases:

(11) We now have the file open for writing, and locked if it was given as a
     file name. Write the message and flush the file, unless there is a setting
     of the local quota option, in which case we can check for its excession
     without doing any writing.

     In the case of MBX format mailboxes, the message is first written to a
     temporary file, in order to get its correct length. This is then copied to
     the real file, preceded by an MBX header.

     If there is a quota error on writing, defer the address. Timeout logic
     will determine for how long retries are attempted. We restore the mailbox
     to its original length. There doesn't seem to be a uniform error code
     for quota excession (it even differs between SunOS4 and some versions of
     SunOS5) so a system-dependent macro called ERRNO_QUOTA is used for it, and
     the value gets put into errno_quota at compile time.

     For any other error (most commonly disc full), do the same.

The following applies after appending to a file:

(12f)Restore the atime; notify_comsat if required; close the file (which
     unlocks it if it was locked). Delete the lock file if it exists.

The following applies after writing a unique file in a directory:

(12d)For non-maildir-format: Generate a unique name for the file from the time
     and the inode number, and rename the file after writing it. For maildir
     format, rename the file into the new directory. For mailstore format,
     rename the envelope file to its correct name.

This transport yields FAIL only when a file name is generated by an alias or
forwarding operation and attempting to open it gives EPERM, ENOENT, or EACCES.
All other failures return DEFER (in addr->transport_return). */


BOOL
appendfile_transport_entry(
  transport_instance *tblock,      /* data for this instantiation */
  address_item *addr)              /* address we are working on */
{
appendfile_transport_options_block *ob =
  (appendfile_transport_options_block *)(tblock->options_block);
struct stat statbuf;
char *path;
char *filename = NULL;     /* Keep picky compiler happy */
char *hitchname = NULL;
char *dataname = NULL;
char *lockname = NULL;
char *newname = NULL;
char *nametag = NULL;
char *cr = "";
struct utimbuf times;
BOOL isdirectory = FALSE;
BOOL isfifo = FALSE;
uid_t uid = geteuid();     /* See note above */
gid_t gid = getegid();
int mbformat;
int mode = (addr->mode > 0)? addr->mode : ob->mode;
int saved_size = 0;
int dircount = 0;
int hd = -1;
int fd = -1;
int yield = FAIL;
int i;

#ifdef SUPPORT_MBX
FILE *temp_file = NULL;
int save_fd = 0;
int mbx_lockfd = -1;
char mbx_lockname[40];
#endif

DEBUG(9) debug_printf("appendfile transport entered\n");

/* See if this is the address_file or address_directory transport, used to
deliver to files specified via .forward or an alias file. If it is, assume
a directory-style delivery if the last character is '/'. (For address_file
this shouldn't ever be the case.) */

if (testflag(addr, af_pfr) && addr->local_part[0] == '/')
  {
  path = addr->local_part;
  if (path[(int)strlen(path)-1] == '/') isdirectory = TRUE;
  }

/* Handle a non-address file delivery. One of the file or directory options is
mandatory; it will have already been checked to be an absolute path. */

else
  {
  char *fdname = ob->filename;
  if (fdname == NULL)
    {
    fdname = ob->dirname;
    isdirectory = TRUE;
    }

  if (fdname == NULL)
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("Mandatory file or directory option "
      "missing from %s transport", tblock->name);
    return FALSE;
    }

  path = expand_string(fdname);

  if (path == NULL)
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("Expansion of \"%s\" (file or directory "
      "name for %s transport) failed: %s", fdname, tblock->name,
      expand_string_message);
    return FALSE;
    }

  /* Check that the expanded name is absolute. This check is needed in case
  it started with an expansion item. If not, the check is done at
  initialization time. */

  if (path[0] != '/')
    {
    addr->message = string_sprintf("appendfile: file or directory name "
      "\"%s\" is not absolute", path);
    addr->basic_errno = ERRNO_NOTABSOLUTE;
    addr->special_action = SPECIAL_FREEZE;
    return FALSE;
    }
  }

/* The available mailbox formats depend on whether it is a directory or a file
delivery. */

if (isdirectory)
  {
  mbformat =
  #ifdef SUPPORT_MAILDIR
    (ob->maildir_format)? mbf_maildir :
  #endif
  #ifdef SUPPORT_MAILSTORE
    (ob->mailstore_format)? mbf_mailstore :
  #endif
    mbf_smail;
  }
else
  {
  mbformat =
  #ifdef SUPPORT_MBX
    (ob->mbx_format)? mbf_mbx :
  #endif
    mbf_unix;
  }

DEBUG(9)
  {
  debug_printf("appendfile: mode=%o notify_comsat=%d quota=%d\n"
    "  %s=%s format=%s\n  prefix=%s\n  suffix=%s\n",
    mode, ob->notify_comsat, ob->quota_value,
    isdirectory? "directory" : "file",
    path, mailbox_formats[mbformat],
    (ob->prefix == NULL)? "null" : string_printing(ob->prefix),
    (ob->suffix == NULL)? "null" : string_printing(ob->suffix));
  if (!isdirectory) debug_printf("  locking by %s%s%s\n",
    ob->use_lockfile? "lockfile " : "",
    ob->use_fcntl? "fcntl " : "",
    ob->use_mbx_lock? "mbx locking" : "");
  }

/* If the -N option is set, can't do any more. */

if (dont_deliver)
  {
  debug_printf("*** delivery by %s transport bypassed by -N option\n",
    tblock->name);
  addr->transport_return = OK;
  return FALSE;
  }

/* Handle the case of a file name. If the file name is /dev/null, we can save
ourselves some effort and just give a success return right away. */

if (!isdirectory)
  {
  BOOL use_lstat = TRUE;
  BOOL file_opened = FALSE;
  BOOL allow_creation_here = TRUE;

  if (strcmp(path, "/dev/null") == 0)
    {
    addr->transport_return = OK;
    return FALSE;
    }

  /* The name of the file to be opened, and the file to which the data
  is written. */

  dataname = filename = path;

  /* If file creation is permitted in certain directories only, check that
  this is actually the case and unset the flag if it is not. The actual
  triggering of the error happens only if we find that the file does not in
  fact exist. Current checks are for in or below the home directory. */

  if (ob->create_file != create_anywhere)
    {
    if (deliver_home != NULL)
      {
      int len = (int)strlen(deliver_home);
      char *file = filename;

      while (file[0] == '/' && file[1] == '/') file++;
      if (strncmp(file, deliver_home, len) != 0 || file[len] != '/' ||
           ( strchr(file+len+2, '/') != NULL &&
             (
             ob->create_file != create_belowhome ||
             strstr(file+len, "/../") != NULL
             )
           )
         ) allow_creation_here = FALSE;

      /* If allow_creation_here is TRUE, the file name starts with the home
      directory, and does not contain any instances of "/../" in the
      "belowhome" case. However, it may still contain symbolic links. We can
      check for this by making use of realpath(), which most Unixes seem to
      have (but make it possible to cut this out). We can't just use realpath()
      on the whole file name, because we know the file itself doesn't exist,
      and intermediate directories may also not exist. What we want to know is
      the real path of the longest existing part of the path. That must match
      the home directory's beginning, whichever is the shorter. */

      #ifndef NO_REALPATH
      if (allow_creation_here && ob->create_file == create_belowhome)
        {
        char *slash, *next;
        char *rp = NULL;
        for (slash = strrchr(file, '/');       /* There is known to be one */
             rp == NULL && slash > file;       /* Stop if reached beginning */
             slash = next)
          {
          *slash = 0;
          rp = realpath(file, big_buffer);
          next = strrchr(file, '/');
          *slash = '/';
          }

        /* If rp == NULL it means that none of the relevant directories exist.
        This is not a problem here - it means that no symbolic links can exist,
        which is all we are worried about. Otherwise, we must compare it
        against the start of the home directory. However, that may itself
        contain symbolic links, so we have to "realpath" it as well, if
        possible. */

        if (rp != NULL)
          {
          char hdbuffer[PATH_MAX+1];
          char *rph = deliver_home;
          int rlen = (int)strlen(big_buffer);

          rp = realpath(deliver_home, hdbuffer);
          if (rp != NULL)
            {
            rph = hdbuffer;
            len = (int)strlen(rph);
            }

          if (rlen > len) rlen = len;
          if (strncmp(rph, big_buffer, rlen) != 0)
            {
            allow_creation_here = FALSE;
            DEBUG(9) debug_printf("Real path \"%s\" does not match \"%s\"\n",
              big_buffer, deliver_home);
            }
          }
        }
      #endif
      }
    }

  /* If ob->create_directory is set, attempt to create the directories in
  which this mailbox lives, but only if we are permitted to create the file
  itself. We know we are dealing with an absolute path, because this has been
  checked above. */

  if (ob->create_directory && allow_creation_here)
    {
    char *p = strrchr(path, '/');
    *p = '\0';
    if (!directory_make("/", path, ob->dirmode, FALSE))
      {
      addr->basic_errno = errno;
      addr->message =
        string_sprintf("failed to create directories for %s: %s", path,
          strerror(errno));
      DEBUG(9) debug_printf("%s transport: %s\n", tblock->name, path);
      return FALSE;
      }
    *p = '/';
    }

  /* If file_format is set we must check that any existing file matches one of
  the configured formats by checking the bytes it starts with. A match then
  indicates a specific transport - if it is not this one, pass control to it.
  Otherwise carry on here. An empty or non-existent file matches the current
  transport. We don't need to distinguish between non-existence and other open
  failures because if an existing file fails to open here, it will also fail
  again later when O_RDWR is used. */

  if (ob->file_format != NULL)
    {
    int cfd = open(path, O_RDONLY);
    if (cfd >= 0)
      {
      transport_instance *tt = check_file_format(cfd, tblock, addr);
      close(cfd);

      /* If another transport is indicated, call it and return; if no transport
      was found, just return - the error data will have been set up.*/

      if (tt != tblock)
        {
        if (tt != NULL)
          {
          set_process_info("delivering %s to %s using %s", message_id,
            addr->local_part, tt->name);
          debug_print_string(tt->debug_string);
          addr->transport = tt;
          (tt->info->code)(tt, addr);
          }
        return FALSE;
        }
      }
    }

  /* The locking of mailbox files is worse than the naming of cats, which is
  known to be "a difficult matter" (T.S. Eliot) and just as cats must have
  three different names, so several different styles of locking are used.

  Research in other programs that lock mailboxes shows that there is no
  universally standard method. Having mailboxes NFS-mounted on the system that
  is delivering mail is not the best thing, but people do run like this,
  and so the code must do its best to cope.

  Three different locking mechanisms are supported. The initialization function
  checks that at least one is configured.

  LOCK FILES

  Unless no_use_lockfile is set, we attempt to build a lock file in a way that
  will work over NFS. Only after that is done do we actually open the mailbox
  and apply locks to it (if configured).

  Originally, Exim got the file opened before doing anything about locking.
  However, a very occasional problem was observed on Solaris 2 when delivering
  over NFS. It is seems that when a file is opened with O_APPEND, the file size
  gets remembered at open time. If another process on another host (that's
  important) has the file open and locked and writes to it and then releases
  the lock while the first process is waiting to get the lock, the first
  process may fail to write at the new end point of the file - despite the very
  definite statement about O_APPEND in the man page for write(). Experiments
  have reproduced this problem, but I do not know any way of forcing a host to
  update its attribute cache for an open NFS file. It would be nice if it did
  so when a lock was taken out, but this does not seem to happen. Anyway, to
  reduce the risk of this problem happening, we now create the lock file
  (if configured) *before* opening the mailbox. That will prevent two different
  Exims opening the file simultaneously. It may not prevent clashes with MUAs,
  however, but Pine at least seems to operate in the same way.

  Lockfiles should normally be used when NFS is involved, because of the above
  problem.

  The logic for creating the lock file is:

  . The name of the lock file is <mailbox-name>.lock

  . First, create a "hitching post" name by adding the primary host name,
    current time and pid to the lock file name. This should be unique.

  . Create the hitching post file using WRONLY + CREAT + EXCL.

  . If that fails EACCES, we assume it means that the user is unable to create
    files in the mail spool directory. Some installations might operate in this
    manner, so there is a configuration option to allow this state not to be an
    error - we proceed to lock using fcntl only, after the file is open.

  . Otherwise, an error causes a deferment of the address.

  . Hard link the hitching post to the lock file name.

  . If the link succeeds, we have successfully created the lock file. Simply
    close and unlink the hitching post file.

  . If the link does not succeed, proceed as follows:

    o Fstat the hitching post file, and then close and unlink it.

    o Now examine the stat data. If the number of links to the file is exactly
      2, the locking succeeded but for some reason, e.g. an NFS server crash,
      the return never made it back, so the link() function gave a failure
      return.

  . This method allows for the lock file to be created by some other process
    right up to the moment of the attempt to hard link it, and is also robust
    against NFS server crash-reboots, which would probably result in timeouts
    in the middle of link().

  . System crashes may cause lock files to get left lying around, and some means
    of flushing them is required. The approach of writing a pid (used by smail
    and by elm) into the file isn't useful when NFS may be in use. Pine uses a
    timeout, which seems a better approach. Since any program that writes to a
    mailbox using a lock file should complete its task very quickly, Pine
    removes lock files that are older than 5 minutes. We allow the value to be
    configurable on the director.

  FCNTL LOCKING

  If use_fcntl_lock is set, then Exim gets an exclusive fcntl() lock on the
  mailbox once it is open. This is done by default with a non-blocking lock.
  Failures to lock cause retries after a sleep, but only for a certain number
  of tries. A blocking lock is deliberately not used so that we don't hold the
  mailbox open. This minimizes the possibility of the NFS problem described
  under LOCK FILES above, if for some reason NFS deliveries are happening
  without lock files. However, the use of a non-blocking lock and sleep, though
  the safest approach, does not give the best performance on very busy systems.
  A blocking lock plus timeout does better. Therefore Exim has an option to
  allow it to work this way. If lock_fcntl_timeout is set greater than zero, it
  enables the use of blocking fcntl() calls.

  MBX LOCKING

  If use_mbx_lock is set (this is supported only if SUPPORT_MBX is defined)
  then the rules used for locking in c-client are used. Exim takes out a shared
  lock on the mailbox file, and an exclusive lock on the file whose name is
  /tmp/.<device-number>.<inode-number>. The shared lock on the mailbox stops
  any other MBX client from getting an exclusive lock on it and expunging it.
  It also stops any other MBX client from unlinking the /tmp lock when it has
  finished with it.

  The exclusive lock on the /tmp file prevents any other MBX client from
  updating the mailbox in any way. When writing is finished, if an exclusive
  lock on the mailbox can be obtained, indicating there are no current sharers,
  the /tmp file is unlinked.

  The fcntl() calls for getting these locks are by default non-blocking, as for
  non-mbx locking, but can be made blocking by setting lock_fcntl_timeout.
  As MBX delivery doesn't work over NFS, it probably makes sense to set it
  for any MBX deliveries. */


  /* Build a lock file if configured to do so - the existence of a lock
  file is subsequently checked by looking for a non-negative value of the
  file descriptor hd - even though the file is no longer open. */

  if (ob->use_lockfile)
    {
    lockname = string_sprintf("%s.lock", filename);
    hitchname = string_sprintf( "%s.%s.%08x.%08x", lockname, primary_hostname,
      (unsigned int)(time(NULL)), (unsigned int)getpid());

    DEBUG(9) debug_printf("lock name: %s\nhitch name: %s\n", lockname,
      hitchname);

    /* Lock file creation retry loop */

    for (i = 0; i < ob->lock_retries; sleep(ob->lock_interval), i++)
      {
      int rc;
      hd = open(hitchname, O_WRONLY | O_CREAT | O_EXCL, ob->lockfile_mode);

      if (hd < 0)
        {
        if (errno != EACCES || ob->require_lockfile)
          {
          addr->basic_errno = errno;
          addr->message =
            string_sprintf("creating lock file hitching post %s "
              "(euid=%ld egid=%ld)", hitchname, (long int)geteuid(),
              (long int)getegid());
          return FALSE;
          }
        else break;
        }

      /* Attempt to hitch the hitching post to the lock file. If link()
      succeeds (the common case, we hope) all is well. Otherwise, fstat the
      file, and get rid of the hitching post. If the number of links was 2,
      the link was created, despite the failure of link(). If the hitch was
      not successful, try again, having unlinked the lock file if it is too
      old.

      There's a version of Linux (2.0.27) which doesn't update its local cache
      of the inode after link() by default - which many think is a bug - but
      if the link succeeds, this code will be OK. It just won't work in the
      case when link() fails after having actually created the link. The Linux
      NFS person is fixing this; a temporary patch is available if anyone is
      sufficiently worried. */

      if ((rc = link(hitchname, lockname)) != 0) fstat(hd, &statbuf);
      close(hd);
      unlink(hitchname);
      if (rc != 0 && statbuf.st_nlink != 2)
        {
        if (ob->lockfile_timeout > 0 && stat(lockname, &statbuf) == 0 &&
            time(NULL) - statbuf.st_ctime > ob->lockfile_timeout)
          {
          DEBUG(2) debug_printf("unlinking timed-out lock file\n");
          unlink(lockname);
          }
        DEBUG(9) debug_printf("link of hitching post failed - retrying\n");
        continue;
        }

      DEBUG(9) debug_printf("lock file created\n");
      break;
      }

    /* Check for too many tries at creating the lock file */

    if (i >= ob->lock_retries)
      {
      addr->basic_errno = ERRNO_LOCKFAILED;
      addr->message = string_sprintf("failed to lock mailbox %s (lock file)",
        filename);
      return FALSE;
      }
    }


  /* We now have to get the file open. First, stat() it and act on existence or
  non-existence. This is in a loop to handle the case of a file's being created
  or deleted as we watch, and also to handle retries when the locking fails.
  Rather than holding the file open while waiting for the fcntl() lock, we
  close and do the whole thing again. This should be safer, especially for NFS
  files, which might get altered from other hosts, making their cached sizes
  incorrect.

  With the default settings, no symlinks are permitted, but there is an option
  to permit symlinks for those sysadmins that know what they are doing.
  Shudder. However, insist that the initial symlink is owned by the right user.
  Thus lstat() is used initially; if a symlink is discovered, the loop is
  repeated such that stat() is used, to look at the end file. */

  for (i = 0; i < ob->lock_retries; i++)
    {
    int sleep_before_retry = TRUE;
    file_opened = FALSE;

    if((use_lstat? lstat(filename, &statbuf) : stat(filename, &statbuf)) != 0)
      {
      /* Let's hope that failure to stat (other than non-existence) is a
      rare event. */

      if (errno != ENOENT)
        {
        addr->basic_errno = errno;
        addr->message = string_sprintf("attempting to stat mailbox %s",
          filename);
        goto RETURN;
        }

      /* File does not exist. If it is required to pre-exist this state is an
      error. */

      if (ob->file_must_exist)
        {
        addr->basic_errno = errno;
        addr->message = string_sprintf("mailbox %s does not exist, "
          "but file_must_exist is set", filename);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* If not permitted to create this file because it isn't in or below
      the home directory, generate an error. */

      if (!allow_creation_here)
        {
        addr->basic_errno = ERRNO_BADCREATE;
        addr->message = string_sprintf("mailbox %s does not exist, "
          "but creation outside the home directory is not permitted",
          filename);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* Attempt to create and open the file. If open fails because of
      pre-existence, go round the loop again. For any other error, defer the
      address, except for an alias or forward generated file name with EPERM,
      ENOENT, or EACCES, as those are most likely to be user errors rather
      than Exim config errors. When a symbolic link is permitted and points
      to a non-existent file, we get here with use_lstat = FALSE. In this case
      we mustn't use O_EXCL, since it doesn't work. The file is opened RDRW for
      consistency and because MBX locking requires it in order to be able to
      get a shared lock. */

      fd = open(filename, O_RDWR | O_APPEND | O_CREAT |
        (use_lstat? O_EXCL : 0), mode);
      if (fd < 0)
        {
        if (errno == EEXIST) continue;
        addr->basic_errno = errno;
        addr->message = string_sprintf("while creating mailbox %s",
          filename);
        if (testflag(addr, af_pfr) && addr->local_part[0] == '/' &&
            (errno == EPERM || errno == ENOENT || errno == EACCES))
          addr->transport_return = FAIL;
        goto RETURN;
        }

      /* We have successfully created and opened the file. Ensure that the group
      and the mode are correct. */

      chown(filename, uid, gid);
      chmod(filename, mode);
      }


    /* The file already exists. Test its type, ownership, and permissions, and
    save the inode for checking later. If symlinks are permitted (not the
    default or recommended state) it may be a symlink that already exists.
    Check its ownership and then look for the file at the end of the link(s).
    This at least prevents one user creating a symlink for another user in
    a sticky directory. */

    else
      {
      int oldmode = (int)statbuf.st_mode;
      int inode = statbuf.st_ino;
      BOOL islink = (oldmode & S_IFMT) == S_IFLNK;

      isfifo = FALSE;        /* In case things are changing */

      /* Check owner if required - the default. */

      if (ob->check_owner && statbuf.st_uid != uid)
        {
        addr->basic_errno = ERRNO_BADUGID;
        addr->message = string_sprintf("mailbox %s%s has wrong uid "
          "(%ld != %ld)", filename,
          islink? " (symlink)" : "",
          (long int)(statbuf.st_uid), (long int)uid);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* Group is checked only if check_group is set. */

      if (ob->check_group && statbuf.st_gid != gid)
        {
        addr->basic_errno = ERRNO_BADUGID;
        addr->message = string_sprintf("mailbox %s%s has wrong gid (%d != %d)",
          filename, islink? " (symlink)" : "", statbuf.st_gid, gid);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* If symlinks are permitted (not recommended), the lstat() above will
      have found the symlink. Its ownership has just been checked; go round
      the loop again, using stat() instead of lstat(). That will never yield a
      mode of S_IFLNK. */

      if (islink && ob->allow_symlink)
        {
        use_lstat = FALSE;
        i--;                   /* Don't count this time round */
        continue;
        }

      /* An actual file exists. Check that it is a regular file, or FIFO
      if permitted. */

      if (ob->allow_fifo && (oldmode & S_IFMT) == S_IFIFO) isfifo = TRUE;

      else if ((oldmode & S_IFMT) != S_IFREG)
        {
        addr->basic_errno = ERRNO_NOTREGULAR;
        addr->message = string_sprintf("mailbox %s is not a regular file%s",
          filename, ob->allow_fifo? " or named pipe" : "");
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* If the mode is not what it would be for a newly created file, change
      the permissions if the mode is supplied for the address. Otherwise,
      reduce but do not extend the permissions. If the newly created
      permissions are greater than the existing permissions, don't change
      things when the mode is not from the address. */

      if ((oldmode = (oldmode & 07777)) != mode)
        {
        int diffs = oldmode ^ mode;
        if (addr->mode > 0 || (diffs & oldmode) == diffs)
          {
          DEBUG(2) debug_printf("chmod %o %s\n", mode, filename);
          if (chmod(filename, mode) < 0)
            {
            addr->basic_errno = errno;
            addr->message = string_sprintf("attempting to chmod mailbox %s",
              filename);
            addr->special_action = SPECIAL_FREEZE;
            goto RETURN;
            }
          oldmode = mode;
          }

        /* Mode not from address, and newly-created permissions are greater
        than existing permissions. Default is to complain, but it can be
        configured to go ahead and try to deliver anyway if that's what
        the administration wants. */

        else if (ob->mode_fail_narrower)
          {
          addr->basic_errno = ERRNO_BADMODE;
          addr->message = string_sprintf("mailbox %s has wrong mode %o",
            filename, oldmode);
          addr->special_action = SPECIAL_FREEZE;
          goto RETURN;
          }
        }

      /* We are happy with the existing file. Open it, and then do further
      tests to ensure that it is the same file that we were just looking at.
      If the file does not now exist, restart this loop, going back to using
      lstat again. For an NFS error, just defer; other opening errors are
      more serious. The file is opened RDWR so that its format can be checked,
      and also MBX locking requires the use of a shared (read) lock. However,
      a FIFO is opened WRONLY + NDELAY so that it fails if there is no process
      reading the pipe. */

      fd = open(filename, isfifo? (O_WRONLY|O_NDELAY) : (O_RDWR|O_APPEND),
        mode);
      if (fd < 0)
        {
        if (errno == ENOENT)
          {
          use_lstat = TRUE;
          continue;
          }
        addr->basic_errno = errno;
        if (isfifo)
          {
          addr->message = string_sprintf("while opening named pipe %s "
            "(could mean no process is reading it)", filename);
          }
        else if (errno != EWOULDBLOCK)
          {
          addr->message = string_sprintf("while opening mailbox %s", filename);
          addr->special_action = SPECIAL_FREEZE;
          }
        goto RETURN;
        }

      /* This fstat really shouldn't fail, as we have an open file! There's a
      dilemma here. We use fstat in order to be sure we are peering at the file
      we have got open. However, that won't tell us if the file was reached
      via a symbolic link. We checked this above, but there is a race exposure
      if the link was created between the previous lstat and the open. However,
      it would have to be created with the same inode in order to pass the
      check below. If ob->allow_symlink is set, causing the use of stat rather
      than lstat above, symbolic links may be there anyway, and the checking is
      weaker. */

      if (fstat(fd, &statbuf) < 0)
        {
        addr->basic_errno = errno;
        addr->message = string_sprintf("attempting to stat open mailbox %s",
          filename);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* Check the inode; this is isn't a perfect check, but gives some
      confidence. */

      if (inode != statbuf.st_ino)
        {
        addr->basic_errno = ERRNO_INODECHANGED;
        addr->message = string_sprintf("opened mailbox %s inode number changed "
          "from %d to %d", filename, inode, statbuf.st_ino);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* Check it's still a regular file or FIFO, and the uid, gid, and
      permissions have not changed. */

      if ((!isfifo && (statbuf.st_mode & S_IFMT) != S_IFREG) ||
          (isfifo && (statbuf.st_mode & S_IFMT) != S_IFIFO))
        {
        addr->basic_errno = ERRNO_NOTREGULAR;
        addr->message =
          string_sprintf("opened mailbox %s is no longer a %s", filename,
            isfifo? "named pipe" : "regular file");
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      if ((ob->check_owner && statbuf.st_uid != uid) ||
          (ob->check_group && statbuf.st_gid != gid))
        {
        addr->basic_errno = ERRNO_BADUGID;
        addr->message =
          string_sprintf("opened mailbox %s has wrong uid or gid", filename);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      if ((statbuf.st_mode & 07777) != oldmode)
        {
        addr->basic_errno = ERRNO_BADMODE;
        addr->message = string_sprintf("opened mailbox %s has wrong mode %o "
          "(%o expected)", filename, statbuf.st_mode & 07777, mode);
        addr->special_action = SPECIAL_FREEZE;
        goto RETURN;
        }

      /* If file_format is set, check that the format of the file has not
      changed. Error data is set by the testing function. */

      if (ob->file_format != NULL &&
          check_file_format(fd, tblock, addr) != tblock)
        {
        addr->message = "open mailbox has changed format";
        goto RETURN;
        }

      /* The file is OK. Carry on to do the locking. */
      }

    /* We now have an open file, and must lock it using fcntl() or MBX locking
    rules if configured to do so. If a lock file is also required, it was
    created above and hd was left >= 0. At least one form of locking is
    required by the initialization function. If locking fails here, close the
    file and go round the loop all over again, after waiting for a bit, unless
    blocking locking was used. */

    file_opened = TRUE;
    if (ob->lock_fcntl_timeout > 0) sleep_before_retry = FALSE;

    /* Simple fcntl() locking */

    if (ob->use_fcntl)
      {
      if (apply_lock(fd, F_WRLCK, ob->lock_fcntl_timeout) >= 0) break;
      }

    /* MBX locking rules */

    #ifdef SUPPORT_MBX
    else if (ob->use_mbx_lock)
      {
      if (apply_lock(fd, F_RDLCK, ob->lock_fcntl_timeout) >= 0 &&
          fstat(fd, &statbuf) >= 0)
        {
        sprintf(mbx_lockname, "/tmp/.%lx.%lx", (long)statbuf.st_dev,
          (long)statbuf.st_ino);

        if (lstat(mbx_lockname, &statbuf) >= 0)
          {
          if ((statbuf.st_mode & S_IFMT) == S_IFLNK)
            {
            addr->basic_errno = ERRNO_LOCKFAILED;
            addr->message = string_sprintf("symbolic link on MBX lock file %s",
              mbx_lockname);
            goto RETURN;
            }
          if (statbuf.st_nlink > 1)
            {
            addr->basic_errno = ERRNO_LOCKFAILED;
            addr->message = string_sprintf("hard link to MBX lock file %s",
              mbx_lockname);
            goto RETURN;
            }
          }

        mbx_lockfd = open(mbx_lockname, O_RDWR | O_CREAT, 0600);
        if (mbx_lockfd < 0)
          {
          addr->basic_errno = ERRNO_LOCKFAILED;
          addr->message = string_sprintf("failed to open MBX lock file %s :%s",
            mbx_lockname, strerror(errno));
          goto RETURN;
          }

        chmod (mbx_lockname, 0600);

        if (apply_lock(mbx_lockfd, F_WRLCK, ob->lock_fcntl_timeout) >= 0) break;

        DEBUG(9) debug_printf("failed to lock %s: %s\n", mbx_lockname,
          strerror(errno));
        close(mbx_lockfd);
        mbx_lockfd = -1;
        }
      else
        {
        DEBUG(9) debug_printf("failed to fstat or get read lock on %s: %s\n",
          filename, strerror(errno));
        }
      }
    #endif

    else break;   /* No on-file locking required; break the open/lock loop */

    DEBUG(9) debug_printf("fcntl() or MBX locking failed - retrying\n");

    close(fd);
    fd = -1;
    use_lstat = TRUE;             /* Reset to use lstat first */


    /* If a blocking call timed out, break the retry loop if the total time
    so far is not less than than retries * interval. */

    if (sigalrm_seen &&
        (i+1) * ob->lock_fcntl_timeout >= ob->lock_retries * ob->lock_interval)
      i = ob->lock_retries;

    /* Wait a bit before retrying, except when it was a blocked fcntl() that
    caused the problem. */

    if (i < ob->lock_retries && sleep_before_retry) sleep(ob->lock_interval);
    }

  /* Test for exceeding the maximum number of tries. Either the file remains
  locked, or, if we haven't got it open, something is terribly wrong... */

  if (i >= ob->lock_retries)
    {
    if (!file_opened)
      {
      addr->basic_errno = ERRNO_EXISTRACE;
      addr->message = string_sprintf("mailbox %s: existence unclear", filename);
      addr->special_action = SPECIAL_FREEZE;
      }
    else
      {
      addr->basic_errno = ERRNO_LOCKFAILED;
      addr->message = string_sprintf("failed to lock mailbox %s (fcntl)",
        filename);
      }
    goto RETURN;
    }

  DEBUG(9) debug_printf("mailbox %s is locked\n", filename);

  /* Save access time (for subsequent restoration), modification time (for
  restoration if updating fails), size of file (for comsat and for re-setting if
  delivery fails in the middle - e.g. for quota exceeded). */

  if (fstat(fd, &statbuf) < 0)
    {
    addr->basic_errno = errno;
    addr->message = string_sprintf("while fstatting opened mailbox %s",
      filename);
    goto RETURN;
    }

  times.actime = statbuf.st_atime;
  times.modtime = statbuf.st_mtime;
  saved_size = statbuf.st_size;
  }

/* Handle the case of creating a unique file in a given directory (not in
maildir or mailstore format - this is how smail did it). A temporary name is
used to create the file. Later, when it is written, the name is changed to a
unique one. There is no need to lock the file. An attempt is made to create the
directory if it does not exist. */

else if (mbformat == mbf_smail)
  {
  DEBUG(9) debug_printf("delivering to unique file (smail format)\n");
  filename = dataname =
    string_sprintf("%s/temp.%d.%s", path, (int)getpid(), primary_hostname);
  fd = open(filename, O_WRONLY|O_CREAT, mode);
  if (fd < 0 &&                                    /* failed to open, and */
      (errno != ENOENT ||                          /* either not non-exist */
       !ob->create_directory ||                    /* or not allowed to make */
       !directory_make(NULL, path, ob->dirmode, FALSE) ||  /* or failed to create dir */
       (fd = open(filename, O_WRONLY|O_CREAT, mode)) < 0)) /* or then failed to open */
    {
    addr->basic_errno = errno;
    addr->message = string_sprintf("while creating file %s", filename);
    return FALSE;
    }

  /* Why are these here? I should have stuck in a comment. Perhaps it's to
  do with soft links? */

  chown(filename, uid, gid);
  chmod(filename, mode);

  /* Build the new unique name. It starts with 'q' for smail compatibility.
  The fiddling around is because string_base62 returns its result in a fixed
  buffer always (exactly 6 chars plus zero). */

  if (stat(filename, &statbuf) < 0)
    {
    addr->basic_errno = errno;
    addr->message = string_sprintf("after stat() for %s", filename);
    goto RETURN;
    }

  strcpy(big_buffer, string_base62(time(NULL)));
  newname = string_sprintf("%s/q%s-%s", path, big_buffer,
    string_base62((unsigned long int)statbuf.st_ino));
  }

#ifdef SUPPORT_MAILDIR

/* Handle the case of a unique file in maildir format. The file is written to
the tmp subdirectory, with a prescribed form of name. */

else if (mbformat == mbf_maildir)
  {
  char *subdirs[] = { "/tmp", "/new", "/cur" };

  DEBUG(9) debug_printf("delivering in maildir format in %s\n", path);

  nametag = ob->maildir_tag;

  /* Check that nametag expands successfully; a hard failure causes a panic
  return. The actual expansion for use happens again later, when $message_size
  is accurately known. */

  if (nametag != NULL && expand_string(nametag) == NULL &&
      !expand_string_forcedfail)
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("Expansion of \"%s\" (maildir_tag "
      "for %s transport) failed: %s", nametag, tblock->name,
      expand_string_message);
    return FALSE;
    }

  /* First ensure that the path we have is a directory; if it does not exist,
  create it. Then make sure the tmp, new & cur subdirs of the maildir are
  there. If not we abort the delivery (even though the cur subdir is not
  actually needed for delivery). Handle all 4 directory tests/creates in a
  loop so that code can be shared. */

  for (i = 0; i < 4; i++)
    {
    int j;
    char *dir, *mdir;

    if (i == 0)
      {
      mdir = "";
      dir = path;
      }
    else
      {
      mdir = subdirs[i-1];
      dir = mdir + 1;
      }

    /* Check an existing path is a directory. This is inside a loop because
    there is a potential race condition when creating the directory - some
    other process may get there first. Give up after trying several times,
    though. */

    for (j = 0; j < 10; j++)
      {
      if (stat(dir, &statbuf) == 0)
        {
        if (S_ISDIR(statbuf.st_mode)) break;   /* out of the race loop */
        addr->message = string_sprintf("%s%s is not a directory", path,
          mdir);
        addr->basic_errno = ERRNO_NOTDIRECTORY;
        addr->special_action = SPECIAL_FREEZE;
        return FALSE;
        }

      /* Try to make if non-existent and configured to do so */

      if (errno == ENOENT && ob->create_directory)
        {
        if (!directory_make(NULL, dir, ob->dirmode, FALSE))
          {
          if (errno == EEXIST) continue;     /* repeat the race loop */
          addr->message = string_sprintf("cannot create %s%s", path, mdir);
	  addr->basic_errno = errno;
	  addr->special_action = SPECIAL_FREEZE;
	  return FALSE;
          }
        DEBUG(9) debug_printf("created directory %s%s\n", path, mdir);
        break;   /* out of the race loop */
        }

      /* stat() error other than ENOENT, or ENOENT and not creatable */

      addr->message = string_sprintf("stat() error for %s%s: %s", path, mdir,
        strerror(errno));
      addr->basic_errno = errno;
      addr->special_action = SPECIAL_FREEZE;
      return FALSE;
      }

    /* If we went round the loop 10 times, the directory was flickering in and
    out of existence like someone in a malfunctioning Star Trek transporter. */

    if (j >= 10)
      {
      addr->message = string_sprintf("existence of %s%s unclear\n", path, mdir);
      addr->basic_errno = errno;
      addr->special_action = SPECIAL_FREEZE;
      return FALSE;
      }

    /* First time through the directories loop, cd to the main directory */

    if (i == 0 && chdir(path) != 0)
      {
      addr->message = string_sprintf ("cannot chdir to %s", path);
      addr->basic_errno = errno;
      addr->special_action = SPECIAL_FREEZE;
      return FALSE;
      }
    }

  /* All relevant directories now exist. Attempt to open the temporary file a
  limited number of times. I think this rather scary-looking for statement is
  actually OK. If open succeeds, the loop is broken; if not, there is a test
  on the value of i. Get the time again afresh each time round the loop. */

  for (i = 1;; i++)
    {
    char *basename = string_sprintf("%lu.%lu.%s", time(NULL), getpid(),
      primary_hostname);

    filename = dataname = string_sprintf("tmp/%s", basename);
    newname = string_sprintf("new/%s", basename);

    if (stat(filename, &statbuf) == 0)
      errno = EEXIST;
    else if (errno == ENOENT)
      {
      fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, mode);
      if (fd >= 0) break;
      DEBUG (9) debug_printf ("open failed for %s: %s\n",
        filename, strerror(errno));
      }

    /* Too many retries - give up */

    if (i >= ob->maildir_retries)
      {
      addr->message = string_sprintf ("failed to open %s (%d tr%s)",
        filename, i, (i == 1)? "y" : "ies");
      addr->basic_errno = errno;
      return FALSE;
      }

    /* Open or stat failed but we haven't tried too many times yet. */

    sleep(2);
    }

  /* Why are these here? Put in because they are present in the non-maildir
  directory case above. */

  chown(filename, uid, gid);
  chmod(filename, mode);
  }

#endif

#ifdef SUPPORT_MAILSTORE

/* Handle the case of a unique file in mailstore format. First write the
envelope to a temporary file, then open the main file. The unique base name for
the files consists of the message id plus the pid of this delivery process. */

else
  {
  FILE *env_file;
  address_item *taddr;
  char *basename = string_sprintf("%s/%s-%s", path, message_id,
    string_base62((long int)getpid()));

  DEBUG(9) debug_printf("delivering in mailstore format in %s\n", path);

  filename = string_sprintf("%s.tmp", basename);
  newname  = string_sprintf("%s.env", basename);
  dataname = string_sprintf("%s.msg", basename);

  fd = open(filename, O_WRONLY|O_CREAT|O_EXCL, mode);
  if (fd < 0 &&                                    /* failed to open, and */
      (errno != ENOENT ||                          /* either not non-exist */
       !ob->create_directory ||                    /* or not allowed to make */
       !directory_make(NULL, path, ob->dirmode, FALSE) ||  /* or failed to create dir */
       (fd = open(filename, O_WRONLY|O_CREAT|O_EXCL, mode)) < 0)) /* or then failed to open */
    {
    addr->basic_errno = errno;
    addr->message = string_sprintf("while creating file %s", filename);
    return FALSE;
    }

  /* Why are these here? Put in because they are present in the non-maildir
  directory case above. */

  chown(filename, uid, gid);
  chmod(filename, mode);

  /* Built a C stream from the open file descriptor. */

  if ((env_file = fdopen(fd, "w")) == NULL)
    {
    addr->basic_errno = errno;
    addr->transport_return = PANIC;
    addr->message = string_sprintf("fdopen of %s ("
      "for %s transport) failed", filename, tblock->name);
    close(fd);
    unlink(filename);
    return FALSE;
    }

  /* Write the envelope file, then close it. */

  if (ob->mailstore_prefix != NULL)
    {
    char *s = expand_string(ob->mailstore_prefix);
    if (s == NULL)
      {
      if (!expand_string_forcedfail)
        {
        addr->transport_return = PANIC;
        addr->message = string_sprintf("Expansion of \"%s\" (mailstore prefix "
          "for %s transport) failed: %s", ob->mailstore_prefix, tblock->name,
          expand_string_message);
        fclose(env_file);
        unlink(filename);
        return FALSE;
        }
      }
    else
      {
      int n = (int)strlen(s);
      fprintf(env_file, "%s", s);
      if (n == 0 || s[n-1] != '\n') fprintf(env_file, "\n");
      }
    }

  fprintf(env_file, "%s\n", sender_address);

  for (taddr = addr; taddr!= NULL; taddr = taddr->next)
    fprintf(env_file, "%s@%s\n", taddr->local_part, taddr->domain);

  if (ob->mailstore_suffix != NULL)
    {
    char *s = expand_string(ob->mailstore_suffix);
    if (s == NULL)
      {
      if (!expand_string_forcedfail)
        {
        addr->transport_return = PANIC;
        addr->message = string_sprintf("Expansion of \"%s\" (mailstore suffix "
          "for %s transport) failed: %s", ob->mailstore_suffix, tblock->name,
          expand_string_message);
        fclose(env_file);
        unlink(filename);
        return FALSE;
        }
      }
    else
      {
      int n = (int)strlen(s);
      fprintf(env_file, "%s", s);
      if (n == 0 || s[n-1] != '\n') fprintf(env_file, "\n");
      }
    }

  if (fclose(env_file) != 0)
    {
    addr->basic_errno = errno;
    addr->message = string_sprintf("while closing %s", filename);
    unlink(filename);
    return FALSE;
    }

  DEBUG(9) debug_printf("Envelope file %s written\n", filename);

  /* Now open the data file */

  fd = open(dataname, O_WRONLY|O_CREAT|O_EXCL, mode);
  if (fd < 0)
    {
    addr->basic_errno = errno;
    addr->message = string_sprintf("while creating file %s", dataname);
    unlink(filename);
    return FALSE;
    }

  /* Why are these here? Put in because they are present in the non-maildir
  directory case above. */

  chown(dataname, uid, gid);
  chmod(dataname, mode);
  }

#endif

/* At last we can write the message to the file, preceded by any configured
prefix line, and followed by any configured suffix line. If there are any
writing errors, we must defer. */

DEBUG(9) debug_printf("writing to file %s\n", dataname);

yield = OK;
errno = 0;

/* If quota or quota_warn_threshold is set for any of the directory formats,
find the current usage within that directory and the number of files, which
will include the new one we are about to write. This is expensive, but happens
only if configured. */

if (isdirectory && (ob->quota_value > 0 || ob->quota_warn_threshold_value > 0))
  {
  const char *error;
  char *check_path = path;
  int offset;
  pcre *regex = NULL;

  if (ob->quota_size_regex != NULL)
    {
    regex = pcre_compile(ob->quota_size_regex, PCRE_COPT, &error,&offset, NULL);
    if (regex == NULL)
      {
      DEBUG(9) debug_printf("check_dir_size: regular expression error: "
        "%s at offset %d while compiling %s", error, offset,
        ob->quota_size_regex);
      }
    }

  /* If we are handling a maildir delivery, and the directory contains a file
  called maildirfolder, this is a maildir++ feature telling us that this is a
  sub-directory of the real inbox. We should therefore do the quota check on
  the parent directory. */

  #ifdef SUPPORT_MAILDIR
  if (mbformat == mbf_maildir)
    {
    struct stat statbuf;
    if (stat("maildirfolder", &statbuf) >= 0)
      {
      char *slash = strrchr(path, '/');
      if (slash != NULL)
        check_path = string_sprintf("%.*s", slash - path, path);
      }
    }
  #endif

  /* Compute the size of the relevant directory */

  saved_size = check_dir_size(check_path, &dircount, regex);
  }

/* If there is a local quota setting, check that we are not going to exceed it
with this message if quota_is_inclusive is set; if it is not set, the check
is for the mailbox already being over quota (i.e. the current message is not
included in the check). */

if (ob->quota_value > 0)
  {
  DEBUG(9)
    {
    debug_printf("Exim quota = %d old size = %d this message = %d "
      "(%sincluded)\n", ob->quota_value, saved_size, message_size,
      ob->quota_is_inclusive? "" : "not ");
    debug_printf("  file quota = %d count = %d\n", ob->quota_filecount,
      dircount);
    }
  if (saved_size + (ob->quota_is_inclusive? message_size : 0) >
      ob->quota_value ||
      (ob->quota_filecount > 0 && dircount > ob->quota_filecount))
    {
    yield = DEFER;
    errno = ERRNO_EXIMQUOTA;
    }
  }

/* If there is a quota warning threshold and we are going to cross it with this
message, set the SPECIAL_WARN flag in the address, to cause a warning message
to be sent. */

if (ob->quota_warn_threshold_value > 0)
  {
  int threshold = ob->quota_warn_threshold_value;
  if (ob->quota_warn_threshold_is_percent)
    threshold = (int)(((double)ob->quota_value * threshold) / 100);
  DEBUG(2)
    debug_printf("quota threshold = %d old size = %d message size = %d\n",
      threshold, saved_size, message_size);
  if (saved_size <= threshold && saved_size + message_size > threshold)
    addr->special_action = SPECIAL_WARN;
  }

/* If we are writing in MBX format, what we actually do is to write the message
to a temporary file, and then copy it to the real file once we know its size.
This is the most straightforward way of getting the correct length in the
separator line. So, what we do here is to save the real file descriptor, and
replace it with one for a temporary file. */

#ifdef SUPPORT_MBX
if (yield == OK && ob->mbx_format)
  {
  temp_file = tmpfile();
  if (temp_file == NULL)
    {
    addr->basic_errno = errno;
    addr->message = "while setting up temporary file";
    yield = DEFER;
    goto RETURN;
    }
  save_fd = fd;
  fd = fileno(temp_file);
  DEBUG(9) debug_printf("writing to temporary file\n");
  }
#endif


/* If the local_smtp option is not unset, we need to write SMTP prefix
information. The various different values for batching are handled outside; if
there is more than one address available here, all must be included. If any
address is a file, use it's parent in the RCPT TO. */

if (tblock->local_smtp != local_smtp_off && yield == OK)
  {
  if (ob->use_crlf) cr = "\r";
  if (tblock->bsmtp_helo &&
      !transport_write_string(fd, "HELO %s%s\n", primary_hostname, cr))
    yield = DEFER;
  else
    if (!transport_write_string(fd, "MAIL FROM:<%s>%s\n", return_path, cr))
      yield = DEFER;
  else
    {
    address_item *a;
    for (a = addr; a != NULL; a = a->next)
      {
      address_item *b = testflag(a, af_pfr)? a->parent: a;
      if (!transport_write_string(fd, "RCPT TO:<%s@%s>%s\n", b->local_part,
        b->domain, cr))
          { yield = DEFER; break; }
      }
    if (yield == OK && !transport_write_string(fd, "DATA%s\n", cr))
      yield = DEFER;
    }
  }

/* Now any other configured prefix. */

if (yield == OK && ob->prefix != NULL && ob->prefix[0] != 0)
  {
  char *prefix = expand_string(ob->prefix);
  if (prefix == NULL)
    {
    errno = ERRNO_EXPANDFAIL;
    addr->transport_return = PANIC;
    addr->message = string_sprintf("Expansion of \"%s\" (prefix for %s "
      "transport) failed", ob->prefix, tblock->name);
    yield = DEFER;
    }
  else if (!transport_write_string(fd, "%s", prefix)) yield = DEFER;
  }

/* Now the message itself. The options for transport_write_message were
set up at initialization time. */

if (yield == OK)
  {
  if (!transport_write_message(addr, fd, ob->options, 0, tblock->add_headers,
      tblock->remove_headers, ob->check_string, ob->escape_string,
      tblock->rewrite_rules, tblock->rewrite_existflags))
    yield = DEFER;
  }

/* Now a configured suffix. */

if (yield == OK && ob->suffix != NULL && ob->suffix[0] != 0)
  {
  char *suffix = expand_string(ob->suffix);
  if (suffix == NULL)
    {
    errno = ERRNO_EXPANDFAIL;
    addr->transport_return = PANIC;
    addr->message = string_sprintf("Expansion of \"%s\" (suffix for %s "
      "transport) failed", ob->suffix, tblock->name);
    yield = DEFER;
    }
  else if (!transport_write_string(fd, "%s", suffix)) yield = DEFER;
  }

/* If local_smtp, write the terminating dot. */

if (yield == OK && tblock->local_smtp != local_smtp_off &&
  !transport_write_string(fd, ".%s\n", cr)) yield = DEFER;


/* If MBX format is being used, all that writing was to the temporary file.
However, if there was an earlier failure (Exim quota exceeded, for example),
the temporary file won't have got opened - and no writing will have been done.
If writing was OK, we restore the fd, and call a function that copies the
message in MBX format into the real file. Otherwise use the temporary name in
any messages. */

#ifdef SUPPORT_MBX
if (temp_file != NULL && ob->mbx_format)
  {
  int save_errno;

  fd = save_fd;

  if (yield == OK)
    {
    transport_count = 0;   /* Reset transport count for actual write */
    yield = copy_mbx_message(fd, fileno(temp_file), saved_size);
    }
  else if (errno >= 0) dataname = "temporary file";

  /* Preserve errno while closing the temporary file. */

  save_errno = errno;
  fclose(temp_file);
  errno = save_errno;
  }
#endif

/* Force out the remaining data to check for any errors; some OS don't allow
fsync() to be called for a FIFO. */

if (yield == OK && !isfifo && fsync(fd) < 0) yield = DEFER;

/* Handle error while writing the file. Control should come here directly after
the error, with the reason in errno. In the case of expansion failure in prefix
or suffix, it will be ERRNO_EXPANDFAIL. */

if (yield != OK)
  {
  addr->special_action = SPECIAL_NONE;     /* Cancel any quota warning */

  /* Save the error number. If positive, it will ultimately cause a strerror()
  call to generate some text. */

  addr->basic_errno = errno;

  /* For system or Exim quota excession, or disc full, set more_errno to the
  time since the file was last read. If delivery was into a directory, the
  time since last read logic is not relevant. */

  if (errno == errno_quota || errno == ERRNO_EXIMQUOTA || errno == ENOSPC)
    addr->more_errno = isdirectory? 0 : time(NULL) - times.actime;

  /* Handle system quota excession. Add an explanatory phrase for the error
  message, since some systems don't have special quota-excession errors,
  and on those that do, "quota" doesn't always mean anything to the user. */

  if (errno == errno_quota)
    {
    #ifndef EDQUOT
    addr->message = string_sprintf("mailbox is full "
      "(quota exceeded while writing to file %s)", filename);
    #else
    addr->message = string_sprintf("mailbox is full");
    #endif
    DEBUG(9) debug_printf("System quota exceeded for %s%s%s\n",
      dataname,
      isdirectory? "" : ": time since file read = ",
      isdirectory? "" : readconf_printtime(addr->more_errno));
    }

  /* Handle Exim's own quota-imposition */

  else if (errno == ERRNO_EXIMQUOTA)
    {
    addr->message = string_sprintf("mailbox is full "
      "(MTA-imposed quota exceeded while writing to file %s)", dataname);
    DEBUG(9) debug_printf("Exim quota exceeded for %s%s%s\n",
      dataname,
      isdirectory? "" : ": time since file read = ",
      isdirectory? "" : readconf_printtime(addr->more_errno));
    }

  /* Handle a process failure while writing via a filter; the return
  from child_close() is in more_errno. */

  else if (errno == ERRNO_FILTER_FAIL)
    addr->message = string_sprintf("filter process failure %d while writing "
      "to %s", addr->more_errno, dataname);

  /* Handle failure to expand header changes */

  else if (errno == ERRNO_CHHEADER_FAIL)
    {
    yield = PANIC;
    addr->message =
      string_sprintf("failed to expand headers_add or headers_remove while "
        "writing to %s: %s", dataname, expand_string_message);
    }

  /* Handle failure to complete writing of a data block */

  else if (errno == ERRNO_WRITEINCOMPLETE)
    {
    addr->message = string_sprintf("failed to write data block while "
      "writing to %s", dataname);
    }

  /* Handle length mismatch on MBX copying */

  #ifdef SUPPORT_MBX
  else if (errno == ERRNO_MBXLENGTH)
    {
    addr->message = string_sprintf("length mismatch while copying MBX "
      "temporary file to %s", dataname);
    }
  #endif

  /* For other errors, a general-purpose explanation, if the message is
  not already set. */

  else if (addr->message == NULL)
    addr->message = string_sprintf("error while writing to %s", dataname);

  /* For a file, reset the file size to what it was before we started, leaving
  the last modification time unchanged, so it will get reset also. All systems
  investigated so far have ftruncate(), whereas not all have the F_FREESP
  fcntl() call (BSDI & FreeBSD do not). */

  if (!isdirectory) ftruncate(fd, saved_size);
  }

/* Handle successful writing - we want the modification time to be now for
appended files. Remove the default backstop error number. For a directory, now
is the time to rename the file with a unique name. As soon as such a name
appears it may get used by another process, so we close the file first and
check that all is well. If a tag exists, expand it and add it to the name,
first resetting $message_size to the accurate size. */

else
  {
  times.modtime = time(NULL);
  addr->basic_errno = 0;

  /* Handle the case of writing to a new file in a directory. This applies
  to all single-file formats. */

  if (isdirectory)
    {
    if (fstat(fd, &statbuf) < 0)
      {
      addr->basic_errno = errno;
      addr->message = string_sprintf("while fstatting opened message file %s",
        filename);
      yield = DEFER;
      }

    else if (close(fd) < 0)
      {
      addr->basic_errno = errno;
      addr->message = string_sprintf("close() error for %s",
        (ob->mailstore_format)? dataname : filename);
      yield = DEFER;
      }

    /* File is successfully written. Arrange to rename it, adding the expanded
    tag value to the new name if required. First, correct the message size to
    the accurate value. */

    else
      {
      char *renamename = newname;
      fd = -1;
      message_size = transport_count;

      /* The expansion of nametag was checked above, before the file was
      opened. It either succeeded, or provoked a soft failure. So any failure
      here can be treated as soft. Ignore non-printing characters and / and put
      a colon at the start if the first character is alphanumeric. */

      if (nametag != NULL)
        {
        char *iptr = expand_string(nametag);
        if (iptr != NULL)
          {
          char *etag = store_get(strlen(iptr) + 2);
          char *optr = etag;
          while (*iptr != 0)
	    {
	    if (isgraph((uschar)*iptr) && *iptr != '/')
              {
              if (optr == etag && isalnum((uschar)*iptr)) *optr++ = ':';
              *optr++ = *iptr;
              }
	    iptr++;
	    }
          *optr = 0;
          renamename = string_sprintf("%s%s", newname, etag);
          }
        }

      /* Do the rename. If the name is too long, try again without the tag */

      if (rename(filename, renamename) < 0 &&
         (errno != ENAMETOOLONG ||
          (renamename = newname, rename(filename, renamename) < 0)))
        {
        addr->basic_errno = errno;
        addr->message = string_sprintf("while renaming %s as %s",
          filename, newname);
        yield = DEFER;
        }
      else
        {
        DEBUG(9) debug_printf("renamed %s as %s\n", filename, renamename);
        filename = dataname = NULL;   /* Prevents attempt to unlink at end */
        }
      }
    }
  }


/* For a file, restore the last access time (atime), and set the modification
time as required - changed if write succeeded, unchanged if not. */

if (!isdirectory) utime(filename, &times);

/* Notify comsat if configured to do so. It only makes sense if the configured
file is the one that the comsat daemon knows about. */

if (ob->notify_comsat && yield == OK && deliver_localpart != NULL)
  notify_comsat(deliver_localpart, saved_size);

/* Pass back the final return code in the address structure */

DEBUG(2) debug_printf("appendfile yields %d with errno=%d more_errno=%d\n",
  yield, addr->basic_errno, addr->more_errno);

addr->transport_return = yield;

/* Close the file, which will release the fcntl lock. For a directory write it
is closed above, except in cases of error which goto RETURN, when we also need
to remove the original file(s). For MBX locking, if all has gone well, before
closing the file, see if we can get an exclusive lock on it, in which case we
can unlink the /tmp lock file before closing it. This is always a non-blocking
lock; there's no need to wait if we can't get it. If everything has gone right
but close fails, defer the message. Then unlink the lock file, if present. This
point in the code is jumped to from a number of places when errors are
detected, in order to get the file closed and the lock file tidied away. */

RETURN:

#ifdef SUPPORT_MBX
if (mbx_lockfd >= 0)
  {
  if (yield == OK)
    {
    flock_t lock_data;
    lock_data.l_type = F_WRLCK;
    lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock_data) >= 0)
      {
      DEBUG(9) debug_printf("unlinking MBX lock file %s\n", mbx_lockname);
      unlink(mbx_lockname);
      }
    }
  close(mbx_lockfd);
  }
#endif

if (fd >= 0 && close(fd) < 0 && yield == OK)
  {
  addr->basic_errno = errno;
  addr->message = string_sprintf("while closing %s", filename);
  addr->transport_return = DEFER;
  }

if (hd >= 0) unlink(lockname);

/* We get here with isdirectory set only in error situations. */

if (isdirectory && filename != NULL)
  {
  unlink(filename);
  if (dataname != filename) unlink(dataname);
  }

return FALSE;
}

/* End of transport/appendfile.c */
