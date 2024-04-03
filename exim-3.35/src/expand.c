/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* Functions for handling string expansion. */


#include "exim.h"

#ifdef STAND_ALONE
#ifndef SUPPORT_CRYPTEQ
#define SUPPORT_CRYPTEQ
#endif
#endif

#ifdef SUPPORT_CRYPTEQ
#ifdef CRYPT_H
#include <crypt.h>
#endif
#endif


/* Recursively called function */

static char *expand_string_internal(char *, BOOL, char **, BOOL);



/*************************************************
*            Local statics and tables            *
*************************************************/

/* Type for main variable table */

typedef struct {
  char *name;
  int   type;
  void *value;
} var_entry;

/* Type for entries pointing to address/length pairs. Not currently
in use. */

typedef struct {
  char **address;
  int  *length;
} alblock;

/* Types of table entry */

enum {
  vtype_int,            /* value is address of int */
  vtype_filter_int,     /* ditto, but recognized only when filtering */
  vtype_string,         /* value is address of string */
  vtype_stringptr,      /* value is address of pointer to string */
  vtype_msgbody,        /* as stringptr, but read when first required */
  vtype_msgbody_end,    /* ditto, the end of the message */
  vtype_msgheaders,     /* the message's headers */
  vtype_localpart,      /* extract local part from string */
  vtype_domain,         /* extract domain from string */
  vtype_recipients,     /* extract recipients from recipients list */
                        /* (enabled only during system filtering */
  vtype_todbsdin,       /* value not used; generate BSD inbox tod */
  vtype_todf,           /* value not used; generate full tod */
  vtype_todl,           /* value not used; generate log tod */
  vtype_reply           /* value not used; get reply from headers */
  };

/* This table must be kept in alphabetical order. */

static var_entry var_table[] = {
  { "address_file",        vtype_stringptr,   &address_file },
  { "address_pipe",        vtype_stringptr,   &address_pipe },
  { "authenticated_id",    vtype_stringptr,   &authenticated_id },
  { "authenticated_sender",vtype_stringptr,   &authenticated_sender },
  { "body_linecount",      vtype_int,         &body_linecount },
  { "caller_gid",          vtype_int,         &real_gid },
  { "caller_uid",          vtype_int,         &real_uid },
  { "compile_date",        vtype_stringptr,   &version_date },
  { "compile_number",      vtype_stringptr,   &version_cnumber },
  { "domain",              vtype_stringptr,   &deliver_domain },
  { "domain_data",         vtype_stringptr,   &domain_data },
  { "errmsg_recipient",    vtype_stringptr,   &errmsg_recipient },
  { "home",                vtype_stringptr,   &deliver_home },
  { "host",                vtype_stringptr,   &deliver_host },
  { "host_address",        vtype_stringptr,   &deliver_host_address },
  { "host_lookup_failed",  vtype_int,         &host_lookup_failed },
  { "interface_address",   vtype_stringptr,   &interface_address },
  { "key",                 vtype_stringptr,   &lookup_key },
  { "local_part",          vtype_stringptr,   &deliver_localpart },
  { "local_part_data",     vtype_stringptr,   &local_part_data },
  { "local_part_prefix",   vtype_stringptr,   &deliver_localpart_prefix },
  { "local_part_suffix",   vtype_stringptr,   &deliver_localpart_suffix },
  { "localhost_number",    vtype_int,         &host_number },
  { "message_age",         vtype_int,         &message_age },
  { "message_body",        vtype_msgbody,     &message_body },
  { "message_body_end",    vtype_msgbody_end, &message_body_end },
  { "message_body_size",   vtype_int,         &message_body_size },
  { "message_headers",     vtype_msgheaders,  NULL },
  { "message_id",          vtype_stringptr,   &message_id },
  { "message_precedence",  vtype_stringptr,   &message_precedence },
  { "message_size",        vtype_int,         &message_size },
  { "n0",                  vtype_filter_int,  &filter_n[0] },
  { "n1",                  vtype_filter_int,  &filter_n[1] },
  { "n2",                  vtype_filter_int,  &filter_n[2] },
  { "n3",                  vtype_filter_int,  &filter_n[3] },
  { "n4",                  vtype_filter_int,  &filter_n[4] },
  { "n5",                  vtype_filter_int,  &filter_n[5] },
  { "n6",                  vtype_filter_int,  &filter_n[6] },
  { "n7",                  vtype_filter_int,  &filter_n[7] },
  { "n8",                  vtype_filter_int,  &filter_n[8] },
  { "n9",                  vtype_filter_int,  &filter_n[9] },
  { "original_domain",     vtype_stringptr,   &deliver_domain_orig },
  { "original_local_part", vtype_stringptr,   &deliver_localpart_orig },
  { "originator_gid",      vtype_int,         &originator_gid },
  { "originator_uid",      vtype_int,         &originator_uid },
  { "parent_domain",       vtype_stringptr,   &deliver_domain_parent },
  { "parent_local_part",   vtype_stringptr,   &deliver_localpart_parent },
  { "primary_hostname",    vtype_stringptr,   &primary_hostname },
  { "prohibition_reason",  vtype_stringptr,   &prohibition_reason },
  { "qualify_domain",      vtype_stringptr,   &qualify_domain_sender },
  { "qualify_recipient",   vtype_stringptr,   &qualify_domain_recipient },
  { "rbl_domain",          vtype_stringptr,   &rbl_domain },
  { "rbl_text",            vtype_stringptr,   &rbl_msg_buffer },
  { "received_for",        vtype_stringptr,   &received_for },
  { "received_protocol",   vtype_stringptr,   &received_protocol },
  { "recipients",          vtype_recipients,  NULL },
  { "recipients_count",    vtype_int,         &recipients_count },
  { "reply_address",       vtype_reply,       NULL },
  { "return_path",         vtype_stringptr,   &return_path },
  { "return_size_limit",   vtype_int,         &return_size_limit },
  { "route_option",        vtype_stringptr,   &route_option },
  { "self_hostname",       vtype_stringptr,   &self_hostname },
  { "sender_address",      vtype_stringptr,   &sender_address },
  { "sender_address_domain", vtype_domain,    &sender_address },
  { "sender_address_local_part", vtype_localpart, &sender_address },
  { "sender_fullhost",     vtype_stringptr,   &sender_fullhost },
  { "sender_helo_name",    vtype_stringptr,   &sender_helo_name },
  { "sender_host_address", vtype_stringptr,   &sender_host_address },
  { "sender_host_authenticated",vtype_stringptr, &sender_host_authenticated },
  { "sender_host_name",    vtype_stringptr,   &sender_host_name },
  { "sender_host_port",    vtype_int,         &sender_host_port },
  { "sender_ident",        vtype_stringptr,   &sender_ident },
  { "sender_rcvhost",      vtype_stringptr,   &sender_rcvhost },
  { "sn0",                 vtype_filter_int,  &filter_sn[0] },
  { "sn1",                 vtype_filter_int,  &filter_sn[1] },
  { "sn2",                 vtype_filter_int,  &filter_sn[2] },
  { "sn3",                 vtype_filter_int,  &filter_sn[3] },
  { "sn4",                 vtype_filter_int,  &filter_sn[4] },
  { "sn5",                 vtype_filter_int,  &filter_sn[5] },
  { "sn6",                 vtype_filter_int,  &filter_sn[6] },
  { "sn7",                 vtype_filter_int,  &filter_sn[7] },
  { "sn8",                 vtype_filter_int,  &filter_sn[8] },
  { "sn9",                 vtype_filter_int,  &filter_sn[9] },
  { "spool_directory",     vtype_stringptr,   &spool_directory },
  { "thisaddress",         vtype_stringptr,   &filter_thisaddress },
#ifdef SUPPORT_TLS
  { "tls_cipher",          vtype_stringptr,   &tls_cipher },
  { "tls_peerdn",          vtype_stringptr,   &tls_peerdn },
#endif
  { "tod_bsdinbox",        vtype_todbsdin,    NULL },
  { "tod_full",            vtype_todf,        NULL },
  { "tod_log",             vtype_todl,        NULL },
  { "value",               vtype_stringptr,   &lookup_value },
  { "version_number",      vtype_stringptr,   &version_string },
  { "warnmsg_delay",       vtype_stringptr,   &warnmsg_delay },
  { "warnmsg_recipient",   vtype_stringptr,   &warnmsg_recipients },
  { "warnmsg_recipients",  vtype_stringptr,   &warnmsg_recipients }
};

static int var_table_size = sizeof(var_table)/sizeof(var_entry);
static char var_buffer[256];
static BOOL malformed_header;

/* For textual hashes */

static char *hashcodes = "abcdefghijklmnopqrtsuvwxyz"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "0123456789";

/* For numeric hashes */

static unsigned int prime[] = {
  2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
 31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
 73,  79,  83,  89,  97, 101, 103, 107, 109, 113};



/*************************************************
*          Check a condition string              *
*************************************************/

/* This function is called to expand a string, and test the result for a "true"
or "false" value. Failure of the expansion is panic-worthy, unless forced or
the result of a lookup defer. All store used by the function can be released on
exit.

Arguments:
  condition     the condition string
  m1            text to be incorporated in panic error
  m2            ditto

Returns:        TRUE if condition is met, FALSE if not
*/

BOOL
expand_check_condition(char *condition, char *m1, char *m2)
{
int rc;
void *reset_point = store_get(0);
char *ss = expand_string(condition);
if (ss == NULL && !expand_string_forcedfail && !search_find_defer)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "failed to expand condition\"%s\" "
    "for %s %s: %s", condition, m1, m2, expand_string_message);
if (ss == NULL) return FALSE;
rc = ss[0] != 0 && strcmp(ss, "0") != 0 && strcmpic(ss, "no") != 0 &&
  strcmpic(ss, "false") != 0;
store_reset(reset_point);
return rc;
}



/*************************************************
*             Pick out a name from a string      *
*************************************************/

/* If the name is too long, it is silently truncated.

Arguments:
  name      points to a buffer into which to put the name
  max       is the length of the buffer
  s         points to the first alphabetic character of the name
  extras    chars other than alphanumerics to permit

Returns:    pointer to the first character after the name

Note: The test for *s != 0 in the while loop is necessary because
strchr() yields non-NULL if the character is zero (which is not something
I expected). */

