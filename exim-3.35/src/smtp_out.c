/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* A number of functions for driving outgoing SMTP calls. */


#include "exim.h"



/*************************************************
*           Connect to remote host               *
*************************************************/

/* Create a socket, and connect it to a remote host. IPv6 addresses are
detected by checking for a colon in the address. AF_INET6 is defined even on
non-IPv6 systems, to enable the code to be less messy. However, on such systems
host->address will always be an IPv4 address.

Arguments:
  host        host item containing name and address
  port        remote port to connect to, in network byte order
  interface   outgoing interface address or NULL
  timeout     timeout value or 0
  keepalive   TRUE to use keepalive

Returns:      connected socket number, or -1 with errno set
*/

int
smtp_connect(host_item *host, int port, char *interface, int timeout,
  BOOL keepalive)
{
struct sockaddr *s_ptr;
struct sockaddr_in s_in4;
int s_len, rc, sock, save_errno;
int host_af = (strchr(host->address, ':') != NULL)? AF_INET6 : AF_INET;

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

HDEBUG(1) debug_printf("Connecting to %s [%s.%d] ... ", host->name,
  host->address, (int)ntohs((unsigned short)port));

/* In the old days of IPv4, failure to create a socket usually meant things
were dire, and so Exim used to panic and die. However, with the arrival of IPv6
there are circumstances where IPv6 sockets fail, but IPv4 ones work, and if a
domain is routed to a mixture of IPv4 and IPv6 addresses, the right thing to do
is to let it try them all. Consequently, this error no longer causes a panic,
but instead gives an error return. */

sock = socket(host_af, SOCK_STREAM, 0);
if (sock < 0)
  {
  int save_errno = errno;
  HDEBUG(1) debug_printf("socket creation failed: %s\n", strerror(errno));
  errno = save_errno;
  return -1;
  }

/* Bind to a specific interface if requested. On an IPv6 system, this has
to be of the same family as the address we are calling. On an IPv4 system the
test is redundant, but it keeps the code tidier. */

if (interface != NULL)
  {
  int interface_af = (strchr(interface, ':') != NULL)? AF_INET6 : AF_INET;

  if (interface_af == host_af)
    {
    #if HAVE_IPV6

    /* Set up for IPv6 binding */

    if (host_af == AF_INET6)
      {
      memset(&s_in6, 0, sizeof(s_in6));
      s_in6.sin6_family = AF_INET6;
      s_in6.sin6_port = 0;
      if (inet_pton(AF_INET6, interface, &s_in6.sin6_addr) != 1)
        log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", interface);
      }
    else
    #endif

    /* Set up for IPv4 binding */

      {
      memset(&s_in4, 0, sizeof(s_in4));
      s_in4.sin_family = AF_INET;
      s_in4.sin_port = 0;
      s_in4.sin_addr.s_addr = (S_ADDR_TYPE)inet_addr(interface);
      }

    /* Bind */

    if (bind(sock, s_ptr, s_len) < 0)
      log_write(0, LOG_PANIC_DIE, "unable to bind outgoing SMTP call to %s: %s",
        interface, strerror(errno));
    }
  }

/* Set up a remote IPv6 address */

#if HAVE_IPV6
if (host_af == AF_INET6)
  {
  memset(&s_in6, 0, sizeof(s_in6));
  s_in6.sin6_family = AF_INET6;
  s_in6.sin6_port = port;
  if (inet_pton(host_af, host->address, &s_in6.sin6_addr) != 1)
    log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", host->address);
  }
else
#endif

/* Set up a remote IPv4 address */

  {
  memset(&s_in4, 0, sizeof(s_in4));
  s_in4.sin_family = AF_INET;
  s_in4.sin_port = port;
  s_in4.sin_addr.s_addr = (S_ADDR_TYPE)inet_addr(host->address);
  }

/* If no connection timeout is set, just call connect() without setting
a timer, thereby allowing the inbuilt timeout to operate. */

sigalrm_seen = FALSE;

if (timeout > 0)
  {
  os_non_restarting_signal(SIGALRM, sigalrm_handler);
  alarm(timeout);
  }

rc = connect(sock, s_ptr, s_len);
save_errno = errno;

if (timeout > 0)
  {
  alarm(0);
  signal(SIGALRM, SIG_IGN);
  }

/* A failure whose error code is "Interrupted system call" is in fact
an externally applied timeout if the signal handler has been run. */

if (rc < 0)
  {
  if (save_errno == EINTR && sigalrm_seen) save_errno = ETIMEDOUT;
  close(sock);
  HDEBUG(1) debug_printf("failed\n");
  errno = save_errno;
  return -1;
  }

HDEBUG(1) debug_printf("connected\n");

/* Add keepalive to the socket if requested, before returning it. */

if (keepalive)
  {
  int fodder = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
      (char *)(&fodder), sizeof(fodder)) != 0)
    log_write(4, LOG_MAIN, "send setsockopt(SO_KEEPALIVE) failed on "
      "connection to %s: %s", host->address, strerror(errno));
  }

