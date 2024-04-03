/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for handling an incoming SMTP call. */


#include "exim.h"


/* Initialize for TCP wrappers if so configured */

#ifdef USE_TCP_WRAPPERS
#include <tcpd.h>
int allow_severity = LOG_INFO;
int deny_severity  = LOG_NOTICE;
#endif


/* Size of buffer for reading SMTP commands */

#define cmd_buffer_size  512      /* Ref. RFC 821 */

/* Size of buffer for reading SMTP incoming packets */

#define in_buffer_size  8192

/* Structure for SMTP command list */

typedef struct {
  char *name;
  int len;
  short int cmd;
  short int has_arg;
} smtp_cmd_list;

/* Codes for identifying commands */

enum { HELO_CMD, EHLO_CMD, MAIL_CMD, RCPT_CMD, DATA_CMD, VRFY_CMD,
  EXPN_CMD, QUIT_CMD, RSET_CMD, NOOP_CMD, DEBUG_CMD, HELP_CMD,
  ETRN_CMD, AUTH_CMD, STARTTLS_CMD, EOF_CMD, OTHER_CMD, BADARG_CMD };



/*************************************************
*                Local static variables          *
*************************************************/

static BOOL host_allow_relay_anywhere;
static BOOL host_allow_relay_anywhere_set;
#ifdef HAVE_AUTH
static BOOL host_must_authenticate;
static BOOL host_must_use_tls_to_authenticate;
static auth_instance *authenticated_by;
#endif
#ifdef SUPPORT_TLS
static BOOL host_must_use_tls;
#endif
static BOOL sender_allow_relay_anywhere;
static BOOL sender_allow_relay_anywhere_set;

static BOOL helo_required = FALSE;
static BOOL helo_seen;
static BOOL helo_accept_junk = FALSE;
static BOOL esmtp;

static BOOL host_refuse_all_rcpts;
static BOOL sender_refuse_all_rcpts;

static char *relay_msg1;
static char *relay_msg2;
static int   relay_errcode;
static int   unknown_command_count;

static char *smtp_data;

static char *cmd_buffer;

static smtp_cmd_list cmd_list[] = {
  { "helo",       sizeof("helo")-1,       HELO_CMD, TRUE },
  { "ehlo",       sizeof("ehlo")-1,       EHLO_CMD, TRUE },
  { "mail from:", sizeof("mail from:")-1, MAIL_CMD, TRUE },
  { "rcpt to:",   sizeof("rcpt to:")-1,   RCPT_CMD, TRUE },
  { "data",       sizeof("data")-1,       DATA_CMD, FALSE },
  { "vrfy",       sizeof("vrfy")-1,       VRFY_CMD, TRUE },
  { "expn",       sizeof("expn")-1,       EXPN_CMD, TRUE },
  { "quit",       sizeof("quit")-1,       QUIT_CMD, FALSE },
  { "rset",       sizeof("rset")-1,       RSET_CMD, FALSE },
  { "noop",       sizeof("noop")-1,       NOOP_CMD, TRUE },
  { "debug",      sizeof("debug")-1,      DEBUG_CMD, TRUE },
  { "help",       sizeof("help")-1,       HELP_CMD, TRUE },
  #ifdef HAVE_AUTH
  { "auth",       sizeof("auth")-1,       AUTH_CMD, TRUE },
  #endif
  #ifdef SUPPORT_TLS
  { "starttls",   sizeof("starttls")-1,   STARTTLS_CMD, FALSE },
  #endif
  { "etrn",       sizeof("etrn")-1,       ETRN_CMD, TRUE}
};

static smtp_cmd_list *cmd_list_end =
  cmd_list + sizeof(cmd_list)/sizeof(smtp_cmd_list);

static char *protocols[] = {
  "local-smtp", "local-esmtp", "local-asmtp" };

#define pnormal  0
#define pextend  1
#define pauthed  2
#define pnlocal  6

/* When reading SMTP from a remote host, we have to use our own versions of the
C input-reading functions, in order to be able to flush the SMTP output only
when about to read more data from the socket. This is the only way to get
optimal performance when the client is using pipelining. Flushing for every
command causes a separate packet and reply packet each time; saving all the
responses up (when pipelining) combines them into one packet and one response.

For simplicity, these functions are used for *all* SMTP input, not only when
receiving over a socket. However, after setting up a secure socket (SSL), input
is read via the OpenSSL library, and another set of functions is used instead
(see tls.c).

These functions are set in the accept_getc etc. variables and called with the
same interface as the C functions. However, since there can only ever be
one incoming SMTP call, we just use a single buffer and flags. There is no need
to implement a complicated private FILE-like structure.*/

static uschar *smtp_inbuffer;
static uschar *smtp_inptr;
static uschar *smtp_inend;
static int     smtp_had_eof;
static int     smtp_had_error;


/*************************************************
*          SMTP version of getc()                *
*************************************************/

/* This gets the next byte from the SMTP input buffer. If the buffer is empty,
it flushes the output, and refills the buffer, with a timeout. This is not used
after a connection has negotated itself into an TLS/SSL state.

Argument:   the FILE for the SMTP input
Returns:    the next character or EOF
*/

int
smtp_getc(FILE *f)
{
if (smtp_inptr >= smtp_inend)
  {
  int rc, save_errno;
  fflush(smtp_out);
  alarm(smtp_receive_timeout);
  rc = read(fileno(f), smtp_inbuffer, in_buffer_size);
  save_errno = errno;
  alarm(0);
  if (rc <= 0)
    {
    /* Must put the error text in fixed store, because this might be during
    header reading, where it releases unused store above the header. */
    if (rc < 0)
      {
      smtp_had_error = save_errno;
      smtp_read_error = string_copy_malloc(
        string_sprintf(" (error: %s)", strerror(save_errno)));
      }
    else smtp_had_eof = 1;
    return EOF;
    }
  smtp_inend = smtp_inbuffer + rc;
  smtp_inptr = smtp_inbuffer;
  }
return *smtp_inptr++;
}



/*************************************************
*          SMTP version of ungetc()              *
*************************************************/

/* Puts a character back in the input buffer. Only ever
called once.

Arguments:
  ch           the character
  f            the SMTP FILE pointer (not used)

Returns:       the character
*/

int
smtp_ungetc(int ch, FILE *f)
{
if (fileno(f) == 0) {}    /* Keep picky compilers happy */
*(--smtp_inptr) = ch;
return ch;
}




/*************************************************
*          SMTP version of feof()                *
*************************************************/

/* Tests for a previous EOF

Argument:      the FILE for the SMTP input
Returns:       non-zero if the eof flag is set
*/

int
smtp_feof(FILE *f)
{
if (fileno(f) == 0) {}    /* Keep picky compilers happy */
return smtp_had_eof;
}




/*************************************************
*          SMTP version of ferror()              *
*************************************************/

/* Tests for a previous read error, and returns with errno
restored to what it was when the error was detected.

Argument:      the FILE for the SMTP input
Returns:       non-zero if the error flag is set
*/

int
smtp_ferror(FILE *f)
{
if (fileno(f) == 0) {}   /* Keep picky compilers happy */
errno = smtp_had_error;
return smtp_had_error;
}




/*************************************************
*     Write formatted string to SMTP channel     *
*************************************************/

/* This is a separate function so that we don't have to repeat
everything for TLS support or debugging. It is global so that the
authentication functions can use it.

Arguments:
  format      format string
  ...         optional arguments

Returns:      nothing
*/

void
smtp_printf(char *format, ...)
{
va_list ap;
va_start(ap, format);
DEBUG(3)
  {
  debug_printf("SMTP>> ");
  debug_vprintf(format, ap);
  }

/* If in a TLS session we have to format the string, and then write it using a
TLS function. */

#ifdef SUPPORT_TLS
if (tls_active >= 0)
  {
  if (!string_vformat(big_buffer, big_buffer_size, format, ap))
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "string too large in smtp_printf");
    smtp_closedown("Unexpected error");
    exim_exit(EXIT_FAILURE);
    }
  (void)tls_write(big_buffer, strlen(big_buffer));
  }
else
#endif

/* Otherwise, just use the standard library function. */

vfprintf(smtp_out, format, ap);
va_end(ap);
}



/*************************************************
*          SMTP command read timeout             *
*************************************************/

/* Signal handler for timing out incoming SMTP commands

Argument: signal number (SIGALRM)
Returns:  nothing
*/

static void
command_timeout_handler(int sig)
{
sig = sig;    /* Keep picky compilers happy */
log_write(4, LOG_MAIN, "SMTP command timeout on%s connection from %s",
  (tls_active >= 0)? " TLS" : "",
  (sender_fullhost != NULL)? sender_fullhost : "local process");
if (smtp_batched_input)
  moan_smtp_batch(NULL, "421 SMTP command timeout");  /* Does not return */
smtp_printf("421 %s: SMTP command timeout - closing connection\r\n",
  primary_hostname);
mac_smtp_fflush();
exim_exit(EXIT_FAILURE);
}



/*************************************************
*               SIGTERM received                 *
*************************************************/

/* Signal handler for handling SIGTERM

Argument: signal number (SIGTERM)
Returns:  nothing
*/

static void
command_sigterm_handler(int sig)
{
sig = sig;    /* Keep picky compilers happy */
log_write(0, LOG_MAIN, "SMTP connection%s%s closed after SIGTERM",
  (sender_fullhost != NULL)? " from " : "",
  (sender_fullhost != NULL)? sender_fullhost : "");
if (smtp_batched_input)
  moan_smtp_batch(NULL, "421 SIGTERM received");  /* Does not return */
smtp_printf("421 %s: Service not available - closing connection\r\n",
  primary_hostname);
exim_exit(EXIT_FAILURE);
}



/*************************************************
*           Read one command line                *
*************************************************/

/* Strictly, SMTP commands coming over the net are supposed to end with CRLF.
There are sites that don't do this, and in any case internal SMTP probably
should check only for LF. Consequently, we check here for LF only. The line
ends up with [CR]LF removed from its end. If we get an overlong line, treat as
an unknown command. The command is read into the static cmd_buffer.

The character reading routine sets up a timeout for each block actually read
from the input (which may contain more than one command). We reset the signal
handlers each time in case they got changed as a result of some processing for
the previous command. If the timeout happens, or we get SIGTERM, Exim just
gives up and dies - control doesn't get back here.

Arguments:  none
Returns:    a code identifying the command (enumerated above)
*/

static int
smtp_read_command(void)
{
int c;
int ptr = 0;
smtp_cmd_list *p;

os_non_restarting_signal(SIGALRM, command_timeout_handler);
signal(SIGTERM, command_sigterm_handler);

while ((c = (accept_getc)(smtp_in)) != '\n' && c != EOF)
  {
  if (ptr >= cmd_buffer_size) return OTHER_CMD;
  cmd_buffer[ptr++] = c;
  }
accept_linecount++;    /* For BSMTP errors */

/* If hit end of file, return pseudo EOF command. Whether we have a
part-line already read doesn't matter, since this is an error state. */

if (c == EOF) return EOF_CMD;

/* Remove any CR and white space at the end of the line, and terminate the
string. */

while (ptr > 0 && isspace((uschar)cmd_buffer[ptr-1])) ptr--;
cmd_buffer[ptr] = 0;

DEBUG(3) debug_printf("SMTP<< %s\n", cmd_buffer);

/* Scan command list and return identity, having set the data pointer
to the start of the actual data characters. */

for (p = cmd_list; p < cmd_list_end; p++)
  {
  if (strncmpic(cmd_buffer, p->name, p->len) == 0)
    {
    smtp_data = cmd_buffer + p->len;
    while (isspace((uschar)*smtp_data)) smtp_data++;
    return (p->has_arg || *smtp_data == 0)? p->cmd : BADARG_CMD;
    }
  }

return OTHER_CMD;
}



/*************************************************
*          Forced closedown of call              *
*************************************************/

/* This function is called from log.c when Exim is dying because of a serious
disaster, and also from some other places. If an incoming non-batched SMTP
channel is open, it swallows the rest of the incoming message if in the DATA
phase, sends the reply string, and gives an error to all subsequent commands
except QUIT. The existence of an SMTP call is detected by the non-NULLness of
smtp_in.

Argument:   SMTP reply string to send, excluding the code
Returns:    nothing
*/

void
smtp_closedown(char *message)
{
if (smtp_in == NULL || smtp_batched_input) return;
accept_swallow_smtp(smtp_in);
smtp_printf("421 %s\r\n", message);

for (;;)
  {
  switch(smtp_read_command())
    {
    case EOF_CMD:
    case QUIT_CMD:
    return;

    case RSET_CMD:
    smtp_printf("250 Reset OK\r\n");
    break;

    default:
    smtp_printf("421 %s\r\n", message);
    break;
    }
  }
}




/*************************************************
*   Check HELO line and set sender_helo_name     *
*************************************************/

/* Check the format of a HELO line. The data for HELO/EHLO is supposed to be
the domain name of the sending host, or an ip literal, optionally followed by
other information. Check the text up to the first white space; then check that
any subsequent text consists of printing characters only. The first item is
placed in sender_helo_name, which is in malloc store, because it must persist
over multiple incoming messages.

Argument:
  s       the data portion of the line (already past any white space)
Returns:  TRUE or FALSE
*/

