/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "plaintext.h"


/* Options specific to the plaintext authentication mechanism. */

optionlist auth_plaintext_options[] = {
  { "client_send",        opt_stringptr,
      (void *)(offsetof(auth_plaintext_options_block, client_send)) },
  { "server_condition",   opt_stringptr,
      (void *)(offsetof(auth_plaintext_options_block, server_condition)) },
  { "server_prompts",     opt_stringptr,
      (void *)(offsetof(auth_plaintext_options_block, server_prompts)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int auth_plaintext_options_count =
  sizeof(auth_plaintext_options)/sizeof(optionlist);

/* Default private options block for the contidion authentication method. */

auth_plaintext_options_block auth_plaintext_option_defaults = {
  NULL,              /* server_condition */
  NULL,              /* server_prompts */
  NULL               /* client_send */
};


/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
auth_plaintext_init(auth_instance *ablock)
{
auth_plaintext_options_block *ob =
  (auth_plaintext_options_block *)(ablock->options_block);
if (ablock->public_name == NULL) ablock->public_name = ablock->name;
if (ob->server_condition != NULL) ablock->server = TRUE;
if (ob->client_send != NULL) ablock->client = TRUE;
}



/*************************************************
*             Server entry point                 *
*************************************************/

/* For interface, see auths/README */

int
auth_plaintext_server(auth_instance *ablock, char *data)
{
auth_plaintext_options_block *ob =
  (auth_plaintext_options_block *)(ablock->options_block);
char *prompts = ob->server_prompts;
char *clear, *cond, *end, *s;
int number = 1;
int len, rc;
int sep = 0;


/* If data was supplied on the AUTH command, decode it, and split it up into
multiple items at binary zeros. If the data consists of the string "=" it
indicates a single, empty string. */

if (*data != 0)
  {
  if (strcmp(data, "=") == 0)
    {
    expand_nstring[++expand_nmax] = "";
    expand_nstring[expand_nmax] = 0;
    }
  else
    {
    if ((len = auth_b64decode(data, &clear)) < 0) return BAD64;
    end = clear + len;
    while (clear < end && expand_nmax < EXPAND_MAXN)
      {
      expand_nstring[++expand_nmax] = clear;
      while (*clear != 0) clear++;
      expand_nlength[expand_nmax] = clear++ - expand_nstring[expand_nmax];
      }
    }
  }

/* Now go through the list of prompt strings. Skip over any whose data has
already been provided as part of the AUTH command. For the rest, send them
out as prompts, and get a data item back. If the data item is "*", abandon the
authentication attempt. Otherwise, split it into items as above. */

while ((s = string_nextinlist(&prompts, &sep, big_buffer, big_buffer_size))
        != NULL && expand_nmax < EXPAND_MAXN)
  {
  if (number++ <= expand_nmax) continue;
  if ((rc = auth_get_data(&data, s)) != OK) return rc;
  if ((len = auth_b64decode(data, &clear)) < 0) return BAD64;
  end = clear + len;
  while (clear < end && expand_nmax < EXPAND_MAXN)
    {
    expand_nstring[++expand_nmax] = clear;
    while (*clear != 0) clear++;
    expand_nlength[expand_nmax] = clear++ - expand_nstring[expand_nmax];
    }
  }

/* We now have a number of items of data in $1, $2, etc. Match against the
decoded data by expanding the condition. Also expand the id to set if
authentication succeeds. */

cond = expand_string(ob->server_condition);

HDEBUG(2)
  {
  int i;
  debug_printf("%s authenticator:\n", ablock->name);
  for (i = 1; i <= expand_nmax; i++)
    debug_printf("  $%d = %.*s\n", i, expand_nlength[i], expand_nstring[i]);
  if (cond == NULL)
    debug_printf("expansion failed: %s\n", expand_string_message);
  else
    debug_printf("expanded string: %s\n", cond);
  }

/* Expansion failure always yields DEFER, which will cause a temporary error
code to be returned to the AUTH command. The problem is at the server end, so
the client should try again later. */

if (cond == NULL)
  {
  auth_defer_msg = expand_string_message;
  return DEFER;
  }

/* Return FAIL for empty string, "0", "no", and "false"; return OK for
"1", "yes", and "true"; return DEFER for anything else, with the string
available as an error text. */

if (*cond == 0 ||
    strcmp(cond, "0") == 0 ||
    strcmpic(cond, "no") == 0 ||
    strcmpic(cond, "false") == 0)
  return FAIL;

if (strcmp(cond, "1") == 0 ||
    strcmpic(cond, "yes") == 0 ||
    strcmpic(cond, "true") == 0)
  return OK;

auth_defer_msg = cond;
return DEFER;
}



/*************************************************
*              Client entry point                *
*************************************************/

/* For interface, see auths/README */

int
auth_plaintext_client(
  auth_instance *ablock,                 /* authenticator block */
  smtp_inblock *inblock,                 /* connection inblock */
  smtp_outblock *outblock,               /* connection outblock */
  int timeout,                           /* command timeout */
  char *buffer,                          /* buffer for reading response */
  int buffsize)                          /* size of buffer */
{
auth_plaintext_options_block *ob =
  (auth_plaintext_options_block *)(ablock->options_block);
char *text = ob->client_send;
char *s;
BOOL first = TRUE;
int sep = 0;

/* The text is broken up into a number of different data items, which are
sent one by one. The first one is sent with the AUTH command; the remainder are
sent in response to subsequent prompts. Each is expanded before being sent. */

while ((s = string_nextinlist(&text, &sep, big_buffer, big_buffer_size)) != NULL)
  {
  int i, len;
  char *ss = expand_string(s);

  /* Forced expansion failure is not an error; authentication is abandoned. On
  all but the first string, we have to abandon the authentication attempt by
  sending a line containing "*". Save the failed expansion string, because it
  is in big_buffer, and that gets used by the sending function. */

  if (ss == NULL)
    {
    char *ssave = string_copy(s);
    if (!first)
      {
      if (smtp_write_command(outblock, FALSE, "*\r\n"))
        (void) smtp_read_response(inblock, US buffer, buffsize, '2', timeout);
      }
    if (expand_string_forcedfail) return FORCEFAIL;
    string_format(buffer, buffsize, "expansion of \"%s\" failed in %s "
      "authenticator: %s", ssave, ablock->name, expand_string_message);
    return ERROR;
    }

  len = (int)strlen(ss);

  /* The character ^ is used as an escape for a binary zero character, which is
  needed for the PLAIN mechanism. It must be doubled if really needed. */

  for (i = 0; i < len; i++)
    {
    if (ss[i] == '^')
      {
      if (ss[i+1] != '^') ss[i] = 0; else
        {
        i++;
        len--;
        memmove(ss + i, ss + i + 1, len - i);
        }
      }
    }

  /* The first string is attached to the AUTH command; others are sent
  unembelished. */

  if (first)
    {
    first = FALSE;
    if (!smtp_write_command(outblock, FALSE, "AUTH %s%s%s\r\n",
         ablock->public_name, (len == 0)? "" : " ", auth_b64encode(ss, len)))
      return FAIL_SEND;
    }
  else
    {
    if (!smtp_write_command(outblock, FALSE, "%s\r\n", auth_b64encode(ss, len)))
      return FAIL_SEND;
    }

  /* If we receive a success response from the server, authentication
  has succeeded. There may be more data to send, but is there any point
  in provoking an error here? */

  if (smtp_read_response(inblock, US buffer, buffsize, '2', timeout)) return OK;

  /* Not a success response. If errno != 0 there is some kind of transmission
  error. Otherwise, check the response code in the buffer. If it starts with
  '3', more data is expected. */

  if (errno != 0 || buffer[0] != '3') return FAIL;

  /* If there is no more data to send, we have to cancel the authentication
  exchange and return ERROR. */

  if (text == NULL)
    {
    if (smtp_write_command(outblock, FALSE, "*\r\n"))
      (void)smtp_read_response(inblock, US buffer, buffsize, '2', timeout);
    string_format(buffer, buffsize, "Too few items in client_send in %s "
      "authenticator", ablock->name);
    return ERROR;
    }
  }

/* Control should never actually get here. */

return FAIL;
}

/* End of plaintext.c */