static char *
read_name(char *name, int max, char *s, char *extras)
{
int ptr = 0;
while (*s != 0 && (isalnum((uschar)*s) || strchr(extras, *s) != NULL))
  {
  if (ptr < max-1) name[ptr++] = *s;
  s++;
  }
name[ptr] = 0;
return s;
}



/*************************************************
*     Pick out the rest of a header name         *
*************************************************/

/* A variable name starting $header_ (or just $h_ for those who like
abbreviations) might not be the complete header name because headers can
contain any printing characters in their names, except ':'. This function is
called to read the rest of the name, chop h[eader]_ off the front, and put ':'
on the end, if the name was terminated by white space.

Arguments:
  name      points to a buffer in which the name read so far exists
  max       is the length of the buffer
  s         points to the first character after the name so far, i.e. the
            first non-alphameric character after $header_xxxxx

Returns:    a pointer to the first character after the header name
*/

static char *
read_header_name(char *name, int max, char *s)
{
int prelen = strchr(name, '_') - name + 1;
int ptr = strlen(name) - prelen;
if (ptr > 0) memmove(name, name+prelen, ptr);
while (mac_isgraph(*s) && *s != ':')
  {
  if (ptr < max-1) name[ptr++] = *s;
  s++;
  }
if (*s == ':') s++;
name[ptr++] = ':';
name[ptr] = 0;
return s;
}



/*************************************************
*           Pick out a number from a string      *
*************************************************/

/* Arguments:
  n     points to an integer into which to put the number
  s     points to the first digit of the number

Returns:  a pointer to the character after the last digit
*/

static char *
read_number(int *n, char *s)
{
*n = 0;
while (isdigit((uschar)*s)) *n = *n * 10 + (*s++ - '0');
return s;
}



/*************************************************
*        Extract keyed subfield from a string    *
*************************************************/

/* This function is also used by the search routines when given a multi-level
key to search for. The yield is in dynamic store; NULL means that the key
was not found.

Arguments:
  key       points to the name of the subkey
  s         points to the string from which to extract the subfield

Returns:    NULL if the subfield was not found, or
            a pointer to the subfield's data
*/

char *
expand_getkeyed(char *key, char *s)
{
int length = (int)strlen(key);
while (isspace((uschar)*s)) s++;

/* Loop to search for the key */

while (*s != 0)
  {
  int subkeylength, datalength;
  char *data;
  char *subkey = s;

  while (*s != 0 && *s != '=' && !isspace((uschar)*s)) s++;
  subkeylength = s - subkey;

  while (isspace((uschar)*s)) s++;
  if (*s == '=') while (isspace((uschar)(*(++s))));

  /* For now, just find the end of the data field - interpret quoted
  string later if it is actually wanted. */

  data = s;
  if (*s == '\"')
    {
    while (*(++s) != 0 && *s != '\"')
      {
      if (*s == '\\' && s[1] != 0) s++;
      }
    if (*s == '\"') s++;
    }
  else while (*s != 0 && !isspace((uschar)*s)) s++;
  datalength = s - data;

  /* If keys match, set up the subfield as the yield and return. If
  the value is quoted, interpret the string (which cannot be longer than
  the original). */

  if (length == subkeylength && strncmp(key, subkey, length) == 0)
    {
    char *yield = store_get(datalength + 1);

    if (*data == '\"')
      {
      int ch, i;
      for (i = 0;;)
        {
        ch = *(++data);
        if (ch == 0 || ch == '\"') break;
        if (ch == '\\') ch = string_interpret_escape(&data);
        yield[i++] = ch;
        }
      yield[i] = 0;
      }

    /* Not a quoted string */
    else
      {
      strncpy(yield, data, datalength);
      yield[datalength] = 0;
      }
    return yield;
    }

  /* Move on to next subkey */

  while (isspace((uschar)*s)) s++;
  }

return NULL;
}




/*************************************************
*   Extract numbered subfield from string        *
*************************************************/

/* Extracts a numbered field from a string that is divided by tokens - for
example a line from /etc/passwd is divided by colon characters.  First field is
numbered one.  Returns NULL if the field number is < 0, or if there are
insufficient tokens in the string

***WARNING***
Modifies final argument - this is a dynamically generated string, so that's OK.

Arguments:
  field       number of field to be extracted,
                first field = 1, whole string = 0
  separators  characters that are used to break string into tokens
  s           points to the string from which to extract the subfield

Returns:      NULL if the field was not found,
              a pointer to the field's data inside s (modified to add 0)
*/

static char *
expand_gettokened (int field, char *separators, char *s)
{
int sep = 1;
char *fieldtext = NULL;

/* Invalid field number returns NULL; zero field number returns the whole
subject string. */

if (field < 0) return NULL;
if (field == 0) return s;

/* Search for the required field; create it in place. */

while (field-- > 0)
  {
  size_t len;
  if (sep == 0) return NULL;   /* Previous field was end of string */
  fieldtext = s;
  len = strcspn(s, separators);
  sep = s[len];
  s[len] = 0;
  s += len + 1;
  }

return fieldtext;
}




/*************************************************
*               Find value of a variable         *
*************************************************/

/* The table of variables is kept in alphabetic order, so we
can search it using a binary chop. The "choplen" variable is
nothing to do with the binary chop. It can be set non-zero to
cause chars at the end of the returned string to be disregarded.
It should already be zero on entry.

Arguments:
  name        the name of the variable being sought
  choplen     a pointer to an int which is set if characters at the end
              of the returned data are to be ignored (typically '\n' at the
              end of header lines); it should normally be set zero before
              calling this function

Returns:      NULL if the variable does not exist, or
              a pointer to the variable's contents
*/

static char *
find_variable(char *name, int *choplen)
{
int first = 0;
int last = var_table_size;

while (last > first)
  {
  header_line *h;
  char *s, *domain;
  char **ss;
  int middle = (first + last)/2;
  int c = strcmp(name, var_table[middle].name);

  if (c == 0) switch (var_table[middle].type)
    {
    case vtype_filter_int:
    if (!filter_running) return NULL;
    /* Fall through */

    case vtype_int:
    sprintf(var_buffer, "%d", *(int *)(var_table[middle].value)); /* Integer */
    return var_buffer;

    case vtype_string:                         /* String */
    return (char *)(var_table[middle].value);

    case vtype_stringptr:                      /* Pointer to string */
    s = *((char **)(var_table[middle].value));
    return (s == NULL)? "" : s;

    case vtype_localpart:                      /* Get local part from address */
    s = *((char **)(var_table[middle].value));
    if (s == NULL) return "";
    domain = strrchr(s, '@');
    if (domain == NULL) return s;
    if (domain - s > sizeof(var_buffer) - 1)
      log_write(0, LOG_PANIC_DIE, "local part longer than %d in string "
        "expansion", sizeof(var_buffer));
    strncpy(var_buffer, s, domain - s);
    var_buffer[domain - s] = 0;
    return var_buffer;

    case vtype_domain:                         /* Get domain from address */
    s = *((char **)(var_table[middle].value));
    if (s == NULL) return "";
    domain = strrchr(s, '@');
    return (domain == NULL)? "" : domain + 1;

    case vtype_msgheaders:
      {
      int ptr = 0;
      int size = 100;
      header_line *h;
      s = store_get(size);
      for (h = header_list; h != NULL && size < 64 * 1024; h = h->next)
        {
        if (h->type == '*') continue;
        s = string_cat(s, &size, &ptr, h->text, h->slen);
        }
      s[ptr] = 0;
      }
    return s;

    case vtype_msgbody:                        /* Pointer to msgbody string */
    case vtype_msgbody_end:                    /* Ditto, the end of the msg */
    ss = (char **)(var_table[middle].value);
    if (*ss == NULL && deliver_datafile >= 0)  /* Read body when needed */
      {
      char *body;
      int start_offset = DATA_START_OFFSET;
      int len = message_body_visible;
      if (len > message_size) len = message_size;
      *ss = body = store_malloc(len+1);
      body[0] = 0;
      if (var_table[middle].type == vtype_msgbody_end)
        {
        struct stat statbuf;
        if (fstat(deliver_datafile, &statbuf) == 0)
          {
          start_offset = statbuf.st_size - len;
          if (start_offset < DATA_START_OFFSET)
            start_offset = DATA_START_OFFSET;
          }
        }
      lseek(deliver_datafile, start_offset, SEEK_SET);
      len = read(deliver_datafile, body, len);
      if (len >= 0) body[len] = 0;
      while (*body != 0)
        {
        if (*body == '\n') *body = ' ';
        body++;
        }
      }
    return (*ss == NULL)? "" : *ss;

    case vtype_todbsdin:                       /* BSD inbox time of day */
    return tod_stamp(tod_bsdin);

    case vtype_todf:                           /* Full time of day */
    return tod_stamp(tod_full);

    case vtype_todl:                           /* Log format time of day */
    return tod_stamp(tod_log);

    case vtype_reply:                          /* Get reply address */
    s = NULL;
    for (h = header_list; h != NULL; h = h->next)
      {
      if (h->type == htype_replyto) s = strchr(h->text, ':') + 1;
      if (h->type == htype_from && s == NULL) s = strchr(h->text, ':') + 1;
      }
    if (s == NULL) return "";
    while (isspace((uschar)*s)) s++;

    /* Disregard final \n in header (but note that isspace() might already
    have skipped over it if the header is empty). */

    if (choplen != NULL && *s != 0) *choplen = 1;
    return s;

    /* A recipients list is available only during system message filtering
    or if there is only one recipient, but not elsewhere. */

    case vtype_recipients:

    if (!enable_dollar_recipients) return NULL; else
      {
      int size = 128;
      int ptr = 0;
      int i;
      s = store_get(size);
      for (i = 0; i < recipients_count; i++)
        {
        if (i != 0) s = string_cat(s, &size, &ptr, ", ", 2);
        s = string_cat(s, &size, &ptr, recipients_list[i].address,
          (int)strlen(recipients_list[i].address));
        }
      s[ptr] = 0;                             /* string_cat leaves room */
      }
    return s;
    }

  else if (c > 0) first = middle + 1;
  else last = middle;
  }

return NULL;
}




/*************************************************
*          Find the value of a header            *
*************************************************/

