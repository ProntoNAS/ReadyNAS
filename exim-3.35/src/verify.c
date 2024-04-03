/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions concerned with verifying things. */


#include "exim.h"



/*************************************************
*           Local static variables               *
*************************************************/


static char ident_buffer[128];
static BOOL sender_ok;
static BOOL sender_is_local;

/* Values for sender_ok */

#define SENDER_OK_OK          0    /* really and truly OK */
#define SENDER_OK_EXCEPTION   1    /* OK by virtue of the exception list */
#define SENDER_OK_WARNING     2    /* OK by virtue of being in warning mode */
#define SENDER_OK_DEFER       3    /* check didn't complete */
#define SENDER_OK_NOT         4    /* Not OK */



/*************************************************
*           Set up new address                   *
*************************************************/

/* This function is called to set up a new address from what was routed or
directed, so as to include completed domain names, or whatever. If
af_local_part_quoted is set, the address is local and the local part was quoted
in some way, so we have to re-instate the quoting. We use a flag to control
this to save time doing it on 99.99% of the addresses that don't need it.

Arguments:
  newaddr       points to where to put the new address
  addr          the routed/directed address item

Returns:        nothing
*/

static void
set_new_address(char **newaddress, address_item *addr)
{
char *prefix = (addr->prefix == NULL)? "" : addr->prefix;
char *suffix = (addr->suffix == NULL)? "" : addr->suffix;

if (testflag(addr, af_local_part_quoted))
  {
  int i;
  int size = 64;
  int ptr = 0;
  char *s = store_get(size);
  char *p = prefix;
  s = string_cat(s, &size, &ptr, "\"", 1);
  for (i = 0; i < 3; i++)
    {
    while (*p != 0)
      {
      if (*p == '\"' || *p == '\\')
        s = string_cat(s, &size, &ptr, "\\", 1);
      s = string_cat(s, &size, &ptr, p++, 1);
      }
    p = (i == 0)? addr->local_part : suffix;
    }
  s = string_cat(s, &size, &ptr, "\"@", 2);
  s = string_cat(s, &size, &ptr, addr->domain,
    (int)strlen(addr->domain));
  s[ptr] = 0;
  *newaddress = s;
  }

else
  *newaddress = string_sprintf("%s%s%s@%s", prefix, addr->local_part,
    suffix, addr->domain);
}



/*************************************************
*            Verify an email address             *
*************************************************/

/* The local flag indicates whether an unqualified address is acceptable or
not. The local_domain flag is set TRUE if the address turns out to be in a
local domain, FALSE if it is in a remote domain. If the file is NULL, don't
print anything. If newaddress is not null, return the address, possibly
modified by the routing process. The yield is OK, FAIL, or DEFER. If there is a
parse error, return FAIL with the parse message in verify_address_parse_error,
which is otherwise set to NULL. If debugging, output fuller information,
provided there is an output file. If address_test_mode is true, don't skip
directors and routers that have no_verify set - we are doing a test rather than
a verify.

Arguments:
  s               address to verify
  f               if not NULL, write the result to this file
  local_domain    if not NULL, set TRUE if address turns out to be in a
                    local domain
  newaddress      if not NULL, return the top-level address, possibly modified
  options         various option bits:
                    vopt_is_recipient => this is a recipient address, otherwise
                      it's a sender address - this affects qualification and
                      rewriting
                    vopt_local => qualify an unqualified address; else error
                    vopt_expn => called from SMTP EXPN command

Returns:          OK
                  FAIL
                  DEFER
*/

