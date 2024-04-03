/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* The main code for delivering a message. */


#include "exim.h"


/* Data block for keeping track of subprocesses for parallel remote
delivery. */

typedef struct pardata {
  address_item *addrlist;      /* chain of addresses */
  address_item *addr;          /* next address data expected for */
  pid_t pid;                   /* subprocess pid */
  int fd;                      /* pipe fd for getting result from subprocess */
  BOOL done;                   /* no more data needed */
  char *msg;                   /* error message */
} pardata;

/* Values for the process_recipients variable */

enum { RECIP_ACCEPT, RECIP_IGNORE, RECIP_DEFER,
       RECIP_FAIL, RECIP_FAIL_FILTER, RECIP_FAIL_TIMEOUT };



/*************************************************
*            Local static variables              *
*************************************************/

/* addr_duplicate is global because it needs to be seen from the Envelope-To
writing code. */

static address_item *addr_defer = NULL;
static address_item *addr_direct = NULL;
static address_item *addr_failed = NULL;
static address_item *addr_fallback = NULL;
static address_item *addr_local = NULL;
static address_item *addr_new = NULL;
static address_item *addr_remote = NULL;
static address_item *addr_route = NULL;
static address_item *addr_succeed = NULL;

static BOOL update_spool;
static BOOL remove_journal;
static int  parcount = 0;
static pardata *parlist = NULL;
static int  return_count;
static char *frozen_info = "";

#if defined PATH_MAX
static char spoolname[PATH_MAX];
#elif defined MAXPATHLEN
static char spoolname[MAXPATHLEN];
#else
static char spoolname[1024];
#endif


/* Table for turning base-62 numbers into binary */

static char tab62[] =
          {0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,     /* 0-9 */
           0,10,11,12,13,14,15,16,17,18,19,20,  /* A-K */
          21,22,23,24,25,26,27,28,29,30,31,32,  /* L-W */
          33,34,35, 0, 0, 0, 0, 0,              /* X-Z */
           0,36,37,38,39,40,41,42,43,44,45,46,  /* a-k */
          47,48,49,50,51,52,53,54,55,56,57,58,  /* l-w */
          59,60,61};                            /* x-z */

/* Default data for newly created address */

static address_item default_addr = {
  NULL,                 /* next */
  NULL,                 /* parent */
  NULL,                 /* shadow_parent */
  NULL,                 /* first */
  0,                    /* flags */
  NULL,                 /* onetime_parent */
  NULL,                 /* orig */
  NULL,                 /* unique */
  NULL,                 /* local_part */
  NULL,                 /* prefix */
  NULL,                 /* suffix */
  NULL,                 /* domain */
  NULL,                 /* route_domain */
  NULL,                 /* errors_address */
  NULL,                 /* self_hostname */
  NULL,                 /* extra_headers */
  NULL,                 /* remove_headers */
  NULL,                 /* domain_data */
  NULL,                 /* local_part_data */
  NULL,                 /* start_director */
  NULL,                 /* director */
  NULL,                 /* router */
  NULL,                 /* transport */
  NULL,                 /* host_list */
  NULL,                 /* transported */
  NULL,                 /* fallback_hosts */
  0,                    /* child_count */
  (uid_t)(-1),          /* uid */
  (gid_t)(-1),          /* gid */
  -1,                   /* mode */
  NULL,                 /* home_dir */
  NULL,                 /* current_dir */
  NULL,                 /* route_option */
  NULL,                 /* pipe_expandn */
  DEFER,                /* transport_return */
  ERRNO_UNKNOWNERROR,   /* basic_errno */
  0,                    /* more_errno */
  NULL,                 /* message */
  NULL,                 /* shadow_message */
  SPECIAL_NONE,         /* special_action */
  NULL,                 /* retries */
  NULL,                 /* local_retry_key */
  NULL,                 /* return_filename */
  -1,                   /* return_file */
  NULL                  /* reply */

  #ifdef SUPPORT_TLS
  ,NULL,                /* cipher */
  NULL                  /* peerdn */
  #endif

  /* These are the remains of an abandoned attempt to put in some
  DSN support. */

  #ifdef SUPPORT_DSN
  ,NULL,                /* dsn_orcpt */
  0                     /* dsn_flags */
  #endif
};



/*************************************************
*             Make a new address item            *
*************************************************/

/* This function gets the store and initializes with default values. The
transport_return value defaults to DEFER, so that any unexpected failure to
deliver does not wipe out the message.

Argument:
  address     the RFC822 address string
  copy        force a copy of the address

Returns:      a pointer to an initialized address_item
*/

address_item *
deliver_make_addr(char *address, BOOL copy)
{
address_item *addr = store_get(sizeof(address_item));
*addr = default_addr;
if (copy) address = string_copy(address);
addr->orig = address;
addr->unique = addr->orig;
return addr;
}




/*************************************************
*     Set expansion values for an address        *
*************************************************/

/* Certain expansion variables are valid only when handling an address.
This function sets them up or clears them, according to its argument.

Arguments:
  addr          the address in question, or NULL to clear values
Returns:        nothing
*/

void
deliver_set_expansions(address_item *addr)
{
address_item *addr_orig;

if (addr == NULL)
  {
  deliver_domain = NULL;
  deliver_domain_orig = NULL;
  deliver_domain_parent = NULL;     /* !!! Remember also the save/restore */
  deliver_localpart = NULL;         /* !!! function if you add or remove  */
  deliver_localpart_orig = NULL;    /* !!! any variables in this list     */
  deliver_localpart_parent = NULL;
  deliver_localpart_prefix = NULL;
  deliver_localpart_suffix = NULL;
  deliver_recipients = NULL;
  route_option = NULL;
  deliver_host = NULL;              /* get set individually */
  deliver_home = NULL;              /* but can be cleared here */
  address_file = NULL;
  address_pipe = NULL;
  self_hostname = NULL;
  return;
  }

/* Find the original address */

for (addr_orig = addr; addr_orig->parent != NULL;
  addr_orig = addr_orig->parent);

/* Things that are always set */

deliver_recipients = addr;
route_option = addr->route_option;
deliver_host = (addr->host_list == NULL)? "" : addr->host_list->name;

/* These may be unset for multiple addresses */

deliver_domain = addr->domain;
self_hostname = addr->self_hostname;

/* If only one address there is no complication */

if (addr->next == NULL)
  {
  deliver_localpart = addr->local_part;
  deliver_localpart_prefix = addr->prefix;
  deliver_localpart_suffix = addr->suffix;

  /* If there's a parent, make its domain and local part available, and if
  delivering to a pipe or file, or sending an autoreply, get the local
  part from the parent. For pipes and files, put the pipe or file string
  into address_pipe and address_file. */

  if (addr->parent != NULL)
    {
    deliver_domain_parent = addr->parent->domain;
    deliver_localpart_parent = addr->parent->local_part;
    if (testflag(addr, af_pfr))
      {
      if (deliver_localpart[0] == '|') address_pipe = addr->local_part;
        else if (deliver_localpart[0] == '/') address_file = addr->local_part;
      deliver_localpart = addr->parent->local_part;
      deliver_localpart_prefix = addr->parent->prefix;
      deliver_localpart_suffix = addr->parent->suffix;
      }
    }

  deliver_domain_orig = addr_orig->domain;
  deliver_localpart_orig = addr_orig->local_part;
  }

/* For multiple addresses, don't set local part, and leave the domain and
self_hostname set only if it is the same for all of them. */

else
  {
  address_item *addr2;
  for (addr2 = addr->next; addr2 != NULL; addr2 = addr2->next)
    {
    if (strcmp(deliver_domain, addr2->domain) != 0)
      {
      deliver_domain = NULL;
      break;
      }
    }
  if (self_hostname != NULL)
    {
    for (addr2 = addr->next; addr2 != NULL; addr2 = addr2->next)
      {
      if (addr2->self_hostname == NULL ||
          strcmp(self_hostname, addr2->self_hostname) != 0)
        {
        self_hostname = NULL;
        break;
        }
      }
    }
  }
}




/*************************************************
*  Save/restore expansion values for an address  *
*************************************************/

/* Expansion variables for an address have to saved and restored when
verifying another address (typically an error address) during directing
or routing. This is a nasty kludgy bit of code. I should have designed it
better than this.

Arguments:   none
Returns:     nothing
*/

static char *save_deliver_domain;
static char *save_deliver_domain_orig;
static char *save_deliver_domain_parent;
static char *save_deliver_localpart;
static char *save_deliver_localpart_orig;
static char *save_deliver_localpart_parent;
static char *save_deliver_localpart_prefix;
static char *save_deliver_localpart_suffix;
static address_item *save_deliver_recipients;
static char *save_route_option;
static char *save_deliver_host;
static char *save_deliver_home;
static char *save_address_file;
static char *save_address_pipe;
static char *save_self_hostname;


void
deliver_save_expansions(void)
{
save_deliver_domain =             deliver_domain;
save_deliver_domain_orig =        deliver_domain_orig;
save_deliver_domain_parent =      deliver_domain_parent;
save_deliver_localpart =          deliver_localpart;
save_deliver_localpart_orig =     deliver_localpart_orig;
save_deliver_localpart_parent =   deliver_localpart_parent;
save_deliver_localpart_prefix =   deliver_localpart_prefix;
save_deliver_localpart_suffix =   deliver_localpart_suffix;
save_deliver_recipients =         deliver_recipients;
save_route_option =               route_option;
save_deliver_host =               deliver_host;
save_deliver_home =               deliver_home;
save_address_file =               address_file;
save_address_pipe =               address_pipe;
save_self_hostname =              self_hostname;
}

void
deliver_restore_expansions(void)
{
deliver_domain =             save_deliver_domain;
deliver_domain_orig =        save_deliver_domain_orig;
deliver_domain_parent =      save_deliver_domain_parent;
deliver_localpart =          save_deliver_localpart;
deliver_localpart_orig =     save_deliver_localpart_orig;
deliver_localpart_parent =   save_deliver_localpart_parent;
deliver_localpart_prefix =   save_deliver_localpart_prefix;
deliver_localpart_suffix =   save_deliver_localpart_suffix;
deliver_recipients =         save_deliver_recipients;
route_option =               save_route_option;
deliver_host =               save_deliver_host;
deliver_home =               save_deliver_home;
address_file =               save_address_file;
address_pipe =               save_address_pipe;
self_hostname =              save_self_hostname;
}




/*************************************************
*            Replicate status for batch          *
*************************************************/

/* When a transport handles a batch of addresses, it may treat them
individually, or it may just put the status in the first one, and return FALSE,
requesting that the status be copied to all the others externally. This is the
replication function. As well as the status, it copies the transport pointer,
which may have changed if appendfile passed the addresses on to a different
transport.

Argument:    pointer to the first address in a chain
Returns:     nothing
*/

static void
replicate_status(address_item *addr)
{
address_item *addr2;
for (addr2 = addr->next; addr2 != NULL; addr2 = addr2->next)
  {
  addr2->transport = addr->transport;
  addr2->transport_return = addr->transport_return;
  addr2->basic_errno = addr->basic_errno;
  addr2->more_errno = addr->more_errno;
  addr2->special_action = addr->special_action;
  addr2->message = addr->message;
  }
}



/*************************************************
*              Compare lists of headers          *
*************************************************/

/* This function is given two pointers to chains of header items, and it yields
TRUE if the lists refer to the same headers in the same order.

Arguments:
  one       points to the first header list
  two       points to the second header list

Returns:    TRUE if the lists refer to the same header set
*/

static BOOL
same_headers(header_line *one, header_line *two)
{
for (;;)
  {
  if (one == NULL && two == NULL) return TRUE;
  if (one == NULL || two == NULL) return FALSE;
  if (strcmp(one->text, two->text) != 0) return FALSE;
  one = one->next;
  two = two->next;
  }
}



/*************************************************
*      Decrease counts in parents and mark done  *
*************************************************/

/* This function is called when an address is complete. The chain of parents is
scanned, and the count of children of each parent is decremented. If it becomes
zero for any parent, that address is added to the non-recipients tree because
it is complete. We must also scan the duplicates address list for such an
address and mark any duplicates as complete as well by recursive call to this
function.

Arguments:
  addr      points to the completed address item
  now       the current time as a string, for writing to the message log

Returns:    a pointer to the address item for the original ancestor.
*/

static address_item *
child_done(address_item *addr, char *now)
{
int decrement = 1;
while (addr->parent != NULL)
  {
  addr = addr->parent;
  if ((addr->child_count -= decrement) <= 0)
    {
    address_item *aa;

    /* Don't deliver to this address again. For local addresses, the unique
    field contains the lower cased form of the local part. However, we do need
    also to have the original form of the address updated in the spool file so
    that listings of the spool mark the address as delivered. */

    tree_add_nonrecipient(addr->unique, testflag(addr, af_pfr));
    if (testflag(addr, af_local) && addr->parent == NULL)
      tree_add_nonrecipient(addr->orig, testflag(addr, af_pfr));

    /* Log the completion of descendents only when there is no ancestor with
    the same original address. */

    for (aa = addr->parent; aa != NULL; aa = aa->parent)
      if (strcmp(aa->orig, addr->orig) == 0) break;

    if (aa == NULL)
      {
      fprintf(message_log, "%s %s: children all complete\n", now, addr->orig);
      fflush(message_log);
      DEBUG(9) debug_printf("%s: children all complete\n", addr->orig);
      }

    /* Check for duplicates, and their parents. */

    for (aa = addr_duplicate; aa != NULL; aa = aa->next)
      {
      if (strcmp(addr->unique, aa->unique) == 0)
        {
        tree_add_nonrecipient(aa->orig, testflag(aa, af_pfr));
        (void)child_done(aa, now);
        }
      }

    /* Ensure the spool file gets rewritten */

    update_spool = TRUE;
    }
  else decrement = 0;
  }
return addr;
}




/*************************************************
*    Actions at the end of handling an address   *
*************************************************/

/* This is a function for processing a single address when all that can be done
with it has been done.

Arguments:
  addr         points to the address block
  result       the result of the delivery attempt
  logflags     flags for log_write() (LOG_MAIN and/or LOG_PANIC)
  driver_type  indicates which type of driver (transport, director, or
               router) was last to process the address
  logchar      '=' or '-' for use when logging deliveries with => or ->

Returns:       nothing
*/

static void
post_process_one(address_item *addr, int result, int logflags, int driver_type,
  int logchar)
{
char *now = tod_stamp(tod_log);
char *driver_kind = NULL;
char *driver_name = NULL;

int size = 256;         /* Used for a temporary, */
int ptr = 0;            /* expanding buffer, for */
char *s;                /* building log lines;   */
void *reset_point;      /* released afterwards.  */


DEBUG(9) debug_printf("post-process %s (%d)\n", addr->orig, result);

/* Set up driver kind and name for logging */

if (driver_type == DTYPE_TRANSPORT)
  {
  if (addr->transport != NULL)
    {
    driver_name = addr->transport->name;
    driver_kind = " transport";
    }
  else driver_kind = "transporting";
  }
else if (driver_type == DTYPE_DIRECTOR)
  {
  if (addr->director != NULL)
    {
    driver_name = addr->director->name;
    driver_kind = " director";
    }
  else driver_kind = "directing";
  }
else if (driver_type == DTYPE_ROUTER)
  {
  if (addr->router != NULL)
    {
    driver_name = addr->router->name;
    driver_kind = " router";
    }
  else driver_kind = "routing";
  }

/* If there's an error message set, ensure that it contains only printing
characters - it should, but occasionally things slip in and this at least
stops the log format from getting wrecked. */

if (addr->message != NULL) addr->message = string_printing(addr->message);

/* If we used a transport that has one of the "return_output" options set, and
if it did in fact generate some output, then for return_output we treat the
message as failed if it was not already set that way, so that the output gets
returned to the sender, provided there is a sender to send it to. For
return_fail_output, do this only if the delivery failed. Otherwise we just
unlink the file, and remove the name so that if the delivery failed, we don't
try to send back an empty or unwanted file. The log_output options operate only
on a non-empty file.

In any case, we close the message file, because we cannot afford to leave a
file-descriptor for one address while processing (maybe very many) others. */

if (addr->return_file >= 0 && addr->return_filename != NULL)
  {
  BOOL return_output = FALSE;
  struct stat statbuf;
  fsync(addr->return_file);

  /* If there is no output, do nothing. */

  if (fstat(addr->return_file, &statbuf) == 0 && statbuf.st_size > 0)
    {
    transport_instance *tb = addr->transport;

    /* Handle logging options */

    if (tb->log_output || (result == FAIL && tb->log_fail_output) ||
                          (result == DEFER && tb->log_defer_output))
      {
      char *s;
      FILE *f = fopen(addr->return_filename, "r");
      if (f == NULL)
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to open %s to log output "
          "from %s transport: %s", addr->return_filename, tb->name,
          strerror(errno));
      else
        {
        s = fgets(big_buffer, big_buffer_size, f);
        if (s != NULL)
          {
          char *p = big_buffer + (int)strlen(big_buffer);
          while (p > big_buffer && isspace((uschar)p[-1])) p--;
          *p = 0;
          s = string_printing(big_buffer);
          log_write(0, LOG_MAIN, "<%s>: %s transport output: %s",
            addr->orig, tb->name, s);
          }
        fclose(f);
        }
      }

    /* Handle returning options, but only if there is an address to return
    the text to. */

    if (sender_address[0] != 0 || addr->errors_address != NULL)
      {
      if (tb->return_output)
        {
        addr->transport_return = result = FAIL;
        if (addr->basic_errno == 0 && addr->message == NULL)
          addr->message = "return message generated";
        return_output = TRUE;
        }
      else
        if (tb->return_fail_output && result == FAIL) return_output = TRUE;
      }
    }

  /* Get rid of the file unless it might be returned, but close it in
  all cases. */

  if (!return_output)
    {
    unlink(addr->return_filename);
    addr->return_filename = NULL;
    addr->return_file = -1;
    }

  close(addr->return_file);
  }

/* The sucess case happens only after delivery by a transport. */

if (result == OK)
  {
  address_item *topaddr, *dup;
  addr->next = addr_succeed;
  addr_succeed = addr;

  /* Don't deliver to this address again. For local addresses, the unique
  field contains the lower cased form of the local part. However, we do need
  also to have the original form of the address updated in the spool file so
  that listings of the spool mark the address as delivered. */

  DEBUG(9) debug_printf("%s succeeded: adding to nonrecipients list\n",
    addr->orig);

  tree_add_nonrecipient(addr->unique, testflag(addr, af_pfr));
  if (testflag(addr, af_local) && addr->parent == NULL)
    tree_add_nonrecipient(addr->orig, testflag(addr, af_pfr));

  /* Check the list of duplicate addresses and ensure they are now marked
  done as well. Also their parents. */

  for (dup = addr_duplicate; dup != NULL; dup = dup->next)
    {
    if (strcmp(addr->unique, dup->unique) == 0)
      {
      tree_add_nonrecipient(dup->orig, testflag(dup, af_pfr));
      (void)child_done(dup, now);
      }
    }

  /* Ensure the header file gets re-written */

  update_spool = TRUE;

  /* Update the message log */

  if (addr->parent == NULL)
    fprintf(message_log, "%s %s: %s%s succeeded\n", now, addr->orig,
      driver_name, driver_kind);
  else
    fprintf(message_log, "%s %s <%s>: %s%s succeeded\n", now, addr->orig,
      addr->parent->orig, driver_name, driver_kind);
  fflush(message_log);

  /* Crawl back up the parents chain, decreasing the counts, and handling
  any that are complete. */

  topaddr = child_done(addr, now);

  /* Log the delivery. We use an extensible string to build up the log line,
  and reset the store afterwards.

  Both local and remote deliveries can be set up by either directors or
  routers. Remote delivers should always have a pointer to the host item that
  succeeded; local deliveries can have a pointer to a single host item in their
  host list, for use by the transport. A separate function builds a string
  containing the address and an appropriate list of parents. */

  s = reset_point = store_get(size);
  s[ptr++] = logchar;

  s = mac_cat2(s, &size, &ptr, "> ",
    string_log_address(addr, topaddr,
      (addr->transport->info->local && addr->router == NULL),
      log_all_parents));

  if (log_sender_on_delivery)
    {
    s = mac_cat2(s, &size, &ptr, " F=<", sender_address);
    s = string_cat(s, &size, &ptr, ">", 1);
    }

  if (addr->director != NULL)
    s = mac_cat2(s, &size, &ptr, " D=", addr->director->name);

  if (addr->router != NULL)
    s = mac_cat2(s, &size, &ptr, " R=", addr->router->name);

  s = mac_cat2(s, &size, &ptr, " T=", addr->transport->name);

  /* Local delivery */

  if (addr->transport->info->local)
    {
    if (addr->host_list != NULL)
      s = mac_cat2(s, &size, &ptr, " H=", addr->host_list->name);
    if (addr->shadow_message != NULL)
      s = string_cat(s, &size, &ptr, addr->shadow_message,
        (int)strlen(addr->shadow_message));
    }

  /* Remote delivery */

  else
    {
    if (addr->transported != NULL)
      {
      s = mac_cat2(s, &size, &ptr, " H=", addr->transported->name);
      s = mac_cat2(s, &size, &ptr, " [", addr->transported->address);
      s = string_cat(s, &size, &ptr, "]", 1);
      if (continue_sequence > 1)
        s = string_cat(s, &size, &ptr, "*", 1);
      }

    #ifdef SUPPORT_TLS
    if (tls_log_cipher && addr->cipher != NULL)
      s = mac_cat2(s, &size, &ptr, " X=", addr->cipher);
    if (tls_log_peerdn && addr->peerdn != NULL)
      {
      s = mac_cat2(s, &size, &ptr, " DN=\"", addr->peerdn);
      s = string_cat(s, &size, &ptr, "\"", 1);
      }
    #endif

    if (log_smtp_confirmation && addr->message != NULL)
      {
      int i;
      char *p = big_buffer;
      char *ss = addr->message;
      *p++ = '\"';
      for (i = 0; i < 100 && ss[i] != 0; i++)
        {
        if (ss[i] == '\"' || ss[i] == '\\') *p++ = '\\';
        *p++ = ss[i];
        }
      *p++ = '\"';
      *p = 0;
      s = mac_cat2(s, &size, &ptr, " C=", big_buffer);
      }
    }

  /* string_cat() always leaves room for the terminator. Release the
  store we used to build the line after writing it. */

  s[ptr] = 0;
  log_write(0, LOG_MAIN, "%s", s);
  store_reset(reset_point);
  return;
  }


/* Soft failure, or local delivery process failed; freezing may be
requested. */