/*
Arguments:
  name       the name of the header, without the leading $header_ or $h_

Returns:     NULL if the header does not exist, else
             a pointer to the header's contents; truncated to 16K if there
             lots that got concatenated
*/

static char *
find_header(char *name)
{
int len = (int)strlen(name);
int ptr = -1;
int size = 0;
char *yield = NULL;
header_line *h;

for (h = header_list; h != NULL && size < 64 * 1024; h = h->next)
  {
  if (h->type != htype_old && h->text != NULL)  /* NULL => Received: placeholder */
    {
    if (len <= h->slen && strncmpic(name, h->text, len) == 0)
      {
      /* If this is the first relevant header, just point to its data. Often
      there is only one, so this is efficient. */

      if (yield == NULL)
        {
        yield = h->text + len;
        while (isspace((uschar)*yield)) yield++;
        }

      /* If this is not the first header, see if it's the second, and in that
      case start off an extensible string. Otherwise add to the string using
      string_cat(), which tries to extend the block rather than copying if
      it can. */

      else if (ptr < 0)
        {
        char *newyield;
        size = (int)strlen(yield) + h->slen + 100;
        newyield = store_get(size);
        strcpy(newyield, yield);
        strcat(newyield, h->text + len);
        yield = newyield;
        ptr = (int)strlen(yield);
        yield[ptr] = 0;
        }

      /* Third or subsequent header */

      else
        {
        yield = string_cat(yield, &size, &ptr, h->text +len, h->slen - len);
        yield[ptr] = 0;
        }
      }
    }
  }
return yield;
}




/*************************************************
*        Read and evaluate a condition           *
*************************************************/

/*
Arguments:
  s        points to the start of the condition text
  yield    points to a BOOL to hold the result of the condition test;
           if NULL, we are just reading through a condition that is
           part of an "or" combination to check syntax, or in a state
           where the answer isn't required

Returns:   a pointer to the first character after the condition, or
           NULL after an error
*/

static char *
eval_condition(char *s, BOOL *yield)
{
BOOL testfor = TRUE;
char name[256];

for (;;)
  {
  while (isspace((uschar)*s)) s++;
  if (*s == '!') { testfor = !testfor; s++; } else break;
  }

/* Numeric comparisons are symbolic */

if (*s == '=' || *s == '>' || *s == '<')
  {
  int p = 0;
  name[p++] = *s++;
  if (*s == '=')
    {
    name[p++] = '=';
    s++;
    }
  name[p] = 0;
  }

/* All other conditions are named */

else s = read_name(name, 256, s, "_");

/* If we haven't read a name, it means some non-alpha character is first. */

if (name[0] == 0)
  {
  expand_string_message = string_sprintf("condition name expected, "
    "but found \"%.16s\"", s);
  return NULL;
  }

/* def: tests for a non-zero or non-NULL variable, or for an existing
header */

if (strcmp(name, "def") == 0 && *s == ':')
  {
  char *value;

  s = read_name(name, 256, s+1, "_");

  /* Test for a header's existence */

  if (strncmp(name, "header_", 7) == 0 || strncmp(name, "h_", 2) == 0)
    {
    s = read_header_name(name, 256, s);
    value = find_header(name);
    if (yield != NULL) *yield = (value != NULL) == testfor;
    }

  /* Test for a variable's having a non-empty value. */

  else
    {
    value = find_variable(name, NULL);
    if (value == NULL)
      {
      expand_string_message = (name[0] == 0)?
        string_sprintf("variable name omitted after \"def:\"") :
        string_sprintf("unknown variable \"%s\" after \"def:\"", name);
      return NULL;
      }
    if (yield != NULL)
      *yield = (value[0] != 0 && strcmp(value, "0") != 0) == testfor;
    }

  return s;
  }

/* first_delivery tests for first delivery attempt */

else if (strcmp(name, "first_delivery") == 0)
  {
  if (yield != NULL) *yield = deliver_firsttime == testfor;
  return s;
  }

/* queue_running tests for any process started by a queue runner */

else if (strcmp(name, "queue_running") == 0)
  {
  if (yield != NULL) *yield = (queue_run_pid != (pid_t)0) == testfor;
  return s;
  }

/* exists: tests for file existence
      pam: does PAM authentication
*/

else if (strcmp(name, "exists") == 0
        #ifdef SUPPORT_PAM
        || strcmp(name, "pam") == 0
        #endif /* SUPPORT_PAM */
        )
  {
  char *sub;
  struct stat statbuf;
  while (isspace((uschar)*s)) s++;
  if (*s != '{') goto COND_FAILED_CURLY_START;
  sub = expand_string_internal(s+1, TRUE, &s, yield == NULL);
  if (sub == NULL) return NULL;
  if (*s++ != '}') goto COND_FAILED_CURLY_END;
  if (yield != NULL)
    {
    #ifdef SUPPORT_PAM
    if (name[0] == 'p')
      {
      int rc = auth_call_pam(sub, &expand_string_message);
      if (rc == ERROR) return NULL;
      *yield = (rc == OK) == testfor;
      }
    else
    #endif /* SUPPORT_PAM */
      {
      if (expand_forbid_exists)
        {
        expand_string_message = "File existence tests are not permitted";
        return NULL;
        }
      *yield = (stat(sub, &statbuf) == 0) == testfor;
      }
    }
  return s;
  }

/* eq:   tests for string equality
match:   does a regular expression match and sets up the numerical
           variables if it succeeds
crypteq: encrypts plaintext and compares against an encrypted text, using
           crypt() as is done for passwords
symbolic operators for numeric testing
*/


else if (strcmp(name, "eq") == 0 ||
         strcmp(name, "match") == 0 ||
         strcmp(name, "crypteq") == 0 ||
         !isalpha((uschar)name[0]))
  {
  int i;
  int roffset;
  int num[2];
  pcre *re;
  const char *rerror;
  char *sub[2];

  for (i = 0; i < 2; i++)
    {
    while (isspace((uschar)*s)) s++;
    if (*s != '{')
      {
      if (i == 0) goto COND_FAILED_CURLY_START;
      expand_string_message = string_sprintf("missing 2nd string in {} "
        "after \"%s\"", name);
      return NULL;
      }
    sub[i] = expand_string_internal(s+1, TRUE, &s, yield == NULL);
    if (sub[i] == NULL) return NULL;
    if (*s++ != '}') goto COND_FAILED_CURLY_END;

    /* Convert to numerical if required */

    if (!isalpha((uschar)name[0]))
      {
      char *endptr;
      num[i] = (int)strtol((const char *)sub[i], &endptr, 10);
      if (tolower(*endptr) == 'k')
        {
        num[i] *= 1024;
        endptr++;
        }
      else if (tolower(*endptr) == 'm')
        {
        num[i] *= 1024*1024;
        endptr++;
        }
      if (*endptr != 0)
        {
        expand_string_message = string_sprintf("\"%s\" is not a number",
          sub[i]);
        return NULL;
        }
      }
    }

  /* Result not required */

  if (yield == NULL) return s;

  /* Do an appropriate comparison */

  switch(name[0])
    {
    case '=':
    *yield = (num[0] == num[1]) == testfor;
    break;

    case '>':
    *yield = ((name[1] == '=')? (num[0] >= num[1]) : (num[0] > num[1]))
      == testfor;
    break;

    case '<':
    *yield = ((name[1] == '=')? (num[0] <= num[1]) : (num[0] < num[1]))
      == testfor;
    break;

    case 'e':   /* Straight text comparison */
    *yield = (strcmp(sub[0], sub[1]) == 0) == testfor;
    break;

    case 'm':   /* Regular expression match */
    re = pcre_compile(sub[1], PCRE_COPT, &rerror, &roffset, NULL);
    if (re == NULL)
      {
      expand_string_message = string_sprintf("regular expression error in "
        "\"%s\": %s at offset %d", sub[1], rerror, roffset);
      return NULL;
      }
    *yield = regex_match_and_setup(re, sub[0], 0, -1) == testfor;
    break;

    /* Various "encrypted" comparisons. If the second string starts with
    "{" then an encryption type is given. Default to crypt(). */

    case 'c':
    #ifdef SUPPORT_CRYPTEQ
    if (strncmp(sub[1], "{md5}", 5) == 0)
      {
      md5 base;
      uschar digest[16];
      md5_start(&base);
      md5_end(&base, (uschar *)sub[0], (int)strlen(sub[0]), digest);
      *yield = (strcmp(auth_b64encode((char *)digest, 16), sub[1]+5) == 0)
        == testfor;
      }
    else   /* {crypt} and non-{ start use crypt(). */
      {
      if (strncmp(sub[1], "{crypt}", 7) == 0) sub[1] += 7;
      else if (sub[1][0] == '{')
        {
        expand_string_message = string_sprintf("unknown encryption mechanism "
          "in \"%s\"", sub[1]);
        return NULL;
        }

      /* If the encrypted string contains fewer than two characters (for the
      salt), force failure. Otherwise we get false positives: with an empty
      string the yield of crypt() is an empty string! */

      *yield = (strlen(sub[1]) < 2)? !testfor :
        (strcmp(crypt(CS sub[0], CS sub[1]), sub[1]) == 0) == testfor;
      }
    break;
    #else
    expand_string_message = "support for \"crypteq\" not compiled";
    return NULL;
    #endif
    }

  return s;
  }

/* and/or: computes logical and/or of several conditions */

else if (strcmp(name, "or") == 0 || strcmp(name, "and") == 0)
  {
  BOOL temp;
  BOOL *ptr = (yield == NULL)? NULL : &temp;
  BOOL and = strcmp(name, "and") == 0;
  int comb = and;

  while (isspace((uschar)*s)) s++;
  if (*s++ != '{') goto COND_FAILED_CURLY_START;

  for (;;)
    {
    while (isspace((uschar)*s)) s++;
    if (*s == '}') break;
    if (*s != '{')
      {
      expand_string_message = string_sprintf("subcondition in {} expected "
        "inside \"%s{...}\" condition", name);
      return NULL;
      }
    s = eval_condition(s+1, ptr);
    if (s == NULL) return NULL;
    while (isspace((uschar)*s)) s++;
    if (*s++ != '}')
      {
      expand_string_message = string_sprintf("missing } at end of condition "
        "inside \"%s\" group", name);
      return NULL;
      }
    if (yield != NULL)
      {
      if (and)
        {
        comb &= temp;
        if (!comb) ptr = NULL;  /* once false, don't evaluate any more */
        }
      else
        {
        comb |= temp;
        if (comb) ptr = NULL;   /* once true, don't evaluate any more */
        }
      }
    }

  if (yield != NULL) *yield = (comb == testfor);
  return ++s;
  }

/* Unknown type of condition */

expand_string_message = string_sprintf("unknown condition \"%s\"", name);
return NULL;

/* Missing braces at start and end of data */

COND_FAILED_CURLY_START:
expand_string_message = string_sprintf("missing { after \"%s\"", name);
return NULL;

COND_FAILED_CURLY_END:
expand_string_message = string_sprintf("missing } at end of \"%s\" condition",
  name);
return NULL;
}




