/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "nisplus.h"

/* We can't just compile this code and allow the library mechanism to omit the
functions if they are not wanted, because we need to have the NIS+ header
available for compiling. Therefore, compile these functions only if
LOOKUP_NISPLUS is defined. However, some compilers don't like compiling empty
modules, so keep them happy with a dummy when skipping the rest. Make it
reference itself to stop picky compilers complaining that it is unused, and put
in a dummy argument to stop even pickier compilers complaining about infinite
loops. */

#ifndef LOOKUP_NISPLUS
static void dummy(int x) { dummy(x-1); }
#else


#include <rpcsvc/nis.h>


/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description. */

void *
nisplus_open(char *filename, char **errmsg)
{
return (void *)(1);    /* Just return something non-null */
}



/*************************************************
*               Find entry point                 *
*************************************************/

/* See local README for interface description. The format of queries for a
NIS+ search is

  [field=value,...],table-name
or
  [field=value,...],table-name:result-field-name

in other words, a normal NIS+ "indexed name", with an optional result field
name tagged on the end after a colon. If there is no result-field name, the
yield is the concatenation of all the fields, preceded by their names and an
equals sign. */

int
nisplus_find(void *handle, char *filename, char *query, int length,
  char **result, char **errmsg)
{
int i;
int ssize = 0;
int offset = 0;
int error_error = FAIL;
char *field_name = NULL;
nis_result *nrt = NULL;
nis_result *nre = NULL;
nis_object *tno, *eno;
struct entry_obj *eo;
struct table_obj *ta;
char *p = query + length;
char *yield = NULL;

/* Search backwards for a colon to see if a result field name
has been given. */

while (p > query && p[-1] != ':') p--;

if (p > query)
  {
  field_name = p;
  p[-1] = 0;
  }
else p = query + length;

/* Now search backwards to find the comma that starts the
table name. */

while (p > query && p[-1] != ',') p--;
if (p <= query)
  {
  *errmsg = "NIS+ query malformed";
  error_error = DEFER;
  goto NISPLUS_EXIT;
  }

/* Look up the data for the table, in order to get the field names,
check that we got back a table, and set up pointers so the field
names can be scanned. */

nrt = nis_lookup(p, EXPAND_NAME | NO_CACHE);
if (nrt->status != NIS_SUCCESS)
  {
  *errmsg = string_sprintf("NIS+ error accessing %s table: %s", p,
    nis_sperrno(nrt->status));
  if (nrt->status != NIS_NOTFOUND && nrt->status != NIS_NOSUCHTABLE)
    error_error = DEFER;
  goto NISPLUS_EXIT;
  }
tno = nrt->objects.objects_val;
if (tno->zo_data.zo_type != TABLE_OBJ)
  {
  *errmsg = string_sprintf("NIS+ error: %s is not a table", p);
  goto NISPLUS_EXIT;
  }
ta = &(tno->zo_data.objdata_u.ta_data);

/* Now look up the entry in the table, check that we got precisely one
object and that it is a table entry. */

nre = nis_list(query, EXPAND_NAME, NULL, NULL);
if (nre->status != NIS_SUCCESS)
  {
  *errmsg = string_sprintf("NIS+ error accessing entry %s: %s",
    query, nis_sperrno(nre->status));
  goto NISPLUS_EXIT;
  }
if (nre->objects.objects_len > 1)
  {
  *errmsg = string_sprintf("NIS+ returned more than one object for %s",
    query);
  goto NISPLUS_EXIT;
  }
else if (nre->objects.objects_len < 1)
  {
  *errmsg = string_sprintf("NIS+ returned no data for %s", query);
  goto NISPLUS_EXIT;
  }
eno = nre->objects.objects_val;
if (eno->zo_data.zo_type != ENTRY_OBJ)
  {
  *errmsg = string_sprintf("NIS+ error: %s is not an entry", query);
  goto NISPLUS_EXIT;
  }

/* Scan the columns in the entry and in the table. If a result field
was given, look for that field; otherwise concatenate all the fields
with their names. */

eo = &(eno->zo_data.objdata_u.en_data);
for (i = 0; i < eo->en_cols.en_cols_len; i++)
  {
  table_col *tc = ta->ta_cols.ta_cols_val + i;
  entry_col *ec = eo->en_cols.en_cols_val + i;
  int len = ec->ec_value.ec_value_len;
  char *value = ec->ec_value.ec_value_val;

  /* The value may be NULL for a zero-length field. Turn this into an
  empty string for consistency. Remove trailing whitespace and zero
  bytes. */

  if (value == NULL) value = ""; else
    while (len > 0 && (value[len-1] == 0 || isspace((uschar)value[len-1])))
      len--;

  /* Concatenate all fields if no specific one selected */

  if (field_name == NULL)
    {
    yield = string_cat(yield, &ssize, &offset, tc->tc_name,
      (int)strlen(tc->tc_name));
    yield = string_cat(yield, &ssize, &offset, "=", 1);

    /* Quote the value if it contains spaces or is empty */

    if (value[0] == 0 || strchr(value, ' ') != NULL)
      {
      int j;
      yield = string_cat(yield, &ssize, &offset, "\"", 1);
      for (j = 0; j < len; j++)
        {
        if (value[j] == '\"' || value[j] == '\\')
          yield = string_cat(yield, &ssize, &offset, "\\", 1);
        yield = string_cat(yield, &ssize, &offset, value+j, 1);
        }
      yield = string_cat(yield, &ssize, &offset, "\"", 1);
      }
    else yield = string_cat(yield, &ssize, &offset, value, len);

    yield = string_cat(yield, &ssize, &offset, " ", 1);
    }

  /* When the specified field is found, grab its data and finish */

  else if (strcmp(field_name, tc->tc_name) == 0)
    {
    yield = string_copyn(value, len);
    goto NISPLUS_EXIT;
    }
  }

/* Error if a field name was specified and we didn't find it; if no
field name, ensure the concatenated data is zero-terminated. */

if (field_name != NULL)
  *errmsg = string_sprintf("NIS+ field %s not found for %s", field_name,
    query);
else yield[offset] = 0;

/* Restore the colon in the query, and free result store before
finishing. */

NISPLUS_EXIT:
if (field_name != NULL) field_name[-1] = ':';
if (nrt != NULL) nis_freeresult(nrt);
if (nre != NULL) nis_freeresult(nre);

if (yield != NULL)
  {
  *result = yield;
  return OK;
  }

return error_error;      /* FAIL or DEFER */
}



/*************************************************
*               Quote entry point                *
*************************************************/

/* The only quoting that is necessary for NIS+ is to double any doublequote
characters.

Argument: the string to be quoted
Returns:  the processed string
*/

char *
nisplus_quote(char *s)
{
int count = 0;
char *quoted;
char *t = s;
while (*t != 0) if (*t++ == '\"') count++;
if (count == 0) return s;

t = quoted = store_get((int)strlen(s) + count + 1);

while (*s != 0)
  {
  *t++ = *s;
  if (*s++ == '\"') *t++ = '\"';
  }

*t = 0;
return quoted;
}

#endif  /* LOOKUP_NISPLUS */

/* End of lookups/nisplus.c */