int
verify_address(char *s, FILE *f, BOOL *local_domain, char **newaddress,
  int options)
{
int start, end, domain;
BOOL allok = TRUE;
BOOL full_info = (f == NULL)? FALSE : (debug_level > 0);
BOOL is_recipient = (options & vopt_is_recipient) != 0;
BOOL expn         = (options & vopt_expn) != 0;
int i;
int yield = OK;
int verify_type = expn? v_expn :
     address_test_mode? v_none :
          is_recipient? v_recipient : v_sender;
address_item *addr_list;
address_item *addr_orig;
address_item *addr_new;
address_item *addr_remote = NULL;
address_item *addr_local = NULL;
address_item *addr_succeed = NULL;
char *ko_prefix, *cr;

char *receiver =
  parse_extract_address(s, &verify_address_parse_error, &start, &end, &domain,
    FALSE);

if (expn)
  {
  ko_prefix = "553 ";
  cr = "\r";
  }
else ko_prefix = cr = "";

if (receiver == NULL)
  {
  if (f != NULL)
    fprintf(f, "%s%s - bad address: %s%s\n", ko_prefix, s,
      verify_address_parse_error, cr);
  return FAIL;
  }
else verify_address_parse_error = NULL;

/* Add qualify domain if permitted. */

if (parse_find_at(receiver) == NULL)
  {
  if ((options & vopt_local) == 0)
    {
    if (f != NULL)
      fprintf(f, "%sA domain is required for \"%s\"%s\n", ko_prefix, receiver,
        cr);
    return FAIL;
    }
  receiver = rewrite_address_qualify(receiver, is_recipient);
  }

DEBUG(9)
  {
  debug_printf(">>>>>>>>>>>>>>>>>>>>>>>>\n");
  debug_printf("%s %s\n", address_test_mode? "Testing" : "Verifying", receiver);
  }

/* Rewrite and report on it. */

if (global_rewrite_rules != NULL)
  {
  char *old = receiver;
  receiver = rewrite_address(receiver, is_recipient, FALSE,
    global_rewrite_rules, rewrite_existflags);
  if (receiver != old && f != NULL && !expn)
    fprintf(f, "Address rewritten as: %s\n", receiver);
  }

/* Set up an initial address structure. */

addr_new = addr_orig = deliver_make_addr(receiver, FALSE);

/* We need a loop, since a directed address might generate a number of new
addresses. We must also cope with generated pipes and files at the top
level. (See also the code/comment in deliver.c.) However, it is usually
the case that the forwardfile director has its verify flag turned off.

The loop is used after directing, however, only when full_info is set, and this
can only be set locally. Remote enquiries just get information about the top
level address, not anything that it generated.

In the case of a router discovering that an apparently remote address is in
fact local, the loop is always re-run. */

while (addr_new != NULL)
  {
  int rc;
  address_item *addr = addr_new;
  addr_new = addr->next;
  addr->next = NULL;

  /* Handle generated pipe, file or reply addresses. We don't get these
  when handling EXPN, as it does only one level of expansion. */

  if (testflag(addr, af_pfr))
    {
    allok = FALSE;
    if (addr->orig[0] == '|')
      {
      if (f != NULL)
        fprintf(f, "%s -> %s %s\n", addr->parent->orig,
          addr->orig, testflag(addr, af_allow_pipe)? "" : "*** forbidden ***");
      }
    else if (addr->orig[0] == '/')
      {
      if (f != NULL)
        fprintf(f, "%s -> %s %s\n", addr->parent->orig,
          addr->orig, testflag(addr, af_allow_file)? "" : "*** forbidden ***");
      }
    else
      {
      if (f != NULL)
        fprintf(f, "%s -> mail %s %s\n", addr->parent->orig,
          addr->orig+1, testflag(addr, af_allow_reply)? "" : "*** forbidden ***");
      }

    if (addr->basic_errno == ERRNO_BADTRANSPORT && f != NULL)
      {
      fprintf(f, "*** Error in setting up pipe, file, or autoreply:\n  %s\n",
        addr->message);
      }

    continue;
    }

  /* All addresses should either have been made fully qualified above,
  or been qualified when generated by a director, so panic if we find
  an unqualified one. */

  if (parse_find_at(addr->orig) == NULL)
    log_write(0, LOG_PANIC_DIE, "Unqualified address found: %s", addr->orig);

  /* Determine locality - this sets "local_part", "domain", and "local"
  fields. Pass back the locality if a variable is supplied. If we cannot
  determine the locality (lookup defer), behave as if directing or routing
  deferred. An error will have been set for the address. */

  if (!deliver_setlocal(addr)) rc = DEFER; else
    {
    if (local_domain != NULL) *local_domain = testflag(addr, af_local);

    /* DEBUG: show what's been done to this address */

    DEBUG(7)
      {
      debug_printf("address %s\n", addr->orig);
      debug_printf("  local_part=%s domain=%s\n  %s\n",
        addr->local_part, addr->domain,
        testflag(addr, af_local)? "domain is local" : "domain is not local");
      }

    /* Handle a local address with the directors, or a remote address with the
    routers. Note that a director may set up local or remote delivery. */

    verify_forced_errmsg = NULL;

    rc = testflag(addr, af_local)?
      direct_address(addr, &addr_local, &addr_remote, &addr_new, &addr_succeed,
        verify_type) :
      route_address(addr, &addr_local, &addr_remote, &addr_new, verify_type);

    /* If directing/routing succeeded and set up a list of hosts, and the
    client host is in sender_verify_hosts_callback and the domain is in
    sender_verify_callback_domains, do the full call-back verification for a
    sender. This is expensive. */

    /* This is still somewhat experimental code. */

    if (rc == OK &&
        addr->host_list != NULL &&
        (options & vopt_is_recipient) == 0 &&
        verify_check_host(&sender_verify_hosts_callback, FALSE) &&
        sender_verify_callback_domains != NULL &&
        match_isinlist(addr->domain, &sender_verify_callback_domains, FALSE,
          TRUE, NULL))
      {
      host_item *host;
      BOOL done = FALSE;

      HDEBUG(1) debug_printf("Attempting full verification using callback\n");

      if (host_checking)
        {
        HDEBUG(1) debug_printf("... callback omitted when host testing\n");
        done = TRUE;
        }

      else for (host = addr->host_list;
                host != NULL && ! done;
                host = host->next)
        {
        int port = running_in_test_harness? 1225 : 25;
        smtp_inblock inblock;
        smtp_outblock outblock;
        uschar inbuffer[1024];
        uschar outbuffer[1024];

        /* Skip this host if we don't have an IP address for it. */

        if (host->address == NULL) continue;

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

        inblock.sock = outblock.sock =
          smtp_connect(host, (int)htons((unsigned short)port), NULL,
            sender_verify_callback_timeout, TRUE);

        if (inblock.sock < 0) continue;

        if (smtp_read_response(&inblock, US big_buffer, big_buffer_size, '2',
              sender_verify_callback_timeout) &&
            smtp_write_command(&outblock, FALSE, "HELO %s\r\n",
              primary_hostname) &&
            smtp_read_response(&inblock, US big_buffer, big_buffer_size, '2',
              sender_verify_callback_timeout) &&
            smtp_write_command(&outblock, FALSE, "MAIL FROM:<>\r\n") &&
            smtp_read_response(&inblock, US big_buffer, big_buffer_size, '2',
              sender_verify_callback_timeout) &&
            smtp_write_command(&outblock, FALSE, "RCPT TO:<%.1000s>\r\n",
              addr->orig))
          {
          if (smtp_read_response(&inblock, US big_buffer, big_buffer_size, '2',
              sender_verify_callback_timeout))
            {
            done = TRUE;                /* Address accepted */
            }
          else if (errno == 0)
            {
            sender_verify_callback_error =
              string_sprintf("response from %s [%s] was %s", host->name,
                host->address, big_buffer);
            if (big_buffer[0] == '5')   /* Address rejected */
              {
              rc = FAIL;
              addr->message =
                string_sprintf("callback check with host %s failed: %s",
                  host->name, big_buffer);
              done = TRUE;
              }
            }
          }

        smtp_write_command(&outblock, FALSE, "QUIT\r\n");
        close(inblock.sock);
        }

      /* Failure to connect to any host, or any response other than 2xx or 5xx
      is a temporary error. */

      if (!done)
        {
        rc = DEFER;
        addr->message =
          string_sprintf("could not complete callback check");
        }
      }   /* Callback */
    }     /* Routing/directing done, or setlocal() failed */

  /* If a remote address turned out to be local after all, set it up for
  reprocessing and restart the loop. If this is a sender and the router
  doesn't want sender rewrites, cancel newaddress. */

  if (rc == ISLOCAL)
    {
    if (newaddress != NULL && !is_recipient && addr->router != NULL &&
        !addr->router->sender_rewrite)
      {
      *newaddress = NULL;
      newaddress = NULL;
      DEBUG(7) debug_printf("sender rewrite disabled\n");
      }
    DEBUG(7) debug_printf("routing %s caused it to become local\n", addr->orig);
    addr->orig = string_sprintf("%s@%s", addr->local_part, addr->domain);
    addr->host_list = NULL;
    addr->next = addr_new;
    addr_new = addr;
    continue;
    }

  /* Handle hard failures */

  if (rc == FAIL)
    {
    allok = FALSE;
    if (f != NULL)
      {
      if (expn && !testflag(addr, af_local))
        fprintf(f, "550 Not a local domain\r\n");
      else
        {
        fprintf(f, "%s%s %s", ko_prefix, addr->orig,
          address_test_mode? "is undeliverable" : "failed to verify");
        if (addr->basic_errno > 0)
          fprintf(f, ": %s", strerror(addr->basic_errno));
        if (addr->message != NULL)
          fprintf(f, ":%s%s", expn? " " : "\n  ", addr->message);
        fprintf(f, "%s\n", cr);
        }
      }

    if (!full_info) return FAIL; else yield = FAIL;
    }

  /* If the yield is ERROR or PANIC, there has been some cock-up in the
  directors or routers. This doesn't really mean the address is undeliverable
  or unverifyable, so we treat it the same as DEFER. */

  else if (rc == DEFER || rc == ERROR || rc == PANIC)
    {
    allok = FALSE;
    if (f != NULL)
      fprintf(f, "%s%s cannot be resolved at this time:%s%s%s%s%s\n",
      ko_prefix,
      addr->orig,
      expn? " " : "\n  ",
      (addr->basic_errno <= 0)? "" : strerror(addr->basic_errno),
      (addr->basic_errno <= 0)? "" : ": ",
      (addr->message != NULL)? addr->message :
        (addr->basic_errno <= 0)? "unknown error" : "", cr);
    if (!full_info) return DEFER;
      else if (yield == OK) yield = DEFER;
    }

  /* If we are handling EXPN, we do not want to continue to route beyond
  the top level. If the address was not local, give an error. We have to put
  this test here, after the routing, in case the routing turned it into a
  local address which then got directed. Otherwise, output what has been
  generated. */

  else if (expn)
    {
    char *ok_prefix = "250-";

    if (addr == addr_orig && !testflag(addr, af_local))
      {
      fprintf(f, "550 Not a local domain\r\n");
      return FALSE;
      }

    if (addr_new == NULL)
      {
      if (addr_local == NULL)
        fprintf(f, "250 mail to <%s> is discarded\r\n", s);
      else
        fprintf(f, "250 <%s>\r\n", addr_local->orig);
      }
    else while (addr_new != NULL)
      {
      address_item *addr = addr_new;
      addr_new = addr->next;
      if (addr_new == NULL) ok_prefix = "250 ";
      fprintf(f, "%s<%s>\r\n", ok_prefix, addr->orig);
      }
    return OK;
    }

  /* Successful routing/directing other than EXPN. Pass back the routed/
  directed address if requested (so as to get completed domain names or
  whatever). */

  else
    {
    if (newaddress != NULL)
      {
      set_new_address(newaddress, addr);
      newaddress = NULL;                  /* Only at top level */
      }

    /* Handle successful routing or directing when short info wanted. Otherwise
    continue for other (generated) addresses.

    There is a conflict between the use of aliasing as an alternate email
    address, and as a sort of mailing list. If an alias turns the incoming
    address into just one address (e.g. J.Caesar->jc44) you may well want to
    carry on verifying the generated address to ensure it is valid when
    checking incoming mail. If aliasing generates multiple addresses, you
    probably don't want to do this. Exim therefore treats the generation of
    just a single new address as a special case, and continues on to verify the
    generated address, when the address was directed and the director has
    one_continue set TRUE (aliasfile and smartuser do, forwardfile does not). */

    if (!full_info &&
         (addr_new == NULL ||                    /* No new address */
          addr_new->next != NULL ||              /* More than one new address */
          testflag(addr_new, af_pfr) ||          /* New address is pfr */
          addr->director == NULL ||              /* Address was routed */
          !addr->director->one_continue))        /* Director said stop */
      {
      if (f != NULL) fprintf(f, "%s %s\n",
        addr->orig,
        address_test_mode? "is deliverable" : "verified");
      return OK;
      }
    }
  }

/* Display the full results of the successful directing and routing, including
any generated addresses. Control gets here only when full_info is set, which
requires f not to be NULL, and this occurs only when a top-level verify is
called with the debugging switch on.

If there are no local and no remote addresses, and there were no pipes, files,
or autoreplies, and there were no errors or deferments, the message is to be
discarded, usually because of the use of :blackhole: in an alias file. */

if (allok && addr_local == NULL && addr_remote == NULL)
  fprintf(f, "mail to %s is discarded\n", s);

for (addr_list = addr_local, i = 0; i < 2; addr_list = addr_remote, i++)
  {
  while (addr_list != NULL)
    {
    address_item *addr = addr_list;
    address_item *p = addr->parent;
    addr_list = addr->next;

    fprintf(f, "%s", addr->orig);

    while (p != NULL)
      {
      fprintf(f, "\n    <-- %s", p->orig);
      p = p->parent;
      }

    /* Show director and/or router, and transport */

    if (addr->router != NULL)
      {
      fprintf(f, "\n  deliver to %s@%s", addr->local_part, addr->domain);

      if (strcmp(addr->domain, addr->route_domain) != 0)
        fprintf(f, " via domain %s", addr->route_domain);
      fprintf(f, "\n  ");

      if (addr->director != NULL)
        fprintf(f, "director = %s, ", addr->director->name);
      fprintf(f, "router = %s, ", addr->router->name);
      }

    else if (addr->director != NULL)
      {
      fprintf(f, "\n  deliver to %s in domain %s\n",
        addr->local_part, addr->domain);
      fprintf(f, "  director = %s, ", addr->director->name);
      }

    fprintf(f, "transport = %s\n", (addr->transport == NULL)? "unset" :
      addr->transport->name);

    /* Show any hosts that are set up by a router unless the transport
    is going to override them; fiddle a bit to get a nice format. */

    if (addr->host_list != NULL && addr->transport != NULL &&
        !addr->transport->overrides_hosts)
      {
      host_item *h;
      int maxlen = 0;
      int maxaddlen = 0;
      for (h = addr->host_list; h != NULL; h = h->next)
        {
        int len = (int)strlen(h->name);
        if (len > maxlen) maxlen = len;
        len = (h->address != NULL)? (int)strlen(h->address) : 7;
        if (len > maxaddlen) maxaddlen = len;
        }
      for (h = addr->host_list; h != NULL; h = h->next)
        {
        int len = (int)strlen(h->name);
        fprintf(f, "  host %s ", h->name);
        while (len++ < maxlen) fprintf(f, " ");
        if (h->address != NULL)
          {
          fprintf(f, "[%s] ", h->address);
          len = (int)strlen(h->address);
          }
        else if (!addr->transport->info->local)  /* Omit [unknown] for local */
          {
          fprintf(f, "[unknown] ");
          len = 7;
          }
        else len = -3;
        while (len++ < maxaddlen) fprintf(f," ");
        if (h->mx >= 0) fprintf(f, "MX=%d", h->mx);
        if (h->status == hstatus_unusable) fprintf(f, " ** unusable **");
        fprintf(f, "\n");
        }
      }
    }
  }

return yield;  /* Will be DEFER or FAIL if any one has */
}