/*************************************************
*            Read and expand n substrings        *
*************************************************/

/* This function is called to read and expand argument substrings for various
expansion items.

Arguments:
  sub        points to vector of pointers to set
  n          number of substrings
  sptr       points to current string pointer
  skipping   the skipping flag

Returns:     0 OK; string pointer updated
             1 curly bracketing error
             2 other error
*/

static int
read_subs(char **sub, int n, char **sptr, BOOL skipping)
{
int i;
char *s = *sptr;

while (isspace((uschar)*s)) s++;
for (i = 0; i < n; i++)
  {
  if (*s != '{') return 1;
  sub[i] = expand_string_internal(s+1, TRUE, &s, skipping);
  if (sub[i] == NULL) return 2;
  while (isspace((uschar)*s)) s++;
  if (*s++ != '}') return 1;
  while (isspace((uschar)*s)) s++;
  }
if (*s++ != '}') return 1;

*sptr = s;
return 0;
}




/*************************************************
*          Save numerical variables              *
*************************************************/

/* This function is called from items such as "if" that want to preserve and
restore the numbered variables.

Arguments:
  save_expand_string    points to an array of pointers to set
  save_expand_nlength   points to an array of ints for the lengths

Returns:                the value of expand max to save
*/

static int
save_expand_strings(char **save_expand_nstring, int *save_expand_nlength)
{
int i;
for (i = 0; i <= expand_nmax; i++)
  {
  save_expand_nstring[i] = expand_nstring[i];
  save_expand_nlength[i] = expand_nlength[i];
  }
return expand_nmax;
}



/*************************************************
*           Restore numerical variables          *
*************************************************/

/* This function restored saved values of numerical strings.

Arguments:
  save_expand_nmax      the number of strings to restore
  save_expand_string    points to an array of pointers
  save_expand_nlength   points to an array of ints

Returns:                nothing
*/

static void
restore_expand_strings(int save_expand_nmax, char **save_expand_nstring,
  int *save_expand_nlength)
{
int i;
expand_nmax = save_expand_nmax;
for (i = 0; i <= expand_nmax; i++)
  {
  expand_nstring[i] = save_expand_nstring[i];
  expand_nlength[i] = save_expand_nlength[i];
  }
}





/*************************************************
*            Handle yes/no substrings            *
*************************************************/

/* This function is used by ${if}, ${lookup} and ${extract} to handle the
alternative substrings that depend on whether or not the condition was true,
or the lookup or extraction succeeded. The substrings always have to be
expanded, to check their syntax, but "skipping" is set when the result is not
needed - this avoids unnecessary nested lookups.

Arguments:
  skipping       TRUE if we were skipping when this item was reached
  yes            TRUE if the first string is to be used, else use the second
  errmsg         NULL, or an additional message for failure
  save_lookup    a value to put back into lookup_value before the 2nd expansion
  sptr           points to the input string pointer
  yieldptr       points to the output string pointer
  sizeptr        points to the output string size
  ptrptr         points to the output string pointer
  type           "lookup" or "if" or "extract", for error message

Returns:         0 OK; lookup_value has been reset to save_lookup
                 1 expansion failed
                 2 expansion failed because of bracketing error
*/

static int
process_yesno(BOOL skipping, BOOL yes, char *errmsg, char *save_lookup, char
  **sptr, char **yieldptr, int *sizeptr, int *ptrptr, char *type)
{
int rc = 0;
char *s = *sptr;    /* Local value */
char *sub1, *sub2;

/* If there are no following strings, we substitute the contents of $value for
lookups and for extractions in the success case. In the fail case, nothing is
substituted. In the case of "if", lack of following strings is an error. */

while (isspace((uschar)*s)) s++;
if (*s == '}')
  {
  if (type[0] == 'i') goto FAILED_CURLY;
  if (yes)
    *yieldptr = string_cat(*yieldptr, sizeptr, ptrptr, lookup_value,
      (int)strlen(lookup_value));
  lookup_value = save_lookup;
  s++;
  goto RETURN;
  }

/* Expand the first substring. Forced failures are noticed only if we actually
want this string. Set skipping in the call in the fail case (this will always
be the case if we were already skipping). */

sub1 = expand_string_internal(s+1, TRUE, &s, !yes);
if (sub1 == NULL && (yes || !expand_string_forcedfail)) goto FAILED;
expand_string_forcedfail = FALSE;
if (*s++ != '}') goto FAILED_CURLY;

/* If we want the first string, add it to the output */

if (yes)
  *yieldptr = string_cat(*yieldptr, sizeptr, ptrptr, sub1, (int)strlen(sub1));

/* If this is called from a lookup or an extract, we want to restore $value to
what it was at the start of the item, so that it has this value during the
second string expansion. For the call from "if" to this function, save_lookup
is set to lookup_value, so that this statement does nothing. */

lookup_value = save_lookup;

/* There now follows either another substring, or "fail", or nothing. This
time, forced failures are noticed only if we want the second string. We must
set skipping in the nested call if we don't want this string, or if we were
already skipping. */

while (isspace((uschar)*s)) s++;
if (*s == '{')
  {
  sub2 = expand_string_internal(s+1, TRUE, &s, yes || skipping);
  if (sub2 == NULL && (!yes || !expand_string_forcedfail)) goto FAILED;
  expand_string_forcedfail = FALSE;
  if (*s++ != '}') goto FAILED_CURLY;

  /* If we want the second string, add it to the output */

  if (!yes)
    *yieldptr = string_cat(*yieldptr, sizeptr, ptrptr, sub2, (int)strlen(sub2));
  }

/* If there is no second string, but the word "fail" is present when the use of
the second string is wanted, set a flag indicating it was a forced failure
rather than a syntactic error. Swallow the terminating } in case this is nested
inside another lookup or if or extract. */

else if (*s != '}')
  {
  char name[256];
  s = read_name(name, sizeof(name), s, "_");
  if (strcmp(name, "fail") == 0)
    {
    if (!yes)
      {
      while (isspace((uschar)*s)) s++;
      if (*s++ != '}') goto FAILED_CURLY;
      expand_string_message =
        string_sprintf("\"%s\" failed and \"fail\" requested%s%s", type,
          (errmsg == NULL || errmsg[0] == 0)? "" : ": ",
          (errmsg == NULL)? "" : errmsg);
      expand_string_forcedfail = TRUE;
      goto FAILED;
      }
    }
  else
    {
    expand_string_message =
      string_sprintf("syntax error in \"%s\" item - \"fail\" expected", type);
    goto FAILED;
    }
  }

/* All we have to do now is to check on the final closing brace. */

while (isspace((uschar)*s)) s++;
if (*s++ == '}') goto RETURN;

/* Get here if there is a bracketing failure */

FAILED_CURLY:
rc++;

/* Get here for other failures */

FAILED:
rc++;

/* Update the input pointer value before returning */

RETURN:
*sptr = s;
return rc;
}






/*************************************************
*                 Expand string                  *
*************************************************/

/* Returns either an unchanged string, or the expanded string in stacking pool
store. Interpreted sequences are:

   \...                    normal escaping rules
   $name                   substitutes the variable
   ${name}                 ditto
   ${op:string}            operates on the string value
   ${extract {key} {string}}
                           extracts keyed substring; null if not found
   ${extract {field} {separator} {string}}
                           extracts numbered field from separated string;
                           null if not found; field 0 is the whole string

   Either form of extract may now have two additional arguments; these are
   used in the success/failure cases, as for ${if} and ${lookup}.

   ${if cond {s1} {s2}}    conditional; the yielded string is expanded
                           {s2} can be replaced by "fail" or be omitted
   ${perl{sub}{a1}...      call Perl (if configured)
   ${sg {source}{s1}{s2}}  global substitution
   ${tr {source}{s1}{s2}}  translate characters

One-key+file lookups:
   ${lookup{key}search-type{file}{found}{not-found}}
                           the key, file, & strings are expanded; $value
                           is available in {found}, and {not-found} can be
                           replaced by "fail" or be omitted.
                           The key can in fact consist of mainkey:subkey, in
                           which case a subfield is extracted from the found
                           string, which must consist of key=value pairs.

Database-query lookups:
   ${lookup search-type{query}{found}{not-found}}
                           the query and strings are expanded; $value available
                           and {not-found} can be replaced by "fail" or be
                           omitted.

Operators:
   domain                  extracts domain from an address
   escape                  escapes non-printing characters
   expand                  expands the string one more time
   hash_<n>_<m>            hash the string, making one that is of length n,
                             using m (default 26) characters from hashcodes
   nhash_<n>_<m>           hash the string into one or two numerical values,
                           using a div/mod scheme to produce a string of the
                           form "a/b" where 0 <= a < n-1 and 0 <= b <= m-1.
                           If <m> is omitted, just give one number.
   md5                     yields the md5 hash of the argument
   lc                      lowercase the string
   uc                      uppercase the string
   length_<n>              take first n chars only
   l_<n>                   ditto
   local_part              extracts local part from an address
   mask                    masks an IP address
   quote                   quote the argument if it contains anything other
                             than letters, digits, underscores, dots, & hyphens;
                             quoting means putting inside "" and \-quoting any
                             \ or " in the string
   quote_xxx               quote the argument by calling the quoting function
                             of the xxx lookup
   rxquote                 regular expression quote: any non-alphameric is
                             quoted with \
   substr_<m>_<n>          substring n chars from offset m
   s_<m>_<n>               ditto
                             negative offset works from the rhs
                             omitted length => rest (either to left or right)

Conditions:
   !cond                   negates condition
   def:variable            variable is defined and not empty
   def:$h_xxx              header xxx exists
   exists {string}         file exists
   match {string}{re}      regular expression match
   eq {string1}{string2}   strings are equal, case included
   crypteq{s1}{s2}         encrypt s1, compare with s2
                           s2 may be of the form {crypt}text or {md5}text
   pam {data}              PAM authentication (if configured in)
   == {num1}{num2}         numbers are equal
   > >= < <=               similar numeric comparisons
   or {{cond1}{cond2}...}  as it says
   and {{cond1}{cond2}...} ditto

We use an internal routine recursively to handle embedded substrings. The
external function follows. The yield is NULL if the expansion failed, and there
are two cases: if something collapsed syntactically, or if "fail" was given
as the action on a lookup failure. These can be distinguised by looking at the
variable expand_string_forcedfail, which is TRUE in the latter case.

The skipping flag is set true when expanding a substring that isn't actually
going to be used (after "if" or "lookup") and it prevents lookups from
happening lower down.

Store usage: At start, a store block of the length of the input plus 64
is obtained. This is expanded as necessary by string_cat(), which might have to
get a new block, or might be able to expand the original. At the end of the
function we can release any store above that portion of the yield block that
was actually used. In many cases this will be optimal.

Arguments:
  string         the string to be expanded
  ket_ends       true if expansion is to stop at }
  left           if not NULL, a pointer to the first character after the
                 expansion is placed here (typically used with ket_ends)
  skipping       TRUE for recursive calls when the value isn't actually going
                 to be used (to allow for optimisation)

Returns:         NULL if expansion fails:
                   expand_string_forcedfail is set TRUE if failure was forced
                   expand_string_message contains a textual error message
                 a pointer to the expanded string on success
*/

