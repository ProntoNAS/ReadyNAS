/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* This module provides TLS (aka SSL) support for Exim. The code is based on a
patch that was contributed by Steve Haslam. It was adapted from stunnel, a GPL
program by Michal Trojnara. No cryptographic code is included in Exim. All this
module does is to call functions from the OpenSSL library. */


#include "exim.h"

/* This module is compiled only when it is specifically requested in the
build-time configuration. However, some compilers don't like compiling empty
modules, so keep them happy with a dummy when skipping the rest. Make it
reference itself to stop picky compilers complaining that it is unused, and put
in a dummy argument to stop even pickier compilers complaining about infinite
loops. */

#ifndef SUPPORT_TLS
static void dummy(int x) { dummy(x-1); }
#else


#include <openssl/lhash.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

/* Structure for collecting random data for seeding. */

typedef struct randstuff {
  time_t t;
  pid_t  p;
} randstuff;

/* Local static variables */

static char ssl_library_info[128];
static char ssl_errstring[256];

static int session_timeout = 200;

static const char *sid_ctx = "exim";
static uschar *ssl_xfer_buffer = NULL;
static int ssl_xfer_buffer_size = 4096;
static int ssl_xfer_buffer_lwm = 0;
static int ssl_xfer_buffer_hwm = 0;
static int ssl_xfer_eof = 0;
static int ssl_xfer_error = 0;

static SSL_CTX *ctx = NULL;
static SSL *ssl = NULL;





/*************************************************
*               Handle TLS error                 *
*************************************************/

/* Called from lots of places when errors occur. Always returns FALSE because
most calls can then use "return tls_error(...)" to do this processing and then
return FALSE. A single function is used for both server and client, because it
is called from some shared functions. In the case of a server, we don't use
this function after responding to the initial STARTTLS, when an SMTP response
is not required.

Argument:
  prefix    text to include in the logged error
  host      NULL if setting up a server;
            the connected host if setting up a client

Returns:    FALSE
*/

static BOOL
tls_error(char *prefix, host_item *host)
{
ERR_error_string(ERR_get_error(), ssl_errstring);

if (host == NULL)
  {
  log_write(0, LOG_MAIN, "TLS error on connection from %s (%s): %s",
    (sender_fullhost != NULL)? sender_fullhost : "local process",
    prefix, ssl_errstring);
  smtp_printf("454 %s currently unavailable\r\n", ssl_library_info);
  }
else
  {
  log_write(0, LOG_MAIN, "TLS error on connection to %s [%s] (%s): %s",
    host->name, host->address, prefix, ssl_errstring);
  }

return FALSE;
}



/*************************************************
*        Callback to generate RSA key            *
*************************************************/

/*
Arguments:
  s          SSL connection
  export     not used
  keylength  keylength

Returns:     pointer to generated key
*/

static RSA *
rsa_callback(SSL *s, int export, int keylength)
{
RSA *rsa_key;
export = export;     /* Shut picky compilers up */
DEBUG(5) debug_printf("Generating %d bit RSA key...\n", keylength);
rsa_key = RSA_generate_key(keylength, RSA_F4, NULL, NULL);
if (rsa_key == NULL)
  {
  ERR_error_string(ERR_get_error(), ssl_errstring);
  log_write(0, LOG_MAIN|LOG_PANIC, "TLS error (RSA_generate_key): %s",
    ssl_errstring);
  return NULL;
  }
return rsa_key;
}





/*************************************************
*        Callback for verification               *
*************************************************/

/* The SSL libraries do some verification of their own - the current yes/no
state is in "state". All this function does is to log a verify error if the
current state is "no", and otherwise to set up the tls_peerdn string.

Arguments:
  state      current yes/no state as 1/0
  x509ctx    certificate information.

Returns:     1 if verified, 0 if not
*/

