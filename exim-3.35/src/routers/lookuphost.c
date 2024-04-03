/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */



#include "../exim.h"
#include "lookuphost.h"



/* Options specific to the lookuphost router. */

optionlist lookuphost_router_options[] = {
  { "check_secondary_mx", opt_bool,
      (void *)(offsetof(lookuphost_router_options_block, check_secondary_mx)) },
  { "gethostbyname",      opt_bool,
      (void *)(offsetof(lookuphost_router_options_block, gethostbyname)) },
  { "mx_domains",         opt_stringptr,
      (void *)(offsetof(lookuphost_router_options_block, mx_domains)) },
  { "qualify_single",     opt_bool,
      (void *)(offsetof(lookuphost_router_options_block, qualify_single)) },
  { "rewrite_headers",    opt_bool,
      (void *)(offsetof(lookuphost_router_options_block, rewrite_headers)) },
  { "search_parents",     opt_bool,
      (void *)(offsetof(lookuphost_router_options_block, search_parents)) },
  { "widen_domains",      opt_stringptr,
      (void *)(offsetof(lookuphost_router_options_block, widen_domains)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int lookuphost_router_options_count =
  sizeof(lookuphost_router_options)/sizeof(optionlist);

/* Default private options block for the lookuphost router. */

lookuphost_router_options_block lookuphost_router_option_defaults = {
  FALSE,           /* check_secondary_mx */
  FALSE,           /* gethostbyname */
  TRUE,            /* qualify_single */
  FALSE,           /* search_parents */
  TRUE,            /* rewrite_headers */
  NULL,            /* widen_domains */
  NULL             /* mx_domains */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
lookuphost_router_init(router_instance *rblock)
{
/*
lookuphost_router_options_block *ob =
  (lookuphost_router_options_block *)(rblock->options_block);
*/

/* There must be a transport, unless just verifying. */

if (rblock->transport == NULL &&
    rblock->expand_transport == NULL &&
    !rblock->verify_only)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "a transport specification is required", rblock->name);
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface details */

int
lookuphost_router_entry(
  router_instance *rblock,        /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if successful & local */
  address_item **addr_remote,     /* add it to this if successful & remote */
  BOOL verify,                    /* TRUE if verifying */
  char *ignore_target_hosts)      /* list of hosts to ignore (expanded) */
{
host_item h;
int rc;
int widen_sep = 0;
lookuphost_router_options_block *ob =
  (lookuphost_router_options_block *)(rblock->options_block);
char *widen = NULL;
char *pre_widen = addr->route_domain;
char *post_widen = NULL;
char *fully_qualified_name;
char *listptr;
char widen_buffer[256];

DEBUG(2)
  debug_printf("%s router called for %s\n  %s lookup: route_domain = %s\n",
  rblock->name, addr->orig, (ob->gethostbyname)? "gethostbyname" : "dns",
  addr->route_domain);


/* Set up the first of any widening domains. The code further down copes with
either pre- or post-widening, but at present there is no way to turn on
pre-widening, as actually doing so seems like a rather bad idea, and nobody has
requested it. Pre-widening would cause local abbreviated names to take
precedence over global names. For example, if the domain is "xxx.ch" it might
be something in the "ch" toplevel domain, but it also might be xxx.ch.xyz.com.
The choice of pre- or post-widening affects which takes precedence. If ever
somebody comes up with some kind of requirement for pre-widening, presumably
with some conditions under which it is done, it can be selected here. */

if (ob->widen_domains != NULL)
  {
  listptr = ob->widen_domains;
  widen = string_nextinlist(&listptr, &widen_sep, widen_buffer,
    sizeof(widen_buffer));

/****
  if (some condition requiring pre-widening)
    {
    post_widen = pre_widen;
    pre_widen = NULL;
    }
****/

  }

/* Loop to cope with explicit widening of domains as configured. This code
copes with widening that may happen before or after the original name. The
decision as to which is taken above. */

for (;;)
  {
  if (pre_widen != NULL)
    {
    h.name = pre_widen;
    pre_widen = NULL;
    }
  else if (widen != NULL)
    {
    h.name = string_sprintf("%s.%s", addr->route_domain, widen);
    widen = string_nextinlist(&listptr, &widen_sep, widen_buffer,
      sizeof(widen_buffer));
    DEBUG(2) debug_printf("%s router widened %s to %s\n", rblock->name,
      addr->route_domain, h.name);
    }
  else if (post_widen != NULL)
    {
    h.name = post_widen;
    post_widen = NULL;
    DEBUG(2) debug_printf("%s router trying %s after widening failed\n",
      rblock->name, h.name);
    }
  else return DECLINE;

  /* Set up the rest of the initial host item. Others may get chained on if
  there is more than one IP address. We set it up here instead of outside the
  loop so as to re-initialize if a previous try succeeded but was rejected
  because of not having an MX record. */

  h.next = NULL;
  h.address = NULL;
  h.mx = -1;
  h.status = hstatus_unknown;
  h.why = hwhy_unknown;
  h.last_try = 0;

  /* Call the appropriate function to fill in the address and chain on
  additional host_items if necessary. */

  if (ob->gethostbyname)
    rc = host_find_byname(&h, ignore_target_hosts, &fully_qualified_name,
      FALSE);

  /* Unfortunately, we cannot set the mx_only option in advance, because the
  DNS lookup may extend an unqualified name. Therefore, we must do the test
  subsequently. */

  else
    {
    BOOL removed = FALSE;
    rc = host_find_bydns(&h, ignore_target_hosts, FALSE, FALSE,
      ob->qualify_single, ob->search_parents, &fully_qualified_name,
      &removed);
    if (removed) setflag(addr, af_local_host_removed);

    /* If host found with only A records, test for the domain's being in the
    mx_domains list, and not turned into a local domain. */

    if ((rc == HOST_FOUND || rc == HOST_FOUND_LOCAL) && h.mx < 0)
      {
      BOOL become_local;
      BOOL in_mx_domains = ob->mx_domains != NULL &&
        match_isinlist(fully_qualified_name, &(ob->mx_domains),TRUE,TRUE,NULL);

      if (search_find_defer)
        {
        addr->message = "lookup defer for mx_domains";
        return DEFER;
        }

      become_local =
        match_isinlist(fully_qualified_name, &local_domains,TRUE,TRUE,NULL);

      if (search_find_defer)
        {
        addr->message = "lookup defer for local_domains";
        return DEFER;
        }

      if (in_mx_domains && !become_local)
        {
        DEBUG(2) debug_printf("%s router rejected %s: no MX record(s)\n",
          rblock->name, fully_qualified_name);
        continue;
        }
      }
    }

  /* Deferral returns forthwith, and anything other than failure breaks the
  loop. */

  if (rc == HOST_FIND_AGAIN)
    {
    if (rblock->pass_on_timeout)
      {
      DEBUG(2) debug_printf("%s router timed out & pass_on_timeout set\n",
        rblock->name);
      return PASS;
      }
    addr->message = "host lookup did not complete";
    return DEFER;
    }

  if (rc != HOST_FIND_FAILED) break;

  /* Check to see if the failure is the result of MX records pointing
  to non-existent domains, and if so, set an appropriate error message;
  otherwise "unknown mail domain" is used, which is confusing. Also, in this
  case don't do the widening. We need check only the first host to see if
  its MX has been filled in, but there is no address, because if there were
  any usable addresses returned, we would not have had HOST_FIND_FAILED.

  As a common cause of this problem is MX records with IP addresses on the
  RHS, give a special message in this case. */

  if (h.mx >= 0 && h.address == NULL)
    {
    addr->message = "all relevant MX records point to non-existent hosts";
    if (!allow_mx_to_ip && string_is_ip_address(h.name, NULL))
      addr->message = string_sprintf("%s: it appears that the DNS operator for "
         "this domain has installed an invalid MX record with an IP address "
         "instead of a domain name on the right hand side", addr->message);
    return DECLINE;
    }

  /* If there's a syntax error, do not continue with any widening, and note
  the error. */

  if (host_find_failed_syntax)
    {
    addr->message = string_sprintf("mail domain \"%s\" is syntactically "
      "invalid", addr->domain);
    return DECLINE;
    }
  }


/* If the original domain name has been changed as a result of the host lookup,
change the name in the address structure and request header rewrites if so
configured. Then check to see if the fully qualified name is in fact one of the
local domain names, or if the lookup yield was HOST_FOUND_LOCAL. If so, return
ISLOCAL so that the address can be passed back for re-directing or re-routing
in its full form, and force header rewriting. */

if (strcmp(addr->route_domain, fully_qualified_name) != 0)
  {
  addr->route_domain = fully_qualified_name;
  if (ob->rewrite_headers) setflag(addr, af_rewrite_headers);
  search_find_defer = FALSE;
  if (rc == HOST_FOUND_LOCAL ||
      match_isinlist(fully_qualified_name, &local_domains, TRUE, TRUE, NULL))
    {
    setflag(addr, af_rewrite_headers);
    return ISLOCAL;
    }
  else if (search_find_defer)
    {
    addr->message = "lookup defer for local_domains";
    return DEFER;
    }
  }


/* If the yield is HOST_FOUND_LOCAL, the remote domain name either found MX
records with the lowest numbered one pointing to a host with an IP address that
is set on one of the interfaces of this machine, or found A records or got
addresses from gethostbyname() that contain one for this machine. This can
happen quite legitimately if the original name was a shortened form of a local
domain, but if so, the fully qualified name will be a local domain and will
have been detected above. If it is not, there may be some kind of configuration
error or lookuphost error.

In the case of a source-routed address, the best plan is to move on to the next
host in the source routing, as happens if a source-routed address starts with
a local domain. This is done by rewriting the domain and local part, and
returning ISLOCAL. For non-source-routed addresses, the action to be taken can
be configured by the self option, the handling of which is now in a separate
function, as it is also required for other routers. */

if (rc == HOST_FOUND_LOCAL)
  {
  rc = host_self_action(addr, &h, rblock->self_code, rblock->self_rewrite,
    rblock->self);
  if (rc != OK) return rc;
  }

/* Otherwise, insist on being a secondary MX if so configured */

else if (ob->check_secondary_mx && !testflag(addr, af_local_host_removed))
  {
  DEBUG(2) debug_printf("check_secondary_mx set and local host not secondary\n");
  return DECLINE;
  }

/* Set up the errors address, if any. The function uses only fields that
are in the same place in director and router blocks. */

rc = direct_get_errors_address(addr, (director_instance *)rblock, "router",
  verify, &(addr->errors_address));
if (rc != OK) return rc;

/* Set up the additional and removeable headers for this address. The function
uses only fields that are in the same place in director and router blocks. */

rc = direct_get_munge_headers(addr, (director_instance *)rblock, "router",
  &(addr->extra_headers), &(addr->remove_headers));
if (rc != OK) return rc;

/* Get store in which to preserve the original host item, chained on
to the address. */

addr->host_list = store_get(sizeof(host_item));
addr->host_list[0] = h;

/* Fill in the transport, queue the address for local or remote delivery, and
yield success. If just verifying, there may not be a transport, in which case
it doesn't matter which queue is used. This is all now handled by the
route_queue() function. Initialization insists on the presence of a transport
of some kind. */

if (!route_get_transport(rblock->transport, rblock->expand_transport,
      addr, &(addr->transport), rblock->name, NULL))
  return ERROR;

return route_queue(addr, addr_local, addr_remote, rblock, NULL, "router")?
  OK : ERROR;
}

/* End of routers/lookuphost.c */