static char *
expand_string_internal(char *string, BOOL ket_ends, char **left, BOOL skipping)
{
int ptr = 0;
int size = (int)strlen(string) + 64;
char *s = string;
char *yield = store_get(size);
char *save_expand_nstring[EXPAND_MAXN+1];
int save_expand_nlength[EXPAND_MAXN+1];

expand_string_forcedfail = FALSE;
expand_string_message = "";

while (*s != 0)
  {
  char *value;
  char name[256];

  /* \ escapes the next character, which must exist, or else
  the expansion fails. */

  if (*s == '\\')
    {
    char ch[1];
    if (s[1] == 0)
      {
      expand_string_message = "\\ at end of string";
      goto EXPAND_FAILED;
      }
    ch[0] = string_interpret_escape(&s);
    s++;
    yield = string_cat(yield, &size, &ptr, ch, 1);
    continue;
    }

  /* Anything other than $ is just copied verbatim, unless we are
  looking for a terminating } character. */

  if (ket_ends && *s == '}') break;

  if (*s != '$')
    {
    yield = string_cat(yield, &size, &ptr, s++, 1);
    continue;
    }

  /* No { after the $ - must be a plain name or a number for string
  match variable. There has to be a fudge for variables that are the
  names of header fields preceded by "$header_" because header field
  names can contain any printing characters except space and colon.
  For those that don't like typing this much, "$h_" is a synonym for
  "$header_". A non-existent header yields a NULL value; nothing is
  inserted. */

  if (isalpha((uschar)(*(++s))))
    {
    int choplen = 0;
    s = read_name(name, sizeof(name), s, "_");
    if (strncmp(name, "header_", 7) == 0 || strncmp(name, "h_", 2) == 0)
      {
      s = read_header_name(name, sizeof(name), s);
      value = find_header(name);
      choplen = 1;

      /* If we didn't find the header, and the header contains a closing brace
      characters, this may be a user error where the terminating colon
      has been omitted. Set a flag to adjust the error message. */

      if (value == NULL && strchr(name, '}') != NULL) malformed_header = TRUE;
      }
    else
      {
      value = find_variable(name, &choplen);
      if (value == NULL)
        {
        expand_string_message =
          string_sprintf("unknown variable name \"%s\"", name);
        goto EXPAND_FAILED;
        }
      }
    if (value != NULL)
      {
      int len = (int)strlen(value) - choplen;
      if (len < 0) len = 0;
      yield = string_cat(yield, &size, &ptr, value, len);
      }
    continue;
    }

  if (isdigit((uschar)*s))
    {
    int n;
    s = read_number(&n, s);
    if (n >= 0 && n <= expand_nmax)
      yield = string_cat(yield, &size, &ptr, expand_nstring[n],
        expand_nlength[n]);
    continue;
    }

  /* Otherwise, if there's no '{' after $ it's an error. */

  if (*s != '{')
    {
    expand_string_message = "$ not followed by letter, digit, or {";
    goto EXPAND_FAILED;
    }

  /* After { there can be various things, but they all start with
  an initial word, except for a number for a string match variable. */

  if (isdigit((uschar)(*(++s))))
    {
    int n;
    s = read_number(&n, s);
    if (*s++ != '}')
      {
      expand_string_message = "} expected after number";
      goto EXPAND_FAILED;
      }
    if (n >= 0 && n <= expand_nmax)
      yield = string_cat(yield, &size, &ptr, expand_nstring[n],
        expand_nlength[n]);
    continue;
    }

  if (!isalpha((uschar)*s))
    {
    expand_string_message = "letter or digit expected after ${";
    goto EXPAND_FAILED;
    }

  /* Allow "-" in names to cater for substrings with negative
  arguments. Since we are checking for known names after { this is
  OK. */

  s = read_name(name, sizeof(name), s, "_-");

  /* Handle conditionals - preserve the values of the numerical expansion
  variables in case they get changed by a regular expression match in the
  condition. If not, they retain their external settings. At the end
  of this "if" section, they get restored to their previous values. */

  if (strcmp(name, "if") == 0)
    {
    BOOL cond = FALSE;
    int save_expand_nmax =
      save_expand_strings(save_expand_nstring, save_expand_nlength);

    s = eval_condition(s, skipping? NULL : &cond);
    if (s == NULL) goto EXPAND_FAILED;  /* message already set */

    /* The handling of "yes" and "no" result strings is now in a separate
    function that is also used by ${lookup} and ${extract}. */

    switch(process_yesno(
             skipping,                     /* were previously skipping */
             cond,                         /* success/failure indicator */
             NULL,                         /* message for failure */
             lookup_value,                 /* value to reset for string2 */
             &s,                           /* input pointer */
             &yield,                       /* output pointer */
             &size,                        /* output size */
             &ptr,                         /* output current point */
             "if"))                        /* condition type */
      {
      case 1: goto EXPAND_FAILED;          /* when all is well, the */
      case 2: goto EXPAND_FAILED_CURLY;    /* returned value is 0 */
      }

    /* Restore external setting of expansion variables for continuation
    at this level. */

    restore_expand_strings(save_expand_nmax, save_expand_nstring,
      save_expand_nlength);
    continue;
    }

  /* Handle database lookups unless locked out. If "skipping" is TRUE, we are
  expanding an internal string that isn't actually going to be used. All we
  need to do is check the syntax, so don't do a lookup at all. Preserve the
  values of the numerical expansion variables in case they get changed by a
  partial lookup. If not, they retain their external settings. At the end
  of this "lookup" section, they get restored to their previous values. */

  if (strcmp(name, "lookup") == 0)
    {
    int stype;
    int pv = -1;
    int expand_setup = 0;
    char *ss, *key, *filename;
    char *lookup_errmsg = NULL;
    char *save_lookup_value = lookup_value;
    int save_expand_nmax =
      save_expand_strings(save_expand_nstring, save_expand_nlength);

    if (expand_forbid_lookup)
      {
      expand_string_message = "File lookups are not permitted";
      goto EXPAND_FAILED;
      }

    /* Get the key we are to look up for single-key+file style lookups.
    Otherwise set the key NULL pro-tem. */

    while (isspace((uschar)*s)) s++;
    if (*s == '{')
      {
      key = expand_string_internal(s+1, TRUE, &s, skipping);
      if (key == NULL) goto EXPAND_FAILED;
      if (*s++ != '}') goto EXPAND_FAILED_CURLY;
      while (isspace((uschar)*s)) s++;
      }
    else key = NULL;

    /* Find out the type of database */

    if (!isalpha((uschar)*s))
      {
      expand_string_message = "missing lookup type";
      goto EXPAND_FAILED;
      }
    s = read_name(name, sizeof(name), s, "-_");
    ss = name;
    while (isspace((uschar)*s)) s++;

    /* Handle request for partial lookup */

    if (strncmp(ss, "partial", 7) == 0)
      {
      ss += 7;
      if (isdigit ((uschar)*ss))
        {
        pv = 0;
        while (isdigit((uschar)*ss)) pv = pv*10 + *ss++ - '0';
        }
      else pv = 2;
      if (*ss++ != '-')
        {
        expand_string_message = string_sprintf("unknown lookup type \"%s\"",
          name);
        goto EXPAND_FAILED;
        }
      }

    /* If a single-key lookup type is followed by "*" it requests looking
    for a default "*" entry if all else fails. If it is followed by "*@" it
    requests replacing everying before @ by * first. These are independent of
    partial matching, but are encoded by adding 1024 and 2048 to the partial
    match value. */

    if (strncmp(s, "*@", 2) == 0)
      {
      s += 2;
      while (isspace((uschar)*s)) s++;
      pv += 1024 + 2048;
      }
    else if (*s == '*')
      {
      s++;
      while (isspace((uschar)*s)) s++;
      pv += 1024;
      }

    /* Now check for the individual search type. Only those that are actually
    in the binary are valid. */

    stype = search_findtype(ss, &expand_string_message);
    if (stype < 0) goto EXPAND_FAILED;

    /* Check that a key was provided for those lookup types that need it,
    and was not supplied for those that use the query style, and that
    "partial" was provided only for a non-query lookup. */

    if (!mac_islookup(stype, lookup_querystyle))
      {
      if (key == NULL)
        {
        expand_string_message = string_sprintf("missing {key} for single-"
          "key \"%s\" lookup", name);
        goto EXPAND_FAILED;
        }
      }
    else
      {
      if (pv >= 0)
        {
        expand_string_message = string_sprintf("\"partial\" is not permitted "
          "for lookup type \"%s\"", ss);
        goto EXPAND_FAILED;
        }

      if (key != NULL)
        {
        expand_string_message = string_sprintf("a single key was given for "
          "lookup type \"%s\", which is not a single-key lookup type", name);
        goto EXPAND_FAILED;
        }
      }

    /* Get the next string in brackets and expand it. It is the file name for
    single-key+file lookups, and the whole query otherwise. */

    if (*s != '{') goto EXPAND_FAILED_CURLY;
    filename = expand_string_internal(s+1, TRUE, &s, skipping);
    if (filename == NULL) goto EXPAND_FAILED;
    if (*s++ != '}') goto EXPAND_FAILED_CURLY;
    while (isspace((uschar)*s)) s++;

    /* If this isn't a single-key+file lookup, re-arrange the variables
    to be appropriate for the search_ functions. */

    if (key == NULL)
      {
      key = filename;
      filename = NULL;
      }

    /* If skipping, don't do the next bit - just lookup_value == NULL, as if
    the entry was not found. Note that there is no search_close() function.
    Files are left open in case of re-use. At suitable places in higher logic,
    search_tidyup() is called to tidy all open files. This can save opening
    the same file several times. However, files may also get closed when others
    are opened, if too many are open at once. The rule is that a handle should
    not be used after a second search_open().

    Request that a partial search sets up $1 and maybe $2 by passing
    expand_setup containing zero. If its value changes, reset expand_nmax,
    since new variables will have been set. Note that at the end of this
    "lookup" section, the old numeric variables are restored. */

    if (skipping)
      lookup_value = NULL;
    else
      {
      void *handle = search_open(filename, stype, 0, NULL, NULL,
        &expand_string_message);
      if (handle == NULL) goto EXPAND_FAILED;
      lookup_value = search_find(handle, filename, key, pv, &expand_setup,
        &lookup_errmsg);
      if (search_find_defer)
        {
        expand_string_message =
          string_sprintf("lookup of \"%s\" gave DEFER: %s", key, lookup_errmsg);
        goto EXPAND_FAILED;
        }
      if (expand_setup > 0) expand_nmax = expand_setup;
      }

    /* The handling of "yes" and "no" result strings is now in a separate
    function that is also used by ${if} and ${extract}. */

    switch(process_yesno(
             skipping,                     /* were previously skipping */
             lookup_value != NULL,         /* success/failure indicator */
             lookup_errmsg,                /* message for failure */
             save_lookup_value,            /* value to reset for string2 */
             &s,                           /* input pointer */
             &yield,                       /* output pointer */
             &size,                        /* output size */
             &ptr,                         /* output current point */
             "lookup"))                    /* condition type */
      {
      case 1: goto EXPAND_FAILED;          /* when all is well, the */
      case 2: goto EXPAND_FAILED_CURLY;    /* returned value is 0 */
      }

    /* Restore external setting of expansion variables for carrying on
    at this level, and continue. */

    restore_expand_strings(save_expand_nmax, save_expand_nstring,
      save_expand_nlength);
    continue;
    }

  /* If Perl support is configured, handle calling embedded perl subroutines,
  unless locked out at this time. Syntax is ${perl{sub}} or ${perl{sub}{arg}}
  or ${perl{sub}{arg1}{arg2}} or up to a maximum of EXIM_PERL_MAX_ARGS
  arguments (defined below). */

  #ifdef EXIM_PERL
  #define EXIM_PERL_MAX_ARGS 8

  if (strcmp(name, "perl") == 0)
    {
    int i = 0;
    char *sub_name;
    char *sub_arg[EXIM_PERL_MAX_ARGS + 1];
    char *new_yield;

    if (expand_forbid_perl)
      {
      expand_string_message = "Perl calls are not permitted";
      goto EXPAND_FAILED;
      }

    while (isspace((uschar)*s)) s++;
    if (*s != '{') goto EXPAND_FAILED_CURLY;
    sub_name = expand_string_internal(s+1, TRUE, &s, skipping);
    if (sub_name == NULL) goto EXPAND_FAILED;
    while (isspace((uschar)*s)) s++;
    if (*s++ != '}') goto EXPAND_FAILED_CURLY;

    while (isspace((uschar)*s)) s++;

    while (*s == '{')
      {
      if (i == EXIM_PERL_MAX_ARGS)
        {
        expand_string_message =
          string_sprintf("Too many arguments passed to Perl subroutine \"%s\" "
            "(max is %d)", sub_name, EXIM_PERL_MAX_ARGS);
        goto EXPAND_FAILED;
        }
      sub_arg[i] = expand_string_internal(s+1, TRUE, &s, skipping);
      if (sub_arg[i++] == NULL) goto EXPAND_FAILED;
      while (isspace((uschar)*s)) s++;
      if (*s++ != '}') goto EXPAND_FAILED_CURLY;
      while (isspace((uschar)*s)) s++;
      }

    if (*s++ != '}') goto EXPAND_FAILED_CURLY;
    sub_arg[i] = 0;

    /* If skipping, we don't actually do anything */

    if (skipping) continue;

    /* Start the interpreter if necessary */

    if (!opt_perl_started)
      {
      char *initerror;
      if (opt_perl_startup == NULL)
        {
        expand_string_message = "A setting of perl_startup is needed when "
          "using the Perl interpreter";
        goto EXPAND_FAILED;
        }
      DEBUG(9) debug_printf("Starting Perl interpreter\n");
      initerror = init_perl(opt_perl_startup);
      if (initerror != NULL)
        {
        expand_string_message =
          string_sprintf("error in perl_startup code: %s\n", initerror);
        goto EXPAND_FAILED;
        }
      opt_perl_started = TRUE;
      }

    /* Call the function */

    new_yield = call_perl_cat(yield, &size, &ptr, &expand_string_message,
      sub_name, sub_arg);

    /* NULL yield indicates failure; if the message pointer has been set to
    NULL, the yield was undef, indicating a forced failure. Otherwise the
    message will indicate some kind of Perl error. */

    if (new_yield == NULL)
      {
      if (expand_string_message == NULL)
        {
        expand_string_message =
          string_sprintf("Perl subroutine \"%s\" returned undef to force "
            "failure", sub_name);
        expand_string_forcedfail = TRUE;
        }
      goto EXPAND_FAILED;
      }

    /* Yield succeeded. Ensure forcedfail is unset, just in case it got
    set during a callback from Perl. */

    expand_string_forcedfail = FALSE;
    yield = new_yield;
    continue;
    }
  #endif /* EXIM_PERL */

  /* Handle character translation for "tr" */

  if (strcmp(name, "tr") == 0)
    {
    int oldptr = ptr;
    int o2m;
    char *sub[3];

    switch(read_subs(sub, 3, &s, skipping))
      {
      case 1: goto EXPAND_FAILED_CURLY;
      case 2: goto EXPAND_FAILED;
      }

    yield = string_cat(yield, &size, &ptr, sub[0], (int)strlen(sub[0]));
    o2m = (int)strlen(sub[2]) - 1;

    if (o2m >= 0) for (; oldptr < ptr; oldptr++)
      {
      char *m = strrchr(sub[1], yield[oldptr]);
      if (m != NULL)
        {
        int o = m - sub[1];
        yield[oldptr] = sub[2][(o < o2m)? o : o2m];
        }
      }

    continue;
    }

  /* Handle global substitution for "sg" - like Perl's s/xxx/yyy/g operator.
  We have to save the numerical variables and restore them afterwards. */

  if (strcmp(name, "sg") == 0)
    {
    pcre *re;
    int moffset, moffsetextra, slen;
    int roffset;
    int emptyopt;
    const char *rerror;
    char *subject;
    char *sub[3];
    int save_expand_nmax =
      save_expand_strings(save_expand_nstring, save_expand_nlength);

    switch(read_subs(sub, 3, &s, skipping))
      {
      case 1: goto EXPAND_FAILED_CURLY;
      case 2: goto EXPAND_FAILED;
      }

    /* Compile the regular expression */

    re = pcre_compile(sub[1], PCRE_COPT, &rerror, &roffset, NULL);
    if (re == NULL)
      {
      expand_string_message = string_sprintf("regular expression error in "
        "\"%s\": %s at offset %d", sub[1], rerror, roffset);
      goto EXPAND_FAILED;
      }

    /* Now run a loop to do the substitutions as often as necessary. It ends
    when there are no more matches. Take care over matches of the null string;
    do the same thing as Perl does. */

    subject = sub[0];
    slen = (int)strlen(sub[0]);
    moffset = moffsetextra = 0;
    emptyopt = 0;

    for (;;)
      {
      int ovector[3*(EXPAND_MAXN+1)];
      int n = pcre_exec(re, NULL, subject, slen, moffset + moffsetextra,
        PCRE_EOPT | emptyopt, ovector, sizeof(ovector)/sizeof(int));
      int nn;
      char *insert;

      /* No match - if we previously set PCRE_NOTEMPTY after a null match, this
      is not necessarily the end. We want to repeat the match from one
      character further along, but leaving the basic offset the same (for
      copying below). We can't be at the end of the string - that was checked
      before setting PCRE_NOTEMPTY. If PCRE_NOTEMPTY is not set, we are
      finished; copy the remaining string and end the loop. */

      if (n < 0)
        {
        if (emptyopt != 0)
          {
          moffsetextra = 1;
          emptyopt = 0;
          continue;
          }
        yield = string_cat(yield, &size, &ptr, subject+moffset, slen-moffset);
        break;
        }

      /* Match - set up for expanding the replacement. */

      if (n == 0) n = EXPAND_MAXN + 1;
      expand_nmax = 0;
      for (nn = 0; nn < n*2; nn += 2)
        {
        expand_nstring[expand_nmax] = subject + ovector[nn];
        expand_nlength[expand_nmax++] = ovector[nn+1] - ovector[nn];
        }
      expand_nmax--;

      /* Copy the characters before the match, plus the expanded insertion. */

      yield = string_cat(yield, &size, &ptr, subject + moffset,
        ovector[0] - moffset);
      insert = expand_string(sub[2]);
      if (insert == NULL) goto EXPAND_FAILED;
      yield = string_cat(yield, &size, &ptr, insert, (int)strlen(insert));

      moffset = ovector[1];
      moffsetextra = 0;
      emptyopt = 0;

      /* If we have matched an empty string, first check to see if we are at
      the end of the subject. If so, the loop is over. Otherwise, mimic
      what Perl's /g options does. This turns out to be rather cunning. First
      we set PCRE_NOTEMPTY and PCRE_ANCHORED and try the match a non-empty
      string at the same point. If this fails (picked up above) we advance to
      the next character. */

      if (ovector[0] == ovector[1])
        {
        if (ovector[0] == slen) break;
        emptyopt = PCRE_NOTEMPTY | PCRE_ANCHORED;
        }
      }

    /* All done - restore numerical variables. */

    restore_expand_strings(save_expand_nmax, save_expand_nstring,
      save_expand_nlength);
    continue;
    }

  /* Handle keyed and numbered substring extraction. If the first argument
  consists entirely of digits, then a numerical extraction is assumed. */

  if (strcmp(name, "extract") == 0)
    {
    int i;
    int j = 2;
    int field_number = -1;
    char *save_lookup_value = lookup_value;
    char *sub[3];
    int save_expand_nmax =
      save_expand_strings(save_expand_nstring, save_expand_nlength);

    /* Read the arguments */

    for (i = 0; i < j; i++)
      {
      while (isspace((uschar)*s)) s++;
      if (*s == '{')
        {
        sub[i] = expand_string_internal(s+1, TRUE, &s, skipping);
        if (sub[i] == NULL) goto EXPAND_FAILED;
        if (*s++ != '}') goto EXPAND_FAILED_CURLY;

        /* The first argument must not be empty; if it consists entirely of
        digits, this is a numerical extraction, and we expect 3 arguments. */

        if (i == 0)
          {
          int x = 0;
          char *p = sub[0];

          if (*p == 0)
            {
            expand_string_message = "first argument of \"expand\" must not "
              "be empty";
            goto EXPAND_FAILED;
            }

          while (*p != 0 && isdigit((uschar)*p)) x = x * 10 + *p++ - '0';
          if (*p == 0)
            {
            field_number = x;
            j = 3;
            }
          }
        }
      else goto EXPAND_FAILED_CURLY;
      }

    /* Extract either the numbered or the keyed substring into $value. If
    skipping, just pretend the extraction failed. */

    lookup_value = skipping? NULL : (field_number >= 0)?
      expand_gettokened(field_number, sub[1], sub[2]) :
      expand_getkeyed(sub[0], sub[1]);

    /* If no string follows, $value gets substituted; otherwise there can
    be yes/no strings, as for lookup or if. */

    switch(process_yesno(
             skipping,                     /* were previously skipping */
             lookup_value != NULL,         /* success/failure indicator */
             NULL,                         /* message for failure */
             save_lookup_value,            /* value to reset for string2 */
             &s,                           /* input pointer */
             &yield,                       /* output pointer */
             &size,                        /* output size */
             &ptr,                         /* output current point */
             "extract"))                   /* condition type */
      {
      case 1: goto EXPAND_FAILED;          /* when all is well, the */
      case 2: goto EXPAND_FAILED_CURLY;    /* returned value is 0 */
      }

    /* All done - restore numerical variables. */

    restore_expand_strings(save_expand_nmax, save_expand_nstring,
      save_expand_nlength);

    continue;
    }

  /* Handle various operations on a subsequent string */

  if (*s == ':')
    {
    char *sub = expand_string_internal(s+1, TRUE, &s, skipping);
    if (sub == NULL) goto EXPAND_FAILED;
    s++;

    /* expand expands another time */

    if (strcmp(name, "expand") == 0)
      {
      char *expanded = expand_string_internal(sub, FALSE, NULL, skipping);
      if (expanded == NULL)
        {
        expand_string_message =
          string_sprintf("internal expansion of \"%s\" failed: %s", sub,
            expand_string_message);
        goto EXPAND_FAILED;
        }
      yield = string_cat(yield, &size, &ptr, expanded, (int)strlen(expanded));
      continue;
      }

    /* lc lowercases */

    if (strcmp(name, "lc") == 0)
      {
      int count = 0;
      char *t = sub - 1;
      while (*(++t) != 0) { *t = tolower(*t); count++; }
      yield = string_cat(yield, &size, &ptr, sub, count);
      continue;
      }

    /* uc lowercases */

    if (strcmp(name, "uc") == 0)
      {
      int count = 0;
      char *t = sub - 1;
      while (*(++t) != 0) { *t = toupper(*t); count++; }
      yield = string_cat(yield, &size, &ptr, sub, count);
      continue;
      }

    /* md5 yields the md5 hash */

    if (strcmp(name, "md5") == 0)
      {
      md5 base;
      uschar digest[16];
      int j;
      char st[33];
      md5_start(&base);
      md5_end(&base, sub, (int)strlen(sub), digest);
      for(j = 0; j < 16; j++) sprintf(st+2*j, "%02x", digest[j]);
      yield = string_cat(yield, &size, &ptr, st, (int)strlen(st));
      continue;
      }

    /* mask applies a mask to an IP address; for example the result of
    ${mask:131.111.10.206/28} is 131.111.10.192/28. */

    if (strcmp(name, "mask") == 0)
      {
      int count;
      char *endptr;
      int binary[4];
      int mask, maskoffset;
      int type = string_is_ip_address(sub, &maskoffset);
      char buffer[64];

      if (type == 0)
        {
        expand_string_message = string_sprintf("\"%s\" is not an IP address",
         sub);
        goto EXPAND_FAILED;
        }

      if (maskoffset == 0)
        {
        expand_string_message = string_sprintf("missing mask value in \"%s\"",
          sub);
        goto EXPAND_FAILED;
        }

      /* The call to string_is_ip_address will have turned the '/' separator
      into a null, and pointed maskoffset there. */

      mask = strtol(sub + maskoffset + 1, &endptr, 10);

      if (*endptr != 0 || mask < 0 || mask > ((type == 4)? 32 : 128))
        {
        sub[maskoffset] = '/';
        expand_string_message = string_sprintf("mask value too big in \"%s\"",
          sub);
        goto EXPAND_FAILED;
        }

      /* Convert the address to binary integer(s) and apply the mask */

      count = host_aton(sub, binary);
      host_mask(count, binary, mask);

      /* Convert to masked textual format and add to output. */

      yield = string_cat(yield, &size, &ptr, buffer,
        host_nmtoa(count, binary, mask, buffer));
      continue;
      }

    /* local_part and domain split up an address; parse fail yields null */

    if (strcmp(name, "local_part") == 0 || strcmp(name, "domain") == 0)
      {
      char *error;
      int start, end, domain;
      char *t = parse_extract_address(sub, &error, &start, &end, &domain,
        FALSE);
      if (t != NULL)
        {
        if (name[0] == 'l')
          {
          if (domain != 0) end = start + domain - 1;
          yield = string_cat(yield, &size, &ptr, sub+start, end-start);
          }
        else if (domain != 0)
          {
          domain += start;
          yield = string_cat(yield, &size, &ptr, sub+domain, end-domain);
          }
        }
      continue;
      }

    /* quote sticks it in quotes if it contains anything other than
    alphamerics, underscore, dot, or hyphen. */

    if (strcmp(name, "quote") == 0)
      {
      BOOL needs_quote = FALSE;
      char *t = sub - 1;
      while (*(++t) != 0)
        {
        if (*t != '_' && *t != '-' && *t != '.' && !isalnum((uschar)*t))
          { needs_quote = TRUE; break; }
        }

      if (needs_quote)
        {
        yield = string_cat(yield, &size, &ptr, "\"", 1);
        t = sub - 1;
        while (*(++t) != 0)
          {
          if (*t == '\\' || *t == '"')
            yield = string_cat(yield, &size, &ptr, "\\", 1);
          yield = string_cat(yield, &size, &ptr, t, 1);
          }
        yield = string_cat(yield, &size, &ptr, "\"", 1);
        }
      else yield = string_cat(yield, &size, &ptr, sub, (int)strlen(sub));
      continue;
      }

    /* quote_xxx calls the quoting function of the xxx lookup */

    if (strncmp(name, "quote_", 6) == 0)
      {
      int n = search_findtype(name+6, &expand_string_message);
      if (n < 0) goto EXPAND_FAILED;
      if (lookup_list[n].quote != NULL) sub = (lookup_list[n].quote)(sub);
      yield = string_cat(yield, &size, &ptr, sub, (int)strlen(sub));
      continue;
      }

    /* rx quote sticks in \ before any non-alphameric character so that
    the insertion works in a regular expression. */

    if (strcmp(name, "rxquote") == 0)
      {
      char *t = sub - 1;
      while (*(++t) != 0)
        {
        if (!isalnum((uschar)*t))
          yield = string_cat(yield, &size, &ptr, "\\", 1);
        yield = string_cat(yield, &size, &ptr, t, 1);
        }
      continue;
      }

    /* escape turns all non-printing characters into escape sequences. */

    if (strcmp(name, "escape") == 0)
      {
      char *t = string_printing(sub);
      yield = string_cat(yield, &size, &ptr, t, (int)strlen(t));
      continue;
      }

    /* length_n or l_n takes just the first n characters or the whole string,
    whichever is the shorter;

    substr_m_n, and s_m_n take n characters from offset m; negative m take
    from the end; l_n is synonymous with s_0_n. If n is omitted in substr it
    takes the rest, either to the right or to the left.

    hash_n or h_n makes a hash of length n from the string, yielding n
    characters from the set a-z; hash_n_m makes a hash of length n, but
    uses m characters from the set a-zA-Z0-9.

    nhash_n returns a single number between 0 and n-1 (in text form), while
    nhash_n_m returns a div/mod hash as two numbers "a/b". The first lies
    between 0 and n-1 and the second between 0 and m-1. */

    if (strncmp(name, "length_", 7) == 0 || strncmp(name, "l_", 2) == 0 ||
        strncmp(name, "substr_", 7) == 0 || strncmp(name, "s_", 2) == 0 ||
        strncmp(name, "hash_",   5) == 0 || strncmp(name, "h_", 2) == 0 ||
        strncmp(name, "nhash_",  6) == 0 || strncmp(name, "nh_",3) == 0)
      {
      int sign = 1;
      int value1 = 0;
      int value2 = -1;
      int *pn;
      int sublen = (int)strlen(sub);
      char *num = strchr(name, '_') + 1;

      /* "length" has only one argument, effectively being synonymous with
      substr_0_n. */

      if (name[0] == 'l')
        {
        pn = &value2;
        value2 = 0;
        }

      /* The others have one or two arguments; for "substr" the first may be
      negative. The second being negative means "not supplied". */

      else
        {
        pn = &value1;
        if (name[0] == 's' && *num == '-') { sign = -1; num++; }
        }

      /* Read up to two numbers, separated by underscores */

      while (*num != 0)
        {
        if (*num == '_' && pn == &value1)
          {
          pn = &value2;
          value2 = 0;
          num++;
          }
        else if (!isdigit((uschar)*num))
          {
          expand_string_message =
            string_sprintf("non-digit after underscore in \"%s\"", name);
          goto EXPAND_FAILED;
          }
        else *pn = (*pn)*10 + *num++ - '0';
        }
      value1 *= sign;

      /* For a text hash, the first argument is the length, and the
      second is the number of characters to use, defaulting to 26. */

      if (name[0] == 'h')
        {
        if (value2 < 0) value2 = 26;
        else if (value2 > (int)strlen(hashcodes))
          {
          expand_string_message =
            string_sprintf("hash char count too big in \"%s\"", name);
          goto EXPAND_FAILED;
          }

        /* Calculate the hash text. We know it is shorter than the original
        string, so can safely place it in sub[]. */

        if (value1 < sublen)
          {
          int c;
          int i = 0;
          int j = value1;
          while ((c = (uschar)(sub[j])) != 0)
            {
            int shift = (c + j++) & 7;
            sub[i] ^= (c << shift) | (c >> (8-shift));
            if (++i >= value1) i = 0;
            }
          for (i = 0; i < value1; i++)
            sub[i] = hashcodes[(uschar)(sub[i]) % value2];
          }
        else value1 = sublen;

        yield = string_cat(yield, &size, &ptr, sub, value1);
        continue;
        }

      /* Numeric hash. The first characters of the string are treated
      as most important, and get the highest prime numbers. */

      if (name[0] == 'n')
        {
        char *s = sub;
        int i = 0;
        unsigned long int total = 0; /* no overflow */

        while (*s != 0)
          {
          if (i == 0) i = sizeof(prime)/sizeof(int) - 1;
          total += prime[i--] * (unsigned int)(*s++);
          }

        /* If value2 is unset, just compute one number */

        if (value2 < 0)
          {
          s = string_sprintf("%d", total % value1);
          }

        /* Otherwise do a div/mod hash */

        else
          {
          total = total % (value1 * value2);
          s = string_sprintf("%d/%d", total/value2, total % value2);
          }

        yield = string_cat(yield, &size, &ptr, s, (int)strlen(s));
        continue;
        }

      /* Otherwise we are handling a substring or subbit operation, with value1
      the offset and value2 the length. A negative offset positions from the
      rhs. (For "length" and "bits" the offset is always 0.) */

      if (value1 < 0)    /* substr only */
        {
        value1 += sublen;

        /* If the position is before the start, skip to the start, unless
        length is unset, in which case the substring is null. */

        if (value1 < 0)
          {
          if (value2 < 0) value2 = 0; else value2 += value1;
          value1 = 0;
          }

        /* Otherwise an unset length => characters before the value1 */

        else if (value2 < 0)
          {
          value2 = value1;
          value1 = 0;
          }
        }

      /* For a non-negative offset, no length means "rest"; just set
      it to the maximum. */

      else if (value2 < 0) value2 = sublen;

      /* For a substring operation, cut length down to maximum possible for
      the offset value, and get the required characters. */

      if (sublen < value1 + value2) value2 = sublen - value1;
      if (value2 > 0)
        yield = string_cat(yield, &size, &ptr, sub + value1, value2);
      continue;
      }

    /* Unknown operator */

    expand_string_message =
      string_sprintf("unknown expansion operator \"%s\"", name);
    goto EXPAND_FAILED;
    }

  /* Handle a plain name */

  if (*s++ == '}')
    {
    int choplen = 0;
    value = find_variable(name, &choplen);
    if (value == NULL)
      {
      expand_string_message =
        string_sprintf("unknown variable in \"${%s}\"", name);
      goto EXPAND_FAILED;
      }
    yield = string_cat(yield, &size, &ptr, value,
      (int)strlen(value) - choplen);
    continue;
    }

  /* Else there's something wrong */

  expand_string_message =
    string_sprintf("\"${%s\" is not a known operator (or a } is missing "
    "in a variable reference)", name);
  goto EXPAND_FAILED;
  }

/* If we hit the end of the string when ket_ends is set, there is a missing
terminating brace. */

if (ket_ends && *s == 0)
  {
  expand_string_message = malformed_header?
    "missing } at end of string - could be header name not terminated by colon"
    :
    "missing } at end of string";
  goto EXPAND_FAILED;
  }

/* Expansion succeeded; add a terminating zero, and if left != NULL, return a
pointer to the terminator. */

yield[ptr] = 0;
if (left != NULL) *left = s;

/* Any stacking store that was used above the final string is no longer needed.
In many cases the final string will be the one that was got at the start and so
there will be optimal store usage. */

store_reset(yield + ptr + 1);
return yield;

/* This is the failure exit: easiest to program with a goto. We still need
to update the pointer to the terminator, for cases of nested calls with "fail".
*/

EXPAND_FAILED_CURLY:
expand_string_message = malformed_header?
  "missing or misplaced { or } - could be header name not terminated by colon"
  :
  "missing or misplaced { or }";

EXPAND_FAILED:
if (left != NULL) *left = s;
store_reset(yield);
return NULL;
}


