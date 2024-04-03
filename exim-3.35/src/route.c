/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions concerned with routing, and the list of generic router options. */


#include "exim.h"



/* Generic options for routers, all of which live inside router_instance
data blocks and which therefore have the opt_public flag set. */

optionlist optionlist_routers[] = {
  { "*expand_group",      opt_stringptr | opt_hidden | opt_public,
                 (void *)(offsetof(router_instance, expand_gid)) },
  { "*expand_transport",  opt_stringptr|opt_public|opt_hidden,
                 (void *)offsetof(router_instance, expand_transport) },
  { "*expand_user",       opt_stringptr | opt_hidden | opt_public,
                 (void *)(offsetof(router_instance, expand_uid)) },
  { "*set_group",         opt_bool | opt_hidden | opt_public,
                 (void *)(offsetof(router_instance, gid_set)) },
  { "*set_user",          opt_bool | opt_hidden | opt_public,
                 (void *)(offsetof(router_instance, uid_set)) },
  { "condition",          opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, condition) },
  { "debug_print",        opt_stringptr | opt_public,
                 (void *)offsetof(router_instance, debug_string) },
  { "domains",            opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, domains) },
  { "driver",             opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, driver_name) },
  { "errors_to",          opt_stringptr|opt_public,
                 (void *)(offsetof(router_instance, errors_to)) },
  { "fail_verify",        opt_bool_verify|opt_hidden|opt_public,
                 (void *)offsetof(router_instance, fail_verify_sender) },
  { "fail_verify_recipient", opt_bool|opt_public,
                 (void *)offsetof(router_instance, fail_verify_recipient) },
  { "fail_verify_sender", opt_bool|opt_public,
                 (void *)offsetof(router_instance, fail_verify_sender) },
  { "fallback_hosts",     opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, fallback_hosts) },
  { "group",              opt_expand_gid | opt_public,
                 (void *)(offsetof(router_instance, gid)) },
  { "headers_add",        opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, extra_headers) },
  { "headers_remove",     opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, remove_headers) },
  { "ignore_target_hosts",       opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, ignore_target_hosts) },
  { "initgroups",         opt_bool | opt_public,
                 (void *)(offsetof(router_instance, initgroups)) },
  { "local_parts",        opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, local_parts) },
  { "more",               opt_bool|opt_public,
                 (void *)offsetof(router_instance, more) },
  { "pass_on_timeout",    opt_bool|opt_public,
                 (void *)offsetof(router_instance, pass_on_timeout) },
  { "require_files",      opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, require_files) },
  { "self",               opt_stringptr|opt_public,
                 (void *)(offsetof(router_instance, self)) },
  { "senders",            opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, senders) },
  #ifdef SUPPORT_TRANSLATE_IP_ADDRESS
  { "translate_ip_address", opt_stringptr|opt_public,
                 (void *)offsetof(router_instance, translate_ip_address) },
  #endif
  { "transport",          opt_transportptr|opt_public,
                 (void *)offsetof(router_instance, transport) },
  { "unseen",             opt_bool|opt_public,
                 (void *)offsetof(router_instance, unseen) },
  { "user",               opt_expand_uid | opt_public,
                 (void *)(offsetof(router_instance, uid)) },
  { "verify",             opt_bool_verify|opt_hidden|opt_public,
                 (void *)offsetof(router_instance, verify_sender) },
  { "verify_only",        opt_bool|opt_public,
                 (void *)offsetof(router_instance, verify_only) },
  { "verify_recipient",   opt_bool|opt_public,
                 (void *)offsetof(router_instance, verify_recipient) },
  { "verify_sender",      opt_bool|opt_public,
                 (void *)offsetof(router_instance, verify_sender) }
};

int optionlist_routers_size =
  sizeof(optionlist_routers)/sizeof(optionlist);


/*************************************************
*             Initialize router list             *
*************************************************/

/* Read the routers configuration file, and set up a chain of router
instances according to its contents. Each router has generic options and may
also have its own private options. This function is only ever called when
routers == NULL. We use generic code in readconf to do the work. */

