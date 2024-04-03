/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "cram_md5.h"


/* Options specific to the cram_md5 authentication mechanism. */

optionlist auth_cram_md5_options[] = {
  { "client_name",        opt_stringptr,
      (void *)(offsetof(auth_cram_md5_options_block, client_name)) },
  { "client_secret",      opt_stringptr,
      (void *)(offsetof(auth_cram_md5_options_block, client_secret)) },
  { "server_secret",      opt_stringptr,
      (void *)(offsetof(auth_cram_md5_options_block, server_secret)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int auth_cram_md5_options_count =
  sizeof(auth_cram_md5_options)/sizeof(optionlist);

/* Default private options block for the contidion authentication method. */

auth_cram_md5_options_block auth_cram_md5_option_defaults = {
  NULL,             /* server_secret */
  NULL,             /* client_secret */
  NULL              /* client_name */
};


/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
auth_cram_md5_init(auth_instance *ablock)
{
auth_cram_md5_options_block *ob =
  (auth_cram_md5_options_block *)(ablock->options_block);
if (ob->server_secret != NULL) ablock->server = TRUE;
if (ob->client_secret != NULL)
  {
  ablock->client = TRUE;
  if (ob->client_name == NULL) ob->client_name = primary_hostname;
  }
}



/*************************************************
*      Peform the CRAM-MD5 algorithm             *
*************************************************/

/* The CRAM-MD5 algorithm is described in RFC 2195. It computes

  MD5((secret XOR opad), MD5((secret XOR ipad), challenge))

where secret is padded out to 64 characters (after being reduced to an MD5
digest if longer than 64) and ipad and opad are 64-byte strings of 0x36 and
0x5c respectively, and comma means concatenation.

Arguments:
  secret         the shared secret
  challenge      the challenge text
  digest         16-byte slot to put the answer in

Returns:         nothing
*/

static void
compute_cram_md5(char *secret, char *challenge, uschar *digestptr)
{
md5 base;
int i;
int len = (int)strlen(secret);
uschar isecret[64];
uschar osecret[64];
uschar md5secret[16];

/* If the secret is longer than 64 characters, we compute its MD5 digest
and use that. */

if (len > 64)
  {
  md5_start(&base);
  md5_end(&base, (uschar *)secret, len, md5secret);
  secret = (char *)md5secret;
  len = 16;
  }

/* The key length is now known to be <= 64. Set up the padded and xor'ed
versions. */

memcpy(isecret, secret, len);
memset(isecret+len, 0, 64-len);
memcpy(osecret, secret, len);
memset(osecret+len, 0, 64-len);

for (i = 0; i < 64; i++)
  {
  isecret[i] ^= 0x36;
  osecret[i] ^= 0x5c;
  }

/* Compute the inner MD5 digest */

md5_start(&base);
md5_mid(&base, isecret);
md5_end(&base, (uschar *)challenge, (int)strlen(challenge), md5secret);

/* Compute the outer MD5 digest */

md5_start(&base);
md5_mid(&base, osecret);
md5_end(&base, md5secret, 16, digestptr);
}



/*************************************************
*             Server entry point                 *
*************************************************/

/* For interface, see auths/README */

int
auth_cram_md5_server(auth_instance *ablock, char *data)
{
auth_cram_md5_options_block *ob =
  (auth_cram_md5_options_block *)(ablock->options_block);
char *challenge = string_sprintf("<%d.%d@%s>", getpid(), time(NULL),
  primary_hostname);
char *clear, *secret;
uschar digest[16];
int i, rc, len;

/* If we are running in the test harness, always send the same challenge,
an example string taken from the RFC. */

if (running_in_test_harness)
  challenge = "<1896.697170952@postoffice.reston.mci.net>";

/* No data should have been sent with the AUTH command */

if (*data != 0) return UNEXPECTED;

/* Send the challenge, read the return */

if ((rc = auth_get_data(&data, challenge)) != OK) return rc;
if ((len = auth_b64decode(data, &clear)) < 0) return BAD64;

/* The return consists of a user name, space-separated from the CRAM-MD5
digest, expressed in hex. Extract the user name and put it in $1. Then check
that the remaining length is 32. */

expand_nstring[1] = clear;
while (*clear != 0 && !isspace((uschar)*clear)) clear++;
if (!isspace((uschar)*clear)) return FAIL;
*clear++ = 0;

expand_nlength[1] = clear - expand_nstring[1] - 1;
if (len - expand_nlength[1] - 1 != 32) return FAIL;
expand_nmax = 1;

/* Expand the server_secret string so that it can compute a value dependent on
the user name if necessary. */

secret = expand_string(ob->server_secret);

/* A forced fail implies failure of authentication - i.e. we have no secret for
the given name. */

if (secret == NULL)
  {
  if (expand_string_forcedfail) return FAIL;
  auth_defer_msg = expand_string_message;
  return DEFER;
  }

/* Compute the CRAM-MD5 digest that we should have received from the client. */

compute_cram_md5(secret, challenge, digest);

/* We now have to compare the digest, which is 16 bytes in binary, with the
data received, which is expressed in lower case hex. We checked above that
there were 32 characters of data left. */

for (i = 0; i < 16; i++)
  {
  int a = *clear++;
  int b = *clear++;
  if (((((a >= 'a')? a - 'a' + 10 : a - '0') << 4) +
        ((b >= 'a')? b - 'a' + 10 : b - '0')) != digest[i]) return FAIL;
  }

return OK;
}



/*************************************************
*              Client entry point                *
*************************************************/

/* For interface, see auths/README */

int
auth_cram_md5_client(
  auth_instance *ablock,                 /* authenticator block */
  smtp_inblock *inblock,                 /* input connection */
  smtp_outblock *outblock,               /* output connection */
  int timeout,                           /* command timeout */
  char *buffer,                          /* for reading response */
  int buffsize)                          /* size of buffer */
{
auth_cram_md5_options_block *ob =
  (auth_cram_md5_options_block *)(ablock->options_block);
char *secret = expand_string(ob->client_secret);
char *name = expand_string(ob->client_name);
char *challenge, *p;
int i;
uschar digest[16];

/* If expansion of either the secret or the user name failed, return FORCEFAIL
or ERROR, as approriate. */

if (secret == NULL || name == NULL)
  {
  if (expand_string_forcedfail) return FORCEFAIL;
  string_format(buffer, buffsize, "expansion of \"%s\" failed in "
    "%s authenticator: %s",
    (secret == NULL)? ob->client_secret : ob->client_name,
    ablock->name, expand_string_message);
  return ERROR;
  }

/* Initiate the authentication exchange and read the challenge, which arrives
in base 64. */

if (!smtp_write_command(outblock, FALSE, "AUTH %s\r\n", ablock->public_name))
  return FAIL_SEND;
if (!smtp_read_response(inblock, (uschar *)buffer, buffsize, '3', timeout))
  return FAIL;

if (auth_b64decode(buffer + 4, &challenge) < 0)
  {
  string_format(buffer, buffsize, "bad base 64 string in challenge: %s",
    big_buffer + 4);
  return ERROR;
  }

/* Run the CRAM-MD5 algorithm on the secret and the challenge */

compute_cram_md5(secret, challenge, digest);

/* Create the response from the user name plus the CRAM-MD5 digest */

string_format(big_buffer, big_buffer_size - 36, "%s", name);
p = big_buffer;
while (*p != 0) p++;
*p++ = ' ';

for (i = 0; i < 16; i++)
  {
  sprintf(p, "%02x", digest[i]);
  p += 2;
  }

/* Send the response, in base 64, and check the result. The response is
in big_buffer, but auth_b64encode() returns its result in working store,
so calling smtp_write_command(), which uses big_buffer, is OK. */

buffer[0] = 0;
if (!smtp_write_command(outblock, FALSE, "%s\r\n", auth_b64encode(big_buffer,
  p - big_buffer))) return FAIL_SEND;

return smtp_read_response(inblock, (uschar *)buffer, buffsize, '2', timeout)?
  OK : FAIL;
}

/* End of cram_md5.c */
