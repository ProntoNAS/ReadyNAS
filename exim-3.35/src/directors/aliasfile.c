/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "dfunctions.h"
#include "aliasfile.h"



/* Options specific to the aliasfile director. */

optionlist aliasfile_director_options[] = {
  { "*expand_directory2_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(aliasfile_director_options_block, expand_directory2_transport) },
  { "*expand_directory_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(aliasfile_director_options_block, expand_directory_transport) },
  { "*expand_file_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(aliasfile_director_options_block, expand_file_transport) },
  { "*expand_pipe_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(aliasfile_director_options_block, expand_pipe_transport) },
  { "*partial_match", opt_int | opt_hidden,
      (void *)(offsetof(aliasfile_director_options_block, partial_match)) },
  { "check_ancestor",     opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, check_ancestor)) },
  { "directory2_transport",opt_transportptr,
      (void *)(offsetof(aliasfile_director_options_block, directory2_transport)) },
  { "directory_transport",opt_transportptr,
      (void *)(offsetof(aliasfile_director_options_block, directory_transport)) },
  { "expand",             opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, expand)) },
  { "file",               opt_stringptr,
      (void *)(offsetof(aliasfile_director_options_block, file)) },
  { "file_transport",     opt_transportptr,
      (void *)(offsetof(aliasfile_director_options_block, file_transport)) },
  { "forbid_file",        opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, forbid_file)) },
  { "forbid_include",     opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, forbid_include)) },
  { "forbid_pipe",        opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, forbid_pipe)) },
  { "forbid_special",     opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, forbid_special)) },
  { "freeze_missing_include", opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, freeze_missing_include)) },
  { "hide_child_in_errmsg", opt_bool,
      (void *)offsetof(aliasfile_director_options_block, hide_child_in_errmsg) },
  { "include_domain",     opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, include_domain)) },
  { "modemask",           opt_octint,
      (void *)(offsetof(aliasfile_director_options_block, modemask)) },
  { "one_time",           opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, one_time)) },
  { "optional",           opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, optional)) },
  { "owners",             opt_uidlist,
      (void *)(offsetof(aliasfile_director_options_block, owners)) },
  { "owngroups",          opt_gidlist,
      (void *)(offsetof(aliasfile_director_options_block, owngroups)) },
  { "pipe_transport",     opt_transportptr,
      (void *)(offsetof(aliasfile_director_options_block, pipe_transport)) },
  { "qualify_preserve_domain", opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, qualify_preserve_domain)) },
  { "queries",            opt_stringptr,
      (void *)(offsetof(aliasfile_director_options_block, queries)) },
  { "query",              opt_stringptr,
      (void *)(offsetof(aliasfile_director_options_block, query)) },
  { "rewrite",            opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, rewrite)) },
  { "search_type",        opt_searchtype,
      (void *)(offsetof(aliasfile_director_options_block, search_type)) },
  { "skip_syntax_errors", opt_bool,
      (void *)(offsetof(aliasfile_director_options_block, skip_syntax_errors)) },
  { "syntax_errors_text", opt_stringptr,
      (void *)(offsetof(aliasfile_director_options_block, syntax_errors_text)) },
  { "syntax_errors_to",   opt_stringptr,
      (void *)(offsetof(aliasfile_director_options_block, syntax_errors_to)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int aliasfile_director_options_count =
  sizeof(aliasfile_director_options)/sizeof(optionlist);

/* Default private options block for the aliasfile director. */

aliasfile_director_options_block aliasfile_director_option_defaults = {
  /************** Common block ****************/
  NULL,     /* directory_transport */
  NULL,     /* expand_directory_transport */
  NULL,     /* directory2_transport */
  NULL,     /* expand_directory2_transport */
  NULL,     /* file_transport */
  NULL,     /* expand_file_transport */
  NULL,     /* pipe_transport */
  NULL,     /* expand_pipe_transport */
  NULL,     /* reply_transport */
  NULL,     /* expand_reply_transport */
  NULL,     /* file */
  NULL,     /* queries */
  NULL,     /* query */
  022,      /* modemask */
  NULL,     /* owners */
  NULL,     /* owngroups */
  -1,       /* partial_match */
  -1,       /* search_type */
  FALSE,    /* check_ancestor */
  FALSE,    /* check_local_user */
  FALSE,    /* expand */
  FALSE,    /* forbid_file */
  FALSE,    /* forbid_pipe */
  FALSE,    /* forbid_reply */
  FALSE,    /* include_domain */
  FALSE,    /* hide_child_in_errmsg */
  FALSE,    /* one_time */
  FALSE,    /* optional */
  /********************************************/
  NULL,     /* syntax_errors_to */
  NULL,     /* syntax_errors_text */
  FALSE,    /* forbid_include */
  FALSE,    /* forbid_special */
  FALSE,    /* qualify_preserve_domain */
  TRUE,     /* freeze_missing_include */
  TRUE,     /* rewrite */
  FALSE     /* skip_syntax_errors */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up.

Argument:
  dblock       a pointer to the director instance block

Returns:       nothing
*/

void
aliasfile_director_init(director_instance *dblock)
{
aliasfile_director_options_block *ob =
  (aliasfile_director_options_block *)(dblock->options_block);

/* A search type is mandatory */

if (ob->search_type < 0)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "a search type option is required", dblock->name);

/* For single-key+file search types, a file name is mandatory; for other types
a query is mandatory. An absolute file name is mandatory for lsearch and dbm;
but can't check for absoluteness if the name is being looked up. */

if (!mac_islookup(ob->search_type, lookup_querystyle))
  {
  if (ob->query != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "\"query\" specified for a single-key search type", dblock->name);

  if (ob->file == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "no file name specified", dblock->name);

  if (mac_islookup(ob->search_type, lookup_absfile) &&
       ob->file[0] != '/' && ob->file[0] != '$')
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "an absolute file path name is required for %s",
        dblock->name, lookup_list[ob->search_type].name);
  }

/* Non-single-key search type; either a single query or a list of queries must
be specified. */

else
  {
  if (ob->file != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "\"file\" specified for a non-single-key search type", dblock->name);

  if (ob->query == NULL && ob->queries == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "no query specified for %s search", dblock->name,
      lookup_list[ob->search_type].name);

  if (ob->query != NULL && ob->queries != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "only one of \"query\" or \"queries\" may be given", dblock->name);
  }

/* Onetime aliases can only be real addresses */

if (ob->one_time && (!ob->forbid_pipe || !ob->forbid_file))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "forbid_pipe and forbid_file must be set when one_time is set",
      dblock->name);
}




