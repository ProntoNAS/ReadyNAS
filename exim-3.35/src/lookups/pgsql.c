/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Thanks to Petr Cech for contributing the original code for these
functions. */

#include "../exim.h"
#include "pgsql.h"       /* The local header */

/* We can't just compile this code and allow the library mechanism to omit the
functions if they are not wanted, because we need to have the PGSQL header
available for compiling. Therefore, compile these functions only if
LOOKUP_PGSQL is defined. However, some compilers don't like compiling empty
modules, so keep them happy with a dummy when skipping the rest. Make it
reference itself to stop picky compilers complaining that it is unused, and put
in a dummy argument to stop even pickier compilers complaining about infinite
loops. */

#ifndef LOOKUP_PGSQL
static void dummy(int x) { dummy(x-1); }
#else


#include <libpq-fe.h>       /* The system header */

/* Structure and anchor for caching connections. */

typedef struct pgsql_connection {
  struct pgsql_connection *next;
  char   *server;
  PGconn *handle;
} pgsql_connection;

static pgsql_connection *pgsql_connections = NULL;



/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description. */

void *
pgsql_open(char *filename, char **errmsg)
{
return (void *)(1);    /* Just return something non-null */
}



/*************************************************
*               Tidy entry point                 *
*************************************************/

/* See local README for interface description. */

void
pgsql_tidy(void)
{
pgsql_connection *cn;
while ((cn = pgsql_connections) != NULL)
  {
  pgsql_connections = cn->next;
  DEBUG(9) debug_printf("close PGSQL connection: %s\n", cn->server);
  PQfinish(cn->handle);
  store_free(cn->server);
  store_free(cn);
  }
}



/*************************************************
*        Internal search function                *
*************************************************/

/* This function is called from the find entry point to do the search for a
single server. The server string is of the form "host/dbname/user/password".
The host can be host:port.

NOTE:
 1) All three '/' must be present.
 2) If host is omitted the local unix socket is used.

Arguments:
  query        the query string
  server       the server string
  resultptr    where to store the result
  errmsg       where to point an error message
  defer_break  TRUE if no more servers are to be tried after DEFER

Returns:       OK, FAIL, or DEFER
*/

static int
perform_pgsql_search(char *query, char *server, char **resultptr, char **errmsg,
  BOOL *defer_break)
{
PGconn *pg_conn = NULL;
PGresult *pg_result = NULL;

int i;
int ssize = 0;
int offset = 0;
int yield = DEFER;
unsigned int num_fields, num_tuples;
char *result = NULL;
pgsql_connection *cn;

/* See if we have a cached connection to the server */

for (cn = pgsql_connections; cn != NULL; cn = cn->next)
  {
  if (strcmp(cn->server, server) == 0)
    {
    pg_conn = cn->handle;
    break;
    }
  }

/* If no cached connection, we must set one up */

if (cn == NULL)
  {
  int port = 0;
  char *p = server;
  char *server_copy = string_copy_malloc(server);
  char *sdata[5];

  /* Disaggregate the parameters from the server argument. The order is
  host, database, user, password. We can write to the string, since it is in an
  isinlist temporary buffer. */

  for (i = 0; i < 3; i++)
    {
    char *pp = strchr(p, '/');
    if (pp == NULL)
      {
      *errmsg = string_sprintf("incomplete PGSQL server data: %s", server_copy);
      store_free(server_copy);
      *defer_break = TRUE;
      return DEFER;
      }
    sdata[i] = p;
    *pp++ = 0;
    p = pp;
    }
  sdata[3] = p;    /* Final thing (password) is all the rest */

  /* If host contains a colon, sort out the port number. */

  if ((p = strchr(sdata[0], ':')) != NULL)
    {
    *p++ = 0;
    sdata[4] = p;
    }
  else
    sdata[4]="";

  /* If the database is the empty string, set it NULL - the query must then
  define it. */

  if (sdata[1][0] == 0) sdata[1] = NULL;

  DEBUG(9) debug_printf("PGSQL new connection: host=%s port=%d database=%s "
    "user=%s password=%s\n", sdata[0], port, sdata[1], sdata[2], sdata[3]);

  /* Get store for a new handle, initialize it, and connect to the server */

                    /*  host       port  options tty   database  user      passwd */
  pg_conn=PQsetdbLogin(sdata[0], sdata[4], NULL, NULL, sdata[1], sdata[2], sdata[3]);

  if(PQstatus(pg_conn) == CONNECTION_BAD)
    {
    *errmsg = string_sprintf("PGSQL connection failed: %s",
      PQerrorMessage(pg_conn));
    PQfinish(pg_conn);
    store_free(server_copy);
    *defer_break = FALSE;
    goto PGSQL_EXIT;
    }

  /* Add the connection to the cache */

  cn = store_malloc(sizeof(pgsql_connection));
  cn->server = server_copy;
  cn->handle = pg_conn;
  cn->next = pgsql_connections;
  pgsql_connections = cn;
  }

/* Else use a previously cached connection */

else
  {
  DEBUG(9) debug_printf("PGSQL using cached connection for %s\n", server);
  }

/* Run the query */

  pg_result = PQexec(pg_conn, query);
  switch(PQresultStatus(pg_result))
    {
    case PGRES_EMPTY_QUERY:
    case PGRES_COMMAND_OK:
    case PGRES_TUPLES_OK:
    break;

    default:
    *errmsg = string_sprintf("PGSQL: query failed: %s\n",
                             PQresultErrorMessage(pg_result));
    *defer_break = FALSE;
    goto PGSQL_EXIT;
    }

/* Result is in pg_result. Find the number of fields returned. If this is one,
we don't add field names to the data. Otherwise we do. */

num_fields = PQnfields(pg_result);
num_tuples = PQntuples(pg_result);

/* Get the fields and construct the result string. If there is more than one
row, we insert '\n' between them. */

for (i = 0; i < num_tuples; i++)
  {
  if (result != NULL)
    result = string_cat(result, &ssize, &offset, "\n", 1);

   if (num_fields == 1)
    {
    result = string_cat(result, &ssize, &offset,
      PQgetvalue(pg_result, i, 0), PQgetlength(pg_result, i, 0));
    }

   else
    {
    int j;
    for (j = 0; j < num_fields; j++)
      {
      char *tmp = PQfname(pg_result, j);
      result = string_cat(result, &ssize, &offset, tmp, (int)strlen(tmp));
      result = string_cat(result, &ssize, &offset, "=", 1);

      /* Quote the value if it contains spaces or is empty */

      tmp = PQgetvalue(pg_result, i, j);
      if (*tmp == 0 || (strchr(tmp, ' ') != NULL))
	{
	int k, t_len = (int)strlen(tmp);
	result = string_cat(result, &ssize, &offset, "\"", 1);
	for(k = 0; k < t_len; k++)
	  {
	  if (tmp[k] == '\"' || tmp[k] == '\\')
	     result = string_cat(result, &ssize, &offset, "\\", 1);
	  result = string_cat(result, &ssize, &offset, tmp+k, 1);
	  }
	result = string_cat(result, &ssize, &offset, "\"", 1);
	}
      else
	{
	result = string_cat(result, &ssize, &offset, tmp, (int)strlen(tmp));
	}
      result = string_cat(result, &ssize, &offset, " ", 1);
      }
    }
  }

/* If result is NULL then no data has been found and so we return FAIL.
Otherwise, we must terminate the string which has been built; string_cat()
always leaves enough room for a terminating zero. */

if (result == NULL)
  {
  yield = FAIL;
  *errmsg = "PGSQL: no data found";
  }
else result[offset] = 0;

/* Get here by goto from various error checks. */

PGSQL_EXIT:

/* Free store for any result that was got; don't close the connection, as
it is cached. */

if (pg_result != NULL) PQclear(pg_result);

/* Non-NULL result indicates a sucessful result */

if (result != NULL)
  {
  *resultptr = result;
  return OK;
  }
else
  {
  DEBUG(9) debug_printf("%s\n", *errmsg);
  return yield;      /* FAIL or DEFER */
  }
}




