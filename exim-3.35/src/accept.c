/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Code for receiving a message and setting up spool files. */


#include "exim.h"


/* Uncomment this definition to restore the code that recognizes LF
only after CR for incoming SMTP from a remote host. */

/* #define STRICT_CRLF */



/*************************************************
*                Local static variables          *
*************************************************/

static FILE *data_file = NULL;
static int  data_fd = -1;

static int   message_sequence_number = 0;
static int   thismessage_size_limit = 0;
static char  last_message_id[MESSAGE_ID_LENGTH + 1];
static int   last_received_time = 0;
static char  spool_name[256];



/*************************************************
*     Check space on spool and log partitions    *
*************************************************/

/* This function is called before accepting a message; if any thresholds are
set, it checks them. If a message_size is supplied, it checks that there is
enough space for that size plus the threshold - i.e. that the message won't
reduce the space to the threshold. Not all OS have statvfs(); for those that
don't, this function always returns TRUE. For some OS the old function and
struct name statfs is used; that is handled by a macro, defined in exim.h.

Arguments:
  msg_size     the (estimated) size of an incoming message

Returns:       FALSE if there isn't enough space, or if the information cannot
                 be obtained
               TRUE if no check was done or there is enough space
*/

BOOL
accept_check_fs(int msg_size)
{
#ifdef HAVE_STATFS
BOOL rc = TRUE;
struct STATVFS statbuf;

memset(&statbuf, 0, sizeof(statbuf));

/* The field names are macros, because not all OS have F_FAVAIL and it seems
tidier to have a macro for F_BAVAIL as well. Some kinds of file server do not
have inodes, and they return -1 for the number available, so we do the check
only when this field is non-negative. */

if (check_spool_space > 0 || msg_size > 0 || check_spool_inodes > 0)
  {
  if (STATVFS(spool_directory, &statbuf) != 0 ||
      statbuf.F_BAVAIL < (check_spool_space + msg_size) / statbuf.F_FRSIZE ||
      (statbuf.F_FAVAIL >= 0 && statbuf.F_FAVAIL < check_spool_inodes))
    rc = FALSE;
  DEBUG(5) debug_printf("spool directory %s space = %d blocks; inodes = %d; "
    "check_space = %d (%d blocks); inodes = %d; msg_size = %d (%d blocks)\n",
    spool_directory, statbuf.F_BAVAIL, statbuf.F_FAVAIL, check_spool_space,
    check_spool_space / statbuf.F_FRSIZE, check_spool_inodes,
    msg_size, msg_size / statbuf.F_FRSIZE);
  if (!rc) return FALSE;
  }

/* Need to cut down the log file path to the directory, and to
ignore any appearance of "syslog" in it. */

if (check_log_space > 0 || check_log_inodes > 0)
  {
  char *path;
  if (log_file_path[0] == 0)
    path = string_copy(spool_directory);
  else
    {
    int sep = ':';              /* Not variable - outside scripts use */
    char *cp;
    char *p = log_file_path;
    char buffer[1024];

    while ((path = string_nextinlist(&p, &sep, buffer, sizeof(buffer))) != NULL)
      {
      if (strcmp(path, "syslog") != 0) break;
      }

    if (path == NULL) return TRUE;    /* No log files, so no problem */

    if ((cp = strrchr(path, '/')) == NULL)
      {
      DEBUG(5) debug_printf("cannot find slash in %s\n", path);
      return FALSE;
      }
    *cp = 0;
    }

  if (STATVFS(path, &statbuf) != 0 ||
      statbuf.F_BAVAIL < check_log_space / statbuf.F_FRSIZE ||
      statbuf.F_FAVAIL < check_log_inodes) rc = FALSE;

  DEBUG(5) debug_printf("log directory %s space = %d blocks; inodes = %d; "
    "check_space = %d (%d blocks); inodes = %d\n",
    path,
    statbuf.F_BAVAIL, statbuf.F_FAVAIL,
    check_log_space, check_log_space / statbuf.F_FRSIZE, check_log_inodes);

  if (!rc) return FALSE;
  }

#endif
return TRUE;
}



/*************************************************
*         Bomb out while reading a message       *
*************************************************/

/* The common case of wanting to bomb out is if a SIGTERM or SIGINT is
received, or if there is a timeout. A rarer case might be if the log files are
screwed up and Exim can't open them to record a message's arrival. Handling
that case is done by setting a flag to cause the log functions to call this
function if there is an ultimate disaster. That is why it is globally
accessible.

Arguments:   SMTP response to give if in an SMTP session
Returns:     it doesn't
*/

void
accept_bomb_out(char *msg)
{
/* If spool_name is set, it contains the name of the data file that is being
written. Unlink it before closing so that it cannot be picked up by a delivery
process. Ensure that any header file is also removed. */

if (spool_name[0] != 0)
  {
  unlink(spool_name);
  spool_name[(int)strlen(spool_name) - 1] = 'H';
  unlink(spool_name);
  }

/* Now close the file if it is open, either as a fd or a stream. */

if (data_file != NULL) fclose(data_file);
  else if (data_fd >= 0) close(data_fd);

/* Attempt to close down an SMTP connection tidily. */

if (smtp_input)
  {
  if (!smtp_batched_input)
    {
    smtp_printf("421 %s %s - closing connection.\r\n", primary_hostname, msg);
    mac_smtp_fflush();
    }

  /* Control does not return from moan_smtp_batch(). */

  else moan_smtp_batch(NULL, "421 %s - message abandoned", msg);
  }

/* Exit from the program (non-BSMTP cases) */

exim_exit(EXIT_FAILURE);
}


/*************************************************
*              Data read timeout                 *
*************************************************/

/* Handler function for timeouts that occur while reading the data that
comprises a message.

Argument:  the signal number
Returns:   nothing
*/

static void
data_timeout_handler(int sig)
{
char *msg = NULL;

sig = sig;    /* Keep picky compilers happy */

if (smtp_input)
  {
  msg = "SMTP incoming data timeout";
  log_write(4, LOG_MAIN, "SMTP data timeout (message abandoned) on connection "
  "from %s",
  (sender_fullhost != NULL)? sender_fullhost : "local process");
  }
else
  {
  fprintf(stderr, "exim: timed out while reading - message abandoned\n");
  log_write(4, LOG_MAIN, "timed out while reading local message");
  }

accept_bomb_out(msg);   /* Does not return */
}



/*************************************************
*           SIGTERM or SIGINT received           *
*************************************************/

/* Handler for SIGTERM or SIGINT signals that occur while reading the
data that comprises a message.

Argument:  the signal number
Returns:   nothing
*/

static void
data_sigterm_sigint_handler(int sig)
{
char *msg = NULL;

if (smtp_input)
  {
  msg = "Service not available - SIGTERM or SIGINT received";
  log_write(0, LOG_MAIN, "SMTP connection%s%s closed after %s",
    (sender_fullhost != NULL)? " from " : "",
    (sender_fullhost != NULL)? sender_fullhost : "",
    (sig == SIGTERM)? "SIGTERM" : "SIGINT");
  }
else
  {
  if (filter_test == NULL)
    {
    fprintf(stderr, "\nexim: %s received - message abandoned\n",
      (sig == SIGTERM)? "SIGTERM" : "SIGINT");
    log_write(0, LOG_MAIN, "%s received while reading local message",
      (sig == SIGTERM)? "SIGTERM" : "SIGINT");
    }
  }

accept_bomb_out(msg);    /* Does not return */
}



/*************************************************
*          Add new recipient to list             *
*************************************************/

/* This function builds a list of recipient addresses in argc/argv
format.

Argument:
  receiver   the next address to add to recipients_list
  orcpt      the DSN orcpt field, if any (ignored if support not configured)
  flags      address flags
  pno        parent number for fixed aliases

Returns:     nothing
*/

void
accept_add_recipient(char *receiver, char *orcpt, int flags, int pno)
{
if (recipients_count >= recipients_list_max)
  {
  recipient_item *oldlist = recipients_list;
  int oldmax = recipients_list_max;
  recipients_list_max = recipients_list_max? 2*recipients_list_max : 50;
  recipients_list = store_get(recipients_list_max * sizeof(recipient_item));
  if (oldlist != NULL)
    memcpy(recipients_list, oldlist, oldmax * sizeof(recipient_item));
  }

#ifdef SUPPORT_DSN
recipients_list[recipients_count].orcpt = orcpt;
#else
orcpt = orcpt;    /* Keep picky compilers happy */
#endif

recipients_list[recipients_count].address = receiver;
recipients_list[recipients_count].flags = flags;
recipients_list[recipients_count++].pno = pno;
}




/*************************************************
*     Read data portion of a non-SMTP message    *
*************************************************/

/* This function is called to read the remainder of a message (following the
headers) when the input is not from SMTP - we are receiving a local message on
a standard input stream. The message is always terminated by EOF, and is also
terminated by a dot on a line by itself if the flag dot_ends is TRUE. Split the
two cases for maximum efficiency.

Ensure that the body ends with a newline. This will naturally be the case when
the termination is "\n.\n" but may not be otherwise. The RFC defines messages
as "sequences of lines" - this of course strictly applies only to SMTP, but
deliveries into BSD-type mailbox files also require it. Exim used to have a
flag for doing this at delivery time, but as it was always set for all
transports, I decided to simplify things by putting the check here instead.

There is at least one broken MUA (dtmail) that sends CRLF via this interface.
We pander slightly to it by having an option to drop *all* CRs.

Arguments:
  fin       a FILE from which to read the message
  fout      a FILE to which to write the message

Returns:    One of the END_xxx values indicating why it stopped reading
*/

