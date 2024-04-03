/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "smtp.h"

#define PENDING          256
#define PENDING_DEFER   (PENDING + DEFER)
#define PENDING_OK      (PENDING + OK)


/* Options specific to the smtp transport. This transport also supports LMTP
over TCP/IP. The options must be in alphabetic order (note that "_" comes
before the lower case letters). Some live in the transport_instance block so as
to be publicly visible; these are flagged with opt_public. */

optionlist smtp_transport_options[] = {
  { "allow_localhost",      opt_bool,
      (void *)(offsetof(smtp_transport_options_block, allow_localhost)) },
  #ifdef HAVE_AUTH
  { "authenticate_hosts",   opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, authenticate_hosts)) },
  #endif
  { "batch_max",            opt_int,
      (void *)(offsetof(smtp_transport_options_block, batch_max)) },
  { "command_timeout",      opt_time,
      (void *)(offsetof(smtp_transport_options_block, command_timeout)) },
  { "connect_timeout",      opt_time,
      (void *)(offsetof(smtp_transport_options_block, connect_timeout)) },
  { "data_timeout",         opt_time,
      (void *)(offsetof(smtp_transport_options_block, data_timeout)) },
  { "delay_after_cutoff", opt_bool,
      (void *)(offsetof(smtp_transport_options_block, delay_after_cutoff)) },
  { "dns_qualify_single",   opt_bool,
      (void *)(offsetof(smtp_transport_options_block, dns_qualify_single)) },
  { "dns_search_parents",   opt_bool,
      (void *)(offsetof(smtp_transport_options_block, dns_search_parents)) },
  { "fallback_hosts",       opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, fallback_hosts)) },
  { "final_timeout",        opt_time,
      (void *)(offsetof(smtp_transport_options_block, final_timeout)) },
  { "gethostbyname",        opt_bool,
      (void *)(offsetof(smtp_transport_options_block, gethostbyname)) },
  { "helo_data",            opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, helo_data)) },
  { "hosts",                opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, hosts)) },
  #ifdef SUPPORT_TLS
  { "hosts_avoid_tls",      opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, hosts_avoid_tls)) },
  #endif
  { "hosts_max_try",        opt_int,
      (void *)(offsetof(smtp_transport_options_block, hosts_max_try)) },
  { "hosts_override",       opt_bool,
      (void *)(offsetof(smtp_transport_options_block, hosts_override)) },
  { "hosts_randomize",      opt_bool,
      (void *)(offsetof(smtp_transport_options_block, hosts_randomize)) },
  #ifdef SUPPORT_TLS
  { "hosts_require_tls",    opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, hosts_require_tls)) },
  #endif
  { "interface",            opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, interface)) },
  { "keepalive",            opt_bool,
      (void *)(offsetof(smtp_transport_options_block, keepalive)) },
  { "max_rcpt",             opt_int,
      (void *)(offsetof(smtp_transport_options_block, max_rcpt)) },
/***
  { "max_rcpt_serialize",   opt_bool,
      (void *)(offsetof(smtp_transport_options_block, max_rcpt_serialize)) },
***/

  { "multi_domain",         opt_bool | opt_public,
      (void *)(offsetof(transport_instance, multi_domain)) },
  { "mx_domains",           opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, mx_domains)) },
  { "port",                 opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, service)) },
  { "protocol",             opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, protocol)) },
  { "retry_include_ip_address", opt_bool,
      (void *)(offsetof(smtp_transport_options_block, retry_include_ip_address)) },
  { "serialize_hosts",      opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, serialize_hosts)) },
  { "service",              opt_stringptr | opt_hidden,
      (void *)(offsetof(smtp_transport_options_block, service)) },
  { "size_addition",        opt_int,
      (void *)(offsetof(smtp_transport_options_block, size_addition)) }
  #ifdef SUPPORT_TLS
 ,{ "tls_certificate",      opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, tls_certificate)) },
  { "tls_privatekey",       opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, tls_privatekey)) },
  { "tls_verify_certificates", opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, tls_verify_certificates)) },
  { "tls_verify_ciphers",   opt_stringptr,
      (void *)(offsetof(smtp_transport_options_block, tls_verify_ciphers)) }
  #endif
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int smtp_transport_options_count =
  sizeof(smtp_transport_options)/sizeof(optionlist);

/* Default private options block for the smtp transport. */

smtp_transport_options_block smtp_transport_option_defaults = {
  NULL,                /* hosts */
  NULL,                /* fallback_hosts */
  NULL,                /* hostlist */
  NULL,                /* fallback_hostlist */
  "$primary_hostname", /* helo_data */
  NULL,                /* interface */
  NULL,                /* service (i.e. port) */
  "smtp",              /* protocol */
  NULL,                /* mx_domains */
  NULL,                /* serialize_hosts */
  NULL,                /* authenticate_hosts */
  NULL,                /* hosts_require_tls */
  NULL,                /* hosts_avoid_tls */
  500,                 /* batch_max */
  5*60,                /* command_timeout */
  5*60,                /* connect_timeout; shorter system default overrides */
  5*60,                /* data timeout */
  10*60,               /* final timeout */
  1024,                /* size_addition */
  0,                   /* max_rcpt */
  5,                   /* hosts_max_try */
  FALSE,               /* allow_localhost */
  FALSE,               /* gethostbyname */
  TRUE,                /* dns_qualify_single */
  FALSE,               /* dns_search_parents */
  TRUE,                /* delay_after_cutoff */
  FALSE,               /* hosts_override */
  FALSE,               /* hosts_randomize */
  TRUE,                /* keepalive */
  FALSE,               /* max_rcpt_serialize */
  TRUE                 /* retry_include_ip_address */
  #ifdef SUPPORT_TLS
 ,NULL,                /* tls_certificate */
  NULL,                /* tls_privatekey */
  NULL,                /* tls_verify_certificates */
  NULL                 /* tls_verify_ciphers */
  #endif
};


/* Local statics */

static char *smtp_command;   /* Points to last cmd for error messages */
static char *mail_command;   /* Points to MAIL cmd for error messages */

#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is left
here just in case. Nothing is documented. */

static int   rf_list[] = {rf_notify_never, rf_notify_success,
  rf_notify_failure, rf_notify_delay };

static char *rf_names[] = { "NEVER", "SUCCESS", "FAILURE", "DELAY" };
#endif


/*************************************************
*             Setup entry point                  *
*************************************************/

/* This function is called when the transport is about to be used,
but before running it in a sub-process.

Arguments:
  tblock    pointer to the transport instance block
  addrlist  list of addresses about to be transported
  errmsg    place for error message

Returns:  OK always (FAIL, DEFER not used)
*/

static int
smtp_transport_setup(transport_instance *tblock, address_item *addrlist,
  char **errmsg)
{
smtp_transport_options_block *ob =
  (smtp_transport_options_block *)(tblock->options_block);

errmsg = errmsg;    /* Keep picky compilers happy */

/* Set the fallback host list for all the addresses that don't have fallback
host lists, provided that the local host wasn't present in the original host
list. */

if (!testflag(addrlist, af_local_host_removed))
  {
  for (; addrlist != NULL; addrlist = addrlist->next)
    if (addrlist->fallback_hosts == NULL)
      addrlist->fallback_hosts = ob->fallback_hostlist;
  }

return OK;
}



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up.

Argument:   pointer to the transport instance block
Returns:    nothing
*/

void
smtp_transport_init(transport_instance *tblock)
{
smtp_transport_options_block *ob =
  (smtp_transport_options_block *)(tblock->options_block);

/* Set the default port according to the protocol */

if (ob->service == NULL)
  ob->service = (strcmpic(ob->protocol, "lmtp") == 0)? "lmtp" : "smtp";

/* Set up the setup entry point, to be called before subprocesses for this
transport. */

tblock->setup = smtp_transport_setup;

/*** This bit of code isn't really right and consequently is excluded. The
interactions between an external address selection and an internal selection
need to be worked out properly some day. Meanwhile, just set the external max
addresses to the internal RCPT limit. ***/

tblock->max_addresses = ob->max_rcpt;

#ifdef NEVER
/* If max_rcpt is set and max_rcpt_serialize is not set, copy the value into
the max_addresses field in the public transport block so that we never get
more than that many addresses at once and others can be sent in parallel.
Otherwise, if batch_max is set, put an outer limit on that will ....
NEEDS MORE THOUGHT. */

if (ob->max_rcpt > 0)
  {
  if (!ob->max_rcpt_serialize)
    tblock->max_addresses = ob->max_rcpt;
  else if (ob->batch_max > 0)
    tblock->max_addresses = ob->max_rcpt * ob->batch_max;
  }
#endif


/* Complain if any of the timeouts are zero. */

if (ob->command_timeout <= 0 || ob->data_timeout <= 0 ||
    ob->final_timeout <= 0)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "command, data, or final timeout value is zero for %s transport",
      tblock->name);

/* If hosts_override is set and there are local hosts, set the global
flag that stops verify from showing router hosts. */

if (ob->hosts_override && ob->hosts != NULL) tblock->overrides_hosts = TRUE;

/* If there are any fallback hosts listed, build a chain of host items
for them, but do not do any lookups at this time. */

host_build_hostlist(&(ob->fallback_hostlist), ob->fallback_hosts, FALSE);

/* If an interface is set, check that it is a valid IP address. */

if (ob->interface != NULL && string_is_ip_address(ob->interface, NULL) == 0)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "\"%s\" is not a valid IP address for the \"interface\" option of the "
    "%s transport", ob->interface, tblock->name);
}





/*************************************************
*   Set delivery info into all active addresses  *
*************************************************/

/* Only addresses whose status is >= PENDING are relevant. A lesser
status means that an address is not currently being processed.

Arguments:
  addrlist     points to a chain of addresses
  errno_value  to put in each address's errno field
  msg          to put in each address's message field
  rc           to put in each address's transport_return field

Returns:       nothing
*/

static
void set_errno(address_item *addrlist, int errno_value, char *msg, int rc)
{
address_item *addr;
for (addr = addrlist; addr != NULL; addr = addr->next)
  {
  if (addr->transport_return < PENDING) continue;
  addr->basic_errno = errno_value;
  if (msg != NULL) addr->message = msg;
  addr->transport_return = rc;
  }
}



/*************************************************
*          Check an SMTP response                *
*************************************************/