else if (result == DEFER || result == PANIC || result == ERROR)
  {
  if (result == PANIC) logflags |= LOG_PANIC;

  /* This puts them on the chain in reverse order. Do not change this, because
  the code for handling retries assumes that the one with the retry
  information is last. */

  addr->next = addr_defer;
  addr_defer = addr;

  /* The only currently implemented special action is to freeze the
  message. Logging of this is done later, just before the -H file is
  updated. */

  if (addr->special_action == SPECIAL_FREEZE)
    {
    deliver_freeze = TRUE;
    deliver_frozen_at = time(NULL);
    update_spool = TRUE;
    }

  /* If doing a 2-stage queue run, we skip writing to either the message
  log or the main log for SMTP defers. */

  if (!queue_2stage || addr->basic_errno != 0)
    {
    char ss[32];

    /* For errors of the type "retry time not reached" (also remotes skipped
    on queue run), require a log level of 5. Note that this kind of error
    number is negative, and all the retry ones are less than any others. */

    int use_log_level = (addr->basic_errno <= ERRNO_RETRY_BASE)? 5 : 0;

    /* Build up the line that is used for both the message log and the main
    log. */

    s = reset_point = store_get(size);
    s = string_cat(s, &size, &ptr, addr->orig, (int)strlen(addr->orig));

    if (addr->parent != NULL)
      {
      s = mac_cat2(s, &size, &ptr, " <", addr->parent->orig);
      s = string_cat(s, &size, &ptr, ">", 1);
      }

    /* Either driver_name contains something and driver_kind contains
    " director" or whatever (note the leading space), or driver_name is
    a null string and driver_kind contains "directing" etc, without the
    leading space, if all directing or routing has been deferred. When
    a domain has been held, so nothing has been done at all, both variables
    contain null strings. */

    if (driver_name == NULL)
      {
      if (driver_kind != NULL)
        s = mac_cat2(s, &size, &ptr, " ", driver_kind);
      }
     else
      {
      strcpy(ss, " ?=");
      ss[1] = toupper(driver_kind[1]);
      s = mac_cat2(s, &size, &ptr, ss, driver_name);
      }

    sprintf(ss, " defer (%d)", addr->basic_errno);
    s = string_cat(s, &size, &ptr, ss, (int)strlen(ss));

    if (addr->basic_errno > 0)
      s = mac_cat2(s, &size, &ptr, ": ", strerror(addr->basic_errno));

    if (addr->message != NULL)
      s = mac_cat2(s, &size, &ptr, ": ", addr->message);

    s[ptr] = 0;

    /* Log the deferment in the message log, but don't clutter it
    up with retry-time defers after the first delivery attempt if
    the log level is too low */

    if (deliver_firsttime || log_level >= use_log_level)
      {
      fprintf(message_log, "%s %s\n", now, s);
      fflush(message_log);
      }

    /* Write the main log and reset the store */

    log_write(use_log_level, logflags, "== %s", s);
    store_reset(reset_point);
    }
  return;
  }


/* Hard failure. If there is an address to which an error message can be sent,
put this address on the failed list. If not, put it on the deferred list and
freeze the mail message for human attention. The latter action can also be
explicitly requested by a router/director/transport. */

else
  {
  /* If this is a delivery error, or a message for which no replies are
  wanted, and ignore_errmsg_errors is set, force the af_ignore_error flag.
  This will cause the address to be discarded later (with a log entry). */

  if (sender_address[0] == 0 && ignore_errmsg_errors)
    setflag(addr, af_ignore_error);

  /* Freeze the message if requested, or if this is a delivery
  error message or a message for which no replies are ever wanted.
  Exim used to test

    (sender_address[0] == 0 && addr->errors_address == NULL)

  but this can lead to meltdown when delivery to the errors_address
  is failing. Don't freeze if errors are being ignored - the actual
  code to ignore occurs later, instead of sending a message. Logging
  of freezing occurs later, just before writing the -H file. */

  if (!testflag(addr, af_ignore_error) &&
      (addr->special_action == SPECIAL_FREEZE || sender_address[0] == 0))
    {
    frozen_info = (addr->special_action == SPECIAL_FREEZE)? "" :
      (sender_local && !local_error_message)?
        " (message created with -f <>)" : " (delivery error message)";
    deliver_freeze = TRUE;
    deliver_frozen_at = time(NULL);
    update_spool = TRUE;

    /* The address is put on the defer rather than the failed queue, because
    the message is being retained. */

    addr->next = addr_defer;
    addr_defer = addr;
    }

  /* Don't put the address on the nonrecipients tree yet; wait until an
  error message has been successfully sent. */

  else
    {
    addr->next = addr_failed;
    addr_failed = addr;
    }

  /* Build up the log line for the message and main logs */

  s = reset_point = store_get(size);

  s = string_cat(s, &size, &ptr, addr->orig, (int)strlen(addr->orig));

  if (addr->parent != NULL)
    {
    s = mac_cat2(s, &size, &ptr, " <", addr->parent->orig);
    s = string_cat(s, &size, &ptr, ">", 1);
    }

  if (log_sender_on_delivery)
    {
    s = mac_cat2(s, &size, &ptr, " F=<", sender_address);
    s = string_cat(s, &size, &ptr, ">", 1);
    }

  if (addr->director != NULL)
    s = mac_cat2(s, &size, &ptr, " D=", addr->director->name);
  if (addr->router != NULL)
    s = mac_cat2(s, &size, &ptr, " R=", addr->router->name);
  if (addr->transport != NULL)
    s = mac_cat2(s, &size, &ptr, " T=", addr->transport->name);

  if (addr->transported != NULL)
    {
    s = mac_cat2(s, &size, &ptr, " H=", addr->transported->name);
    s = mac_cat2(s, &size, &ptr, " [", addr->transported->address);
    s = string_cat(s, &size, &ptr, "]", 1);
    }

  if (addr->basic_errno > 0)
    s = mac_cat2(s, &size, &ptr, ": ", strerror(addr->basic_errno));

  if (addr->message != NULL)
    s = mac_cat2(s, &size, &ptr, ": ", addr->message);

  s[ptr] = 0;

  /* Do the logging. For the message log, add "directing failed" or
  "routing failed" for those cases, just to make it clearer. */

  if (driver_name == NULL)
    fprintf(message_log, "%s %s failed for %s\n", now, driver_kind, s);
  else
    fprintf(message_log, "%s %s\n", now, s);
  fflush(message_log);

  log_write(0, LOG_MAIN, "** %s", s);
  store_reset(reset_point);
  }
}




/*************************************************
*           Perform a local delivery             *
*************************************************/

/* Each local delivery is performed in a separate process which sets its
uid and gid as specified. This is a safer way than simply changing and
restoring using seteuid; there is a body of opinion that seteuid cannot be
used safely. Besides, not all systems have seteuid.

If the security level is 1 or 3 this function will be running sete{g,u}id to
the exim uid and gid, and must use sete{g,u}id to regain root privilege before
using set{g,u}id. Otherwise it is running as root.

If the uid/gid are specified in the transport_instance, they are used; the
transport initialization must ensure that either both or neither are set.
Otherwise, the values associated with the address are used. If neither are set,
it is a configuration error.

The transport or the address may specify a home directory (transport over-
rides), and if they do, this is set as $home and as the working directory.
Otherwise $home is left unset and the cwd is set to "/" - a directory that
should be accessible to all users.

Using a separate process makes it more complicated to get error information
back. We use a pipe to pass the return code and also an error code and error
text string back to the parent process.

Arguments:
  addr       points to an address block for this delivery; for "normal" local
             deliveries this is the only address to be delivered, but for
             pseudo-remote deliveries (e.g. by batch SMTP to a file or pipe)
             a number of addresses can be handled simultaneously, and in this
             case addr will point to a chain of addresses with the same
             characteristics.

  shadowing  TRUE if running a shadow transport; this causes output from pipes
             to be ignored.

Returns:     nothing
*/

static void
deliver_local(address_item *addr, BOOL shadowing)
{
BOOL gid_set = FALSE;
BOOL use_initgroups;
uid_t uid;
gid_t gid;
int status, len, rc;
int pfd[2];
pid_t pid;
char *working_directory;
address_item *addr2;
transport_instance *tp = addr->transport;

/* If transport not set or not local, panic */

if (tp == NULL)
  log_write(0, LOG_PANIC_DIE, "No transport set for local delivery of %s",
    addr->orig);
if (!tp->info->local)
  log_write(0, LOG_PANIC_DIE, "Attempt local delivery with non-local "
  "transport for %s: transport %s", addr->orig, tp->name);

/* Set up variables that are relevant to a single delivery, for use
in various expansions and the environment of scripts. Need to set them as
early as this in case an expansion is required to get a uid or gid or home.
Domain_data and local_part_data are not in the common function, because it is
used during directing and routing at points where we don't want to make changes
to their values. */

deliver_set_expansions(addr);
domain_data = addr->domain_data;
local_part_data = addr->local_part_data;

/* Set up the return path from the errors or sender address. If the transport
has its own return path setting, expand it and replace the existing value. */

return_path = (addr->errors_address != NULL)?
  addr->errors_address : sender_address;

if (tp->return_path != NULL)
  {
  char *new_return_path = expand_string(tp->return_path);
  if (new_return_path == NULL)
    {
    if (!expand_string_forcedfail)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Failed to expand return path "
        "\"%s\" in %s transport: %s", tp->return_path, tp->name,
        expand_string_message);
    }
  else return_path = new_return_path;
  }

/* Get the uid and gid to use. First see if the transport either set explicit
values, or set the deliver_as_creator flag. It should not be possible for both
to be set. Take the use_initgroups flag from the transport if using uid/gid
from there, otherwise from the address. */

use_initgroups = tp->initgroups;

/* First see if there's a gid on the transport. */

if (tp->gid_set)
  {
  gid = tp->gid;
  gid_set = TRUE;
  }
else if (tp->expand_gid != NULL)
  {
  if (direct_find_expanded_group(tp->expand_gid, tp->name, "transport", &gid,
    &(addr->message))) gid_set = TRUE;
  else
    {
    addr->basic_errno = ERRNO_GIDFAIL;
    goto END_DELIVER_LOCAL;
    }
  }

/* Pick up a uid from the transport if one is set. */

if (tp->uid_set) uid = tp->uid;

/* Otherwise, try for an expandable uid field. If it ends up as a numeric id,
it does not provide a passwd value from which a gid can be taken. */

else if (tp->expand_uid != NULL)
  {
  struct passwd *pw;
  if (!direct_find_expanded_user(tp->expand_uid, tp->name, "transport", &pw,
       &uid, &(addr->message)))
    {
    addr->basic_errno = ERRNO_UIDFAIL;
    goto END_DELIVER_LOCAL;
    }

  /* If no gid yet set, and it was a symbolic id, get the corresponding gid */

  if (!gid_set && pw != NULL)
    {
    gid = pw->pw_gid;
    gid_set = TRUE;
    }
  }

/* Otherwise, test the deliver_creator flag. */

else if (tp->deliver_as_creator)
  {
  uid = originator_uid;
  if (!gid_set) gid = originator_gid;
  gid_set = TRUE;
  }

/* If nothing else has specified the uid, then the address must, and this can
be done in either a director or a router. */

else
  {
  if (!testflag(addr, af_uid_set))
    {
    char *type, *name;

    if (addr->director != NULL)
      {
      type = "director";
      name = addr->director->name;
      }
    else
      {
      type = "router";
      name = addr->router->name;
      }

    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Neither the %s %s nor the %s "
      "transport set a uid for local delivery of %s", name, type,
      tp->name, addr->orig);
    }

  uid = addr->uid;
  if (!gid_set)
    {
    gid = addr->gid;
    gid_set = TRUE;
    }
  use_initgroups = testflag(addr, af_initgroups);
  }

/* Control gets here only if a uid has been found. If no gid has been set by
this time, it is a disaster. */

if (!gid_set)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "User set without group for %s transport",
    tp->name);

/* See if the uid is on the list of banned uids for local delivery, and if
so, substitute the nobody user if configured. If there is no nobody, default to
"nobody", and if that isn't available, give up. */

if (never_users != NULL)
  {
  int i;
  uid_t *nn = never_users;
  for (i = 1; i <= (int)(nn[0]); i++) if (nn[i] == uid)
    {
    if (!nobody_uid_set)
      {
      struct passwd *pw;
      if (direct_finduser("nobody", &pw, NULL))
        {
        nobody_uid = pw->pw_uid;
        nobody_gid = pw->pw_gid;
        nobody_uid_set = nobody_gid_set = TRUE;
        }
      }
    if (nobody_uid_set)
      {
      uid = nobody_uid;
      gid = nobody_gid;
      }
    else
      log_write(0, LOG_PANIC_DIE, "Uid %d is not permitted to deliver, but "
        "there is no \"nobody\" user specified", (int)uid);
    }
  }

/* See if either the transport or the address specifies a home (current)
directory. Expand it if necessary. If nothing is set, use "/", which is
assumed to be a directory to which all users have access. It is necessary
to be in a visible directory for some operating systems when running pipes,
as some commands (e.g. "rm" under Solaris 2.5) require this. */

deliver_home = (tp->home_dir != NULL)? tp->home_dir :
               (addr->home_dir != NULL)? addr->home_dir : NULL;

if (deliver_home != NULL)
  {
  char *rawhome = deliver_home;
  deliver_home = NULL;                      /* in case it contains $home */
  deliver_home = expand_string(rawhome);
  if (deliver_home == NULL)
    {
    addr->basic_errno = ERRNO_EXPANDFAIL;
    addr->message = string_sprintf("home directory \"%s\" failed to expand "
      "for %s director: %s", rawhome, tp->name, expand_string_message);
    log_write(0, LOG_MAIN|LOG_PANIC, "%s", addr->message);
    goto END_DELIVER_LOCAL;
    }
  if (*deliver_home != '/')
    {
    addr->basic_errno = ERRNO_NOTABSOLUTE;
    addr->message = string_sprintf("home directory path \"%s\" is not "
      "absolute for %s transport", deliver_home, tp->name);
    log_write(0, LOG_MAIN|LOG_PANIC, "%s", addr->message);
    }
  }

/* If an explicit current directory was set by the address or transport,
expand and use it; otherwise default to the home directory, if any; otherwise
default to "/". */

working_directory =
  (tp->current_dir != NULL)? tp->current_dir :
  (addr->current_dir != NULL)? addr->current_dir : NULL;

if (working_directory != NULL)
  {
  char *raw = working_directory;
  working_directory = expand_string(raw);
  if (working_directory == NULL)
    {
    addr->basic_errno = ERRNO_EXPANDFAIL;
    addr->message = string_sprintf("current directory \"%s\" failed to expand "
      "for %s director: %s", raw, tp->name, expand_string_message);
    log_write(0, LOG_MAIN|LOG_PANIC, "%s", addr->message);
    goto END_DELIVER_LOCAL;
    }
  if (*working_directory != '/')
    {
    addr->basic_errno = ERRNO_NOTABSOLUTE;
    addr->message = string_sprintf("current directory path \"%s\" is not "
      "absolute for %s transport", working_directory, tp->name);
    log_write(0, LOG_MAIN|LOG_PANIC, "%s", addr->message);
    }
  }
else working_directory = (deliver_home == NULL)? "/" : deliver_home;

/* If one of the return_output flags is set on the transport, create and open a
file in the message log directory for the transport to write its output onto.
This is mainly used by pipe transports. The file needs to be unique to the
address. This feature is not available for shadow transports. */

if (!shadowing && (tp->return_output || tp->return_fail_output ||
    tp->log_output || tp->log_fail_output))
  {
  addr->return_filename =
    string_sprintf("%s/msglog/%s/%s-%d-%d", spool_directory, message_subdir,
      message_id, getpid(), return_count++);
  addr->return_file =
    open(addr->return_filename, O_WRONLY|O_CREAT|O_APPEND, 0400);
  if (addr->return_file < 0)
    log_write(0, LOG_PANIC_DIE, "Unable to create file for %s transport to "
      "return message: %s", tp->name, strerror(errno));

  /* Set the close-on-exec flag */

  fcntl(addr->return_file, F_SETFD, fcntl(addr->return_file, F_GETFD) |
   FD_CLOEXEC);

  /* If there is an exim uid and we are currently root, the file must be
  changed so that exim is the owner, because that will be the uid when it
  comes to be read. This happens when security = setuid (i.e. seteuid not
  being used). */

  if (exim_uid_set && geteuid() == root_uid &&
    chown(addr->return_filename, exim_uid, exim_gid) < 0)
      log_write(0, LOG_PANIC_DIE, "Unable to chmod return message file for %s "
        "transport: %s", tp->name, strerror(errno));
  }

/* Create the pipe for inter-process communication. */

if (pipe(pfd) != 0)
  log_write(0, LOG_PANIC_DIE, "Creation of pipe failed: %s", strerror(errno));

/* Now fork the process to do the real work in the sub-process. */

if ((pid = fork()) == 0)
  {
  BOOL replicate = TRUE;
  uid_t euid = geteuid();
  gid_t egid = getegid();

  /* Prevent core dumps, as we don't want them in users' home directories.
  HP-UX doesn't have RLIMIT_CORE; I don't know how to do this in that
  system. Some experimental/developing systems (e.g. GNU/Hurd) may define
  RLIMIT_CORE but not support it in setrlimit(). For such systems, do not
  complain if the error is "not supported". */

  #ifdef RLIMIT_CORE
  struct rlimit rl;
  rl.rlim_cur = 0;
  rl.rlim_max = 0;
  if (setrlimit(RLIMIT_CORE, &rl) < 0)
    {
    #ifdef SETRLIMIT_NOT_SUPPORTED
    if (errno != ENOSYS && errno != ENOTSUP)
    #endif
      log_write(0, LOG_MAIN|LOG_PANIC, "setrlimit failed: %s", strerror(errno));
    }
  #endif

  /* Reset the random number generator, so different processes don't all
  have the same sequence. */

  random_seed = 0;

  /* If the transport has a setup entry, call this first, while still
  privileged. (Appendfile uses this to expand quota, for example, while
  able to read private files.) */

  if (addr->transport->setup != NULL)
    {
    switch((addr->transport->setup)(addr->transport, addr, &(addr->message)))
      {
      case DEFER:
      addr->transport_return = DEFER;
      goto PASS_BACK;

      case FAIL:
      addr->transport_return = PANIC;
      goto PASS_BACK;
      }
    }

  /* Ignore SIGINT and SIGTERM during delivery. Also ignore SIGUSR1, as
  when the process becomes unprivileged, it won't be able to write to the
  process log. SIGHUP is ignored throughout exim, except when it is being
  run as a daemon. */

  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGUSR1, SIG_IGN);

  /* Close the unwanted half of the pipe, and set close-on-exec for the other
  half - for transports that exec things (e.g. pipe). Then set the required
  gid/uid, first regaining root privilege if necessary. Note that
  mac_sete{g,u}id expands to a failure on systems without the sete{g,u}id
  functions, but on such systems we should always be running as root here.
  For the support of non-setuid-root installations (there are some live ones,
  and this can also arise in testing), don't attempt to regain root privilege
  if it isn't available and if we are already running as the correct persona
  and don't need to initialize supplementary groups. */

  close(pfd[pipe_read]);
  fcntl(pfd[pipe_write], F_SETFD, fcntl(pfd[pipe_write], F_GETFD) |
    FD_CLOEXEC);

  if (getuid() == root_uid || euid != uid || egid != gid || use_initgroups)
    {
    if (euid != root_uid && mac_seteuid(root_uid) < 0)
      {
      log_write(0, LOG_MAIN|LOG_PANIC_DIE,
        "Unable to get root to set uid %d and gid %d for local delivery to %s: "
        "uid=%d euid=%d", uid, gid, addr->local_part, getuid(), euid);
      }

    /* Ignore initgroups if unable to get a passwd entry for the uid, or if
    initgroups fails. At least one OS returns +1 for initgroups failure, so
    just check for non-zero. */

    if (use_initgroups)
      {
      struct passwd *pw = getpwuid(uid);
      if (pw != NULL)
        {
        if (initgroups(pw->pw_name, gid) != 0)
          DEBUG(2) debug_printf("initgroups failed for uid=%ld: %s\n",
            (long int)uid, strerror(errno));
        }
      else DEBUG(2) debug_printf("no passwd entry for uid=%ld: initgroups "
        "ignored\n", (long int)uid);
      }

    /* Set the required uid and gid */

    exim_setugid(uid, gid, string_sprintf("local delivery to %s",
      addr->local_part));
    }

  /* If debugging, show the uid, gid, and additional groups if any. */

  DEBUG(2)
    {
    address_item *batched;
    int group_count;
    gid_t group_list[NGROUPS_MAX];
    debug_printf("delivering %s as %s using %s:\n  "
      "uid=%d gid=%d home=%s current=%s\n",
      addr->orig, addr->local_part, tp->name, (int)geteuid(), (int)getegid(),
      deliver_home, working_directory);
    group_count = getgroups(NGROUPS_MAX, group_list);
    debug_printf("  auxiliary group list:");
    if (group_count > 0)
      {
      int i;
      for (i = 0; i < group_count; i++) debug_printf(" %d", group_list[i]);
      }
    else debug_printf(" <none>");
    debug_printf("\n");
    for (batched = addr->next; batched != NULL; batched = batched->next)
      debug_printf("additional batched address: %s\n", batched->orig);
    }

  /* Set an appropriate working directory. */

  if (chdir(working_directory) < 0)
    {
    addr->transport_return = DEFER;
    addr->basic_errno = errno;
    addr->message = string_sprintf("failed to chdir to %s", working_directory);
    }

  /* If successful, call the transport */

  else
    {
    BOOL ok = TRUE;
    set_process_info("delivering %s to %s using %s", message_id,
     addr->local_part, addr->transport->name);

    /* If a transport filter has been specified, set up its argument list.
    Any errors will get put into the address, and FALSE yielded. */

    if (addr->transport->filter_command != NULL)
      {
      ok = transport_set_up_command(&transport_filter_argv,
        addr->transport->filter_command,
        TRUE, PANIC, addr, "transport filter", NULL);
      }
    else transport_filter_argv = NULL;

    if (ok)
      {
      debug_print_string(addr->transport->debug_string);
      transport_count = 0;
      replicate = !(addr->transport->info->code)(addr->transport, addr);
      }
    }

  /* Pass the results back down the pipe. If necessary, first replicate the
  status in the top address to the others in the batch. The label is the
  subject of a goto when a call to the transport's setup function fails. We
  pass the pointer to the transport back in case it got changed as a result of
  file_format in appendfile. */

  PASS_BACK:

  if (replicate) replicate_status(addr);
  for (addr2 = addr; addr2 != NULL; addr2 = addr2->next)
    {
    int message_length =
      (addr2->message == NULL)? 0 : (int)strlen(addr2->message) + 1;
    write(pfd[pipe_write], (void *)&(addr2->transport_return), sizeof(int));
    write(pfd[pipe_write], (void *)&(addr2->basic_errno), sizeof(int));
    write(pfd[pipe_write], (void *)&(addr2->more_errno), sizeof(int));
    write(pfd[pipe_write], (void *)&(addr2->special_action), sizeof(int));
    write(pfd[pipe_write], (void *)&(addr2->transport),
      sizeof(transport_instance *));
    write(pfd[pipe_write], (void *)&message_length, sizeof(int));
    if (message_length > 0)
      write(pfd[pipe_write], addr2->message, message_length);
    }

  /* OK, this process is now done */

  close(pfd[pipe_write]);
  search_tidyup();
  exit(EXIT_SUCCESS);
  }

/* Back in the main process: panic if the fork did not succeed. */

if (pid <= 0)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Fork failed for local delivery to %s",
    addr->orig);