return sock;
}


/*************************************************
*        Flush outgoing command buffer           *
*************************************************/

/* This function is called only from smtp_write_command() below. It flushes
the buffer of outgoing commands. There is more than one in the buffer only when
pipelining.

Argument:
  outblock   the SMTP output block

Returns:     TRUE if OK, FALSE on error, with errno set
*/

static BOOL
flush_buffer(smtp_outblock *outblock)
{
int rc;

#ifdef SUPPORT_TLS
if (tls_active == outblock->sock)
  rc = tls_write(outblock->buffer, outblock->ptr - outblock->buffer);
else
#endif

rc = send(outblock->sock, outblock->buffer, outblock->ptr - outblock->buffer, 0);
if (rc <= 0)
  {
  HDEBUG(1) debug_printf("send failed: %s\n", strerror(errno));
  return FALSE;
  }

outblock->ptr = outblock->buffer;
outblock->cmd_count = 0;
return TRUE;
}



/*************************************************
*             Write SMTP command                 *
*************************************************/

/* The formatted command is left in big_buffer so that it can be reflected in
any error message.

Arguments:
  outblock   contains buffer for pipelining, and socket
  noflush    if TRUE, save the command in the output buffer, for pipelining
  format     a format, starting with one of
             of HELO, MAIL FROM, RCPT TO, DATA, ".", or QUIT.
  ...        data for the format

Returns:     0 if command added to pipelining buffer, with nothing transmitted
            +n if n commands transmitted (may still have buffered the new one)
            -1 on error, with errno set
*/

int
smtp_write_command(smtp_outblock *outblock, BOOL noflush, char *format, ...)
{
int count;
int rc = 0;
va_list ap;

va_start(ap, format);
if (!string_vformat(big_buffer, big_buffer_size, format, ap))
  log_write(0, LOG_PANIC_DIE, "overlong write_command in outgoing SMTP");
va_end(ap);
count = (int)strlen(big_buffer);
HDEBUG(1) debug_printf("  SMTP>> %s", big_buffer);

if (count > outblock->buffersize - (outblock->ptr - outblock->buffer))
  {
  rc = outblock->cmd_count;                 /* flush resets */
  if (!flush_buffer(outblock)) return -1;
  }

strncpy(CS outblock->ptr, big_buffer, count);
outblock->ptr += count;
outblock->cmd_count++;
big_buffer[count-2] = 0;     /* remove \r\n for error message */

if (!noflush)
  {
  rc += outblock->cmd_count;                /* flush resets */
  if (!flush_buffer(outblock)) return -1;
  }

return rc;
}



/*************************************************
*          Read one line of SMTP response        *
*************************************************/

/* This function reads one line of SMTP response from the server host. This may
not be a complete response - it could be just part of a multiline response. We
have to use a buffer for incoming packets, because when pipelining or using
LMTP, there may well be more than one response in a single packet. This
function is called only from the one that follows.

Arguments:
  inblock   the SMTP input block (contains holding buffer, socket, etc.)
  buffer    where to put the line
  size      space available for the line
  timeout   the timeout to use when reading a packet

Returns:    length of a line that has been put in the buffer
            -1 otherwise, with errno set
*/