static BOOL
check_helo(char *s)
{
char *start = s;
char *end = NULL;
BOOL yield = FALSE;

/* Discard any previous helo name */

if (sender_helo_name != NULL)
  {
  store_free(sender_helo_name);
  sender_helo_name = NULL;
  }

/* Check IP literal, unless host is allowed to send junk */

if (*s == '[' && !helo_accept_junk)
  {
  while (*s != 0 && !isspace((uschar)*s)) s++;
  if (s[-1] == ']')
    {
    s[-1] = 0;
    yield = string_is_ip_address(start+1, NULL);
    s[-1] = ']';
    end = s;
    }
  }

/* Check domain name. Really, this should be restricted to alphanumeric
characters plus dot and hyphen. Sadly, there seems to be common software out
there that uses underscores, so we permit those to save hassle in the default
case, but an option can make things tighter. I bet nobody ever sets it, but it
was asked for. It is also possible to permit certain hosts and networks to
send any old junk after HELO/EHLO, since some sites need this to cope with
their local broken hosts. */

else if (*s != 0)
  {
  yield = TRUE;
  while (*s != 0 && !isspace((uschar)*s))
    {
    if (!helo_accept_junk &&
        !isalnum((uschar)*s) &&
        *s != '.' && *
        s != '-' &&
        (helo_strict_syntax || *s != '_'))
      {
      yield = FALSE;
      break;
      }
    s++;
    }
  end = s;
  }
else if (helo_accept_junk) yield = TRUE;

/* If OK, check remaining characters, and if still OK, set up the variable in
permanent store. */

if (yield)
  {
  while (*s != 0)
    {
    int c = (uschar)(*s++);
    if (!mac_isprint(c)) { yield = FALSE; break; }
    }
  if (yield)
    {
    sender_helo_name = store_malloc(end - start + 1);
    sprintf(sender_helo_name, "%.*s", end-start, start);
    }
  }

return yield;
}





/*************************************************
*         Extract SMTP command option            *
*************************************************/

/* This function picks the next option setting off the end of smtp_data. It
is called for MAIL FROM and RCPT TO commands, to pick off the optional ESMTP
things that can appear there.

Arguments:
   name           point this at the name
   value          point this at the data string

Returns:          TRUE if found an option
*/

static BOOL
extract_option(char **name, char **value)
{
char *n;
char *v = smtp_data + (int)strlen(smtp_data) -1;
while (isspace((uschar)*v)) v--;
v[1] = 0;

while (v > smtp_data && *v != '=' && !isspace((uschar)*v)) v--;
if (*v != '=') return FALSE;

n = v;
while(isalpha((uschar)n[-1])) n--;

if (n[-1] != ' ') return FALSE;

n[-1] = 0;
*name = n;
*v++ = 0;
*value = v;
return TRUE;
}







/*************************************************
*         Reset for new message                  *
*************************************************/

/* This function is called whenever the SMTP session is reset from
within either of the setup functions.

Argument:   the stacking pool storage reset point
Returns:    nothing
*/

static void
smtp_reset(void *reset_point)
{
store_reset(reset_point);
recipients_list = NULL;
recipients_count = recipients_list_max = 0;
sender_address = NULL;
sender_address_unrewritten = NULL;
raw_sender = NULL;
sender_verify_callback_error = NULL;

#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is
left here just in case. Nothing is documented. */
dsn_ret = 0;
dsn_envid = NULL;
#endif

#ifdef HAVE_AUTH
authenticated_sender = NULL;
#endif
}





/*************************************************
*  Initialize for incoming batched SMTP message  *
*************************************************/

/* This function is called from smtp_setup_msg() in the case when
smtp_batched_input is true. This happens when -bS is used to pass a whole batch
of messages in one file with SMTP commands between them. All errors must be
reported by sending a message, and only MAIL FROM, RCPT TO, and DATA are
relevant. After an error on a sender, or an invalid recipient, the remainder
of the message is skipped. The value of received_protocol is already set.

Argument: none
Returns:  > 0 message successfully started (reached DATA)
          = 0 QUIT read or end of file reached
          < 0 should not occur
*/

static int
smtp_setup_batch_msg(void)
{
int done = 0;
void *reset_point = store_get(0);

/* Save the line count at the start of each transaction - single commands
like HELO and RSET count as whole transactions. */

bsmtp_transaction_linecount = accept_linecount;

if ((accept_feof)(smtp_in)) return 0;   /* Treat EOF as QUIT */

smtp_reset(reset_point);                /* Reset for start of message */

/* Deal with SMTP commands. This loop is exited by setting done to a POSITIVE
value. The values are 2 larger than the required yield of the function. */

while (done <= 0)
  {
  char *errmess;
  char *receiver = NULL;
  int errcode, start, end, sender_domain, receiver_domain;

  switch(smtp_read_command())
    {
    /* The HELO/EHLO commands set sender_address_helo if they have
    valid data; otherwise they are ignored, except that they do
    a reset of the state. */

    case HELO_CMD:
    case EHLO_CMD:

    check_helo(smtp_data);
    /* Fall through */

    case RSET_CMD:
    smtp_reset(reset_point);
    bsmtp_transaction_linecount = accept_linecount;
    break;


    /* The MAIL FROM command requires an address as an operand. All we
    do here is to parse it for syntactic correctness. The form "<>" is
    a special case which converts into an empty string. The start/end
    pointers in the original are not used further for this address, as
    it is the canonical extracted address which is all that is kept. */

    case MAIL_CMD:
    if (sender_address != NULL)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "503 Sender already given");

    if (smtp_data[0] == 0)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "501 MAIL FROM must have an address operand");

    /* Reset to start of message */

    smtp_reset(reset_point);

    /* Apply SMTP rewrite, then extract the address. The TRUE flag allows "<>"
    as a sender address */

    raw_sender = ((rewrite_existflags & rewrite_smtp) != 0)?
      rewrite_one(smtp_data, rewrite_smtp, NULL, FALSE, "",
        global_rewrite_rules) : smtp_data;

    rfc821_domains = TRUE;
    raw_sender =
      parse_extract_address(raw_sender, &errmess, &start, &end, &sender_domain,
        TRUE);
    rfc821_domains = FALSE;

    if (raw_sender == NULL)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "501 %s", errmess);

    sender_address = string_copy(raw_sender);

    /* Qualify unqualified sender addresses. There doesn't seem much point
    in causing trouble here. */

    if (sender_domain == 0 && sender_address[0] != 0 && sender_address[0] != '@')
      {
      sender_address = rewrite_address_qualify(sender_address, FALSE);
      DEBUG(9) debug_printf("unqualified address %s accepted\n",
        raw_sender);
      }

    /* If configured to check sender addresses, do the preliminary check
    now, unless the sender is local (in which case whatever is given here
    is ignored anyway). The check will fail if the message is to be refused at
    this stage. Another check function is called after the message has been
    received, to do more checking when the headers are available. Sender
    verification is most often turned off for BSMTP, as it doesn't make too
    much sense really. */

    errmess = NULL;
    refuse_all_rcpts = FALSE;

    if (sender_verify || sender_try_verify)
      {
      if (!sender_local && !verify_sender_preliminary(&errcode, &errmess))
        {
        log_write(1, LOG_MAIN|LOG_REJECT, "rejected MAIL FROM in SMTP batch: "
          "%s <%s>", errmess, raw_sender);
        /* The function moan_smtp_batch() does not return. */
        moan_smtp_batch(cmd_buffer, "%d rejected MAIL FROM: %s <%s>\n",
          errcode, errmess, raw_sender);
        }
      }
    break;


    /* The RCPT TO command requires an address as an operand. All we do
    here is to parse it for syntactic correctness. There may be any number
    of RCPT TO commands, specifying multiple senders. We build them all into
    a data structure that is in argc/argv format. The start/end values
    given by parse_extract_address are not used, as we keep only the
    extracted address. */

    case RCPT_CMD:
    if (sender_address == NULL)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "503 No sender yet given");

    if (smtp_data[0] == 0)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "501 RCPT TO must have an address operand");

    if (refuse_all_rcpts)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "550 cannot route to sender address <%s>",
        sender_address);

    /* Check maximum number allowed */

    if (recipients_max > 0 && recipients_count + 1 > recipients_max)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "%s too many recipients",
        recipients_max_reject? "552": "452");

    /* Apply SMTP rewrite, then extract address. Don't allow "<>" as a
    recipient address */

    receiver = ((rewrite_existflags & rewrite_smtp) != 0)?
      rewrite_one(smtp_data, rewrite_smtp, NULL, FALSE, "",
        global_rewrite_rules) : smtp_data;

    rfc821_domains = TRUE;
    receiver = parse_extract_address(receiver, &errmess, &start, &end,
      &receiver_domain, FALSE);
    rfc821_domains = FALSE;

    if (receiver == NULL)
      /* The function moan_smtp_batch() does not return. */
      moan_smtp_batch(cmd_buffer, "501 %s", errmess);

    /* If the receiver address is unqualified, qualify it. There doesn't seem
    much point in enforcing any checking here. Then add it to the list of
    recipients. */

    if (receiver_domain == 0 && receiver[0] != '@')
      {
      DEBUG(9) debug_printf("unqualified address %s accepted\n",
        receiver);
      receiver = rewrite_address_qualify(receiver, TRUE);
      }
    accept_add_recipient(receiver, NULL, 0, 0);
    break;


    /* The DATA command is legal only if it follows successful MAIL FROM
    and RCPT TO commands. This function is complete when a valid DATA
    command is encountered. */

    case DATA_CMD:
    if (sender_address == NULL || recipients_count <= 0)
      {
      /* The function moan_smtp_batch() does not return. */
      if (sender_address == NULL)
        moan_smtp_batch(cmd_buffer,
          "503 MAIL FROM:<sender> command must precede DATA");
      else
        moan_smtp_batch(cmd_buffer,
          "503 RCPT TO:<recipient> must precede DATA");
      }
    else
      {
      done = 3;                      /* DATA successfully achieved */
      message_ended = END_NOTENDED;  /* Indicate in middle of message */
      }
    break;


    /* The VRFY, EXPN, HELP, ETRN, and DEBUG commands are ignored. */

    case VRFY_CMD:
    case EXPN_CMD:
    case HELP_CMD:
    case DEBUG_CMD:
    case NOOP_CMD:
    case ETRN_CMD:
    bsmtp_transaction_linecount = accept_linecount;
    break;


    case EOF_CMD:
    case QUIT_CMD:
    done = 2;
    break;


    case BADARG_CMD:
    /* The function moan_smtp_batch() does not return. */
    moan_smtp_batch(cmd_buffer, "501 Unexpected argument data");
    break;


    default:
    /* The function moan_smtp_batch() does not return. */
    moan_smtp_batch(cmd_buffer, "500 Command unrecognized");
    break;
    }
  }

/* Reset the signal handlers used in this function. */

signal(SIGALRM, SIG_DFL);
signal(SIGTERM, SIG_DFL);

return done - 2;  /* Convert yield values */
}




/*************************************************
*          Build host+ident message              *
*************************************************/

/* Used when logging rejections below and in other modules. The text return
depends on whether sender_fullhost and sender_ident are set or not:

  no ident, no host   => if ustring set, "<ustring>unknown" else ""
  no ident, host set  => "<hstring>sender_fullhost"
  ident set, no host  => if ustring set, "<ustring>ident" else ""
  ident set, host set => if ustring set,
                           "<hstring>sender_fullhost<ustring>ident"
                         else
                           "<hstring>sender_fullhost (ident)"
Arguments:
  hstring  string to put in front of a host name
  ustring  string to put in front of an ident name

Returns:   pointer to sender_fullhost, or a string in big_buffer
*/

char *
host_and_ident(char *hstring, char *ustring)
{
if (sender_fullhost == NULL)
  {
  if (ustring != NULL)
    (void)string_format(big_buffer, big_buffer_size, "%s%s", ustring,
       (sender_ident == NULL)? "unknown" : sender_ident);
  else
    big_buffer[0] = 0;
  }
else
  {
  if (sender_ident == NULL)
    (void)string_format(big_buffer, big_buffer_size, "%s%s",
      hstring, sender_fullhost);
  else if (ustring == NULL)
    (void) string_format(big_buffer, big_buffer_size, "%s%s (%s)",
      hstring, sender_fullhost, sender_ident);
  else
    (void) string_format(big_buffer, big_buffer_size, "%s%s%s%s",
      hstring, sender_fullhost, ustring, sender_ident);
  }
return big_buffer;
}





/*************************************************
*        Output tailored rejection message       *
*************************************************/

/* This function is called when an error caused by an administrative
prohibition is about to be sent down an SMTP channel. It gives the
administrator a change to send additional information. If prohibition_message
is set, it is expanded and written as a series of continued response lines,
assuming the the standard response will be output afterwards.

Arguments:
  errorcode  the SMTP error code
  reason     a value to put in $prohibition_reason for the expansion

Returns:     nothing
*/

void
smtp_send_prohibition_message(int errorcode, char *reason)
{
char *msg, *s;

if (prohibition_message == NULL) return;

prohibition_reason = reason;
msg = expand_string(prohibition_message);
prohibition_reason = NULL;

if (msg == NULL)
  {
  log_write(0, LOG_MAIN|LOG_PANIC,
    "Expansion of \"%s\" (prohibition_message) failed: %s",
    prohibition_message, expand_string_message);
  return;
  }

s = msg;
while (isspace((uschar)*s)) s++;
if (*s == 0) return;            /* Empty message ignored */

while (*s != 0)
  {
  char *ss = s + 1;
  while (*ss != 0 && *ss != '\n' && *ss != '|') ss++;
  smtp_printf("%d-%.*s\r\n", errorcode, ss-s, s);
  if (*ss == 0) break;
  s = ss + 1;
  while (isspace((uschar)*s)) s++;
  }
}




/*************************************************
*       Check host for permission to relay       *
*************************************************/

/* This function determines whether a host is permitted to relay or not. Once
run, it sets host_allow_relay_anywhere_set = TRUE, to avoid having to do the
work more than once, as it could involve expensive lookups. (So it is called
only when first needed rather than always.) However, when a host authenticates,
the flag is reset if relaying is not permitted, because authentication might
change the relay status. Also, when a TLS session is started, the flag is
always reset.

Arguments:   none     (all data in statics)
Returns:     nothing  (answer put in statics)
*/

