/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Thanks to Paul Kelly for contributing the original code for these
functions. */


#include "../exim.h"
#include "mysql.h"       /* The local header */


/* We can't just compile this code and allow the library mechanism to omit the
functions if they are not wanted, because we need to have the MYSQL header
available for compiling. Therefore, compile these functions only if
LOOKUP_MYSQL is defined. However, some compilers don't like compiling empty
modules, so keep them happy with a dummy when skipping the rest. Make it
reference itself to stop picky compilers complaining that it is unused, and put
in a dummy argument to stop even pickier compilers complaining about infinite
loops. */

#ifndef LOOKUP_MYSQL
static void dummy(int x) { dummy(x-1); }
#else


#include <mysql.h>       /* The system header */


/* Structure and anchor for caching connections. */

typedef struct mysql_connection {
  struct mysql_connection *next;
  char  *server;
  MYSQL *handle;
} mysql_connection;

static mysql_connection *mysql_connections = NULL;



/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description. */

void *
mysql_open(char *filename, char **errmsg)
{
return (void *)(1);    /* Just return something non-null */
}



/*************************************************
*               Tidy entry point                 *
*************************************************/

/* See local README for interface description. */

void
mysql_tidy(void)
{
mysql_connection *cn;
while ((cn = mysql_connections) != NULL)
  {
  mysql_connections = cn->next;
  DEBUG(9) debug_printf("close MYSQL connection: %s\n", cn->server);
  mysql_close(cn->handle);
  store_free(cn->server);
  store_free(cn->handle);
  store_free(cn);
  }
}



/*************************************************
*        Internal search function                *
*************************************************/

/* This function is called from the find entry point to do the search for a
single server.

Arguments:
  query        the query string
  server       the server string
  resultptr    where to store the result
  errmsg       where to point an error message
  defer_break  TRUE if no more servers are to be tried after DEFER

The server string is of the form "host/dbname/user/password". The host can be
host:port.

Returns:       OK, FAIL, or DEFER
*/