/* This function is given an errno code and the SMTP response buffer
to analyse, together with the host identification for generating messages. It
sets an appropriate message and puts the first digit of the response code into
the yield variable. If no response was actually read, a suitable digit is
chosen.

Arguments:
  host         the current host, to get its name for messages
  errno_value  pointer to the errno value
  more_errno   from the top address for use with ERRNO_FILTER_FAIL
  buffer       the SMTP response buffer
  yield        where to put a one-digit SMTP response code
  message      where to put an errror message

Returns:       TRUE if an SMTP "QUIT" command should be sent, else FALSE
*/

static BOOL check_response(host_item *host, int *errno_value, int more_errno,
  uschar *buffer, int *yield, uschar **message)
{
*yield = '4';    /* Default setting is to give a temporary error */

/* Handle response timeout */

if (*errno_value == ETIMEDOUT)
  {
  *message = US string_sprintf("SMTP timeout while connected to %s [%s] "
    "after %s", host->name, host->address, smtp_command);
  if (transport_count > 0)
    *message = US string_sprintf("%s (%d bytes written)", *message,
      transport_count);
  return FALSE;
  }

/* Handle malformed SMTP response */

if (*errno_value == ERRNO_SMTPFORMAT)
  {
  *message = US string_sprintf("Malformed SMTP response from %s [%s] after %s: %s",
    host->name, host->address, smtp_command, string_printing(CS buffer));
  return FALSE;
  }

/* Handle a failed filter process error; can't send QUIT as we mustn't
end the DATA. */

if (*errno_value == ERRNO_FILTER_FAIL)
  {
  *message = US string_sprintf("delivery filter process failed (%d)", more_errno);
  return FALSE;
  }

/* Handle a failed add_headers expansion; can't send QUIT as we mustn't
end the DATA. */

if (*errno_value == ERRNO_CHHEADER_FAIL)
  {
  *message =
    US string_sprintf("failed to expand headers_add or headers_remove: %s",
      expand_string_message);
  return FALSE;
  }

/* Handle failure to write a complete data block */

if (*errno_value == ERRNO_WRITEINCOMPLETE)
  {
  *message = US string_sprintf("failed to write a data block");
  return FALSE;
  }

/* Handle error responses from the remote mailer. */

if (buffer[0] != 0)
  {
  char *s = string_printing(CS buffer);
  *message = US string_sprintf("SMTP error from remote mailer after %s: "
    "host %s [%s]: %s", smtp_command, host->name, host->address, s);
  *yield = buffer[0];
  return TRUE;
  }

/* No data was read. If there is no errno, this must be the EOF (i.e.
connection closed) case, which causes deferral. Otherwise, put the host's
identity in the message, leaving the errno value to be interpreted as well. In
all cases, we have to assume the connection is now dead. */

if (*errno_value == 0)
  {
  *errno_value = ERRNO_SMTPCLOSED;
  *message = US string_sprintf("Remote host %s [%s] closed connection after %s",
    host->name, host->address, smtp_command);
  }
else *message = US string_sprintf("%s [%s]", host->name, host->address);

return FALSE;
}



/*************************************************
*          Write error message to logs           *
*************************************************/

/* This writes to the main log and to the message log.

Arguments:
  addr     the address item containing error information
  host     the current host

Returns:   nothing
*/

static void
write_logs(address_item *addr, host_item *host)
{
if (addr->message != NULL)
  {
  char *message = addr->message;
  if (addr->basic_errno > 0)
    message = string_sprintf("%s: %s", message, strerror(addr->basic_errno));
  log_write(0, LOG_MAIN, "%s", message);
  fprintf(message_log, "%s %s\n", tod_stamp(tod_log), message);
  }
else
  {
  log_write(0, LOG_MAIN, "%s [%s]: %s",
    host->name,
    host->address,
    strerror(addr->basic_errno));
  fprintf(message_log, "%s %s [%s]: %s\n",
    tod_stamp(tod_log),
    host->name,
    host->address,
    strerror(addr->basic_errno));
  }
fflush(message_log);
}



/*************************************************
*           Synchronize SMTP responses           *
*************************************************/

/* This function is called from smtp_deliver() to receive SMTP responses from
the server, and match them up with the commands to which they relate. When
PIPELINING is not in use, it is called after every command, and is therefore
somewhat over-engineered, but it is simpler to use a single scheme that works
both with and without PIPELINING instead of having two separate sets of code.

The set of commands that are buffered up with pipelining may start with MAIL
and may end with DATA; in between are RCPT commands that correspond to the
addresses whose status is PENDING_DEFER. All other commands (STARTTLS, AUTH,
etc.) are never buffered.

Errors after MAIL or DATA abort the whole process leaving the response in the
buffer. After MAIL, pending responses are flushed, and the original command is
re-instated in big_buffer for error messages. For RCPT commands, the remote is
permitted to reject some recipient addresses while accepting others. However
certain errors clearly abort the whole process. Set the value in
transport_return to PENDING_OK if the address is accepted. If there is a
subsequent general error, it will get reset accordingly. If not, it will get
converted to OK at the end.

Arguments:
  addrlist      the complete address list
  sync_addr     ptr to the ptr of the one to start scanning at (updated)
  host          the host we are connected to
  count         the number of responses to read
  pending_MAIL  true if the first response is for MAIL
  pending_DATA  0 if last command sent was not DATA
               +1 if previously had a good recipient
               -1 if not previously had a good recipient
  inblock       incoming SMTP block
  timeout       timeout value
  buffer        buffer for reading response
  buffsize      size of buffer

Returns:      3 if at least one address had 2xx and one had 5xx
              2 if at least one address had 5xx but none had 2xx
              1 if at least one host had a 2xx response, but none had 5xx
              0 no address had 2xx or 5xx but no errors (all 4xx, or just DATA)
             -1 timeout while reading RCPT response
             -2 I/O or other non-response error for RCPT
             -3 DATA or MAIL failed - errno and buffer set
*/

static int
sync_responses(address_item *addrlist, address_item **sync_addr,
  host_item *host, int count, BOOL pending_MAIL, int pending_DATA,
  smtp_inblock *inblock, int timeout, uschar *buffer, int buffsize)
{
address_item *addr = *sync_addr;
int yield = 0;

/* Handle the response for a MAIL command. On error, unless it was the only
command, reinstate the original in big_buffer for error message use, and flush
any further pending responses before returning, except after I/O errors and
timeouts. */

if (pending_MAIL)
  {
  count--;
  if (!smtp_read_response(inblock, buffer, buffsize, '2', timeout))
    {
    if (count != 0 && errno == 0 && buffer[0] != 0)
      {
      uschar flushbuffer[4096];
      while (count-- > 0)
        {
        if (!smtp_read_response(inblock, flushbuffer, sizeof(flushbuffer),
                   '2', timeout)
            && (errno != 0 || flushbuffer[0] == 0))
          break;
        }
      strcpy(big_buffer, mail_command);  /* Fits, because it came from there! */
      }
    return -3;
    }
  }

if (pending_DATA) count--;  /* Number of RCPT responses to come */

/* Read and handle the required number of RCPT responses, matching each one up
with an address by scanning for the next address whose status is PENDING_DEFER.
*/

while (count-- > 0)
  {
  while (addr->transport_return != PENDING_DEFER) addr = addr->next;

  /* The address was accepted */

  if (smtp_read_response(inblock, buffer, buffsize, '2', timeout))
    {
    yield |= 1;
    addr->transport_return = PENDING_OK;

    /* If af_dr_retry_exists is set, there was a routing or directing
    delay on this address; ensure that any address-specific retry record is
    expunged. */

    if (testflag(addr, af_dr_retry_exists))
      retry_add_item(addr, testflag(addr, af_local)? "D" : "R", TRUE, NULL,
        rf_delete);
    }

  /* Timeout while reading the response */

  else if (errno == ETIMEDOUT)
    {
    int save_errno = errno;
    char *message = string_sprintf("SMTP timeout while connected to %s [%s] "
      "after RCPT TO:<%s@%s>", host->name, host->address, addr->local_part,
      addr->domain);
    set_errno(addrlist, save_errno, message, DEFER);
    retry_add_item(addr, testflag(addr, af_local)? "D" : "R", TRUE, NULL, 0);
    host->update_waiting = FALSE;
    return -1;
    }

  /* Handle other errors in obtaining an SMTP response by returning -1. This
  will cause all the addresses to be deferred. */

  else if (errno != 0 || buffer[0] == 0) return -2;

  /* Handle SMTP permanent and temporary response codes. */

  else
    {
    addr->message =
      string_sprintf("SMTP error from remote mailer after RCPT TO:<%s@%s>: "
        "host %s [%s]: %s", addr->local_part, addr->domain,
        host->name, host->address, string_printing(CS buffer));
    fprintf(message_log, "%s %s\n", tod_stamp(tod_log), addr->message);

    /* The response was 5xx */

    if (buffer[0] == '5')
      {
      addr->transport_return = FAIL;
      yield |= 2;
      }

    /* The response was 4xx */

    else
      {
      /* Log temporary errors if there are more hosts to be tried. */
      if (host->next != NULL) log_write(0, LOG_MAIN, "%s", addr->message);
      addr->transport_return = DEFER;

      /* Do not put this message on the list of those waiting for this host,
      as otherwise it is likely to be tried too often. */

      host->update_waiting = FALSE;

      /* Add a retry item for the address so that it doesn't get tried
      again too soon. */

      retry_add_item(addr, testflag(addr, af_local)? "D" : "R", TRUE, NULL, 0);
      }
    }
  }       /* Loop for next RCPT response */

/* Update where to start at for the next block of responses, unless we
have already handled all the addresses. */

if (addr != NULL) *sync_addr = addr->next;

/* Handle a response to DATA. If we have not had any good recipients, either
previously or in this block, the response is ignored. */

if (pending_DATA != 0 &&
    !smtp_read_response(inblock, buffer, buffsize, '3', timeout) &&
    (pending_DATA > 0 || (yield & 1) != 0))
  return -3;

/* All responses read and handled; MAIL (if present) received 2xx and DATA (if
present) received 3xx. If any RCPTs were handled and yielded anything other
than 4xx, yield will be set non-zero. */

return yield;
}



/*************************************************
*       Deliver address list to given host       *
*************************************************/