static void
check_host_for_relay(void)
{
if (host_allow_relay_anywhere_set)
  {
  HDEBUG(9) debug_printf("host previously checked for relaying: %s"
    "permitted\n", host_allow_relay_anywhere? "" : "not ");
  return;
  }
host_allow_relay_anywhere_set = TRUE;

/* Default failure messages. Even if this host is permitted, a subsequent
check on the sender may fail. */

relay_msg1 = "%d relaying to <%s> prohibited by administrator\r\n";
relay_msg2 = "";
relay_errcode = 550;

host_allow_relay_anywhere =
  verify_check_host(&host_accept_relay, FALSE);

#ifdef HAVE_AUTH
if (!host_allow_relay_anywhere && sender_host_authenticated != NULL)
  host_allow_relay_anywhere =
    verify_check_host(&host_auth_accept_relay, FALSE);
#endif

#ifdef SUPPORT_TLS
if (!host_allow_relay_anywhere && tls_active >= 0)
  host_allow_relay_anywhere =
    verify_check_host(&tls_host_accept_relay, FALSE);
#endif

if (host_allow_relay_anywhere)
  {
  HDEBUG(9) debug_printf("host is permitted to relay\n");
  }
else
  {
  if (host_lookup_msg[0] != 0)
    relay_msg1 =
      string_sprintf("%.*s%s\r\n", (int)strlen(relay_msg1) - 2,
        relay_msg1, host_lookup_msg);
  if (search_find_defer)
    {
    relay_msg2 = "lookup deferred: temporarily ";
    relay_errcode = 451;
    }
  }
}




/*************************************************
*          Start an SMTP session                 *
*************************************************/

/* This function is called at the start of an SMTP session. Thereafter,
smtp_setup_msg() is called to initiate each separate message. This
function does host-specific testing, and outputs the banner line.

Arguments:     none
Returns:       FALSE if the session can not continue; something has
               gone wrong, or the connection to the host is blocked
*/

BOOL
smtp_start_session(void)
{
int size = 256;
int ptr;
char *p, *s, *ss;

helo_seen = esmtp = helo_accept_junk = FALSE;
host_allow_relay_anywhere_set = FALSE;
host_refuse_all_rcpts = FALSE;
rbl_msg_buffer = NULL;
unknown_command_count = 0;

#ifdef HAVE_AUTH
host_must_authenticate = FALSE;
host_must_use_tls_to_authenticate = FALSE;
sender_host_authenticated = NULL;
authenticated_by = NULL;
#endif

#ifdef SUPPORT_TLS
host_must_use_tls = FALSE;
tls_cipher = tls_peerdn = NULL;
#endif

cmd_buffer = (char *)malloc(cmd_buffer_size + 1);  /* allow for trailing 0 */
if (cmd_buffer == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "malloc() failed for SMTP command buffer");

/* For batched input, the protocol setting can be overridden from the
command line by a trusted caller. */

if (smtp_batched_input)
  {
  if (received_protocol == NULL) received_protocol = "local-bsmtp";
  }

/* For non-batched SMTP input, the protocol setting is forced here. It may be
reset later if EHLO is received. */

else
  received_protocol =
    protocols[pnormal] + ((sender_host_address != NULL)? pnlocal : 0);

/* Set up the buffer for inputting using direct read() calls, and arrange to
call the local functions instead of the standard C ones. */

smtp_inbuffer = (uschar *)malloc(in_buffer_size);
if (smtp_inbuffer == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "malloc() failed for SMTP input buffer");
accept_getc = smtp_getc;
accept_ungetc = smtp_ungetc;
accept_feof = smtp_feof;
accept_ferror = smtp_ferror;
smtp_inptr = smtp_inend = smtp_inbuffer;
smtp_had_eof = smtp_had_error = 0;

/* When a message is input locally via the -bs or -bS options, sender_host_
unknown is set unless -oMa was used to force an IP address, in which case it
is checked like a real remote connection. When -bs is used from inetd, this
flag is not set, causing the sending host to be checked. The code that deals
with IP source routing (if configured) is never required for -bs or -bS and
the flag sender_host_notsocket is used to suppress it.

After the IP options test, check for forbidden hosts and reject the call if it
is one of them.

(1) Accept a connection from any host that is not in host_reject. However, if
the connection is from a host listed in host_reject_recipients, set the flag to
reject all recipients, that is, to fail all messages (as opposed to rejecting
connections). There can be ident values associated with any host.

(2) If the host is listed in auth_hosts, set a flag indicating that
authentication is needed before the host can do anything. Similarly, a flag is
set for hosts listed in tls_hosts.

(3) If smtp_accept_max and smtp_accept_reserve are set, keep some connections
in reserve for certain hosts and/or networks.

(4) If host_accept_relay or sender_address_relay is set, further checks are
done on individual recipient addresses, at RCPT TO time, in an order which is
supposed to minimize the work done, which is why nothing is done here - the
host does not need to be checked unless the domain is a restricted one. */

if (!sender_host_unknown)
  {
  BOOL reserved_host = FALSE;
  BOOL reject_recipients = FALSE;

  /* Look up IP options (source routing info) on the outgoing socket if
  configured to do so and this is not an -oMa "host", and flatten the options
  if requested, or refuse the call.

  Linux (and others now, see below) is different to everyone else, so there
  has to be some conditional compilation here. Versions of Linux before 2.1.15
  used a structure whose name was "options". Somebody finally realized that
  this name was silly, and it got changed to "ip_options". I use the
  newer name here, but there is a fudge in the script that sets up os.h
  to define a macro in older Linux systems.

  Sigh. Linux is a fast-moving target. Another generation of Linux uses
  glibc 2, which has chosen ip_opts for the structure name. This is now
  really a glibc thing rather than a Linux thing, so the condition name
  has been changed to reflect this. It is relevant also to GNU/Hurd.

  Some DGUX versions on older hardware appear not to support IP options at
  all, so there is now a general macro which can be set to cut out this
  support altogether.

  How to do this properly in IPv6 is not yet known. */

  #if !HAVE_IPV6 && !defined(NO_IP_OPTIONS)

  if (!host_checking && !sender_host_notsocket &&
       (log_ip_options || kill_ip_options || refuse_ip_options))
    {
    #ifdef GLIBC_IP_OPTIONS

      #if (!defined __GLIBC__) || (__GLIBC__ < 2)
      int optlen = sizeof(struct ip_options) + MAX_IPOPTLEN;
      struct ip_options *ipopt = store_get(optlen);
      #else
      struct ip_opts ipoptblock;
      struct ip_opts *ipopt = &ipoptblock;
      int optlen = sizeof(ipoptblock);
      #endif

    /* Non-glibc systems */

    #else
      struct ipoption ipoptblock;
      struct ipoption *ipopt = &ipoptblock;
      int optlen = sizeof(ipoptblock);
    #endif

    /* Occasional genuine failures of getsockopt() have been seen - for
    example, "reset by peer". Therefore, just log and give up on this
    call, unless the error is ENOPROTOOPT. This error is given by systems
    that have the interfaces but not the mechanism - e.g. GNU/Hurd at the time
    of writing. So for that error, carry on - we just can't do an IP options
    check. */

    DEBUG(9) debug_printf("checking for IP options\n");

    if (getsockopt(fileno(smtp_out), IPPROTO_IP, IP_OPTIONS, (char *)(ipopt),
          &optlen) < 0)
      {
      if (errno != ENOPROTOOPT)
        {
        log_write(0, LOG_MAIN, "getsockopt() failed from %s: %s",
          host_and_ident("", NULL), strerror(errno));
        smtp_printf("451 SMTP service not available\r\n");
        return FALSE;
        }
      }

    /* Deal with any IP options that are set. On the systems I have looked at,
    the value of MAX_IPOPTLEN has been 40, meaning that there should never be
    more logging data than will fit in big_buffer. Nevertheless, after somebody
    questioned this code, I've added in some paranoid checking. */

    else if (optlen > 0)
      {
      DEBUG(9) debug_printf("IP options exist\n");
      if (log_ip_options)
        {
        char *p = big_buffer;
        char *pend = big_buffer + big_buffer_size;
        uschar *opt, *adptr;
        int optcount;
        struct in_addr addr;

        #ifdef GLIBC_IP_OPTIONS
          #if (!defined __GLIBC__) || (__GLIBC__ < 2)
          uschar *optstart = (uschar *)(ipopt->__data);
          #else
	  uschar *optstart = (uschar *)(ipopt->ip_opts);
	  #endif
        #else
        uschar *optstart = (uschar *)(ipopt->ipopt_list);
        #endif

        strcpy(p, "IP options on incoming call:");
        p += (int)strlen(p);

        for (opt = optstart; opt != NULL &&
             opt < (uschar *)(ipopt) + optlen;)
	  {
	  switch (*opt)
	    {
	    case IPOPT_EOL:
	    opt = NULL;
	    break;

	    case IPOPT_NOP:
	    opt++;
	    break;

	    case IPOPT_SSRR:
	    case IPOPT_LSRR:
            if (!string_format(p, pend-p, " %s [@%s",
                 (*opt == IPOPT_SSRR)? "SSRR" : "LSRR",
            #ifdef GLIBC_IP_OPTIONS
              #if (!defined __GLIBC__) || (__GLIBC__ < 2)
              inet_ntoa(*((struct in_addr *)(&(ipopt->faddr))))))
              #else
	      inet_ntoa(ipopt->ip_dst)))
	      #endif
            #else
              inet_ntoa(ipopt->ipopt_dst)))
            #endif
              {
              opt = NULL;
              break;
              }

            p += (int)strlen(p);
            optcount = (opt[1] - 3) / sizeof(struct in_addr);
            adptr = opt + 3;
            while (optcount-- > 0)
              {
              memcpy(&addr, adptr, sizeof(addr));
              if (!string_format(p, pend - p - 1, "%s%s",
                    (optcount == 0)? ":" : "@", inet_ntoa(addr)))
                {
                opt = NULL;
                break;
                }
              p += (int)strlen(p);
              adptr += sizeof(struct in_addr);
              }
            *p++ = ']';
	    opt += opt[1];
	    break;

	    default:
              {
              int i;
              if (pend - p < 4 + 3*opt[1]) { opt = NULL; break; }
              strcat(p, "[ ");
              p += 2;
              for (i = 0; i < opt[1]; i++)
                {
                sprintf(p, "%2.2x ", opt[i]);
                p += 3;
                }
              *p++ = ']';
              }
	    opt += opt[1];
	    break;
	    }
	  }

        *p = 0;
        log_write(0, LOG_MAIN, "%s", big_buffer);
        }

      /* Refuse any call with IP options if configured to do so. This is
      what tcpwrappers 7.5 does. */

      if (refuse_ip_options)
        {
        log_write(0, LOG_MAIN|LOG_REJECT,
          "connection from %s refused (IP options)", host_and_ident("", NULL));
        smtp_printf("554 SMTP service not available\r\n");
        return FALSE;
        }

      /* Kill any IP options if configured to do so. This is a defence
      against source routing, as practiced by earlier versions of
      tcpwrappers. */

      if (kill_ip_options)
        {
        if (setsockopt(fileno(smtp_out), IPPROTO_IP, IP_OPTIONS, (char *)0, 0) != 0)
          log_write(0, LOG_MAIN|LOG_PANIC_DIE, "setsockopt() failed: %s",
            strerror(errno));
        log_write(0, LOG_MAIN, "IP options removed");
        }
      }

    /* Length of options = 0 => there are no options */

    else
      {
      DEBUG(9) debug_printf("no IP options found\n");
      }
    }
  #endif  /* HAVE_IPV6 && !defined(NO_IP_OPTIONS) */

  /* Set keep-alive in socket options. The option is on by default. This
  setting is an attempt to get rid of some hanging connections that stick in
  read() when the remote end (usually a dialup) goes away. */

  if (smtp_accept_keepalive && !sender_host_notsocket)
    {
    int fodder = 1;
    if (setsockopt(fileno(smtp_out), SOL_SOCKET, SO_KEEPALIVE,
        (char *)(&fodder), sizeof(fodder)) != 0)
      log_write(4, LOG_MAIN, "setsockopt(SO_KEEPALIVE) on connection from %s "
	"failed: %s", sender_host_address, strerror(errno));
    }

  /* If the current host matches host_lookup, set the name by doing a
  reverse lookup. On failure, sender_host_name will be NULL and
  host_lookup_failed will be TRUE. This may or may not be serious - optional
  checks later. */

  if (verify_check_host(&host_lookup, FALSE))
    {
    (void)host_name_lookup();
    host_build_sender_fullhost();
    set_process_info("handling incoming connection from %s", sender_fullhost);
    }

  /* Test with TCP Wrappers if so configured */

  #ifdef USE_TCP_WRAPPERS
  if (!hosts_ctl("exim",
	 (sender_host_name == NULL)? STRING_UNKNOWN : sender_host_name,
	 (sender_host_address == NULL)? STRING_UNKNOWN : sender_host_address,
	 (sender_ident == NULL)? STRING_UNKNOWN : sender_ident))
    {
    HDEBUG(9) debug_printf("tcp wrappers rejection\n");
    log_write(1, LOG_MAIN|LOG_REJECT, "connection from %s refused "
      "(tcp wrappers)", host_and_ident("", NULL));
    smtp_send_prohibition_message(554, "host_reject");
    smtp_printf("554 SMTP service not available\r\n" );
    return FALSE;
    }
  #endif

  /* Reject connections from any host in the reject list, and also for any host
  in the reject_recipients list if a lookup deferred while checking it. */

  if (verify_check_host(&host_reject, TRUE) ||
       ((reject_recipients = verify_check_host(&host_reject_recipients, TRUE))
         && search_find_defer))
    {
    log_write(1, LOG_MAIN|LOG_REJECT, "connection from %s refused%s%s",
      host_and_ident("", NULL), host_lookup_msg,
      search_find_defer? " (lookup deferred)" : "");
    smtp_send_prohibition_message(554, "host_reject");
    smtp_printf("554 SMTP service not available%s\r\n", host_lookup_msg);
    return FALSE;
    }

  /* Reject recipients from any host in a reject_recipients list. */

  if (reject_recipients)
    {
    log_write(1, LOG_MAIN|LOG_REJECT, "recipients from %s refused%s",
      host_and_ident("", NULL), host_lookup_msg);
    host_refuse_all_rcpts = TRUE;
    }

  /* If we are not yet refusing recipients, and any Realtime Blocking List
  domains are configured, and the host matches rbl_hosts, look up this host in
  the RBL domains, and if it is found in any one of them, reject recipients if
  so configured. Otherwise set a flag and create a warning header if required.
  Logging will happen later, possibly including the headers if wanted. */

  rbl_host = FALSE;
  if (!host_refuse_all_rcpts && rbl_domains != NULL &&
      verify_check_host(&rbl_hosts, TRUE))
    {
    char domain[256];
    char *listptr = rbl_domains;
    int sep = 0;

    while (string_nextinlist(&listptr, &sep, domain, sizeof(domain)) != NULL)
      {
      BOOL reject = rbl_reject_recipients;
      BOOL accept = FALSE;
      char *s = strrchr(domain, '/');

      HDEBUG(9) debug_printf("checking RBL domain %s\n", domain);

      /* Adjust rejection per domain, or skip if the host is in
      host_accept_relay and /skiprelay is set. */

      while (s != NULL)
        {
        *s++ = 0;
        if (strcmp(s, "warn") == 0) accept = reject = FALSE;
        else if (strcmp(s, "reject") == 0) { reject = TRUE; accept = FALSE; }
        else if (strcmp(s, "accept") == 0) { accept = TRUE; reject = FALSE; }
        else if (strcmp(s, "skiprelay") == 0)
          {
          if (! host_allow_relay_anywhere_set) check_host_for_relay();
          if (host_allow_relay_anywhere)
            {
            HDEBUG(9) debug_printf("RBL check skipped because host is in "
              "host_accept_relay\n");
            goto RBL_NEXT; /* Skip this check */
            }
          }
        else
          log_write(0, LOG_MAIN|LOG_PANIC, "\"/%s\" unknown in rbl_domains - "
            "ignored", s);

        s = strrchr(domain, '/');
        }

      /* Now see if there is an IP list for classes of RBL that we act on;
      this goes before the /option, and starts with an = char. */

      s = strchr(domain, '=');
      if (s != NULL) *s++ = 0;

      /* Now look up the DNS entry. If it succeeds and rbl_warn_header is set,
      add the warning header, even if reject is true, because there may be
      recipients that are allowed through by an exception list. */

      switch(host_check_rbl(sender_host_address, domain, s, &rbl_msg_buffer))
        {
        case DNS_SUCCEED:
        if (rbl_warn_header)
          {
          if (rbl_header == NULL)
            {
            rbl_header = store_get(sizeof(header_line));
            rbl_header->text = NULL;
            }
          rbl_header->text = string_sprintf("%sX-RBL-Warning: (%s) %s\n",
            (rbl_header->text == NULL)? "" : rbl_header->text,
            domain, rbl_msg_buffer);
          rbl_header->type = htype_other;
          rbl_header->slen = (int)strlen(rbl_header->text);
          }

        /* Handle explicit accept (white list) */

        if (accept)
          {
          log_write(1, LOG_MAIN|LOG_REJECT, "recipients accepted from %s "
            "(RBL %s/accept)", host_and_ident("", NULL), domain);
          listptr = NULL;        /* To break the loop */
          }

        /* Handle rejection */

        else if (reject)
          {
          log_write(1, LOG_MAIN|LOG_REJECT, "recipients refused from %s "
            "(RBL %s)", host_and_ident("", NULL), domain);
          host_refuse_all_rcpts = TRUE;
          rbl_domain = string_copy(domain);
          listptr = NULL;        /* To break the loop */
          }

        /* Otherwise it is a warning */

        else
          {
          log_write(1, LOG_MAIN|LOG_REJECT, "%s in RBL list at %s "
            "(warning mode)", host_and_ident("", NULL), domain);
          rbl_host = TRUE;
          rbl_msg_buffer = NULL;   /* In case rejected by another criterion */
          }
        break;

        case DNS_NOMATCH:
        break;                 /* Continue looking */

        case DNS_AGAIN:        /* Continue at the moment; should there be */
        case DNS_FAIL:         /* an option for a soft reject? */
        break;
        }

      RBL_NEXT: continue;      /* Target for breaking inner loop */
      }
    }

  /* Unless we are refusing all recipients, check to see if this host
  must authenticate before doing anything. */

  #ifdef HAVE_AUTH
  if (!host_refuse_all_rcpts && verify_check_host(&auth_hosts, TRUE))
    host_must_authenticate = TRUE;

  /* Check to see if authentication must take place over TLS */

  #ifdef SUPPORT_TLS
  host_must_use_tls_to_authenticate =
    verify_check_host(&auth_over_tls_hosts, TRUE);
  #endif
  #endif

  /* Similarly for the use of TLS */

  #ifdef SUPPORT_TLS
  if (!host_refuse_all_rcpts && verify_check_host(&tls_hosts, TRUE))
    host_must_use_tls = TRUE;
  #endif

  /* Check for reserved slots. Note that the count value doesn't include
  this process, as it gets upped in the parent process. */

  if (smtp_accept_max > 0 &&
      smtp_accept_count + 1 > smtp_accept_max - smtp_accept_reserve)
    {
    if (!verify_check_host(&smtp_reserve_hosts, FALSE))
      {
      log_write(1, LOG_MAIN, "Connection from %s temporarily refused: not in "
        "reserve list: connected=%d max=%d reserve=%d",
        host_and_ident("", NULL), smtp_accept_count, smtp_accept_max,
        smtp_accept_reserve);
      smtp_printf("421 %s: Too many concurrent SMTP connections; "
        "please try again later\r\n", primary_hostname);
      return FALSE;
      }
    reserved_host = TRUE;
    }

  /* If a load level above which only messages from reserved hosts are
  accepted is set, check the load. For incoming calls via the daemon, the
  check is done in the superior process if there are no reserved hosts, to
  save a fork. In all cases, the load average will already be available
  in a global variable at this point. */

  if (smtp_load_reserve >= 0 &&
       load_average > smtp_load_reserve &&
       !reserved_host &&
       !verify_check_host(&smtp_reserve_hosts, FALSE))
    {
    log_write(1, LOG_MAIN, "Connection from %s temporarily refused: not in "
      "reserve list and load average = %.2f", host_and_ident("", NULL),
      (double)load_average/1000.0);
    smtp_printf("421 %s: Too much load; please try again later\r\n",
      primary_hostname);
    return FALSE;
    }

  /* Determine whether unqualified senders or recipients are permitted
  for this host. Unfortunately, we have to do this every time, in order to
  set the flags so that they can be inspected when considering qualifying
  addresses in the headers. For a site that permits no qualification, this
  won't take long, however. */

  allow_unqualified_sender = verify_check_host(&sender_unqualified_hosts,
    FALSE);

  allow_unqualified_recipient = verify_check_host(&receiver_unqualified_hosts,
    FALSE);

  /* Determine whether HELO/EHLO is required for this host. */

  helo_required = verify_check_host(&helo_verify, TRUE);

  /* Determine whether this hosts is permitted to send syntactic junk
  after a HELO or EHLO command. */

  helo_accept_junk = verify_check_host(&helo_accept_junk_hosts, FALSE);
  }