static int
read_message_data(FILE *fin, FILE *fout)
{
int ch_state;
register int ch;

/* Handle the case when only EOF terminates the message */

if (!dot_ends)
  {
  register int last_ch = '\n';
  while ((ch = (accept_getc)(fin)) != EOF)
    {
    if (drop_cr && ch == '\r') continue;
    if (fputc(ch, fout) == EOF) return END_WERROR;
    if (ch == '\n') body_linecount++;
    last_ch = ch;
    if (++message_size > thismessage_size_limit) return END_SIZE;
    }
  if (last_ch != '\n')
    {
    if (fputc('\n', fout) == EOF) return END_WERROR;
    body_linecount++;
    message_size++;
    }
  return END_EOF;
  }

/* Handle the case when a dot on its own or EOF terminate */

ch_state = 1;

while ((ch = (accept_getc)(fin)) != EOF)
  {
  if (drop_cr && ch == '\r') continue;
  switch (ch_state)
    {
    case 0:                         /* Normal state */
    if (ch == '\n')
      { body_linecount++; ch_state = 1; }
    break;

    case 1:                         /* After "\n" */
    if (ch == '.')
      {
      ch_state = 2;
      continue;
      }
    if (ch != '\n') ch_state = 0;
    break;

    case 2:                         /* After "\n." */
    if (ch == '\n') return END_DOT;
    if (message_size++, fputc('.', fout) == EOF) return END_WERROR;
    ch_state = 0;
    break;
    }

  if (fputc(ch, fout) == EOF) return END_WERROR;
  if (++message_size > thismessage_size_limit) return END_SIZE;
  }

/* Get here if EOF read */

switch(ch_state)
  {
  case 2:
  if (fputc('.', fout) == EOF) return END_WERROR;
  message_size++;
  /* Fall through */

  case 0:
  if (fputc('\n', fout) == EOF) return END_WERROR;
  body_linecount++;
  message_size++;
  break;
  }

return END_EOF;
}




/*************************************************
*      Read data portion of an SMTP message      *
*************************************************/

/* This function is called to read the remainder of an SMTP message (after the
headers), or to skip over it when an error has occurred. In this case, the
output file is passed as NULL.

If any line begins with a dot, that character is skipped. The input should only
be successfully terminated by CR LF . CR LF unless it is local (non-network)
SMTP, in which case the CRs are optional, but...

FUDGE: It seems that sites on the net send out messages with just LF
terminators, despite the warnings in the RFCs, and other MTAs handle this. So
we make the CRs optional in all cases. Restore the original spec by
uncommenting the #definition of STRICT_CRLF at the head of this file.

Arguments:
  fin       a FILE from which to read the message
  fout      a FILE to which to write the message; NULL if skipping

Returns:    One of the END_xxx values indicating why it stopped reading
*/

static int
read_message_data_smtp(FILE *fin, FILE *fout)
{
int ch_state = 0;
register int ch;

while ((ch = (accept_getc)(fin)) != EOF)
  {
  switch (ch_state)
    {
    case 0:                             /* After LF or CRLF */
    if (ch == '.')
      {
      ch_state = 3;
      continue;                         /* Don't ever write . after LF */
      }
    ch_state = 1;

    /* Else fall through to handle as normal char. */

    case 1:                             /* Normal state */
    if (ch == '\n'
          #ifdef STRICT_CRLF
          && sender_local
          #endif
          )
      {
      ch_state = 0;
      body_linecount++;
      }
    else if (ch == '\r')
      {
      ch_state = 2;
      continue;
      }
    break;

    case 2:                             /* After CR */
    if (ch == '\n')
      {
      ch_state = 0;
      body_linecount++;
      }
    else
      {
      message_size++;
      if (fout != NULL && fputc('\r', fout) == EOF) return END_WERROR;
      if (ch != '\r') ch_state = 1; else continue;
      }
    break;

    case 3:                             /* After [CR] LF . */
    if (ch == '\n'
          #ifdef STRICT_CRLF
          && sender_local
          #endif
          )
      return END_DOT;
    if (ch == '\r')
      {
      ch_state = 4;
      continue;
      }
    ch_state = 1;                       /* The dot itself is removed */
    break;

    case 4:                             /* After [CR] LF . CR */
    if (ch == '\n') return END_DOT;
    message_size++;
    if (fout != NULL && fputc('\r', fout) == EOF) return END_WERROR;

    if (ch == '\r')
      {
      ch_state = 2;
      continue;
      }
    ch_state = 1;
    break;
    }

  /* Add the character to the spool file, unless skipping; then loop for the
  next. */

  message_size++;
  if (fout != NULL)
    {
    if (fputc(ch, fout) == EOF) return END_WERROR;
    if (message_size > thismessage_size_limit) return END_SIZE;
    }
  }

/* Fall through here if EOF encountered. This indicates some kind of error,
since a correct message is terminated by [CR] LF . [CR] LF. */

return END_EOF;
}




/*************************************************
*             Swallow SMTP message               *
*************************************************/

/* This function is called when there has been some kind of error while reading
an SMTP message, and the remaining data may need to be swallowed. It is global
because it is called from smtp_closedown() to shut down an incoming call
tidily.

Argument:    a FILE from which to read the message
Returns:     nothing
*/

void
accept_swallow_smtp(FILE *fin)
{
if (message_ended >= END_NOTENDED)
  message_ended = read_message_data_smtp(fin, NULL);
}



/*************************************************
*                 Accept message                 *
*************************************************/

/* Accept a message on the given input, and put it into a pair of
spool files. Either a non-null list of recipients, or the extract flag
will be true, or both. The flag sender_local is true for locally
generated messages, and smtp_input is true if the message is to be
handled using SMTP conventions about termination and lines starting
with dots. For non-SMTP messages, dot_ends is true for dot-terminated
messages.

If a message was successfully read, message_id[0] will be non-zero.

The general actions of this function are:

  . Read the headers of the message (if any) into a chain of store
    blocks.

  . If there is a "sender:" header and the message is locally originated,
    throw it away.

  . If recipients are to be extracted from the message, build the
    recipients list from the headers, removing any that were on the
    original recipients list (unless extract_addresses_remove_arguments is
    false), and at the same time, remove any bcc header that may be present.

  . Get the spool file for the data, sort out its unique name, open
    and lock it (but don't give it the name yet).

  . Generate a "Message-ID" header if the message doesn't have one.

  . Generate a "Received" header.

  . Ensure the recipients list is fully qualified and rewritten if necessary.

  . If there are any rewriting rules, apply them to the sender address
    and also to the headers.

  . If there is no from: header, generate one.

  . If the sender is local, check that from: is correct, and if not, generate
    a Sender header unless message comes from a trusted caller, or this feature
    is disabled.

  . Ensure the presence of "to" or "cc" or "bcc".

  . If there is no "date" header, generate one.

  . Copy the rest of the input, or up to a terminating "." if in
    SMTP or dot_ends mode, to the data file. Leave it open, to hold the lock.

  . Write the envelope and the headers to a new file.

  . Set the name for the header file; close it.

  . Set the name for the data file; close it.

Because this function can potentially be called many times in a single
SMTP connection, we must take care to free any store obtained by
store_malloc(). However, with the new store arrangements, all store should
be got by store_get(), as it will be automatically retrieved after the message
is accepted.

FUDGE: It seems that sites on the net send out messages with just LF
terminators, despite the warnings in the RFCs, and other MTAs handle this. So
we make the CRs optional in all cases. Restore the original spec by
uncommenting the #definition of STRICT_CRLF at the head of this file.

Arguments:
  fin            a FILE to read the message from
  extract_recip  TRUE if recipients are to be extracted from the message's
                   headers

Returns:  TRUE   there are more messages to be read (SMTP input)
          FALSE  there are no more messages to be read (non-SMTP input
                 or SMTP connection collapsed, or other failure)

When reading a message for filter testing, the returned value indicates
whether the headers (which is all that is read) were terminated by '.' or
not. */