void
route_init(void)
{
router_instance *r;

readconf_driver_init("router",
  (driver_instance **)(&routers),     /* chain anchor */
  (driver_info *)routers_available,   /* available drivers */
  sizeof(router_info),                /* size of info blocks */
  &router_defaults,                   /* default values for generic options */
  sizeof(router_instance),            /* size of instance block */
  optionlist_routers,                 /* generic options */
  optionlist_routers_size);

/* A router may not have more=FALSE and unseen=TRUE. The "self" option
needs to be decoded into a code value and possibly a new domain string
and a rewrite boolean. */

for (r = routers; r != NULL; r = r->next)
  {
  char *s = r->self;

  if (r->unseen && !r->more )
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "the combination of \"unseen\" and \"no_more\" is not permitted on "
      "a router,\n  but was set for the %s router", r->name);

  if      (strcmp(s, "freeze") == 0)    r->self_code = self_freeze;
  else if (strcmp(s, "defer") == 0)     r->self_code = self_defer;
  else if (strcmp(s, "send") == 0)      r->self_code = self_send;
  else if (strcmp(s, "pass") == 0)      r->self_code = self_pass;
  else if (strcmp(s, "fail") == 0)      r->self_code = self_forcefail;
  else if (strcmp(s, "local") == 0)     r->self_code = self_local;
  else if (strncmp(s, "reroute:", 8) == 0)
    {
    s += 8;
    while (isspace((uschar)*s)) s++;
    if (strncmp(s, "rewrite:", 8) == 0)
      {
      r->self_rewrite = TRUE;
      s += 8;
      while (isspace((uschar)*s)) s++;
      }
    r->self = s;
    r->self_code = self_reroute;
    }

  /* The next two are obsolete - retained for compatibility */
  else if (strcmp(s, "fail_soft") == 0) r->self_code = self_pass;
  else if (strcmp(s, "fail_hard") == 0) r->self_code = self_forcefail;

  else log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
      "%s is not valid for the self option", r->name, s);

  /* Build a host list if fallback hosts is set */

  host_build_hostlist(&(r->fallback_hostlist), r->fallback_hosts, FALSE);
  }
}



/*************************************************
*             Tidy up after routing              *
*************************************************/

/* Routers are entitled to keep hold of certain resources in their instance
blocks so as to save setting them up each time. An example is an open file.
Such routers must provide a tidyup entry point which is called when all routing
is finished, via this function. */

void
route_tidyup(void)
{
router_instance *r;
for (r = routers; r != NULL; r = r->next)
  if (r->info->tidyup != NULL) (r->info->tidyup)(r);
}




/*************************************************
*        Queue address for transport             *
*************************************************/

/* This function is called to put an address onto the local or remote transport
queue, as appropriate. The same action is required by some (but not all) of the
directors, and so they use this too. The fields in the router block that are
used by this function have identical counterparts in the director block, to
enable this to work. The generic uid/gid options are inspected and put into the
address if they are set. For a remote transport, if there are fallback hosts,
they are added to the address.

When the driver is for verifying only, a transport need not be set, in which
case it doesn't actually matter which queue the address gets put on.

Arguments:
  addr          the address, with the transport field set (if not verify only)
  addr_local    the local transport chain
  addr_remote   the remote transport chain,
  rblock        the router block
  ugidptr       pointer to ugid block, or NULL if not yet set up
  type          "director" or "router"

Returns:        FALSE on error; the only case is failing to get a uid/gid
*/

BOOL
route_queue(address_item *addr, address_item **addr_local,
  address_item **addr_remote, router_instance *rblock, ugid_block *ugidptr,
  char *type)
{
addr->domain_data = domain_data;            /* Save these values for use */
addr->local_part_data = local_part_data;    /* in the transport */

if (addr->transport != NULL && addr->transport->info->local)
  {
  ugid_block ugid;
  if (ugidptr == NULL)
    {
    if (!direct_get_ugid((director_instance *)rblock, type, addr, &ugid))
      return FALSE;
    ugidptr = &ugid;
    }
  direct_set_ugid(addr, ugidptr);
  addr->next = *addr_local;
  *addr_local = addr;
  }
else
  {
  addr->next = *addr_remote;
  *addr_remote = addr;
  addr->fallback_hosts = rblock->fallback_hostlist;
  }

DEBUG(2)
  {
  debug_printf("queued for %s transport: local_part=%s domain=%s\n"
    "  errors_to=%s\n",
    (addr->transport == NULL)? "<unset>" : addr->transport->name,
    addr->local_part, addr->domain, addr->errors_address);
  debug_printf("  domain_data=%s local_part_data=%s\n", domain_data,
    local_part_data);
  }

return TRUE;
}