/* If continue_hostname is not null, we get here only when continuing to
deliver down an existing channel. The channel was passed as the standard
input.

Otherwise, we have to make a connection to the remote host, and do the
initial protocol exchange.

Arguments:
  addrlist        chain of potential addresses to deliver; only those whose
                  transport_return field is set to PENDING_DEFER are currently
                  being processed; others should be skipped - they have either
                  been delivered to an earlier host or IP address, or been
                  failed by one of them.
  host            host to deliver to
  port            TCP/IP port to use, in network byte order
  tblock          transport instance block
  copy_host       TRUE if host set in addr->transported must be copied, because
                    it is specific to this call of the transport
  keepalive       TRUE if keepalive should be set on the socket
  message_defer   set TRUE if yield is OK, but all addresses were deferred
                    because of a non-recipient, non-host failure, that is, a
                    4xx response to MAIL FROM, DATA, or ".". This is a defer
                    that is specific to the message.

Returns:          OK    - the connection was made and the delivery attempted;
                          the result for each address is in its data block.
                  DEFER - the connection could not be made, or something failed
                          while setting up the SMTP session, or there was a
                          non-message-specific error, such as a timeout.
                  ERROR - a filter command is specified for this transport,
                          and there was a problem setting it up; OR helo_data
                          or add_headers is specified for this transport, and
                          the string failed to expand
*/

static int
smtp_deliver(address_item *addrlist, host_item *host, int port,
  transport_instance *tblock, BOOL copy_host, BOOL keepalive,
  BOOL *message_defer)
{
address_item *addr;
address_item *first_addr = addrlist;
address_item *sync_addr = addrlist;
int yield = OK;
int address_count;
int save_errno;
int rc;
smtp_transport_options_block *ob =
  (smtp_transport_options_block *)(tblock->options_block);
BOOL lmtp = strcmpic(ob->protocol, "lmtp") == 0;
BOOL ok = FALSE;
BOOL send_rset = TRUE;
BOOL send_quit = TRUE;
BOOL setting_up = TRUE;
BOOL completed_address = FALSE;
BOOL esmtp = TRUE;
BOOL pending_MAIL;
smtp_inblock inblock;
smtp_outblock outblock;
int max_rcpt = ob->max_rcpt;
uschar *message = NULL;
uschar new_message_id[MESSAGE_ID_LENGTH + 1];
uschar *p;
uschar buffer[4096];
uschar inbuffer[4096];
uschar outbuffer[1024];

*message_defer = FALSE;
smtp_command = "initial connection";
if (max_rcpt == 0) max_rcpt = 999999;

/* Set up the buffer for reading SMTP response packets. */

inblock.buffer = inbuffer;
inblock.buffersize = sizeof(inbuffer);
inblock.ptr = inbuffer;
inblock.ptrend = inbuffer;

/* Set up the buffer for holding SMTP commands while pipelining */

outblock.buffer = outbuffer;
outblock.buffersize = sizeof(outbuffer);
outblock.ptr = outbuffer;
outblock.cmd_count = 0;

/* Make a connection to the host if this isn't a continued delivery, and handle
the initial interaction and HELO/EHLO/LHLO. */

if (continue_hostname == NULL)
  {
  char *helo_data;

  #ifdef SUPPORT_TLS
  tls_offered = FALSE;
  #endif

  inblock.sock = outblock.sock =
    smtp_connect(host, port, ob->interface, ob->connect_timeout, keepalive);
  if (inblock.sock < 0)
    {
    set_errno(addrlist, errno, NULL, DEFER);
    return DEFER;
    }

  /* The first thing is to wait for an initial OK response. The dreaded "goto"
  is nevertheless a reasonably clean way of programming this kind of logic,
  where you want to escape on any error. */

  if (!smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
    ob->command_timeout)) goto RESPONSE_FAILED;

/** Debugging without sending a message
addrlist->transport_return = DEFER;
goto SEND_QUIT;
**/

  /* Errors that occur after this point follow an SMTP command, which is
  left in big_buffer by smtp_write_command() for use in error messages. */

  smtp_command = big_buffer;

  /* Tell the remote who we are...

  February 1998: A convention has evolved that ESMTP-speaking MTAs include the
  string "ESMTP" in their greeting lines, so make Exim send EHLO if the
  greeting is of this form. The assumption was that the far end supports it
  properly... but experience shows that there are some that give 5xx responses,
  even though the banner includes "ESMTP" (there's a bloody-minded one that
  says "ESMTP not spoken here"). Cope with that case.

  September 2000: Time has passed, and it seems reasonable now to always send
  EHLO at the start. It is also convenient to make the change while installing
  the TLS stuff.

  Exim originally sent "Helo" at this point and ran for nearly a year that way.
  Then somebody tried it with a Microsoft mailer... It seems that all other
  mailers use upper case for some reason (the RFC is quite clear about case
  independence) so, for peace of mind, I gave in. */

  deliver_host = host->name;
  deliver_host_address = host->address;
  helo_data = expand_string(ob->helo_data);
  deliver_host = deliver_host_address = NULL;

  if (helo_data == NULL)
    {
    char *message = string_sprintf("failed to expand helo_data: %s",
      expand_string_message);
    set_errno(addrlist, 0, message, DEFER);
    yield = ERROR;
    goto SEND_QUIT;
    }

  if (!smtp_write_command(&outblock, FALSE, "%s %s\r\n", lmtp? "LHLO" : "EHLO",
        helo_data))
    goto SEND_FAILED;

  if (!smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
       ob->command_timeout))
    {
    if (errno != 0 || buffer[0] == 0 || lmtp) goto RESPONSE_FAILED;
    if (!smtp_write_command(&outblock, FALSE, "HELO %s\r\n", primary_hostname))
      goto SEND_FAILED;
    if (!smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
      ob->command_timeout)) goto RESPONSE_FAILED;
    esmtp = FALSE;
    }

  /* Set tls_offered if the response to EHLO specifies support for STARTTLS. */

  #ifdef SUPPORT_TLS
  tls_offered = esmtp &&
    pcre_exec(regex_STARTTLS, NULL, buffer, (int)strlen(buffer), 0,
      PCRE_EOPT, NULL, 0) >= 0;
  #endif
  }

/* For continuing deliveries down the same channel, the socket is the standard
input, and we don't need to redo EHLO here (but may need to do so for TLS - see
below). Set up the pointer to where subsequent commands will be left, for
error messages. Note that smtp_use_size and smtp_use_pipelining will have been
set from the command line if they were set in the process that passed the
connection on. */

else
  {
  inblock.sock = outblock.sock = fileno(stdin);
  smtp_command = big_buffer;
  }

/* If TLS is available on this connection, whether continued or not, attempt to
start up a TLS session, unless the host is in hosts_avoid_tls. If successful,
send another EHLO - the server may give a different answer in secure mode. We
use a separate buffer for reading the response to STARTTLS so that if it is
negative, the original EHLO data is available for subsequent analysis, should
the client not be required to use TLS. If the response is bad, copy the buffer
for error analysis. */

#ifdef SUPPORT_TLS
if (tls_offered &&
      !verify_check_this_host(&(ob->hosts_avoid_tls), FALSE, host->name,
        host->address, NULL))
  {
  uschar buffer2[4096];
  if (!smtp_write_command(&outblock, FALSE, "STARTTLS\r\n")) goto SEND_FAILED;

  /* If there is an I/O error, or if there is a temporary failure of
  the STARTTLS command, transmission of this message is deferred. If there is
  an outright rejection of STARTTLS, we proceed to try to send the message in
  clear, unless the host is in hosts_require_tls (tested below). */

  if (!smtp_read_response(&inblock, buffer2, sizeof(buffer2), '2',
      ob->command_timeout))
    {
    strncpy(buffer, buffer2, sizeof(buffer));
    if (errno != 0 || buffer2[0] == 0 || buffer2[0] == '4')
      goto RESPONSE_FAILED;
    }

  /* STARTTLS accepted: try to negotiate a TLS session. Make the host
  name and address available for use in expansions. */

  else
    {
    BOOL ok;

    deliver_host = host->name;
    deliver_host_address = host->address;
    ok = tls_client_start(inblock.sock, host, addrlist,
      NULL,                    /* No DH param */
      ob->tls_certificate,
      ob->tls_privatekey,
      ob->tls_verify_certificates,
      ob->tls_verify_ciphers,
      ob->command_timeout);
    deliver_host = deliver_host_address = NULL;

    if (!ok)
      {
      save_errno = ERRNO_TLSFAILURE;
      message = "failure while setting up TLS session";
      send_quit = FALSE;
      goto FAILED;
      }

    for (addr = addrlist; addr != NULL; addr = addr->next)
      {
      addr->cipher = tls_cipher;
      addr->peerdn = tls_peerdn;
      }
    }
  }

/* If we started TLS, re-do the EHLO exchange over the secure channel. */

if (tls_active >= 0)
  {
  if (!smtp_write_command(&outblock, FALSE, "EHLO %s\r\n", primary_hostname))
    goto SEND_FAILED;
  if (!smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
       ob->command_timeout))
    goto RESPONSE_FAILED;
  }

/* If the host is required to use a secure channel, ensure that we
have one. */

else if (verify_check_this_host(&(ob->hosts_require_tls), TRUE, host->name,
          host->address, NULL))
  {
  save_errno = ERRNO_TLSREQUIRED;
  message = string_sprintf("a TLS session is required for %s [%s], but %s",
    host->name, host->address,
    tls_offered? "an attempt to start TLS failed" :
                 "the server did not offer TLS support");
  goto FAILED;
  }
#endif

/* If TLS is active, we have just started it up and re-done the EHLO command,
so its response needs to be analyzed. If TLS is not active and this is a
continued session down a previously-used socket, we haven't just done EHLO, so
we skip this. */