/*************************************************
*             Get valid header address           *
*************************************************/

/* Scan the originator headers of the message, looking for an address that
verifies successfully. This function is called from the second verification
function below. RFC 822 says:

    o   The "Sender" field mailbox should be sent  notices  of
        any  problems in transport or delivery of the original
        messages.  If there is no  "Sender"  field,  then  the
        "From" field mailbox should be used.

    o   If the "Reply-To" field exists, then the reply  should
        go to the addresses indicated in that field and not to
        the address(es) indicated in the "From" field.

So we check a Sender field if there is one, else a Reply_to field,
else a From field. As some strange messages may have more than one of these
fields, especially if they are resent- fields, check all of them if there is
more than one.

Arguments:
  newaddr        address of pointer to point at the verified address, or
                   NULL, if value of the address not wanted
  header_id      address of pointer to point at name of header used,
                   or NULL if not wanted
  sender_yield   the yield to give if the sender_address is found (saves
                   verifying it again)

Returns:         result of the verification attempt: OK, FAIL, or DEFER;
                 FAIL is given if no appropriate headers are found
*/

static int
verify_get_header_address(char **newaddr, char **header_id, int sender_yield)
{
static int header_types[] = { htype_sender, htype_replyto, htype_from };
static int header_name_offsets[] = { hn_sender, hn_replyto, hn_from };
int senderlen = (int)strlen(sender_address);
int yield = FAIL;
int i;

for (i = 0; i < 3; i++)
  {
  header_line *h;
  for (h = header_list; h != NULL; h = h->next)
    {
    BOOL is_local;
    int terminator, new_ok;
    char *s, *ss;

    if (h->type != header_types[i]) continue;
    s = strchr(h->text, ':') + 1;

    while (*s != 0)
      {
      while (isspace((uschar)*s) || *s == ',') s++;
      if (*s == 0) continue;        /* Skip if header is empty */

      ss = parse_find_address_end(s, FALSE);
      terminator = *ss;
      *ss = 0;

      HDEBUG(2) debug_printf("verifying header address %s%s", s,
        (ss[-1] == '\n')? "" : "\n");

      /* Sender address in header may have whitespace on the end; if this
      test doesn't work, it isn't really harmful other than that a redundant
      verification gets done. */

      if (strncmp(s, sender_address, senderlen) == 0 &&
                (s[senderlen] == 0 || isspace((uschar)s[senderlen])))
        {
        new_ok = sender_yield;
        HDEBUG(2) debug_printf("same as sender\n");
        }
      else
        {
        new_ok = verify_address(s, NULL, &is_local, newaddr, 0);
        }

      *ss = terminator;

      /* If verification failed because of a syntax error, fail this function,
      and ensure that the failing address gets added to the error message. */

      if (new_ok == FAIL && verify_address_parse_error != NULL)
        {
        header_name *header_names = (strncmpic(h->text, "resent-", 7) == 0)?
          header_names_resent : header_names_normal;
        while (ss > s && isspace((uschar)ss[-1])) ss--;
        verify_address_parse_error =
          string_sprintf("syntax error in '%s' header when scanning for sender: "
            "%s in \"%.*s\"",
            header_names[header_name_offsets[i]].name,
            verify_address_parse_error, ss - s, s);
        return FAIL;
        }

      /* Success: set up the name of the header */

      if (new_ok == OK)
        {
        if (header_id != NULL)
          {
          header_name *header_names = (strncmpic(h->text, "resent-", 7) == 0)?
            header_names_resent : header_names_normal;
          *header_id = header_names[header_name_offsets[i]].name;
          }
        return OK;
        }

      if (new_ok == DEFER) yield = DEFER;

      /* Move on to any more addresses in the header */

      s = ss;
      }
    }
  }

return yield;
}