static int
verify_callback(int state, X509_STORE_CTX *x509ctx)
{
static char txt[256];

X509_NAME_oneline(X509_get_subject_name(x509ctx->current_cert),
  txt, sizeof(txt));

if (state == 0)
  {
  log_write(1, LOG_MAIN, "SSL verify error: depth=%d error=%s cert=%s",
    x509ctx->error_depth,
    X509_verify_cert_error_string(x509ctx->error),
    txt);
  return 0;   /* reject */
  }

if (x509ctx->error_depth != 0)
  {
  DEBUG(1) debug_printf("SSL verify ok: depth=%d cert=%s\n",
     x509ctx->error_depth, txt);
  }
else
  {
  DEBUG(1) debug_printf("SSL authenticated peer: %s\n", txt);
  tls_peerdn = txt;
  }

return 1;   /* accept */
}



/*************************************************
*           Information callback                 *
*************************************************/

/* The SSL library functions call this from time to time to indicate what they
are doing. We copy the string to the debugging output when the level is high
enough.

Arguments:
  s         the SSL connection
  where
  ret

Returns:    nothing
*/

static void
info_callback(SSL *s, int where, int ret)
{
where = where;
ret = ret;
DEBUG(5) debug_printf("SSL info: %s\n", SSL_state_string_long(s));
}





/*************************************************
*       Expand string; give error on failure     *
*************************************************/

/* If expansion is forced to fail, set the result NULL and return TRUE.
Other failures return FALSE. For a server, an SMTP response is given.

Arguments:
  s         the string to expand; if NULL just return TRUE
  name      name of string being expanded (for error)
  result    where to put the result
  isserver  TRUE if setting up a server

Returns:    TRUE if OK; result may still be NULL after forced failure
*/

static BOOL
expand_check(char *s, char *name, char **result, BOOL isserver)
{
if (s == NULL) *result = NULL; else
  {
  *result = expand_string(s);
  if (*result == NULL && !expand_string_forcedfail)
    {
    if (isserver)
      smtp_printf("454 %s currently unavailable\r\n", ssl_library_info);
    log_write(0, LOG_MAIN|LOG_PANIC, "expansion of %s failed: %s", name,
      expand_string_message);
    return FALSE;
    }
  }
return TRUE;
}



/*************************************************
*                Initialize for DH               *
*************************************************/

/* If dhparam is set, expand it, and load up the parameters for DH encryption.

Arguments:
  isserver  TRUE if called from a server
  dhparam   DH parameter file

Returns:    TRUE if OK (nothing to set up, or setup worked)
*/

static BOOL
init_dh(BOOL isserver, char *dhparam)
{
BOOL yield = TRUE;
BIO *bio;
DH *dh;
char *dhexpanded;

if (!expand_check(dhparam, "tls_dhparam", &dhexpanded, isserver))
  return FALSE;

if (dhexpanded == NULL) return TRUE;

if ((bio = BIO_new_file(dhexpanded, "r")) == NULL)
  {
  log_write(0, LOG_MAIN, "DH: could not read %s: %s", dhexpanded,
    strerror(errno));
  yield = FALSE;
  }
else
  {
  if ((dh = PEM_read_bio_DHparams(bio, NULL, NULL, NULL)) == NULL)
    {
    log_write(0, LOG_MAIN, "DH: could not load params from %s",
      dhexpanded);
    yield = FALSE;
    }
  else
    {
    SSL_CTX_set_tmp_dh(ctx, dh);
    DEBUG(5)
      debug_printf("Diffie-Hellman initialised from %s with %d-bit key\n",
        dhexpanded, 8*DH_size(dh));
    DH_free(dh);
    }
  BIO_free(bio);
  }

if (isserver && !yield)
  smtp_printf("454 %s currently unavailable\r\n", ssl_library_info);

return yield;
}




/*************************************************
*       Set up certificate or key                *
*************************************************/

/* Expand a file name for a certificate or a key, and get it set up.

Arguments:
  name        file name to be expanded
  name_txt    file name for error message
  func        setup function to call
  func_txt    name of function for errors
  host        NULL if in a server; the remote host if in a client

Returns:      TRUE if no configured file name,
              or expansion forced failure,
              or setup was successful
*/

