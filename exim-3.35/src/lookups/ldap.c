/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Many thanks to Stuart Lynne for contributing the original code for this
driver. Further modification from michael@freenet-ag.de, Brian Candler,
and Barry Pederson. */


#include "../exim.h"
#include "ldap.h"


/* We can't just compile this code and allow the library mechanism to omit the
functions if they are not wanted, because we need to have the LDAP headers
available for compiling. Therefore, compile these functions only if LOOKUP_LDAP
is defined. However, some compilers don't like compiling empty modules, so keep
them happy with a dummy when skipping the rest. Make it reference itself to
stop picky compilers complaining that it is unused, and put in a dummy argument
to stop even pickier compilers complaining about infinite loops. */

#ifndef LOOKUP_LDAP
static void dummy(int x) { dummy(x-1); }
#else


/* Include LDAP headers */

#include <lber.h>
#include <ldap.h>


/* For Exim release 3.20 the behaviour with regard to multiple attributes in
responses was changed. Instead of comma-separated fields, space separated
fields, quoted if necessary, with attribute names are used. The old behaviour
can be re-instated by

#define OLD_LDAP_BEHAVIOUR

This should only be used if you really need it as a quick fix.
*/


/* Annoyingly, the different LDAP libraries handle errors in different ways,
and some other things too. There doesn't seem to be an automatic way of
distinguishing between them. Local/Makefile should contain a setting of
LDAP_LIB_TYPE, which in turn causes appropriate macros to be defined for the
different kinds. Those that matter are:

LDAP_LIB_NETSCAPE
LDAP_LIB_SOLARIS   with synonym LDAP_LIB_SOLARIS7
LDAP_LIB_OPENLDAP2

These others may be defined, but are in fact the default, so are not tested:

LDAP_LIB_UMICHIGAN
LDAP_LIB_OPENLDAP1
*/

#if defined(LDAP_LIB_SOLARIS7) && ! defined(LDAP_LIB_SOLARIS)
#define LDAP_LIB_SOLARIS
#endif


/* Just in case LDAP_NO_LIMIT is not defined by some of these libraries. */

#ifndef LDAP_NO_LIMIT
#define LDAP_NO_LIMIT 0
#endif


/* Three types of LDAP search are implemented */

#define SEARCH_LDAP_MULTIPLE 0       /* Get attributes from multiple entries */
#define SEARCH_LDAP_SINGLE 1         /* Get attributes from one entry only */
#define SEARCH_LDAP_DN 2             /* Get DN from one entry */


/* Structure and anchor for caching connections. */

typedef struct ldap_connection {
  struct ldap_connection *next;
  char *host;
  char *user;
  char *password;
  BOOL bound;
  int   port;
  LDAP *ld;
} LDAP_CONNECTION;

static LDAP_CONNECTION *ldap_connections = NULL;



/*************************************************
*         Internal search function               *
*************************************************/

/* This is the function that actually does the work. It is called (indirectly
via control_ldap_search) from eldap_find(), eldapdn_fing(), and eldapm_find(),
with a difference in the "single" argument.

Arguments:
  ldap_url      the URL to be looked up
  server        server host name, when URL contains none
  s_port        server port, used when URL contains no name
  search_type   SEARCH_LDAP_MULTIPLE allows values from multiple entries
                SEARCH_LDAP_SINGLE allows values from one entry only
                SEARCH_LDAP_DN gets the DN from one entry
  res           set to point at the result
  errmsg        set to point a message if result is not OK
  defer_break   set TRUE if no more servers to be tried after a DEFER
  user          user name for authentication, or NULL
  password      password for authentication, or NULL
  sizelimit     max number of entries returned, or 0 for no limit
  timelimit     max time to wait, or 0 for no limit

Returns:        OK or FAIL or DEFER
                FAIL is given only if a lookup was performed successfully, but
                returned no data.
*/