/*************************************************
*            Verify the sender of a message      *
*************************************************/

/* The next two functions operate in tandem, and the second must not be
called without first calling the first one. The reason for this approach is
that some SMTP mailers treat any error returned after the data has been
transmitted as temporary (contrary to RFC821) and keep retrying, even after
they have been sent a 5xx error at the previous attempt. To get round this,
exim keeps a database of failed messages and their hosts, and if the same bad
address is received from the same host soon afterwards, it is rejected at the
preliminary stage (meaning after MAIL FROM for SMTP) in the hope that the far
end might now give up.

The reason for not rejecting at this stage in all cases is that remote
postmasters, when told their systems have been sending out bad messages, always
ask "what were the headers?" and so one needs to have read them in order to log
them. This also helps track down mail forgers. It also makes it possible to
replace bad envelope sender addresses with good ones from inside the message if
that option is configured.

September 1996: Some mailers keep on trying even after getting a 5xx error for
MAIL FROM. If the same bad address is received from the same host for a third
time in a short time, MAIL FROM is accepted, but refuse_all_rcpts is set, and
all subsequent RCPT TO commands get rejected with a 550. */



/*************************************************
*        First check on sender address           *
*************************************************/

/* This function is called as soon as a sender address has been received
from an SMTP connection. Unless the host is in the list of those from which no
verification is required, it verifies the address. If it is bad and
sender_verify_reject is FALSE, it gives an OK response with a warning message.

When sender_verify_reject is TRUE and a bad address is received, it checks to
see if the same address has recently been rejected. If not, it sets a flag for
verify_sender() to interrogate. If it has been rejected once recently, an error
return is given for MAIL FROM. If it has been rejected more than once recently,
then MAIL FROM is accepted, but a flag is set to cause all subsequent RCPT TO
commands to be rejected.

Arguments:
  errcode    set this to an SMTP error code on failure, or if there is a
               warning message on success
  errmess    set this to point to an error message on failure

Returns:     TRUE if address verified, or did not fail recently, or host
               is in the exception list, i.e. if is OK to proceed. The
               variable sender_ok indicates why it was allowed to proceed.
*/