/*************************************************
*               Find entry point                 *
*************************************************/

/* See local README for interface description. The handle and filename
arguments are not used. Loop through a list of servers while the query is
deferred with a retryable error. */

int
pgsql_find(void *handle, char *filename, char *query, int length,
  char **result, char **errmsg)
{
int sep = 0;
char *server;
char *list = pgsql_servers;
char buffer[512];

DEBUG(9) debug_printf("PGSQL query: %s\n", query);

while ((server = string_nextinlist(&list, &sep, buffer, sizeof(buffer)))
        != NULL)
  {
  BOOL defer_break;
  int rc = perform_pgsql_search(query, server, result, errmsg, &defer_break);
  if (rc != DEFER || defer_break) return rc;
  }

if (pgsql_servers == NULL)
  *errmsg = "no PGSQL servers defined (pgsql_servers option)";

return DEFER;
}



/*************************************************
*               Quote entry point                *
*************************************************/

/* The characters that always need to be quoted (with backslash) are newline,
tab, carriage return, backspace, backslash itself, and the quote characters.
Percent and underscore are only special in contexts where they can be wild
cards, and this isn't usually the case for data inserted from messages, since
that isn't likely to be treated as a pattern of any kind. However, pgsql seems
to allow escaping "on spec". If you use something like "where id="ab\%cd" it
does treat the string as "ab%cd". So we can safely quote percent and
underscore. [This is different to MySQL, where you can't do this.]

Argument: the string to be quoted
Returns:  the processed string
*/

char *
pgsql_quote(char *s)
{
register int c;
int count = 0;
char *t = s;
char *quoted;

while ((c = *t++) != 0)
  if (strchr("\n\t\r\b\'\"\\%_", c) != NULL) count++;

if (count == 0) return s;
t = quoted = store_get((int)strlen(s) + count + 1);

while ((c = *s++) != 0)
  {
  if (strchr("\n\t\r\b\'\"\\%_", c) != NULL)
    {
    *t++ = '\\';
    switch(c)
      {
      case '\n': *t++ = 'n';
      break;
      case '\t': *t++ = 't';
      break;
      case '\r': *t++ = 'r';
      break;
      case '\b': *t++ = 'b';
      break;
      default:   *t++ = c;
      break;
      }
    }
  else *t++ = c;
  }

*t = 0;
return quoted;
}

#endif  /* PGSQL_LOOKUP */

/* End of lookups/pgsql.c */