/*************************************************
*       Get transport for a driver               *
*************************************************/

/* If expand_transport is set on a driver, it must be expanded each time and
used as a transport name. This action is identical for both routers and
directors, and this function is called from both of them.

Arguments:
  tp            the fixed transport from the driver (may be NULL, but see below)
  expand_tp     the expansion string (may be NULL)
  addr          the address being processed
  tpptr         where to put the answer
  driver_name   for error messages
  require_name  if not NULL, fixed transport may not be NULL; this text
                used in the error message

Returns:        TRUE if a transport has been placed in tpptr; this may be NULL
                if tp == expand_tp == require_name == NULL.
                FALSE if there's a problem, in which case
                addr->message contains a message, and addr->basic_errno has
                ERRNO_BADTRANSPORT set in it.
*/

BOOL
route_get_transport(transport_instance *tp, char *expand_tp, address_item *addr,
  transport_instance **tpptr, char *driver_name, char *require_name)
{
char *ss;

/* If there's nothing to expand, use the fixed transport. For some calls, it is
required to be set. */

if (expand_tp == NULL)
  {
  *tpptr = tp;
  if (tp != NULL || require_name == NULL) return TRUE;
  addr->basic_errno = ERRNO_BADTRANSPORT;
  addr->message = string_sprintf("%s unset in %s driver", require_name,
    driver_name);
  return FALSE;
  }

ss = expand_string(expand_tp);
if (ss == NULL)
  {
  addr->basic_errno = ERRNO_BADTRANSPORT;
  addr->message = string_sprintf("failed to expand dynamic transport "
    "\"%s\" in %s driver: %s", expand_tp, driver_name, expand_string_message);
  return FALSE;
  }

for (tp = transports; tp != NULL; tp = tp->next)
  {
  if (strcmp(tp->name, ss) == 0)
    {
    DEBUG(9) debug_printf("dynamic transport \"%s\" set\n", ss);
    *tpptr = tp;
    return TRUE;
    }
  }

addr->basic_errno = ERRNO_BADTRANSPORT;
addr->message = string_sprintf("dynamic transport \"%s\" not found "
  "in %s driver", ss, driver_name);
return FALSE;
}



/*************************************************
*             Check for driver skipping          *
*************************************************/

/* This function performs various checks to see whether a driver should be
skipped. Many of the checks are identical for both routers and directors. The
relevant data is arranged to be in the same place in the options blocks for
both types of driver so that this function can be used for both types of
driver for the common cases. Hence it is globally accessible.

Arguments:
  dr           pointer to router or director instance block
  local_part   local part of the address being handled
  domain       ditto domain
  verify       the verification type
  pmore        address of the "more" flag
  driver_type  either "router" or "director"

Returns:       TRUE if this driver is to be skipped, but if search_find_defer
               is set, it means a domains or local parts lookup deferred
*/


BOOL
route_skip_driver(router_instance *dr, char *local_part, char *domain,
  int verify, BOOL *pmore, char *driver_type)
{
/* Variables to hold data from lookup of a domain or local part. */

domain_data = NULL;
local_part_data = NULL;

/* Unset this in case there aren't any actual lookups. */

search_find_defer = FALSE;

/* Skip this driver if not verifying and it has verify_only set */

if ((verify == v_none || verify == v_expn) && dr->verify_only)
  {
  DEBUG(9) debug_printf("%s %s skipped: verify_only set\n",
    dr->name, driver_type);
  return TRUE;
  }

/* Skip this driver if domain mismatch. Set $key not to expand, so the user
doesn't have to remember the backslash. */

if (dr->domains != NULL)
  {
  char *domains;
  lookup_key = "$key";
  domains = expand_string_panic(dr->domains, dr->name, driver_type);
  lookup_key = NULL;
  if (!match_isinlist(domain, &domains, TRUE, TRUE, &domain_data))
    {
    DEBUG(9) debug_printf("%s %s skipped: domain mismatch\n",
      dr->name, driver_type);
    return TRUE;
    }
  }

/* Skip this driver if local part mismatch. Set $key not to expand, so the
user doesn't have to remember the backslash. */

if (dr->local_parts != NULL)
  {
  char *local_parts;
  lookup_key = "$key";
  local_parts = expand_string_panic(dr->local_parts, dr->name, driver_type);
  lookup_key = NULL;
  if (!match_isinlist(local_part, &local_parts, FALSE,
        (driver_type[0] == 'd' && locally_caseless),
        &local_part_data))
    {
    DEBUG(9) debug_printf("%s %s skipped: local part mismatch\n",
      dr->name, driver_type);
    return TRUE;
    }
  }

/* If this driver's "more" flag is FALSE, arrange that no subsequent
drivers are called. */

*pmore = dr->more;
DEBUG(9)
  {
  if (!dr->more) debug_printf("%s %s has more set FALSE\n", dr->name,
    driver_type);
  }

/* Skip this driver if verifying and it hasn't got the appropriate verify flag
set. */

if ((verify == v_sender && !dr->verify_sender) ||
    (verify == v_recipient && !dr->verify_recipient))
  {
  DEBUG(9) debug_printf("%s %s skipped: verify %d %d %d\n",
    dr->name, driver_type, verify, dr->verify_sender, dr->verify_recipient);
  return TRUE;
  }

/* Nothing in this function calls for skipping the driver; specific code for
routers and directors may yet do so later. */

return FALSE;
}