/* Read the pipe to get the delivery status codes and error messages. Our copy
of the writing end must be closed first, as otherwise read() won't return zero
on an empty pipe. We check that a status exists for each address before
overwriting the address structure. If data is missing, the default DEFER status
will remain. Afterwards, close the reading end. */

close(pfd[pipe_write]);

for (addr2 = addr; addr2 != NULL; addr2 = addr2->next)
  {
  int message_length = 0;
  len = read(pfd[pipe_read], (void *)&status, sizeof(int));
  if (len > 0)
    {
    addr2->transport_return = status;
    len = read(pfd[pipe_read], (void *)&(addr2->basic_errno), sizeof(int));
    len = read(pfd[pipe_read], (void *)&(addr2->more_errno), sizeof(int));
    len = read(pfd[pipe_read], (void *)&(addr2->special_action), sizeof(int));
    len = read(pfd[pipe_read], (void *)&(addr2->transport),
      sizeof(transport_instance *));
    len = read(pfd[pipe_read], (void *)&message_length, sizeof(int));
    if (message_length > 0)
      {
      len = read(pfd[pipe_read], (void *)big_buffer, message_length);
      if (len > 0) addr2->message = string_copy(big_buffer);
      }
    }
  else
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "failed to read delivery status for %s "
      "from delivery subprocess", addr2->unique);
    break;
    }
  }

close(pfd[pipe_read]);

/* Unless shadowing, write all successful addresses immediately to the journal
file, to ensure they are recorded asap. For local addresses, the unique field
contains the lower cased form of the local part (when locally_caseless is set -
the usual case), and this is the main thing not to deliver to again. Failure to
write the journal is panic-worthy, but don't stop, as it may prove possible
subsequently to update the spool file in order to record the delivery. */

if (!shadowing)
  {
  for (addr2 = addr; addr2 != NULL; addr2 = addr2->next)
    {
    if (addr2->transport_return != OK) continue;
    sprintf(big_buffer, "%c%.500s\n", testflag(addr, af_pfr)? 'Y':'N',
      addr2->unique);
    len = (int)strlen(big_buffer);
    if (write(journal_fd, big_buffer, len) != len)
      log_write(0, LOG_MAIN|LOG_PANIC, "failed to update journal for %s: %s",
        addr2->unique, strerror(errno));
    DEBUG(9) debug_printf("journalled %s\n", addr2->unique);
    }

  /* Ensure the journal file is pushed out to disc. */

  if (fsync(journal_fd) < 0)
    log_write(0, LOG_MAIN|LOG_PANIC, "failed to fsync journal: %s",
      strerror(errno));
  }

/* Wait for the process to finish. If it terminates with a non-zero code,
freeze the message, but leave the status values of all the addresses as they
are. Take care to handle the case when the subprocess doesn't seem to exist.
This has been seen on one system when Exim was called from an MUA that set
SIGCHLD to SIG_IGN. When that happens, wait() doesn't recognize the termination
of child processes. Exim now resets SIGCHLD to SIG_DFL, but this code should
still be robust. */

while ((rc = wait(&status)) != pid)
  {
  if (rc < 0 && errno == ECHILD)      /* Process has vanished */
    {
    log_write(0, LOG_MAIN, "%s transport process vanished unexpectedly",
      addr->transport->driver_name);
    status = 0;
    break;
    }
  }

if ((status & 0xffff) != 0)
  {
  int msb = (status >> 8) & 255;
  int lsb = status & 255;
  int code = (msb == 0)? (lsb & 0x7f) : msb;
  addr->special_action = SPECIAL_FREEZE;
  log_write(0, LOG_MAIN|LOG_PANIC, "%s transport process returned non-zero "
    "status 0x%04x: %s %d",
    addr->transport->driver_name,
    status,
    (msb == 0)? "terminated by signal" : "exit code",
    code);
  }

/* If SPECIAL_WARN is set in the top address, send a warning message. */

if (addr->special_action == SPECIAL_WARN &&
    addr->transport->warn_message != NULL)
  {
  int fd;
  char *warn_message;

  DEBUG(2) debug_printf("Warning message requested by transport\n");

  warn_message = expand_string(addr->transport->warn_message);
  if (warn_message == NULL)
    log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand \"%s\" (warning "
      "message for %s transport): %s", addr->transport->warn_message,
      addr->transport->name, expand_string_message);
  else
    {
    pid_t pid = child_open_exim(&fd);
    if (pid > 0)
      {
      FILE *f = fdopen(fd, "w");

      if (errors_reply_to != NULL)
        fprintf(f, "Reply-To: %s\n", errors_reply_to);
      fprintf(f, "From: Mail Delivery System <Mailer-Daemon@%s>\n",
        qualify_domain_sender);
      fprintf(f, "%s", warn_message);

      /* Close and wait for child process to complete, without a timeout. */

      fclose(f);
      (void)child_close(pid, 0);
      }
    }

  addr->special_action = SPECIAL_NONE;
  }

END_DELIVER_LOCAL:

/* Reset variables that are relevant to a single delivery. */

deliver_set_expansions(NULL);
}



/*************************************************
*              Do local deliveries               *
*************************************************/

/* This function processes the list of addresses in addr_local. True local
deliveries are always done one address at a time. However, local deliveries can
be batched up in some cases. Typically this is when writing batched SMTP output
files for use by some external transport mechanism, or when running local
deliveries over LMTP. Batching can also be specified in other special cases
using either directors or routers.

Arguments:   None
Returns:     Nothing
*/

static void
do_local_deliveries(void)
{
open_db dbblock;
open_db *dbm_file = NULL;
time_t now = time(NULL);

/* Loop until we have exhausted the supply of local deliveries */

while (addr_local != NULL)
  {
  address_item *addr2, *addr3, *nextaddr;
  int logflags = LOG_MAIN;
  int logchar = dont_deliver? '*' : '=';
  transport_instance *tp;

  /* Pick the first undelivered address off the chain */

  address_item *addr = addr_local;
  addr_local = addr->next;
  addr->next = NULL;

  /* An internal disaster if there is no transport. */

  if ((tp = addr->transport) == NULL)
    {
    logflags |= LOG_PANIC;
    addr->message =
      (addr->director != NULL)?
        string_sprintf("No transport set by %s director", addr->director)
      :
      (addr->router != NULL)?
        string_sprintf("No transport set by %s router", addr->router)
      :
      string_sprintf("No transport specified for local delivery from system "
        "filter - check %s",
        (addr->local_part[0] == '|')?
          "message_filter_pipe_transport" :
        (addr->local_part[0] == '/')?
          "message_filter_file_transport or message_filter_directory_transport" :
          "message_filter_reply_transport");

    post_process_one(addr, DEFER, logflags, DTYPE_TRANSPORT, 0);
    continue;
    }

  /* Check for batched addresses and possible amalgamation. We can't amalgamate
  if the transport depends on the local-part in any way. If amalgamation is
  permitted (even if it doesn't occur), don't use the local part in any retry
  key. Otherwise, use it if configured to do so. Before actually doing the
  delivery, the retry information will be checked. */

  if (tp->local_batch > local_batch_one &&
      !readconf_depends((driver_instance *)tp, "local_part"))
    {
    BOOL tp_sets_uid = tp->uid_set || tp->expand_uid != NULL ||
      tp->deliver_as_creator;
    int local_batch = tp->local_batch;
    int batch_count = 1;
    address_item **anchor = &addr_local;
    address_item *last = addr;
    address_item *next;

    /* If the transport's options depend on the domain, turn the batch
    option from "all" into "domain". */

    if (local_batch == local_batch_all &&
      readconf_depends((driver_instance *)tp, "domain"))
        local_batch = local_batch_domain;

    /* Pick off all addresses which have the same transport and errors
    address and (first) host and route_options and extra headers. If the
    transport has no uid/gid setting, they must also have the same uid/gid.
    If the local_batch option is "domain" they must also have the same domain.
    There is a maximum count set in the transport's batch_max field. */

    while ((next = *anchor) != NULL && batch_count < tp->batch_max)
      {
      if (next->transport == addr->transport &&
          next->errors_address == addr->errors_address &&
          same_headers(next->extra_headers, addr->extra_headers) &&
            (next->remove_headers == addr->remove_headers ||
              (next->remove_headers != NULL &&
               addr->remove_headers != NULL &&
               strcmp(next->remove_headers, addr->remove_headers) == 0)) &&
          (next->route_option == addr->route_option ||
            (next->route_option != NULL && addr->route_option != NULL &&
              strcmp(next->route_option, addr->route_option) == 0)) &&
          (tp_sets_uid ||
            (testflagsall(next, af_uid_set|af_gid_set) &&
             testflagsall(addr, af_uid_set|af_gid_set) &&
             next->uid == addr->uid &&
             next->gid == addr->gid)
          ) &&
          (local_batch == local_batch_all ||
            strcmp(next->domain, addr->domain) == 0) &&
          ((addr->host_list == NULL && next->host_list == NULL) ||
           (addr->host_list != NULL && next->host_list != NULL &&
            strcmp(addr->host_list->name, next->host_list->name) == 0)))
        {
        *anchor = next->next;
        next->next = NULL;
        last->next = next;
        last = next;
        batch_count++;
        }
      else anchor = &(next->next);
      }
    }

  /* We now have one or more addresses that can be delivered in a batch. Check
  whether the transport is prepared to accept a message of this size. If not,
  fail them all forthwith. */

  if (tp->message_size_limit > 0 && message_size > tp->message_size_limit)
    {
    addr->message =
      string_sprintf("message is too big (transport limit = %d)",
        tp->message_size_limit);
    replicate_status(addr);
    while (addr != NULL)
      {
      addr2 = addr->next;
      post_process_one(addr, FAIL, logflags, DTYPE_TRANSPORT, 0);
      addr = addr2;
      }
    continue;    /* With next batch of addresses */
    }

  /* If we are not running the queue, or if forcing, all deliveries will be
  attempted. Otherwise, we must respect the retry times for each address. Even
  when not doing this, we need to set up the retry key string, and determine
  whether a retry record exists, because after a successful delivery, a delete
  retry item must be set up. Keep the retry database open only for the duration
  of these checks, rather than for all local deliveries, because some local
  deliveries (e.g. to pipes) can take a substantial time. */

  dbm_file = dbfn_open("retry", O_RDONLY, &dbblock, FALSE);

  addr2 = addr;
  addr3 = NULL;
  while (addr2 != NULL)
    {
    BOOL ok = TRUE;   /* to deliver this address */

    /* Set up the retry key */

    addr2->local_retry_key = string_sprintf("T:%s%s%s",
        tp->retry_use_local_part? addr2->local_part : "",
        tp->retry_use_local_part? "@" : "",
        addr2->domain);

    /* Inspect the retry data. If there is no hints file, delivery happens. */

    if (dbm_file != NULL)
      {
      dbdata_retry *retry_record = dbfn_read(dbm_file, addr2->local_retry_key);

      /* If there is no retry record, delivery happens */

      if (retry_record != NULL)
        {
        setflag(addr2, af_lt_retry_exists);

        /* A retry record exists for this address. If queue running and not
        forcing, inspect its contents. If the record is too old, or if its
        retry time has come, or if it has passed its cutoff time, delivery
        will go ahead. */

        if (queue_running && !deliver_force)
          {
          ok = (now - retry_record->time_stamp > retry_data_expire) ||
               (now >= retry_record->next_try ) ||
               retry_record->expired;

          /* If we haven't reached the retry time, there is one more check
          to do, which is for the ultimate address timeout. */

          if (!ok)
            {
            retry_config *retry =
              retry_find_config(addr2->local_retry_key+2, addr2->domain,
                retry_record->basic_errno,
                retry_record->more_errno);

            DEBUG(9) debug_printf("retry time not reached for %s: "
              "checking ultimate address timeout\n", addr2->orig);

            if (retry != NULL)
              {
              retry_rule *last_rule;
              for (last_rule = retry->rules;
                   last_rule->next != NULL;
                   last_rule = last_rule->next);
              if (now - received_time > last_rule->timeout) ok = TRUE;
              }
            else ok = TRUE;    /* No rule => timed out */

            DEBUG(5)
              {
              if (ok) debug_printf("on queue longer than maximum retry for "
                "address - allowing delivery\n");
              }
            }
          }
        }
      }

    /* This address is to be delivered. Leave it on the chain. */

    if (ok)
      {
      addr3 = addr2;
      addr2 = addr2->next;
      }

    /* This address is to be deferred. Take it out of the chain, and
    post-process it as complete. Must take it out of the chain first,
    because post processing puts it on another chain. */

    else
      {
      address_item *this = addr2;
      this->message = "Retry time not yet reached";
      this->basic_errno = ERRNO_LRETRY;
      if (addr3 == NULL) addr2 = addr = addr2->next;
        else addr2 = addr3->next = addr2->next;
      post_process_one(this, DEFER, logflags, DTYPE_TRANSPORT, 0);
      }
    }

  if (dbm_file != NULL) dbfn_close(dbm_file);

  /* If there are no addresses left on the chain, they all deferred. Loop
  for the next set of addresses. */

  if (addr == NULL) continue;

  /* So, finally, we do have some addresses that can be passed to the
  transport. */

  deliver_local(addr, FALSE);

  /* If a shadow transport (which must perforce be another local transport), is
  defined, and its condition is met, we must pass the message to the shadow
  too, but only those addresses that succeeded. We do this by making a new
  chain of addresses - also to keep the original chain uncontaminated.

  NOTE: if the condition fails because of a lookup defer, there is nothing we
  can do! */

  if (tp->shadow != NULL &&
      (tp->shadow_condition == NULL ||
      expand_check_condition(tp->shadow_condition, tp->name, "transport")))
    {
    address_item *shadow_addr = NULL;
    address_item **last = &shadow_addr;

    for (addr2 = addr; addr2 != NULL; addr2 = addr2->next)
      {
      if (addr2->transport_return != OK) continue;
      addr3 = store_get(sizeof(address_item));
      *addr3 = *addr2;
      addr3->next = NULL;
      addr3->shadow_parent = addr2;
      addr3->transport = addr->transport->shadow;
      addr3->transport_return = DEFER;
      addr3->return_filename = NULL;
      addr3->return_file = -1;
      *last = addr3;
      last = &(addr3->next);
      }

    if (shadow_addr != NULL)
      {
      DEBUG(1) debug_printf(">>>>>> Shadow delivery >>>>>>\n");
      deliver_local(shadow_addr, TRUE);

      for(; shadow_addr != NULL; shadow_addr = shadow_addr->next)
        {
        int sresult = shadow_addr->transport_return;
        shadow_addr->shadow_parent->shadow_message = (sresult == OK)?
          string_sprintf(" ST=%s", tp->shadow->name) :
          string_sprintf(" ST=%s (%s%s%s)", tp->shadow->name,
            (shadow_addr->basic_errno <= 0)?
              "" : strerror(shadow_addr->basic_errno),
            (shadow_addr->basic_errno <= 0 || shadow_addr->message == NULL)?
              "" : ": ",
            (shadow_addr->message != NULL)? shadow_addr->message :
              (shadow_addr->basic_errno <= 0)? "unknown error" : "");

        DEBUG(9) debug_printf("%s shadow transport returned %s for %s\n",
          tp->shadow->name,
          (sresult == OK)?    "OK" :
          (sresult == DEFER)? "DEFER" :
          (sresult == FAIL)?  "FAIL" :
          (sresult == PANIC)? "PANIC" : "?",
          shadow_addr->orig);
        }

      DEBUG(1) debug_printf(">>>>>> End shadow delivery >>>>>>\n");
      }
    }

  /* Now we can process the results of the real transport. We must take each
  address off the chain first, because post_process_one() puts it on another
  chain. */

  for (addr2 = addr; addr2 != NULL; addr2 = nextaddr)
    {
    int result = addr2->transport_return;
    nextaddr = addr2->next;

    DEBUG(9) debug_printf("%s transport returned %s for %s\n",
      tp->name,
      (result == OK)?    "OK" :
      (result == DEFER)? "DEFER" :
      (result == FAIL)?  "FAIL" :
      (result == PANIC)? "PANIC" : "?",
      addr2->orig);

    /* If there is a retry_record, or if delivery is deferred, build a retry
    item for setting a new retry time or deleting the old retry record from
    the database. These items are handled all together after all addresses
    have been handled (so the database is open just for a short time for
    updating). */

    if (result == DEFER || testflag(addr2, af_lt_retry_exists))
      retry_add_item(addr2, "T", FALSE, addr2->local_retry_key+2,
        (result != DEFER)? rf_delete : 0);

    /* Done with this address */

    post_process_one(addr2, result, logflags, DTYPE_TRANSPORT, logchar);

    /* If a pipe delivery generated text to be sent back, the result may be
    changed to FAIL, and we must copy this for subsequent addresses in the
    batch. */

    if (addr2->transport_return != result)
      {
      for (addr3 = nextaddr; addr3 != NULL; addr3 = addr3->next)
        {
        addr3->transport_return = addr2->transport_return;
        addr3->basic_errno = addr2->basic_errno;
        addr3->message = addr2->message;
        }
      result = addr2->transport_return;
      }

    /* Whether or not the result was changed to FAIL, we need to copy the
    return_file value from the first address into all the addresses of the
    batch, so they are all listed in the error message. */

    addr2->return_file = addr->return_file;

    /* Change log character for recording successful deliveries. */

    if (result == OK) logchar = '-';
    }
  }        /* Loop back for next batch of addresses */
}




/*************************************************
*              Compare lists of hosts            *
*************************************************/

/* This function is given two pointers to chains of host items, and it yields
TRUE if the lists refer to the same hosts in the same order, except that
multiple hosts with the same non-negative MX values are permitted to appear in
different orders. Round-robinning nameservers can cause this to happen.

This enables exim to use a single SMTP transaction for sending to two entirely
different domains that happen to end up pointing at the same hosts.

Arguments:
  one       points to the first host list
  two       points to the second host list

Returns:    TRUE if the lists refer to the same host set
*/

static BOOL
same_hosts(host_item *one, host_item *two)
{
while (one != NULL && two != NULL)
  {
  if (strcmp(one->name, two->name) != 0)
    {
    int mx = one->mx;
    host_item *end_one = one;
    host_item *end_two = two;

    /* Batch up only if MX >= 0 */

    if (mx < 0) return FALSE;

    /* Find the ends of the shortest sequence of identical MX values */

    while (end_one->next != NULL && end_one->next->mx == mx &&
           end_two->next != NULL && end_two->next->mx == mx)
      {
      end_one = end_one->next;
      end_two = end_two->next;
      }

    /* If there aren't any duplicates, there's no match. */

    if (end_one == one) return FALSE;

    /* For each host in the 'one' sequence, check that it appears in the 'two'
    sequence, returning FALSE if not. */

    for (;;)
      {
      host_item *hi;
      for (hi = two; hi != end_two->next; hi = hi->next)
        if (strcmp(one->name, hi->name) == 0) break;
      if (hi == end_two->next) return FALSE;
      if (one == end_one) break;
      one = one->next;
      }

    /* All the hosts in the 'one' sequence were found in the 'two' sequence.
    Ensure both are pointing at the last host, and carry on as for equality. */

    two = end_two;
    }

  /* Hosts matched */

  one = one->next;
  two = two->next;
  }

/* True if both are NULL */

return (one == two);
}



/*************************************************
*           Sort remote deliveries               *
*************************************************/

/* This function is called if remote_sort is set. It arranges that the chain of
addresses for remote deliveries is ordered according to the strings specified.
Try to make this shuffling reasonably efficient by handling sequences of
addresses rather than just single ones.

Arguments:  None
Returns:    Nothing
*/

static void
sort_remote_deliveries(void)
{
int sep = 0;
address_item **aptr = &addr_remote;
char *listptr = remote_sort;
char *pattern;
char patbuf[256];

while (*aptr != NULL &&
       (pattern = string_nextinlist(&listptr, &sep, patbuf, sizeof(patbuf)))
       != NULL)
  {
  address_item *moved = NULL;
  address_item **bptr = &moved;

  while (*aptr != NULL)
    {
    address_item **next;
    if (match_check_string((*aptr)->domain, pattern, -1, TRUE, TRUE, NULL))
      {
      aptr = &((*aptr)->next);
      continue;
      }

    next = &((*aptr)->next);
    while (*next != NULL &&
      !match_check_string((*next)->domain, pattern, -1, TRUE, TRUE, NULL))
        next = &((*next)->next);

    /* If the batch of non-matchers is at the end, add on any that were
    extracted further up the chain, and end this iteration. Otherwise,
    extract them from the chain and hang on the moved chain. */

    if (*next == NULL)
      {
      *next = moved;
      break;
      }

    *bptr = *aptr;
    *aptr = *next;
    *next = NULL;
    bptr = next;
    aptr = &((*aptr)->next);
    }

  /* If the loop ended because the final address matched, *aptr will
  be NULL. Add on to the end any extracted non-matching addresses. If
  *aptr is not NULL, the loop ended via "break" when *next is null, that
  is, there was a string of non-matching addresses at the end. In this
  case the extracted addresses have already been added on the end. */

  if (*aptr == NULL) *aptr = moved;
  }

DEBUG(9)
  {
  address_item *addr;
  debug_printf("remote addresses after sorting:\n");
  for (addr = addr_remote; addr != NULL; addr = addr->next)
    debug_printf("  %s\n", addr->orig);
  }
}



/*************************************************
*  Read from pipe for remote delivery subprocess *
*************************************************/

/* This function is called when the subprocess is complete, but can also be
called before it is complete, in order to empty a pipe that is full (to prevent
deadlock). It must therefore keep track of its progress in the parlist data
block.

We read the pipe to get the delivery status codes and a possible error message
for each address, optionally preceded by unusability data for the hosts and
also by optional retry data.

Read in large chunks into the big buffer and then scan through, interpreting
the data therein. In most cases, only a single read will be necessary. No
individual item will ever be anywhere near 500 bytes in length, so by ensuring
that we read the next chunk when there is less than 500 bytes left in the
non-final chunk, we can assume each item is complete in store before handling
it. Actually, each item is written using a single write(), which is atomic for
small items (less than PIPE_BUF, which seems to be at least 512 in any Unix) so
even if we are reading while the subprocess is still going, we should never
have only a partial item in the buffer.

Argument:
  poffset     the offset of the parlist item
  eop         TRUE if the process has completed

Returns:      TRUE if the terminating 'Z' item has been read,
              or there has been a disaster (i.e. no more data needed);
              FALSE otherwise
*/