static int
perform_ldap_search(char *ldap_url, char *server, int s_port, int search_type,
  char **res, char **errmsg, BOOL *defer_break, char *user, char *password,
  int sizelimit, int timelimit)
{
LDAPURLDesc  *ludp = NULL;
LDAPMessage  *result;
BerElement   *ber;
LDAP_CONNECTION *lcp;

struct timeval timeout;
struct timeval *timeoutptr = NULL;

char  *attr;
char  *data = NULL;
char  *host;
char  **attrp;
char  **values;
char  **firstval;
int   attr_count = 0;
int   error_yield = DEFER;
int   msgid;
int   rc;
int   port;
int   ptr = 0;
int   rescount = 0;
int   size = 0;
BOOL  add_newline = FALSE;
BOOL  attribute_found = FALSE;

DEBUG(9)
  debug_printf("perform_ldap_search: ldap%s URL =\"%s\" server=%s port=%d "
    "sizelimit=%d timelimit=%d\n",
    (search_type == SEARCH_LDAP_MULTIPLE)? "m" :
    (search_type == SEARCH_LDAP_DN)? "dn" : "",
    ldap_url, server, s_port, sizelimit, timelimit);

/* Check if LDAP thinks the URL is a valid LDAP URL */

if (!ldap_is_ldap_url(ldap_url))
  {
  *errmsg = string_sprintf("ldap_is_ldap_url: not an LDAP url \"%s\"\n",
    ldap_url);
  goto RETURN_ERROR_BREAK;
  }

/* Parse the URL */

if ((rc = ldap_url_parse(ldap_url, &ludp)) != 0)
  {
  *errmsg = string_sprintf("ldap_url_parse: (error %d) parsing \"%s\"\n", rc,
    ldap_url);
  goto RETURN_ERROR_BREAK;
  }

/* If the host name is empty, take it from the separate argument, if
one is given. OpenLDAP 2.0.6 sets an unset hostname to "" rather than
empty, but expects NULL later to mean "default", annoyingly. OpenLDAP 2.0.11
seems to have fixed this. */

if ((ludp->lud_host == NULL || ludp->lud_host[0] == 0) && server != NULL)
  {
  host = server;
  port = s_port;
  }
else
  {
  host = ludp->lud_host;
  if (host != NULL && host[0] == 0) host = NULL;
  port = ludp->lud_port;
  }

/* Count the attributes; we need this later to tell us how to format results */

for (attrp = ludp->lud_attrs; attrp != NULL && *attrp != NULL; attrp++)
  attr_count++;

/* See if we can find a cached connection to this host. The host name pointer
is set to NULL if no host was given, rather than to the empty string. */

for (lcp = ldap_connections; lcp != NULL; lcp = lcp->next)
  {
  if (lcp->port != port) continue;
  if (lcp->host == NULL)
    {
    if (host == NULL) break;
    }
  else
    {
    if (host != NULL && strcmpic(lcp->host, host) == 0) break;
    }
  }

/* If no cached connection found, we must open a connection to the server. */

if (lcp == NULL)
  {
  LDAP *ld = ldap_open(host, (port != 0)? port : LDAP_PORT);
  if (ld == NULL)
    {
    *errmsg = string_sprintf("failed to open connection to LDAP server %s:%d "
      "- %s", host, port, strerror(errno));
    goto RETURN_ERROR;
    }

  DEBUG(9) debug_printf("Opened connection to LDAP server %s:%d\n", host, port);

  lcp = store_malloc(sizeof(LDAP_CONNECTION));
  lcp->host = (host == NULL)? NULL : string_copy_malloc(host);
  lcp->bound = FALSE;
  lcp->port = port;
  lcp->ld = ld;
  lcp->next = ldap_connections;
  ldap_connections = lcp;
  }

/* Found cached connection */

else
  {
  DEBUG(9) debug_printf("Re-using cached connection to LDAP server %s:%d\n",
    host, port);
  }

/* Bind with the user/password supplied, or an anonymous bind if these values
are NULL, unless a cached connection is already bound with the same values. */

if (!lcp->bound ||
    (lcp->user == NULL && user != NULL) ||
    (lcp->user != NULL && user == NULL) ||
    (lcp->user != NULL && user != NULL && strcmp(lcp->user, user) != 0) ||
    (lcp->password == NULL && password != NULL) ||
    (lcp->password != NULL && password == NULL) ||
    (lcp->password != NULL && password != NULL &&
      strcmp(lcp->password, password) != 0))
  {
  DEBUG(9) debug_printf("Binding with user=%s password=%s\n", user, password);
  if ((rc = ldap_bind_s(lcp->ld, user, password, LDAP_AUTH_SIMPLE))
       != LDAP_SUCCESS)
    {
    *errmsg = string_sprintf("failed to bind the LDAP connection to server "
      "%s:%d - LDAP error %d: %s", host, port, rc, ldap_err2string(rc));
    goto RETURN_ERROR;
    }
  lcp->bound = TRUE;
  lcp->user = (user == NULL)? NULL : string_copy_malloc(user);
  lcp->password = (password == NULL)? NULL : string_copy_malloc(password);
  }

/* Before doing the search, set the time and size limits (if given). Here again
the different implementations of LDAP have chosen to do things differently. */

#if defined(LDAP_OPT_SIZELIMIT)
ldap_set_option(lcp->ld, LDAP_OPT_SIZELIMIT, (void *)&sizelimit);
ldap_set_option(lcp->ld, LDAP_OPT_TIMELIMIT, (void *)&timelimit);
#else
lcp->ld->ld_sizelimit = sizelimit;
lcp->ld->ld_timelimit = timelimit;
#endif

/* Start the search on the server. */

DEBUG(9) debug_printf("Start search\n");

msgid = ldap_search(lcp->ld, ludp->lud_dn, ludp->lud_scope, ludp->lud_filter,
  ludp->lud_attrs, 0);

if (msgid == -1)
  {
  *errmsg = string_sprintf("ldap search initiation failed");
  goto RETURN_ERROR;
  }

/* Initialize chunk of store in which to return the answer. */

size = 100;
ptr = 0;
data = store_get(size);

/* Loop to pick up results as they come in, setting a timeout if one was
given. */

if (timelimit > 0)
  {
  timeout.tv_sec = timelimit;
  timeout.tv_usec = 0;
  timeoutptr = &timeout;
  }

while ((rc = ldap_result(lcp->ld, msgid, 0, timeoutptr, &result)) ==
        LDAP_RES_SEARCH_ENTRY)
  {
  LDAPMessage  *e;

  DEBUG(9) debug_printf("ldap_result loop\n");

  for(e = ldap_first_entry(lcp->ld, result);
      e != NULL;
      e = ldap_next_entry(lcp->ld, e))
    {
    BOOL add_space = FALSE;

    DEBUG(9) debug_printf("LDAP entry loop\n");

    rescount++;   /* Count results */

    /* Results for multiple entries values are separated by newlines. */

    if (add_newline)
      data = string_cat(data, &size, &ptr, "\n", 1);
    else
      add_newline = TRUE;

    /* If the data we want is actually the DN rather than any attribute values,
    extract it. If there are multiple entries, the DNs will be concatenated,
    but we test for this case below, as for SEARCH_LDAP_SINGLE, and give an
    error. */

    if (search_type == SEARCH_LDAP_DN)
      {
      char *dn = ldap_get_dn(lcp->ld, e);
      if (dn != NULL)
        {
        data = string_cat(data, &size, &ptr, dn, (int)strlen(dn));
        data[ptr] = 0;
        #if defined LDAP_LIB_NETSCAPE || defined LDAP_LIB_OPENLDAP2
        ldap_memfree(dn);
        #else   /* OPENLDAP 1, UMich, Solaris */
        free(dn);
        #endif
        attribute_found = TRUE;
        }
      }

    /* Otherwise, loop through the entry, grabbing attribute values. Multiple
    attribute values are separated by spaces and quoted if necessary, just like
    NIS+. */

    else for (attr = ldap_first_attribute(lcp->ld, e, &ber);
              attr != NULL;
              attr = ldap_next_attribute(lcp->ld, e, ber))
      {
      if (attr[0] != 0)
        {
        /* Get array of values for this attribute. */

        if ((firstval = values = ldap_get_values(lcp->ld, e, attr)) != NULL)
          {
          while (*values != NULL)
            {
            DEBUG(9) debug_printf("LDAP attr loop %s:%s\n", attr, *values);

            if (add_space)
              {
              #ifdef OLD_LDAP_BEHAVIOUR
              data = string_cat(data, &size, &ptr, ", ", 2);
              #else
              if (attr_count != 1)
                data = string_cat(data, &size, &ptr, " ", 1);
              else
                data = string_cat(data, &size, &ptr, ", ", 2);
              #endif
              }
            else
              add_space = TRUE;

            #ifdef OLD_LDAP_BEHAVIOUR
            if (attr_count > 1)
            #else
            if (attr_count != 1)
            #endif

              {
              data = string_cat(data, &size, &ptr, attr, strlen(attr));
              data = string_cat(data, &size, &ptr, "=", 1);
              }

            #if OLD_LDAP_BEHAVIOUR
            data = string_cat(data, &size, &ptr, *values, strlen(*values));
            #else

            /* New behaviour: quote the value if it contains spaces or is
            empty, except for single attributes. */

            if (attr_count != 1 && ((*values)[0] == 0 ||
                strchr(*values, ' ') != NULL))
              {
              char *value = *values;
              int j;
              int len = (int)strlen(value);
              data = string_cat(data, &size, &ptr, "\"", 1);
              for (j = 0; j < len; j++)
                {
                if (value[j] == '\"' || value[j] == '\\')
                  data = string_cat(data, &size, &ptr, "\\", 1);
                data = string_cat(data, &size, &ptr, value+j, 1);
                }
              data = string_cat(data, &size, &ptr, "\"", 1);
              }
            else
              data = string_cat(data, &size, &ptr, *values, strlen(*values));
            #endif

            data[ptr] = 0;
            values++;
            attribute_found = TRUE;
            }

          ldap_value_free(firstval);
          }
        }

      #if defined LDAP_LIB_NETSCAPE || defined LDAP_LIB_OPENLDAP2

      /* Netscape and OpenLDAP2 LDAP's attrs are dynamically allocated and need
      to be freed. UMich LDAP stores them in static storage and does not require
      this. */

      ldap_memfree(attr);
      #endif
      }        /* End "for" loop for extracting attributes from an entry */
    }          /* End "for" loop for extracting entries from a result */

  /* Free the result */

  ldap_msgfree(result);
  }            /* End "while" loop for multiple results */

DEBUG(9) debug_printf("search ended by ldap_result yielding %d\n",rc);

if (rc == 0)
  {
  *errmsg = "ldap_result timed out";
  goto RETURN_ERROR;
  }

/* Annoyingly, the different implementations of LDAP have gone for different
methods of handling error codes and generating error messages. */

if (rc == -1)
  {
  DEBUG(9) debug_printf("ldap_result failed\n");

  #if defined LDAP_LIB_SOLARIS || defined LDAP_LIB_OPENLDAP2
    *errmsg = string_sprintf("ldap_result failed: %s",
      ldap_err2string(ldap_result2error(lcp->ld, result, 0)));

  #elif defined LDAP_LIB_NETSCAPE
    {
    char *error, *matched;
    (void)ldap_get_lderrno(lcp->ld, &matched, &error);
    *errmsg = string_sprintf("ldap_result failed: %s (%s)", error, matched);
    }

  #else                             /* UMich LDAP aka OpenLDAP 1.x */
    *errmsg = string_sprintf("ldap_result failed: %s (%s)",
      lcp->ld->ld_error,
      lcp->ld->ld_matched);
  #endif

  goto RETURN_ERROR;
  }

/* Check if we have too many results */

if (search_type != SEARCH_LDAP_MULTIPLE && rescount > 1)
  {
  *errmsg = string_sprintf("LDAP search: more than one entry (%d) was returned "
    "(filter not specific enough?)", rescount);
  goto RETURN_ERROR_BREAK;
  }

/* Check if we have too few (zero) entries */

if (rescount < 1)
  {
  *errmsg = string_sprintf("LDAP search: no results");
  error_yield = FAIL;
  goto RETURN_ERROR_BREAK;
  }

/* If an entry was found, but it had no attributes, we behave as if no entries
were found, that is, the lookup failed. */

if (!attribute_found)
  {
  *errmsg = "LDAP search: found no attributes";
  error_yield = FAIL;
  goto RETURN_ERROR;
  }

/* Otherwise, it's all worked */

DEBUG(9) debug_printf("LDAP search: returning: %s\n", data);
*res = data;
ldap_free_urldesc(ludp);
return OK;

/* Error returns */

RETURN_ERROR_BREAK:
*defer_break = TRUE;

RETURN_ERROR:
if (ludp != NULL) ldap_free_urldesc(ludp);
DEBUG(9) debug_printf("%s\n", *errmsg);
return error_yield;
}