static int
perform_mysql_search(char *query, char *server, char **resultptr, char **errmsg,
  BOOL *defer_break)
{
MYSQL *mysql_handle = NULL;        /* Keep compilers happy */
MYSQL_RES *mysql_result = NULL;
MYSQL_ROW mysql_row_data;
MYSQL_FIELD *fields;

int i;
int ssize = 0;
int offset = 0;
int yield = DEFER;
unsigned int num_fields;
char *result = NULL;
mysql_connection *cn;

/* See if we have a cached connection to the server */

for (cn = mysql_connections; cn != NULL; cn = cn->next)
  {
  if (strcmp(cn->server, server) == 0)
    {
    mysql_handle = cn->handle;
    break;
    }
  }

/* If no cached connection, we must set one up */

if (cn == NULL)
  {
  int port = 0;
  char *p = server;
  char *server_copy = string_copy_malloc(server);
  char *sdata[4];

  /* Disaggregate the parameters from the server argument. The order is
  host, database, user, password. We can write to the string, since it is in an
  isinlist temporary buffer. */

  for (i = 0; i < 3; i++)
    {
    char *pp = strchr(p, '/');
    if (pp == NULL)
      {
      *errmsg = string_sprintf("incomplete MYSQL server data: %s", server_copy);
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
    port = atoi(p);
    }

  /* If the database is the empty string, set it NULL - the query must then
  define it. */

  if (sdata[1][0] == 0) sdata[1] = NULL;

  DEBUG(9) debug_printf("MYSQL new connection: host=%s port=%d database=%s "
    "user=%s password=%s\n", sdata[0], port, sdata[1], sdata[2], sdata[3]);

  /* Get store for a new handle, initialize it, and connect to the server */

  mysql_handle = store_malloc(sizeof(MYSQL));
  mysql_init(mysql_handle);
                                    /*  host       user    passwd    database */
  if (mysql_real_connect(mysql_handle, sdata[0], sdata[2], sdata[3], sdata[1],
      port, NULL, 0) == NULL)
    {
    *errmsg = string_sprintf("MYSQL connection failed: %s",
      mysql_error(mysql_handle));
    store_free(mysql_handle);
    store_free(server_copy);
    *defer_break = FALSE;
    goto MYSQL_EXIT;
    }

  /* Add the connection to the cache */

  cn = store_malloc(sizeof(mysql_connection));
  cn->server = server_copy;
  cn->handle = mysql_handle;
  cn->next = mysql_connections;
  mysql_connections = cn;
  }

/* Else use a previously cached connection */

else
  {
  DEBUG(9) debug_printf("MYSQL using cached connection for %s\n", server);
  }

/* Run the query */

if (mysql_query(mysql_handle, query) != 0)
  {
  *errmsg = string_sprintf("MYSQL: query failed: %s\n",
    mysql_error(mysql_handle));
  *defer_break = FALSE;
  goto MYSQL_EXIT;
  }

/* Pick up the result */

if ((mysql_result = mysql_use_result(mysql_handle)) == NULL)
  {
  *errmsg = string_sprintf("MYSQL: lookup result failed: %s\n",
    mysql_error(mysql_handle));
  *defer_break = FALSE;
  goto MYSQL_EXIT;
  }

/* Find the number of fields returned. If this is one, we don't add field
names to the data. Otherwise we do. */

num_fields = mysql_num_fields(mysql_result);

/* Get the fields and construct the result string. If there is more than one
row, we insert '\n' between them. */

fields = mysql_fetch_fields(mysql_result);

while ((mysql_row_data = mysql_fetch_row(mysql_result)) != NULL)
  {
  unsigned long *lengths = mysql_fetch_lengths(mysql_result);

  if (result != NULL)
      result = string_cat(result, &ssize, &offset, "\n", 1);

  if (num_fields == 1)
    {
    result = string_cat(result ,&ssize, &offset, mysql_row_data[0],
      lengths[0]);
    }

  else for (i = 0; i < num_fields; i++)
    {
    result = string_cat(result, &ssize, &offset, fields[i].name,
      strlen(fields[i].name));
    result = string_cat(result, &ssize, &offset, "=", 1);

    /* Quote the value if it contains spaces or is empty */

    if (mysql_row_data[i][0] == 0 || strchr(mysql_row_data[i], ' ') != NULL)
      {
      int j;
      result = string_cat(result, &ssize, &offset, "\"", 1);
      for (j = 0; j < lengths[i]; j++)
        {
	if (mysql_row_data[i][j] == '\"' || mysql_row_data[i][j] == '\\')
	  result = string_cat(result, &ssize, &offset, "\\", 1);
	result = string_cat(result, &ssize, &offset, mysql_row_data[i]+j, 1);
        }
      result = string_cat(result, &ssize, &offset, "\"", 1);
      }
    else
      {
      result = string_cat(result ,&ssize, &offset, mysql_row_data[i],
        lengths[i]);
      }
    result = string_cat(result, &ssize, &offset, " ", 1);
    }
  }

/* If result is NULL then no data has been found and so we return FAIL.
Otherwise, we must terminate the string which has been built; string_cat()
always leaves enough room for a terminating zero. */

if (result == NULL)
  {
  yield = FAIL;
  *errmsg = "MYSQL: no data found";
  }
else result[offset] = 0;

/* Get here by goto from various error checks. */

MYSQL_EXIT:

/* Free mysal store for any result that was got; don't close the connection, as
it is cached. */

if (mysql_result != NULL) mysql_free_result(mysql_result);

/* Non-NULL result indicates a sucessful result */

if(result != NULL)
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
mysql_find(void *handle, char *filename, char *query, int length,
  char **result, char **errmsg)
{
int sep = 0;
char *server;
char *list = mysql_servers;
char buffer[512];

DEBUG(9) debug_printf("MYSQL query: %s\n", query);

while ((server = string_nextinlist(&list, &sep, buffer, sizeof(buffer))) != NULL)
  {
  BOOL defer_break;
  int rc = perform_mysql_search(query, server, result, errmsg, &defer_break);
  if (rc != DEFER || defer_break) return rc;
  }

if (mysql_servers == NULL)
  *errmsg = "no MYSQL servers defined (mysql_servers option)";

return DEFER;
}



/*************************************************
*               Quote entry point                *
*************************************************/

/* The only characters that need to be quoted (with backslash) are newline,
tab, carriage return, backspace, backslash itself, and the quote characters.
Percent, and underscore and not escaped. They are only special in contexts
where they can be wild cards, and this isn't usually the case for data inserted
from messages, since that isn't likely to be treated as a pattern of any kind.
Sadly, MySQL doesn't seem to behave like other programs. If you use something
like "where id="ab\%cd" it does not treat the string as "ab%cd". So you really
can't quote "on spec".

Argument: the string to be quoted
Returns:  the processed string
*/

char *
mysql_quote(char *s)
{
register int c;
int count = 0;
char *t = s;
char *quoted;

while ((c = *t++) != 0)
  if (strchr("\n\t\r\b\'\"\\", c) != NULL) count++;

if (count == 0) return s;
t = quoted = store_get((int)strlen(s) + count + 1);

while ((c = *s++) != 0)
  {
  if (strchr("\n\t\r\b\'\"\\", c) != NULL)
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


#endif  /* MYSQL_LOOKUP */

/* End of lookups/mysql.c */