/* For batch SMTP input we are now done. */

if (smtp_batched_input) return TRUE;

/* log connections if requested */

if (log_smtp_connections)
  log_write(4, LOG_MAIN, "SMTP connection from %s",
    sender_host_unknown? sender_ident : sender_fullhost);

/* Output the initial message for a two-way SMTP connection. It may contain
newlines, which then cause a multi-line response to be given. */

s = expand_string(smtp_banner);
if (s == NULL)
  log_write(0, LOG_PANIC_DIE, "Expansion of \"%s\" (smtp_banner) failed: %s",
    smtp_banner, expand_string_message);

/* Remove any terminating newlines; might as well remove trailing space too */

p = s + (int)strlen(s);
while (p > s && isspace((uschar)p[-1])) p--;
*p = 0;

/* It seems that CC:Mail is braindead, and assumes that the greeting message
is all contained in a single IP packet. The original code wrote out the
greeting using several calls to fprint/fputc, and on busy servers this could
cause it to be split over more than one packet - which caused CC:Mail to fall
over when it got the second part of the greeting after sending its first
command. Sigh. To try to avoid this, build the complete greeting message
first, and output it in one fell swoop. This gives a better chance of it
ending up as a single packet. */

ss = store_get(size);
ptr = 0;

p = s;
while (*p != 0)
  {
  int len;
  char *linebreak = strchr(p, '\n');
  if (linebreak == NULL)
    {
    len = (int)strlen(p);
    ss = string_cat(ss, &size, &ptr, "220 ", 4);
    }
  else
    {
    len = linebreak - p;
    ss = string_cat(ss, &size, &ptr, "220-", 4);
    }
  ss = string_cat(ss, &size, &ptr, p, len);
  ss = string_cat(ss, &size, &ptr, "\r\n", 2);
  p += len;
  if (linebreak != NULL) p++;
  }
ss[ptr] = 0;  /* string_cat leaves room for this */

smtp_printf("%s", ss);
return TRUE;
}





/*************************************************
*     Log SMTP syntax error if so configured     *
*************************************************/

/* Write to the log for SMTP syntax errors in incoming commands, if configured
to do so.

Argument:   the error message
Returns:    nothing
*/

static void
log_syntax_error(char *errmess)
{
if (!log_smtp_syntax_errors) return;
log_write(4, LOG_MAIN, "SMTP syntax error in \"%s\"%s: %s",
  string_printing(cmd_buffer), host_and_ident(" H=", " U="), errmess);
}




/*************************************************
*     Check authenticated and/or using TLS       *
*************************************************/

/* This function checks that the caller is authenticated and/or is using TLS,
if the configuration requires it.

Argument:  name of command (for logging)
Returns:   TRUE if OK
*/

static BOOL
check_auth_or_tls(char *name)
{
name = name;       /* Suppress compile error when neither configured */

#ifdef HAVE_AUTH
if (host_must_authenticate && sender_host_authenticated == NULL)
  {
  smtp_printf("530 Authentication required\r\n");
  log_write(0, LOG_MAIN|LOG_REJECT, "rejected %s from %s: "
    "not authenticated", name, sender_fullhost);
  return FALSE;
  }
#endif

#ifdef SUPPORT_TLS
if (host_must_use_tls && tls_active < 0)
  {
  smtp_printf("530 Use of TLS required\r\n");
  log_write(0, LOG_MAIN|LOG_REJECT, "rejected %s from %s: "
    "not using TLS", name, sender_fullhost);
  return FALSE;
  }
#endif

return TRUE;
}




/*************************************************
*       Initialize for SMTP incoming message     *
*************************************************/

/* This function conducts the initial dialogue at the start of an incoming SMTP
message, and builds a list of recipients. However, if the incoming message
is part of a batch (-bS option) a separate function is called since it would
be messy having tests splattered about all over this function. This function
therefore handles the case where interaction is occurring. The input and output
files are set up in smtp_in and smtp_out.

The global recipients_list is set to point to a vector of recipient_item
blocks, whose number is given by recipients_count. This is extended by the
accept_add_recipient() function. The global variable sender_address is set to
the sender's address. The yield is +1 if a message has been successfully
started, 0 if a QUIT command was encountered or the connection was refused from
the particular host, or -1 if the connection was lost.

Argument: none

Returns:  > 0 message successfully started (reached DATA)
          = 0 QUIT read or end of file reached or call refused, or call
              dropped because of too many unknown commands
          < 0 lost connection
*/