/*************************************************
*        Internal search control function        *
*************************************************/

/* This function is called from eldap_find(), eldapdn_find(), and eldapm_find()
with a difference in the "search_type" argument. It controls calls to
perform_ldap_search() which actually does the work. We call that repeatedly for
certain types of defer in the case when the URL contains no host name and
ldap_default_servers is set to a list of servers to try. This gives more
control than just passing over a list of hosts to ldap_open() because it
handles other kinds of defer as well as just a failure to open. Note that the
URL is defined to contain either zero or one "hostport" only.

Parameter data in addition to the URL can be passed as preceding text in the
string, as items of the form XXX=yyy. The URL itself can be detected because it
must begin "ldap:///".

Arguments:
  ldap_url      the URL to be looked up, optionally preceded by other parameter
                settings
  search_type   SEARCH_LDAP_MULTIPLE allows values from multiple entries
                SEARCH_LDAP_SINGLE allows values from one entry only
                SEARCH_LDAP_DN gets the DN from one entry
  res           set to point at the result
  errmsg        set to point a message if result is not OK

Returns:        OK or FAIL or DEFER
*/

static int
control_ldap_search(char *ldap_url, int search_type, char **res, char **errmsg)
{
BOOL defer_break = FALSE;
int timelimit = LDAP_NO_LIMIT;
int sizelimit = LDAP_NO_LIMIT;
int sep = 0;
char *url = ldap_url;
char *user = NULL;
char *password = NULL;
char *server, *list;
char buffer[512];

/* Until the string begins "ldap://", search for the other parameter settings
that are recognized. They are of the form NAME=VALUE, with the value being
optionally double-quoted. There must still be a space after it, however. */

while (strncmp(url, "ldap://", 7) != 0)
  {
  char *name = url;
  while (*url != 0 && *url != '=') url++;
  if (*url == '=')
    {
    int namelen = ++url - name;
    char *value = string_dequote(&url);
    if (isspace((uschar)*url))
      {
      if (strncmpic(name, "USER=", namelen) == 0) user = value;
      else if (strncmpic(name, "PASS=", namelen) == 0) password = value;
      else if (strncmpic(name, "SIZE=", namelen) == 0) sizelimit = atoi(value);
      else if (strncmpic(name, "TIME=", namelen) == 0) timelimit = atoi(value);
      else
        {
        *errmsg =
          string_sprintf("unknown parameter \"%.*s\" precedes LDAP URL",
          namelen, name);
        return DEFER;
        }
      while (isspace((uschar)*url)) url++;
      continue;
      }
    }
  *errmsg = "malformed parameter setting precedes LDAP URL";
  return DEFER;
  }

DEBUG(9) debug_printf("LDAP parameters: user=%s pass=%s size=%d time=%d\n",
  user, password, sizelimit, timelimit);

/* No default servers, or URL contains a server name */

if (ldap_default_servers == NULL || strncmpic(url, "ldap:///", 8) != 0)
  {
  return perform_ldap_search(url, NULL, 0, search_type, res, errmsg,
    &defer_break, user, password, sizelimit, timelimit);
  }

/* Loop through the default servers until OK or FAIL */

list = ldap_default_servers;
while ((server = string_nextinlist(&list, &sep, buffer, sizeof(buffer))) != NULL)
  {
  int rc;
  int port = 0;
  char *colon = strchr(server, ':');
  if (colon != NULL)
    {
    *colon = 0;
    port = atoi(colon+1);
    }
  rc = perform_ldap_search(url, server, port, search_type, res, errmsg,
    &defer_break, user, password, sizelimit, timelimit);
  if (rc != DEFER || defer_break) return rc;
  }

return DEFER;
}