/*************************************************
*            Handle an unseen routing            *
*************************************************/

/* This function is called when an address is routed by a router with "unseen"
set, or directed by a director with "unseen" set. It must make a clone of the
address, for handling by subsequent drivers. The clone will fall through to the
next router or director, by the anti-looping rules. The original address must
be replaced by an invented "parent" which has the routed/directed address plus
the clone as its children. This is necessary in case the address is at the top
level - we don't want to mark it complete until both deliveries have been done.

A new unique field must be made, so that the record of the delivery isn't a
record of the original address, and checking for already delivered has
therefore to be done here. If the delivery has happened, then take the base
address off whichever delivery queue it is on - it will always be the top item.

Arguments:
  name         router or director name
  addr         address that was routed/directed
  addr_local   chain of local-delivery addresses
  addr_remote  chain of remote-delivery addresses
  addr_new     chain for newly created addresses

Returns:       nothing
*/

void
route_unseen(char *name, address_item *addr, address_item **addr_local,
  address_item **addr_remote, address_item **addr_new)
{
address_item *parent = deliver_make_addr(addr->orig, TRUE);
address_item *new = deliver_make_addr(addr->orig, TRUE);


/* The invented parent is a copy that replaces the original; note that
this copies its parent pointer. It has two children, and its errors_address is
from the original address' parent, if present, otherwise unset. */

*parent = *addr;
parent->child_count = 2;
parent->errors_address =
  (addr->parent == NULL)? NULL : addr->parent->errors_address;

/* The routed/directed address gets a new parent. */

addr->parent = parent;

/* Set up the cloned address and put it on the new address queue, setting its
errors address to ignore any errors_to setting on the unseen driver. */

new->parent = parent;
new->errors_address = parent->errors_address;
copyflag(new, addr, af_ignore_error|af_forced_local);
new->next = *addr_new;
*addr_new = new;

DEBUG(2) debug_printf("\"unseen\" set: replicated %s\n", addr->orig);

/* Make a new unique field, to distinguish from the normal one. */

addr->unique = string_sprintf("%s/%s", addr->unique, name);

/* If the address has been directed or routed to a transport, see if it
was previously delivered. If so, we take it off the relevant queue so that
it isn't delivered again. Otherwise, it was an alias or something, and the
addresses it generated are handled in the normal way. */

if (addr->transport != NULL &&
    tree_search_addr(tree_nonrecipients, addr->unique, FALSE) != NULL)
  {
  DEBUG(2)
    debug_printf("\"unseen\" delivery previously done - discarded\n");
  parent->child_count--;
  if (*addr_remote == addr) *addr_remote = addr->next;
  if (*addr_local == addr) *addr_local = addr->next;
  }
}



/*************************************************
*                 Route one address              *
*************************************************/

/* This function is passed in one address item, for processing by the routers.
It has been determined that the address is (apparently) not for one of the
local domains. The verify flag is set if this is being called for verification
rather than delivery. If the router doesn't have its "verify" flag set, it is
skipped.

Arguments:
  addr         address to route
  addr_local   chain of local-delivery addresses
  addr_remote  chain of remote-delivery addresses
  addr_new     chain for newly created addresses
  verify       v_none if not verifying
               v_sender if verifying a sender address
               v_recipient if verifying a recipient address

Returns:       OK      => address successfully routed
               FAIL    => address could not be routed
               DEFER   => some temporary problem
               ISLOCAL => address routed to the local host
               ERROR   => some major internal or configuration failure
*/

