/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "dfunctions.h"



/*************************************************
*           Process query or queries             *
*************************************************/

/* This function is called by aliasfile and forwardfile to process the "query"
or "queries" options.

Arguments:
  dblock      pointer to director instance
  addr        the address being directed
  textptr     pointer for returning the looked up text
  fnptr       pointer for returning file name (for error messages)

Returns:      OK, DEFER, DECLINE, or ERROR
*/

int
df_lookup(director_instance *dblock, address_item *addr, uschar **textptr,
  uschar **fnptr)
{
common_director_options_block *ob =
  (common_director_options_block *)(dblock->options_block);
uschar *query;
uschar *filename;
uschar *error;
void *handle;

DEBUG(9) debug_printf("df_lookup entered: search type = %s\n",
  lookup_list[ob->search_type].name);

/* For single-key+file search types, set the required file name and expand it.
If the expansion fails, log the incident and indicate an internal error. The
file name has already been checked for absoluteness, at initialization time,
but only if it did not start with an expansion, so we double check here. */

if (!mac_islookup(ob->search_type, lookup_querystyle))
  {
  query = (ob->include_domain)?
    US string_sprintf("%s@%s", addr->local_part, addr->domain) :
    US addr->local_part;
  *fnptr = filename = US expand_string(ob->file);
  if (filename == NULL)
    {
    addr->message = string_sprintf("failed to expand \"%s\": %s",
      ob->file, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }

  else if (mac_islookup(ob->search_type, lookup_absfile) &&
           filename[0] != '/')
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "%s director requires absolute file name "
      "for %s: %s generated from expanding %s", dblock->name,
      lookup_list[ob->search_type].name, filename, ob->file);
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }

  DEBUG(2)
    {
    uschar *pmstar = US "";
    uschar *pmat = US "";
    int pm = ob->partial_match;
    if (pm > 2000)
      {
      pmstar = US "*@";
      pm -= 2048;
      }
    if (pm > 1000)
      {
      pmstar = US "*";
      pm -= 1024;
      }
    debug_printf("%s director: ", dblock->name);
    if (pm >= 0) debug_printf("partial%d-", pm);
    debug_printf("%s%s%s key=%s\n  file=\"%s\"\n",
      lookup_list[ob->search_type].name, pmstar, pmat, query, filename);
    }
  }

/* For query-style lookups, expand the query and set the filename NULL.
For historical reasons, there may be just one query or a list of queries, but
not both. Initialization ensures that one and only one option is set. */

else
  {
  *fnptr = filename = NULL;
  query = US expand_string((ob->query == NULL)? ob->queries : ob->query);
  if (query == NULL)
    {
    addr->message = string_sprintf("failed to expand \"%s\": %s",
      (ob->query == NULL)? ob->queries : ob->query, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  DEBUG(2) debug_printf("%s director: %s query=%s\n", dblock->name,
    lookup_list[ob->search_type].name, query);
  }

/* Open the file (or whatever) for searching, according to the search type
that is set. The search functions return a handle identifying the search. For
files this is a FILE * or a DBM *; for other things it is < 0. If the
optional flag is set, failure to open is not an error; we just fail to
direct. */

handle = search_open(CS filename, ob->search_type, ob->modemask, ob->owners,
  ob->owngroups, CSS &error);

if (handle == NULL)
  {
  if (ob->optional && errno == ENOENT)
    {
    DEBUG(2) debug_printf("%s director skipped: file failed to open and "
      "optional flag set\n", dblock->name);
    return DECLINE;
    }
  addr->basic_errno = ERRNO_BADALIAS;
  addr->message = CS error;

  if (errno == ERRNO_BADMODE)
    error = US string_sprintf("%s (check the modemask option)", error);

  log_write(0, LOG_MAIN|LOG_PANIC, "%s director: %s", dblock->name, error);
  return ERROR;
  }

/* Now search the file (or whatever) for the entry we are interested in.
The text is returned in dynamic store. If ob->queries is set, we have
a list of queries to try. */

if (ob->queries != NULL)
  {
  uschar *onequery;
  int sep = 0;
  while ((onequery = US string_nextinlist(CSS &query, &sep, big_buffer,
          BIG_BUFFER_SIZE)) != NULL)
    {
    *textptr = US search_find(handle, CS filename, CS onequery, ob->partial_match,
      NULL, CSS &error);
    if (*textptr != NULL || search_find_defer) break;
    }
  }
else
  *textptr = US search_find(handle, CS filename, CS query, ob->partial_match, NULL,
    CSS &error);

/* The lookup did not succeed. Defer if it deferred unless optional is set;
otherwise this director declines to handle the address. */

if (*textptr == NULL)
  {
  if (search_find_defer && !ob->optional)
    {
    addr->message = string_sprintf("search_find deferred: %s", error);
    return DEFER;
    }

  DEBUG(2) debug_printf("%s director declined for %s: %s\n", dblock->name,
    addr->local_part, error);
  return DECLINE;
  }

/* If the expand option is set, pass the text through the string expander. */

if (ob->expand)
  {
  uschar *newtext = US expand_string(CS *textptr);
  if (newtext == NULL)
    {
    addr->message = string_sprintf("failed to expand \"%s\" "
      "(generated from local part %s): %s", *textptr,
      addr->local_part, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  *textptr = newtext;
  }

return OK;
}

/* End of df_lookup.c */