BOOL
verify_sender_preliminary(int *errcode, char **errmess)
{
char *newaddr;
open_db dbblock;
open_db *dbm_file;
dbdata_reject *reject;
int rc, rejectlen, yield;
time_t now;
char buffer[ADDRESS_MAXLENGTH + 256];

/* If the sender address is empty, it's an error message with, in effect,
no sender, and we can't check anything. */

if (sender_address[0] == 0)
  {
  sender_ok = SENDER_OK_OK;
  return TRUE;
  }

/* Don't check if this host isn't required to be checked. */

if (!verify_check_host(&sender_verify_hosts, FALSE))
  {
  sender_ok = SENDER_OK_EXCEPTION;
  return TRUE;
  }

/* Try to verify the address */

HDEBUG(2) debug_printf("verifying sender %s\n", sender_address);

rc = verify_address(sender_address, NULL, &sender_is_local, &newaddr, 0);

/* After a successful return, the address may have been changed (typically
a domain will be canonicized or expanded by a router). */

if (rc == OK)
  {
  if (newaddr != NULL)
    {
    DEBUG(2) debug_printf("%s verified ok as %s\n", sender_address, newaddr);
    sender_address_unrewritten = sender_address;
    sender_address = newaddr;
    }
  else
    {
    DEBUG(2) debug_printf("%s verified ok\n", sender_address);
    }
  sender_ok = SENDER_OK_OK;
  return TRUE;
  }

/* If we are not going to reject permanent failures or if we are going to
accept temporary failures and it is one of them, accept the address, and note
it is a "warning" state. */

if (!sender_verify_reject || (rc == DEFER && sender_try_verify))
  {
  if (rc == DEFER)
    {
    *errmess = "warning: temporarily unable to resolve sender address: "
      "accepted unverified";
    }
  else
    {
    *errmess = sender_is_local?
      "warning: unknown local-part in sender address" :
      "warning: cannot route to sender address";
    }
  *errcode = 250;
  sender_ok = SENDER_OK_WARNING;
  return TRUE;
  }

/* Set up the key for the reject hints database, and attempt to open it.
If successful, read the record. */

snprintf(buffer, sizeof(buffer), "%s:%.200s", sender_address,
  (sender_host_name != NULL)? sender_host_name :
  (sender_host_address != NULL)? sender_host_address : "");

dbm_file = dbfn_open("reject", O_RDWR, &dbblock, FALSE);
reject = (dbm_file == NULL)?
  NULL : dbfn_read_with_length(dbm_file, buffer, &rejectlen);

/* Ignore data that is more than 24 hours old */

now = time(NULL);
if (reject != NULL && now - reject->time_stamp > 24*60*60) reject = NULL;

/* Defer is usually a DNS time out. If we have given this temporary rejection
too often recently, convert it into a permanent failure. Once the failure is
permanent, subsequent temporary failures are all made permanent, until the hint
expires (currently 24 hours). Otherwise accept the address for now, leaving
sender_ok set to DEFER, thus postponing possible (temporary) rejection to the
second verification function. */

if (rc == DEFER)
  {
  float temp_rate;

  sender_ok = SENDER_OK_DEFER;

  /* If there's no hints file, or no (recent) record in it, accept the address
  for now. This enables the headers to be read (for logging) and a rejection
  to be given later - or maybe the address can be fixed up from the headers. */

  if (dbm_file == NULL || reject == NULL)
    {
    HDEBUG(2) debug_printf("temporary error while verifying %s - will give "
      "error after DATA\n", sender_address);
    if (dbm_file != NULL) dbfn_close(dbm_file);
    return TRUE;
    }

  /* Control gets here if there has been a temporary or permanent rejection of
  this address recently. If we read a short record, this is old data from a
  previous version of Exim, which did not store temp_rate fields because it
  kept hints for permanent rejection only. Treat as a permanent rejection. This
  temporary code was installed in September 1998. Remove in a couple of years'
  time. */

  temp_rate = (rejectlen < sizeof(dbdata_reject))? 0.0 : reject->temp_rate;

  /* If temp_rate is zero, the previous rejection was permanent, so this one
  must be as well. Otherwise, if the max retry rate is set, we convert to a
  permanent error if it is exceeded. The test here is for continuing: failure
  is falling through. The first failure of an address will have succeeded here
  and given the error later, so as to log the headers. Subsequent failures can
  therefore give the failure here, updating the rate as they do so. */

  if (temp_rate > 0.0 &&
       (sender_verify_max_retry_rate <= 0 ||
        (int)temp_rate < sender_verify_max_retry_rate))
    {
    reject->temp_rate = ((reject->temp_rate + 1.0) * 3600) /
                        (now - reject->time_stamp + 3600);
    dbfn_write(dbm_file, buffer, reject, sizeof(dbdata_reject));
    dbfn_close(dbm_file);
    DEBUG(2) debug_printf("%s verification deferred\n", sender_address);
    *errcode = 451;
    *errmess = "temporarily unable to verify sender address (try again later)";
    if (sender_verify_callback_error != NULL)
      *errmess = string_sprintf("%s: %s", *errmess, sender_verify_callback_error);
    return FALSE;
    }

  HDEBUG(2)
    {
    if (temp_rate == 0.0)
      debug_printf("%s verification deferred after previous failure - "
        "treating as permanent error\n", sender_address);
    else
      debug_printf("%s verification deferred too frequently - "
        "forcing a permanent error\n", sender_address);
    }
  }

/* If control gets here it's a hard failure, possibly converted from a
temporary failure that happened too often. */

sender_ok = SENDER_OK_NOT;

/* If there has been no recent rejection, pass now, leaving rejection to the
second function, which will create a DBM entry if necessary (it might not be
necessary if the sender is fixed up from the headers). */

if (dbm_file == NULL || reject == NULL)
  {
  HDEBUG(2)
    debug_printf("%s verification failed - will give error after DATA\n",
      sender_address);
  if (dbm_file != NULL) dbfn_close(dbm_file);
  return TRUE;
  }

/* There's been a recent rejection. If the previous rejection was after DATA,
reject now (i.e. reject the MAIL FROM), and set the flag to say so. */

if (!reject->rejected_mail_from)
  {
  *errcode = 550;
  *errmess = sender_is_local?
    "unknown local-part in sender" : "cannot route to sender";
  if (sender_verify_callback_error != NULL)
    *errmess = string_sprintf("%s: %s", *errmess, sender_verify_callback_error);
  reject->rejected_mail_from = TRUE;
  DEBUG(2) debug_printf("%s verification failed after MAIL FROM\n",
    sender_address);
  yield = FALSE;
  }

/* There has been a previous recent rejection after MAIL FROM; the mailer
at the far end is horribly broken. Allow through this MAIL FROM with warning
text, but set refuse_all_rcpts to cause all RCPT TO commands to be failed
with 550 - which seems to be the only thing some mailers understand. We still
need to rewrite the hint record to update the timestamp. */

else
  {
  refuse_all_rcpts = TRUE;
  *errcode = 250;
  *errmess = "reject all recipients: 3 times bad sender";
  if (sender_verify_callback_error != NULL)
    *errmess = string_sprintf("%s: %s", *errmess, sender_verify_callback_error);
  HDEBUG(2) debug_printf("%s verification failed - will reject all recipients\n",
    sender_address);
  yield = TRUE;
  }

/* Update the hint and return; reject_rate = 0.0 => permanent error */

reject->temp_rate = 0.0;
dbfn_write(dbm_file, buffer, reject, sizeof(dbdata_reject));
dbfn_close(dbm_file);
return yield;
}




/*************************************************
*        Second check on sender address          *
*************************************************/


/* This function is called when a message has been completely read, but the
headers haven't yet been written to the spool file, if the sender_verify
option is set. The sender check actually took place in the preliminary
function; its result is left in sender_ok. If it is bad, it may (depending on
the configuration) be permitted to replace it with a value taken from one of
the headers (From, Sender) if that address is viable.

Arguments:
  errcode    set this to an SMTP error code on failure
  errmess    set this to point to an error message on failure

Returns:     TRUE if address verified or fixed up, FALSE otherwise
*/