int
route_address(address_item *addr, address_item **addr_local,
  address_item **addr_remote, address_item **addr_new, int verify)
{
BOOL more = TRUE;
router_instance *r;

DEBUG(9) debug_printf(">>>>>>>>>>>>>>>>>>>>>>>>\n"
  "routing %s, domain %s\n", addr->orig, addr->domain);

/* Set the domain used by the routers; a router is permitted to change this and
yield PASS, causing subsequent routers to use the new name, without changing
addr->domain, which is the envelope address. Equally, a router is also
permitted to change the envelope address. */

addr->route_domain = addr->domain;

/* Loop through all router instances. */

for (r = routers; more && r != NULL; r = r->next)
  {
  char *old_route_domain = addr->route_domain;
  char *error;
  address_item *parent;
  BOOL loop_detected = FALSE;
  int yield = FALSE;
  int rc;

  /* Check the conditions that are common to both routers and directors. We
  need the expansions set in case anything therein is expanded. If there was
  a lookup defer while checking, defer this address. */

  deliver_set_expansions(addr);
  if (route_skip_driver(r, addr->local_part, addr->route_domain,
      verify, &more, "router"))
    {
    if (!search_find_defer) continue;
    addr->message = "lookup defer for domains or local_parts";
    addr->router = r;
    return DEFER;
    }

  /* Loop protection: If this address has a parent with the same address that
  was routed by this router, we skip this router. This prevents a looping
  states when a new address is created by the use of "unseen" on a router.
  That is (at present) the only time a router can generate another address
  and hence make an address a parent; however, forwarding of local addresses
  can of course cause there to be parents at this point even without the
  use of "unseen". */

  for (parent = addr->parent; parent != NULL; parent = parent->parent)
    {
    if (strcmpic(parent->local_part, addr->local_part) == 0 &&
        strcmpic(parent->domain, addr->domain) == 0 &&
        parent->router == r)
      {
      DEBUG(9) debug_printf("%s router skipped: previously routed %s@%s\n",
        r->name, addr->local_part, addr->domain);
      loop_detected = TRUE;
      break;
      }
    }
  if (loop_detected) continue;

  /* Before calling the router, output any debugging string. Also, unset the
  rewrite flag; routers must explicitly request rewriting. */

  debug_print_string(r->debug_string);
  clearflag(addr, af_rewrite_headers);

  /* Do file existence tests - must do after setting expansion values
  as the string is expanded - and if they succeed, do any necessary sender
  checks, and run the router. Otherwise set an appropriate yield value. */

  switch(match_exists(r->require_files, &error))
    {
    case DEFER:
    addr->message = string_sprintf("file existence defer in %s router: %s",
      r->name, error);
    yield = DEFER;
    break;

    case FAIL:
    DEBUG(9) debug_printf("%s router: file existence failure\n", r->name);
    yield = DECLINE;
    break;

    case OK:
    if (r->senders != NULL &&
         (rc = match_sender(
           expand_string_panic(r->senders, r->name, "router"))) != OK)
      {
      if (rc == DEFER)
        {
        addr->message = "sender check lookup defer";
        yield = DEFER;
        }
      else
        {
        DEBUG(9) debug_printf("%s router: sender match failure\n",
          r->name);
        yield = DECLINE;
        }
      }
    else if (r->condition != NULL &&
          !expand_check_condition(r->condition, r->name, "router"))
      {
      if (search_find_defer)
        {
        addr->message = "condition check lookup defer";
        yield = DEFER;
        DEBUG(9) debug_printf("%s\n", addr->message);
        }
      else
        {
        DEBUG(9) debug_printf("%s router: condition failure; more set TRUE\n",
          r->name);
        yield = DECLINE;
        more = TRUE;      /* Condition tests are logically before "more" */
        }
      }

    /* All conditions are OK - we are now going to run the router. First
    expand ignore_target_hosts. */

    else
      {
      char *ignore_target_hosts = NULL;
      if (r->ignore_target_hosts != NULL)
        {
        ignore_target_hosts = expand_string(r->ignore_target_hosts);
        if (ignore_target_hosts == NULL &&
            (search_find_defer || !expand_string_forcedfail))
          {
          addr->message = string_sprintf("expansion of \"%s\" failed "
            "in %s router: %s", r->ignore_target_hosts, r->name,
            expand_string_message);
          yield = DEFER;
          break;
          }
        }
      clearflag(addr, af_local_host_removed);
      yield = (r->info->code)(r, addr, addr_local, addr_remote,
        verify != v_none, ignore_target_hosts);
      }
    break;
    }

  deliver_set_expansions(NULL);

  /* If succeeded while verifying but fail_verify is set, convert into
  a failure, and take it off the local or remote delivery list. */

  if (((verify == v_sender && r->fail_verify_sender) ||
       (verify == v_recipient && r->fail_verify_recipient)) &&
      (yield == OK || yield == PASS))
    {
    addr->message = string_sprintf("%s router forced verify failure",
      r->name);
    if (*addr_remote == addr) *addr_remote = addr->next;
    if (*addr_local == addr) *addr_local = addr->next;
    break;
    }

  /* Router (might have) modified the address; loop for next router, whatever
  the setting of "more". */

  if (yield == PASS)
    {
    DEBUG(2)
      {
      debug_printf("%s router passed; \"more\" overridden\n", r->name);
      if (strcmp(old_route_domain, addr->route_domain) != 0)
        debug_printf("domain %s rewritten as %s\n", old_route_domain,
          addr->route_domain);
      }
    more = TRUE;
    continue;
    }

  /* Declined to handle this address; loop for next router, respecting the
  setting of "more". */

  if (yield == DECLINE)
    {
    DEBUG(2) debug_printf("%s router declined\n", r->name);
    continue;
    }

  /* Router wants this address to be failed; do not loop for next router. */

  if (yield == FORCEFAIL)
    {
    DEBUG(2) debug_printf("%s router forced address failure\n", r->name);
    break;
    }

  /* Deferral means we are finished with this address, as does an internal
  or configuration failure. */

  if (yield == DEFER || yield == ERROR)
    {
    DEBUG(2)
      {
      if (yield == DEFER)
        {
        debug_printf("%s router deferred %s\n", r->name, addr->route_domain);
        debug_printf("  message: %s\n", (addr->message == NULL)?
          "<none>" : addr->message);
        }
      else
        {
        debug_printf("%s router: error for %s\n", r->name, addr->route_domain);
        debug_printf("  message: %s\n", (addr->message == NULL)?
          "<none>" : addr->message);
        }
      }

    addr->router = r;
    return yield;
    }

  /* The yield must be either OK or ISLOCAL. */

  if (yield != OK && yield != ISLOCAL)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s router returned unknown value %d",
      r->name, yield);

  /* In both cases we rewrite envelope and headers if requested before
  finishing with this address. The rewriting takes its data from what the
  router did to the route_address, which may no longer be the same as the
  envelope address (in which case the rewriting probably isn't relevant there
  or in the headers, but the code does support it). */

  if (testflag(addr, af_rewrite_headers) &&
      strcmp(old_route_domain, addr->route_domain) != 0)
    {
    DEBUG(9) debug_printf("rewriting after %s router\n", r->name);

    /* First the domain in the envelope, if it was the same as the old routing
    domain. */

    if (strcmp(addr->domain, old_route_domain) == 0)
      addr->domain = addr->route_domain;

    /* Now the headers, except when verifying, when there aren't any! The
    first header is always "Received:" so we can skip it. There may not
    always be headers, however. In "-bt" mode for instance, there won't be
    any. When a header gets rewritten, set the header_changed flag to force the
    spool header file to be rewritten. */

    if (verify == v_none && header_list != NULL)
      {
      header_line *h;
      for (h = header_list->next; h != NULL; h = h->next)
        {
        header_line *newh =
          rewrite_header(h, old_route_domain, addr->route_domain,
            global_rewrite_rules, rewrite_existflags, TRUE);
        if (newh != NULL)
          {
          h = newh;
          header_changed = TRUE;
          }
        }
      }
    }


  /* If the yield was ISLOCAL, an apparently remote address was really that
  of a local domain (e.g. foo got looked up in the DNS and turned into
  foo.co.uk, which is one of the local domains). Alternatively, a remote
  domain had its name changed as a result of routing (to another non-local
  domain), but its lowest numbered MX record points to this host. Again,
  this could be foo turning into foo.co.uk.

  We return ISLOCAL so that the expanded address can be re-processed by the
  directors or routers, as appropriate. If rewrite_headers is not set we
  won't have processed the domain above, so do it here. This doesn't happen
  for domain expansion, but it can happen when MX->self causes re-routing. */

  if (yield == ISLOCAL)
    {
    DEBUG(2) debug_printf("%s router found %s to be local, or locally MX'd\n",
      r->name, old_route_domain);
    if (!testflag(addr, af_rewrite_headers) &&
         strcmp(addr->domain, old_route_domain) == 0)
      addr->domain = addr->route_domain;
    addr->router = r;
    return ISLOCAL;
    }


  /* The only remaining possibility is that the router succeeded. */


  /* If the translate_ip_address options is set and host addresses were
  associated with the address, run them through the translation. This feature
  is for weird and wonderful situations (the amateur packet radio people need
  it) or very broken networking, so it is included in the binary only if
  requested. */

  #ifdef SUPPORT_TRANSLATE_IP_ADDRESS

  if (r->translate_ip_address != NULL)
    {
    host_item *h;
    for (h = addr->host_list; h != NULL; h = h->next)
      {
      if (h->address != NULL)
        {
        char *newaddress;
        deliver_host_address = h->address;
        newaddress = expand_string(r->translate_ip_address);
        deliver_host_address = NULL;
        if (newaddress == NULL)
          {
          if (!expand_string_forcedfail)
            {
            addr->basic_errno = ERRNO_EXPANDFAIL;
            addr->message = string_sprintf("translate_ip_address expansion "
              "failed: %s", expand_string_message);
            return DEFER;
            }
          }
        else
          {
          DEBUG(2) debug_printf("%s [%s] translated to %s\n",
            h->name, h->address, newaddress);
          if (string_is_ip_address(newaddress, NULL))
            {
            h->address = newaddress;
            }
          else
            {
            char *oldname = h->name;
            char *oldaddress = h->address;

            h->name = newaddress;
            h->address = NULL;

            switch (host_find_byname(h, NULL, NULL, TRUE))
              {
              case HOST_FOUND_LOCAL:
              DEBUG(2) debug_printf("translated address is local host\n");
              addr->router = r;
              return ISLOCAL;

              case HOST_FIND_FAILED:
              addr->basic_errno = ERRNO_UNKNOWNHOST;
              addr->message = string_sprintf("host %s not found when "
                "translating %s [%s]", h->name, oldname, oldaddress);
              return DEFER;
              }
            }
          }
        }
      }
    }
  #endif  /* SUPPORT_TRANSLATE_IP_ADDRESS */

  DEBUG(2)
    {
    host_item *h;

    debug_printf("routed by %s router%s:\n  deliver to %s@%s\n", r->name,
      (r->unseen)? " (unseen)" : "",
      addr->local_part, addr->domain);

    debug_printf("  transport: %s\n", (addr->transport == NULL)?
      "<none>" : addr->transport->name);

    if (addr->errors_address != NULL)
      debug_printf("  errors to %s\n", addr->errors_address);

    for (h = addr->host_list; h != NULL; h = h->next)
      {
      debug_printf("  host %s ", h->name);
      if (h->address != NULL)
        debug_printf("[%s] ", h->address);
      if (h->mx >= 0) debug_printf("MX=%d", h->mx);
      debug_printf("\n");
      }
    }

  /* Record which router did it, and return success, having cleared any
  temporary error message set by a router that declined, and handled the
  "unseen" option. */

  addr->router = r;
  addr->message = NULL;
  if (r->unseen)
    route_unseen(r->name, addr, addr_local, addr_remote, addr_new);
  return OK;
  }

/* No routers accepted this address; fail it, with a default message if nothing
is set. */

DEBUG(2)
  {
  if (!more && r != NULL)
    debug_printf("no_more set: routing abandoned\n");
  else
    debug_printf("end of routers reached\n");
  }

if (addr->message == NULL)
  addr->message = string_sprintf("unrouteable mail domain \"%s\"",
    addr->route_domain);

deliver_set_expansions(NULL);    /* In case final one was skipped */
return FAIL;
}

/* End of route.c */