if (continue_hostname == NULL
    #ifdef SUPPORT_TLS
    || tls_active >= 0
    #endif
    )
  {
  /* If the response to EHLO specified support for the SIZE parameter, note
  this, provided size_addition is non-negative. */

  smtp_use_size = esmtp && ob->size_addition >= 0 &&
    pcre_exec(regex_SIZE, NULL, CS buffer, (int)strlen(CS buffer), 0,
      PCRE_EOPT, NULL, 0) >= 0;

  /* Note whether the server supports PIPELINING */

  smtp_use_pipelining = esmtp &&
    pcre_exec(regex_PIPELINING, NULL, CS buffer, (int)strlen(CS buffer), 0,
      PCRE_EOPT, NULL, 0) >= 0;

  /* Note if the response to EHLO specifies support for DSN, but only if
  configured to support DSN. This partial attempt at doing DSN was abandoned.
  This obsolete code is left here just in case. Nothing is documented. */

  #ifdef SUPPORT_DSN
  smtp_use_dsn = dsn &&
    esmtp && pcre_exec(regex_DSN, NULL, buffer, (int)strlen(buffer), 0,
      PCRE_EOPT, NULL, 0) >= 0;
  DEBUG(9) debug_printf("use_dsn=%d\n", smtp_use_dsn);
  #endif

  /* Note if the response to EHLO specifies support for the AUTH extension,
  but only if built with AUTH support. If it has, check that this host is one
  we want to authenticate to, and do the business. The host name and address
  must be available when the authenticator's client driver is running. */

  #ifdef HAVE_AUTH
  smtp_authenticated = FALSE;
  if (esmtp && regex_match_and_setup(regex_AUTH, CS buffer, 0, -1))
    {
    char *names = string_copyn(expand_nstring[1], expand_nlength[1]);
    expand_nmax = -1;                          /* reset */

    /* Must not do this check until after we have saved the result of the
    regex match above. */

    if (verify_check_this_host(&(ob->authenticate_hosts), FALSE,
          host->name, host->address, NULL))
      {
      auth_instance *au;

      DEBUG(9) debug_printf("scanning authentication mechanisms\n");

      /* Scan the configured authenticators looking for one which is configured
      for use as a client and whose name matches an authentication mechanism
      supported by the server. If one is found, attempt to authenticate by
      calling its client function. */

      for (au = auths; !smtp_authenticated && au != NULL; au = au->next)
        {
        char *p = names;
        if (!au->client) continue;

        /* Loop to scan supported server mechanisms */

        while (*p != 0)
          {
          int rc;
          int len = (int)strlen(au->public_name);
          while (isspace((uschar)*p)) p++;

          if (strncmpic(au->public_name, p, len) != 0 ||
              (p[len] != 0 && !isspace((uschar)p[len])))
            {
            while (*p != 0 && !isspace((uschar)*p)) p++;
            continue;
            }

          /* Found data for a listed mechanism. Call its client entry. Make
          the server's name and address available for use in expansions. */

          deliver_host = host->name;
          deliver_host_address = host->address;
          rc = (au->info->clientcode)(au, &inblock, &outblock,
            ob->command_timeout, CS buffer, sizeof(buffer));
          deliver_host = deliver_host_address = NULL;

          DEBUG(9) debug_printf("%s authenticator yielded %d\n",
            au->name, rc);

          /* A temporary authentication failure must hold up delivery to
          this host. After a permanent authentication failure, we might as
          well carry on to try other authentication methods, and if all fail
          hard, try to deliver the message unauthenticated. */

          switch(rc)
            {
            case OK:
            smtp_authenticated = TRUE;   /* stops the outer loop */
            break;

            /* Failure after writing a command */

            case FAIL_SEND:
            goto SEND_FAILED;

            /* Failure after reading a response */

            case FAIL:
            if (errno != 0 || buffer[0] != '5') goto RESPONSE_FAILED;
            log_write(0, LOG_MAIN, "%s authenticator failed H=%s [%s] %s",
              au->name, host->name, host->address, buffer);
            break;

            /* Failure by some other means. In effect, the authenticator
            decided it wasn't prepared to handle this case. Typically this
            is the result of "fail" in an expansion string. Do we need to
            log anything here? */

            case FORCEFAIL:
            break;

            /* Internal problem, message in buffer. */

            case ERROR:
            yield = ERROR;
            set_errno(addrlist, 0, string_copy(CS buffer), yield);
            goto SEND_QUIT;
            }

          break;  /* If not authenticated, try next authenticator */
          }       /* Loop for scanning supported mechanisms */
        }         /* Loop for further authenticators */
      }
    }
  #endif
  }

/* The setting up of the SMTP call is now complete. Any subsequent errors are
message-specific. */

setting_up = FALSE;

/* If there is a filter command specified for this transport, we can now
set it up. This cannot be done until the identify of the host is known. */

if (tblock->filter_command != NULL)
  {
  BOOL rc;
  char buffer[64];
  sprintf(buffer, "%.50s transport", tblock->name);
  deliver_host = host->name;
  deliver_host_address = host->address;
  rc = transport_set_up_command(&transport_filter_argv, tblock->filter_command,
    TRUE, DEFER, addrlist, buffer, NULL);
  deliver_host = deliver_host_address = NULL;

  /* On failure, copy the error to all addresses, abandon the SMTP call, and
  yield ERROR. */

  if (!rc)
    {
    set_errno(addrlist->next, addrlist->basic_errno, addrlist->message,
      DEFER);
    yield = ERROR;
    goto SEND_QUIT;
    }
  }


/* For messages that have more than the maximum number of recipients, we want
to send several transactions down the same SMTP connection. This optimization
was added to Exim after the following code was already working. The simplest
way to put it in without disturbing the code was to use a goto to jump back to
this point when there is another transaction to handle.

********
This facility isn't currently used because the external max_addresses value is
set to the internal max_rcpt value. However, the coding is left in situ because
it may be needed one day.
********
*/

SEND_MESSAGE:
address_count = 0;
ok = FALSE;
send_rset = TRUE;
completed_address = FALSE;


/* Initiate a message transfer. If we know the receiving MTA supports the SIZE
qualification, send it, adding something to the message size to allow for
imprecision and things that get added en route. Exim keeps the number of lines
in a message, so we can give an accurate value for the original message, but we
need some additional to handle added headers. (Double "." characters don't get
included in the count.) */

p = buffer;
*p = 0;

if (smtp_use_size)
  {
  sprintf(CS p, " SIZE=%d", message_size+message_linecount+ob->size_addition);
  while (*p) p++;
  }

#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is left
here just in case. Nothing is documented. */

if (smtp_use_dsn)
  {
  if (dsn_ret == dsn_ret_hdrs)
    {
    strcpy(p, " RET=HDRS");
    while (*p) p++;
    }
  else if (dsn_ret == dsn_ret_full)
    {
    strcpy(p, " RET=FULL");
    while (*p) p++;
    }
  if (dsn_envid != NULL)
    string_format(p, sizeof(buffer) - (p-buffer), " ENVID=%s", dsn_envid);
  }
#endif

/* Add the authenticated sender address if present */

#ifdef HAVE_AUTH
if (smtp_authenticated && authenticated_sender != NULL)
  {
  string_format(CS p, sizeof(buffer) - (p-buffer), " AUTH=%s",
    auth_xtextencode(authenticated_sender, (int)strlen(authenticated_sender)));
  }
#endif

/* From here until we send the DATA command, we can make use of PIPELINING
if the server host supports it. The code has to be able to check the responses
at any point, for when the buffer fills up, so we write it totally generally.
When PIPELINING is off, each command written reports that it has flushed the
buffer. */

pending_MAIL = TRUE;     /* The block starts with MAIL */

rc = smtp_write_command(&outblock, smtp_use_pipelining,
       "MAIL FROM:<%s>%s\r\n", return_path, buffer);
mail_command = string_copy(big_buffer);  /* Save for later error message */

switch(rc)
  {
  case -1:                /* Transmission error */
  goto SEND_FAILED;

  case +1:                /* Block was sent */
  if (!smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
    ob->command_timeout)) goto RESPONSE_FAILED;
  pending_MAIL = FALSE;
  break;
  }

/* Pass over all the relevant recipient addresses for this host, which are the
ones that have status PENDING_DEFER. If we are using PIPELINING, we can send
several before we have to read the responses for those seen so far. This
checking is done by a subroutine because it also needs to be done at the end.

Send only up to max_rcpt addresses at a time, leaving first_addr pointing to
the next one if not all are sent. **** The code for doing that is never
currently exercised, since the external max_addresses count is set to the
internal max_rcpt value. However, the code is left in for possible future use.
**** */

for (addr = first_addr;
     address_count < max_rcpt && addr != NULL;
     addr = addr->next)
  {
  int count;

  if (addr->transport_return != PENDING_DEFER) continue;

  address_count++;

  /* Handle any DSN options */

  p = buffer;
  *p = 0;

  #ifdef SUPPORT_DSN
  /* This partial attempt at doing DSN was abandoned. This obsolete code is
  left here just in case. Nothing is documented. */

  if (smtp_use_dsn)
    {
    if ((addr->dsn_flags & rf_dsnflags) != 0)
      {
      int i;
      BOOL first = TRUE;
      strcpy(p, " NOTIFY=");
      while (*p) p++;
      for (i = 0; i < 4; i++)
        {
        if ((addr->dsn_flags & rf_list[i]) != 0)
          {
          if (!first) *p++ = ',';
          first = FALSE;
          strcpy(p, rf_names[i]);
          while (*p) p++;
          }
        }
      }

    if (addr->dsn_orcpt != NULL)
      string_format(p, sizeof(buffer) - (p-buffer), " ORCPT=%s",
        addr->dsn_orcpt);
    }
  #endif

  /* Now send the RCPT command, and process outstanding responses when
  necessary. After a timeout on RCPT, we just end the function, leaving the
  yield as OK, because this error can often mean that there is a problem with
  just one address, so we don't want to delay the host. */

  count = smtp_write_command(&outblock, smtp_use_pipelining,
          "RCPT TO:<%s@%s>%s\r\n", addr->local_part, addr->domain, buffer);
  if (count < 0) goto SEND_FAILED;
  if (count > 0)
    {
    switch(sync_responses(first_addr, &sync_addr, host, count, pending_MAIL,
             0, &inblock, ob->command_timeout, buffer, sizeof(buffer)))
      {
      case 3: ok = TRUE;                   /* 2xx & 5xx => OK & progress made */
      case 2: completed_address = TRUE;    /* 5xx (only) => progress made */
      break;

      case 1: ok = TRUE;                   /* 2xx (only) => OK, but if LMTP, */
      if (!lmtp) completed_address = TRUE; /* can't tell about progress yet */
      case 0:                              /* No 2xx or 5xx, but no probs */
      break;

      case -1: goto END_OFF;               /* Timeout on RCPT */
      default: goto RESPONSE_FAILED;       /* I/O error, or any MAIL error */
      }
    pending_MAIL = FALSE;                  /* Dealt with MAIL */
    }
  }      /* Loop for next address */


/* If ok is TRUE, we know we have got at least one good recipient, and must now
send DATA, but if it is FALSE, we may still have a good recipient buffered up
if we are pipelining. We don't want to waste time sending DATA needlessly, so
we only send it if either ok is TRUE or if we are pipelining. The responses are
all handled by sync_responses(). */