int
smtp_setup_msg(void)
{
int done = 0;
int rcount = 0;
int rcpt_count = 0;
BOOL toomany = FALSE;
void *reset_point = store_get(0);

DEBUG(2) debug_printf("smtp_setup_msg entered\n");

/* Reset for start of new message. */

smtp_reset(reset_point);
message_ended = END_NOTSTARTED;

/* Batched SMTP is handled in a different function. */

if (smtp_batched_input) return smtp_setup_batch_msg();

/* Deal with SMTP commands. This loop is exited by setting done to a POSITIVE
value. The values are 2 larger than the required yield of the function. */

while (done <= 0)
  {
  char **argv;
  char *argvector[6 + MAX_CLMACROS];
  char *etrn_command;
  char *errmess;
  char *receiver = NULL;
  char *orig_receiver = NULL;
  char *hello = NULL;
  char *s;
  char *orcpt;
  char *lcdomain;
  char *lclp;
  int flags;
  int mail_from_size = -1;
  void (*oldsignal)(int);
  BOOL islocal_domain;
  pid_t pid;
  int errcode, start, end, sender_domain, receiver_domain;
  int ptr, size, sep1, sep2;

  #ifdef HAVE_AUTH
  int c;
  auth_instance *au;
  #endif

  switch(smtp_read_command())
    {
    /* The AUTH command is not permitted to occur inside a transaction, and may
    occur only once per connection. The code is compiled only if some
    authentication mechanisms are included in the binary. */

    #ifdef HAVE_AUTH
    case AUTH_CMD:
    if (sender_host_authenticated != NULL)
      {
      smtp_printf("503 Already authenticated\r\n");
      break;
      }
    if (sender_address != NULL)
      {
      smtp_printf("503 Not permitted in mail transaction\r\n");
      break;
      }

    /* Reject if permitted only in a TLS session. */

    #ifdef SUPPORT_TLS
    if (host_must_use_tls_to_authenticate && tls_active < 0)
      {
      smtp_printf("503 STARTTLS required before AUTH\r\n");
      break;
      }
    #endif

    /* Find the name of the requested authentication mechanism. */

    s = smtp_data;
    while ((c = *smtp_data) != 0 && !isspace(c))
      {
      if (!isalnum(c) && c != '-' && c != '_')
        {
        smtp_printf("501 Invalid character in name\r\n");
        goto COMMAND_LOOP;
        }
      smtp_data++;
      }

    /* If not at the end of the line, we must be at white space. Terminate the
    name and move the pointer on to any data that may be present. */

    if (*smtp_data != 0)
      {
      *smtp_data++ = 0;
      while (isspace((uschar)*smtp_data)) smtp_data++;
      }

    /* Search for an authentication mechanism which is configured for use
    as a server. */

    for (au = auths; au != NULL; au = au->next)
      {
      if (strcmpic(s, au->public_name) == 0 && au->server) break;
      }

    if (au == NULL)
      {
      smtp_printf("504 %s mechanism not supported\r\n", s);
      break;
      }

    /* Run the checking code, passing the remainder of the command
    line as data. Initialize $0 empty. The authenticator may set up
    other numeric variables. If authentication succeeds, expand the
    set_id string if set and put it in authenticated_id. Save this in
    permanent store, as the working store gets reset at HELO, RSET, etc. */

    expand_nmax = 0;
    expand_nlength[0] = 0;   /* $0 contains nothing */

    if ((c = (au->info->servercode)(au, smtp_data)) == OK)
      {
      if (au->set_id != NULL &&
          (authenticated_id = expand_string(au->set_id)) == NULL)
        {
        auth_defer_msg = expand_string_message;
        c = DEFER;
        }
      else
        {
        if (authenticated_id != NULL)
          authenticated_id = string_copy_malloc(authenticated_id);
        sender_host_authenticated = au->name;
        if (!host_allow_relay_anywhere)             /* Re-evaluate this */
          host_allow_relay_anywhere_set = FALSE;    /* as it might change */
        received_protocol =
          protocols[pauthed] +((sender_host_address != NULL)? pnlocal : 0);
        }
      }

    expand_nmax = -1;  /* Reset numeric variables */

    switch(c)
      {
      case OK:
      s = "235 Authentication succeeded";
      authenticated_by = au;
      break;

      case DEFER:
      s = string_sprintf("435 Unable to authenticate at present: %s",
        auth_defer_msg);
      break;

      case BAD64:
      s = "501 Invalid base64 data";
      break;

      case FORCEFAIL:
      s = "501 Authentication cancelled";
      break;

      case UNEXPECTED:
      s = "553 Initial data not expected";
      break;

      case FAIL:
      s = "535 Incorrect authentication data";
      break;

      default:
      s = string_sprintf("435 Internal error: return %d from authentication "
        "check", c);
      break;
      }

    smtp_printf("%s\r\n", s);
    if (c != OK)
      log_write(0, LOG_MAIN|LOG_REJECT, "Authentication failed for %s: %s",
        host_and_ident("", NULL), s);

    break;  /* AUTH_CMD */
    #endif

    /* The HELO/EHLO commands are permitted to appear in the middle of
    a session as well as at the beginning. They have the effect of a
    reset in addition to their other functions. Their absence at the
    start cannot be taken to be an error. */

    case HELO_CMD:
    hello = "HELO";
    esmtp = FALSE;
    goto HELO_EHLO;

    case EHLO_CMD:
    hello = "EHLO";
    esmtp = TRUE;

    HELO_EHLO:      /* Common code for HELO and EHLO */

    received_protocol = protocols[
      (sender_host_authenticated != NULL)? pauthed :
      (esmtp? pextend : pnormal)] +
        ((sender_host_address != NULL)? pnlocal : 0);

    /* Reset for the start of a message, even if the HELO causes trouble. */

    smtp_reset(reset_point);
    rcount = 0;
    toomany = FALSE;

    /* This gets set true if all goes well */

    helo_seen = FALSE;

    /* Reject the HELO if its argument was invalid or non-existent. A
    successful check causes the argument to be saved in malloc store. */

    if (!check_helo(smtp_data))
      {
      smtp_printf("501 Syntactically invalid %s argument(s)\r\n", hello);
      log_write(0, LOG_MAIN|LOG_REJECT, "rejected %s from %s: syntactically "
        "invalid argument(s): %s", hello,
        (sender_fullhost == NULL)? "local process" : sender_fullhost,
        (*smtp_data == 0)? "(no argument given)" :
                           string_printing(smtp_data));
      break;
      }

    /* If sender_host_unknown is true, we have got here via the -bs interface,
    not called from inetd. Otherwise, we are running an IP connection and the
    host address will be set. If the helo name is the primary name of this
    host and we haven't done a reverse lookup, force one now. If helo_required
    is set, ensure that the HELO name matches the actual host. */

    if (!sender_host_unknown)
      {
      char *p = smtp_data;
      while (*p != 0 && !isspace((uschar)*p)) { *p = tolower(*p); p++; }
      *p = 0;

      /* Force a reverse lookup if HELO quoted our name or the name of one of
      our local domains. If there's a lookup defer while checking
      local_domains, just carry on without doing the reverse lookup. If it is a
      persistent failure it will cause soft errors later. */

      if (sender_host_name == NULL &&
          (strcmp(sender_helo_name, primary_hostname) == 0 ||
            match_isinlist(sender_helo_name,&local_domains,TRUE,TRUE,NULL)))
        host_name_lookup();

      /* Rebuild the fullhost info to include the HELO name (and the real name
      if it was looked up.) */

      host_build_sender_fullhost();  /* Rebuild */
      set_process_info("handling%s incoming connection from %s",
        (tls_active >= 0)? " TLS" : "", sender_fullhost);

      /* Verify if required. This doesn't give much security, but it does make
      some people happy to be able to do it. Note that HELO is legitimately
      allowed to quote an address literal. */

      if (helo_required)
        {
        BOOL OK = TRUE;

        /* Check an address literal. Allow for IPv6 ::ffff: literals. */

        if (sender_helo_name[0] == '[')
          {
          OK = strncmp(sender_helo_name+1, sender_host_address,
            (int)strlen(sender_host_address)) == 0;

          #if HAVE_IPV6
          if (!OK)
            {
            if (strncmpic(sender_host_address, "::ffff:", 7) == 0)
              OK = strncmp(sender_helo_name+1, sender_host_address + 7,
                (int)strlen(sender_host_address) - 7) == 0;
            }
          #endif
          }

        /* Do a reverse lookup if one hasn't already been done. If that fails,
        or the name doesn't match, try checking with a forward lookup. */

        else
          {
          if (sender_host_name == NULL && !host_lookup_failed)
            (void)host_name_lookup();

          if (host_lookup_failed ||
               strcmpic(sender_host_name, sender_helo_name) != 0)
            {
            host_item h;
            h.name = sender_helo_name;
            h.address = NULL;
            h.next = NULL;
            OK = FALSE;
            HDEBUG(9) debug_printf("getting IP address for %s\n",
              sender_helo_name);
            if (host_find_byname(&h, NULL, NULL, FALSE) != HOST_FIND_FAILED)
              {
              host_item *hh = &h;
              while (hh != NULL)
                {
                if (strcmp(hh->address, sender_host_address) == 0)
                  {
                  OK = TRUE;
                  HDEBUG(9) debug_printf("IP address for %s matches calling "
                    "address\n", sender_helo_name);
                  break;
                  }
                hh = hh->next;
                }
              }
            }
          }

        if (!OK)
          {
          smtp_printf("550 %s argument does not match calling host\n", hello);
          log_write(0, LOG_MAIN|LOG_REJECT, "rejected %s from %s", hello,
            sender_fullhost);
          break;
          }
        }
      }

    /* Generate an OK reply, including the ident if present, and also
    the IP address if present. Reflecting back the ident is intended
    as a deterrent to mail forgers. For maximum efficiency, and also
    because some broken systems expect each response to be in a single
    packet, arrange that it is sent in one write(). */

    s = string_sprintf("250 %s Hello %s%s%s",
      primary_hostname,
      (sender_ident == NULL)?  "" : sender_ident,
      (sender_ident == NULL)?  "" : " at ",
      (sender_host_name == NULL)? sender_helo_name : sender_host_name);

    ptr = (int)strlen(s);
    size = ptr + 1;

    if (sender_host_address != NULL)
      {
      s = string_cat(s, &size, &ptr, " [", 2);
      s = string_cat(s, &size, &ptr, sender_host_address,
        (int)strlen(sender_host_address));
      s = string_cat(s, &size, &ptr, "]", 1);
      }

    s = string_cat(s, &size, &ptr, "\r\n", 2);

    /* If we received EHLO, we must create a multiline response which includes
    the functions supported. */

    if (esmtp)
      {
      s[3] = '-';

      /* I'm not entirely happy with this, as an MTA is supposed to check
      that it has enough room to accept a message of maximum size before
      it sends this. However, there seems little point in not sending it.
      The actual size check happens later at MAIL FROM time. By postponing it
      till then, VRFY and EXPN can be used after EHLO when space is short. */

      if (message_size_limit > 0)
        {
        sprintf(big_buffer, "250-SIZE %d\r\n", message_size_limit);
        s = string_cat(s, &size, &ptr, big_buffer, (int)strlen(big_buffer));
        }
      else
        {
        s = string_cat(s, &size, &ptr, "250-SIZE\r\n", 10);
        }

      /* Exim does not do protocol conversion or data conversion. It is 8-bit
      clean; if it has an 8-bit character in its hand, it just sends it. It
      cannot therefore specify 8BITMIME and remain consistent with the RFCs.
      However, some users want this option simply in order to stop MUAs
      mangling messages that contain top-bit-set characters. It is therefore
      provided as an option. */

      if (accept_8bitmime)
        s = string_cat(s, &size, &ptr, "250-8BITMIME\r\n", 14);

      /* Advertise DSN support if configured to do so. This partial attempt at
      doing DSN was abandoned. This obsolete code is left here just in case.
      Nothing is documented. */

      #ifdef SUPPORT_DSN
      if (dsn)
        s = string_cat(s, &size, &ptr, "250-DSN\r\n", 9);
      #endif

      /* Advertise ETRN if any hosts are permitted to issue it; a check is
      made when any host actually does. */

      if (smtp_etrn_hosts != NULL)
        {
        s = string_cat(s, &size, &ptr, "250-ETRN\r\n", 10);
        }

      /* Advertise EXPN if any hosts are permitted to issue it; a check is
      made when any host actually does. */

      if (smtp_expn_hosts != NULL)
        {
        s = string_cat(s, &size, &ptr, "250-EXPN\r\n", 10);
        }

      /* Exim is quite happy with pipelining, so let the other end know that
      it is safe to use it. */

      s = string_cat(s, &size, &ptr, "250-PIPELINING\r\n", 16);

      /* If any server authentication mechanisms are configured, advertise
      them, if either auth_always_advertise is set, or if the current host is
      in host_auths or host_auth_accept_relay without being in host_accept
      relay, that is, if authentication is (probably) required. The problem
      with advertising always is that some clients then require users to
      authenticate (and aren't configurable otherwise) even though it may not
      be necessary (e.g. if the host is in host_accept_relay).

      Do not advertise AUTH if the host is in auth_over_tls_hosts and we are
      not in a TLS session.

      RFC 2222 states that SASL mechanism names contain only upper case
      letters, so output the names in upper case, though we actually recognize
      them in either case in the AUTH command. */

      #ifdef HAVE_AUTH
      if (auths != NULL)
        {
        BOOL advertise = auth_always_advertise || host_must_authenticate;

        if (!advertise)
          {
          check_host_for_relay();
          advertise = !host_allow_relay_anywhere &&
            verify_check_host(&host_auth_accept_relay, FALSE);
          }

        if (host_must_use_tls_to_authenticate && tls_active < 0)
          advertise = FALSE;

        if (advertise)
          {
          auth_instance *au;
          BOOL first = TRUE;
          for (au = auths; au != NULL; au = au->next)
            {
            if (au->server)
              {
              int saveptr;
              if (first)
                {
                s = string_cat(s, &size, &ptr, "250-AUTH", 8);
                first = FALSE;
                }
              saveptr = ptr;
              s = string_cat(s, &size, &ptr, " ", 1);
              s = string_cat(s, &size, &ptr, au->public_name,
                (int)strlen(au->public_name));
              while (++saveptr < ptr) s[saveptr] = toupper(s[saveptr]);
              }
            }
          if (!first) s = string_cat(s, &size, &ptr, "\r\n", 2);
          }
        }
      #endif

      /* Advertise TLS (Transport Level Security) aka SSL (Secure Socket Layer)
      if it has been included in the binary, and the host matches
      tls_advertise_hosts or is required to use it. We must *not* advertise if
      if we are already in a secure connection. */

      #ifdef SUPPORT_TLS
      if (tls_active < 0 &&
          (verify_check_host(&tls_advertise_hosts, TRUE) || host_must_use_tls))
	s = string_cat(s, &size, &ptr, "250-STARTTLS\r\n", 14);
      #endif

      /* Finish off the multiline reply with one that is always available. */

      s = string_cat(s, &size, &ptr, "250 HELP\r\n", 10);
      }

    /* Terminate the string (for debug), write it, and note that HELO/EHLO
    has been seen. */

    s[ptr] = 0;

    #ifdef SUPPORT_TLS
    if (tls_active >= 0) (void)tls_write(s, ptr); else
    #endif

    fwrite(s, 1, ptr, smtp_out);
    DEBUG(3) debug_printf("SMTP>> %s", s);
    helo_seen = TRUE;
    break;


    /* The MAIL FROM command requires an address as an operand. All we
    do here is to parse it for syntactic correctness. The form "<>" is
    a special case which converts into an empty string. The start/end
    pointers in the original are not used further for this address, as
    it is the canonical extracted address which is all that is kept. */

    case MAIL_CMD:
    if (!check_auth_or_tls("MAIL")) break;

    if (helo_required && !helo_seen)
      {
      smtp_printf("550 HELO or EHLO required\r\n");
      log_write(0, LOG_MAIN|LOG_REJECT, "rejected MAIL FROM from %s: no "
        "HELO/EHLO given", sender_fullhost);
      break;
      }

    if (sender_address != NULL)
      {
      smtp_printf("503 Sender already given\r\n");
      break;
      }

    if (smtp_data[0] == 0)
      {
      smtp_printf("501 MAIL FROM must have an address operand\r\n");
      break;
      }

    /* Reset for start of message - even if this is going to fail, we
    obviously need to throw away any previous data. */

    smtp_reset(reset_point);
    rcount = rcpt_count = 0;
    toomany = FALSE;

    /* Reset the flag that refuses all recipients for bad senders when
    the reject style is configured that way. */

    sender_refuse_all_rcpts = FALSE;

    /* Loop, checking for ESMTP additions to the MAIL FROM command. */

    if (esmtp) for(;;)
      {
      char *name, *value, *end;
      int size;

      if (!extract_option(&name, &value)) break;

      /* Handle SIZE= by reading the value. We don't do the check till later,
      in order to be able to log the sender address on failure. */

      if (strcmpic(name, "SIZE") == 0 &&
          ((size = (int)strtoul(value, &end, 10)), *end == 0))
        mail_from_size = size;

      /* If this session was initiated with EHLO and accept_8bitmime is set,
      Exim will have indicated that it supports the BODY=8BITMIME option. In
      fact, it does not support this according to the RFCs, in that it does not
      take any special action for forwarding messages containing 8-bit
      characters. That is why accept_8bitmime is not the default setting, but
      some sites want the action that is provided. We recognize both "8BITMIME"
      and "7BIT" as body types, but take no action. */

      else if (accept_8bitmime && strcmpic(name, "BODY") == 0 &&
          (strcmpic(value, "8BITMIME") == 0 ||
           strcmpic(value, "7BIT") == 0)) {}

      /* Handle the two DSN options, but only if configured to do so (which
      will have caused "DSN" to be given in the EHLO response). The code itself
      is included only if configured in at build time. This partial attempt at
      doing DSN was abandoned. This obsolete code is left here just in case.
      Nothing is documented. */

      #ifdef SUPPORT_DSN
      else if (dsn && strcmpic(name, "RET") == 0)
        dsn_ret = (strcmpic(value, "HDRS") == 0)? dsn_ret_hdrs :
                  (strcmpic(value, "FULL") == 0)? dsn_ret_full : 0;

      else if (dsn && strcmpic(name, "ENVID") == 0)
        dsn_envid = string_copy(value);
      #endif

      /* Handle the AUTH extension, but only if configured to do so at build
      time. If the sending host is authenticated, and the value given is
      not "<>" we set it up as the authenticated sender. However, if the
      authenticator set a condition to be tested, we ignore AUTH on MAIL unless
      the condition is met. The value of AUTH is an xtext, which means that +,
      = and cntrl chars are coded in hex; however "<>" will be unaffected by
      this coding. */

      #ifdef HAVE_AUTH
      else if (strcmpic(name, "AUTH") == 0)
        {
        if (sender_host_authenticated != NULL && strcmp(value, "<>") != 0)
          {
          if (auth_xtextdecode(value, &authenticated_sender) < 0)
            {
            smtp_printf("501 %s: invalid AUTH=%s\r\n", smtp_data, value);
            goto COMMAND_LOOP;
            }
          if (authenticated_by->mail_auth_condition != NULL &&
              !expand_check_condition(authenticated_by->mail_auth_condition,
                  authenticated_by->name, "authenticator"))
            {
            authenticated_sender = NULL;
            log_write(2, LOG_MAIN, "ignoring AUTH=%s from authenticated ID %s",
              value, authenticated_id);
            }
          }
        }
      #endif

      /* Unknown option. Stick back the terminator characters and break
      the loop. An error for a malformed address will occur. */

      else
        {
        name[-1] = ' ';
        value[-1] = '=';
        break;
        }
      }

    /* Now extract the address, first applying any SMTP-time rewriting. The
    TRUE flag allows "<>" as a sender address. */

    raw_sender = ((rewrite_existflags & rewrite_smtp) != 0)?
      rewrite_one(smtp_data, rewrite_smtp, NULL, FALSE, "",
        global_rewrite_rules) : smtp_data;

    rfc821_domains = TRUE;
    raw_sender =
      parse_extract_address(raw_sender, &errmess, &start, &end, &sender_domain,
        TRUE);
    rfc821_domains = FALSE;

    if (raw_sender == NULL)
      {
      log_syntax_error(errmess);
      smtp_printf("501 %s: %s\r\n", smtp_data, errmess);
      break;
      }

    sender_address = raw_sender;

    /* If there is a configured size limit for mail, check that this message
    doesn't exceed it. The check is postponed to this point so that the sender
    can be logged. The log level is 2 to match the rejection in accept.c. */

    if (message_size_limit > 0 && mail_from_size > message_size_limit)
      {
      smtp_printf("552 Message size exceeds maximum permitted\r\n");
      log_write(2, LOG_MAIN|LOG_REJECT, "rejected MAIL FROM:<%s>%s: "
          "message too large: size=%d max=%d",
          sender_address,
          host_and_ident(" H=", " U="),
          mail_from_size,
          message_size_limit);
      break;
      }

    /* Check there is enough space on the disc unless configured not to.
    When smtp_check_spool_space is set, the check is for message_size_limit
    plus the current message - i.e. we accept the message only if it won't
    reduce the space below the threshold. Add 5000 to the size to allow for
    overheads such as the Received: line and storing of recipients, etc.
    By putting the check here, even when SIZE is not given, it allow VRFY
    and EXPN etc. to be used when space is short. */

    if (!accept_check_fs(
         (smtp_check_spool_space && mail_from_size >= 0)?
            mail_from_size + 5000 : 0))
      {
      smtp_printf("452 space shortage, please try later\r\n");
      sender_address = NULL;
      break;
      }

    /* If sender_address is unqualified, reject it, unless this is a
    locally generated message, in which case it will be ignored anyway.
    However, if the sending host or net is listed as permitted to send
    unqualified addresses - typically local machines behaving as MUAs -
    then just qualify the address. The flag is set above at the start
    of the SMTP connection. */

    if (!sender_local && sender_domain == 0 && sender_address[0] != 0)
      {
      if (allow_unqualified_sender)
        {
        sender_domain = (int)strlen(sender_address) + 1;
        sender_address = rewrite_address_qualify(sender_address, FALSE);
        DEBUG(9) debug_printf("unqualified address %s accepted\n",
          raw_sender);
        }
      else
        {
        smtp_printf("501 %s: sender address must contain a domain\r\n",
          smtp_data);
        log_write(1, LOG_MAIN|LOG_REJECT, "unqualified sender rejected: "
          "<%s>%s%s",
          raw_sender,
          host_and_ident(" H=", " U="),
          host_lookup_msg);
        sender_address = NULL;
        break;
        }
      }

    /* If configured to reject any senders explicitly (spam filtering),
    check now, but don't bother if the host has already been rejected. The
    sender check is independent of sender verification, and does not
    happen for local senders. We must always allow through the null
    sender, though. If a lookup defers, we give a soft error to the MAIL
    command, for both sender_reject and sender_reject_recipients; we want
    the remote to try again, so there's little point saving the 4xx for
    the RCPT in the latter case. */

    if (!host_refuse_all_rcpts && !sender_local && sender_address[0] != 0)
      {
      BOOL rr;
      sep1 = sep2 = 0;

      if (match_address_list(sender_address, sender_domain, TRUE,
            &sender_reject, -1, &sep1, TRUE) ||
          ((rr = match_address_list(sender_address, sender_domain, TRUE,
            &sender_reject_recipients, -1, &sep2, TRUE)) && search_find_defer))
        {
        int rc = search_find_defer? 451 : 550;
        smtp_send_prohibition_message(rc, "sender_reject");
        smtp_printf("%d rejected: administrative prohibition%s\r\n",
          rc, host_lookup_msg);
        log_write(1, LOG_MAIN|LOG_REJECT, "sender rejected: <%s>%s%s%s",
          raw_sender,
          host_and_ident(" H=", " U="),
          host_lookup_msg,
          search_find_defer? " (lookup deferred)" : "");
        sender_address = NULL;
        break;     /* Ends case statement - MAIL FROM finished */
        }

      /* Handle sender_reject_recipients when no lookup defer */

      if (rr)
        {
        sender_refuse_all_rcpts = TRUE;
        log_write(1, LOG_MAIN|LOG_REJECT, "recipients refused from <%s>%s",
          raw_sender,
          host_and_ident(" H=", " U="));
        }
      }

    /* If configured to check sender addresses, do the preliminary check
    now, unless the sender is local (in which case whatever is given here
    is ignored anyway). The check will fail if the message is to be refused at
    this stage. Another check function is called after the message has been
    received, to do more checking when the headers are available. However,
    don't bother with the check if all recipients are in any case going to
    be refused. */

    errmess = NULL;
    refuse_all_rcpts = FALSE;

    if (sender_verify || sender_try_verify)
      {
      char *old_sender_address = sender_address;

      if (!sender_local &&
          !host_refuse_all_rcpts &&
          !sender_refuse_all_rcpts &&
          !verify_sender_preliminary(&errcode, &errmess))
        {
        smtp_send_prohibition_message(errcode, "sender_verify");
        smtp_printf("%d rejected: %s <%s>\r\n", errcode, errmess, raw_sender);
        log_write(1, LOG_MAIN|LOG_REJECT, "rejected MAIL FROM:<%s>%s %s",
          raw_sender, host_and_ident(" H=", " U="), errmess);
        sender_address = NULL;
        sender_address_unrewritten = NULL;
        break;
        }

      /* Verification may cause rewriting of the address. We need to reset
      sender_domain as it might be used later on when checking recipients
      for relay permissions. */

      if (sender_address != old_sender_address)
        sender_domain = strchr(sender_address, '@') + 1 - sender_address;
      }

    /* The sender address is acceptable - for now. If verification is running
    in warning mode, errmess is set non-NULL if there is a warning to be
    given. This is also the case when a bad address has been received 3 times
    and refuse_all_rcpts is set. If the address has been verified, a new sender
    address is always produced on success. However, we reflect the original one
    to the outside world. */

    if (errmess != NULL)
      {
      smtp_printf("%d %s <%s>\r\n", errcode, errmess, raw_sender);
      log_write(1, LOG_MAIN|LOG_REJECT, "%s <%s>%s", errmess, raw_sender,
        host_and_ident(" H=", " U="));
      }
    else
      {
      smtp_printf("250 <%s> is syntactically correct\r\n", raw_sender);
      }

    /* Note that we haven't yet checked this sender for permission to relay
    messages through this host. */

    sender_allow_relay_anywhere_set = FALSE;
    break;


    /* The RCPT TO command requires an address as an operand. All we do
    here is to parse it for syntactic correctness. There may be any number
    of RCPT TO commands, specifying multiple senders. We build them all into
    a data structure that is in argc/argv format. The start/end values
    given by parse_extract_address are not used, as we keep only the
    extracted address. */

    case RCPT_CMD:
    rcpt_count++;

    if (sender_address == NULL)
      {
      smtp_printf("503 No sender yet given\r\n");
      break;
      }

    if (smtp_data[0] == 0)
      {
      smtp_printf("501 RCPT TO must have an address operand\r\n");
      break;
      }

    /* Loop, checking for ESMTP additions to the RCPT TO command. These are
    all for DSN, and the code is included only when requested. */

    orcpt = NULL;
    flags = 0;

    #ifdef SUPPORT_DSN
    /* This partial attempt at doing DSN was abandoned. This obsolete code is
    left here just in case. Nothing is documented. */

    if (esmtp) for(;;)
      {
      char *name, *value, *end;
      int size;

      if (!extract_option(&name, &value)) break;

      if (strcmpic(name, "ORCPT") == 0) orcpt = string_copy(value);

      else if (strcmpic(name, "NOTIFY") == 0)
        {
        if (strcmpic(value, "NEVER") == 0) flags |= rf_notify_never; else
          {
          char *p = value;
          while (*p != 0)
            {
            char *pp = p;
            while (*pp != 0 && *pp != ',') pp++;
              if (*pp == ',') *pp++ = 0;
            if (strcmpic(p, "SUCCESS") == 0) flags |= rf_notify_success;
            else if (strcmpic(p, "FAILURE") == 0) flags |= rf_notify_failure;
            else if (strcmpic(p, "DELAY") == 0) flags |= rf_notify_delay;
            p = pp;
            }
          }
        }

      /* Unknown option. Stick back the terminator characters and break
      the loop. An error for a malformed address will occur. */

      else
        {
        name[-1] = ' ';
        value[-1] = '=';
        break;
        }
      }
    #endif

    /* Apply SMTP rewriting then extract the working address. Don't allow "<>"
    as a recipient address */

    orig_receiver = ((rewrite_existflags & rewrite_smtp) != 0)?
      rewrite_one(smtp_data, rewrite_smtp, NULL, FALSE, "",
        global_rewrite_rules) : smtp_data;

    rfc821_domains = TRUE;
    orig_receiver =
      parse_extract_address(orig_receiver, &errmess, &start, &end,
        &receiver_domain, FALSE);
    rfc821_domains = FALSE;

    if (orig_receiver == NULL)
      {
      log_syntax_error(errmess);
      smtp_printf("501 %s: %s\r\n", smtp_data, errmess);
      break;
      }

    receiver = orig_receiver;

    /* If the receiver address is unqualified, reject it, unless this is a
    locally generated message. However, unqualified addresses are permitted
    from a configured list of hosts and nets - typically when behaving as
    MUAs rather than MTAs. Sad that SMTP is used for both types of traffic,
    really. The flag is set at the start of the SMTP connection.

    RFC 1123 talks about supporting "the reserved mailbox postmaster"; I always
    assumed this meant "reserved local part", but the revision of RFC 821 and
    friends now makes it absolutely clear that it means *mailbox*. Consequently
    we must always qualify this address, regardless. */

    if (receiver_domain == 0)
      {
      if (allow_unqualified_recipient || strcmpic(receiver, "postmaster") == 0)
        {
        DEBUG(9) debug_printf("unqualified address %s accepted\n", receiver);
        receiver_domain = (int)strlen(receiver) + 1;
        receiver = rewrite_address_qualify(receiver, TRUE);
        }
      else
        {
        smtp_printf("501 %s: recipient address must contain a domain\r\n",
          smtp_data);
        log_write(1, LOG_MAIN|LOG_REJECT, "unqualified recipient rejected: "
          "<%s>%s%s", receiver, host_and_ident(" H=", " U="), host_lookup_msg);
        break;
        }
      }

    /* Handle various cases when all recipients are to be refused. This is
    the only known way of getting some remote mailers to give up on attempting
    to send a message.

    (1) The sender verify function sets refuse_all_rcpts when a bad sender has
        been received at least twice from the same host.
    (2) This function sets host_refuse_all_rcpts if the sending host is on
        a reject by recipients list.
    (3) This function sets sender_refuse_all_rcpts if the sender is on a
        rejection by recipients list.

    If any of these flags is set, bounce all recipients, using 550, which is
    the only version of "no" that some mailers understand, apparently.
    However, there is an exception list for the policy rejections. */

    if (refuse_all_rcpts)
      {
      if (sender_verify_callback_error != NULL)
        smtp_printf("550-%s\r\n", sender_verify_callback_error);
      smtp_printf("550 cannot route to sender address <%s>\r\n",
        sender_address);
      break;
      }

    if (host_refuse_all_rcpts || sender_refuse_all_rcpts)
      {
      BOOL reject = TRUE;
      char *which = "";

      sep1 = sep2 = 0;

      if (match_address_list(receiver, receiver_domain, TRUE,
            &recipients_reject_except, -1, &sep1, FALSE))
         {
         reject = FALSE;
         }

       else if (match_address_list(sender_address, sender_domain, TRUE,
            &recipients_reject_except_senders, -1, &sep2, FALSE))
         {
         reject = FALSE;
         which = "_senders";
         }

      if (!reject)
        {
        log_write(1, LOG_MAIN|LOG_REJECT,
          "exception recipient %s accepted from <%s>%s "
            "(recipients_reject_except%s)",
          receiver, raw_sender, host_and_ident(" H=", " U="), which);
        }

      else
        {
        char *extra;

        /* If RBL-rejected, output the message if there is no tailored
        prohibition message; otherwise make the RBL message available for
        use in the tailored message. Don't give the host lookup message for
        RBL failures - it just confuses people. Say "blacklisted" instead. */

        if (rbl_msg_buffer != NULL)
          {
          extra = " (host is blacklisted)";
          if (prohibition_message == NULL)
            smtp_printf("550-%s\r\n", rbl_msg_buffer);
          else
            smtp_send_prohibition_message(550, "rbl_reject");
          }
        else
          {
          extra = host_lookup_msg;
          smtp_send_prohibition_message(550, host_refuse_all_rcpts?
            "host_reject_recipients" : "sender_reject_recipients");
          }

        /* Now the final response line */

        if (host_refuse_all_rcpts)
          {
          smtp_printf("550 mail from %s rejected: administrative "
            "prohibition%s\r\n", sender_host_address, extra);
          }
        else  /* sender_refuse_all_rcpts */
          {
          smtp_printf("550 rejected: administrative prohibition\r\n");
          }

        if (log_refused_recipients)
          log_write(1, LOG_MAIN|LOG_REJECT, "recipient <%s> refused from %s "
            "sender=<%s>%s",
            receiver, host_and_ident("", NULL), sender_address,
            host_refuse_all_rcpts?
              ((host_lookup_msg[0] != 0)? host_lookup_msg :
               (rbl_msg_buffer != NULL)? " (RBL)" : " (host_reject_recipients)")
            :
            " (sender_reject_recipients)");
        break;
        }
      }

    /* Check maximum allowed */

    if (recipients_max > 0 && ++rcount > recipients_max)
      {
      char *code = recipients_max_reject? "552" : "452";
      smtp_printf("%s too many recipients\r\n", code);
      toomany = TRUE;
      break;
      }

    /* Check for mail relaying unless -bS or -bs input without host info, in
    which case sender_host_unknown will be set. Ensure that the domain of the
    recipient is acceptable, either because it is one that is allowed for all
    sending hosts and sending addresses, or because the host and the sender
    are permitted to relay to all domains. */

    if (!sender_host_unknown)
      {
      lcdomain = string_copylc(receiver + receiver_domain);
      islocal_domain = FALSE;
      lclp = NULL;

      /* The host test is needed only for non-local domains that are not in
      relay_domains. However, if the domain is local, we must check for use of
      the %-hack. Host_allow_relay_anywhere gets set the first time we need
      its value. (The test for for MXed domains is later.) */

      while (match_isinlist(lcdomain, &local_domains, TRUE, TRUE, NULL))
        {
        /* Don't waste effort getting the lowercase local part and doing
        any checks if there are no percent-hack domains. */

        if (percent_hack_domains != NULL)
          {
          if (lclp == NULL)
            lclp = string_copynlc(receiver, receiver_domain - 1);

          if (match_isinlist(lcdomain, &percent_hack_domains, TRUE, TRUE, NULL))
            {
            char *pc = strrchr(lclp, '%');
            if (pc != NULL)
              {
              lcdomain = pc+1;       /* Set up new checking domain */
              *pc = 0;               /* And new local part */
              continue;              /* Retest new domain for locality */
              }
            }
          }

        /* Not a %-hack domain, or local part did not contain %. */

        islocal_domain = TRUE;
        break;
        }

      /* If there was a lookup defer while checking local_domains or percent_
      hack domains, we have to give a temporary error. */

      if (search_find_defer)
        {
        smtp_printf("451 temporary local failure; please try later\r\n");
        break;
        }

      /* If not a local domain, and not a relay domain, check the host. Again,
      lookup failures cause temporary errors. */

      if (!islocal_domain &&
          !match_isinlist(lcdomain, &relay_domains, TRUE, TRUE, NULL))
        {
        BOOL permitted = FALSE;

        if (search_find_defer)
          {
          smtp_printf("451 temporary local failure; please try later\r\n");
          break;
          }

        check_host_for_relay();

        /* If the host is acceptable, check up on the sender address when
        configured to require both to be valid. If the host is not
        acceptable, check up on the sender address when configured to
        require only one of them. The "set" flag gets cleared for each new
        "MAIL FROM". This saves doing the check for multiple recipients,
        as it could involve a file lookup and therefore be expensive. */

        if (host_allow_relay_anywhere != relay_need_either &&
            !sender_allow_relay_anywhere_set)
          {
          sep1 = 0;
          sender_allow_relay_anywhere = (sender_address_relay == NULL)? TRUE :
            (
            !verify_check_host(&sender_address_relay_hosts, FALSE)
            ||
            match_address_list(sender_address, sender_domain, TRUE,
              &sender_address_relay, -1, &sep1, FALSE)
            );

          /* There's a feature to allow additional checks on sender
          addresses that appear for relays. It is a special rewrite. If
          any such rules exist, rewrite the sender and if that does change it,
          verify the new address. */

          if (sender_allow_relay_anywhere &&
              (rewrite_existflags & rewrite_X) != 0)
            {
            char *new_sender =
              rewrite_one(sender_address, rewrite_X, NULL, FALSE, "",
                global_rewrite_rules);

            if (new_sender != sender_address)
              {
              switch(verify_address(new_sender, NULL, NULL, NULL, 0))
                {
                case DEFER:
                relay_msg1 = "%d temporarily unable to check sender for "
                  "relaying permission to <%s>\r\n";
                relay_msg2 = "temporarily ";
                relay_errcode = 451;
                /* Fall through */

                case FAIL:
                sender_allow_relay_anywhere = FALSE;
                break;
                }
              }
            }

          sender_allow_relay_anywhere_set = TRUE;
          HDEBUG(9) debug_printf("sender is%s permitted to relay\n",
            sender_allow_relay_anywhere? "" : " not");
          }

        /* An option determines whether only one or both tests are needed */

        permitted = relay_need_either?
          (host_allow_relay_anywhere || sender_allow_relay_anywhere) :
          (host_allow_relay_anywhere && sender_allow_relay_anywhere);

        /* There is one last possibility, which is that the domain is one
        that is MXed to this host, and Exim is configured to permit relaying
        to all such. We delay the test till this point because it requires
        a DNS lookup and so it seems better to do the host checking first.
        It also means we can give a temporary error for DNS timeouts etc. */

        if (!permitted && relay_domains_include_local_mx)
          {
          host_item h;
          BOOL removed;
          int rc;

          h.next = NULL;
          h.name = lcdomain;
          h.address = NULL;

          rc = host_find_bydns(&h,
            NULL,       /* ignore list */
            TRUE,       /* DNS only */
            FALSE,      /* not A only */
            FALSE,      /* no widening */
            FALSE,      /* no widening */
            NULL,       /* no feedback FQDN */
            &removed);  /* feedback if local removed */

          if (rc == HOST_FOUND_LOCAL || (rc == HOST_FOUND && removed))
            {
            HDEBUG(9) debug_printf("domain is MXed to this host\n");
            permitted = TRUE;
            }
          else if (rc == HOST_FIND_AGAIN)
            {
            relay_msg1 = "%d temporarily unable to check <%s> for relaying "
              "permission\r\n";
            relay_msg2 = "temporarily ";
            relay_errcode = 451;
            }
          }

        /* Forbidden relaying. */

        if (!permitted)
          {
          smtp_send_prohibition_message(relay_errcode,
            relay_need_either? "sender+host_accept_relay" :
              host_allow_relay_anywhere? "sender_relay" : "host_accept_relay");
          smtp_printf(relay_msg1, relay_errcode, orig_receiver);
          log_write(1, LOG_MAIN|LOG_REJECT, "%srefused relay (%s) to <%s> from "
            "<%s>%s%s",
            relay_msg2,
            relay_need_either? "sender & host_accept_relay" :
              host_allow_relay_anywhere? "sender" : "host",
            orig_receiver, sender_address,
            host_and_ident(" H=", " U="),
            host_lookup_msg);
          break;     /* End of handling the RCPT TO command */
          }
        }
      }

    /* If configured to check the receiver address now, do so, but not if
    the host matches one of the exception lists. Verification can also be
    conditional on the sender or recipient matching a pattern. */

    sep1 = sep2 = 0;
    if ((receiver_verify || receiver_try_verify) &&
        (receiver_verify_addresses == NULL ||
         match_address_list(receiver, receiver_domain, TRUE,
           &receiver_verify_addresses, -1, &sep1, TRUE)
        ) &&
        (receiver_verify_senders == NULL ||
         match_address_list(sender_address, sender_domain, TRUE,
           &receiver_verify_senders, -1, &sep2, TRUE)
        ) &&
        verify_check_host(&receiver_verify_hosts, FALSE))
      {
      BOOL receiver_local;
      int rc = verify_address(receiver, NULL, &receiver_local, NULL,
        vopt_is_recipient);

      /* Failure causes a hard error */

      if (rc == FAIL)
        {
        char *s;

        if (verify_forced_errmsg != NULL)
          s = string_sprintf("550 <%s> %s", orig_receiver,
            verify_forced_errmsg);
        else if (receiver_local)
          {
          char *at = strrchr(orig_receiver, '@');
          if (at == NULL) at = orig_receiver + (int)strlen(orig_receiver);
          s = string_sprintf("550 Unknown local part %.*s in <%s>",
            at - orig_receiver, orig_receiver, orig_receiver);
          }
        else s = string_sprintf("550 Cannot route to <%s>", orig_receiver);

        smtp_send_prohibition_message(550, "receiver_verify");
        smtp_printf("%s\r\n", s);

        log_write(3, LOG_MAIN,
          "verify failed for SMTP recipient %s from <%s>%s",
          orig_receiver,
          sender_address,
          host_and_ident(" H=", " U="));
        break;     /* End of handling the RCPT TO command */
        }

      /* If verification can't be done now, give a temporary error unless
      receiver_try_verify is set, in which case accept the address, but say
      it's unverified. */

      if (rc == DEFER)
        {
        char *s;
        if (verify_forced_errmsg != NULL)
          s = string_sprintf("<%s> %s", orig_receiver, verify_forced_errmsg);
        else
          s = string_sprintf("Cannot check <%s> at this time - please try "
            "later", orig_receiver);

        if (!receiver_try_verify)
          {
          smtp_printf("451 %s\r\n", s);
          break;   /* End of handling the RCPT TO command */
          }

        smtp_printf("250 %s accepted unverified\r\n", s);
        }

      /* Verification succeeded */

      else smtp_printf("250 <%s> verified\r\n", orig_receiver);
      }

    /* Otherwise the receiver address is only known to be syntactically
    acceptable. Any delivery errors will happen later. */

    else
      {
      smtp_printf("250 <%s> is syntactically correct\r\n", orig_receiver);
      }

    /* Phew! All the checks have succeeded. Add to the list of receivers */

    accept_add_recipient(receiver, orcpt, flags, 0);
    break;


    /* The DATA command is legal only if it follows successful MAIL FROM
    and RCPT TO commands. This function is complete when a valid DATA
    command is encountered. */

    case DATA_CMD:
    if (sender_address == NULL)
      {
      smtp_printf("503 MAIL FROM command must precede DATA\r\n");
      break;
      }

    if (rbl_host && rbl_log_rcpt_count)
      log_write(1, LOG_REJECT, "%d RCPT from %s (%d accepted)", rcpt_count,
        host_and_ident("", NULL), recipients_count);

    if (recipients_count <= 0)
      {
      smtp_printf("503 Valid RCPT TO <recipient> must precede DATA\r\n");
      break;
      }

    if (toomany && recipients_max_reject)
      {
      sender_address = NULL;  /* This will allow a new MAIL without RSET */
      sender_address_unrewritten = NULL;
      smtp_printf("554 Too many recipients\r\n");
      break;
      }

    smtp_printf("354 Enter message, ending with \".\" on a line by itself\r\n");
    done = 3;
    message_ended = END_NOTENDED;   /* Indicate in middle of data */
    break;


    /* The VRFY command is enabled by a configuration option. Despite RFC1123
    it defaults disabled. However, discussion in connection with RFC 821bis
    has concluded that the response should be 252 in the disabled state,
    because there are broken clients that try VRFY before RCPT. A 5xx response
    should be given only when the address is positively known to be
    undeliverable. Sigh. */

    case VRFY_CMD:
    if (!smtp_verify)
      {
      smtp_printf("252 VRFY not available\r\n");
      log_write(1, LOG_MAIN|LOG_REJECT, "VRFY rejected%s: %s",
        host_and_ident(" H=", " U="), smtp_data);
      }

    /* When VRFY is enabled, it verifies only addresses that contain no domain
    or one of the local domains. However, we have to let the verify function
    and the routers and directors decide what is local. */

    else if (check_auth_or_tls("VRFY"))  /* Check for AUTH or TLS */
      {
      char *address;
      char *s = NULL;

      rfc821_domains = TRUE;
      address = parse_extract_address(smtp_data, &errmess, &start, &end,
        &receiver_domain, FALSE);
      rfc821_domains = FALSE;

      if (address == NULL)
        s = string_sprintf("501 %s", errmess);
      else
        {
        BOOL is_local_domain;
        int rc = verify_address(address, NULL, &is_local_domain, NULL,
          vopt_is_recipient | vopt_local);

        if (!is_local_domain)
          s = string_sprintf("252 <%s> contains a non-local domain", address);
        else if (rc == OK)
          s = string_sprintf("250 <%s> is deliverable", address);
        else switch(rc)
          {
          case DEFER:
          s = (verify_forced_errmsg != NULL)?
            string_sprintf("451 <%s> %s", address, verify_forced_errmsg) :
            string_sprintf("451 Cannot resolve <%s> at this time", address);
          break;

          case FAIL:
          s = (verify_forced_errmsg != NULL)?
            string_sprintf("550 <%s> %s", address, verify_forced_errmsg) :
            string_sprintf("550 <%s> is not deliverable", address);
          log_write(3, LOG_MAIN, "VRFY failed for %s%s",
            smtp_data, host_and_ident(" H=", " U="));
          break;
          }
        }

      smtp_printf("%s\r\n", s);
      }
    break;

    /* The EXPN command is available only from specified hosts or nets,
    and authentication may be required. */

    case EXPN_CMD:
    if (!verify_check_host(&smtp_expn_hosts, FALSE))
      {
      if (sender_fullhost == NULL)
        smtp_printf("550 EXPN not available\r\n");
      else
        smtp_printf("550 EXPN not available to %s%s\r\n", sender_fullhost,
          host_lookup_msg);
      log_write(1, LOG_MAIN|LOG_REJECT, "EXPN rejected%s%s: %s",
        host_and_ident(" H=", " U="),
        host_lookup_msg,
        smtp_data);
      }
    else if (check_auth_or_tls("EXPN"))
      {
      address_test_mode = TRUE;
      (void) verify_address(smtp_data, smtp_out, NULL, NULL,
        vopt_is_recipient | vopt_local | vopt_expn);
      address_test_mode = FALSE;
      }
    break;


    #ifdef SUPPORT_TLS
    case STARTTLS_CMD:

    /* RFC 2487 is not clear on when this command may be sent, though it
    does state that all information previously obtained from the client
    must be discarded if a TLS session is started. It seems reasonble to
    do an implied RSET when STARTTLS is received. */

    smtp_reset(reset_point);
    rcount = 0;
    toomany = FALSE;

    /* Attempt to start up a TLS session, and if successful, discard all
    knowledge that was obtained previously. One cause for failure is a nested
    STARTTLS, in which case tls_active remains set, but we must still reject
    all incoming commands. */

    if (tls_server_start())
      {
      helo_seen = esmtp= FALSE;
      if (sender_helo_name != NULL)
        {
        store_free(sender_helo_name);
        sender_helo_name = NULL;
        host_build_sender_fullhost();  /* Rebuild */
        set_process_info("handling incoming TLS connection from %s",
          sender_fullhost);
        }
      received_protocol =
        protocols[pnormal] + ((sender_host_address != NULL)? pnlocal : 0);
      sender_host_authenticated = NULL;
      authenticated_id = NULL;
      host_allow_relay_anywhere_set = FALSE;  /* Re-evaluate */
      DEBUG(1) debug_printf("TLS active\n");
      break;     /* Successful STARTTLS */
      }

    /* Failure. Reject everything except QUIT. */

    DEBUG(1) debug_printf("TLS failed to start\n");
    for (;;)
      {
      int cmd = smtp_read_command();
      if (cmd == EOF_CMD || cmd == QUIT_CMD) break;
      smtp_printf("554 Security failure\r\n");
      }
    /* Fall through to do QUIT processing */
    #endif


    case QUIT_CMD:   /* Must follow STARTTLS */
    smtp_printf("221 %s closing connection\r\n", primary_hostname);
    #ifdef SUPPORT_TLS
    tls_close(TRUE);
    #endif
    done = 2;
    if (log_smtp_connections)
      log_write(4, LOG_MAIN, "SMTP connection from %s closed by QUIT",
        sender_host_unknown? sender_ident : sender_fullhost);
    break;


    case RSET_CMD:
    smtp_reset(reset_point);
    rcount = 0;
    toomany = FALSE;
    smtp_printf("250 Reset OK\r\n");
    break;


    case NOOP_CMD:
    smtp_printf("250 OK\r\n");
    break;


    case DEBUG_CMD:
    smtp_printf("500 No way!\r\n");
    log_write(4, LOG_MAIN, "SMTP \"debug\" command received:%s",
      host_and_ident(" H=", " U="));
    break;


    /* Show ETRN/EXPN if any hosts are permitted to use them; if actually
    used, a check will be done for permitted hosts. */

    case HELP_CMD:
    smtp_printf("214-Commands supported:\r\n");
    smtp_printf("214-    HELO EHLO MAIL RCPT DATA%s%s%s\r\n",
      #ifdef HAVE_AUTH
        " AUTH",
      #else
        "",
      #endif
        (smtp_etrn_hosts != NULL)? " ETRN" :"",
        (smtp_expn_hosts != NULL)? " EXPN" :"");
    smtp_printf("214     NOOP QUIT RSET HELP %s\r\n",
      smtp_verify? "VRFY" : "");
    break;


    case EOF_CMD:
    smtp_printf("421 %s lost input connection\r\n", primary_hostname);

    /* Don't log by default unless in the middle of a message, as some mailers
    just drop the call rather than sending QUIT, and it clutters up the logs.
    */

    if (sender_address != NULL || recipients_count > 0)
      log_write(4, LOG_MAIN, "unexpected %s while reading SMTP command "
        "from %s%s",
          sender_host_unknown? "EOF" : "disconnection",
          sender_host_unknown? sender_ident : sender_fullhost,
          smtp_read_error);

    else if (log_smtp_connections)
      log_write(4, LOG_MAIN, "SMTP connection from %s lost%s",
        sender_host_unknown? sender_ident : sender_fullhost,
        smtp_read_error);

    done = 1;
    break;


    case ETRN_CMD:
    if (smtp_etrn_hosts == NULL)
      {
      smtp_printf("500 Command unrecognized\r\n");
      break;
      }

    if (sender_address != NULL)
      {
      smtp_printf("503 ETRN not permitted inside transaction\r\n");
      break;
      }

    /* Check that the current host is permitted to do this */

    if (!verify_check_host(&smtp_etrn_hosts, FALSE))
      {
      log_write(4, LOG_MAIN|LOG_REJECT, "Rejected ETRN %s from %s%s",
        smtp_data, sender_fullhost, host_lookup_msg);
      smtp_printf("458 Administrative prohibition\r\n");
      break;
      }

    /* Check for mandatory authentication or use of TLS */

    if (!check_auth_or_tls("ETRN")) break;

    /* Log the incident */

    log_write(4, LOG_MAIN, "ETRN %s received from %s", smtp_data,
      sender_fullhost);

    /* If a command has been specified for running as a result of ETRN, we
    permit any argument to ETRN. If not, only the # standard form is permitted,
    since that is strictly the only kind of ETRN that can be implemented.
    Advance the pointer past the #. */

    if (smtp_etrn_command == NULL)
      {
      if (smtp_data[0] != '#')
        {
        smtp_printf("501 Syntax error\r\n");
        break;
        }
      else smtp_data++;
      }

    /* If a command has been specified, set it up for execution. Place the
    argument string in the $value expansion variable. */

    if (smtp_etrn_command != NULL)
      {
      char *error;
      BOOL rc;
      etrn_command = smtp_etrn_command;
      deliver_domain = smtp_data;
      rc = transport_set_up_command(&argv, smtp_etrn_command, TRUE, 0, NULL,
        "ETRN processing", &error);
      deliver_domain = NULL;
      if (!rc)
        {
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to set up ETRN command: %s",
          error);
        smtp_printf("458 Internal failure\r\n");
        break;
        }
      }

     /* Else set up to call Exim with the - R option. */

     else
       {
       int i = 0;
       etrn_command = "exim -R";
       argv = argvector;
       argv[i++] = exim_path;
       if (clmacro_count > 0)
         {
         memcpy(argv + i, clmacros, clmacro_count * sizeof(char *));
         i += clmacro_count;
         }
       if (config_changed)
         {
         argv[i++] = "-C";
         argv[i++] = config_filename;
         }
       argv[i++] = "-R";
       argv[i++] = smtp_data;
       argv[i++] = (char *)0;
       }

    /* If we are host-testing, don't actually do anything. Just pretend
    that we spawned a process. */

    if (host_checking)
      {
      HDEBUG(1) debug_printf("ETRN command is: %s\n", etrn_command);
      pid = 1234;
      }
    else
      {
      /* If ETRN queue runs are to be serialized, check the database to
      ensure one isn't already running. */

      if (smtp_etrn_serialize &&
          !transport_check_serialized("etrn-runs", smtp_data))
        {
        smtp_printf("458 Already processing %s\r\n", smtp_data);
        break;
        }

      /* Fork a child process and run the command. We don't want to have to
      wait for the process at any point, so set SIGCHLD to SIG_IGN before
      forking. It should be set that way anyway for external incoming SMTP,
      but we save and restore to be tidy. */

      oldsignal = signal(SIGCHLD, SIG_IGN);

      if ((pid = fork()) == 0)
        {
        int save_errno;

        smtp_input = FALSE;    /* This process is not associated with the */
        fclose(smtp_in);       /* SMTP call any more. */
        fclose(smtp_out);

        execv(argv[0], argv);

        /* If control gets here, execv has failed. Undo any serialization
        and bomb out. */

        save_errno = errno;
        if (smtp_etrn_serialize)
          transport_end_serialized("etrn-runs", smtp_data);
        log_write(0, LOG_MAIN|LOG_PANIC_DIE, "exec of \"%s\" (ETRN) failed: %s",
          etrn_command, strerror(save_errno));
        _exit(EXIT_FAILURE);         /* paranoia */
        }

      signal(SIGCHLD, oldsignal);    /* restore */
      }

    if (pid < 0)
      {
      log_write(0, LOG_MAIN|LOG_PANIC, "fork of process for ETRN failed");
      smtp_printf("458 Unable to fork process\r\n");
      if (smtp_etrn_serialize) transport_end_serialized("etrn-runs", smtp_data);
      }
    else
      {
      smtp_printf("250 OK\r\n");
      }
    break;

    case BADARG_CMD:
    log_syntax_error("unexpected argument data");
    smtp_printf("501 Unexpected argument data in \"%s\"\r\n", cmd_buffer);
    break;


    default:
    log_syntax_error("unrecognized command");
    if (unknown_command_count++ >= 5)
      {
      smtp_printf("500 Too many unrecognized commands\r\n");
      done = 2;
      log_write(0, LOG_MAIN|LOG_REJECT, "SMTP call from %s dropped: too many "
        "unrecognized commands", (sender_host_address == NULL)? sender_ident :
          sender_fullhost);
      }
    else smtp_printf("500 Unrecognized command\r\n");
    break;
    }

  /* This label is used by goto's inside loops that want to break out to
  the end of the command-processing loop. At present, these are all conditional
  on including the AUTH code. */

  #ifdef HAVE_AUTH
  COMMAND_LOOP: continue;
  #endif
  }

/* Reset the signal handlers used in this function. */

signal(SIGALRM, SIG_DFL);
signal(SIGTERM, SIG_DFL);

return done - 2;  /* Convert yield values */
}

/* End of smtp_in.c */