static int
read_response_line(smtp_inblock *inblock, uschar *buffer, int size, int timeout)
{
fd_set select_inset;
struct timeval tv;
uschar *p = buffer;
uschar *ptr = inblock->ptr;
uschar *ptrend = inblock->ptrend;
int sock = inblock->sock;
int rc;

/* Loop for reading multiple packets or reading another packet after emptying
a previously-read one. */

for (;;)
  {
  /* If there is data in the input buffer left over from last time, copy
  characters from it until the end of a line, at which point we can return,
  having removed any whitespace (which will include CR) at the end of the line.
  The rules for SMTP say that lines end in CRLF, but there are have been cases
  of hosts using just LF, and other MTAs are reported to handle this, so we
  just look for LF. If we run out of characters before the end of a line,
  carry on to read the next incoming packet. */

  while (ptr < ptrend)
    {
    int c = *ptr++;
    if (c == '\n')
      {
      while (p > buffer && isspace(p[-1])) p--;
      *p = 0;
      inblock->ptr = ptr;
      return p - buffer;
      }
    *p++ = c;
    if (--size < 4)
      {
      *p = 0;                     /* Leave malformed line for error message */
      errno = ERRNO_SMTPFORMAT;
      return -1;
      }
    }

  /* We need to read a new input packet. Loop to cover select() getting
  interrupted, and the possibility of select() returning with a positive
  result but no ready descriptor. Is this in fact possible? */

  for (;;)
    {
    FD_ZERO (&select_inset);
    FD_SET (sock, &select_inset);
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    rc = select(sock + 1, (SELECT_ARG2_TYPE *)&select_inset, NULL, NULL, &tv);

    /* If some interrupt arrived, just retry. We presume this to be rare,
    but it can happen (e.g. the SIGUSR1 signal sent by exiwhat causes
    select() to exit). */

    if (rc < 0 && errno == EINTR)
      {
      HDEBUG(9) debug_printf("EINTR while selecting for SMTP response\n");
      continue;
      }

    /* Handle a timeout, and treat any other error as a timeout. */

    if (rc <= 0)
      {
      errno = ETIMEDOUT;
      goto ERROR_RETURN;
      }

    /* If the socket is ready, read from it (via TLS if it's active), and break
    out of this select/retry loop. */

    if (FD_ISSET(sock, &select_inset))
      {
      #ifdef SUPPORT_TLS
      if (tls_active == sock)
        rc = tls_read(inblock->buffer, size-1);
      else
      #endif
        rc = recv(sock, inblock->buffer, inblock->buffersize, 0);
      break;
      }
    }

  /* We get here when a read operation has been obeyed. First, handle errors.
  On EOF (i.e. close down of the connection), set errno to zero; otherwise
  leave it alone. */

  if (rc <= 0)
    {
    if (rc == 0) errno = 0;
    goto ERROR_RETURN;
    }

  /* Another block of data has been successfully read. Set up the pointers
  and let the loop continue. */

  ptrend = inblock->ptrend = inblock->buffer + rc;
  ptr = inblock->buffer;
  DEBUG(9) debug_printf("read response data: size=%d\n", rc);
  }

/* Get here if there has been some kind of error; errno is set, but we
ensure that the result buffer is empty before returning. */

ERROR_RETURN:

*buffer = 0;
return -1;
}





/*************************************************
*              Read SMTP response                *
*************************************************/

/* This function reads an SMTP response with a timeout, and returns the
response in the given buffer, as a string. A multiline response will contain
newline characters between the lines. The function also analyzes the first
digit of the reply code and returns FALSE if it is not acceptable. FALSE is
also returned after a reading error. In this case buffer[0] will be zero, and
the error code will be in errno.

Arguments:
  inblock   the SMTP input block (contains holding buffer, socket, etc.)
  buffer    where to put the response
  size      the size of the buffer
  okdigit   the expected first digit of the response
  timeout   the timeout to use

Returns:    TRUE if a valid, non-error response was received; else FALSE
*/

BOOL
smtp_read_response(smtp_inblock *inblock, uschar *buffer, int size, int okdigit,
   int timeout)
{
uschar *ptr = buffer;
int count;

errno = 0;  /* Ensure errno starts out zero */

/* This is a loop to read and concatentate the lines that make up a multi-line
response. */

for (;;)
  {
  if ((count = read_response_line(inblock, ptr, size, timeout)) < 0)
    return FALSE;

  HDEBUG(1)
    debug_printf("  %s %s\n", (ptr == buffer)? "SMTP<<" : "      ", ptr);

  /* Check the format of the response: it must start with three digits; if
  these are followed by a space or end of line, the response is complete. If
  they are followed by '-' this is a multi-line response and we must look for
  another line until the final line is reached. The only use made of multi-line
  responses is to pass them back as error messages. We therefore just
  concatenate them all within the buffer, which should be large enough to
  accept any reasonable number of lines. */

  if (count < 3 ||
     !isdigit(ptr[0]) ||
     !isdigit(ptr[1]) ||
     !isdigit(ptr[2]) ||
     (ptr[3] != '-' && ptr[3] != ' ' && ptr[3] != 0))
    {
    errno = ERRNO_SMTPFORMAT;    /* format error */
    return FALSE;
    }

  /* If the line we have just read is a terminal line, line, we are done.
  Otherwise more data has to be read. */

  if (ptr[3] != '-') break;

  /* Move the reading pointer upwards in the buffer and insert \n between the
  components of a multiline response. Space is left for this by read_response_
  line(). */

  ptr += count;
  *ptr++ = '\n';
  size -= count + 1;
  }

/* Return a value that depends on the SMTP return code. On some systems a
non-zero value of errno has been seen at this point, so ensure it is zero,
because the caller of this function looks at errno when FALSE is returned, to
distinguish between an unexpected return code and other errors such as
timeouts, lost connections, etc. */

errno = 0;
return buffer[0] == okdigit;
}

/* End of smtp_out.c */