if (ok || smtp_use_pipelining)
  {
  int count = smtp_write_command(&outblock, FALSE, "DATA\r\n");
  if (count < 0) goto SEND_FAILED;
  switch(sync_responses(first_addr, &sync_addr, host, count, pending_MAIL,
           ok? +1 : -1, &inblock, ob->command_timeout, buffer, sizeof(buffer)))
    {
    case 3: ok = TRUE;                   /* 2xx & 5xx => OK & progress made */
    case 2: completed_address = TRUE;    /* 5xx (only) => progress made */
    break;

    case 1: ok = TRUE;                   /* 2xx (only) => OK, but if LMTP, */
    if (!lmtp) completed_address = TRUE; /* can't tell about progress yet */
    case 0: break;                       /* No 2xx or 5xx, but no probs */

    case -1: goto END_OFF;               /* Timeout on RCPT */
    default: goto RESPONSE_FAILED;       /* I/O error, or any MAIL/DATA error */
    }
  }

/* Save the first address of the next batch. */

first_addr = addr;

/* If there were no good recipients (but otherwise there have been no
problems), just set ok TRUE, since we have handled address-specific errors
already. Otherwise, it's OK to send the message. Use the check/escape mechanism
for handling the SMTP dot-handling protocol, flagging to apply to headers as
well as body. Set the appropriate timeout value to be used for each chunk. The
SIGALRM handler must be set up here. (Haven't been able to make it work using
select() for writing yet.) */

if (!ok) ok = TRUE; else
  {
  sigalrm_seen = FALSE;
  os_non_restarting_signal(SIGALRM, sigalrm_handler);
  transport_write_timeout = ob->data_timeout;
  smtp_command = "sending data block";   /* For error messages */
  DEBUG(1) debug_printf("  SMTP>> writing message and terminating \".\"\n");
  ok = transport_write_message(addrlist, inblock.sock,
    topt_use_crlf | topt_end_dot | topt_escape_headers |
      (tblock->body_only? topt_no_headers : 0) |
      (tblock->headers_only? topt_no_body : 0) |
      (tblock->return_path_add? topt_add_return_path : 0) |
      (tblock->delivery_date_add? topt_add_delivery_date : 0) |
      (tblock->envelope_to_add? topt_add_envelope_to : 0),
    0,            /* No size limit */
    tblock->add_headers, tblock->remove_headers,
    ".", "..",    /* Escaping strings */
    tblock->rewrite_rules, tblock->rewrite_existflags);

  /* transport_write_message() uses write() because it is called from other
  places to write to non-sockets. This means that under some OS (e.g. Solaris)
  it can exit with "Broken pipe" as its error. This really means that the
  socket got closed at the far end. */

  save_errno = errno;            /* Preserve over signal call */
  transport_write_timeout = 0;   /* for subsequent transports */
  signal(SIGALRM, SIG_IGN);
  errno = save_errno;

  /* Failure can either be some kind of I/O disaster (including timeout),
  or the failure of a transport filter or the expansion of added headers. */

  if (!ok)
    {
    buffer[0] = 0;              /* There hasn't been a response */
    goto RESPONSE_FAILED;
    }

  /* We used to send the terminating "." explicitly here, but because of
  buffering effects at both ends of TCP/IP connections, you don't gain
  anything by keeping it separate, so it might as well go in the final
  data buffer for efficiency. This is now done by setting the topt_end_dot
  flag above. */

  smtp_command = "end of data";

  /* For SMTP, we now read a single response that applies to the whole message.
  If it is OK, then all the addresses have been delivered. */

  if (!lmtp) ok = smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
    ob->final_timeout);

  /* For LMTP, we get back a response for every RCPT command that we sent;
  some may be accepted and some rejected. For those that get a response, their
  status is fixed; any that are accepted have been handed over, even if later
  responses crash - at least, that's how I read RFC 2033.

  If all went well, mark the recipient addresses as completed, record which
  host/IPaddress they were delivered to, and cut out RSET when sending another
  message down the same channel. Write the completed addresses to the journal
  now so that they are recorded in case there is a crash of hardware or
  software before the spool gets updated. Also record the final SMTP
  confirmation if needed (for SMTP only). */

  if (ok)
    {
    int flag = '=';
    int len;
    host_item *thost;

    uschar *conf = NULL;
    send_rset = FALSE;

    /* Make a copy of the host if it is local to this invocation
    of the transport. */

    if (copy_host)
      {
      thost = store_get(sizeof(host_item));
      *thost = *host;
      thost->name = string_copy(host->name);
      thost->address = string_copy(host->address);
      }
    else thost = host;

    /* Set up confirmation if needed - applies only to SMTP */

    if (log_smtp_confirmation && !lmtp)
      {
      uschar *s = US string_printing(CS buffer);
      conf = (s == buffer)? (uschar *)string_copy(CS s) : s;
      }

    /* Process all transported addresses - for LMTP, read a status for
    each one. */

    for (addr = addrlist; addr != first_addr; addr = addr->next)
      {
      if (addr->transport_return != PENDING_OK) continue;

      /* LMTP - if the response fails badly (e.g. timeout), use it for all the
      remaining addresses. Otherwise, it's a return code for just the one
      address. */

      if (lmtp)
        {
        if (!smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
            ob->final_timeout))
          {
          if (errno != 0 || buffer[0] == 0) goto RESPONSE_FAILED;
          addr->message = string_sprintf("LMTP error after %s: %s",
            big_buffer, string_printing(CS buffer));
          addr->transport_return = (buffer[0] == '5')? FAIL : DEFER;
          continue;
          }
        completed_address = TRUE;   /* NOW we can set this flag */
        }

      /* SMTP, or success return from LMTP for this address */

      addr->transport_return = OK;
      addr->transported = thost;
      addr->special_action = flag;
      addr->message = CS conf;
      flag = '-';

      /* Just carry on after write error, as it may prove possible to
      update the spool file later. */

      sprintf(CS buffer, "N%s\n", addr->orig);
      len = (int)strlen(CS buffer);
      if (write(journal_fd, buffer, len) != len)
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to write journal for "
          "address %s: %s", addr->orig, strerror(errno));
      }

    /* Ensure the journal file is pushed out to disc. */

    if (fsync(journal_fd) < 0)
      log_write(0, LOG_MAIN|LOG_PANIC, "failed to fsync journal: %s",
        strerror(errno));
    }
  }


/* Handle general (not specific to one address) failures here. The value of ok
is used to skip over this code on the falling through case. A timeout causes a
deferral. Other errors may defer or fail according to the response code, and
may set up a special errno value, e.g. after connection chopped, which is
assumed if errno == 0 and there is no text in the buffer. If control reaches
here during the setting up phase (i.e. before MAIL FROM) then always defer, as
the problem is not related to this specific message. */

if (!ok)
  {
  int code;

  RESPONSE_FAILED:
  save_errno = errno;
  message = NULL;
  send_quit = check_response(host, &save_errno, addrlist->more_errno,
    buffer, &code, &message);
  goto FAILED;

  SEND_FAILED:
  save_errno = errno;
  code = '4';
  message = US string_sprintf("send() to %s [%s] failed",
    host->name, host->address, strerror(save_errno));
  send_quit = FALSE;

  /* This label is jumped to directly when a TLS negotiation has failed,
  or was not done for a host for which it is required. Values will be set
  in message and save_errno, and setting_up will always be true. */

  FAILED:
  ok = FALSE;        /* For when gone to */

  /* If the failure happened while setting up the call, see if the failure was
  a 5xx response (this will either be on connection, or following HELO - a 5xx
  after EHLO causes it to try HELO). If so, fail all addresses, as this host is
  never going to accept them. For other errors during setting up (timeouts or
  whatever), defer all addresses, and yield DEFER, so that the host is not
  tried again for a while. */

  if (setting_up)
    {
    if (code == '5')
      {
      set_errno(addrlist, save_errno, CS message, FAIL);
      }
    else
      {
      set_errno(addrlist, save_errno, CS message, DEFER);
      yield = DEFER;
      }
    }

  /* If there was an I/O error or timeout or other transportation error,
  indicated by errno being non-zero, defer all addresses and yield DEFER,
  except for the case of failed add_headers expansion, when the yield should
  be ERROR, to stop it trying other hosts.

  However, handle timeouts after MAIL FROM or "." and loss of connection after
  "." specially. They can indicate a problem with the sender address or with
  the contents of the message rather than a real error on the connection.
  Therefore, treat these cases in the same way as a 4xx response.

  The following condition tests for NOT these special cases. */

  else if (save_errno != 0 &&
           (save_errno != ETIMEDOUT ||
             (strncmp(smtp_command,"MAIL",4) != 0 &&
              strncmp(smtp_command,"end ",4) != 0)) &&
           (save_errno != ERRNO_SMTPCLOSED ||
              strncmp(smtp_command,"end ",4) != 0))
    {
    yield = (save_errno == ERRNO_CHHEADER_FAIL)? ERROR : DEFER;
    set_errno(addrlist, save_errno, CS message, yield);
    }

  /* Otherwise we have a message-specific error response from the remote
  host. This is one of
    (a) negative response or timeout after "mail from"
    (b) negative response after "data"
    (c) negative response or timeout or dropped connection after "."
  It won't be a negative response or timeout after "rcpt to", as that is dealt
  with separately above. The action in all cases is to set an appropriate
  error code for all the addresses, but to leave yield set to OK because
  the host itself has not failed. [It might in practice have failed for a
  timeout after MAIL FROM, or "." but if so, we'll discover that at the next
  delivery attempt.] For a temporary error, set the message_defer flag, and
  write to the logs for information if this is not the last host. The error for
  the last host will be logged as part of the address's log line. */

  else
    {
    set_errno(addrlist, save_errno, CS message, (code == '5')? FAIL : DEFER);

    /* If there's an errno, the message contains just the identity of
    the host. */

    if (code == '4')
      {
      if (save_errno > 0)
        message = US string_sprintf("%s: %s", message, strerror(save_errno));
      if (host->next != NULL) log_write(0, LOG_MAIN, "%s", message);
      fprintf(message_log, "%s %s\n", tod_stamp(tod_log), message);
      *message_defer = TRUE;
      }
    }
  }