static BOOL
par_read_pipe(int poffset, BOOL eop)
{
host_item *h;
retry_item *r;
pardata *p = parlist + poffset;
address_item *addrlist = p->addrlist;
address_item *addr = p->addr;
pid_t pid = p->pid;
int fd = p->fd;
char *endptr = big_buffer;
char *ptr = endptr;
char *msg = p->msg;
BOOL done = p->done;
BOOL unfinished = TRUE;

/* Loop through all items, reading from the pipe when necessary. The pipe
is set up to be non-blocking, but there are two different Unix mechanisms in
use. Exim uses O_NONBLOCK if it is defined. This returns 0 for end of file,
and EAGAIN for no more data. If O_NONBLOCK is not defined, Exim uses O_NDELAY,
which returns 0 for both end of file and no more data. We distinguish the
two cases by taking 0 as end of file only when we know the process has
completed.

Each separate item is written to the pipe in a single write(), and as they are
all short items, the writes will all be atomic and we should never find
ourselves in the position of having read an incomplete item. */

DEBUG(9) debug_printf("reading pipe for subprocess %d (%s)\n",
  p->pid, eop? "ended" : "not ended");

while (!done)
  {
  int remaining = endptr - ptr;

  /* Read (first time) or top up the chars in the buffer if necessary.
  There will be only one read if we get all the available data (i.e. don't
  fill the buffer completely). */

  if (remaining < 500 && unfinished)
    {
    int len;
    int available = big_buffer_size - remaining;

    if (remaining > 0) memmove(big_buffer, ptr, remaining);

    ptr = big_buffer;
    endptr = big_buffer + remaining;
    len = read(fd, endptr, available);

    DEBUG(9) debug_printf("read() yielded %d\n", len);

    /* If the result is EAGAIN and the process is not complete, just
    stop reading any more and process what we have already. */

    if (len < 0)
      {
      if (!eop && errno == EAGAIN) len = 0; else
        {
        msg = string_sprintf("failed to read pipe from transport process "
          "%d for transport %s", pid, addr->transport->driver_name);
        break;
        }
      }

    /* If the length is zero (eof or no-more-data), just process what we
    already have. Note that if the process is still running and we have
    read all the data in the pipe (but less that "available") then we
    won't read any more, as "unfinished" will get set FALSE. */

    endptr += len;
    unfinished = len == available;
    }

  /* If we are at the end of the available data, exit the loop. */

  if (ptr >= endptr) break;

  /* Handle each possible type of item, assuming the complete item is
  available in store. */

  switch (*ptr++)
    {
    /* Host items exist only if any hosts were marked unusable. Match
    up by checking the IP address. */

    case 'H':
    for (h = addrlist->host_list; h != NULL; h = h->next)
      {
      if (h->address == NULL || strcmp(h->address, ptr+2) != 0) continue;
      h->status = ptr[0];
      h->why = ptr[1];
      }
    ptr += 2;
    while (*ptr++);
    break;

    /* Retry items must cause copies to be added to the toplevel address
    in this process. */

    case 'R':
    r = store_get(sizeof(retry_item));
    r->next = addrlist->retries;
    addrlist->retries = r;
    r->flags = *ptr++;
    r->destination = string_copy(ptr);
    while (*ptr++);
    memcpy(&(r->basic_errno), ptr, sizeof(r->basic_errno));
    ptr += sizeof(r->basic_errno);
    memcpy(&(r->more_errno), ptr, sizeof(r->more_errno));
    ptr += sizeof(r->more_errno);
    r->message = (*ptr)? string_copy(ptr) : NULL;
    while(*ptr++);
    break;

    /* Address items are in the order of items on the address chain. We
    remember the current address value in case this function is called
    several times to empty the pipe in stages. Information about delivery
    over TLS is sent in a preceding X item for each address. We don't put
    it in with the other info, in order to keep each message short enough to
    guarantee it won't be split in the pipe. */

    #ifdef SUPPORT_TLS
    case 'X':
    if (addr != NULL)
      {
      addr->cipher = (*ptr)? string_copy(ptr) : NULL;
      while (*ptr++);
      addr->peerdn = (*ptr)? string_copy(ptr) : NULL;
      while (*ptr++);
      break;
      }
    /* If addr == NULL, fall through to 'A', which diagnoses the error */
    #endif

    case 'A':
    if (addr == NULL)
      {
      msg = string_sprintf("address count mismatch for data read from pipe "
        "for transport process %d for transport %s", pid,
          addr->transport->driver_name);
      done = TRUE;
      break;
      }

    addr->transport_return = *ptr++;
    addr->special_action = *ptr++;
    memcpy(&(addr->basic_errno), ptr, sizeof(addr->basic_errno));
    ptr += sizeof(addr->basic_errno);
    memcpy(&(addr->more_errno), ptr, sizeof(addr->more_errno));
    ptr += sizeof(addr->more_errno);
    addr->message = (*ptr)? string_copy(ptr) : NULL;
    while(*ptr++);

    /* Always two strings for host information */

    if (*ptr)
      {
      h = store_get(sizeof(host_item));
      h->name = string_copy(ptr);
      while (*ptr++);
      h->address = string_copy(ptr);
      while(*ptr++);
      addr->transported = h;
      }
    else ptr++;

    /* Finished with this address */

    addr = addr->next;
    break;

    /* Z marks the logical end of the data. */

    case 'Z':
    done = TRUE;
    DEBUG(9) debug_printf("Z item read\n");
    break;

    /* Anything else is a disaster. */

    default:
    msg = string_sprintf("malformed data (%d) read from pipe for transport "
      "process %d for transport %s", ptr[-1], pid,
        addr->transport->driver_name);
    done = TRUE;
    break;
    }
  }

/* The done flag is inspected externally, to determine whether or not to
call the function again when the process finishes. */

p->done = done;

/* If the process hadn't finished, and we haven't seen the end of the data
or suffered a disaster, update the rest of the state, and return FALSE to
indicate "not finished". */

if (!eop && !done)
  {
  p->addr = addr;
  p->msg = msg;
  return FALSE;
  }

/* Close our end of the pipe, to prevent deadlock if the far end is still
pushing stuff into it. */

close(fd);
p->fd = -1;

/* If we have finished without error, but haven't had data for every address,
something is wrong. */

if (msg == NULL && addr != NULL)
  msg = string_sprintf("insufficient address data read from pipe "
    "for transport process %d for transport %s", pid,
      addr->transport->driver_name);

/* If an error message is set, something has gone wrong in getting back
the delivery data. Put the message into each address and freeze it. */

if (msg != NULL)
  {
  for (addr = addrlist; addr != NULL; addr = addr->next)
    {
    addr->transport_return = DEFER;
    addr->special_action = SPECIAL_FREEZE;
    addr->message = msg;
    }
  }

/* Return TRUE to indicate we have got all we need from this process, even
if it hasn't actually finished yet. */

return TRUE;
}



/*************************************************
*     Wait for one remote delivery subprocess    *
*************************************************/

/* This function is called when doing remote deliveries in parallel when either
the maximum number of processes exist and we need one to complete so that
another can be created, or when waiting for the last ones to complete. It must
wait for the completion of one subprocess, empty the control block slot, and
return a pointer to the address chain.

Arguments:    none
Returns:      pointer to the chain of addresses handled by the process;
              NULL if no subprocess found - this is an unexpected error
*/

static address_item *
par_wait(void)
{
int poffset, status;
address_item *addr, *addrlist;
pid_t pid;

set_process_info("delivering %s: waiting for a remote delivery subprocess "
  "to finish", message_id);

/* Loop until either a subprocess completes, or there are no subprocesses in
existence - in which case give an error return. We cannot proceed just by
waiting for a completion, because a subprocess may have filled up its pipe, and
be waiting for it to be emptied. Therefore, if no processes have finished, we
wait for one of the pipes to acquire some data by calling select(), with a
timeout just in case.

The simple approach is just to iterate after reading data from a ready pipe.
This leads to non-ideal behaviour when the subprocess has written its final Z
item, closed the pipe, and is in the process of exiting (the common case). A
call to waitpid() yields nothing completed, but select() shows the pipe ready -
reading it yields EOF, so you end up with busy-waiting until the subprocess has
actually finished.

To avoid this, if all the data that is needed has been read from a subprocess
after select(), an explicit wait() for it is done. We know that all it is doing
is writing to the pipe and then exiting, so the wait should not be long.

The non-blocking waitpid() is to some extent just insurance; if we could
reliably detect end-of-file on the pipe, we could always know when to do a
blocking wait() for a completed process. However, because some systems use
NDELAY, which doesn't distinguish between EOF and pipe empty, it is easier to
use code that functions without the need to recognize EOF. */

while ((pid = waitpid(-1, &status, WNOHANG)) <= 0)
  {
  struct timeval tv;
  fd_set select_pipes;
  int maxpipe, readycount;

  /* There are no subprocesses in existence */

  if (pid < 0)
    {
    DEBUG(9) debug_printf("*** no delivery children found\n");
    if (errno == ECHILD) return NULL;
    continue;
    }

  /* There is at least one subprocess, but it has not completed. */

  DEBUG(9) debug_printf("selecting on subprocess pipes\n");

  maxpipe = 0;
  FD_ZERO(&select_pipes);
  for (poffset = 0; poffset < remote_max_parallel; poffset++)
    {
    if (parlist[poffset].pid != 0)
      {
      int fd = parlist[poffset].fd;
      FD_SET(fd, &select_pipes);
      if (fd > maxpipe) maxpipe = fd;
      }
    }

  /* Stick in a 60-second timeout, just in case. */

  tv.tv_sec = 60;
  tv.tv_usec = 0;

  readycount = select(maxpipe + 1, (SELECT_ARG2_TYPE *)&select_pipes,
       NULL, NULL, &tv);

  /* Scan through the pipes and read any that are ready; use the count returned
  by select() to stop when there are no more. Select() can return with no
  processes (e.g. if interrupted). This shouldn't matter.

  If par_read_pipe() returns TRUE, it means that either the terminating Z was
  read, or there was a disaster. In either case, we are finished with this
  process. Do an explicit wait() and break the main loop if it succeeds. */

  for (poffset = 0; readycount > 0 && poffset < remote_max_parallel; poffset++)
    {
    if ((pid = parlist[poffset].pid) != 0)
      {
      if (FD_ISSET(parlist[poffset].fd, &select_pipes))
        {
        readycount--;
        if (par_read_pipe(poffset, FALSE) && waitpid(pid, &status, 0) == pid)
          goto PROCESS_DONE;
        }
      }
    }

  /* Now go back and look for a completed sub-process again. */
  }

/* A completed process was detected by the non-blocking waitpid(). Find the
data block that corresponds to this subprocess. */

for (poffset = 0; poffset < remote_max_parallel; poffset++)
  if (pid == parlist[poffset].pid) break;

if (poffset >= remote_max_parallel)
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "Process %d finished: not found in remote "
    "transport process list", pid);
  return NULL;
  }

/* Come here when all the data was completely read after a select(), and
the process in pid has been wait()ed for. */

PROCESS_DONE:

DEBUG(2)
  {
  if (status == 0)
    debug_printf("remote delivery process %d ended\n", pid);
  else
    debug_printf("remote delivery process %d ended: status=%04x\n", pid,
      status);
  }

set_process_info("delivering %s", message_id);

/* Get the chain of processed addresses */

addrlist = parlist[poffset].addrlist;

/* If the process did not finish cleanly, record an error and freeze, and also
ensure the journal is not removed, in case the delivery did actually happen. */

if ((status & 0xffff) != 0)
  {
  char *msg;
  int msb = (status >> 8) & 255;
  int lsb = status & 255;
  int code = (msb == 0)? (lsb & 0x7f) : msb;

  msg = string_sprintf("%s transport process returned non-zero status 0x%04x: "
    "%s %d",
    addrlist->transport->driver_name,
    status,
    (msb == 0)? "terminated by signal" : "exit code",
    code);

  for (addr = addrlist; addr != NULL; addr = addr->next)
    {
    addr->transport_return = DEFER;
    addr->special_action = SPECIAL_FREEZE;
    addr->message = msg;
    }

  remove_journal = FALSE;
  }

/* Else complete reading the pipe to get the result of the delivery, all
the data has not yet been obtained. */

else if (!parlist[poffset].done) (void)par_read_pipe(poffset, TRUE);

/* Mark the data slot unused, decrement the count of subprocesses, and return
the address chain. */

parlist[poffset].pid = 0;
parcount--;
return addrlist;
}



/*************************************************
*   Post-process a set of remote addresses       *
*************************************************/

/* Do what has to be done immediately after a remote delivery for each set of
addresses, then re-write the spool if necessary. Note that post_process_one
puts the address on an appropriate queue; hence we must fish off the next
one first. This function is also called if there is a problem with setting
up a subprocess to do a remote delivery in parallel. In this case, the final
argument contains a message, and the action must be forced to DEFER.

Argument:
   addr      pointer to chain of address items
   logflags  flags for logging
   msg       NULL for normal cases; -> error message for unexpected problems
   fallback  TRUE if processing fallback hosts

Returns:     nothing
*/

static void
remote_post_process(address_item *addr, int logflags, char *msg, BOOL fallback)
{
host_item *h;

/* If any host addresses were found to be unusable, add them to the unusable
tree so that subsequent deliveries don't try them. */

for (h = addr->host_list; h != NULL; h = h->next)
  {
  if (h->address == NULL) continue;
  if (h->status >= hstatus_unusable) tree_add_unusable(h);
  }

/* Now handle each address on the chain. The transport has placed '=' or '-'
into the special_action field for each successful delivery. */

while (addr != NULL)
  {
  address_item *next = addr->next;

  /* If msg == NULL (normal processing) and the result is DEFER and we are
  processing the main hosts and there are fallback hosts available, put the
  address on the list for fallback delivery. */

  if (addr->transport_return == DEFER &&
      addr->fallback_hosts != NULL &&
      !fallback &&
      msg == NULL)
    {
    addr->host_list = addr->fallback_hosts;
    addr->next = addr_fallback;
    addr_fallback = addr;
    DEBUG(1) debug_printf("%s queued for fallback host(s)\n", addr->orig);
    }

  /* If msg is set (=> unexpected problem), set it in the address before
  doing the ordinary post processing. */

  else
    {
    if (msg != NULL)
      {
      addr->message = msg;
      addr->transport_return = DEFER;
      }
    (void)post_process_one(addr, addr->transport_return, logflags,
      DTYPE_TRANSPORT, addr->special_action);
    }

  /* Next address */

  addr = next;
  }
}



/*************************************************
*           Do remote deliveries                 *
*************************************************/

/* This function is called to process the addresses in addr_remote. We must
pick off the queue all addresses that have the same transport, remote
destination, and errors_address, and hand them to the transport in one go,
subject to some configured limitations. If this is a run to continue delivering
to an existing delivery channel, skip all but those addresses that can go to
that channel. The skipped addresses just get defered.

Remote deliveries may be handled in parallel by separate subprocesses, up to a
configured maximum number at once.

Arguments:
  fallback  TRUE if processing fallback hosts

Returns:    Nothing
*/

static void
do_remote_deliveries(BOOL fallback)
{
parcount = 0;

while (addr_remote != NULL)
  {
  int address_count = 1;
  int address_count_max;
  BOOL multi_domain;
  address_item **anchor = &addr_remote;
  address_item *addr = addr_remote;
  address_item *last = addr;
  address_item *next;

  /* Pull the first address right off the list. */

  addr_remote = addr->next;
  addr->next = NULL;

  /* If no transport has been set, there has been a big screw-up somewhere. */

  if (addr->transport == NULL)
    {
    remote_post_process(addr, LOG_MAIN|LOG_PANIC,
      "No transport set by router or director", fallback);
    continue;
    }

  /* Force failure if the message is too big. */

  if (addr->transport->message_size_limit > 0 &&
      message_size > addr->transport->message_size_limit)
    {
    addr->message =
      string_sprintf("message is too big (transport limit = %d)",
        addr->transport->message_size_limit);
    addr->transport_return = FAIL;
    remote_post_process(addr, LOG_MAIN, NULL, fallback);
    continue;
    }

  /* Get the flag which specifies whether the transport can handle different
  domains that nevertheless resolve to the same set of hosts. */

  multi_domain = addr->transport->multi_domain;

  /* Get the maximum it can handle at once, with zero meaning unlimited. */

  address_count_max = addr->transport->max_addresses;
  if (address_count_max == 0) address_count_max = 999999;

  /* Pick off all addresses which have the same transport, errors address,
  destination, extra headers, and route_option. In some cases they point to
  the same host list, but we also need to check for identical host lists
  generated from entirely different domains. The host list pointers can be
  NULL in the case where the hosts are defined in the transport. If all
  addresses have the same domain, we can set the $domain expansion variable -
  configurations can arrange this by using the "domain" option, and then
  being able to look up things by domain can be useful. In fact, there is now
  a flag which can be set on a transport that restricts it to handling one
  domain at a time. There is also a configured maximum limit of addresses
  that can be handled at once. */

  deliver_domain = addr->domain;
  while ((next = *anchor) != NULL && address_count < address_count_max)
    {
    if ((multi_domain || strcmp(next->domain, addr->domain) == 0)
        &&
        next->transport == addr->transport
        &&
        same_hosts(next->host_list, addr->host_list)
        &&
        (next->errors_address == addr->errors_address ||
          (next->errors_address != NULL && addr->errors_address != NULL &&
             strcmp(next->errors_address, addr->errors_address) == 0))
        &&
        same_headers(next->extra_headers, addr->extra_headers)
        &&
        (next->remove_headers == addr->remove_headers ||
          (next->remove_headers != NULL &&
           addr->remove_headers != NULL &&
           strcmp(next->remove_headers, addr->remove_headers) == 0))
        &&
        (next->route_option == addr->route_option ||
          (next->route_option != NULL && addr->route_option != NULL &&
            strcmp(next->route_option, addr->route_option) == 0)))
      {
      *anchor = next->next;
      next->next = NULL;
      next->first = addr;  /* remember top one (for retry processing) */
      last->next = next;
      last = next;
      address_count++;
      if (multi_domain && deliver_domain != NULL &&
        strcmp(next->domain, deliver_domain) != 0)
          deliver_domain = NULL;
      }
    else anchor = &(next->next);
    }

  /* If there is only one address, we can set $local_part. */

  deliver_localpart = (addr->next == NULL)? addr->local_part : NULL;

  /* Set up data from any lookups in the router or director. */

  domain_data = addr->domain_data;
  local_part_data = addr->local_part_data;

  /* If this transport has a setup function, call it now so that it gets
  run in this process and not in any subprocess. That way, the results of
  any setup that are retained by the transport can be reusable. */

  if (addr->transport->setup != NULL)
    (void)((addr->transport->setup)(addr->transport, addr, NULL));

  /* If this is a run to continue delivery down an already-established
  channel, check that this set of addresses matches the transport and
  the channel. If it does not, defer the addresses. If a host list exists,
  we must check that the continue host is on the list. Otherwise, the
  host is set in the transport. */

  continue_more = FALSE;           /* In case got set for the last lot */
  if (continue_transport != NULL)
    {
    BOOL ok = strcmp(continue_transport, addr->transport->name) == 0;
    if (ok && addr->host_list != NULL)
      {
      host_item *h;
      ok = FALSE;
      for (h = addr->host_list; h != NULL; h = h->next)
        {
        if (strcmp(h->name, continue_hostname) == 0)
          { ok = TRUE; break; }
        }
      }

    /* Addresses not suitable; defer or queue for fallback hosts (which
    might be the continue host) and skip to next address. */

    if (!ok)
      {
      next = addr;

      if (addr->fallback_hosts != NULL && !fallback)
        {
        for (;;)
          {
          next->host_list = next->fallback_hosts;
          DEBUG(1) debug_printf("%s queued for fallback host(s)\n",
            next->orig);
          if (next->next == NULL) break;
          next = next->next;
          }
        next->next = addr_fallback;
        addr_fallback = addr;
        }

      else
        {
        while (next->next != NULL) next = next->next;
        next->next = addr_defer;
        addr_defer = addr;
        }

      continue;
      }

    /* Force remote_max_parallel to 1 so that we don't try to send two
    messages simultaneously down the same channel. This could happen if there
    are different domains that include the same host in otherwise different
    host lists. */

    remote_max_parallel = 1;

    /* Set a flag indicating whether there are further addresses that list
    the continued host. */

    for (next = addr_remote; next != NULL; next = next->next)
      {
      host_item *h;
      for (h = next->host_list; h != NULL; h = h->next)
        {
        if (strcmp(h->name, continue_hostname) == 0)
          { continue_more = TRUE; break; }
        }
      }
    }

  /* Compute the return path, expanding a new one if required. The old one
  must be set first, as it might be referred to in the expansion. */

  return_path = (addr->errors_address != NULL)?
    addr->errors_address : sender_address;

  if (addr->transport->return_path != NULL)
    {
    char *new_return_path = expand_string(addr->transport->return_path);
    if (new_return_path == NULL)
      {
      if (!expand_string_forcedfail)
        log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Failed to expand return path "
          "\"%s\": %s", addr->transport->return_path, expand_string_message);
      }
    else return_path = new_return_path;
    }

  /* Set up the route_option string, make the recipients list available for
  expansion, ensure that the transport is indeed a remote one, and then hand
  it the chain of addresses. */

  route_option = addr->route_option;
  deliver_recipients = addr;

  if (addr->transport->info->local)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Attempt non-local delivery with "
      "local transport for %s: transport %s", addr->orig,
      addr->transport->name);

  /* The transports set up the process info themselves as they may connect
  to more than one remote machine. They also have to set up the filter
  arguments, if required, so that the host name and address are available
  for expansion. */

  transport_filter_argv = NULL;

  /* If parallel delivery is disabled, or if there are no outstanding
  delivery subprocesses, and this is the final delivery to be made, then do
  the delivery in this process. */

  if (remote_max_parallel < 2 || (parcount == 0 && addr_remote == NULL))
    {
    transport_count = 0;
    debug_print_string(addr->transport->debug_string);
    if (!(addr->transport->info->code)(addr->transport, addr))
      replicate_status(addr);
    set_process_info("delivering %s (just run %s for %s%s)", message_id,
      addr->transport->name, addr->orig, (addr->next == NULL)? "" : ", ...");
    remote_post_process(addr, LOG_MAIN, NULL, fallback);

    /* If we have just delivered down a passed SMTP channel, and that was
    the last address, the channel will have been closed down. Now that
    we have logged that delivery, set continue_sequence to 1 so that
    any subsequent deliveries don't get "*" incorrectly logged. */

    if (continue_transport == NULL) continue_sequence = 1;
    }

  /* Parallel delivery is required. Create up to the configured number
  of subprocesses, each of which must pass back the delivery state via
  a pipe. */

  else
    {
    pid_t pid;
    BOOL pipe_done = FALSE;
    int parmax = remote_max_parallel;
    int poffset;
    int pfd[2];

    /* If the data for keeping a list of processes hasn't yet been
    set up, do so. */

    if (parlist == NULL)
      {
      parlist = store_get(remote_max_parallel * sizeof(pardata));
      for (poffset = 0; poffset < remote_max_parallel; poffset++)
        parlist[poffset].pid = 0;
      }

    /* Create the pipe for inter-process communication. If pipe creation
    fails, it is probably because the value of remote_max_parallel is so
    large that too many file descriptors for pipes have been created. Arrange
    to wait for a process to finish, and then try again. If we still can't
    create a pipe when all processes have finished, break the retry loop. */

    while (!pipe_done)
      {
      if (pipe(pfd) == 0) pipe_done = TRUE;
        else if (parcount > 0) parmax = parcount;
          else break;

      /* We need to make the reading end of the pipe non-blocking. There are
      two different options for this. Exim is cunningly (I hope!) coded so
      that it can use either of them, though it prefers O_NONBLOCK, which
      distinguishes between EOF and no-more-data. */

      #ifdef O_NONBLOCK
      fcntl(pfd[pipe_read], F_SETFL, O_NONBLOCK);
      #else
      fcntl(pfd[pipe_read], F_SETFL, O_NDELAY);
      #endif

      /* If the maximum number of subprocesses already exist, or if we have
      run out of file descriptors, wait for a process to finish. If we can't
      find one, there is some shambles. Better not bomb out, as that might
      lead to multiple copies of the message. Just log and proceed as if all
      done. */

      while (parcount >= parmax)
        {
        address_item *doneaddr = par_wait();
        if (doneaddr == NULL)
          {
          log_write(0, LOG_MAIN|LOG_PANIC,
            "remote delivery process count got out of step");
          parcount = 0;
          }
        else remote_post_process(doneaddr, LOG_MAIN, NULL, fallback);
        }
      }

    /* If we failed to create a pipe and there were no processes to wait
    for, we have to give up on this one. Do this outside the above loop
    so that we can continue the main loop. */

    if (!pipe_done)
      {
      remote_post_process(addr, LOG_MAIN|LOG_PANIC,
        string_sprintf("unable to create pipe: %s", strerror(errno)), fallback);
      continue;
      }

    /* Find a free slot in the pardata list. Must do this after the possible
    waiting for processes to finish, because a terminating process will free
    up a slot. */

    for (poffset = 0; poffset < remote_max_parallel; poffset++)
      if (parlist[poffset].pid == 0) break;

    /* If there isn't one, there has been a horrible disaster. */

    if (poffset >= remote_max_parallel)
      {
      close(pfd[pipe_write]);
      close(pfd[pipe_read]);
      remote_post_process(addr, LOG_MAIN|LOG_PANIC,
        "Unexpectedly no free subprocess slot", fallback);
      continue;
      }

    /* Now fork a sub-process to do the remote delivery. */

    if ((pid = fork()) == 0)
      {
      int fd = pfd[pipe_write];
      host_item *h;
      retry_item *r;
      debug_pid = getpid();
      DEBUG(1) debug_printf("Remote delivery process %d started\n",
        debug_pid);

      /* Reset the random number generator, so different processes don't all
      have the same sequence. */

      random_seed = 0;

      /* Close open file descriptors for the pipes of other processes
      that are running in parallel. */

      for (poffset = 0; poffset < remote_max_parallel; poffset++)
        if (parlist[poffset].pid != 0) close(parlist[poffset].fd);

      /* This process has inherited a copy of the file descriptor
      for the data file, but its file pointer is shared with all the
      other processes running in parallel. Therefore, we have to re-open
      the file in order to get a new file descriptor with its own
      file pointer. We don't need to lock it, as the lock is held by
      the parent process. There doesn't seem to be any way of doing
      a dup-with-new-file-pointer. */

      close(deliver_datafile);
      sprintf(spoolname, "%s/input/%s/%s-D", spool_directory, message_subdir,
        message_id);
      deliver_datafile = open(spoolname, O_RDWR | O_APPEND);

      if (deliver_datafile < 0)
        log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Failed to reopen %s for remote "
          "parallel delivery: %s", spoolname, strerror(errno));

      /* Set the close-on-exec flag */

      fcntl(deliver_datafile, F_SETFD, fcntl(deliver_datafile, F_GETFD) |
        FD_CLOEXEC);

      /* Close the unwanted half of this process' pipe, set the process state,
      and run the transport. */

      close(pfd[pipe_read]);
      set_process_info("delivering %s using %s", message_id,
        addr->transport->name);
      transport_count = 0;
      debug_print_string(addr->transport->debug_string);
      if (!(addr->transport->info->code)(addr->transport, addr))
        replicate_status(addr);

      set_process_info("delivering %s (just run %s for %s%s in subprocess)",
        message_id, addr->transport->name, addr->orig,
        (addr->next == NULL)? "" : ", ...");

      /* Pass the result back down the pipe. This is a lot more information
      than is needed for a local delivery. We have to send back the error
      status for each address, the usability status for each host that is
      flagged as unusable, and all the retry items. When TLS is in use, we
      send also the cipher and peerdn information. Each type of information
      is flagged by an identifying byte, and is then in a fixed format (with
      strings terminated by zeros), and there is a final terminator at the
      end. The host information and retry information is all attached to
      the first address, so that gets sent at the start. */

      /* Host unusability information: for most success cases this will
      be null. */

      for (h = addr->host_list; h != NULL; h = h->next)
        {
        if (h->address == NULL || h->status < hstatus_unusable) continue;
        sprintf(big_buffer, "H%c%c%s", h->status, h->why, h->address);
        write(fd, big_buffer, (int)strlen(big_buffer+3) + 4);
        }

      /* Retry information: for most success cases this will be null. */

      for (r = addr->retries; r != NULL; r = r->next)
        {
        char *ptr;
        sprintf(big_buffer, "R%c%.500s", r->flags, r->destination);
        ptr = big_buffer + (int)strlen(big_buffer+2) + 3;
        memcpy(ptr, &(r->basic_errno), sizeof(r->basic_errno));
        ptr += sizeof(r->basic_errno);
        memcpy(ptr, &(r->more_errno), sizeof(r->more_errno));
        ptr += sizeof(r->more_errno);
        if (r->message == NULL) *ptr++ = 0; else
          {
          sprintf(ptr, "%.512s", r->message);
          while(*ptr++);
          }
        write(fd, big_buffer, ptr - big_buffer);
        }

      /* Information about what happened to each address. Two items are used:
      an 'X' item first, for TLS information, followed by an 'A' item for the
      remaining data. */

      for(; addr != NULL; addr = addr->next)
        {
        char *ptr;

        /* Use an X item only if there's something to send */

        #ifdef SUPPORT_TLS
        if (addr->cipher != NULL)
          {
          ptr = big_buffer;
          *ptr++ = 'X';
          sprintf(ptr, "%.128s", addr->cipher);
          while(*ptr++);
          if (addr->peerdn == NULL) *ptr++ = 0; else
            {
            sprintf(ptr, "%.512s", addr->peerdn);
            while(*ptr++);
            }
          write(fd, big_buffer, ptr - big_buffer);
          }
        #endif

        /* The rest of the information goes in an 'A' item. */

        ptr = big_buffer + 3;
        sprintf(big_buffer, "A%c%c", addr->transport_return,
          addr->special_action);
        memcpy(ptr, &(addr->basic_errno), sizeof(addr->basic_errno));
        ptr += sizeof(addr->basic_errno);
        memcpy(ptr, &(addr->basic_errno), sizeof(addr->more_errno));
        ptr += sizeof(addr->more_errno);

        if (addr->message == NULL) *ptr++ = 0; else
          {
          sprintf(ptr, "%.512s", addr->message);
          while(*ptr++);
          }

        if (addr->transported == NULL) *ptr++ = 0; else
          {
          sprintf(ptr, "%.256s", addr->transported->name);
          while(*ptr++);
          sprintf(ptr, "%.64s", addr->transported->address);
          while(*ptr++);
          }
        write(fd, big_buffer, ptr - big_buffer);
        }

      /* Add termination flag, close the pipe, and that's it. */

      write(fd, "Z", 1);
      close(fd);
      search_tidyup();
      exit(EXIT_SUCCESS);
      }

    /* Back in the mainline: close the unwanted half of the pipe. */

    close(pfd[pipe_write]);

    /* Fork failed; defer with error message */

    if (pid <= 0)
      {
      close(pfd[pipe_read]);
      remote_post_process(addr, LOG_MAIN|LOG_PANIC,
        string_sprintf("fork failed for remote delivery to %s: %s",
          addr->route_domain, strerror(errno)), fallback);
      continue;
      }

    /* Fork succeeded; increment the count, and remember relevant data for
    when the process finishes. */

    parcount++;
    parlist[poffset].addrlist = parlist[poffset].addr = addr;
    parlist[poffset].pid = pid;
    parlist[poffset].fd = pfd[pipe_read];
    parlist[poffset].done = FALSE;
    parlist[poffset].msg = NULL;
    }
  }