static BOOL
setup_certkey(char *name, char *name_txt,
  int (*func)(SSL_CTX *, const char *, int),
  char *func_txt, host_item *host)
{
char *expanded;
if (!expand_check(name, name_txt, &expanded, host == NULL)) return FALSE;
if (expanded != NULL)
  {
  DEBUG(5) debug_printf("using %s %s\n", name_txt, expanded);
  if (!(func)(ctx, expanded, SSL_FILETYPE_PEM))
    return tls_error(func_txt, host);
  }
return TRUE;
}




/*************************************************
*            Initialize for TLS                  *
*************************************************/

/* Called from both server and client code, to do preliminary initialization of
the library.

Arguments:
  host            connected host, if client; NULL if server
  dhparam         DH parameter file
  certificate     certificate file
  privatekey      private key
  addr            address if client; NULL if server (for some randomness)

Returns:          TRUE if OK
*/

static BOOL
tls_init(host_item *host, char *dhparam, char *certificate, char *privatekey,
  address_item *addr)
{
SSL_load_error_strings();          /* basic set up */
OpenSSL_add_ssl_algorithms();

/* This magic code is cribbed from stunnel; unmodified */

if (OPENSSL_VERSION_NUMBER <= 0x0922)
  {
  sprintf(ssl_library_info, "OpenSSL/0.%ld.%ld%c",
    (OPENSSL_VERSION_NUMBER&0x0f00)>>8,
    (OPENSSL_VERSION_NUMBER&0x00f0)>>4,
    'a' + (char)(OPENSSL_VERSION_NUMBER&0x000f));
  }
else
  {
  sprintf(ssl_library_info, "OpenSSL/%ld.%ld.%ld",
    (OPENSSL_VERSION_NUMBER&0xf0000000)>>28,
    (OPENSSL_VERSION_NUMBER&0x0ff00000)>>20,
    (OPENSSL_VERSION_NUMBER&0x000ff000)>>12);

  if (OPENSSL_VERSION_NUMBER&0x100)
    {
    /* release */
    if (OPENSSL_VERSION_NUMBER&0xff)
      {
      char s[2];
      s[0] = 'a' + (OPENSSL_VERSION_NUMBER&0x0ff) - 1;
      s[1] = '\0';
      strcat(ssl_library_info, s);
      }
    }
  else
    {
    if (OPENSSL_VERSION_NUMBER&0xff)
      strcat(ssl_library_info, "beta");
    else
     strcat(ssl_library_info, "-dev");
    }
  }

/* Create a context */

ctx = SSL_CTX_new((host == NULL)?
  SSLv23_server_method() : SSLv23_client_method());

if (ctx == NULL) return tls_error("SSL_CTX_new", host);

/* It turns out that we need to seed the random number generator this early in
order to get the full complement of ciphers to work. It took me roughly a day
of work to discover this by experiment.

On systems that have /dev/urandom, SSL may automatically seed itself from
there. Otherwise, we have to make something up as best we can. Double check
afterwards. */

if (!RAND_status())
  {
  randstuff r;
  r.t = time(NULL);
  r.p = getpid();

  RAND_seed((uschar *)(&r), sizeof(r));
  RAND_seed((uschar *)big_buffer, big_buffer_size);
  if (addr != NULL) RAND_seed((uschar *)addr, sizeof(addr));

  if (!RAND_status())
    {
    if (host == NULL)
      {
      log_write(0, LOG_MAIN, "TLS error on connection from %s: "
        "unable to seed random number generator",
        (sender_fullhost != NULL)? sender_fullhost : "local process");
      smtp_printf("454 %s currently unavailable\r\n", ssl_library_info);
      }
    else
      {
      log_write(0, LOG_MAIN, "TLS error on connection to %s [%s]: "
        "unable to seed random number generator",
        host->name, host->address);
      }
    return FALSE;
    }
  }

/* Set up the information callback, which outputs if debugging is at a suitable
level. */

if (!SSL_CTX_set_info_callback(ctx, info_callback))
  return tls_error("SSL_CTX_set_info_callback", host);

/* Initialize with DH parameters if supplied */

if (!init_dh((host == NULL), dhparam)) return FALSE;

/* Set up keys, certificates, and the RSA callback. */

if (!setup_certkey(certificate, "tls_certificate",
     SSL_CTX_use_certificate_file, "SSL_CTX_use_certificate_file", host))
  return FALSE;

if (!setup_certkey(privatekey, "tls_privatekey",
     SSL_CTX_use_PrivateKey_file, "SSL_CTX_use_PrivateKey_file", host))
  return FALSE;

SSL_CTX_set_tmp_rsa_callback(ctx, rsa_callback);

/* Finally, set the timeout, and we are done */

SSL_CTX_set_timeout(ctx, session_timeout);
DEBUG(1) debug_printf("Initialised TLS (%s)\n", ssl_library_info);
return TRUE;
}