BOOL
verify_sender(int *errcode, char **errmess)
{
open_db dbblock;
open_db *dbm_file;
dbdata_reject newreject;
char buffer[256];

/* The sender verified correctly on the preliminary check. However, sometimes
we are required to inspect the headers as well. */

switch(sender_ok)
  {
  /* Wasn't really OK, but we are running in warning mode. If sender_verify_
  fixup is set, log what it would have done if we weren't running in warning
  mode. */

  case SENDER_OK_WARNING:
  if (sender_verify_fixup)
    {
    char *newaddr, *header_id;
    if (verify_get_header_address(&newaddr, &header_id, FAIL) == OK)
      log_write(0, LOG_MAIN, "return-path %s is rewritable as %s using %s",
        sender_address, newaddr, header_id);
    }

  /* Fall through */

  /* Sender was really, really OK. Verify a sender in an appropriate header
  if configured to do so. */

  case SENDER_OK_OK:
  if (headers_sender_verify ||
      (sender_address[0] == 0 && headers_sender_verify_errmsg))
    {
    switch (verify_get_header_address(NULL, NULL, OK))
      {
      case DEFER:
      if (headers_checks_fail)
        {
        DEBUG(2) debug_printf("verification of sender from message "
          "headers deferred\n");
        *errcode = 451;
        *errmess = string_sprintf(
          "can't currently verify any sender in the header lines "
          "(envelope sender is <%.1024s>) - try later", sender_address);
        return FALSE;
        }

      log_write(3, LOG_REJECT, "warning: from%s <%.1024s>: can't check for "
        "valid sender in headers", host_and_ident(" ", NULL), sender_address);
      break;

      case FAIL:
      if (headers_checks_fail)
        {
        *errcode = 550;
        *errmess = (verify_address_parse_error == NULL)?
          string_sprintf("there is no valid sender in any header line "
            "(envelope sender is <%.1024s>)", sender_address) :
          string_sprintf("%s (envelope sender is <%.1024s>)",
            verify_address_parse_error,
            sender_address);
        return FALSE;
        }

      log_write(3, LOG_REJECT, "warning: from%s <%.1024s>: %s",
        host_and_ident(" ", NULL), sender_address,
        (verify_address_parse_error == NULL)?
          "no valid sender in headers" : verify_address_parse_error);
      break;
      }
    }

  /* Fall through */

  /* Sender was accepted by exception. Don't verify a sender from the headers
  in this case. */

  case SENDER_OK_EXCEPTION:
  return TRUE;
  }

/* Original sender not OK, or the verification attempt deferred. If configured,
attempt a fix-up by scanning the messages's originator headers for a valid
address. */

if (sender_verify_fixup)
  {
  char *newaddr, *header_id;
  if (verify_get_header_address(&newaddr, &header_id, FAIL) == OK)
    {
    log_write(0, LOG_MAIN, "return-path %s rewritten as %s using %s",
      sender_address, newaddr, header_id);
    header_add(htype_other,
      "X-BadReturnPath: %s rewritten as %s\n  using \"%s\" header\n",
      sender_address, newaddr, header_id);
    sender_address = newaddr;
    return TRUE;
    }
  }

/* Set up for temporary or permanent rejection */

if (sender_ok == SENDER_OK_DEFER)
  {
  *errcode = 451;
  *errmess = string_sprintf(
    "temporarily unable to verify sender address <%.1024s> (try later)",
    sender_address);
  }
else
  {
  *errcode = 550;
  *errmess = string_sprintf(sender_is_local?
    "unknown local part in sender <%.1024s>" :
    "cannot route to sender <%.1024s>",
    sender_address);
  }

if (sender_verify_callback_error != NULL)
  *errmess = string_sprintf("%s: %s", *errmess, sender_verify_callback_error);

/* Set up the key name and open the hints database. O_RDWR (rather than
O_WRONLY) is needed by Berkeley native DB even when reading only. If the
database won't open, we can do no more. */

snprintf(buffer, sizeof(buffer), "%s:%.200s", sender_address,
  (sender_host_name != NULL)? sender_host_name :
  (sender_host_address != NULL)? sender_host_address : "");

dbm_file = dbfn_open("reject", O_RDWR|O_CREAT, &dbblock, FALSE);

/* Write a new record for this failure. A temporary failure rate of 0.0
indicates a permanent error. Otherwise initialize to "1 per hour". */

if (dbm_file != NULL)
  {
  newreject.rejected_mail_from = FALSE;
  newreject.temp_rate = (sender_ok == SENDER_OK_DEFER)? 1.0 : 0.0;
  dbfn_write(dbm_file, buffer, &newreject, sizeof(dbdata_reject));
  dbfn_close(dbm_file);
  }

return FALSE;
}





/*************************************************
*            Get RFC 1413 identification         *
*************************************************/

/* Attempt to get an id from the sending machine via the RFC 1413 protocol. If
the timeout is set to zero, then the query is not done. There may also be lists
of hosts and nets which are exempt. We copy as many bytes from the result as
will fit into ident_buffer and then free the store that ident_id has got
(several K, I think). To guard against malefactors sending non-printing
characters which could, for example, disrupt a message's headers, make sure the
string consists of printing characters only.

Argument: the socket of the connection for which the ident value is required
Returns:  nothing

Side effect: any received ident value is put in sender_ident (NULL otherwise)
*/

void
verify_get_ident(int socket)
{
sender_ident = NULL;
if (rfc1413_query_timeout > 0 && verify_check_host(&rfc1413_hosts, FALSE))
  {
  char *ident_ptr = ident_id(socket, rfc1413_query_timeout);
  if (ident_ptr != NULL)
    {
    strncpy(ident_buffer, ident_ptr, sizeof(ident_buffer));
    ident_buffer[sizeof(ident_buffer) - 1] = 0;
    sender_ident = string_printing(ident_buffer);
    DEBUG(9) debug_printf("sender_ident = %s\n", sender_ident);
    /* NB: free() not store_free() as it wasn't got by store_malloc() */
    free(ident_ptr);
    }
  else DEBUG(9) debug_printf("ident callback failed\n");
  }
}




/*************************************************
*      Match host to a single host-list item     *
*************************************************/

/* This function compares a host (name or address) against a single item
from a host list. The host name gets looked up if it is needed and is not
already known. If this fails, hm_ff is yielded.

Arguments:
  ss             the host-list item
  host_name      the host name or NULL, implying use sender_host_name and
                   sender_host_aliases, looking them up if required
  host_address   the host address
  host_ipv4      the IPv4 address taken from an IPv6 one
  host_ident     the host_ident, or NULL

Returns:         hm_yes    for a positive match
                 hm_no     for a negative match
                 hm_pos    for no match, last one was positive
                 hm_neg    for no match, last one was negative
                 hm_ff     failed to find the host name
*/

enum { hm_yes, hm_no, hm_pos, hm_neg, hm_ff };  /* Return codes */