/* Reached the end of the list of addresses. If we are doing parallel
deliveries, there will be subprocesses still running when we get to this
point. Wait for them and post-process their addresses. */

while (parcount > 0)
  {
  address_item *doneaddr = par_wait();
  if (doneaddr == NULL)
    {
    log_write(0, LOG_MAIN|LOG_PANIC,
      "remote delivery process count got out of step");
    parcount = 0;
    }
  else remote_post_process(doneaddr, LOG_MAIN, NULL, fallback);
  }
}




/*************************************************
*          Determine locality of an address      *
*************************************************/

/* This function determines whether an address is local or not, and sets the
"local" field in the address accordingly. It also sets the "local_part" and
"domain" fields, and lowercases appropriate parts of the address. If the
address is local, we must be prepared to do the "percent hack" for configured
domains.

Argument:
  addr      points to an addr_item block containing the original address
            in its orig field; the af_forced_local flag is also inspected.

Returns:    TRUE on success
            FALSE if it couldn't proceed because of a search lookup defer
*/

BOOL
deliver_setlocal(address_item *addr)
{
char *active = addr->orig;
char *domain = parse_find_at(active);

addr->domain = string_copylc(domain+1);    /* Domains are always caseless */

/* Handle the case when the address contains a local domain. On most Unix
systems, local parts should be treated caselessly, but there is an option not
to do this. The implication in the RFCs (though I can't say I've seen it
spelled out explicitly) is that quoting should be removed from local parts at
the point where they are locally interpreted. [The new draft "821" is more
explicit on this, Jan 1999.] We know the syntax is valid, so this can be done
by simply removing quoting backslashes and any unquoted doublequotes. Remember
if this was done so that the local part can be reconstructed correctly if
necessary. */

if (testflag(addr, af_forced_local) ||
      match_isinlist(addr->domain, &local_domains, TRUE, TRUE, NULL))
  {
  int i;
  int len = domain - active;
  char *t = addr->local_part = store_get(len+1);
  for (i = 0; i < len; i++)
    {
    int c = *active++;
    if (c == '\"')
      {
      setflag(addr, af_local_part_quoted);
      continue;
      }
    if (c == '\\')
      {
      setflag(addr, af_local_part_quoted);
      *t++ = *active++;
      i++;
      }
    else *t++ = (locally_caseless)? tolower(c) : c;
    }
  *t = 0;

  /* Flag the address as local */

  setflag(addr, af_local);

  /* We do the percent hack only for those domains that are listed in
  percent_hack_domains. A loop is needed to handle the case of multiple
  local %-hacks. */

  while (match_isinlist(addr->domain, &percent_hack_domains, TRUE, TRUE, NULL))
    {
    char *pc = strrchr(addr->local_part, '%');
    if (pc == NULL) break;

    /* Handle a % hack by making a copy of the new domain and then removing
    it from the local_part field. If the new domain is remote, we are done;
    otherwise let the loop continue to check for more % hacks. */

    addr->domain = string_copy(pc+1);
    *pc = 0;

    if (!match_isinlist(addr->domain, &local_domains, TRUE, TRUE, NULL))
      {
      clearflag(addr, af_local);
      break;
      }
    }
  }

/* The locality couldn't be determined because of a lookup defer. */

else if (search_find_defer)
  {
  addr->message = "local_domains lookup deferred";
  addr->basic_errno = ERRNO_LISTDEFER;
  return FALSE;
  }

/* The address refers to a remote domain. Don't mess with the case
of the local part, or change it in any way at all. */

else
  {
  addr->local_part = string_copyn(active, domain-active);
  clearflag(addr, af_local);
  }

return TRUE;
}




/*************************************************
*      Get next error message text               *
*************************************************/

/* If f is not NULL, read the next "paragraph", from a customized error message
text file, terminated by a line containing ****, and expand it.

Arguments:
  f          NULL or a file to read from
  which      string indicating which string (for errors)

Returns:     NULL or an expanded string
*/

static char *
next_emf(FILE *f, char *which)
{
int size = 256;
int ptr = 0;
char *para, *yield;
char buffer[256];

if (f == NULL) return NULL;

if (fgets(buffer, sizeof(buffer), f) == NULL ||
    strcmp(buffer, "****\n") == 0) return NULL;

para = store_get(size);
for (;;)
  {
  para = string_cat(para, &size, &ptr, buffer, (int)strlen(buffer));
  if (fgets(buffer, sizeof(buffer), f) == NULL ||
      strcmp(buffer, "****\n") == 0) break;
  }
para[ptr] = 0;

yield = expand_string(para);
if (yield != NULL) return yield;

log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand string from "
  "errmsg_file or warnmsg_file (%s): %s", which, expand_string_message);
return NULL;
}




/*************************************************
*      Close down a passed transport channel     *
*************************************************/

/* This function is called when a passed transport channel cannot be used.
It attempts to close it down tidily. The yield is always FAIL so that the
function call can be the argument of a "return" statement.

Arguments:  None
Returns:    FAIL
*/

static int
continue_closedown(void)
{
if (continue_transport != NULL)
  {
  transport_instance *t;
  for (t = transports; t != NULL; t = t->next)
    {
    if (strcmp(t->name, continue_transport) == 0)
      {
      if (t->info->closedown != NULL) (t->info->closedown)(t);
      break;
      }
    }
  }
return FAIL;
}




/*************************************************
*           Print address information            *
*************************************************/

/* This function is called to output an address, or information about an
address, for bounce or defer messages. If the hide_child flag is set, all we
output is the original ancestor address.

Arguments:
  addr         points to the address
  f            the FILE to print to
  si           an initial string
  sc           a continuation string for before "generated"
  se           an end string

Returns:       TRUE if the address is not hidden
*/

static BOOL
print_address_information(address_item *addr, FILE *f, char *si, char *sc,
  char *se)
{
BOOL yield = TRUE;
char *printed = "";
address_item *ancestor = addr;
while (ancestor->parent != NULL) ancestor = ancestor->parent;

fprintf(f, "%s", si);

if (addr->parent != NULL && testflag(addr, af_hide_child))
  {
  printed = "an undisclosed address";
  yield = FALSE;
  }

else if (!testflag(addr, af_pfr) || addr->parent == NULL)
  printed = addr->orig;

else
  {
  char *s = addr->orig;
  char *ss;

  if (addr->orig[0] == '>') { ss = "mail"; s++; }
  else if (addr->orig[0] == '|') ss = "pipe";
  else ss = "save";

  fprintf(f, "%s to %s%sgenerated by ", ss, s, sc);
  printed = addr->parent->orig;
  }

fprintf(f, "%s", string_printing(printed));

if (ancestor != addr)
  {
  char *original = (ancestor->onetime_parent == NULL)?
    ancestor->orig : ancestor->onetime_parent;
  if (strcmpic(original, printed) != 0)
    fprintf(f, "%s(%sgenerated from %s)", sc,
      (ancestor != addr->parent)? "ultimately " : "",
      string_printing(original));
  }

fprintf(f, "%s", se);
return yield;
}






/*************************************************
*         Print error for an address             *
*************************************************/

/* This function is called to print the error information out of an address for
a bounce or a warning message. It tries to format the message reasonably by
introducing newlines. All lines are indented by 4; the initial printing
position must be set before calling.

Arguments:
  addr         points to the address
  f            the FILE to print on

Returns:       nothing
*/

static void
print_address_error(address_item *addr, FILE *f)
{
char *s = addr->message;
if (addr->basic_errno > 0)
  {
  fprintf(f, "%s%s", strerror(addr->basic_errno),
    (s == NULL)? "" : ":\n    ");
  }
if (s == NULL)
  {
  if (addr->basic_errno <= 0) fprintf(f, "unknown error");
  }
else
  {
  int count = 0;
  while (*s != 0)
    {
    if (*s == '\\' && s[1] == 'n')
      {
      fprintf(f, "\n    ");
      s += 2;
      count = 0;
      }
    else
      {
      fputc(*s, f);
      count++;
      if (*s++ == ':' && isspace((uschar)*s) && count > 45)
        {
        fprintf(f, "\n   ");  /* sic (because space follows) */
        count = 0;
        }
      }
    }
  }
}





/*************************************************
*              Deliver one message               *
*************************************************/

/* This is the function which is called when a message is to be delivered. It
is passed the id of the message. It is possible that the message no longer
exists, if some other process has delivered it, and it is also possible that
the message is being worked on by another process, in which case the data file
will be locked.

If no delivery is attempted for any of the above reasons, the function returns
FALSE, otherwise TRUE.

If the give_up flag is set true, do not attempt any deliveries, but instead
fail all outstanding addresses and return the message to the sender (or
whoever).

A delivery operation has a process all to itself; we never deliver more than
one message in the same process. Therefore we needn't worry about store
leakage.

This function is called while running as root unless the security level is
1 or 3, in which case root can be regained by calling mac_sete{g,u}id.

Arguments:
  id          the id of the message to be delivered
  forced      TRUE if delivery was forced by an administrator; this overrides
              retry delays and causes a delivery to be tried regardless
  give_up     TRUE if an administrator has requested that delivery attempts
              be abandoned

Returns:      OK if an attempt was made at delivering the message; DEFER if the
              load average was too high to attempt delivery; FAIL otherwise
*/

int
deliver_message(char *id, BOOL forced, BOOL give_up)
{
int i, rc, fd;
int load_max = (queue_run_pid != (pid_t)0 && deliver_queue_load_max >= 0)?
      deliver_queue_load_max : deliver_load_max;
time_t now = time(NULL);
address_item *addr_last = NULL;
FILE *jread;
int process_recipients = RECIP_ACCEPT;
open_db dbblock;
open_db *dbm_file;

if (queue_run_pid == (pid_t)0)
  {
  DEBUG(1) debug_printf("delivering message %s\n", id);
  set_process_info("delivering %s", id);
  }
else
  {
  DEBUG(1) debug_printf("delivering message %s (queue run pid %d fd %d)\n",
    id, queue_run_pid, queue_run_pipe);
  set_process_info("delivering %s (queue run pid %d)", id, queue_run_pid);
  }

/* Ensure that we catch any sub-processes that are created. Although Exim
sets SIG_DFL as its initial default, some routes through the code end up
here with it set to SIG_IGN - cases where a non-synchronous delivery process
has been forked, but no re-exec has been done. */

signal(SIGCHLD, SIG_DFL);

/* Make the forcing flag available for directors/routers/transports, set up
the global message id field, and initialize the count for returned files and
the message size. */

deliver_force = forced;
strcpy(message_id, id);
return_count = 0;
message_size = 0;

/* Initialise some flags */

update_spool = FALSE;
remove_journal = TRUE;

/* Reset the random number generator, so that if several delivery processes are
started from a queue runner that has already used random numbers (for sorting),
they don't all get the same sequence. */

random_seed = 0;

/* If deliver_load_max is non-negative, check that the load average is low
enough to permit deliveries. If running the queue, and deliver_queue_load_max
is set, its value is used instead. If we are not root here, we can become root
by seteuid() and need to do this for the benefit of some OS that require it. */

if (!forced && load_max >= 0)
  {
  #ifdef LOAD_AVG_NEEDS_ROOT
  if (geteuid() != root_uid)
    {
    mac_seteuid(root_uid);
    load_average = os_getloadavg();
    mac_setegid(exim_gid);
    mac_seteuid(exim_uid);
    }
  else
  #endif

  load_average = os_getloadavg();

  DEBUG(2) debug_printf("load average = %.2f max = %.2f\n",
    (double)load_average/1000.0,
    (double)load_max/1000.0);

  /* Write the log entry for queue run abandon here, so we can include
  the load average value. */

  if (load_average > load_max)
    {
    DEBUG(2) debug_printf("abandoning delivery - load average too high\n");
    if (queue_running)
      log_write(0, LOG_MAIN, "Abandon queue run (load %.2f, max %.2f)",
        (double)load_average/1000.0,
        (double)load_max/1000.0);
    return DEFER;
    }
  }

/* Open and lock the message's data file. Exim locks on this one because the
header file may get replaced as it is re-written during the delivery process.
Any failures cause messages to be written to the log, except for missing files
while queue running - another process probably completed delivery. As part of
opening the data file, message_subdir gets set. */

if (!spool_open_datafile(id)) return continue_closedown();

/* Now read the contents of the header file, which will set up the headers in
store, and also the list of recipients and the tree of non-recipients and
assorted flags. If there is a reading or format error, give up; if the message
has been around for sufficiently long, remove it. */

sprintf(spoolname, "%s-H", id);
if ((rc = spool_read_header(spoolname, TRUE, TRUE)) != spool_read_OK)
  {
  if (errno == ERRNO_SPOOLFORMAT)
    {
    struct stat statbuf;
    sprintf(big_buffer, "%s/input/%s/%s", spool_directory, message_subdir,
      spoolname);
    if (stat(big_buffer, &statbuf) == 0)
      {
      int size = statbuf.st_size;   /* Because might be a long */
      log_write(0, LOG_MAIN, "Format error in spool file %s: size=%d",
        spoolname, size);
      }
    else log_write(0, LOG_MAIN, "Format error in spool file %s", spoolname);
    }
  else
    log_write(0, LOG_MAIN, "Error reading spool file %s: %s", spoolname,
      strerror(errno));

  /* If we managed to read the envelope data, received_time contains the
  time the message was received. Otherwise, we can calculate it from the
  message id. */

  if (rc != spool_read_hdrerror)
    {
    received_time = 0;
    for (i = 0; i < 6; i++)
      received_time = received_time * 62 + tab62[id[i] - '0'];
    }

  /* If we've had this malformed message too long, sling it. */

  if (now - received_time > keep_malformed)
    {
    sprintf(spoolname, "%s/msglog/%s/%s", spool_directory, message_subdir, id);
    unlink(spoolname);
    sprintf(spoolname, "%s/input/%s/%s-D", spool_directory, message_subdir, id);
    unlink(spoolname);
    sprintf(spoolname, "%s/input/%s/%s-H", spool_directory, message_subdir, id);
    unlink(spoolname);
    sprintf(spoolname, "%s/input/%s/%s-J", spool_directory, message_subdir, id);
    unlink(spoolname);
    log_write(0, LOG_MAIN, "Message removed because older than %s",
      readconf_printtime(keep_malformed));
    }

  close(deliver_datafile);
  deliver_datafile = -1;
  return continue_closedown();
  }

/* The spool header file has been read. Look to see if there is an existing
journal file for this message. If there is, it means that a previous delivery
attempt crashed (program or host) before it could update the spool header file.
Read the list of delivered addresses from the journal and add them to the
nonrecipients tree. Then update the spool file. We can leave the journal in
existence, as it will get further successful deliveries added to it in this
run, and it will be deleted if this function gets to its end successfully.
Otherwise it might be needed again. */

sprintf(spoolname, "%s/input/%s/%s-J", spool_directory, message_subdir, id);
jread = fopen(spoolname, "r");
if (jread != NULL)
  {
  while (fgets(big_buffer, big_buffer_size, jread) != NULL)
    {
    int n = (int)strlen(big_buffer);
    big_buffer[n-1] = 0;
    tree_add_nonrecipient(big_buffer+1, big_buffer[0] == 'Y');
    DEBUG(2) debug_printf("Previously delivered address %s taken from "
      "journal file\n", big_buffer+1);
    }
  fclose(jread);
  /* Panic-dies on error */
  (void)spool_write_header(message_id, SW_DELIVERING, NULL);
  }
else if (errno != ENOENT)
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "attempt to open journal for reading gave: "
    "%s", strerror(errno));
  return continue_closedown();
  }