/*************************************************
*           Get name of cipher in use            *
*************************************************/

/* The answer is left in a static buffer, and tls_cipher is set to point
to it.

Argument:   pointer to an SSL structure for the connection
Returns:    nothing
*/

static void
construct_cipher_name(SSL *ssl)
{
static char cipherbuf[256];
SSL_CIPHER *c;
char *ver;
int bits;

switch (ssl->session->ssl_version)
  {
  case SSL2_VERSION:
  ver="SSLv2";
  break;

  case SSL3_VERSION:
  ver="SSLv3";
  break;

  case TLS1_VERSION:
  ver="TLSv1";
  break;

  default:
  ver="UNKNOWN";
  }

c = SSL_get_current_cipher(ssl);
SSL_CIPHER_get_bits(c, &bits);

string_format(cipherbuf, sizeof(cipherbuf), "%s:%s:%u", ver,
  SSL_CIPHER_get_name(c), bits);
tls_cipher = cipherbuf;

DEBUG(1) debug_printf("Cipher: %s\n", cipherbuf);
}





/*************************************************
*        Set up for checking certs/ciphers       *
*************************************************/

/* Called by both client and server startup

Arguments:
  certs         certs file or NULL
  ciphers       cipher list, or NULL
  host          NULL in a server; the remote host in a client

Returns:     TRUE if setup was ok
*/

static BOOL
setup_certcipher(char *certs, char *ciphers, host_item *host)
{
char *expcerts, *expciphers;

/* Set cipher restrictions */

if (!expand_check(ciphers, "tls_verify_ciphers", &expciphers, (host == NULL)))
  return FALSE;

if (expciphers != NULL)
  {
  if (!SSL_CTX_set_cipher_list(ctx, expciphers))
    return tls_error("SSL_CTX_set_cipher_list", host);
  }

/* Set certificate restrictions */

if (!expand_check(certs, "tls_verify_certificates", &expcerts, TRUE))
  return FALSE;

if (expcerts != NULL)
  {
  struct stat statbuf;
  if (!SSL_CTX_set_default_verify_paths(ctx))
    return tls_error("SSL_CTX_set_default_verify_paths", host);
  if (stat(expcerts, &statbuf) < 0)
    {
    log_write(0, LOG_MAIN|LOG_PANIC,
      "failed to stat %s for certificates", expcerts);
    }
  else
    {
    char *file, *dir;
    if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
      { file = NULL; dir = expcerts; }
    else
      { file = expcerts; dir = NULL; }
    if (!SSL_CTX_load_verify_locations(ctx, file, dir))
      return tls_error("SSL_CTX_load_verify_locations", host);
    if (file != NULL)
      SSL_CTX_set_client_CA_list(ctx, SSL_load_client_CA_file(file));
    }

  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
    verify_callback);
  }