/* If all has gone well, send_quit will be set TRUE, implying we can end the
SMTP session tidily. However, if there were too many addresses to send in one
message (indicated by first_addr being non-NULL) we want to carry on with the
rest of them. Also, it is desirable to send more than one message down the SMTP
connection if there are several waiting, provided we haven't already sent so
many as to hit the configured limit. The function transport_check_waiting looks
for a waiting message and returns its id. Then transport_pass_socket tries to
set up a continued delivery by passing the socket on to another process. The
variable send_rset is FALSE if a message has just been successfully transfered.

If we are already sending down a continued channel, there may be further
addresses not yet delivered that are aimed at the same host. In this case,
continue_more will be true, and all we should do is send RSET if necessary,
and return, leaving the channel open.

However, if no address was disposed of, i.e. all addresses got 4xx errors, we
do not want to continue with other messages down the same channel, because that
can lead to looping between two or more messages, all with the same,
temporarily failing address(es). [The retry information isn't updated yet, so
new processes keep on trying.] We probably also don't want to try more of this
message's addresses either. */

DEBUG(9) debug_printf("ok=%d send_quit=%d send_rset=%d continue_more=%d "
  "yield=%d first_address=%d\n", ok, send_quit, send_rset, continue_more,
  yield, first_addr);

if (completed_address && ok && send_quit)
  {
  BOOL more;
  if (first_addr != NULL || continue_more ||
    transport_check_waiting(tblock->name, host->name, ob->batch_max,
      CS new_message_id, &more))
    {
    uschar *msg;

    if (send_rset)
      {
      if (! (ok = smtp_write_command(&outblock, FALSE, "RSET\r\n")))
        {
        msg = US string_sprintf("send() to %s [%s] failed", host->name,
          host->address, strerror(save_errno));
        send_quit = FALSE;
        }
      else if (! (ok = smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
                  ob->command_timeout)))
        send_quit = check_response(host, &errno, 0, buffer, &yield, &msg);
      }

    /* Either RSET was not needed, or it succeeded */

    if (ok)
      {
      if (first_addr != NULL)      /**** shouldn't currently happen ****/
        {
        continue_sequence++;
        goto SEND_MESSAGE;
        }
      if (continue_more) return yield;

      /* Pass the socket to a new Exim process. Before doing so, we must shut
      down TLS. If the socket is successfully passed, we musn't send QUIT (or
      indeed anything!) from here. */

      #ifdef SUPPORT_TLS
      tls_close(TRUE);
      #endif

      if (transport_pass_socket(tblock->name, host->name, CS new_message_id,
          inblock.sock))
        {
        send_quit = FALSE;
        }
      }

    /* If RSET failed and there are addresses left, they get deferred. */

    else set_errno(first_addr, errno, CS msg, DEFER);
    }
  }

/* End off tidily with QUIT unless the connection has died or the socket has
been passed to another process. There has been discussion on the net about what
to do after sending QUIT. The wording of the RFC suggests that it is necessary
to wait for a response, but on the other hand, there isn't anything one can do
with an error response, other than log it. Exim used to do that. However,
further discussion suggested that it is positively advantageous not to wait for
the response, but to close the session immediately. This is supposed to move
the TCP/IP TIME_WAIT state from the server to the client, thereby removing some
load from the server. (Hosts that are both servers and clients may not see much
difference, of course.) Further discussion indicated that this was safe to do
on Unix systems which have decent implementations of TCP/IP that leave the
connection around for a while (TIME_WAIT) after the application has gone away.
This enables the response sent by the server to be properly ACKed rather than
timed out, as can happen on broken TCP/IP implementations on other OS.

This change is being made on 31-Jul-98. After over a year of trouble-free
operation, the old commented-out code was removed on 17-Sep-99. */

SEND_QUIT:
if (send_quit) (void) smtp_write_command(&outblock, FALSE, "QUIT\r\n");

END_OFF:

#ifdef SUPPORT_TLS
tls_close(TRUE);
#endif

/* Close the socket, and return the appropriate value, setting
continue_transport and continue_hostname NULL to prevent any other addresses
that may include the host from trying to re-use a continuation socket. If all
went well and continue_more is set, we shouldn't actually get here if there are
further addresses, as the return above will be taken. However, writing RSET
might have failed, or there may be other addresses whose hosts are specified in
the transports, and therefore not visible at top level, in which case
continue_more won't get set. When delivering down an old channel,
remote_max_parallel is forced to 1, so these values will be seen by subsequent
deliveries. */

close(inblock.sock);
continue_transport = NULL;
continue_hostname = NULL;
return yield;
}




/*************************************************
*              Closedown entry point             *
*************************************************/

/* This function is called when exim is passed an open smtp channel
from another incarnation, but the message which it has been asked
to deliver no longer exists. The channel is on stdin.

We might do fancy things like looking for another message to send down
the channel, but if the one we sought has gone, it has probably been
delivered by some other process that itself will seek further messages,
so just close down our connection.

Argument:   pointer to the transport instance block
Returns:    nothing
*/

void
smtp_transport_closedown(transport_instance *tblock)
{
smtp_transport_options_block *ob =
  (smtp_transport_options_block *)(tblock->options_block);
smtp_inblock inblock;
smtp_outblock outblock;
uschar buffer[256];
uschar inbuffer[4096];
uschar outbuffer[16];

inblock.sock = fileno(stdin);
inblock.buffer = inbuffer;
inblock.buffersize = sizeof(inbuffer);
inblock.ptr = inbuffer;
inblock.ptrend = inbuffer;

outblock.sock = inblock.sock;
outblock.buffersize = sizeof(outbuffer);
outblock.buffer = outbuffer;
outblock.ptr = outbuffer;
outblock.cmd_count = 0;

smtp_write_command(&outblock, FALSE, "QUIT\r\n");
(void) smtp_read_response(&inblock, buffer, sizeof(buffer), '2',
  ob->command_timeout);
close(inblock.sock);
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface details. As this is a remote transport, it is
given a chain of addresses to be delivered in one connection, if possible. It
always returns TRUE, indicating that each address has its own independent
status set, except if there is a setting up problem, in which case it returns
FALSE. */

BOOL
smtp_transport_entry(
  transport_instance *tblock,      /* data for this instantiation */
  address_item *addrlist)          /* addresses we are working on */
{
int cutoff_retry;
int port;
int hosts_defer = 0;
int hosts_fail  = 0;
int hosts_looked_up = 0;
int hosts_mx_to_nonexist = 0;
int hosts_retry = 0;
int hosts_serial = 0;
int hosts_total = 0;
address_item *addr;
BOOL expired = TRUE;
BOOL continuing = continue_hostname != NULL;
char *continue_host_address = NULL;
char *expanded_hosts = NULL;
char *portstring;
smtp_transport_options_block *ob =
  (smtp_transport_options_block *)(tblock->options_block);
host_item *hostlist = addrlist->host_list;
host_item *host;

DEBUG(2) debug_printf("%s transport entered\n", tblock->name);
DEBUG(9)
  {
  for (addr = addrlist; addr != NULL; addr = addr->next)
    debug_printf("  %s\n", addr->orig);
  }

/* If this is a delivery down an existing channel, find the IP address to
which we are connected. */

if (continuing)
  {
  #if HAVE_IPV6
  struct sockaddr_in6 continue_sock;
  #else
  struct sockaddr continue_sock;
  #endif

  int size = sizeof(continue_sock);
  if (getpeername(fileno(stdin), (struct sockaddr *)(&continue_sock), &size)
      == 0)
    continue_host_address = host_ntoa(-1, (struct sockaddr *)(&continue_sock),
      NULL, NULL);

  if (continue_host_address == NULL)
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "failed to get IP address of stdin "
      "when socket passed from previous delivery: %s", strerror(errno));
    continue_host_address = "";
    }

  DEBUG(9) debug_printf("already connected to %s [%s]\n", continue_hostname,
    continue_host_address);
  }

/* If a host list is not defined for the addresses - they must all have the
same one in order to be passed to a single transport - or if the transport has
a host list with hosts_override set, use the host list supplied with the
transport. It is an error for this not to exist. */

if (hostlist == NULL || (ob->hosts_override && ob->hosts != NULL))
  {
  if (ob->hosts == NULL)
    log_write(0, LOG_PANIC_DIE, "%s transport called with no hosts set",
      tblock->name);

  DEBUG(9) debug_printf("using the transport's hosts: %s\n", ob->hosts);

  /* If the transport's host list contains no '$' characters, and we are not
  randomizing, it is fixed and therefore a chain of hosts can be built once
  and for all, and remembered for subsequent use by other calls to this
  transport. If, on the other hand, the host list does contain '$', or we are
  randomizing its order, we have to rebuild it each time. In the fixed case,
  as the hosts string will never be used again, it doesn't matter that we
  replace all the : characters with zeros. */

  if (ob->hostlist == NULL)
    {
    char *s = ob->hosts;

    if (strchr(s, '$') != NULL)
      {
      expanded_hosts = expand_string(s);
      if (expanded_hosts == NULL)
        {
        addrlist->message = string_sprintf("failed to expand hostlist %s in %s "
          "transport: %s", s, tblock->name, expand_string_message);
        addrlist->transport_return = search_find_defer? DEFER : PANIC;
        return FALSE;     /* Only top address has status */
        }
      DEBUG(9) debug_printf("Expanded host list %s to %s\n", s, expanded_hosts);
      s = expanded_hosts;
      }
    else
      if (ob->hosts_randomize) s = expanded_hosts = string_copy(s);

    host_build_hostlist(&hostlist, s, ob->hosts_randomize);

    /* If there was no expansion of hosts, save the host list for
    next time. */

    if (expanded_hosts == NULL) ob->hostlist = hostlist;
    }

  /* This is not the first time this transport has been run in this delivery;
  the host list was built previously. */

  else hostlist = ob->hostlist;
  }


/* Sort out the service, i.e. the port number. We want the port number in
network byte order, and that's what getservbyname() produces, so we have
to use htons() if the configuration specified a port by number instead of
by name. Set up a string for adding to the retry key if the port number is not
the standard SMTP port. */

if (isdigit((uschar)(*ob->service)))
  {
  char *end;
  port = (int)htons((unsigned short)strtol(ob->service, &end, 0));
  if (end != ob->service + (int)strlen(ob->service))
    log_write(0, LOG_PANIC_DIE, "Invalid SMTP service: %s", ob->service);
  }