/* A null recipients list indicates some kind of disaster. */

if (recipients_list == NULL)
  {
  close(deliver_datafile);
  deliver_datafile = -1;
  log_write(0, LOG_MAIN, "Spool error: no recipients for %s", spoolname);
  return continue_closedown();
  }


/* Handle a message that is frozen. There are a number of different things that
can happen, but in the default situation, unless forced, no delivery is
attempted. */

if (deliver_freeze)
  {
  #ifdef SUPPORT_MOVE_FROZEN_MESSAGES
  /* Moving to another directory removes the message from Exim's view. Other
  tools must be used to deal with it. Logging of this action happens in
  spool_move_message() and its subfunctions. */

  if (move_frozen_messages &&
      spool_move_message(id, message_subdir, "", "F"))
    return continue_closedown();
  #endif

  /* For bounce messages, and others with no sender, check for error message
  ignore timer and thaw if exceeded, setting ignore_errmsg_errors, so the
  message will be discarded if this delivery fails. */

  if (sender_address[0] == 0 && ignore_errmsg_errors_after > 0 &&
      message_age >= ignore_errmsg_errors_after)
    {
    ignore_errmsg_errors = TRUE;
    log_write(0, LOG_MAIN, "Unfrozen by errmsg timer");
    }

  /* For all frozen messages (bounces or not), timeout_frozen_after sets the
  maximum time to keep messages that are frozen. Thaw if we reach it, with a
  flag causing all recipients to be failed. The time is the age of the
  message, not the time since freezing. */

  else if (timeout_frozen_after > 0 && message_age >= timeout_frozen_after)
    {
    log_write(0, LOG_MAIN, "cancelled by timeout_frozen_after");
    process_recipients = RECIP_FAIL_TIMEOUT;
    }

  /* If there's no auto thaw, or we haven't reached the auto thaw time yet, and
  this delivery is not forced by an admin user, do not attempt delivery of this
  message. Note that forced is set for continuing messages down the same
  channel, in order to skip load checking and ignore hold domains, but we
  don't want unfreezing in that case. */

  else
    {
    if ((auto_thaw <= 0 || now <= deliver_frozen_at + auto_thaw) &&
      (!forced || !deliver_force_thaw || !admin_user ||
        continue_hostname != NULL))
      {
      close(deliver_datafile);
      deliver_datafile = -1;
      log_write(5, LOG_MAIN, "Message is frozen");
      return continue_closedown();
      }

    /* If delivery was forced (by an admin user), assume a manual thaw.
    Otherwise it's an auto thaw. */

    if (forced)
      {
      deliver_manual_thaw = TRUE;
      log_write(0, LOG_MAIN, "Unfrozen by forced delivery");
      }
    else log_write(0, LOG_MAIN, "Unfrozen by auto-thaw");
    }

  /* We get here if any of the rules for unfreezing have triggered. */

  deliver_freeze = FALSE;
  update_spool = TRUE;
  }

/* If asked to give up on a message, log who did it, and set the action for all
the addresses. */

if (give_up)
  {
  struct passwd *pw = getpwuid(real_uid);
  log_write(0, LOG_MAIN, "cancelled by %s", (pw != NULL)?
        pw->pw_name : string_sprintf("uid %ld", (long int)real_uid));
  process_recipients = RECIP_FAIL;
  }

/* Otherwise, if a system-wide, address-independent message filter is
specified, run it now. If it yields "delivered", then ignore the true
recipients of the message. Failure of the filter file is logged, and the
delivery attempt fails. */

else if (message_filter != NULL)
  {
  int action;
  BOOL delivered;
  char *error;
  void *reset_point;

  return_path = sender_address;
  enable_dollar_recipients = TRUE;   /* Permit $recipients in system filter */

  /* Use the temporary pool for storage so it can be reclaimed. Any created
  addresses will be put in the normal pool. */

  store_pool = POOL_TEMP;
  reset_point = store_get(0);

  /* Any error in the filter file causes a delivery to be abandoned. */

  if (!filter_system_interpret(&addr_new, &delivered, &action, &error))
    {
    close(deliver_datafile);
    deliver_datafile = -1;
    log_write(0, LOG_MAIN|LOG_PANIC, "Error in message_filter file: %s",
      string_printing(error));
    return continue_closedown();
    }

  /* Reset things - if there is a freeze/fail message, we must copy it
  into persistent store before resetting. */

  enable_dollar_recipients = FALSE;
  if (filter_fmsg != NULL)
    filter_fmsg = string_copy_malloc(filter_fmsg);
  store_reset(reset_point);
  store_pool = POOL_MAIN;

  /* Save the values of the system filter variables so that user filters
  can use them. */

  memcpy(filter_sn, filter_n, sizeof(filter_sn));

  /* The filter can request that a message be frozen, but this does not
  take place if the message has been manually thawed. In that case, we must
  unset "delivered", which is forced by the "freeze" command to make -bF
  work properly. */

  if (action == SPECIAL_FREEZE && !deliver_manual_thaw)
    {
    deliver_freeze = TRUE;
    deliver_frozen_at = time(NULL);
    process_recipients = RECIP_DEFER;
    frozen_info = string_sprintf(" by message filter%s%s",
      (filter_fmsg == NULL)? "" : ": ",
      (filter_fmsg == NULL)? "" : filter_fmsg);
    }

  /* The filter can request that a message be failed. */

  else if (action == SPECIAL_FAIL)
    {
    delivered = FALSE;
    process_recipients = RECIP_FAIL_FILTER;
    log_write(0, LOG_MAIN, "cancelled by message filter%s%s",
      (filter_fmsg == NULL)? "" : ": ",
      (filter_fmsg == NULL)? "" : filter_fmsg);
    }

  /* Delivery can be restricted only to those recipients (if any) that the
  filter specified. */

  else if (delivered)
    {
    process_recipients = RECIP_IGNORE;
    if (addr_new == NULL)
      log_write(1, LOG_MAIN, "=> discarded (message_filter)");
    else
      log_write(1, LOG_MAIN, "original recipients ignored (message_filter)");
    }

  /* If any new addresses were created by the filter, fake up a "parent"
  for them. This is necessary for pipes, etc., which are expected to have
  parents, and it also gives some sensible logging for others. Allow
  pipes, files, and autoreplies, and run them as the filter uid if set,
  otherwise as the current uid. These addresses also get an extra header,
  X-Envelope-To, containing up to the first 100 envelope recipients. */

  if (addr_new != NULL)
    {
    int size = 1024;
    int slen;
    int uid = (message_filter_uid_set)? message_filter_uid : geteuid();
    int gid = (message_filter_gid_set)? message_filter_gid : getegid();

    header_line *h = store_get(sizeof(header_line));

    /* The text "message filter" is tested in transport_set_up_command() and in
    set_up_shell_command() in the pipe transport, to enable them to permit
    $recipients, so don't change it here without also changing it there. */

    address_item *parent = deliver_make_addr("message filter", FALSE); /* See above */
    address_item *p;

    h->text = store_get(size);
    strcpy(h->text, "X-Envelope-To: ");
    slen = (int)strlen(h->text);

    for (i = 0; i < recipients_count && i < 100; i++)
      {
      char *s = recipients_list[i].address;
      int len = (int)strlen(s);

      if (slen + len + 4 >= size)
        {
        int oldsize = size;
        while (slen + len + 4 >= size) size *= 2;
        if (!store_extend(h->text, oldsize, size))
          {
          char *newtext = store_get(size);
          memcpy(newtext, h->text, slen);
          h->text = newtext;
          }
        }

      if (i != 0)
        {
        strcpy(h->text + slen, ",\n ");
        slen += 3;
        }

      strcpy(h->text + slen, s);
      slen += len;
      }

    strcpy(h->text + slen, "\n");
    h->slen = slen + 1;
    h->next = NULL;
    h->type = htype_other;

    parent->domain = string_copylc(qualify_domain_recipient);
    parent->local_part = "message filter";

    for (p = addr_new; p != NULL; p = p->next)
      {
      addr_last = p;
      p->extra_headers = h;
      parent->child_count++;
      p->parent = parent;
      if (testflag(p, af_pfr))
        {
        p->uid = uid;
        p->gid = gid;
        setflag(p, af_uid_set |
                   af_gid_set |
                   af_allow_file |
                   af_allow_pipe |
                   af_allow_reply);

        /* If the message filter specific transports are not set,
        the global settings will be used. */

        if (p->orig[0] == '|')
          p->transport = transport_message_filter_pipe;
        else
          if (p->orig[0] == '>') p->transport = transport_message_filter_reply;
        else
          {
          int len = (int)strlen(p->orig);
          if (p->orig[len-1] == '/')
            {
            p->transport = transport_message_filter_directory;
            if (len > 1 && p->orig[len-2] == '/' &&
                transport_message_filter_directory2 != NULL)
              p->transport = transport_message_filter_directory2;
            }
          else p->transport = transport_message_filter_file;
          }
        }
      DEBUG(9) debug_printf("message_filter added %s\n", p->orig);
      }
    }
  }

/* Scan the recipients list, and for every one that is not in the non-
recipients tree, add an addr item to the chain of new addresses. If the
rf_onetime flag is set, then we must set the onetime parent from the pno value,
which points to the relevant entry in the recipients list.

This processing can be altered by the setting of the process_recipients
variable, which is changed if recipients are to be ignored, failed, or
deferred. This can happen as a result of system filter activity, or if the -Mg
option is used to fail all of them.

Duplicate addresses are handled later by a different tree structure; we can't
just extend the non-recipients tree, because that will be re-written to the
spool if the message is deferred, and in any case there are casing
complications for local addresses. */

if (process_recipients != RECIP_IGNORE)
  {
  for (i = 0; i < recipients_count; i++)
    {
    if (tree_search_addr(tree_nonrecipients, recipients_list[i].address, FALSE)
         == NULL)
      {
      recipient_item *r = recipients_list + i;
      address_item *new = deliver_make_addr(r->address, FALSE);

      if ((r->flags & rf_onetime) != 0)
        new->onetime_parent = recipients_list[r->pno].address;

      #ifdef SUPPORT_DSN
      new->dsn_flags = r->flags & rf_dsnflags;
      new->dsn_orcpt = r->orcpt;
      #endif

      switch (process_recipients)
        {
        /* RECIP_DEFER is set when a system filter freezes a message. */

        case RECIP_DEFER:
        new->next = addr_defer;
        addr_defer = new;
        break;

        /* RECIP_FAIL is set when -Mg has been used. If the message has a null
        sender and an address has no errors address, send it to the global
        errors address. Put the address on the failed list so that it is
        used to create a bounce. */

        case RECIP_FAIL:
        new->message  = "delivery cancelled by administrator";
        if (sender_address[0] == 0 && new->errors_address == NULL)
          new->errors_address = errors_address;
        new->next = addr_failed;
        addr_failed = new;
        break;

        /* RECIP_FAIL_FILTER is set when a system filter has obeyed a "fail"
        command. */

        case RECIP_FAIL_FILTER:
        new->message =
          (filter_fmsg == NULL)? "delivery cancelled" : filter_fmsg;
        new->next = addr_failed;
        addr_failed = new;
        break;

        /* RECIP_FAIL_TIMEOUT is set when a message is frozen, but is older
        than the value in timeout_frozen_after. Treat non-bounce messages
        similarly to -Mg; for bounce messages we just want to discard, so
        don't put the address on the failed list. */

        case RECIP_FAIL_TIMEOUT:
        new->message  = "delivery cancelled; message timed out";
        if (sender_address[0] != 0)
          {
          new->next = addr_failed;
          addr_failed = new;
          }
        break;

        /* Value should be RECIP_ACCEPT; take this as the safe default. */

        default:
        if (addr_new == NULL) addr_new = new; else addr_last->next = new;
        addr_last = new;
        break;
        }
      }
    }
  }

DEBUG(7)
  {
  address_item *p = addr_new;
  debug_printf("Delivery address list:\n");
  while (p != NULL)
    {
    debug_printf("  %s %s\n", p->orig, (p->onetime_parent == NULL)? "" :
      p->onetime_parent);
    p = p->next;
    }
  }

/* Set up the buffers used for copying over the file when delivering. */

deliver_in_buffer = store_malloc(DELIVER_IN_BUFFER_SIZE);
deliver_out_buffer = store_malloc(DELIVER_OUT_BUFFER_SIZE);


/* Open the message log file. This records details of deliveries, deferments,
and failures for the benefit of the mail administrator. The log is not used by
exim itself to track the progress of a message; that is done by rewriting the
header spool file.

Exim is running as root here, unless seteuid() has been used to reduce
privilege while directing and routing. The message log can therefore end up
being owned by root. However, if an exim uid is defined, the msglog directory
will be owned by exim, and so when the time comes to delete the file, the
ownership doesn't matter. So don't waste effort making exim own it. However,
ensure that the mode is the same as other spool files. */

sprintf(spoolname, "%s/msglog/%s/%s", spool_directory, message_subdir, id);
fd = open(spoolname, O_WRONLY|O_APPEND|O_CREAT, SPOOL_MODE);

if (fd < 0)
  {
  if(errno == ENOENT)
    {
    char temp[16];
    sprintf(temp, "msglog/%s", message_subdir);
    if (message_subdir[0] == 0) temp[6] = 0;
    (void)directory_make(spool_directory, temp, MSGLOG_DIRECTORY_MODE, TRUE);
    fd = open(spoolname, O_WRONLY|O_APPEND|O_CREAT, SPOOL_MODE);
    }
  if (fd < 0)
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "Couldn't open message log %s: %s",
      spoolname, strerror(errno));
    return continue_closedown();
    }
  }

/* Set the close-on-exec flag. */

fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);

/* Make sure the file's group is the Exim gid if exim_uid exists (can't have
exim_uid set without exim_gid), and double-check the mode because the group
setting doesn't always get set automatically. */

if (exim_uid_set)
  {
  fchown(fd, exim_uid, exim_gid);
  fchmod(fd, SPOOL_MODE);
  }

/* Now make a C stream out of it. */

message_log = fdopen(fd, "a");
if (message_log == NULL)
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "Couldn't fdopen message log %s: %s",
    spoolname, strerror(errno));
  return continue_closedown();
  }

/* Until there are no more new addresses, handle each one as follows:

 . If this is a generated address (indicated by the presence of a parent
   pointer) then check to see whether it is a pipe or a file, and if so,
   handle it directly here. The director that produced the address will have
   set the allow flags into the address, and also set the uid/gid required.
   Having the directors generate new addresses and then checking them here at
   the outer level is tidier than making each director do its checking, and
   means that directors don't need access to the failed address queue.

 . Determine if it is a local address; this may involve removing one
   or more leading "@<local-domain>" strings, and/or handing the percent
   hack on the local user name. A subroutine handles all this, setting the
   "local", "local_part", and "domain" fields in the address. Local addresses
   are those that involve local domains, or those that have been passed back
   from routers via the "self" option.

.  If it is a local address that was generated by another director, determine
   if any of its parents have the same local name. If so, generate a different
   string for previous delivery checking, and do the check. Without this
   code, if the address spqr generates spqr via a forward or alias file,
   delivery of the generated spqr stops further attempts at the top level spqr,
   which is not what is wanted.

 . Local addresses get put on the addr_direct chain, while remote addresses
   get put on the addr_route chain. However, before doing so, we check up
   on the retry database to see if a delay is set for directing or routing
   the address. If so, the address gets put directly onto the addr_defer
   chain. For directed addresses, while we have the retry database open, we
   check for the existence of a transport retry record, and save the next_try
   time if one is found. This saves a later database access for normal
   deliveries to local users.

 . Now we run the local addresses through the directors. A director may put
   the address on the addr_local chain for local delivery, or put it on the
   addr_failed chain if it is undeliveable, or it may generate child addresses
   and put them on the addr_new chain, or it may defer an address. All these
   things are passed as arguments so that the directors can be called for
   verification purposes as well.

 . Because directing may produce new addresses, we have to have an outer loop
   to do this all again. The reason for not doing the directing as soon as
   an address is determined to be local is that directing can in principle take
   some appreciable amount of time, and we do not want to have the retry
   database open any longer than is necessary, nor do we want to open and close
   it for each local address.

.  When all the directing is done, run the remote addresses through the
   routers. It may turn out that some apparently remote addresses are in fact
   local ones that have been abbreviated and so not recognized as local until
   the router expands them. Such addresses get put onto the addr_new chain
   and the whole thing recycles yet again. Wheels within wheels...
*/