return TRUE;
}







/*************************************************
*       Start a TLS session in a server          *
*************************************************/

/* This is called when Exim is running as a server, after having received
the STARTTLS command. It must respond to that command, and then negotiate
a TLS session.

Arguments:  none

Returns:    TRUE on success; FALSE on failure
            Errors before the start of the negotiation are indicated by SMTP
              return codes, output via smtp_printf().
            Errors during the negotation just return FALSE; the server can't
              continue running.
*/

BOOL
tls_server_start(void)
{
int rc;

/* Check for previous activation */

if (tls_active >= 0)
  {
  log_write(0, LOG_MAIN, "multiple STARTTLS statements detected in "
    "connection from %s",
    (sender_fullhost != NULL)? sender_fullhost : "local process");
  smtp_printf("554 Already in TLS\r\n");
  return FALSE;
  }

/* Initialize the SSL library. If it fails, it will already have logged
the error and sent an SMTP response. */

if (!tls_init(NULL, tls_dhparam, tls_certificate, tls_privatekey, NULL))
  return FALSE;

/* If this is a host for which verification is required, set up for that.
Verification of ciphers and certificates are controlled independently. */

if (verify_check_host(&tls_verify_hosts, FALSE) &&
     !setup_certcipher(tls_verify_certificates, tls_verify_ciphers, NULL))
  return FALSE;

/* Prepare for new connection */

if ((ssl = SSL_new(ctx)) == NULL) return tls_error("SSL_new", NULL);
SSL_clear(ssl);

/* Set context and tell client to go ahead. We need to have an explicit
fflush() here, to force out the response. Other smtp_printf() calls do not need
it, because in non-TLS mode, the fflush() happens when smtp_getc() is called. */

SSL_set_session_id_context(ssl, sid_ctx, strlen(sid_ctx));
smtp_printf("220 %s go ahead\r\n", ssl_library_info);
fflush(smtp_out);

/* Now negotiate the TLS session. We put our own timer on it, since it seems
that the OpenSSL library doesn't. */

SSL_set_fd(ssl, fileno(smtp_out));
SSL_set_accept_state(ssl);

DEBUG(9) debug_printf("Calling SSL_accept\n");

sigalrm_seen = FALSE;
os_non_restarting_signal(SIGALRM, sigalrm_handler);
alarm(smtp_receive_timeout);
rc = SSL_accept(ssl);
alarm(0);
signal(SIGALRM, SIG_IGN);

if (rc <= 0)
  {
  if (sigalrm_seen) strcpy(ssl_errstring, "timed out");
    else ERR_error_string(ERR_get_error(), ssl_errstring);
  log_write(0, LOG_MAIN, "TLS error on connection from %s (SSL_accept): %s",
    (sender_fullhost != NULL)? sender_fullhost : "local process",
    ssl_errstring);
  return FALSE;
  }

DEBUG(9) debug_printf("SSL_accept was successful\n");

/* TLS has been set up. Adjust the input functions to read via TLS,
and initialize things. */

construct_cipher_name(ssl);

DEBUG(9)
  {
  char buf[2048];
  if (SSL_get_shared_ciphers(ssl, buf, sizeof(buf)) != NULL)
    debug_printf("Shared ciphers: %s\n", buf);
  }


ssl_xfer_buffer = store_malloc(ssl_xfer_buffer_size);
ssl_xfer_buffer_lwm = ssl_xfer_buffer_hwm = 0;
ssl_xfer_eof = ssl_xfer_error = 0;

accept_getc = tls_getc;
accept_ungetc = tls_ungetc;
accept_feof = tls_feof;
accept_ferror = tls_ferror;

tls_active = fileno(smtp_out);
return TRUE;
}





/*************************************************
*    Start a TLS session in a client             *
*************************************************/