/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface description */

int
aliasfile_director_entry(
  director_instance *dblock,      /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if it's local */
  address_item **addr_remote,     /* add it to this if it's remote */
  address_item **addr_new,        /* put new addresses on here */
  address_item **addr_succeed,    /* put the old one here on success */
  BOOL verify)                    /* true if verifying */
{
aliasfile_director_options_block *ob =
  (aliasfile_director_options_block *)(dblock->options_block);
address_item *generated = NULL;
char *save_qualify_domain_recipient = qualify_domain_recipient;
char *aliastext = NULL;
char *errors_to;
char *filename = NULL;
char *error;
header_line *extra_headers;
char *remove_headers;
error_block *eblock = NULL;
transport_instance *transport;
ugid_block ugid;
void *reset_point;
int yield = OK;
int extracted, rc;

/* Perform file existence and sender verification checks. */

rc = direct_check_fsc(dblock, addr);
if (rc != OK) return rc;

/* There is now a common function for doing the lookup, shared with
forwardfile - one of these days the two directors will coalesce. */

yield = df_lookup(dblock, addr, USS &aliastext, (uschar **)&filename);
if (yield != OK) return yield;

/* Sort out the fixed or dynamic uid/gid */

if (!direct_get_ugid(dblock, "director", addr, &ugid)) return ERROR;

/* Set up the errors address for the children of this address. Preserve it in
the parent address - this is used if a transport exists, and also after
"unseen" handling of any of the children. */

rc = direct_get_errors_address(addr, dblock, "director", verify, &errors_to);
if (rc != OK) return rc;
addr->errors_address = errors_to;

/* Set up the additional and removeable headers for the children of this
address. */

rc = direct_get_munge_headers(addr, dblock, "director", &extra_headers,
  &remove_headers);
if (rc != OK) return rc;

/* Get the transport for this director, if any. It might be expanded, or it
might be unset. */

if (!route_get_transport(dblock->transport, dblock->expand_transport,
  addr, &transport, dblock->name, NULL)) return ERROR;

/* If there is a transport specified for the director, then set up this
address to use that transport. Ignore the alias text. */

if (transport != NULL)
  {
  addr->transport = transport;
/*  addr->errors_address = errors_to;  */
  addr->home_dir = dblock->home_directory;
  addr->current_dir = dblock->current_directory;
  addr->extra_headers = extra_headers;
  addr->remove_headers = remove_headers;
  return route_queue(addr, addr_local, addr_remote, (router_instance *)dblock,
    &ugid, "director")? OK : ERROR;
  }

/* If qualify_preserve_domain is set, we temporarily reset the value of
qualify_domain_recipient to the current domain, so that any unqualified
addresses get qualified with the same domain as the incoming address. */

if (ob->qualify_preserve_domain) qualify_domain_recipient = addr->domain;

/* There is a common function for use by forwarding and aliasing directors that
extracts a list of addresses from a text string. If the last argument is NULL,
syntax errors are hard; otherwise they just get listed.

It is possible that a fair amount of store might be used, especially if there
is an :include: file. Therefore, switch to the temporary pool for the duration.
Note that addresses built by the extractor are forced into the MAIN pool. */

store_pool = POOL_TEMP;
reset_point = store_get(0);

extracted = parse_extract_addresses(aliastext, addr->domain, &generated,
  &error,
  FALSE,                   /* No addresses is an error */
  !ob->forbid_special,     /* Controls :blackhole: etc. */
  ob->rewrite,             /* Rewrite if required */
  ob->forbid_include? "*" : NULL,  /* Restriction on :include: files */
  ob->skip_syntax_errors? &eblock : NULL);

/* Restore the qualify domain, and reset to the main pool for the generation of
any error texts. */

qualify_domain_recipient = save_qualify_domain_recipient;
store_pool = POOL_MAIN;

/* A common function handles cases of extraction failure. */

if (extracted != 0) yield = df_extracted(extracted, dblock, addr, error,
  verify, ob->freeze_missing_include, (filename != NULL)?
    string_sprintf("alias file %s", filename) : "alias lookup");

/* If skip_syntax_errors was set and there were syntax errors in the list,
error messages will be present in eblock. Log them, and sent them off in
a mail message if so configured. A common function is used by aliasfile
and forwardfile. */

if (yield == OK && eblock != NULL &&
    !moan_skipped_syntax_errors(
      dblock->name,                          /* For message content */
      filename,                              /* Ditto */
      eblock,                                /* Ditto */
      (verify || address_test_mode)?
        NULL : ob->syntax_errors_to,         /* Who to mail */
      generated != NULL,                     /* True if not all failed */
      ob->syntax_errors_text))               /* Custom message */
  {
  addr->special_action = SPECIAL_FREEZE;
  yield = ERROR;
  }

/* We are now through with the store in the temporary pool; if there was an
error, we are done. */

store_pool = POOL_TEMP;
store_reset(reset_point);
store_pool = POOL_MAIN;

if (yield != OK) return yield;

/* Set up the new addresses and add them to the addr_new chain. A common
function handles this. */

df_generated(dblock, addr_new, addr, generated, errors_to, extra_headers,
  remove_headers, &ugid, NULL, NULL);

/* If no children were generated for this address, but no error was given,
it means that there was just a :blackhole: entry in the alias file. Log
something. */

if (deliver_firsttime && addr->child_count <= 0 && !verify && !address_test_mode)
  log_write(0, LOG_MAIN, "=> :blackhole: <%s> D=%s", addr->orig, dblock->name);

/* Put the original address onto the succeed queue. This ensures that any
retry item that it acquires gets processed. */

addr->next = *addr_succeed;
*addr_succeed = addr;

return OK;
}

/* End of director/aliasfile.c */