header_changed = FALSE;            /* No headers rewritten yet */
while (addr_new != NULL)           /* Loop again after routing */
  {
  while (addr_new != NULL)         /* Loop again after directing */
    {
    address_item *addr;
    dbm_file = dbfn_open("retry", O_RDONLY, &dbblock, FALSE);

    /* Loop for current batch of new addresses */

    while (addr_new != NULL)
      {
      addr = addr_new;
      addr_new = addr->next;

      DEBUG(9) debug_printf(">>>>>>>>>>>>>>>>>>>>>>>>\n");
      DEBUG(9) debug_printf("Considering: %s\n", addr->orig);

      /* Handle generated address that is a pipe or a file or an autoreply. */

      if (testflag(addr, af_pfr))
        {
        /* If two different users specify delivery to the same pipe or file or
        autoreply, there should be two different deliveries, so build a unique
        string that incorporates the original address, and use this for
        duplicate testing and recording delivery. */

        addr->unique = string_sprintf("%s:%s", addr->orig, addr->parent->orig);

        /* If a filter file specifies two deliveries to the same pipe or file,
        we want to de-duplicate, but this is probably not wanted for two mail
        commands to the same address, where probably both should be delivered.
        So, we have to invent a different unique string in that case. Just
        keep piling '>' characters on the front. */

        if (addr->orig[0] == '>')
          {
          while (tree_search(tree_duplicates, addr->unique) != NULL)
            addr->unique = string_sprintf(">%s", addr->unique);
          }

        else if (tree_search(tree_duplicates, addr->unique) != NULL)
          {
          DEBUG(9) debug_printf("%s is a duplicate address: discarded\n",
            addr->orig);
          addr->next = addr_duplicate;
          addr_duplicate = addr;
          continue;
          }

        DEBUG(9) debug_printf("unique = %s\n", addr->unique);

        /* Check for previous delivery */

        if (tree_search(tree_nonrecipients, addr->unique) != NULL)
          {
          DEBUG(9) debug_printf("%s was previously delivered: discarded\n",
            addr->orig);
          (void)child_done(addr, tod_stamp(tod_log));
          continue;
          }

        /* Save for checking future duplicates */

        tree_add_duplicate(addr->unique, TRUE);

        /* Set local part and domain */

        addr->local_part = addr->orig;
        addr->domain = addr->parent->domain;

        /* Handle a file. Treat /dev/null as a special case and abandon the
        delivery. This avoids having to specify a uid on the transport just
        for this case. Arrange for the transport name to be logged as
        "**bypassed**", by removing any transport that may be present. */

        if (addr->orig[0] == '/')
          {
          if (!testflag(addr, af_allow_file))
            {
            addr->basic_errno = ERRNO_FORBIDFILE;
            addr->message = "delivery to file forbidden";
            (void)post_process_one(addr, FAIL, LOG_MAIN, DTYPE_DIRECTOR, 0);
            continue;   /* with the next new address */
            }
          if (strcmp(addr->orig, "/dev/null") == 0)
            {
            if (addr->basic_errno == ERRNO_BADTRANSPORT)
              (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_DIRECTOR, 0);
            else
              {
              char *save = addr->transport->name;
              addr->transport->name = US"**bypassed**";
              (void)post_process_one(addr, OK, LOG_MAIN, DTYPE_DIRECTOR, '=');
              addr->transport->name = save;
              }
            continue;   /* with the next new address */
            }
          }

        /* Handle a pipe */

        else if (addr->orig[0] == '|')
          {
          if (!testflag(addr, af_allow_pipe))
            {
            addr->basic_errno = ERRNO_FORBIDPIPE;
            addr->message = "delivery to pipe forbidden";
            (void)post_process_one(addr, FAIL, LOG_MAIN, DTYPE_DIRECTOR, 0);
            continue;   /* with the next new address */
            }
          }

        /* Handle an auto-reply */

        else if (!testflag(addr, af_allow_reply))
          {
          addr->basic_errno = ERRNO_FORBIDREPLY;
          addr->message = "autoreply forbidden";
          (void)post_process_one(addr, FAIL, LOG_MAIN, DTYPE_DIRECTOR, 0);
          continue;     /* with the next new address */
          }

        /* If the errno field is already set to BADTRANSPORT, it indicates
        failure to expand a transport string, or find the associated transport,
        or an unset transport when one is required. Leave this test till now so
        that the forbid errors are given in preference. */

        if (addr->basic_errno == ERRNO_BADTRANSPORT)
          {
          (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_DIRECTOR, 0);
          continue;
          }

        /* Pipe, file, or autoreply delivery is to go ahead. */

        addr->next = addr_local;
        addr_local = addr;

        continue;       /* with the next new address */
        }


      /* All addresses should have been made fully qualified when the message
      was accepted or when a director generated a new address, so panic if we
      find an unqualified one. */

      if (parse_find_at(addr->orig) == NULL)
        log_write(0, LOG_PANIC_DIE, "Unqualified address \"%s\" found in %s",
          addr->orig, id);

      /* Determine locality - this sets the "local_part", "domain", and "local"
      fields, and lowercases the domain and the local part if local. If there's
      a lookup defer, defer this address. */

      if (!deliver_setlocal(addr))
        {
        (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_NONE, 0);
        continue;
        }

      /* Check to see if the domain is held. If so, proceed only if the
      delivery was forced by hand. */

      if (!forced && hold_domains != NULL &&
           (match_isinlist(addr->domain, &hold_domains, TRUE, TRUE, NULL) ||
            search_find_defer))
        {
        if (search_find_defer)
          {
          addr->message = "hold_domains lookup deferred";
          addr->basic_errno = ERRNO_LISTDEFER;
          }
        else
          {
          addr->message = "domain is held";
          addr->basic_errno = ERRNO_HELD;
          }
        (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_NONE, 0);
        continue;
        }

      /* Now we can check for duplicates and previously delivered addresses.
      We need to check local addresses using the lower cased form of the local
      part. If the address is local, we must first check for a matching address
      in its parents, and if so, generate a different string to use for
      duplication checking. Stick a \ on the front in the simple case; it *is*
      possible for there to be more than one level; use \n\ for subsequent
      cases.

      Have to take some care about prefix/suffix. If such an address succeeds,
      the prefix and/or suffix are left off the address, but it might be a
      parent of something else as a result of the use of "unseen" in a filter
      or on a director. */

      if (testflag(addr, af_local))
        {
        address_item *parent;
        for (parent = addr->parent; parent != NULL; parent = parent->parent)
          {
          if (strcmpic(addr->domain, parent->domain) != 0) continue;
          if (strcmpic(addr->local_part, parent->local_part) == 0) break;
          if (parent->prefix != NULL || parent->suffix != NULL)
            {
            sprintf(big_buffer, "%.256s%.256s%.256s",
              (parent->prefix == NULL)? "" : parent->prefix,
              parent->local_part,
              (parent->suffix == NULL)? "" : parent->suffix);
            if (strcmpic(addr->local_part, big_buffer) == 0) break;
            }
          }
        if (parent != NULL)
          {
          if (parent->unique[0] == '\\')
            {
            addr->unique =
              string_sprintf("\\%c\\%s@%s",
              (parent->unique[2] == '\\')? parent->unique[1] + 1 : '1',
              addr->local_part, addr->domain);
            }
          else addr->unique =
            string_sprintf("\\%s@%s", addr->local_part, addr->domain);
          }
        else addr->unique =
          string_sprintf("%s@%s", addr->local_part, addr->domain);
        }

      /* Remote addresses may occasionally have identical parents, if the
      "unseen" option is in use, or in cases of domain expansion. In these
      cases, an alternative unique name is required. In the case of domain
      expansion, the parent's expanded domain won't match the child's
      unexpanded one, so compare the unique names as well, to catch those
      cases. */

      else if (addr->parent != NULL)
        {
        address_item *parent;
        for (parent = addr->parent; parent != NULL; parent = parent->parent)
          {
          if (strcmp(addr->unique, parent->unique) == 0) break;
          if (strcmp(addr->local_part, parent->local_part) == 0 &&
              strcmp(addr->domain, parent->domain) == 0)
            break;
          }
        if (parent != NULL)
          {
          if (parent->unique[0] == '\\')
            {
            addr->unique =
              string_sprintf("\\%c\\%s",
              (parent->unique[2] == '\\')? parent->unique[1] + 1 : '1',
              addr->unique);
            }
          else addr->unique = string_sprintf("\\%s", addr->unique);
          }
        }

      DEBUG(9) debug_printf("unique = %s\n", addr->unique);

      /* Check for previous delivery. This can happen for generated addresses
      or if configuration files change or if things got out of step. Ensure
      the counts in any parents are updated. */

      if (tree_search_addr(tree_nonrecipients, addr->unique,
           testflag(addr, af_pfr)) != NULL)
        {
        DEBUG(9) debug_printf("%s was previously delivered: discarded\n",
          addr->unique);
        (void)child_done(addr, tod_stamp(tod_log));
        continue;
        }

      /* Check for duplication. Remember duplicated addresses so they can
      be marked "delivered" when the duplicate is delivered. */

      if (tree_search_addr(tree_duplicates, addr->unique,
           testflag(addr, af_pfr)) != NULL)
        {
        DEBUG(9) debug_printf("%s is a duplicate address: discarded\n",
          addr->unique);
        addr->next = addr_duplicate;
        addr_duplicate = addr;
        continue;
        }

      /* Remember the first, to check for subsequent duplicates. */

      tree_add_duplicate(addr->unique, testflag(addr, af_pfr));

      /* If the address is local, check on directing retry status, and add
      either to the directing chain or the defer chain. */

      if (testflag(addr, af_local))
        {
        char *destination = string_sprintf("D:%s@%s", addr->local_part,
          addr->domain);
        dbdata_retry *retry_record =
          (dbm_file == NULL)? NULL : dbfn_read(dbm_file, destination);

        if (retry_record != NULL &&
            now - retry_record->time_stamp > retry_data_expire)
          retry_record = NULL;

        /* Defer directing if queue running, unless no retry data or we've
        passed the next retry time, or this message is forced. However, if
        the retry time has expired, try just one more time. If this fails,
        subsequent processing of the retry data should cause the address to
        fail. This ensures that local addresses are always directed at least
        once before being rejected. */

        if (queue_running && !deliver_force &&
            retry_record != NULL && now < retry_record->next_try &&
            !retry_record->expired)
          {
          addr->message = "retry time not reached";
          addr->basic_errno = ERRNO_DRETRY;
          (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_DIRECTOR, 0);
          }

        /* Otherwise set up for directing. Remember whether there is a retry
        record or not, so a request for its updating can be forced when
        directing occurs. */

        else
          {
          addr->next = addr_direct;
          addr_direct = addr;
          if (retry_record != NULL) setflag(addr, af_dr_retry_exists);
          DEBUG(7) debug_printf("%s: queued for directing\n", addr->orig);
          }
        }

      /* If the address is not local, check on routing retry status, and
      add either to the remote chain or the defer chain. */

      else
        {
        dbdata_retry *domain_retry_record = (dbm_file == NULL)? NULL :
          dbfn_read(dbm_file, string_sprintf("R:%s", addr->domain));

        dbdata_retry *address_retry_record = (dbm_file == NULL)? NULL :
          dbfn_read(dbm_file, string_sprintf("R:%s@%s", addr->local_part,
          addr->domain));

        if (domain_retry_record != NULL &&
            now - domain_retry_record->time_stamp > retry_data_expire)
          domain_retry_record = NULL;

        if (address_retry_record != NULL &&
            now - address_retry_record->time_stamp > retry_data_expire)
          address_retry_record = NULL;

        /* Defer routing unless no retry data or we've passed the next
        retry time for the domain, or this message is forced. However,
        if the domain retry time has expired, allow the routing attempt.
        If it fails again, the address will be failed. This ensures that
        each address is routed at least once, even after long-term routing
        failures.

        If there is an address retry, check that too; just wait for the next
        retry time, but only do this for queue-running deliveries. This helps
        with the case when the temporary error on the address was really
        message-specific rather than address specific, since it allows other
        messages through. */

        if (!deliver_force &&
            ((domain_retry_record != NULL &&
              now < domain_retry_record->next_try &&
              !domain_retry_record->expired)
            ||
            (queue_running && address_retry_record != NULL &&
              now < address_retry_record->next_try))
           )
          {
          addr->message = "retry time not reached";
          addr->basic_errno = ERRNO_RRETRY;
          (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_ROUTER, 0);
          }

        /* The domain is OK for routing. Check to see if there is a retry
        delay on this specific address. These can happen if an SMTP transport
        received a temporary error response from a remote host. */

        else
          {
          if (domain_retry_record != NULL || address_retry_record != NULL)
            setflag(addr, af_dr_retry_exists);
          addr->next = addr_route;
          addr_route = addr;
          DEBUG(7) debug_printf("%s: queued for routing\n", addr->orig);
          }
        }
      }

    /* The database is closed while directing and routing is happening.
    Requests to update it are put on a chain and all processed together
    at the end. */

    if (dbm_file != NULL) dbfn_close(dbm_file);

    /* Run the local addresses that are not already deferred through the
    directors. If a director defers an address, or if there is an existing
    retry record in the database, add a retry item. Note that a director is
    permitted to generate a remote delivery if it is set up with a non-local
    transport. Ensure return_path is available so that it can be referred to
    in filter files or elsewhere. */

    while ((addr = addr_direct) != NULL)
      {
      int rc;
      addr_direct = addr->next;
      addr->next = NULL;
      return_path = (addr->errors_address != NULL)?
        addr->errors_address : sender_address;
      if ((rc = direct_address(addr, &addr_local, &addr_remote, &addr_new,
        &addr_succeed, v_none)) == DEFER || testflag(addr, af_dr_retry_exists))
          retry_add_item(addr, "D", TRUE, NULL, (rc != DEFER)? rf_delete : 0);

      /* Handle addresses that are finished with because of a failure. */

      if (rc != OK)
        (void)post_process_one(addr, rc, LOG_MAIN, DTYPE_DIRECTOR, 0);
      }
    }       /* Loop to handle any new addresses created by the directors */


  /* DEBUG: verify what's happened after all the directing */

  DEBUG(7)
    {
    address_item *p = addr_local;
    debug_printf(">>>>>>>>>>>>>>>>>>>>>>>>\n");
    debug_printf("After directing:\n  Local deliveries:\n");
    while (p != NULL)
      {
      debug_printf("    %s\n", p->orig);
      p = p->next;
      }

    p = addr_remote;
    debug_printf("  Remote deliveries:\n");
    while (p != NULL)
      {
      debug_printf("    %s\n", p->orig);
      p = p->next;
      }

    p = addr_failed;
    debug_printf("  Failed addresses:\n");
    while (p != NULL)
      {
      debug_printf("    %s\n", p->orig);
      p = p->next;
      }

    p = addr_route;
    debug_printf("  Addresses to be routed:\n");
    while (p != NULL)
      {
      debug_printf("    %s\n", p->orig);
      p = p->next;
      }

    p = addr_defer;
    debug_printf("  Deferred addresses:\n");
    while (p != NULL)
      {
      debug_printf("    %s\n", p->orig);
      p = p->next;
      }
    }

  /* If queue_remote is set, we don't even want to try routing remote
  addresses; otherwise don't touch any that match queue_remote_domains.
  Optimize by skipping this pass through the addresses if neither is set. */

  if (!deliver_force && (queue_remote || queue_remote_domains != NULL))
    {
    address_item *okaddr = NULL;
    while (addr_route != NULL)
      {
      address_item *addr = addr_route;
      addr_route = addr->next;

      if (!queue_remote &&
	  !match_isinlist(addr->domain, &queue_remote_domains, TRUE, TRUE,
            NULL))
	{
        if (search_find_defer)
          {
          addr->basic_errno = ERRNO_LISTDEFER;
	  addr->message = "queue_remote_domains lookup deferred";
	  (void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_ROUTER, 0);
          }
        else
          {
	  addr->next = okaddr;
	  okaddr = addr;
          }
	}
      else
	{
	addr->basic_errno = ERRNO_LOCAL_ONLY;
        addr->message = queue_remote? "remote delivery suppressed" :
          "domain is in queue_remote_domains";
	(void)post_process_one(addr, DEFER, LOG_MAIN, DTYPE_ROUTER, 0);
	}
      }

    addr_route = okaddr;
    }

  /* Now route those remote addresses that are not deferred. Routing may take
  some time (DNS timeouts) and I originally planned to get the local deliveries
  done before the routing, but since routing may cause addresses to get
  re-written, this is not the best plan. */

  while (addr_route != NULL)
    {
    int rc;
    address_item *addr = addr_route;
    char *old_domain = addr->domain;
    addr_route = addr->next;
    addr->next = NULL;

    /* Just in case some router parameter refers to it. */

    return_path = (addr->errors_address != NULL)?
      addr->errors_address : sender_address;

    /* If a router defers an address, or if there is an existing retry record
    in the database, add a retry item. Since the domain might have been
    rewritten (expanded to fully qualified) as a result of routing, ensure
    that both forms are listed if necessary - this will be the delete case,
    since a defer won't change the domain. */

    if ((rc = route_address(addr, &addr_local, &addr_remote, &addr_new,
      v_none)) == DEFER || testflag(addr, af_dr_retry_exists))
        {
        int flags = (rc != DEFER)? rf_delete : 0;
        retry_add_item(addr, "R", FALSE, NULL, flags);
        if (strcmp(addr->domain, old_domain) != 0)
          retry_add_item(addr, "R", FALSE, old_domain, flags);
        }

    /* If an address turned out to be local after all, put it back on the
    addr_new chain for re-directing/re-routing, and build a new original
    address. In fact, we need to do this by creating a new address with the
    old one as parent, so that the original address gets marked done when the
    child is done. */

    if (rc == ISLOCAL)
      {
      address_item *old = addr;
      char *new_address =
        string_sprintf("%s@%s", addr->local_part, addr->domain);
      addr = deliver_make_addr(new_address, FALSE);
      copyflag(addr, old, (af_forced_local|af_ignore_error));
      addr->self_hostname = old->self_hostname;
      addr->parent = old;
      addr->errors_address = old->errors_address;
      old->child_count++;
      old->domain = old_domain;  /* May have changed while routing */

      addr->next = addr_new;
      addr_new = addr;
      }

    /* Handle addresses that are finished with. */

    else if (rc != OK)
      (void)post_process_one(addr, rc, LOG_MAIN, DTYPE_ROUTER, 0);

    /* Routing was successful; the address has been put on an appropriate
    chain. Continue with routing the next address. */
    }
  }     /* Restart entire process if any remotes became local */


/* Free any resources that were cached during directing and routing. It's
debatable as to whether direct_tidyup() should be called immediately after
directing rather than waiting till routing is done. The point is that routing
can cause further directing if a "remote" domain turns out to be local.
However, as the resources are typically just open files (e.g. aliases) I don't
think it's critical. */

search_tidyup();
direct_tidyup();
route_tidyup();


/* If this is a run to continue deliveries to an external channel that is
already set up, defer any local deliveries. */

if (continue_transport != NULL)
  {
  if (addr_defer == NULL) addr_defer = addr_local; else
    {
    address_item *addr = addr_defer;
    while (addr->next != NULL) addr = addr->next;
    addr->next = addr_local;
    }
  addr_local = NULL;
  }


/* Because address rewriting can happen in the routers or directors, we should
not really do ANY deliveries until all addresses have been routed or directed,
so that all recipients of the message get the same headers. However, this is in
practice not always possible, since sometimes remote addresses give DNS
timeouts for days on end. The pragmatic approach is to deliver what we can now,
saving any rewritten headers so that at least the next lot of recipients
benefit from the rewriting that has already been done.

If any headers have been rewritten during routing or directing, update the
spool file to remember them for all subsequent deliveries. This can be delayed
till later if there is only address to be delivered - if it succeeds the spool
write need not happen. */

if (header_changed &&
    ((addr_local != NULL &&
       (addr_local->next != NULL || addr_remote != NULL)) ||
     (addr_remote != NULL && addr_remote->next != NULL)))
  {
  /* Panic-dies on error */
  (void)spool_write_header(message_id, SW_DELIVERING, NULL);
  header_changed = FALSE;
  }


/* The function that read the headers counted the number of Received: headers.
If there are too many, we must not make any deliveries. An earlier version
of Exim allowed local deliveries, but in fact these can sometimes be disguised
remote deliveries via some other means, so now we don't do any deliveries
at all. */

if (received_count > received_headers_max)
  {
  int i;
  address_item **anchor = &addr_remote;
  log_write(0, LOG_MAIN, "** Too many \"Received\" headers - "
    "suspected mail loop");

  for (i = 0; i < 2; anchor = &addr_local, i++)
    {
    while (*anchor != NULL)
      {
      address_item *addr = *anchor;
      *anchor = addr->next;
      addr->message = "Too many \"Received\" headers - suspected mail loop";
      addr->next = addr_failed;
      addr_failed = addr;
      }
    }
  }


/* If there are any deliveries to be done, open the journal file. This is used
to record successful deliveries as soon as possible after each delivery is
known to be complete. This can happen in subprocesses of this process for
parallel remote deliveries. A file opened with O_APPEND is used so that several
processes can run simultaneously.

The journal is just insurance against crashes. When the spool file is
ultimately updated at the end of processing, the journal is deleted. If a
journal is found to exist at the start of delivery, the addresses listed
therein are added to the non-recipients. */

if (addr_local != NULL || addr_remote != NULL)
  {
  sprintf(spoolname, "%s/input/%s/%s-J", spool_directory, message_subdir, id);
  journal_fd = open(spoolname, O_WRONLY|O_APPEND|O_CREAT, SPOOL_MODE);

  if (journal_fd < 0)
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "Couldn't open journal file %s: %s",
      spoolname, strerror(errno));
    return FAIL;
    }

  /* Set the close-on-exec flag */

  fcntl(journal_fd, F_SETFD, fcntl(journal_fd, F_GETFD) | FD_CLOEXEC);

  /* Make sure the file's group is the Exim gid if exim_uid exists (can't have
  exim_uid set without exim_gid), and double-check the mode because the group
  setting doesn't always get set automatically. */

  if (exim_uid_set)
    {
    fchown(journal_fd, exim_uid, exim_gid);
    fchmod(journal_fd, SPOOL_MODE);
    }
  }


/* Now we can get down to the business of actually doing deliveries. Local
deliveries are done first, then remote ones. If ever the problems of how to
handle fallback transports are figured out, this section can be put into a loop
for handling fallbacks, though the uid switching will have to be revised. */

/* There may be expansion lookups and string matches during local deliveries.
So free any cached resources afterwards, so as not to hold them during remote
deliveries. */

if (addr_local != NULL)
  {
  DEBUG(2) debug_printf(">>>>>> Local deliveries >>>>>>\n");
  do_local_deliveries();
  }

/* Once the local deliveries are done, we do not need to be root any longer,
so if a non-root uid has been specified, give up privileges for good at this
point, whatever the security level, using seteuid to recover root privilege
if it has been temporarily given up. */

if (exim_uid_set)
  {
  if (geteuid() != root_uid) mac_seteuid(root_uid);
  exim_setugid(exim_uid, exim_gid, "remote deliveries");
  }

/* Handle remote deliveries */

if (addr_remote != NULL)
  {
  DEBUG(2) debug_printf(">>>>>> Remote deliveries >>>>>>\n");

  /* Precompile some regex that are used to recognize parameters in response
  to an EHLO command, if they aren't already compiled. */

  if (regex_PIPELINING == NULL) regex_PIPELINING =
    regex_must_compile("\\n250[\\s\\-]PIPELINING(\\s|\\n|$)", FALSE, TRUE);

  if (regex_SIZE == NULL) regex_SIZE =
    regex_must_compile("\\n250[\\s\\-]SIZE(\\s|\\n|$)", FALSE, TRUE);

  #ifdef HAVE_AUTH
  if (regex_AUTH == NULL) regex_AUTH =
    regex_must_compile("\\n250[\\s\\-]AUTH\\s+([\\-\\w\\s]+)(?:\\n|$)",
      FALSE, TRUE);
  #endif

  #ifdef SUPPORT_TLS
  if (regex_STARTTLS == NULL) regex_STARTTLS =
    regex_must_compile("\\n250[\\s\\-]STARTTLS(\\s|\\n|$)", FALSE, TRUE);
  #endif

  #ifdef SUPPORT_DSN
  /*This partial attempt at doing DSN was abandoned. This obsolete code is
  left here just in case. Nothing is documented. */
  if (regex_DSN == NULL) regex_DSN  =
    regex_must_compile("\\n250[\\s\\-]DSN(\\s|\\n|$)", FALSE, TRUE);
  #endif

  /* Now sort the addresses if required, and do the deliveries. */

  if (remote_sort != NULL) sort_remote_deliveries();
  do_remote_deliveries(FALSE);

  /* See if any of the addresses that failed got put on the queue for delivery
  to their fallback hosts. We do it this way because often the same fallback
  host is used for many domains, so all can be sent in a single transaction
  (if appropriately configured). */

  if (addr_fallback != NULL)
    {
    DEBUG(1) debug_printf("Delivering to fallback hosts\n");
    addr_remote = addr_fallback;
    addr_fallback = NULL;
    if (remote_sort != NULL) sort_remote_deliveries();
    do_remote_deliveries(TRUE);
    }
  }


/* All deliveries are now complete. Ignore SIGTERM during this tidying up
phase, to minimize cases of half-done things. */

set_process_info("tidying up after delivering %s", message_id);
signal(SIGTERM, SIG_IGN);

/* First we update the retry database. This is done in one fell swoop at the
end in order not to keep opening and closing (and locking) the database. The
code for handling retries is hived off into a separate module for convenience.
We pass it the addresses of the various chains, because deferred addresses can
get moved onto the failed chain if the retry cutoff time has expired for all
alternative destinations. Bypass the updating of the database if the -N flag is
set, which is a debugging thing that prevents actual delivery. */

if (!dont_deliver) retry_update(&addr_defer, &addr_failed, &addr_succeed);

/* If any addresses failed, we must send a message to somebody, unless
af_ignore_error is set, in which case no action is taken. Simple delivery
failures go back to the sender; other cases may go to mailmaster or to an
address owner. It is possible for several messages to get sent if there are
addresses with different requirements. */