else
  {
  struct servent *smtp_service = getservbyname(ob->service, "tcp");
  if (smtp_service == NULL)
    log_write(0, LOG_PANIC_DIE, "TCP service \"%s\" not found", ob->service);
  port = smtp_service->s_port;
  }
portstring = string_sprintf(":%d", (int)ntohs((unsigned short)port));
if (strcmp(portstring, ":25") == 0) portstring = "";


/* For each host-plus-IP-address on the list:

.  If this is a continued delivery and the host isn't the one with the
   current connection, skip.

.  If the status is unusable (i.e. previously failed or retry checked), skip.

.  If no IP address set, get the address, either by turning the name into
   an address, calling gethostbyname if gethostbyname is on, or by calling
   the DNS. The DNS may yield multiple addresses, in which case insert the
   extra ones into the list.

.  Get the retry data if not previously obtained for this address and set the
   field which remembers the state of this address. Skip if the retry time is
   not reached. If not, remember whether retry data was found. The retry string
   contains both the name and the IP address.

.  Scan the list of addresses and mark those whose status is DEFER as
   PENDING_DEFER. These are the only ones that will be processed in this cycle
   of the hosts loop.

.  Make a delivery attempt - addresses marked PENDING_DEFER will be tried.
   Some addresses may be successfully delivered, others may fail, and yet
   others may get temporary errors and so get marked DEFER.

.  The return from the delivery attempt is OK if a connection was made and a
   valid SMTP dialogue was completed. Otherwise it is DEFER.

.  If OK, add a "remove" retry item for this host/IPaddress, if any.

.  If fail to connect, or other defer state, add a retry item.

.  If there are any addresses whose status is still DEFER, carry on to the
   next host/IPaddress, unless we have tried the number of hosts given
   by hosts_max_try; otherwise return.

If we get to the end of the list, all hosts have deferred at least one address,
or not reached their retry times. If delay_after_cutoff is unset, it requests a
delivery attempt to those hosts whose last try was before the arrival time of
the current message. To cope with this, we have to go round the loop a second
time. After that, set the status and error data for any addresses that haven't
had it set already. */

for (cutoff_retry = 0; expired &&
     cutoff_retry < ((ob->delay_after_cutoff)? 1 : 2);
     cutoff_retry++)
  {
  int hosts_tried = 0;
  for (host = hostlist;
       host != NULL && hosts_tried < ob->hosts_max_try;
       host = host->next)
    {
    int rc;
    char *rs;
    BOOL serialized = FALSE;
    BOOL host_is_expired = FALSE;
    BOOL message_defer = FALSE;
    address_item *first_addr = NULL;
    char *retry_host_key = NULL;
    char *retry_message_key = NULL;

    /* Set the flag requesting that this host be added to the waiting
    database if the delivery fails temporarily or if we are running with
    queue_smtp or a 2-stage queue run. This gets unset for certain
    kinds of error, typically those that are specific to the message. */

    host->update_waiting = TRUE;

    /* If the address hasn't yet been obtained from the host name, look it up
    now, unless the host is already marked as unusable. If it is marked as
    unusable, it means that the router was unable to find its IP address (in
    the DNS or wherever) OR we are in the 2nd time round the cutoff loop, and
    the lookup failed last time. We don't get this far if *all* MX records
    point to non-existent hosts; that is treated as a hard error.

    We can just skip this host entirely. When the hosts came from the router,
    the address will timeout based on the other host(s); when the address is
    looked up below, there is an explicit retry record added.

    Note that we mustn't skip unusable hosts if the address is not unset; they
    may be needed as expired hosts on the 2nd time round the cutoff loop. */

    if (host->address == NULL)
      {
      if (host->status >= hstatus_unusable)
        {
        DEBUG(9) debug_printf("%s has no address and is unusable - skipping\n",
          host->name);
        continue;
        }

      DEBUG(9) debug_printf("getting address for %s\n", host->name);

      hosts_looked_up++;

      /* If the "name" is in fact an IP address, just copy it over and check
      for being a local interface; otherwise call host_findxx() to look it up
      with or without the DNS. */

      if (string_is_ip_address(host->name, NULL))
        {
        host->address = host->name;
        host->mx = -1;
        rc = host_scan_for_local_hosts(host, host, FALSE);
        }
      else
        {
        char *canonical_name;
        if (ob->gethostbyname)
          rc = host_find_byname(host, NULL, &canonical_name, FALSE);

        /* Do an MX-only lookup if the name is in mx_domains. If there is
        a lookup defer in the checking, just proceed as if the host doesn't
        match - this is a rarely (if ever?) used feature, and it doesn't lead
        to a serious problem to do this. */

        else
          {
          BOOL mx_only =
            (ob->mx_domains != NULL &&
              match_isinlist(host->name, &(ob->mx_domains), TRUE, TRUE, NULL));
          rc = host_find_bydns(host, NULL, mx_only, FALSE, ob->dns_qualify_single,
            ob->dns_search_parents, &canonical_name, NULL);
          }
        }

      /* Remember if any hosts have MX records pointing to non-existent
      hosts so we can give the right error message if all are like this. */

      if (rc == HOST_FIND_FAILED && host->mx >= 0 && host->address == NULL)
        hosts_mx_to_nonexist++;

      /* Failure to find the host at this time (usually DNS temporary failure)
      is really a kind of routing failure rather than a transport failure.
      Therefore we add a retry item of the routing kind, not to stop us trying
      to look this name up here again, but to ensure the address gets timed
      out if the failures go on long enough. A complete failure at this point
      commonly points to a configuration error, but the best action is still
      to carry on for the next host. */

      if (rc == HOST_FIND_AGAIN || rc == HOST_FIND_FAILED)
        {
        retry_add_item(addrlist, "R", FALSE, host->name, 0);
        expired = FALSE;
        if (rc == HOST_FIND_AGAIN) hosts_defer++; else hosts_fail++;
        DEBUG(2) debug_printf("rc = %s for %s\n", (rc == HOST_FIND_AGAIN)?
          "HOST_FIND_AGAIN" : "HOST_FIND_FAILED", host->name);
        host->status = hstatus_unusable;

        for (addr = addrlist; addr != NULL; addr = addr->next)
          {
          if (addr->transport_return != DEFER) continue;
          addr->basic_errno = ERRNO_UNKNOWNHOST;
          addr->message =
            string_sprintf("failed to lookup IP address for %s", host->name);
          }
        continue;
        }

      /* If the host is actually the local host, we may have a problem, or
      there may be some cunning configuration going on. In the problem case,
      log things and give up. The default transport status is already DEFER. */

      if (rc == HOST_FOUND_LOCAL && !ob->allow_localhost)
        {
        for (addr = addrlist; addr != NULL; addr = addr->next)
          {
          addr->special_action = SPECIAL_FREEZE;
          addr->message = string_sprintf("%s transport found host %s to be "
            "local", tblock->name, host->name);
          }
        goto END_TRANSPORT;
        }
      }   /* End of block for IP address lookup */

    /* If this is a continued delivery, we are interested only in the host
    which matches the name of the existing open channel. The check is put
    here after the local host lookup, in case the name gets expanded as a
    result of the lookup. Set expired FALSE, to save the outer loop executing
    twice. */

    if (continuing && (strcmp(continue_hostname, host->name) != 0 ||
                       strcmp(continue_host_address, host->address) != 0))
      {
      expired = FALSE;
      continue;      /* With next host */
      }

    /* If the queue_smtp option is set, or the domain is in queue_smtp_domains,
    we don't actually want to attempt any deliveries. When doing a queue run,
    queue_smtp_domains is unset. If there is a lookup defer in queue_smtp_
    domains, proceed as if the domain were not in it. We don't want to hold up
    all SMTP deliveries! Except when doing a two-stage queue run, don't do
    this if forcing. */

    if ((!deliver_force || queue_2stage) && (queue_smtp ||
        match_isinlist(addrlist->domain, &queue_smtp_domains,TRUE,TRUE,NULL)))
      {
      expired = FALSE;
      for (addr = addrlist; addr != NULL; addr = addr->next)
        {
        if (addr->transport_return != DEFER) continue;
        addr->message = "domain matches queue_smtp_domains, or -odqs set";
        }
      continue;      /* With next host */
      }

    /* Count hosts being considered - purely for an intelligent comment
    if none are usable. */

    hosts_total++;

    /* The first time round the outer loop, check the status of the host by
    inspecting the retry data. The second time round, we are interested only
    in expired hosts that haven't been tried since this message arrived. */

    if (cutoff_retry == 0)
      {
      /* Ensure the status of the address is set by checking retry data if
      necessary. There maybe host-specific retry data (applicable to all
      messages) and also data for retries of a specific message at this host.
      If either of these retry records are actually read, the keys used are
      returned to save recomputing them later. */

      host_is_expired = retry_check_address(addrlist->domain, host, portstring,
        ob->retry_include_ip_address, &retry_host_key, &retry_message_key);

      DEBUG(2) debug_printf("%s [%s] status = %s\n", host->name,
        (host->address == NULL)? "" : host->address,
        (host->status == hstatus_usable)? "usable" :
        (host->status == hstatus_unusable)? "unusable" :
        (host->status == hstatus_unusable_expired)? "unusable (expired)" : "?");

      /* Skip this address if not usable at this time, noting if it wasn't
      actually expired, both locally and in the address. */

      switch (host->status)
        {
        case hstatus_unusable:
        expired = FALSE;
        setflag(addrlist, af_retry_skipped);
        /* Fall through */

        case hstatus_unusable_expired:
        switch (host->why)
          {
          case hwhy_retry: hosts_retry++; break;
          case hwhy_failed:  hosts_fail++; break;
          case hwhy_deferred: hosts_defer++; break;
          }

        /* If there was a retry message key, implying that previously there
        was a message-specific defer, we don't want to update the list of
        messages waiting for this host. */

        if (retry_message_key != NULL) host->update_waiting = FALSE;
        continue;   /* With the next host or IP address */
        }
      }

    /* Second time round the loop: if the address is set but expired, and
    the message is newer than the last try, let it through. */

    else
      {
      if (host->address == NULL ||
          host->status != hstatus_unusable_expired ||
          host->last_try > received_time)
        continue;
      DEBUG(2)
        debug_printf("trying expired host %s [%s]\n",
          host->name, host->address);
      host_is_expired = TRUE;
      }

    /* Setting "expired=FALSE" doesn't actually mean not all hosts are expired;
    it remains TRUE only if all hosts are expired and none are actually tried.
    */

    expired = FALSE;

    /* If this host is listed as one to which access must be serialized,
    see if another Exim process has a connection to it, and if so, skip
    this host. If not, update the database to record our connection to it
    and remember this for later deletion. Do not do any of this if we are
    sending the message down a pre-existing connection. */

    if (!continuing &&
        verify_check_this_host(&(ob->serialize_hosts), FALSE, host->name,
          host->address, NULL))
      {
      if (!transport_check_serialized(tblock->name, host->name))
        {
        DEBUG(2) debug_printf("skipping host %s because another Exim process "
          "is connected to it\n", host->name);
        hosts_serial++;
        continue;
        }
      serialized = TRUE;
      }

    /* OK, we have an IP address that is not waiting for its retry time to
    arrive (it might be expired) OR (second time round the loop) we have an
    expired host that hasn't been tried since the message arrived. Have a go
    at delivering the message to it, but previous delivery attempts may have
    left error stuff set up; flush that first, and record whether we got here
    via an MX record or not in the more_errno field of the address. We are
    interested only in addresses that are still marked DEFER - others may
    have got delivered to a previously considered IP address. Set their
    status to PENDING_DEFER to indicate which ones are relevant this time.
    Save the first one for use later. */

    for (addr = addrlist; addr != NULL; addr = addr->next)
      {
      if (addr->transport_return != DEFER) continue;
      if (first_addr == NULL) first_addr = addr;
      addr->transport_return = PENDING_DEFER;
      addr->basic_errno = 0;
      addr->more_errno = (host->mx >= 0)? 'M' : 'A';
      addr->message = NULL;
      #ifdef SUPPORT_TLS
      addr->cipher = NULL;
      addr->peerdn = NULL;
      #endif
      }

    DEBUG(2) debug_printf("delivering %s to %s [%s] (%s%s)\n",
      message_id, host->name, host->address, addrlist->orig,
      (addrlist->next == NULL)? "" : ", ...");

    set_process_info("delivering %s to %s [%s] (%s%s)",
      message_id, host->name, host->address, addrlist->orig,
      (addrlist->next == NULL)? "" : ", ...");

    /* This is not for real; don't do the delivery. If there are
    any remaining hosts, list them. */

    if (dont_deliver)
      {
      host_item *host2;
      debug_printf("*** delivery by %s transport bypassed by -N option\n",
        tblock->name);
      set_errno(addrlist, 0, NULL, OK);
      for (addr = addrlist; addr != NULL; addr = addr->next)
        {
        addr->transported = host;
        addr->special_action = '*';
        addr->message = "delivery bypassed by -N option";
        }
      debug_printf("*** host and remaining hosts:\n");
      for (host2 = host; host2 != NULL; host2 = host2->next)
        debug_printf("    %s [%s]\n", host2->name,
          (host2->address == NULL)? "unset" : host2->address);
      rc = OK;
      }

    /* This is for real; attempt the delivery */

    else
      {
      hosts_tried++;
      rc = smtp_deliver(addrlist, host, port, tblock, expanded_hosts != NULL,
        ob->keepalive, &message_defer);
      }

    /* Yield is one of:
       OK     => connection made, each address contains its result;
                   message_defer is set for message-specific defers (when all
                   recipients are marked defer)
       DEFER  => there was a non-message-specific delivery problem;
       ERROR  => there was a problem setting up the arguments for a filter,
                 or there was a problem with expanding added headers
    */

    rs = (rc == OK)? "OK" : (rc == DEFER)? "DEFER" : (rc == ERROR)?
      "ERROR" : "?";

    set_process_info("delivering %s: just tried %s [%s] for %s%s: result %s",
      message_id, host->name, host->address, addrlist->orig,
      (addrlist->next == NULL)? "" : " (& others)", rs);

    /* Release serialization if set up */

    if (serialized) transport_end_serialized(tblock->name, host->name);

    /* If the result is not OK, there was a non-message-specific problem.
    If it's DEFER, we need to write to the logs saying what happened for this
    particular host. If all hosts defer there will be a general message written
    at the end. */

    if (rc == DEFER) write_logs(first_addr, host);

    /* If the result is DEFER, or if a host retry record is known to exist, we
    need to add an item to the retry chain for updating the retry database
    at the end of delivery. We only need to add the item to the top address,
    of course. Also, if DEFER, we mark the IP address unusable so as to skip it
    for any other delivery attempts using the same address. (It is copied into
    the unusable tree at the outer level, so even if different address blocks
    contain the same address, it still won't get tried again.) */

    if (rc == DEFER || retry_host_key != NULL)
      {
      int delete_flag = (rc != DEFER)? rf_delete : 0;
      if (retry_host_key == NULL)
        {
        retry_host_key = ob->retry_include_ip_address?
          string_sprintf("T:%S:%s%s", host->name, host->address, portstring) :
          string_sprintf("T:%S%s", host->name, portstring);
        }
      retry_add_item(first_addr, "T", FALSE, retry_host_key + 2,
        rf_host | delete_flag);

      /* We may have tried an expired host, if its retry time has come; ensure
      the status reflects the expiry for the benefit of any other addresses. */

      if (rc == DEFER)
        {
        host->status = (host_is_expired)?
          hstatus_unusable_expired : hstatus_unusable;
        host->why = hwhy_deferred;
        }
      }

    /* If message_defer is set (host was OK, but every recipient got deferred
    because of some message-specific problem), or if that had happened
    previously so that a message retry key exists, add an appropriate item
    to the retry chain. Note that if there was a message defer but now there is
    a host defer, the message defer record gets deleted. That seems perfectly
    reasonable. Also, stop the message from being remembered as waiting
    for this host. */

    if (message_defer || retry_message_key != NULL)
      {
      int delete_flag = message_defer? 0 : rf_delete;
      if (retry_message_key == NULL)
        {
        retry_message_key = ob->retry_include_ip_address?
          string_sprintf("T:%S:%s%s:%s", host->name, host->address, portstring,
            message_id) :
          string_sprintf("T:%S%s:%s", host->name, portstring, message_id);
        }
      retry_add_item(addrlist, "T", FALSE, retry_message_key+2,
        rf_message | rf_host | delete_flag);
      host->update_waiting = FALSE;
      }

    /* Any return other than DEFER (OK or ERROR) means that the addresses have
    got their final statuses filled in for this host. If no addresses are
    marked DEFER, we are done with this chain of addresses. Otherwise let
    the loop run to try other hosts with the deferred addresses. However,
    if the result is ERROR, we don't want to do this, as a failing filter
    set-up or add_headers expansion is likely to fail for any host we try. */

    if (rc != DEFER)
      {
      BOOL some_deferred = FALSE;
      if (rc != ERROR) for (addr = addrlist; addr != NULL; addr = addr->next)
        {
        if (addr->transport_return == DEFER)
          {
          some_deferred = TRUE;
          break;
          }
        }

      /* If none deferred or result was ERROR, return. */

      if (!some_deferred)
        {
        DEBUG(2) debug_printf("Leaving %s transport\n", tblock->name);
        return TRUE;    /* Each address has its status */
        }
      }

    /* If we were trying to deliver down an existing channel and failed,
    that's it for this message. Don't try any other hosts. */

    if (continuing) break;
    }

  /* This is the end of the loop that repeats iff expired is TRUE and
  ob->delay_after_cutoff is FALSE. The second time round we will
  try those hosts that haven't been tried since the message arrived. */

  DEBUG(2)
    {
    debug_printf("all IP addresses skipped or deferred at least one address\n");
    if (expired && !ob->delay_after_cutoff && cutoff_retry == 0)
      debug_printf("retrying IP addresses not tried since message arrived\n");
    }
  }