static int
check_host(char *ss, char *host_name, char *host_address, char *host_ipv4,
  char *host_ident)
{
int matchyield;
int maskoffset;
char *semicolon;
char *t = strchr(ss, '@');

/* If there's an '@' in the string that is not the last character, it starts
with an ident string. If the test string starts with \ just ignore that
character; otherwise if it starts with ! the ident test is negated. If no
ident information is available, skip this item. */

if (t != NULL && t[1] != 0)
  {
  BOOL match = TRUE;
  if (host_ident == NULL) return hm_pos;
  if (*ss == '\\') ss++;
    else if (*ss == '!') { ss++; match = FALSE; }
  if ((strncmp(ss, host_ident, t - ss) == 0) != match) return hm_pos;
  ss = t + 1;      /* Point to the host bit */
  }

/* Starting with ! specifies a negative host item. */

if (*ss == '!')
  {
  matchyield = hm_no;
  while (isspace((uschar)(*(++ss))));
  }
else matchyield = hm_yes;

/* Optimize for the special case when the name is "*". */

if (*ss == '*' && ss[1] == 0) return matchyield;

/* If the pattern is precisely "@" then match against the primary host name */

if (*ss == '@' && ss[1] == 0) ss = primary_hostname;

/* If the pattern is an IP address, optionally followed by a bitmask count,
mask it and the current IP address, and do a binary comparison on them.
The function string_is_ip_address(), when given a non-NULL second argument,
puts the offset of a '/' in there and clobbers the '/' to a zero. */

if (string_is_ip_address(ss, &maskoffset))
  {
  int i;
  int address[4];
  int incoming[4];
  int mlen;
  int size = host_aton(ss, address);
  int insize;

  /* No mask => all bits to be checked */

  if (maskoffset == 0) mlen = 99999;    /* Big number */
  else
    {
    mlen = atoi(ss + maskoffset + 1);
    ss[maskoffset] = '/';    /* restore the slash */
    }

  /* Convert the incoming address to binary. */

  insize = host_aton(host_address, incoming);

  /* Convert IPv4 addresses given in IPv6 compatible mode, which represent
  connections from IPv4 hosts to IPv6 hosts, that is, addresses of the form
  ::ffff:<v4address>, to IPv4 format. */

  if (insize == 4 && incoming[0] == 0 && incoming[1] == 0 &&
      incoming[2] == 0xffff)
    {
    insize = 1;
    incoming[0] = incoming[3];
    }

  /* No match if the sizes don't agree. */

  if (insize != size) goto NOMATCH;

  /* Else do the masked comparison. */

  for (i = 0; i < size; i++)
    {
    int mask;
    if (mlen == 0) mask = 0;
    else if (mlen < 32)
      {
      mask = (-1) << (32 - mlen);
      mlen = 0;
      }
    else
      {
      mask = -1;
      mlen -= 32;
      }
    if ((incoming[i] & mask) != (address[i] & mask)) goto NOMATCH;
    }
  return matchyield;
  }

/* If the item is of the form net24-dbm;/some/file then it is a file lookup
on a masked IP network, in textual form. */

if (strncmp(ss, "net", 3) == 0 && (semicolon = strchr(ss, ';')) != NULL)
  {
  int mlen = 0;
  for (t = ss + 3; isdigit((uschar)*t); t++) mlen = mlen * 10 + *t - '0';
  if (t >= ss + 3 && *t++ == '-')
    {
    int insize;
    int search_type;
    int incoming[4];
    void *handle;
    char *tt;
    char *error;
    char *filename;
    char buffer[64];

    /* If no mask was supplied, set a negative value */

    if (mlen == 0 && t == ss+4) mlen = -1;

    /* Find the search type */

    *semicolon = 0;
    search_type = search_findtype(t, &error);
    *semicolon = ';';

    if (search_type < 0) log_write(0, LOG_PANIC_DIE, "%s", error);

    /* Set up the key for the lookup from the current IP address, masked
    appropriately, and reconverted to text form, with the mask appended. */

    insize = host_aton(host_address, incoming);
    host_mask(insize, incoming, mlen);
    (void)host_nmtoa(insize, incoming, mlen, buffer);

    /* For query-style lookups, the query has to be expanded in order
    to get the key inserted into the query. The global lookup_key contains
    the value to replace $key in the expansion. */

    if (mac_islookup(search_type, lookup_querystyle))
      {
      lookup_key = buffer;
      tt = expand_string(semicolon+1);
      lookup_key = NULL;
      if (tt == NULL)
        log_write(0, LOG_MAIN|LOG_PANIC_DIE, "failed to expand %s query %s: %s",
          lookup_list[search_type].name, semicolon+1, expand_string_message);
      filename = NULL;
      }
    else
      {
      tt = buffer;
      filename = semicolon + 1;
      while (isspace((uschar)*filename)) filename++;
      }

    /* Now do the actual lookup; note that there is no search_close() because
    of the caching arrangements. */

    handle = search_open(filename, search_type, 0, NULL, NULL, &error);
    if (handle == NULL) log_write(0, LOG_PANIC_DIE, "%s", error);
    if (search_find(handle, filename, tt, -1, NULL, &error) != NULL)
      return matchyield;
    goto NOMATCH;
    }
  }

/* The pattern is not an IP address or network reference of any kind. That is,
it is a host name pattern. Check the characters of the pattern to see if they
comprise only letters, digits, full stops, and hyphens (the constituents of
domain names). Allow underscores, as they are all too commonly found. Sigh. */

for (t = ss; *t != 0; t++)
  if (!isalnum((uschar)*t) && *t != '.' && *t != '-' && *t != '_') break;

/* If the pattern is a complete domain name, with no fancy characters, look up
its IP address and match against that. Note that a multi-homed host will add
items to the chain. */

if (*t == 0)
  {
  host_item h;
  h.next = NULL;
  h.name = ss;
  h.address = NULL;
  if (host_find_byname(&h, NULL, NULL, FALSE) != HOST_FIND_FAILED)
    {
    host_item *hh;
    for (hh = &h; hh != NULL; hh = hh->next)
      {
      if (strcmp(hh->address, (strchr(hh->address, ':') == NULL)?
        host_ipv4 : host_address) == 0)
          return matchyield;
      }
    }
  goto NOMATCH;
  }

/* All subsequent comparisons require the host name, and can be done using the
general string matching function. When this function is called for outgoing
hosts, the name is always given. If it is NULL, it means we must use
sender_host_name and its aliases, looking them up if necessary. */

if (host_name == NULL)
  {
  if (sender_host_name == NULL)
    {
    if (host_lookup_failed || !host_name_lookup()) return hm_ff;
    host_build_sender_fullhost();
    }

  /* Match on the sender host name, using the general matching function */

  if (match_check_string(sender_host_name, ss, -1, TRUE, TRUE, NULL))
    return matchyield;

  /* If there are aliases, try matching on them if we are dealing with the
  sender host name. */

  if (!search_find_defer && sender_host_aliases != NULL)
    {
    char **aliases = sender_host_aliases;
    while (*aliases != NULL)
      {
      if (match_check_string(*aliases++, ss, -1, TRUE, TRUE, NULL))
        return hm_yes;
      if (search_find_defer) break;
      }
    }
  }

/* Otherwise match on the given host name. */

else if (match_check_string(host_name, ss, -1, TRUE, TRUE, NULL))
  return matchyield;

/* If a lookup deferred, return hm_ff */

if (search_find_defer) return hm_ff;

/* Failed to match this item; return whether it was positive or negative. */

NOMATCH:

return (matchyield == hm_yes)? hm_pos : hm_neg;
}




/*************************************************
*    Check a specific host matches a host list   *
*************************************************/