/* Called from the smtp transport after STARTTLS has been accepted.

Argument:
  fd              the fd of the connection
  host            connected host (for messages)
  dhparam         DH parameter file
  certificate     certificate file
  privatekey      private key file
  verify_certs    file for certificate verify
  verify_ciphers  list of allowed ciphers

Returns:   TRUE if OK
*/

BOOL
tls_client_start(int fd, host_item *host, address_item *addr, char *dhparam,
  char *certificate, char *privatekey, char *verify_certs,
  char *verify_ciphers, int timeout)
{
static char txt[256];
X509* server_cert;
int rc;

if (!tls_init(host, dhparam, certificate, privatekey, addr)) return FALSE;

if (!setup_certcipher(verify_certs, verify_ciphers, host)) return FALSE;

if ((ssl = SSL_new(ctx)) == NULL) return tls_error("SSL_new", host);
SSL_set_session_id_context(ssl, sid_ctx, strlen(sid_ctx));
SSL_set_fd(ssl, fd);
SSL_set_connect_state(ssl);

/* There doesn't seem to be a built-in timeout on connection. */

sigalrm_seen = FALSE;
os_non_restarting_signal(SIGALRM, sigalrm_handler);
alarm(timeout);
rc = SSL_connect(ssl);
alarm(0);
signal(SIGALRM, SIG_IGN);

if (rc <= 0)
  {
  if (sigalrm_seen)
    {
    log_write(0, LOG_MAIN, "TLS error on connection to %s [%s]: "
      "SSL_connect timed out", host->name, host->address);
    return FALSE;
    }
  else return tls_error("SSL_connect", host);
  }

server_cert = SSL_get_peer_certificate (ssl);
tls_peerdn = X509_NAME_oneline(X509_get_subject_name(server_cert),
  txt, sizeof(txt));
tls_peerdn = txt;

construct_cipher_name(ssl);   /* Sets tls_cipher */

tls_active = fd;
return TRUE;
}





/*************************************************
*            TLS version of getc                 *
*************************************************/

/* This gets the next byte from the TLS input buffer. If the buffer is empty,
it refills the buffer via the SSL reading function.

Argument:   the FILE for the SMTP input
Returns:    the next character or EOF
*/

int
tls_getc(FILE *f)
{
if (ssl_xfer_buffer_lwm >= ssl_xfer_buffer_hwm)
  {
  int error;
  int inbytes;

  DEBUG(5) debug_printf("Calling SSL_read(%lx, %lx, %u)\n", (long)ssl,
    (long)ssl_xfer_buffer, ssl_xfer_buffer_size);

  alarm(smtp_receive_timeout);
  inbytes = SSL_read(ssl, ssl_xfer_buffer, ssl_xfer_buffer_size);
  error = SSL_get_error(ssl, inbytes);
  alarm(0);

  /* SSL_ERROR_ZERO_RETURN appears to mean that the SSL session has been
  closed down, not that the socket itself has been closed down. Revert to
  non-SSL handling. */

  if (error == SSL_ERROR_ZERO_RETURN)
    {
    DEBUG(5) debug_printf("Got SSL_ERROR_ZERO_RETURN\n");

    accept_getc = smtp_getc;
    accept_ungetc = smtp_ungetc;
    accept_feof = smtp_feof;
    accept_ferror = smtp_ferror;

    SSL_free(ssl);
    ssl = NULL;
    tls_active = -1;
    tls_cipher = NULL;
    tls_peerdn = NULL;

    return smtp_getc(f);
    }

  /* Handle genuine errors */

  else if (error != SSL_ERROR_NONE)
    {
    DEBUG(5) debug_printf("Got SSL error %d\n", error);
    ssl_xfer_error = 1;
    return EOF;
    }

  ssl_xfer_buffer_hwm = inbytes;
  ssl_xfer_buffer_lwm = 0;
  }

/* Something in the buffer; return next char */

return ssl_xfer_buffer[ssl_xfer_buffer_lwm++];
}



/*************************************************
*           TLS version of ungetc                *
*************************************************/