/*************************************************
*               Find entry point                 *
*************************************************/

/* See local README for interface description. The two different searches are
handled by a common function, with a flag to differentiate between them. The
handle and filename arguments are not used. */

int
eldap_find(void *handle, char *filename, char *ldap_url, int length,
  char **result, char **errmsg)
{
return(control_ldap_search(ldap_url, SEARCH_LDAP_SINGLE, result, errmsg));
}

int
eldapm_find(void *handle, char *filename, char *ldap_url, int length,
  char **result, char **errmsg)
{
return(control_ldap_search(ldap_url, SEARCH_LDAP_MULTIPLE, result, errmsg));
}

int
eldapdn_find(void *handle, char *filename, char *ldap_url, int length,
  char **result, char **errmsg)
{
return(control_ldap_search(ldap_url, SEARCH_LDAP_DN, result, errmsg));
}



/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description. */

void *
eldap_open(char *filename, char **errmsg)
{
return (void *)(1);    /* Just return something non-null */
}



/*************************************************
*               Tidy entry point                 *
*************************************************/

/* See local README for interface description. */

void
eldap_tidy(void)
{
LDAP_CONNECTION *lcp = NULL;
while ((lcp = ldap_connections) != NULL)
  {
  DEBUG(9) debug_printf("unbind LDAP connection to %s:%d\n", lcp->host,
    lcp->port);
  ldap_unbind(lcp->ld);
  ldap_connections = lcp->next;
  if (lcp->host != NULL) store_free(lcp->host);
  store_free(lcp);
  }
}



