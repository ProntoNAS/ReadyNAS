/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "dfunctions.h"
#include "smartuser.h"


/* Options specific to the smartuser director. */

optionlist smartuser_director_options[] = {
  { "directory2_transport",opt_transportptr,
      (void *)(offsetof(smartuser_director_options_block, directory2_transport)) },
  { "directory_transport",opt_transportptr,
      (void *)(offsetof(smartuser_director_options_block, directory_transport)) },
  { "file_transport",     opt_transportptr,
      (void *)(offsetof(smartuser_director_options_block, file_transport)) },
  { "forbid_file",        opt_bool,
      (void *)(offsetof(smartuser_director_options_block, forbid_file)) },
  { "forbid_pipe",        opt_bool,
      (void *)(offsetof(smartuser_director_options_block, forbid_pipe)) },
  { "hide_child_in_errmsg", opt_bool,
      (void *)offsetof(smartuser_director_options_block, hide_child_in_errmsg) },
  { "new_address", opt_stringptr,
      (void *)(offsetof(smartuser_director_options_block, new_address)) },
  { "panic_expansion_fail", opt_bool,
      (void *)(offsetof(smartuser_director_options_block, panic_expansion_fail)) },
  { "pipe_transport",     opt_transportptr,
      (void *)(offsetof(smartuser_director_options_block, pipe_transport)) },
  { "qualify_preserve_domain", opt_bool,
      (void *)(offsetof(smartuser_director_options_block, qualify_preserve_domain)) },
  { "rewrite",              opt_bool,
      (void *)(offsetof(smartuser_director_options_block, rewrite)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int smartuser_director_options_count =
  sizeof(smartuser_director_options)/sizeof(optionlist);

/* Default private options block for the smartuser director. The common options
have to be present so it can use the common functions, but not all of these
values are changeable by an option. */

smartuser_director_options_block smartuser_director_option_defaults = {
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
  TRUE,     /* forbid_reply */
  FALSE,    /* include_domain */
  FALSE,    /* hide_child_in_errmsg */
  FALSE,    /* one_time */
  FALSE,    /* optional */
  /********************************************/
  NULL,     /* new_address */
  TRUE,     /* panic_expansion_fail */
  FALSE,    /* qualify_preserve_domain */
  TRUE      /* rewrite */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up.

Argument: points to the director instance block
Returns:  nothing
*/

void
smartuser_director_init(director_instance *dblock)
{
smartuser_director_options_block *ob =
  (smartuser_director_options_block *)(dblock->options_block);

/* If no transport is specified, a new user *must* be given, except
when verifying only. */

if (dblock->transport == NULL &&
    dblock->expand_transport == NULL &&
    !dblock->verify_only)
  {
  if (ob->new_address == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "neither transport nor new_address specified", dblock->name);
  }
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface description. The smartuser director accepts
all local parts that are passed to it. */

int
smartuser_director_entry(
  director_instance *dblock,      /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if it's local */
  address_item **addr_remote,     /* add it to this if it's remote */
  address_item **addr_new,        /* put new addresses on here */
  address_item **addr_succeed,    /* put finished with addresses here */
  BOOL verify)                    /* TRUE when verifying */
{
smartuser_director_options_block *ob =
  (smartuser_director_options_block *)(dblock->options_block);
char *errors_to;
char *remove_headers;
char *raw_new_address, *error;
header_line *extra_headers;
address_item *generated = NULL;
transport_instance *transport;
ugid_block ugid;
int yield = OK;
int extracted, rc;

/* Perform file existence and sender verification checks. */

rc = direct_check_fsc(dblock, addr);
if (rc != OK) return rc;

DEBUG(2) debug_printf("%s director called for %s\n", dblock->name, addr->orig);

/* Sort out the fixed or dynamic uid/gid */

if (!direct_get_ugid(dblock, "director", addr, &ugid)) return ERROR;

/* Set up the errors address, if any. */

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


/* Handle cases where no new address is set. If there is no transport, we must
be verifying, because that configuration is allowed by the initialization
function only when verify_only is set. The director is being used for some
special kind of verification, based on generic conditions, and reaching this
point indicates success. If there is a transport, we put the errors and header
stuff into the current address and queue it for the transport. */

if (ob->new_address == NULL)
  {
  if (transport == NULL) return OK;    /* The verify_only case */
/*  addr->errors_address = errors_to;  */
  addr->extra_headers = extra_headers;
  addr->remove_headers = remove_headers;
  addr->transport = transport;
  addr->home_dir = dblock->home_directory;
  addr->current_dir = dblock->current_directory;
  return route_queue(addr, addr_local, addr_remote, (router_instance *)dblock,
    &ugid, "director")? OK : ERROR;
  }


/* A new_address string was specified. First, expand it. Expansion failure is
either a panic or a director decline, according to an option. A forced fail, or
search defer in the expansion is always soft. */

raw_new_address = expand_string(ob->new_address);
if (raw_new_address == NULL)
  {
  if (search_find_defer)
    {
    addr->message = string_sprintf("expansion of \"%s\" failed: %s",
      ob->new_address, expand_string_message);
    return DEFER;
    }
  else if (ob->panic_expansion_fail && !expand_string_forcedfail)
    {
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Expansion of %s in %s director "
      "failed: %s", ob->new_address, dblock->name, expand_string_message);
    }
  else
    {
    DEBUG(4) debug_printf("expansion of \"%s\" failed: %s\n",
      ob->new_address, expand_string_message);
    DEBUG(2) debug_printf("%s director declined for %s\n", dblock->name,
      addr->local_part);
    return DECLINE;
    }
  }


/* If there is a transport, there must be exactly one, fully qualified address.
We make it the child of the old address, and direct it to that transport. */

if (transport != NULL)
  {
  int start, end, domain;
  char *errmess = NULL;
  char *new_address = parse_extract_address(raw_new_address, &errmess, &start,
    &end, &domain, FALSE);

  if (new_address == NULL || domain == 0)
    {
    addr->basic_errno = ERRNO_BADADDRESS2;
    if (errmess == NULL) errmess = "missing domain";
    addr->message =
      string_sprintf("<%s> is a bad new address: %s", raw_new_address, errmess);
    addr->errors_address = errors_address;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }

  /* Rewrite new address if configured */

  if (ob->rewrite)
    new_address = rewrite_address(new_address, TRUE, FALSE,
      global_rewrite_rules, rewrite_existflags);
  domain = strchr(new_address, '@') - new_address + 1;

  /* Create child, making a copy of the old address, changing the domain and
  local parts except when verifying. */

  generated = store_get(sizeof(address_item));
  *generated = *addr;
  if (!verify)
    {
    generated->local_part = string_copyn(new_address, domain-1);
    generated->domain = new_address + domain;
    }

  /* Put in new values, and connect to parent. */

  generated->onetime_parent = NULL;            /* Mustn't have this */
  generated->errors_address = errors_to;
  generated->extra_headers = extra_headers;
  generated->remove_headers = remove_headers;
  generated->start_director = dblock->new;
  generated->director = dblock;
  generated->transport = transport;
  generated->home_dir = dblock->home_directory;
  generated->current_dir = dblock->current_directory;
  generated->parent = addr;
  addr->child_count++;

  DEBUG(2) debug_printf("  generated new address: %s%s%s%s\n",
    new_address,
    (generated->errors_address != NULL)? " (errors to " : "",
    (generated->errors_address != NULL)? generated->errors_address : "",
    (generated->errors_address != NULL)? ")" : "");

  yield = route_queue(generated, addr_local, addr_remote,
    (router_instance *)dblock, &ugid, "director")? OK : ERROR;
  }


/* When there is no transport, use the common extraction function to extract
one or more new addresses from the expanded string, as for forwarding and
aliasing directors. If qualify_preserve_domain is set, we temporarily reset the
value of qualify_domain_recipient to the current domain, so that any
unqualified addresses get qualified with the same domain as the incoming
address. */

else
  {
  char *save_qualify_domain_recipient = qualify_domain_recipient;

  if (ob->qualify_preserve_domain) qualify_domain_recipient = addr->domain;
  extracted = parse_extract_addresses(raw_new_address, addr->domain, &generated,
    &error,
    FALSE,        /* No addresses is an error */
    TRUE,         /* Allow :blackhole: :defer: :unknown: and :fail: */
    ob->rewrite,  /* Rewrite addresses if required */
    "*",          /* No :include: files permitted */
    NULL          /* Syntax errors are serious */
    );
  qualify_domain_recipient = save_qualify_domain_recipient;

  /* Cases of extraction failure are also handled by a common function. */

  if (extracted != 0) return df_extracted(extracted, dblock, addr, error,
    verify, FALSE, "smartuser new address");

  /* Since there is no transport, this is an aliasing operation, and we can use
  the common function to deal with the generated addresses. */

  df_generated(dblock, addr_new, addr, generated, errors_to, extra_headers,
    remove_headers, &ugid, NULL, NULL);

  /* If no children were generated for this address, but no error was given, it
  means that there was just a :blackhole: entry. Log something. */

  if (addr->child_count <= 0 && !verify && !address_test_mode)
    log_write(0, LOG_MAIN, "=> :blackhole: <%s> D=%s", addr->orig,
      dblock->name);
  }

/* Put the original address onto the succeed queue. This ensures that any
retry item that it acquires gets processed. */

addr->next = *addr_succeed;
*addr_succeed = addr;

return yield;
}

/* End of directors/smartuser.c */