/* This function is passed a host list containing items in a number of
different formats and the identity of a host. Its job is to determine whether
the given host is in the set of hosts defined by the list. If any item starts
with "ident@" then the rfc1413 ident data is also checked. The host name is
passed as a pointer so that it can be looked up if needed and not already
known. This is commonly the case when called from verify_check_host() to check
an incoming connection. When called from elsewhere the host name should usually
be set.

Arguments:
  listptr              pointer to the host list
  find_failed_return   the value to return when a name can't be found
  host_name            pointer to the host name, or NULL to indicate
                         sender_host_name/sender_host_aliases
  host_address         the IP address
  host_ident           the ident or NULL

Returns:    TRUE if the host (+ident) is in the defined set
            FALSE if the host (+ident) is not in the defined set

If the host name was needed in order to make a comparison, and could not be
determined from the IP address, the result is find_failed_return unless the
item "+allow_unknown" was met earlier in the list, in which case the opposite
is returned. */

BOOL
verify_check_this_host(char **listptr, BOOL find_failed_return,
   char *host_name, char *host_address, char *host_ident)
{
BOOL endyield = FALSE;
BOOL ffr = find_failed_return;
BOOL log_unknown = FALSE;
int sep = 0;
char *ss;
char *list = *listptr;
char *ot = NULL;
char *host_ipv4;
char buffer[1024];

/* Save time by not scanning for the option name when we don't need it. */

HDEBUG(1) ot = string_sprintf("host in %s?", readconf_find_option(listptr));

/* If the list is empty, the answer is no. */

if (list == NULL)
  {
  HDEBUG(2) debug_printf("%s no (option unset)\n", ot);
  return FALSE;
  }

/* If the sender host address starts off ::ffff: it is an IPv6 address in
IPv4-compatible mode. Find the IPv4 part for checking against IPv4 addresses.
*/

host_ipv4 = (strncmp(host_address, "::ffff:", 7) == 0)?
  host_address + 7 : host_address;

/* Scan the list, testing against each item in turn. There may be an ident
check before each item; after that, items starting with ! are negated and lead
to a FALSE return on matching. The value of yield is set to the match return;
this means that when the end of the list is reached, the return depends on
whether the final item was negated or not. */

while ((ss = string_nextinlist(&list, &sep, buffer, sizeof(buffer))) != NULL)
  {
  /* If the host item is "+allow_unknown", change the value returned for a
  subsequent failed reverse lookup. Also for "+warn_unknown", but in that
  case set a flag for logging. */

  if (strcmp(ss, "+allow_unknown") == 0 || strcmp(ss, "+warn_unknown") == 0)
    {
    ffr = !find_failed_return;
    log_unknown = ss[1] == 'w';
    continue;
    }

  /* If the item does not start with / or !/ it is a single item (but allow
  for spaces after the !) */

  if (ss[strspn(ss, "! \t")] != '/')
    {
    switch(check_host(ss, host_name, host_address, host_ipv4, host_ident))
      {
      case hm_yes:         /* matched a positive item */
      HDEBUG(2) debug_printf("%s yes (%s)\n", ot, ss);
      return TRUE;

      case hm_no:          /* matched a negative item */
      HDEBUG(2) debug_printf("%s no (%s)\n", ot, ss);
      return FALSE;

      case hm_pos:         /* failed; last item was positive */
      endyield = FALSE;
      break;

      case hm_neg:         /* failed; last item was negative */
      endyield = TRUE;
      break;

      case hm_ff:          /* host name lookup failed - this can only be for */
      HDEBUG(2)            /* an incoming host */
        debug_printf("%s %s (failed to find host name or lookup deferred)\n",
          ot, ffr? "yes" : "no");
      if (ffr != find_failed_return && log_unknown)
        log_write(0, LOG_MAIN, "failed to find host name for %s: "
          "permitted by +warn_unknown", sender_fullhost);
      return ffr;
      }
    }

  /* Handle the case of an inline file, possible negated. */

  else
    {
    FILE *f;
    BOOL yes;
    char fbuffer[1024];

    if (*ss == '!')
      {
      yes = FALSE;
      while (isspace((uschar)(*(++ss))));
      }
    else yes = TRUE;

    f = fopen(ss, "r");

    /* ot will be NULL if we are not debugging; in any case, we get better
    wording by not using it. */

    if (f == NULL)
      {
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s",
        string_open_failed(errno, "%s when checking %s", ss,
        readconf_find_option(listptr)));
      }

    while (fgets(fbuffer, sizeof(fbuffer), f) != NULL)
      {
      if ((ss = strchr(fbuffer, '#')) != NULL) *ss = 0; /* trailing comment */

      ss = fbuffer + (int)strlen(fbuffer);              /* trailing space */
      while (ss > fbuffer && isspace((uschar)ss[-1])) ss--;
      *ss = 0;

      ss = fbuffer;
      while (isspace((uschar)*ss)) ss++;                /* leading space */

      if (*ss == 0) continue;                           /* ignore empty */

      switch(check_host(ss, host_name, host_address, host_ipv4, host_ident))
        {
        case hm_yes:         /* matched a positive item */
        HDEBUG(2) debug_printf("%s %s (%s in %s)\n", ot, yes? "yes" : "no",
          ss, buffer);
        fclose(f);
        return yes;

        case hm_no:          /* matched a negative item */
        HDEBUG(2) debug_printf("%s %s (%s in %s)\n", ot, yes? "no" : "yes",
          ss, buffer);
        fclose(f);
        return !yes;

        case hm_pos:         /* failed; last item was positive */
        endyield = !yes;
        break;

        case hm_neg:         /* failed; last item was negative */
        endyield = yes;
        break;

        case hm_ff:          /* host name lookup failed - this can only be */
        HDEBUG(2)            /* for an incoming host */
          debug_printf("%s %s (failed to find host name or lookup deferred)\n",
            ot, ffr? "yes" : "no");
        if (ffr != find_failed_return && log_unknown)
          log_write(0, LOG_MAIN, "failed to find host name for %s: "
            "permitted by +warn_unknown", sender_fullhost);
        fclose(f);
        return ffr;
        }
      }
    fclose(f);
    }
  }

/* End of list reached: return depends on sense of last item. */

HDEBUG(2) debug_printf("%s %s (end of list)\n", ot, endyield? "yes" : "no");

return endyield;
}




/*************************************************
*      Check the remote host matches a list      *
*************************************************/

/* This is little more than a front end to verify_check_this_host(), used
because it occurs very frequently in smtp_in.c. With luck, a good compiler will
spot the tail recursion and optimize it. If there's no host address, this is
command-line SMTP input, and no checking can be done. The host name is passed
as a char ** so that it can be looked up if needed and not already known.

Arguments:
  listptr              pointer to the host list
  find_failed_return   the value to return when a name can't be found

Returns:               the yield of verify_check_this_host()
*/

BOOL
verify_check_host(char **listptr, BOOL find_failed_return)
{
if (sender_host_address == NULL) return FALSE;
return verify_check_this_host(listptr, find_failed_return, NULL,
  sender_host_address, sender_ident);
}

/* End of verify.c */