/* This is the external function call. Do a quick check for any expansion
metacharacters, and if there are none, just return the input string.

Argument: the string to be expanded
Returns:  the expanded string, or NULL if expansion failed; if failure was
          due to a lookup deferring, search_find_defer will be TRUE
*/

char *
expand_string(char *string)
{
search_find_defer = FALSE;
malformed_header = FALSE;
return (strpbrk(string, "$\\") == NULL)? string :
  expand_string_internal(string, FALSE, NULL, FALSE);
}



/*************************************************
*              Expand and copy                   *
*************************************************/

/* Now and again we want to expand a string and be sure that the result is in a
new bit of store. This function does that.

Argument: the string to be expanded
Returns:  the expanded string, always in a new bit of store, or NULL
*/

char *
expand_string_copy(char *string)
{
char *yield = expand_string(string);
if (yield == string) yield = string_copy(string);
return yield;
}



/*************************************************
*               Expand or panic                  *
*************************************************/

/* Sometimes Exim can't continue if an expansion fails. This function ensures
that it panics.

Arguments:
  string     the string to be expanded
  text1      ) two texts to be placed in the
  text2      ) error message on failure

Returns:     the successfully expanded string
*/

char *
expand_string_panic(char *string, char *text1, char *text2)
{
char *yield;
yield = expand_string(string);
if (yield == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s %s: failed to expand \"%s\" %s",
    text1, text2, string, expand_string_message);
return yield;
}