while (addr_failed != NULL)
  {
  pid_t pid;
  int fd;
  address_item *addr;
  address_item *handled_addr = NULL;
  address_item **paddr;
  address_item *msgchain = NULL;
  address_item **pmsgchain = &msgchain;

  errmsg_recipient = (addr_failed->errors_address == NULL)?
    ((sender_address[0] == 0)? errors_address : sender_address) :
    addr_failed->errors_address;

  /* If this is an error delivery message for which there is no other
  errors address, and it has been failed because of a retry timeout, ignore it.
  This case happens when a delivery error message times out after (auto)
  thawing, since the normal handling of failures for such messages is to defer
  and freeze them. */

  if (testflag(addr_failed, af_retry_timedout) &&
      sender_address[0] == 0 &&
      errmsg_recipient == errors_address)
    setflag(addr_failed, af_ignore_error);

  /* If the first address on the list has af_ignore_error set, just remove
  it from the list, throw away any saved message file, log it, and
  mark the recipient done. */

  if (testflag(addr_failed, af_ignore_error))
    {
    addr = addr_failed;
    addr_failed = addr->next;
    if (addr->return_filename != NULL) unlink(addr->return_filename);

    log_write(0, LOG_MAIN, "%s%s%s%s: error ignored",
      addr->orig,
      (addr->parent == NULL)? "" : " <",
      (addr->parent == NULL)? "" : addr->parent->orig,
      (addr->parent == NULL)? "" : ">");

    tree_add_nonrecipient(addr->unique, testflag(addr, af_pfr));
    /* Panic-dies on error */
    (void)spool_write_header(message_id, SW_DELIVERING, NULL);
    }

  /* Otherwise, handle the sending of a message. Find the error address for
  the first address, then send a message that includes all failed addresses
  that have the same error address. */

  else
    {
    /* Make a subprocess to send a message */

    pid = child_open_exim(&fd);

    /* Creation of child failed */

    if (pid < 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Process %d (parent %d) failed to "
        "create child process to send failure message: %s", getpid(),
        getppid(), strerror(errno));

    /* Creation of child succeeded */

    else
      {
      int ch, rc;
      int filecount = 0;
      int rcount = 0;
      char *bcc, *emf_text;
      FILE *f = fdopen(fd, "w");
      FILE *emf = NULL;
      BOOL to_sender = strcmpic(sender_address, errmsg_recipient) == 0;
      int max = (return_size_limit/DELIVER_IN_BUFFER_SIZE + 1) *
        DELIVER_IN_BUFFER_SIZE;

      DEBUG(9) debug_printf("sending error message to: %s\n", errmsg_recipient);

      /* Scan the addresses for all that have the same errors_address, removing
      them from the addr_failed chain, and putting them on msgchain. */

      paddr = &addr_failed;
      for (addr = addr_failed; addr != NULL; addr = *paddr)
        {
        if (strcmp(errmsg_recipient, (addr->errors_address == NULL)?
            ((sender_address[0] == 0)? errors_address : sender_address) :
              addr->errors_address) != 0 || testflag(addr, af_ignore_error))
          {
          paddr = &(addr->next);
          }
        else
          {
          *paddr = addr->next;
          *pmsgchain = addr;
          addr->next = NULL;
          pmsgchain = &(addr->next);
          }
        }

      /* Include X-Failed-Recipients: for automatic interpretation, but do
      not let any one header line get too long. We do this by starting a
      new header every 50 recipients. */

      for (addr = msgchain; addr != NULL; addr = addr->next)
        {
        if (rcount >= 50)
          {
          fprintf(f, "\n");
          rcount = 0;
          }
        fprintf(f, "%s%s",
          (rcount++ == 0)? "X-Failed-Recipients: " : ",\n  ",
          (testflag(addr, af_pfr) && addr->parent != NULL)?
            string_printing(addr->parent->orig) :
            string_printing(addr->orig));
        }
      fprintf(f, "\n");

      /* Output the standard headers */

      if (errors_reply_to != NULL)
        fprintf(f, "Reply-To: %s\n", errors_reply_to);
      fprintf(f, "From: Mail Delivery System <Mailer-Daemon@%s>\n",
        qualify_domain_sender);
      fprintf(f, "To: %s\n", errmsg_recipient);

      /* Open a template file if one is provided. Log failure to open, but
      carry on - default texts will be used. */

      if (errmsg_file != NULL)
        {
        emf = fopen(errmsg_file, "r");
        if (emf == NULL)
          log_write(0, LOG_MAIN|LOG_PANIC, "Failed to open %s for error "
            "message texts: %s", errmsg_file, strerror(errno));
        }

      /* Quietly copy to configured additional addresses if required. */

      bcc = moan_check_errorcopy(errmsg_recipient);
      if (bcc != NULL) fprintf(f, "Bcc: %s\n", bcc);

      /* The texts for the message can be read from a template file; if there
      isn't one, or if it is too short, built-in texts are used. The first
      emf text is a Subject: and any other headers. */

      emf_text = next_emf(emf, "header");
      if (emf_text != NULL) fprintf(f, "%s\n", emf_text); else
        {
        fprintf(f, "Subject: Mail delivery failed%s\n\n",
          to_sender? ": returning message to sender" : "");
        }

      emf_text = next_emf(emf, "intro");
      if (emf_text != NULL) fprintf(f, "%s", emf_text); else
        {
        fprintf(f,
"This message was created automatically by mail delivery software (Exim).\n");
        if (errmsg_text != NULL) fprintf(f, errmsg_text);
        if (to_sender)
          {
          fprintf(f,
"\nA message that you sent could not be delivered to one or more of its\n"
"recipients. This is a permanent error. The following address(es) failed:\n");
          }
        else
          {
          fprintf(f,
"\nA message sent by\n\n  <%s>\n\n"
"could not be delivered to one or more of its recipients. The following\n"
"address(es) failed:\n", sender_address);
          }
        }
      fprintf(f, "\n");

      /* Process the addresses, leaving them on the msgchain if they have a
      file name for a return message. (There has already been a check in
      post_process_one() for the existence of data in the message file.) */

      paddr = &msgchain;
      for (addr = msgchain; addr != NULL; addr = *paddr)
        {
        if (print_address_information(addr, f, "  ", "\n    ", ""))
          {
          /* A TRUE return from print_address_information() means that the
          address is not hidden. If there is a return file, it has already
          been checked to ensure it is not empty. Omit the bland "return
          message generated" error, but otherwise include error information. */

          if (addr->return_file < 0 ||
              addr->message == NULL ||
              strcmp(addr->message, "return message generated") != 0)
            {
            fprintf(f, "\n    ");
            print_address_error(addr, f);
            }
          }

        /* End the final line for the address */

        fputc('\n', f);

        /* Leave on msgchain if there's a return file. */

        if (addr->return_file >= 0)
          {
          paddr = &(addr->next);
          filecount++;
          }

        /* Else save so that we can tick off the recipient when the
        message is sent. */

        else
          {
          *paddr = addr->next;
          addr->next = handled_addr;
          handled_addr = addr;
          }
        }

      fprintf(f, "\n");

      /* Get the next text, whether we need it or not, so as to be
      positioned for the one after. */

      emf_text = next_emf(emf, "generated text");

      /* If there were any file messages passed by the local transports,
      include them in the message. Then put the address on the handled chain.
      In the case of a batch of addresses that were all sent to the same
      transport, the return_file field in all of them will contain the same
      fd, and the return_filename field in the *last* one will be set (to the
      name of the file). */

      if (msgchain != NULL)
        {
        address_item *nextaddr;

        if (emf_text != NULL) fprintf(f, "%s", emf_text); else
          fprintf(f,
            "The following text was generated during the delivery "
            "attempt%s:\n", (filecount > 1)? "s" : "");

        for (addr = msgchain; addr != NULL; addr = nextaddr)
          {
          FILE *fm;
          address_item *topaddr = addr;

          /* List all the addresses that relate to this file */

          fprintf(f, "\n");
          while(addr != NULL)                   /* Insurance */
            {
            print_address_information(addr, f, "------ ",  "\n       ",
              " ------\n");
            if (addr->return_filename != NULL) break;
            addr = addr->next;
            }
          fprintf(f, "\n");

          /* Now copy the file */

          fm = fopen(addr->return_filename, "r");

          if (fm == NULL)
            fprintf(f, "    +++ Exim error... failed to open text file: %s\n",
              strerror(errno));
          else
            {
            while ((ch = fgetc(fm)) != EOF) fputc(ch, f);
            fclose(fm);
            }
          unlink(addr->return_filename);

          /* Can now add to handled chain, first fishing off the next
          address on the msgchain. */

          nextaddr = addr->next;
          addr->next = handled_addr;
          handled_addr = topaddr;
          }
        fprintf(f, "\n");
        }

      /* Now copy the message, trying to give an intelligible comment if
      it is too long for it all to be copied. The limit isn't strictly
      applied because of the buffering. */

      emf_text = next_emf(emf, "copy");
      if (emf_text != NULL) fprintf(f, "%s", emf_text); else
        {
        fprintf(f,
"------ This is a copy of the message, including all the headers. ------\n");
        }

      /* While reading the "truncated" message, set return_size_limit to
      the actual max testing value, rounded. We need to read the message
      whether we are going to use it or not. */

        {
        int temp = return_size_limit;
        return_size_limit = (max/1000)*1000;
        emf_text = next_emf(emf, "truncated");
        return_size_limit = temp;
        }

      if (return_size_limit > 0)
        {
        struct stat statbuf;
        if (fstat(deliver_datafile, &statbuf) == 0 && statbuf.st_size > max)
          {
          if (emf_text != NULL) fprintf(f, "%s", emf_text); else
            {
            fprintf(f,
"------ The body of the message is %d characters long; only the first\n"
"------ %d or so are included here.\n", (int)statbuf.st_size,
              return_size_limit);
            }
          }
        }

      fprintf(f, "\n");
      fflush(f);
      transport_filter_argv = NULL;   /* Just in case */
      return_path = sender_address;   /* In case not previously set */
      transport_write_message(NULL, fileno(f), topt_add_return_path,
        return_size_limit, NULL, NULL, NULL, NULL, NULL, 0);

      /* Write final text and close the template file if one is open */

      if (emf != NULL)
        {
        emf_text = next_emf(emf, "final");
        if (emf_text != NULL) fprintf(f, "%s", emf_text);
        fclose(emf);
        }

      /* Close the file, which should send an EOF to the child process
      that is receiving the message. Wait for it to finish. */

      fclose(f);
      rc = child_close(pid, 0);     /* Waits for child to close, no timeout */

      /* If the process failed, there was some disaster in setting up the
      error message. Unless the message is very old, ensure that addr_defer
      is non-null, which will have the effect of leaving the message on the
      spool. The failed addresses will get tried again next time. However, we
      don't really want this to happen too often, so freeze the message unless
      there are some genuine deferred addresses to try. To do this we have
      to call spool_write_header() here, because with no genuine deferred
      addresses the normal code below doesn't get run. */

      if (rc != 0)
        {
        char *s = "";
        if (now - received_time < retry_maximum_timeout && addr_defer == NULL)
          {
          addr_defer = (address_item *)(+1);
          deliver_freeze = TRUE;
          deliver_frozen_at = time(NULL);
          /* Panic-dies on error */
          (void)spool_write_header(message_id, SW_DELIVERING, NULL);
          s = " (frozen)";
          }
        fprintf(message_log, "Process failed (%d) when writing error message "
          "to %s%s", rc, errmsg_recipient, s);
        log_write(0, LOG_MAIN, "Process failed (%d) when writing error message "
          "to %s%s", rc, errmsg_recipient, s);
        }

      /* The message succeeded. Ensure that the recipients that failed are
      now marked finished with on the spool and their parents updated. */

      else
        {
        char *logtod = tod_stamp(tod_log);
        log_write(5, LOG_MAIN, "Error message sent to %s", errmsg_recipient);
        for (addr = handled_addr; addr != NULL; addr = addr->next)
          {
          address_item *dup;

          tree_add_nonrecipient(addr->unique, testflag(addr, af_pfr));
          if (testflag(addr, af_local) && addr->parent == NULL)
            tree_add_nonrecipient(addr->orig, testflag(addr, af_pfr));
          (void)child_done(addr, logtod);

          /* Check the list of duplicate addresses and ensure they are now
          marked done as well. Also their parents. */

          for (dup = addr_duplicate; dup != NULL; dup = dup->next)
            {
            if (strcmp(addr->unique, dup->unique) == 0)
              {
              tree_add_nonrecipient(dup->orig, testflag(dup, af_pfr));
              (void)child_done(dup, logtod);
              }
            }
          }

        /* Panic-dies on error */
        (void)spool_write_header(message_id, SW_DELIVERING, NULL);
        }
      }
    }
  }

/* If there are now no deferred addresses, we are done. Preserve the
message log if so configured. */

if (addr_defer == NULL)
  {
  int rc;
  sprintf(spoolname, "%s/msglog/%s/%s", spool_directory, message_subdir, id);
  if (preserve_message_logs)
    {
    sprintf(big_buffer, "%s/msglog.OLD/%s", spool_directory, id);
    if ((rc = rename(spoolname, big_buffer)) < 0)
      {
      (void)directory_make(spool_directory, "msglog.OLD", MSGLOG_DIRECTORY_MODE,
        TRUE);
      rc = rename(spoolname, big_buffer);
      }
    if (rc < 0)
      log_write(0, LOG_PANIC_DIE, "failed to move %s to the msglog.OLD "
        "directory", spoolname);
    }
  else
    {
    if (unlink(spoolname) < 0)
      log_write(0, LOG_PANIC_DIE, "failed to unlink %s", spoolname);
    }

  /* Remove the two message files. */

  sprintf(spoolname, "%s/input/%s/%s-D", spool_directory, message_subdir, id);
  if (unlink(spoolname) < 0)
    log_write(0, LOG_PANIC_DIE, "failed to unlink %s", spoolname);
  sprintf(spoolname, "%s/input/%s/%s-H", spool_directory, message_subdir, id);
  if (unlink(spoolname) < 0)
    log_write(0, LOG_PANIC_DIE, "failed to unlink %s", spoolname);
  log_write(0, LOG_MAIN, "Completed");
  }

/* Otherwise, we are keeping this message because it is not yet completed. Lose
any temporary files that were catching output from pipes for any of the
deferred addresses, handle one-time aliases, and see if the message has been on
the queue for so long that it is time to send a warning message to the sender,
unless it is a mailer-daemon. If all deferred addresses have the same domain,
we can set deliver_domain for the expansion of delay_warning_ condition - if
any of them are pipes, files, or autoreplies, use the parent's domain.

If we can't make a process to send the message, don't worry.

For mailing list expansions we want to send the warning message to the
mailing list manager. We can't do a perfect job here, as some addresses may
have different errors addresses, but if we take the errors address from
each deferred address it will probably be right in most cases.

If addr_defer == +1, it means there was a problem sending an error message
for failed addresses, and there were no "real" deferred addresses. The value
was set just to keep the message on the spool, so there is nothing to do here.
*/

else if (addr_defer != (address_item *)(+1))
  {
  address_item *addr;
  char *recipients = "";

  deliver_domain = testflag(addr_defer, af_pfr)?
    addr_defer->parent->domain : addr_defer->domain;

  for (addr = addr_defer; addr != NULL; addr = addr->next)
    {
    address_item *otaddr;

    if (deliver_domain != NULL)
      {
      char *d = (testflag(addr, af_pfr))? addr->parent->domain : addr->domain;

      /* The domain may be unset for an address that has never been routed
      because the system filter froze the message. */

      if (d == NULL || strcmp(d, deliver_domain) != 0) deliver_domain = NULL;
      }

    if (addr->return_filename != NULL) unlink(addr->return_filename);

    /* Handle the case of one-time aliases. If any address in the ancestry
    of this one is flagged, ensure it is in the recipients list, suitably
    flagged, and that its parent is marked delivered. */

    for (otaddr = addr; otaddr != NULL; otaddr = otaddr->parent)
      if (otaddr->onetime_parent != NULL) break;

    if (otaddr != NULL)
      {
      int i;
      int t = recipients_count;

      for (i = 0; i < recipients_count; i++)
        {
        char *r = recipients_list[i].address;
        if (strcmp(otaddr->onetime_parent, r) == 0) t = i;
        if (strcmp(otaddr->orig, r) == 0) break;
        }

      /* Didn't find the address already in the list, and did find the
      ultimate parent's address in the list. */

      if (i >= recipients_count && t < recipients_count)
        {
        DEBUG(9) debug_printf("one_time: adding %s in place of %s\n",
          otaddr->orig, otaddr->parent->orig);
        accept_add_recipient(otaddr->orig, NULL, rf_onetime, t);
        tree_add_nonrecipient(otaddr->parent->orig, FALSE);
        update_spool = TRUE;
        }
      }

    /* Except for error messages, ensure that either the errors address for
    this deferred address or, if there is none, the sender address, is on the
    list of recipients for a warning message. */

    if (sender_address[0] != 0)
      {
      if (addr->errors_address == NULL)
        {
        if (strstr(recipients, sender_address) == NULL)
          recipients = string_sprintf("%s%s%s", recipients,
            (recipients[0] == 0)? "" : ",", sender_address);
        }
      else
        {
        if (strstr(recipients, addr->errors_address) == NULL)
          recipients = string_sprintf("%s%s%s", recipients,
            (recipients[0] == 0)? "" : ",", addr->errors_address);
        }
      }
    }

  /* Send a warning message if the conditions are right. If the condition check
  fails because of a lookup defer, there is nothing we can do. The warning
  is not sent. */

  if (!queue_2stage &&
      delay_warning[1] > 0 && sender_address[0] != 0 &&
       (delay_warning_condition == NULL ||
          expand_check_condition(delay_warning_condition,
            "delay_warning", "option")))
    {
    int count;
    int show_time;
    int queue_time = time(NULL) - received_time;

    for (count = 0; count < delay_warning[1]; count++)
      if (queue_time < delay_warning[count+2]) break;

    show_time = delay_warning[count+1];

    if (count >= delay_warning[1])
      {
      int extra;
      int last_gap = show_time;
      if (count > 1) last_gap -= delay_warning[count];
      extra = (queue_time - delay_warning[count+1])/last_gap;
      show_time += last_gap * extra;
      count += extra;
      }

    DEBUG(9)
      {
      debug_printf("time on queue = %s\n", readconf_printtime(queue_time));
      debug_printf("warning counts: required %d done %d\n", count,
        warning_count);
      }

    /* We have computed the number of warnings there should have been by now.
    If there haven't been enough, send one, and up the count to what it should
    have been. */

    if (warning_count < count)
      {
      header_line *h;
      int fd;
      pid_t pid = child_open_exim(&fd);

      if (pid > 0)
        {
        char *wmf_text;
        FILE *wmf = NULL;
        FILE *f = fdopen(fd, "w");

        if (warnmsg_file != NULL)
          {
          wmf = fopen(warnmsg_file, "r");
          if (wmf == NULL)
            log_write(0, LOG_MAIN|LOG_PANIC, "Failed to open %s for warning "
              "message texts: %s", warnmsg_file, strerror(errno));
          }

        warnmsg_recipients = recipients;
        warnmsg_delay = (queue_time < 120*60)?
          string_sprintf("%d minutes", show_time/60):
          string_sprintf("%d hours", show_time/3600);

        if (errors_reply_to != NULL)
          fprintf(f, "Reply-To: %s\n", errors_reply_to);
        fprintf(f, "From: Mail Delivery System <Mailer-Daemon@%s>\n",
          qualify_domain_sender);
        fprintf(f, "To: %s\n", recipients);

        wmf_text = next_emf(wmf, "header");
        if (wmf_text != NULL)
          fprintf(f, "%s\n", wmf_text);
        else
          fprintf(f, "Subject: Warning: message %s delayed %s\n\n",
            message_id, warnmsg_delay);

        wmf_text = next_emf(wmf, "intro");
        if (wmf_text != NULL) fprintf(f, "%s", wmf_text); else
          {
          fprintf(f,
"This message was created automatically by mail delivery software (Exim).\n\n");

          if (strcmp(recipients, sender_address) == 0)
            fprintf(f,
"A message that you sent has not yet been delivered to one or more of its\n"
"recipients after more than ");

          else fprintf(f,
"A message sent by\n\n  <%s>\n\n"
"has not yet been delivered to one or more of its recipients after more than \n",
          sender_address);

          fprintf(f, "%s on the queue on %s.\n\n", warnmsg_delay,
            primary_hostname);
          fprintf(f, "The message identifier is:     %s\n", message_id);

          for (h = header_list; h != NULL; h = h->next)
            {
            if (strncmpic(h->text, "Subject:", 8) == 0)
              fprintf(f, "The subject of the message is: %s", h->text + 9);
            else if (strncmpic(h->text, "Date:", 5) == 0)
              fprintf(f, "The date of the message is:    %s", h->text + 6);
            }
          fprintf(f, "\n");

          fprintf(f, "The address%s to which the message has not yet been "
            "delivered %s:\n",
            (addr_defer->next == NULL)? "" : "es",
            (addr_defer->next == NULL)? "is": "are");
          }

        /* List the addresses. For any that are hidden, don't give the delay
        reason, because it might expose that which is hidden. Also, do not give
        "retry time not reached" because that isn't helpful. */

        fprintf(f, "\n");
        while (addr_defer != NULL)
          {
          address_item *addr = addr_defer;
          addr_defer = addr->next;
          if (print_address_information(addr, f, "  ", "\n    ", "") &&
              addr->basic_errno > ERRNO_RETRY_BASE)
            {
            fprintf(f, "\n    Delay reason: ");
            print_address_error(addr, f);
            }
          fprintf(f, "\n");
          }
        fprintf(f, "\n");

        /* Final text */

        if (wmf != NULL)
          {
          wmf_text = next_emf(wmf, "final");
          if (wmf_text != NULL) fprintf(f, "%s", wmf_text);
          fclose(wmf);
          }
        else
          {
          fprintf(f,
"No action is required on your part. Delivery attempts will continue for\n"
"some time, and this warning may be repeated at intervals if the message\n"
"remains undelivered. Eventually the mail delivery software will give up,\n"
"and when that happens, the message will be returned to you.\n");
          }

        /* Close and wait for child process to complete, without a timeout.
        If there's an error, don't update the count. */

        fclose(f);
        if (child_close(pid, 0) == 0)
          {
          warning_count = count;
          update_spool = TRUE;    /* Ensure spool rewritten */
          }
        }
      }
    }

  /* Clear deliver_domain */

  deliver_domain = NULL;

  /* If this was a first delivery attempt, unset the first time flag, and
  ensure that the spool gets updated. */

  if (deliver_firsttime)
    {
    deliver_firsttime = FALSE;
    update_spool = TRUE;
    }

  /* If delivery was frozen, and the mailmaster wants to be told, generate an
  appropriate message, unless the message is a local error message - to prevent
  loops - or any message that is addressed to the local mailmaster. Then
  log the freezing. */

  if (deliver_freeze)
    {
    if (freeze_tell_mailmaster && !local_error_message)
      {
      BOOL moan = TRUE;
      int i;
      for (i = 0; i < recipients_count; i++)
        {
        if (strcmp(recipients_list[i].address, errors_address) == 0)
          {
          moan = FALSE;
          break;
          }
        }

      /* There may be instances of \n in frozen_info when a filter supplies
      some of the text, because newlines have been escaped for logging
      purposes. Turn then back into newlines for the error message. */

      if (moan)
        {
        char *s = string_copy(frozen_info);
        char *ss = s;
        while (*ss != 0)
          {
          if (*ss == '\\' && ss[1] == 'n')
            {
            *ss++ = ' ';
            *ss++ = '\n';
            }
          else ss++;
          }
        moan_tell_someone(errors_address, addr_defer, "Message frozen",
          "Message %s has been frozen%s.\nThe sender is <%s>.\n", message_id,
          s, sender_address);
        }
      }

    /* Log freezing just before we update the -H file, to minimize the chance
    of a race problem. */

    fprintf(message_log, "*** Frozen%s\n", frozen_info);
    fflush(message_log);
    log_write(0, LOG_MAIN, "Frozen%s", frozen_info);
    }

  /* If there have been any updates to the non-recipients list, or other things
  that get written to the spool, we must now update the spool header file so
  that it has the right information for the next delivery attempt. If there
  was more than one address being delivered, the header_change update is done
  earlier, in case one succeeds and then something crashes. */

  DEBUG(9)
    debug_printf("delivery deferred: update_spool=%d header_changed=%d\n",
      update_spool, header_changed);

  if (update_spool || header_changed)
    /* Panic-dies on error */
    (void)spool_write_header(message_id, SW_DELIVERING, NULL);
  }

/* Finished with the message log. If the message is complete, it will have
been unlinked or renamed above. */

fclose(message_log);

/* Now we can close and remove the journal file. Its only purpose is to record
successfully completed deliveries asap so that this information doesn't get
lost if Exim (or the machine) crashes. Forgetting about a failed delivery is
not serious, as trying it again is nor harmful. The journal might not be open
if all addresses were deferred at routing or directing. Nevertheless, we must
remove it if it exists (may have been lying around from a crash during the
previous delivery attempt). We don't remove the journal if a delivery
subprocess failed to pass back delivery information; this is controlled by
the remove_journal flag. When the journal is left, we also don't move the
message off the main spool if frozen and the option is set. It should get moved
at the next attempt, after the journal has been inspected. */

if (journal_fd >= 0) close(journal_fd);
if (remove_journal)
  {
  sprintf(spoolname, "%s/input/%s/%s-J", spool_directory, message_subdir, id);
  if (unlink(spoolname) < 0 && errno != ENOENT)
    log_write(0, LOG_PANIC_DIE, "failed to unlink %s: %s", spoolname,
      strerror(errno));

  /* Move the message off the spool if reqested */

  #ifdef SUPPORT_MOVE_FROZEN_MESSAGES
  if (deliver_freeze && move_frozen_messages)
    (void)spool_move_message(id, message_subdir, "", "F");
  #endif
  }

/* Closing the data file frees the lock; if the file has been unlinked it
will go away. Otherwise the message becomes available for another process
to try delivery. */

close(deliver_datafile);
deliver_datafile = -1;
DEBUG(2) debug_printf("end delivery of %s\n", id);

search_tidyup();  /* Release search resourses */
return OK;
}

/* End of deliver.c */