/* Puts a character back in the input buffer. Only ever
called once.

Arguments:
  ch           the character
  f            the SMTP FILE pointer (not used)

Returns:       the character
*/

int
tls_ungetc(int ch, FILE *f)
{
ssl_xfer_buffer[--ssl_xfer_buffer_lwm] = ch;
return ch;
}



/*************************************************
*           TLS version of feof                  *
*************************************************/

/* Tests for a previous EOF

Argument:      the FILE for the SMTP input
Returns:       non-zero if the eof flag is set
*/

int
tls_feof(FILE *f)
{
return ssl_xfer_eof;
}



/*************************************************
*              TLS version of ferror             *
*************************************************/

/* Tests for a previous read error, and returns with errno
restored to what it was when the error was detected.

>>>>> Hmm. Errno not handled yet. Where do we get it from?  >>>>>


Argument:      the FILE for the SMTP input
Returns:       non-zero if the error flag is set
*/

int
tls_ferror(FILE *f)
{
return ssl_xfer_error;
}



/*************************************************
*          Read bytes from TLS channel           *
*************************************************/

/*
Arguments:
  buff      buffer of data
  len       size of buffer

Returns:    the number of bytes read
            -1 after a failed read
*/

int
tls_read(char *buff, size_t len)
{
int inbytes;
int error;

DEBUG(5) debug_printf("Calling SSL_read(%lx, %lx, %u)\n", (long)ssl,
  (long)buff, len);

inbytes = SSL_read(ssl, buff, len);
error = SSL_get_error(ssl, inbytes);

if (error == SSL_ERROR_ZERO_RETURN)
  {
  DEBUG(5) debug_printf("Got SSL_ERROR_ZERO_RETURN\n");
  return -1;
  }
else if (error != SSL_ERROR_NONE)
  {
  return -1;
  }

return inbytes;
}





/*************************************************
*         Write bytes down TLS channel           *
*************************************************/

/*
Arguments:
  buff      buffer of data
  len       number of bytes

Returns:    the number of bytes after a successful write,
            -1 after a failed write
*/

int
tls_write(const char *buff, size_t len)
{
int outbytes;
int error;
int left = len;

DEBUG(5) debug_printf("tls_do_write(%lx, %d)\n", (long)buff, left);
while (left > 0)
  {
  DEBUG(5) debug_printf("SSL_write(SSL, %lx, %d)\n", (long)buff, left);
  outbytes = SSL_write(ssl, buff, left);
  error = SSL_get_error(ssl, outbytes);
  DEBUG(5) debug_printf("outbytes=%d error=%d\n", outbytes, error);
  switch (error)
    {
    case SSL_ERROR_SSL:
    ERR_error_string(ERR_get_error(), ssl_errstring);
    log_write(0, LOG_MAIN, "TLS error (SSL_write): %s", ssl_errstring);
    return -1;
    break;

    case SSL_ERROR_NONE:
    left -= outbytes;
    buff += outbytes;
    break;

    case SSL_ERROR_ZERO_RETURN:
    log_write(0, LOG_MAIN, "SSL channel closed on write");
    return -1;

    default:
    log_write(0, LOG_MAIN, "SSL_write error %d", error);
    return -1;
    }
  }
return len;
}





/*************************************************
*         Close down a TLS session               *
*************************************************/

/* This is also called from within a delilvery subprocess forked from the
daemon, to shut down the TLS library, without actually doing a shutdown (which
would tamper with the SSL session in the parent process).

Arguments:   TRUE if SSL_shutdown is to be called
Returns:     nothing
*/

void
tls_close(BOOL shutdown)
{
if (tls_active < 0) return;  /* TLS was not active */

if (shutdown)
  {
  DEBUG(1) debug_printf("tls_close(): shutting down SSL\n");
  SSL_shutdown(ssl);
  }

SSL_free(ssl);
ssl = NULL;

tls_active = -1;
}

#endif  /* SUPPORT_TLS */

/* End of tls.c */