/* Get here if all IP addresses are skipped or defer at least one address. Add
a standard message to each deferred address if there hasn't been an error, that
is, if it hasn't actually been tried this time. The variable "expired" will be
FALSE if any deliveries were actually tried, or if there was at least one host
that was not expired. That is, it is TRUE only if no deliveries were tried and
all hosts were expired. If a delivery has been tried, an error code will be
set, and the failing of the message is handled by the retry code later.

If queue_smtp is set, or this transport was called to send a subsequent message
down an existing TCP/IP connection, and something caused the host not to be
found, we end up here, but can detect these cases and handle them specially. */

for (addr = addrlist; addr != NULL; addr = addr->next)
  {
  if (queue_smtp)    /* no deliveries attempted */
    {
    addr->transport_return = DEFER;
    addr->basic_errno = 0;
    addr->message = "queue_smtp option set";
    }

  else if (addr->transport_return == DEFER &&
       (addr->basic_errno == ERRNO_UNKNOWNERROR || addr->basic_errno == 0) &&
       addr->message == NULL)
    {
    addr->basic_errno = ERRNO_HRETRY;
    if (continue_hostname != NULL)
      {
      addr->message = "no host found for existing SMTP connection";
      }
    else if (expired)
      {
      addr->message = (ob->delay_after_cutoff)?
        "retry time not reached for any host after a long failure period" :
        "all hosts have been failing for a long time and were last tried "
          "after this message arrived";
      addr->transport_return = FAIL;
      }
    else
      {
      if (hosts_total == 0 &&        /* No hosts actually tried */
          hosts_looked_up > 0 &&     /* Some were looked up here */
          hosts_looked_up == hosts_mx_to_nonexist)
        addr->message = "all relevant MX records point to non-existent hosts";

      else if (hosts_retry == hosts_total)
        addr->message = "retry time not reached for any host";
      else if (hosts_fail == hosts_total)
        addr->message = "all host address lookups failed permanently";
      else if (hosts_defer == hosts_total)
        addr->message = "all host address lookups failed temporarily";
      else if (hosts_serial == hosts_total)
        addr->message = "connection limit reached for all hosts";
      else if (hosts_fail+hosts_defer == hosts_total)
        addr->message = "all host address lookups failed";
      else addr->message = "some host address lookups failed and retry time "
        "not reached for other hosts or connection limit reached";
      }
    }
  }

/* Update the database which keeps information about which messages are waiting
for which hosts to become available. Each host in the list has a flag which is
set if the data is to be updated. For some message-specific errors, the flag is
turned off because we don't want follow-on deliveries in those cases. */

transport_update_waiting(hostlist, tblock->name);

END_TRANSPORT:

DEBUG(2) debug_printf("Leaving %s transport\n", tblock->name);

return TRUE;   /* Each address has its status */
}

/* End of transport/smtp.c */