BOOL
accept_msg(FILE *fin, BOOL extract_recip)
{
int  i;
int  msg_size;
int  process_info_len = (int)strlen(process_info);
int  error_rc = (error_handling == ERRORS_SENDER)?
       errors_sender_rc : EXIT_FAILURE;
int  header_size = 256;
register int ptr = 0;

BOOL extracted_bcc = FALSE;
BOOL extracted_ignored = FALSE;
BOOL had_zero = FALSE;
BOOL yield = FALSE;
BOOL smtp_yield = TRUE;

#ifdef STRICT_CRLF
BOOL last_was_cr = FALSE;
#endif

flock_t lock_data;
error_block *bad_addresses = NULL;
char *errmsg;

/* Final message to give to SMTP caller */

char *smtp_reply = NULL;

/* Working header pointers */

header_line *h, *next;

/* Pointers to receive the addresses of headers we specifically check
for. */

header_line *from_header = NULL;
header_line *to_header = NULL;
header_line *cc_header = NULL;
header_line *bcc_header = NULL;
header_line *date_header = NULL;
header_line *subject_header = NULL;
header_line *msgid_header = NULL;
header_line *resentmsgid_header = NULL;

/* Variables for use when building the Received: header. */

char *received;
char *timestamp;

/* Release any open files that might have been cached while preparing to
accept the message - e.g. by verifying addresses - because reading a message
might take a fair bit of real time. */

search_tidyup();

/* Initialize the chain of headers by setting up a place-holder for Received:
header, pointed to by the global anchor variable. We keep header_last pointing
to the end of the chain to make adding headers simple. */

header_list = header_last = store_get(sizeof(header_line));
header_list->next = NULL;
header_list->type = htype_other;
header_list->text = NULL;

/* Control block for the next header to be read. */

next = store_get(sizeof(header_line));
next->text = store_get(header_size);

/* Initialize message id to be null (indicating no message read), and the
header names list to be the normal list. Indicate there is no data file open
yet, initialize the size and warning count. */

message_id[0] = 0;
header_names = header_names_normal;
data_file = NULL;
data_fd = -1;
spool_name[0] = 0;
message_size = 0;
warning_count = 0;

/* Initialize overall message size limit; this may get reduced later if
message_size_limit_count_recipients is set. */

thismessage_size_limit = (message_size_limit <= 0)?
  INT_MAX : message_size_limit;

/* While reading the message, body_linecount is computed. The full message_
linecount is set up only when the headers are read back in from the spool for
delivery. */

body_linecount = 0;

/* Remember the time of reception. Be defensive against time warps - on some
systems there are apparently automatic clock correctors that can move the clock
backwards. When several messages are being received by the same process, we
must insist that the clock doesn't do this. */

received_time = time(NULL);
if (received_time >= last_received_time) last_received_time = received_time;
  else received_time = last_received_time;

/* If SMTP input, set the handler for timeouts. The alarm() call happens in the
smtp_getc() function when it refills its buffer. */

if (smtp_input)
  {
  os_non_restarting_signal(SIGALRM, data_timeout_handler);
  }

/* If not SMTP input, timeout happens only if configured, and we just set a
single timeout for the whole message. */

else if (accept_timeout > 0)
  {
  os_non_restarting_signal(SIGALRM, data_timeout_handler);
  alarm(accept_timeout);
  }

/* SIGTERM and SIGINT are caught always. */

signal(SIGTERM, data_sigterm_sigint_handler);
signal(SIGINT, data_sigterm_sigint_handler);

/* Header lines in messages are not supposed to be very long, though when
unfolded, to: and cc: headers can take up a lot of store. We must also cope
with the possibility of junk being thrown at us. Start by getting 256 bytes for
storing the header, and extend this as necessary using string_cat().

To cope with total lunacies, impose an upper limit on the length of the header
section of the message, as otherwise the store will fill up. We must also cope
with the possibility of binary zeros in the data. Hence we cannot use fgets().
Folded header lines are joined into one string, leaving the '\n' characters
inside them, so that writing them out reproduces the input.

Loop for each character of each header; the next structure for chaining the
header is set up already, with ptr the offset of the next character in
next->text. */

for (;;)
  {
  int ch = (accept_getc)(fin);

  /* If we hit EOF on a SMTP connection, it's an error, since incoming
  SMTP must have a correct "." terminator. */

  if (ch == EOF && smtp_input /* && !smtp_batched_input */)
    {
    if (sender_host_unknown)
      {
      log_write(4, LOG_MAIN, "unexpected EOF while reading SMTP data (header) "
        "from %s", sender_ident);
      smtp_reply = "554 Unexpected end of file";
      }
    else
      {
      log_write(4, LOG_MAIN, "unexpected disconnection while reading SMTP data "
        "(header) from %s%s", sender_fullhost, smtp_read_error);
      smtp_reply = "421 Lost incoming connection";
      }
    smtp_yield = FALSE;
    goto TIDYUP;                       /* Skip to end of function */
    }

  /* See if we are at the current header's size limit - there must be at least
  two bytes left (for the char plus zero); if at the limit, extend the text
  buffer. This could have been done automatically using string_cat() but
  because this is a tightish loop storing only one character at a time, we
  choose to do it inline. Normally store_extend() will be able to extend the
  block; only at the end of a big store block will a copy be needed. To handle
  the case of very long headers (and sometimes lunatic messages can have ones
  that are 100s of K long) we call store_release() for strings that have been
  copied - if the string is at the start of a block (and therefore the only
  thing in it, because we aren't doing any other gets), the block gets freed.
  We can only do this because we know there are no other calls to store_get()
  going on. */

  if (ptr >= header_size - 2)
    {
    int oldsize = header_size;
    /* header_size += 256; */
    header_size *= 2;
    if (!store_extend(next->text, oldsize, header_size))
      {
      char *newtext = store_get(header_size);
      memcpy(newtext, next->text, ptr);
      store_release(next->text);
      next->text = newtext;
      }
    }

  /* Cope with receiving a binary zero. There is dispute about whether
  these should be allowed in RFC 822 messages. The middle view is that they
  should not be allowed in headers, at least. Exim takes this attitude at
  the moment. We can't just stomp on them here, because we don't know that
  this line is a header yet. Set a flag to cause scanning later. */

  if (ch == 0) had_zero = TRUE;

  /* Test for termination. Lines in remote SMTP are terminated by CRLF, while
  those from data files use just LF. Treat LF in local SMTP input as a
  terminator too. Treat EOF as a line terminator always.

  FUDGE: There are sites out there that don't send CRs before their LFs, and
  other MTAs accept this. We are therefore forced into this "liberalisation"
  too. To undo it, uncomment the definition of STRICT_CRLF at the head of
  this file. */

  if (ch != EOF && (ch != '\n'
         #ifdef STRICT_CRLF
         || (!sender_local && !last_was_cr)
         #endif
         ))

  /* This is not the end of the line. If this is SMTP input and this is
  the first character in the line and it is a "." character, ignore it.
  This implements the dot-doubling rule, though header lines starting with
  dots aren't exactly common. They are legal in RFC 822, though. If the
  following is CRLF or LF, this is the line that that terminates the
  entire message. We set message_ended to indicate this has happened (to
  prevent further reading), and break out of the loop, having got rid
  of the unwanted header item and set next = NULL. */

    {
    if (ptr == 0 && ch == '.' && (smtp_input || dot_ends))
      {
      ch = (accept_getc)(fin);
      if (ch == '\r')
        {
        ch = (accept_getc)(fin);
        if (ch != '\n')
          {
          accept_ungetc(ch, fin);
          ch = '\r';             /* bare CR is data */
          }
        }
      if (ch == '\n')
        {
        message_ended = END_DOT;
        store_reset(next);
        next = NULL;
        break;
        }
      }

    next->text[ptr++] = ch;    /* Add to buffer */
    message_size++;            /* Total message size so far */

    #ifdef STRICT_CRLF
    last_was_cr = (ch == '\r');
    #endif

    /* Handle failure due to a humungously long header section. The >= allows
    for the terminating \n. Add what we have so far onto the headers list so
    that it gets reflected in any error message, and back up the just-read
    character. */

    if (message_size >= HEADER_MAXSIZE)
      {
      next->text[ptr] = 0;
      next->slen = ptr;
      next->type = htype_other;
      next->next = NULL;
      header_last->next = next;
      header_last = next;

      log_write(0, LOG_MAIN, "ridiculously long message header received from "
        "%s (more than %d characters): message abandoned",
        sender_host_unknown? sender_ident : sender_fullhost, HEADER_MAXSIZE);

      if (smtp_input)
        {
        smtp_reply = "552 Message header is ridiculously long";
        accept_swallow_smtp(fin);
        goto TIDYUP;                             /* Skip to end of function */
        }

      else
        {
        if (error_handling == ERRORS_SENDER)
          {
          if (!moan_to_sender(ERRMESS_VLONGHEADER, NULL, header_list->next,
            fin, FALSE)) error_rc = EXIT_FAILURE;
          }
        else fprintf(stderr, "exim: message header longer than %d characters "
          "received: message not accepted\n", HEADER_MAXSIZE);
        exim_exit(error_rc);
        }
      }

    continue;                  /* With next input character */
    }

  accept_linecount++;          /* For BSMTP errors */

  /* Input line terminated; remove CR from stored text for SMTP input or
  if drop_cr is set (for broken local MUAs). */

  if ((smtp_input || drop_cr) && next->text[ptr-1] == '\r')
    {
    ptr--;
    message_size--;
    }

  /* Now put in the terminating newline. There is always space for
  at least two more characters. */

  next->text[ptr++] = '\n';
  message_size++;

  /* A blank line signals the end of the headers; release the unwanted
  space and set next to NULL to indicate this. */

  if (ptr == 1)
    {
    store_reset(next);
    next = NULL;
    break;
    }

  /* There is data in the line; see if the next input character is a
  whitespace character. If it is, we have a continuation of this header line.
  There is always space for at least one character at this point. */

  if (ch != EOF)
    {
    int nextch = (accept_getc)(fin);
    if (nextch == ' ' || nextch == '\t')
      {
      next->text[ptr++] = nextch;
      message_size++;
      continue;                      /* Iterate the loop */
      }
    else if (nextch != EOF) (accept_ungetc)(nextch, fin);   /* For next time */
    else ch = EOF;                   /* Cause main loop to exit at end */
    }

  /* We have got to the real line end. Terminate the string and release store
  beyond it. If it turns out to be a real header, internal binary zeros will
  be squashed later. */

  next->text[ptr] = 0;
  next->slen = ptr;
  store_reset(next->text + ptr + 1);

  /* Check the running total size against the overall message size limit. We
  don't expect to fail here, but if the overall limit is set less than MESSAGE_
  MAXSIZE and a big header is sent, we want to catch it. Just stop reading
  headers - the code to read the body will then also hit the buffer. */

  if (message_size > thismessage_size_limit) break;

  /* A line that is not syntactically correct for a header also marks
  the end of the headers. In this case, we leave next containing the
  first data line. This might actually be several lines because of the
  continuation logic applied above, but that doesn't matter.

  It turns out that smail, and presumably sendmail, accept leading lines
  of the form

  From ph10 Fri Jan  5 12:35 GMT 1996

  in messages. The "mail" command on Solaris 2 sends such lines. I cannot
  find any documentation of this, but for compatibility it had better be
  accepted. Exim restricts it to the case of non-smtp messages, and
  treats it as an alternative to the -f command line option. Thus it is
  ignored except for trusted users or filter testing. Otherwise it is taken
  as the sender address, unless -f was used (sendmail compatibility).

  It further turns out that some UUCPs generate the From_line in a different
  format, e.g.

  From ph10 Fri, 7 Jan 97 14:00:00 GMT

  The regex for matching these things is now capable of recognizing both
  formats (including 2- and 4-digit years in the latter). In fact, the regex
  is now configurable, as is the expansion string to fish out the sender.

  Even further on it has been discovered that some broken clients send
  these lines in SMTP messages. There is now an option to ignore them from
  specified hosts or networks. Sigh. */

  if (header_last == header_list &&
       (!smtp_input
         ||
         (sender_host_address != NULL &&
           verify_check_host(&ignore_fromline_hosts, FALSE))
         ||
         (sender_host_address == NULL && ignore_fromline_local)
       ) &&
       regex_match_and_setup(regex_From, next->text, 0, -1))
    {
    if ((trusted_caller || untrusted_set_sender || filter_test != NULL)
         && !sender_address_forced)
      {
      char *uucp_sender = expand_string(uucp_from_sender);
      if (uucp_sender == NULL)
        {
        log_write(0, LOG_MAIN|LOG_PANIC,
          "expansion of \"%s\" failed after matching "
          "\"From \" line: %s", uucp_from_sender, expand_string_message);
        }
      else
        {
        int start, end, domain;
        char *errmess;
        char *newsender = parse_extract_address(uucp_sender, &errmess,
          &start, &end, &domain, TRUE);
        if (newsender != NULL)
          {
          if (domain == 0 && newsender[0] != 0)
            newsender = rewrite_address_qualify(newsender, FALSE);
          sender_address = newsender;

          if (trusted_caller || filter_test != NULL)
            {
            authenticated_sender = NULL;
            originator_name = "";
            sender_local = FALSE;
            }

          if (filter_test != NULL)
            printf("Sender taken from \"From\" line\n");
          }
        }
      }
    }

  /* Not a leading "From " line. Check to see if it is a valid header line.
  Header names may contain any non-control characters except space and colon,
  amazingly. */

  else
    {
    char *p = next->text;
    while (mac_isgraph(*p) && *p != ':') p++;
    while (isspace((uschar)*p)) p++;

    /* If not a valid header line, break from the header reading loop, leaving
    next != NULL, indicating that it holds the first line of the body. */

    if (*p != ':') break;

    /* We have a valid header line. If there were any binary zeroes in
    the line, stomp on them here. */

    if (had_zero)
      for (p = next->text; p < next->text + ptr; p++) if (*p == 0) *p = '?';

    /* It is perfectly legal to have an empty continuation line
    at the end of a header, but it is confusing to humans
    looking at such messages, since it looks like a blank line.
    Reduce confusion by removing redundant white space at the
    end. We know that there is at least one printing character
    (the ':' tested for above) so there is no danger of running
    off the end. */

    p = next->text + ptr - 2;
    for (;;)
      {
      while (*p == ' ' || *p == '\t') p--;
      if (*p != '\n') break;
      ptr = (p--) - next->text + 1;
      message_size -= next->slen - ptr;
      next->text[ptr] = 0;
      next->slen = ptr;
      }

    /* Add the header to the chain */

    next->type = htype_other;
    next->next = NULL;
    header_last->next = next;
    header_last = next;

    /* If any resent- headers exist, change the pointer to the active header
    names. */

    if (strncmpic(next->text, "resent-", 7) == 0)
      header_names = header_names_resent;
    }

  /* The line has been handled. If we have hit EOF, break out of the loop,
  indicating no pending data line. */

  if (ch == EOF) { next = NULL; break; }

  /* Set up for the next header */

  header_size = 256;
  next = store_get(sizeof(header_line));
  next->text = store_get(header_size);
  ptr = 0;
  had_zero = FALSE;

  #ifdef STRICT_CRLF
  last_was_cr = FALSE;
  #endif

  }      /* Continue, starting to read the next header */

/* At this point, we have read all the headers into a data structure in main
store. The first header is still the dummy placeholder for the Received: header
we are going to generate a bit later on. If next != NULL, it contains the first
data line - which terminated the headers before reaching a blank line (not the
normal case). */

DEBUG(9)
  {
  debug_printf(">>Original headers (size=%d):\n", message_size-1);
  for (h = header_list->next; h != NULL; h = h->next)
    debug_printf("%s", h->text);
  debug_printf("\n");
  if (next != NULL) debug_printf("%s", next->text);
  }

/* If there is an RBL warning header to chain it on the end of the existing
headers - no need to copy, since it will not be in store that is released after
each message is received. */

if (rbl_header != NULL)
  {
  rbl_header->next = NULL;
  header_last->next = rbl_header;
  header_last = rbl_header;
  }

/* End of file on any SMTP connection is an error. If an incoming SMTP call
is dropped immediately after valid headers, the next thing we will see is EOF.
We must test for this specially, as further down the reading of the data is
skipped if already at EOF. */

if (smtp_input && (accept_feof)(fin))
  {
  if (sender_host_unknown)
    {
    log_write(4, LOG_MAIN, "unexpected EOF while reading SMTP data "
      "(after header) from %s", sender_ident);
    smtp_reply = "554 Unexpected end of file";
    }
  else
    {
    log_write(4, LOG_MAIN, "unexpected disconnection while reading SMTP data "
      "(after header) from %s%s", sender_fullhost, smtp_read_error);
    smtp_reply = "421 Lost incoming connection";
    }
  smtp_yield = FALSE;
  goto TIDYUP;                       /* Skip to end of function */
  }

/* If this is a filter test run and no headers were read, output a warning
in case there is a mistake in the test message. */

if (filter_test != NULL && header_list->next == NULL)
  printf("Warning: no message headers read\n");

/* Scan the headers for those that we must test for. If any resent- headers
exist, then we must use the set of resent- headers, and not use the others, as
mandated by RFC 822. However, we keep both kinds of message-id, so that we can
log message-id if resent-message-id does not exist in the presence of other
resent- headers. If there is a "sender:" header (of the appropriate type) and
the message is locally originated, mark it "old" so that it will not be
transmitted with the message.

The Return-path: header is supposed to be added to messages when they leave the
SMTP system. We shouldn't receive messages that already contain Return-path.
However, since exim generates Return-path: on local delivery, resent messages
may well contain it. We therefore provide an option to remove any Return-path:
headers on input. Removal actually means flagging as "old", which prevents the
header being transmitted with the message.

Similar remarks apply to the non-standard Delivery-date: and Envelope-to:
headers.

If we are testing a mail filter file, use the value of the Return-Path header
to set up the return_path variable, which is not otherwise set. However,
remove any <> that surround the address because the variable doesn't have
these. */

for (h = header_list->next; h != NULL; h = h->next)
  {
  if (header_checkname(h, "return-path", 11))
    {
    if (return_path_remove) h->type = htype_old;
    if (filter_test != NULL)
      {
      char *start = h->text + 12;
      char *end = start + (int)strlen(start);
      while (isspace((uschar)*start)) start++;
      while (end > start && isspace((uschar)end[-1])) end--;
      if (*start == '<' && end[-1] == '>')
        {
        start++;
        end--;
        }
      return_path = string_copyn(start, end - start);
      }
    }
  else if (header_checkname(h, header_names[hn_sender].name,
    header_names[hn_sender].len))
      h->type = (sender_local && !trusted_caller)? htype_old : htype_sender;

  else if (header_checkname(h, header_names[hn_from].name,
    header_names[hn_from].len))
      {
      from_header = h;
      h->type = htype_from;

      /* For Sendmail compatibility, if the From header consists of just the
      login id of the user who called Exim, rewrite it with the gecos field
      first. */

      if (!smtp_input)
        {
        char *s = h->text + header_names[hn_from].len + 1;
        while (isspace((uschar)*s)) s++;
        if (strncmpic(s, originator_login, h->slen - (s - h->text) - 1) == 0)
          {
          header_add(htype_other, "From: %s <%s@%s>\n", originator_name,
            originator_login, qualify_domain_sender);
          h->type = htype_old;
          DEBUG(9) debug_printf("rewrote \"From:\" header using gecos\n");
          }
        }
      }

  else if (header_checkname(h, header_names[hn_to].name,
    header_names[hn_to].len)) { to_header = h; h->type = htype_to; }
  else if (header_checkname(h, header_names[hn_cc].name,
    header_names[hn_cc].len)) { cc_header = h; h->type = htype_cc; }
  else if (header_checkname(h, header_names[hn_bcc].name,
    header_names[hn_bcc].len)) { bcc_header = h; h->type = htype_bcc; }
  else if (header_checkname(h, header_names[hn_replyto].name,
    header_names[hn_replyto].len)) { bcc_header = h; h->type = htype_replyto; }
  else if (header_checkname(h, header_names[hn_date].name,
    header_names[hn_date].len)) date_header = h;
  else if (header_checkname(h, header_names[hn_subject].name,
    header_names[hn_subject].len)) subject_header = h;
  else if (header_checkname(h, "message-id", 10))
    { msgid_header = h; h->type = htype_id; }
  else if (header_checkname(h, "resent-message-id", 17))
    resentmsgid_header = h;
  else if (header_checkname(h, "received", 8)) h->type = htype_received;
  else if (header_checkname(h, "delivery-date", 13) &&
    delivery_date_remove) h->type = htype_old;
  else if (header_checkname(h, "envelope-to", 11) &&
    envelope_to_remove) h->type = htype_old;
  }

/* Extract recipients from the headers if that is required (the -t option).
Note that this is documented as being done *before* any address rewriting takes
place. There are two possibilities:

(1) According to sendmail documentation, any recipients already listed are to
be REMOVED from the message, so we need to build a non-recipients tree for that
list, because in subsequent processing this data is held in a tree and that's
what the spool_write_header() function expects. Make sure that non-recipient
addresses are fully qualified and rewritten if necessary.

(2) It is reported that many sendmails and other MTAs do not in fact do what is
documented, but add extracted recipients to those in the command line
arguments. Therefore, there is an option to make Exim behave this way. */

if (extract_recip)
  {
  int rcount = 0;
  error_block **bnext = &bad_addresses;

  if (extract_addresses_remove_arguments)
    {
    while (recipients_count-- > 0)
      {
      char *s = rewrite_address(recipients_list[recipients_count].address,
        TRUE, TRUE, global_rewrite_rules, rewrite_existflags);
      tree_add_nonrecipient(s, FALSE);
      }
    recipients_list = NULL;
    recipients_count = recipients_list_max = 0;
    }

  parse_allow_group = TRUE;             /* Allow address group syntax */

  /* Now scan the headers */

  for (h = header_list->next; h != NULL; h = h->next)
    {
    if (h->type == htype_to || h->type == htype_cc || h->type == htype_bcc)
      {
      char *s = strchr(h->text, ':') + 1;
      while (isspace((uschar)*s)) s++;

      while (*s != 0)
        {
        char *ss = parse_find_address_end(s, FALSE);
        char *recipient, *errmess, *p, *pp;
        int start, end, domain;

        /* Check on maximum */

        if (recipients_max > 0 && ++rcount > recipients_max)
          {
          if (error_handling == ERRORS_STDERR)
            {
            fprintf(stderr, "exim: too many recipients\n");
            exim_exit(EXIT_FAILURE);
            }
          else
            exim_exit(moan_to_sender(ERRMESS_TOOMANYRECIP, NULL, NULL, stdin,
              FALSE)? error_rc : EXIT_FAILURE);
          }

        /* Make a copy of the address, and remove any internal newlines. These
        may be present as a result of continuations of the header line. The
        white space that follows the newline must not be removed - it is part
        of the header. */

        pp = recipient = store_get(ss - s + 1);
        for (p = s; p < ss; p++) if (*p != '\n') *pp++ = *p;
        *pp = 0;
        recipient = parse_extract_address(recipient, &errmess, &start, &end,
          &domain, FALSE);

        /* Keep a list of all the bad addresses so we can send a single
        error message at the end. However, an empty address is not an error;
        just ignore it. This can come from an empty group list like

          To: Recipients of list:;

        If there are no recipients at all, an error will occur later. */

        if (recipient == NULL && strcmp(errmess, "empty address") != 0)
          {
          int len = (int)strlen(s);
          error_block *b = store_get(sizeof(error_block));
          while (len > 0 && isspace((uschar)s[len-1])) len--;
          b->next = NULL;
          b->text1 = string_printing(string_copyn(s, len));
          b->text2 = errmess;
          *bnext = b;
          bnext = &(b->next);
          }

        /* If the recipient is already in the nonrecipients tree, it must
        have appeared on the command line with the option extract_addresses_
        remove_arguments set. Do not add it to the recipients, and keep a note
        that this has happened, in order to give a better error if there are
        no recipients left. */

        else if (recipient != NULL)
          {
          if (tree_search_addr(tree_nonrecipients, recipient, FALSE) == NULL)
            accept_add_recipient(recipient, NULL, 0, 0);
          else
            extracted_ignored = TRUE;
          }

        /* Move on past this address */

        s = ss + (*ss? 1:0);
        while (isspace((uschar)*s)) s++;
        }

      /* If this was the bcc: header, mark it "old", which means it
      will be kept on the spool, but not transmitted as part of the
      message. */

      if (h->type == htype_bcc)
        {
        h->type = htype_old;
        bcc_header = NULL;
        extracted_bcc = TRUE;
        }
      }
    }

  parse_allow_group = FALSE;      /* Reset group syntax flags */
  parse_found_group = FALSE;
  }

/* Now that we have the recipients, we can adjust the maximum permitted
message size if necessary. */

if (message_size_limit > 0 &&
    message_size_limit_count_recipients &&
    recipients_count > 1)
  thismessage_size_limit /= recipients_count;

/* Now build the unique message id. From smail we copy the idea of using the
current time in base-62 format. Smail adds to this the inode of the data file,
but on fast processors this isn't good enough, because a message can be
completely handled within one second, and another can then arrive and get the
same inode and therefore the same message id. Instead we use the pid of this
process plus a sequence number which gets reset when the time changes, because
a single process is capable of receiving more than one message - indeed, an
SMTP connection might stay for quite some time. Uniqueness now relies on the
fact that process numbers operate on a fairly large cycle, so that it is
unbelievably unlikely that a process could receive a message, finish, and
another process with the same pid could be started to receive another message,
all within the same second.

There doesn't seem to be anything in the RFC which requires a message id to
start with a letter, but Smail was changed to ensure this. The external form of
the message id (as supplied by string expansion) therefore starts with an
additional leading 'E'. The spool file names do not include this leading
letter and it is not used internally.

Note that string_base62() returns its data in a static storage block, so it
must be copied before calling string_base62() again. It always returns exactly
6 characters. We use just 2 of these for the sequence number. This allows a
single process to accept up to 62x62 messages in one second. I think it will be
some time before processors are up to that...

NOTE: If ever the format of message ids is changed, the regular expression for
checking that a string is in this format must be updated in a corresponding
way. It appears in the initializing code in exim.c. The macro MESSAGE_ID_LENGTH
must also be changed to reflect the correct string length.

MODIFICATION: The requirement to cope with host numbers so that a number of
hosts can maintain unique ids has meant a change to the message id format. In
practice, sequence numbers greater than 2 have very rarely been seen. So, if
the host_number has been set we construct a value for the last two 62-digits
as the value (seq * 256) + host_number. This permits sequence numbers up to 14,
which we hope will be good enough. If these run out, wait for a second so that
the time changes. */

strncpy(message_id, string_base62((long int)received_time), 6);
message_id[6] = '-';
strncpy(message_id + 7, string_base62((long int)getpid()), 7);
if (strncmp(message_id, last_message_id, MESSAGE_ID_LENGTH - 3) != 0)
  message_sequence_number = 0;

/* Deal with the case where the host number has to be encoded with the sequence
number. */

if (host_number_string != NULL)
  {
  /* This is, I hope, just defensive programming that won't ever need
  to be exercised! */

  if (message_sequence_number > 14)
    {
    while (received_time <= last_received_time)
      {
      sleep(1);
      received_time = time(NULL);
      }
    last_received_time = received_time;
    strncpy(message_id, string_base62((long int)received_time), 6);
    message_sequence_number = 0;
    }

  sprintf(message_id + MESSAGE_ID_LENGTH - 3, "-%2s",
    string_base62((long int)(message_sequence_number++ *256)+host_number) + 4);
  }

/* The non-host-number case */

else
  sprintf(message_id + MESSAGE_ID_LENGTH - 3, "-%2s",
    string_base62((long int)message_sequence_number++) + 4);

/* Save for comparing with next one */

strcpy(last_message_id, message_id);

/* Add the current message id onto the current process info string if
it will fit. */

(void)string_format(process_info + process_info_len,
  PROCESS_INFO_SIZE - process_info_len, " id=%s", message_id);

/* If we are using multiple input directories, set up the one for this message
to be the least significant base-62 digit of the time of arrival. Otherwise
ensure that it is an empty string. */

message_subdir[0] = split_spool_directory? message_id[5] : 0;

/* Now that we have the message-id, if there is no message-id: header, generate
one. This can be user-configured if required, but we had better flatten any
illegal characters therein. */

if ((header_names == header_names_normal && msgid_header == NULL) ||
    (header_names != header_names_normal && resentmsgid_header == NULL))
  {
  BOOL use_default = TRUE;
  if (message_id_text != NULL)
    {
    char *e = expand_string(message_id_text);
    if (e != NULL)
      {
      if (*e != 0)
        {
        uschar *ee;
        for (ee = (uschar *)e; *ee != 0; ee++)
          if (mac_iscntrl_or_special(*ee)) *ee = '-';
        header_add(htype_id, "%s: <%s.%s@%s>\n", header_names[hn_msgid].name,
          message_id_external, e, primary_hostname);
        use_default = FALSE;
        }
      }
    else
      log_write(0, LOG_MAIN|LOG_PANIC,
        "expansion of \"%s\" (message_id_header_text) "
        "failed: %s", message_id_text, expand_string_message);
    }
  if (use_default)
    header_add(htype_id, "%s: <%s@%s>\n", header_names[hn_msgid].name,
      message_id_external, primary_hostname);
  }

/* Ensure the recipients list is fully qualified and rewritten. If we
are to log recipients, first keep a copy of the raw ones. */

if (log_received_recipients)
  {
  raw_recipients = store_get(recipients_count * sizeof(char *));
  for (i = 0; i < recipients_count; i++)
    raw_recipients[i] = string_copy(recipients_list[i].address);
  }

for (i = 0; i < recipients_count; i++)
  recipients_list[i].address =
    rewrite_address(recipients_list[i].address, TRUE, TRUE,
      global_rewrite_rules, rewrite_existflags);


/* Generate our own Received: header. We must not do this earlier because the
message_id is usually included in the expansion string for this header. We
also want the recipients to be qualified, so that the "for" clause (which works
only if there's a single recipient) shows the qualified address. Generate text
for the received header by expanding the configured string, and also get the
timestamp to put on the end of the text, as required by RFC 822. Note: the
checking for too many Received: headers is handled by the delivery code. */

timestamp = expand_string("${tod_full}");
if (recipients_count == 1) received_for = recipients_list[0].address;
received = expand_string(received_header_text);
received_for = NULL;

if (received == NULL)
  log_write(0, LOG_PANIC_DIE, "Expansion of \"%s\" (received_header_text) "
    "failed: %s", string_printing(received_header_text),
      expand_string_message);

/* The first element on the header chain is reserved for the Received
header, so all we have to do is fill in the text pointer. */

header_list->text = string_sprintf("%s; %s\n", received, timestamp);
header_list->slen = (int)strlen(header_list->text);
header_list->type = htype_received;


/* If there is no From: header, generate one. If there is no sender address,
but the sender is local or this is a local delivery error, use the originator
login. This shouldn't happen for genuine bounces, but might happen for
autoreplies. The addition of From: must be done *before* checking for the
possible addition of a Sender: header, because untrusted_set_sender allows an
untrusted user to set anything in the envelope (which might then get info
From:) but we still want to ensure a valid Sender: if it is required. */

if (from_header == NULL)
  {
  /* Envelope sender is empty */

  if (sender_address[0] == 0)
    {
    if (sender_local || local_error_message)
      {
      header_add(htype_from, "%s: %s%s%s@%s%s\n", header_names[hn_from].name,
        originator_name,
        (originator_name[0] == 0)? "" : " <",
        originator_login,
        qualify_domain_sender,
        (originator_name[0] == 0)? "" : ">");
      }
    }

  /* There is a non-null envelope sender. Build the header using the original
  sender address, before any rewriting that might have been done while
  verifying it. */

  else
    {
    if (!smtp_input || sender_local)
      header_add(htype_from, "%s: %s%s%s%s\n",
        header_names[hn_from].name,
        originator_name,
        (originator_name[0] == 0)? "" : " <",
        (sender_address_unrewritten == NULL)?
          sender_address : sender_address_unrewritten,
        (originator_name[0] == 0)? "" : ">");
    else
      header_add(htype_from, "%s: %s\n", header_names[hn_from].name,
        sender_address);
    from_header = header_last;    /* To get it checked for Sender: */
    }
  }


/* If the sender is local, check that an existing From: is correct, and if not,
generate a Sender: header, unless disabled. Any previously-existing Sender:
header was removed above. Note that sender_local, as well as being TRUE if the
caller of exim is not trusted, is also true if a trusted caller did not supply
a -f argument for non-smtp input. To allow trusted callers to forge From:
without supplying -f, we have to test explicitly here. If the From: header
contains more than one address, then the call to parse_extract_address fails,
and a Sender: header is inserted, as required. */

if (sender_local && local_from_check && !trusted_caller && from_header != NULL)
  {
  BOOL make_sender = TRUE;
  int start, end, domain;
  char *errmess;
  char *from_address =
    parse_extract_address(strchr(from_header->text, ':') + 1, &errmess,
      &start, &end, &domain, FALSE);
  char *login_sender_address = string_sprintf("%s@%s", originator_login,
    qualify_domain_sender);

  /* Remove permitted prefixes and suffixes from the local part
  of the From: address before doing the comparison with the sender. */

  if (from_address != NULL)
    {
    int slen;
    char *at = (domain == 0)? NULL : from_address + domain - 1;

    if (at != NULL) *at = 0;
    from_address += direct_check_prefix(from_address, local_from_prefix);
    slen = direct_check_suffix(from_address, local_from_suffix);
    if (slen > 0)
      {
      memmove(from_address+slen, from_address, (int)strlen(from_address)-slen);
      from_address += slen;
      }
    if (at != NULL) *at = '@';

    if (strcmpic(login_sender_address, from_address) == 0 ||
      (domain == 0 && strcmpic(from_address, originator_login) == 0))
        make_sender = FALSE;
    }

  if (make_sender)
    header_add(htype_sender, "%s: %s <%s>\n",
      header_names[hn_sender].name, originator_name, login_sender_address);
  }


/* If there are any rewriting rules, apply them to the sender address, unless
it has already been rewritten as part of verification for SMTP input. */

if (global_rewrite_rules != NULL && sender_address_unrewritten == NULL &&
    sender_address[0] != 0)
  {
  sender_address = rewrite_address(sender_address, FALSE, TRUE,
    global_rewrite_rules, rewrite_existflags);
  DEBUG(5) debug_printf("rewritten sender = %s\n", sender_address);
  }


/* The headers must always be run through rewrite_header, because it ensures
that addresses are fully qualified, as well as applying any rewriting rules
that may exist. Note: start at the second header; no point wasting time on
Received. This rewriting is documented as happening *after* recipient addresses
are taken from the headers by the -t command line option. An added Sender: gets
rewritten here. */

for (h = header_list->next; h != NULL; h = h->next)
  {
  header_line *newh = rewrite_header(h, NULL, NULL, global_rewrite_rules,
    rewrite_existflags, TRUE);
  if (newh != NULL) h = newh;
  }


/* WARNING: At this point, the saved header variables contain addresses that
are no longer valid, because we haven't bothered to update them on rewriting.
This is because the code below merely tests for their being NULL. */


/* A message is not legal unless it has at least one of "to", "cc", or "bcc".
Note that although the minimal examples in RFC822 show just "to" or "bcc", the
full syntax spec allows "cc" as well. If any resent- header exists, this
applies to the set of resent- headers rather than the normal set.

If we extracted recipients from bcc, then we simply add an empty bcc header,
which is legal according to RFC 822. Do the same for input via SMTP - thus not
disclosing possible bcc recipients. Otherwise (i.e. for a message whose
envelope came in on the command line) generate to: from the envelope address
fields, unless always_bcc is set. Don't bother if there are no recipients (an
error case that is picked up below); such a message isn't going anywhere. */

if (recipients_count > 0 && to_header == NULL && bcc_header == NULL &&
    cc_header == NULL)
  {
  if (extracted_bcc || smtp_input || always_bcc)
    header_add(htype_bcc, "%s:\n", header_names[hn_bcc].name);

  /* This is a bit tedious but is presumably rare. */

  else
    {
    int i;
    int p = 0;
    int count = 0;
    char *temp;

    for (i = 0; i < recipients_count; i++)
      count += (int)strlen(recipients_list[i].address) + 3;

    temp = store_get(count);
    count = (int)strlen(header_names[hn_to].name) + 2;
    for (i = 0; i < recipients_count; i++)
      {
      int j = (int)strlen(recipients_list[i].address);
      if (i != 0)
        {
        if (count + j > 72)
          {
          strcpy(temp + p, ",\n ");
          p += 3;
          count = 0;
          }
        else
          {
          strcpy(temp + p, ", ");
          p += 2;
          count += 2;
          }
        }
      strcpy(temp + p, recipients_list[i].address);
      p += j;
      count += j;
      }

    /* Add new header. */

    header_add(htype_to, "%s: %s\n", header_names[hn_to].name, temp);

    /**** We used to rewrite as well, but that is surely wrong, since
    the recipients have already been rewritten above. ****/
    }
  }

/* If there is no date header, generate one. */

if (date_header == NULL)
  header_add(htype_other, "%s: %s\n", header_names[hn_date].name,
    tod_stamp(tod_full));

/* Show the complete set of headers if debugging, and the next line
if present. */

DEBUG(9)
  {
  debug_printf(">>Final headers:\n");
  for (h = header_list; h != NULL; h = h->next)
    debug_printf("%c %s", h->type, h->text);
  debug_printf("\n");
  if (next != NULL) debug_printf("%s", next->text);
  }

search_tidyup();    /* Free any cached resources */

/* The headers are now complete in store. If we are running in filter
testing mode, that is all this function does. Return TRUE if the message
ended with a dot. */

if (filter_test != NULL)
  {
  process_info[process_info_len] = 0;
  return message_ended == END_DOT;
  }

/* If this is smtp_input, the sender is not local, and sender verification is
requested, call the main checking function and arrange to reject the message if
it fails. */

if (smtp_input && !sender_local && (sender_verify || sender_try_verify))
  {
  char *errmess;
  int errcode;

  if (!verify_sender(&errcode, &errmess))
    {
    accept_swallow_smtp(fin);
    smtp_reply = string_sprintf("%d rejected: %s", errcode, errmess);

    /* Send the configurable message, if any */

    smtp_send_prohibition_message(errcode, "sender_verify");

    /* Log the incident to the main and reject logs (the latter will log
    the headers), set no message current, and then skip to the end of this
    function. */

    log_write(3, LOG_MAIN|LOG_REJECT, "rejected%s: %s",
      host_and_ident(" from ", NULL), errmess);
    message_id[0] = 0;
    goto TIDYUP;
    }
  }

/* If configured to check the syntax of the headers, do so now. However,
there is no point in checking the syntax of To: Cc: or Bcc: when -t is
specified, as they will already have been checked by the address extracting
code. */

if (headers_check_syntax)
  {
  header_line *h;

  for (h = header_list; h != NULL; h = h->next)
    {
    if (h->type == htype_from ||
        h->type == htype_replyto ||
        h->type == htype_sender ||
        (!extract_recip &&
          (h->type == htype_to ||
           h->type == htype_cc ||
           h->type == htype_bcc)
        ))
      {
      char *s = strchr(h->text, ':') + 1;
      while (isspace((uschar)*s)) s++;

      parse_allow_group = TRUE;     /* Allow group syntax */

      /* Loop for multiple addresses in the header */

      while (*s != 0)
        {
        char *ss = parse_find_address_end(s, FALSE);
        char *recipient, *errmess;
        int terminator = *ss;
        int start, end, domain;

        /* Temporarily terminate the string at this point, and extract the
        operative address within. */

        *ss = 0;
        recipient = parse_extract_address(s,&errmess,&start,&end,&domain,FALSE);
        *ss = terminator;

        if (recipient == NULL && strcmp(errmess, "empty address") != 0)
          {
          char hname[64];
          char *t = h->text;
          char *tt = hname;
          char *verb = "is";
          int len;

          while (*t != ':' && (tt < (hname + sizeof(hname)-2)))
	    *tt++ = *t++;
          *tt = 0;

          /* Arrange not to include any white space at the end in the
          error message. */

          t = ss;
          while (t > s && isspace((uschar)t[-1])) t--;

          /* Add the address which failed to the error message, since in a
          header with very many addresses it is sometimes hard to spot
          which one is at fault. However, limit the amount of address to
          quote - cases have been seen where, for example, a missing double
          quote in a humungous To: header creates an "address" that is longer
          than string_sprintf can handle. */

          len = t - s;
          if (len > 1024)
            {
            len = 1024;
            verb = "begins";
            }

          errmess = string_printing(
            string_sprintf("%s: failing address %s: %.*s", errmess,
              verb, len, s));

          if (headers_checks_fail)
            {
            if (smtp_input)
              {
              accept_swallow_smtp(fin);
              smtp_reply = string_sprintf("550 Syntax error in '%s' header: "
                "%.256s", hname, errmess);

              log_write(3, LOG_MAIN|LOG_REJECT, "rejected from%s <%s>: "
                "syntax error in '%s' header: %s",
                  host_and_ident(" ", NULL), sender_address, hname, errmess);

              message_id[0] = 0;            /* Indicate no message accepted */
              goto TIDYUP;                  /* Skip to end of function */
              }
            else
              {
              fprintf(stderr, "exim: syntax error in '%s' header: %s\n",
                hname, errmess);
              fclose(data_file);
              exim_exit(EXIT_FAILURE);
              }
            }

          /* Just log warning message */

          else log_write(3, LOG_REJECT, "warning: from%s <%s>: "
                 "syntax error in '%s' header: %s",
                   host_and_ident(" ", NULL), sender_address, hname, errmess);
          }

        /* Advance to the next address */

        s = ss + (terminator? 1:0);
        while (isspace((uschar)*s)) s++;
        }
      }
    }
  }


/* If we are running in host checking mode, there is nothing more to do except
swallow the rest of the input if necessary. We then leap forward to the code
that writes a "received" log line and ends off, with msg_size set to the size
to be "logged". For real messages, this value is obtained accurately from the
written headers and body. Here we fake it with the count of bytes received plus
the Received: header. The difference is that any other added headers (Date,
From, etc.) don't get counted here. If the message wasn't terminated by a '.'
line, it must have been EOF, which must give the error message. */

if (host_checking)
  {
  accept_swallow_smtp(fin);
  msg_size = message_size + header_list->slen;
  if (message_ended == END_DOT) goto HOST_CHECKING_WRITE_LOG;
  goto HOST_CHECKING_EOF;
  }


/* Open a new spool file for the data portion of the message. We need
to access it both via a file descriptor and a stream. Try to make the
directory if it isn't there. Note re use of sprintf: spool_directory
is checked on input to be < 200 characters long. */

sprintf(spool_name, "%s/input/%s/%s-D", spool_directory, message_subdir,
  message_id);
data_fd = open(spool_name, O_RDWR|O_CREAT|O_EXCL, SPOOL_MODE);
if (data_fd < 0)
  {
  if (errno == ENOENT)
    {
    char temp[16];
    sprintf(temp, "input/%s", message_subdir);
    if (message_subdir[0] == 0) temp[5] = 0;
    (void)directory_make(spool_directory, temp, INPUT_DIRECTORY_MODE, TRUE);
    data_fd = open(spool_name, O_RDWR|O_CREAT|O_EXCL, SPOOL_MODE);
    }
  if (data_fd < 0)
    log_write(0, LOG_MAIN | LOG_PANIC_DIE, "Failed to create spool file %s: %s",
      spool_name, strerror(errno));
  }

/* Make sure the file's group is the Exim gid if exim_uid exists (can't have
exim_uid set without exim_gid), and double-check the mode because the group
setting doesn't always get set automatically. */

if (exim_uid_set)
  {
  fchown(data_fd, exim_uid, exim_gid);
  fchmod(data_fd, SPOOL_MODE);
  }

/* We now have data file open. Build a stream for it and lock it. */

data_file = fdopen(data_fd, "w+");
lock_data.l_type = F_WRLCK;
lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;
if (fcntl(data_fd, F_SETLK, &lock_data) < 0)
  log_write(0, LOG_MAIN | LOG_PANIC_DIE, "Cannot lock %s", spool_name);

/* We have an open, locked data file. Write the message id to it to make it
self-identifying. Then read the remainder of the input of this message and
write it to the data file. If the variable next != NULL, it contains the first
data line (which was read as a header but then turned out not to have the right
format); write it (remembering that it might contain binary zeros). The result
of fwrite() isn't inspected; instead we call ferror() below. */

fprintf(data_file, "%s-D\n", message_id);

if (next != NULL)
  {
  char *s = next->text;
  int len = next->slen;
  fwrite(s, 1, len, data_file);
  body_linecount++;                 /* Assumes only 1 line */
  }

/* Note that we might already be at end of file, or the logical end of file
(indicated by '.'), or might have encountered an error while writing the
message id or "next" line. */

if (!ferror(data_file) && !(accept_feof)(fin) && message_ended != END_DOT)
  {
  if (smtp_input)
    {
    message_ended = read_message_data_smtp(fin, data_file);
    accept_linecount++;                /* The terminating "." line */
    }
  else message_ended = read_message_data(fin, data_file);

  accept_linecount += body_linecount;  /* For BSMTP errors mainly */

  /* Handle premature termination of SMTP */

  if (smtp_input && message_ended == END_EOF)
    {
    unlink(spool_name);                /* Lose data file when closed */

    /* When checking a host with -bh, control jumps here if the incoming
    message was not terminated by EOF, so as to get the error message. */

    HOST_CHECKING_EOF:

    message_id[0] = 0;                 /* Indicate no message accepted */

    if (sender_host_unknown)
      {
      log_write(4, LOG_MAIN, "unexpected EOF while reading SMTP data "
        "from %s", sender_ident);
      smtp_reply = "554 Unexpected end of file";
      }
    else
      {
      log_write(4, LOG_MAIN, "unexpected disconnection while reading SMTP data "
        "from %s%s", sender_fullhost, smtp_read_error);
      smtp_reply = "421 Lost incoming connection";
      }

    smtp_yield = FALSE;
    goto TIDYUP;                       /* Skip to end of function */
    }

  /* Handle message that is too big. Don't use host_or_ident() in the log
  message; we want to see the ident value even for non-remote messages. */

  if (message_ended == END_SIZE)
    {
    unlink(spool_name);                /* Lose the data file when closed */
    if (smtp_input) accept_swallow_smtp(fin);  /* Swallow incoming SMTP */

    log_write(2, LOG_MAIN|LOG_REJECT, "rejected from <%s>%s%s%s%s: "
      "message too large: %d recipient%s size=%d max=%d",
      sender_address,
      (sender_fullhost == NULL)? "" : " H=",
      (sender_fullhost == NULL)? "" : sender_fullhost,
      (sender_ident == NULL)? "" : " U=",
      (sender_ident == NULL)? "" : sender_ident,
      recipients_count,
      (recipients_count == 1)? "" : "s",
      message_size,
      thismessage_size_limit);

    if (smtp_input)
      {
      smtp_reply = "552 Message too large";
      message_id[0] = 0;               /* Indicate no message accepted */
      goto TIDYUP;                     /* Skip to end of function */
      }
    else
      {
      fseek(data_file, (long int)DATA_START_OFFSET, SEEK_SET);
      if (error_handling == ERRORS_SENDER)
        {
        DEBUG(1) debug_printf("Data too large while reading input for "
          "message %s (%d read; limit is %d for %d recipient%s)\n", message_id,
            message_size, thismessage_size_limit, recipients_count,
            (recipients_count == 1)? "" : "s");
        if (!moan_to_sender(ERRMESS_TOOBIG, NULL, header_list, data_file,
            FALSE))
          error_rc = EXIT_FAILURE;
        }
      else fprintf(stderr, "exim: message too large\n");
      fclose(data_file);
      exim_exit(error_rc);
      }
    }
  }

/* The message body has now been read into the data file. Call fflush() to
empty the buffers in C, and then call fsync() to get the data written out onto
the disc, as fflush() doesn't do this (or at least, it isn't documented as
having to do this). If there was an I/O error on either input or output,
attempt to send an error message, and unlink the spool file. For non-SMTP input
we can then give up. Note that for SMTP input we must swallow the remainder of
the input in cases of output errors, since the far end doesn't expect to see
anything until the terminating dot line is sent. */

if (fflush(data_file) == EOF || ferror(data_file) ||
    fsync(fileno(data_file)) < 0 || (accept_ferror)(fin))
  {
  char *msg_errno = strerror(errno);
  BOOL input_error = (accept_ferror)(fin) != 0;
  char *msg = string_sprintf("%s error (%s) while receiving message from %s",
    input_error? "Input read" : "Spool write",
    msg_errno,
    (sender_fullhost != NULL)? sender_fullhost : sender_ident);

  log_write(0, LOG_MAIN, "Message abandoned: %s", msg);

  unlink(spool_name);                /* Lose the data file */

  /* For SMTP, set up response and swallow input for output errors. Then
  cancel the message and return from this runction via the tidying code. */

  if (smtp_input)
    {
    if (input_error)
      smtp_reply = "451 Error while reading input data";
    else
      {
      smtp_reply = "451 Error while writing spool file";
      accept_swallow_smtp(fin);
      }
    message_id[0] = 0;               /* Indicate no message accepted */
    goto TIDYUP;                     /* Skip to end of function */
    }

  /* For non-SMTP input either attempt to send a message, or write the
  error to stderr, and then give up. */

  else
    {
    fseek(data_file, (long int)DATA_START_OFFSET, SEEK_SET);
    if (error_handling == ERRORS_SENDER)
      {
      error_block eblock;
      eblock.next = NULL;
      eblock.text1 = msg;
      if (!moan_to_sender(ERRMESS_IOERR, &eblock, header_list, data_file,
        FALSE)) error_rc = EXIT_FAILURE;
      }
    else fprintf(stderr, "exim: %s\n", msg);
    fclose(data_file);
    exim_exit(error_rc);
    }
  }


/* No I/O errors were encountered while writing the data file. */

DEBUG(9) debug_printf("Data file written for message %s\n", message_id);


/* If there were any bad addresses in the headers (detected only if -t was
specified), or if there were no recipients (possibly as a result of bad
addresses), send a message to the sender of this message, or write it to
stderr if the error handling option is set that way. We need to rewind the
data file in order to read it. In the case of no recipients or stderr error
writing, throw the data file away afterwards, and exit. (This can't be SMTP,
which always ensures there's at least one syntactically good recipient
address.) */

if (bad_addresses != NULL || recipients_count == 0)
  {
  DEBUG(2)
    {
    if (recipients_count == 0) debug_printf("*** No recipients\n");
    if (bad_addresses != NULL)
      {
      error_block *eblock = bad_addresses;
      debug_printf("*** Bad address(es)\n");
      while (eblock != NULL)
        {
        debug_printf("  %s: %s\n", eblock->text1, eblock->text2);
        eblock = eblock->next;
        }
      }
    }

  fseek(data_file, (long int)DATA_START_OFFSET, SEEK_SET);

  /* If configured to send errors to the sender, but this fails, force
  a failure error code. We use a special one for no recipients so that it
  can be detected by the autoreply transport. Otherwise error_rc is set to
  errors_sender_rc, which is EXIT_FAILURE unless -oee was given, in which case
  it is EXIT_SUCCESS. */

  if (error_handling == ERRORS_SENDER)
    {
    if (!moan_to_sender(
          (bad_addresses == NULL)?
            (extracted_ignored? ERRMESS_IGADDRESS : ERRMESS_NOADDRESS) :
          (recipients_list == NULL)? ERRMESS_BADNOADDRESS : ERRMESS_BADADDRESS,
          bad_addresses, header_list, data_file, FALSE))
      error_rc = (bad_addresses == NULL)? EXIT_NORECIPIENTS : EXIT_FAILURE;
    }
  else
    {
    if (bad_addresses == NULL)
      {
      if (extracted_ignored)
        fprintf(stderr, "exim: all -t recipients overridden by command line\n");
      else
        fprintf(stderr, "exim: no recipients in message\n");
      }
    else
      {
      fprintf(stderr, "exim: invalid address%s",
        (bad_addresses->next == NULL)? ":" : "es:\n");
      while (bad_addresses != NULL)
        {
        fprintf(stderr, "  %s: %s\n", bad_addresses->text1,
          bad_addresses->text2);
        bad_addresses = bad_addresses->next;
        }
      }
    }

  if (recipients_count == 0 || error_handling == ERRORS_STDERR)
    {
    unlink(spool_name);
    fclose(data_file);
    exim_exit(error_rc);
    }
  }


/* Data file successfully written. Ignore signals while we are writing the
header file. */

signal(SIGALRM, SIG_IGN);
signal(SIGTERM, SIG_IGN);
signal(SIGINT, SIG_IGN);


/* Keep the data file open until we have written the header file, in order to
hold onto the lock. If writing the header file fails, we have failed to accept
this message. Ensure the first time flag is set in the newly-received message.
*/

deliver_firsttime = TRUE;

if ((msg_size = spool_write_header(message_id, SW_RECEIVING, &errmsg)) < 0)
  {
  log_write(0, LOG_MAIN, "Message abandoned: %s", errmsg);

  unlink(spool_name);           /* Lose the data file */

  if (smtp_input)
    {
    smtp_reply = "451 Error in writing spool file";
    message_id[0] = 0;          /* Indicate no message accepted */
    }
  else
    {
    fseek(data_file, (long int)DATA_START_OFFSET, SEEK_SET);
    if (error_handling == ERRORS_SENDER)
      {
      error_block eblock;
      eblock.next = NULL;
      eblock.text1 = errmsg;
      if (!moan_to_sender(ERRMESS_IOERR, &eblock, header_list, data_file,
        FALSE)) error_rc = EXIT_FAILURE;
      }
    else fprintf(stderr, "exim: %s\n", errmsg);
    fclose(data_file);
    exim_exit(error_rc);
    }
  }


/* Else log the arrival of a new message while the file is still locked, just
in case the machine is *really* fast, and delivers it first! Include any
message id that is in the message - since the syntax of a message id is
actually an addr-spec, we can use the parse routine to canonicize it. */

else
  {
  char *errmess, *old_id, *s;
  struct stat statbuf;
  int start, end, domain, size, ptr;

  /* Reset signal handlers to ignore signals that previously would have caused
  the message to be abandoned. */

  signal(SIGALRM, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, SIG_IGN);

  /* Add data size to written header size. This total differs from message_size
  in that it include the added Received: header and any other headers that got
  created locally. */

  fflush(data_file);
  fstat(data_fd, &statbuf);
  msg_size += statbuf.st_size - MESSAGE_ID_LENGTH - 3 + 1;

  /* When checking a host with -bh, control jumps to here after sucking in
  the body of the message, so that a mock-up log line gets output. The value
  of msg_size is set from the size of data received. */

  HOST_CHECKING_WRITE_LOG:

  /* Generate a "message received" log entry. We do this by building up a
  dynamic string as required. Since we commonly want to add two items at a
  time, use a macro to simplify the coding. */

  size = 256;
  ptr = 0;
  s = store_get(size);

  s = mac_cat2(s, &size, &ptr, "<= ",
    (sender_address[0] == 0)? "<>" : sender_address);
  if (message_reference != NULL)
    s = mac_cat2(s, &size, &ptr, " R=", message_reference);
  if (sender_fullhost != NULL)
    s = mac_cat2(s, &size, &ptr, " H=", sender_fullhost);
  if (sender_ident != NULL)
    s = mac_cat2(s, &size, &ptr, " U=", sender_ident);
  if (received_protocol != NULL)
    s = mac_cat2(s, &size, &ptr, " P=", received_protocol);

  #ifdef SUPPORT_TLS
  if (tls_log_cipher && tls_cipher != NULL)
    s = mac_cat2(s, &size, &ptr, " X=", tls_cipher);
  if (tls_log_peerdn && tls_peerdn != NULL)
    {
    s = mac_cat2(s, &size, &ptr, " DN=\"", tls_peerdn);
    s = string_cat(s, &size, &ptr, "\"", 1);
    }
  #endif

  if (sender_host_authenticated != NULL)
    {
    s = mac_cat2(s, &size, &ptr, " A=", sender_host_authenticated);
    if (authenticated_id != NULL)
      s = mac_cat2(s, &size, &ptr, ":", authenticated_id);
    }

  sprintf(big_buffer, "%d", msg_size);
  s = mac_cat2(s, &size, &ptr, " S=", big_buffer);

  /* If an addr-spec in a message-id contains a quoted string, it can contain
  any characters except " \ and CR and so in particular it can contain NL!
  Therefore, make sure we use a printing-characters only version for the log. */

  old_id = (resentmsgid_header != NULL)? resentmsgid_header->text :
    (msgid_header != NULL)? msgid_header->text : NULL;
  if (old_id != NULL)
    {
    old_id = parse_extract_address(strchr(old_id, ':') + 1,
      &errmess, &start, &end, &domain, FALSE);
    if (old_id != NULL)
      s = mac_cat2(s, &size, &ptr, " id=", string_printing(old_id));
    }

  /* If subject logging is turned on, create suitable printing-character
  text. */

  if (log_subject && subject_header != NULL)
    {
    int i;
    char *p = big_buffer;
    char *ss = subject_header->text + 7;

    while (*ss != ':') ss++;
    while (isspace((uschar)*(++ss)));

    /* Backslash-quote any double quotes or backslashes so as to make a
    a C-like string, and leave off the terminating newline. However, if
    the subject was empty, the isspace() above will have skipped over
    the newline, so we must first test for this case. Then turn any
    non-printers into escape sequences. */

    *p++ = '\"';
    if (*ss != 0) for (i = 0; i < 100 && ss[i+1] != 0; i++)
      {
      if (ss[i] == '\"' || ss[i] == '\\') *p++ = '\\';
      *p++ = ss[i];
      }
    *p++ = '\"';
    *p = 0;
    ss = string_printing(big_buffer);
    s = mac_cat2(s, &size, &ptr, " T=", ss);
    }

  /* Terminate the string: string_cat() leaves room, but does not put the
  zero in. */

  s[ptr] = 0;

  /* While writing to the log, set a flag to cause a call to accept_bomb_out()
  if the log cannot be opened. Do logging of messages accepted despite RBL
  matches here. */

  accept_call_bombout = TRUE;
  log_write(0, LOG_MAIN | (log_received_recipients? LOG_RECIPIENTS : 0) |
                          (log_received_sender? LOG_SENDER : 0), "%s", s);
  if (rbl_host && rbl_log_headers)
    log_write(1, LOG_REJECT, "Message accepted from RBL host");
  accept_call_bombout = FALSE;

  store_reset(s);   /* The store can be reused */
  }


/* Either a message has been successfully received and written to the two spool
files, or an error in writing the spool has occurred for an SMTP message, or
an SMTP message has been rejected because of a bad sender. (For a non-SMTP
message we will have already given up because there's no point in carrying on!)
In either event, we must now close (and thereby unlock) the data file. In the
successful case, this leaves the message on the spool, ready for delivery. In
the error case, the spool file will be deleted. Then tidy up store, interact
with an SMTP call if necessary, and return.

A fflush() was done earlier in the expectation that any write errors on the
data file will be flushed(!) out thereby. Nevertheless, it is theoretically
possible for fclose() to fail - but what to do? What has happened to the lock
if this happens? */

TIDYUP:
process_info[process_info_len] = 0;          /* Remove message id */

if (data_file != NULL) fclose(data_file);    /* Frees the lock */

/* Now reset signal handlers to their defaults */

signal(SIGALRM, SIG_DFL);
signal(SIGTERM, SIG_DFL);
signal(SIGINT, SIG_DFL);

/* Tell an SMTP caller the state of play, and arrange to return the SMTP return
value, which defaults TRUE - meaning there may be more incoming messages from
this connection. For non-SMTP callers (where there is only ever one message),
the default is FALSE. */

if (smtp_input)
  {
  yield = smtp_yield;

  /* Handle interactive SMTP callers */

  if (!smtp_batched_input)
    {
    if (smtp_reply == NULL)
      {
      smtp_printf("250 OK id=%s\r\n", message_id);
      if (host_checking)
        fprintf(stdout,
          "\n**** SMTP testing: that is not a real message id!\n\n");
      }
    else smtp_printf("%.1024s\r\n", smtp_reply);
    }

  /* For batched SMTP, generate an error message on failure, and do
  nothing on success. The function moan_smtp_batch() does not return -
  it exits from the program with a non-zero return code. */

  else if (smtp_reply != NULL) moan_smtp_batch(NULL, "%s", smtp_reply);
  }

/* Reset headers so that logging of rejects for a subsequent message doesn't
include them. It is also important to set header_last = NULL before exiting
from this function, as this prevents certain rewrites that might happen during
subsequent verifying (of another incoming message) from trying to add headers
when they shouldn't. */

header_list = header_last = NULL;

return yield;  /* TRUE if more messages (SMTP only) */
}

/* End of accept.c */