/*************************************************
**************************************************
*             Stand-alone test program           *
**************************************************
*************************************************/

#ifdef STAND_ALONE


BOOL
regex_match_and_setup(pcre *re, char *subject, int options, int setup)
{
int ovector[3*(EXPAND_MAXN+1)];
int n = pcre_exec(re, NULL, subject, (int)strlen(subject), 0, PCRE_EOPT|options,
  ovector, sizeof(ovector)/sizeof(int));
BOOL yield = n >= 0;
if (n == 0) n = EXPAND_MAXN + 1;
if (yield)
  {
  int nn;
  expand_nmax = (setup < 0)? 0 : setup + 1;
  for (nn = (setup < 0)? 0 : 2; nn < n*2; nn += 2)
    {
    expand_nstring[expand_nmax] = subject + ovector[nn];
    expand_nlength[expand_nmax++] = ovector[nn+1] - ovector[nn];
    }
  expand_nmax--;
  }
return yield;
}


int main(int argc, char **argv)
{
int i;
char buffer[1024];

debug_level = 1;
debug_file = stderr;
debug_fd = fileno(debug_file);
debug_trace_memory = 0;
big_buffer = malloc(big_buffer_size);

for (i = 1; i < argc; i++)
  {
  if (argv[i][0] == '+')
    {
    debug_trace_memory = 2;
    argv[i]++;
    }
  if (isdigit(argv[i][0]))
    debug_level = atoi(argv[i]);
  else
    if (strspn(argv[i], "abcdefghijklmnopqrtsuvwxyz0123456789-.:/") ==
        (int)strlen(argv[i]))
      {
      #ifdef LOOKUP_LDAP
      ldap_default_servers = argv[i];
      #endif
      #ifdef LOOKUP_MYSQL
      mysql_servers = argv[i];
      #endif
      #ifdef LOOKUP_PGSQL
      pgsql_servers = argv[i];
      #endif
      }
  #ifdef EXIM_PERL
  else opt_perl_startup = argv[i];
  #endif
  }

printf("Testing string expansion: debug_level = %d\n\n", debug_level);

expand_nstring[1] = "string 1....";
expand_nlength[1] = 8;
expand_nmax = 1;

#ifdef EXIM_PERL
if (opt_perl_startup != NULL)
  {
  char *errstr;
  printf("Starting Perl interpreter\n");
  errstr = init_perl(opt_perl_startup);
  if (errstr != NULL)
    {
    printf("** error in perl_startup code: %s\n", errstr);
    return EXIT_FAILURE;
    }
  }
#endif /* EXIM_PERL */

while (fgets(buffer, sizeof(buffer), stdin) != NULL)
  {
  void *reset_point = store_get(0);
  char *yield = expand_string(buffer);
  if (yield != NULL)
    {
    printf("%s\n", yield);
    store_reset(reset_point);
    }
  else
    {
    if (search_find_defer) printf("search_find deferred\n");
    printf("Failed: %s\n", expand_string_message);
    if (expand_string_forcedfail) printf("Forced failure\n");
    printf("\n");
    }
  }

search_tidyup();

return 0;
}

#endif

/* End of expand.c */