/*************************************************
*               Quote entry point                *
*************************************************/

/* Two levels of quoting have to be done, LDAP quoting, and URL quoting. LDAP
quoting uses a backslash, and requires the quoting of #,+"\<>; for
distinguished names, and *() for filters (and spaces at the start and end
of things). It does no harm to quote when not strictly needed.

For URL quoting, the only characters that need not be quoted are the
alphamerics and !$'()*+-._ - all the others must be hexified and preceded by %.
This includes the backslashes used for LDAP quoting.

Argument: the string to be quoted
Returns:  the processed string
*/

#define LDAP_QUOTE   "#,+\"\\<>;*()"
#define URL_NONQUOTE "!$'()*+-._"

char *
eldap_quote(char *s)
{
register int c;
int count = 0;
char *t = s;
char *quoted;

while ((c = *t++) != 0)
  {
  if (!isalnum(c))
    {
    if (strchr(URL_NONQUOTE, c) == NULL) count += 2;
    if (strchr(LDAP_QUOTE, c) != NULL) count += 3;
    }
  }

if (count == 0) return s;
t = quoted = store_get((int)strlen(s) + count + 1);

while ((c = *s++) != 0)
  {
  if (!isalnum(c))
    {
    if (strchr(LDAP_QUOTE, c) != NULL)
      {
      strncpy(t, "%5C", 3);
      t += 3;
      }
    if (strchr(URL_NONQUOTE, c) == NULL)
      {
      sprintf(t, "%%%02X", c);
      t += 3;
      continue;
      }
    }
  *t++ = c;
  }

*t = 0;
return quoted;
}

#endif  /* LOOKUP_LDAP */

/* End of lookups/ldap.c */
