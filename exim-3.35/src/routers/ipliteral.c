/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "ipliteral.h"



/* Options specific to the ipliteral router. Because some compilers do not like
empty declarations ("undefined" in the Standard) we put in a dummy value. */


optionlist ipliteral_router_options[] = {
  { "", opt_hidden, NULL }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int ipliteral_router_options_count =
  sizeof(ipliteral_router_options)/sizeof(optionlist);

/* Default private options block for the ipliteral router. Again, a dummy
value is present to keep some compilers happy. */

ipliteral_router_options_block ipliteral_router_option_defaults = { 0 };



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
ipliteral_router_init(router_instance *rblock)
{
/*
ipliteral_router_options_block *ob =
  (ipliteral_router_options_block *)(rblock->options_block);
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
ipliteral_router_entry(
  router_instance *rblock,        /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if successful & local */
  address_item **addr_remote,     /* add it to this if successful & remote */
  BOOL verify,                    /* TRUE if verifying */
  char *ignore_target_hosts)      /* ignore host list (expanded) */
{
/*
ipliteral_router_options_block *ob =
  (ipliteral_router_options_block *)(rblock->options_block);
*/
host_item *h;
char *domain = addr->route_domain;
int len = (int)strlen(domain);
int rc;

ignore_target_hosts = ignore_target_hosts;   /* Stop compiler warnings */

DEBUG(2) debug_printf("%s router called for %s: route_domain = %s\n",
  rblock->name, addr->orig, addr->route_domain);

/* Check that the domain is an IP address enclosed in square brackets. If
not, the router declines. Otherwise route to the single IP address, setting the
host name to "(unnamed)". */

if (domain[0] != '[' || domain[len-1] != ']') return DECLINE;
domain[len-1] = 0;  /* temporarily */

if (!string_is_ip_address(domain+1, NULL))
  {
  domain[len-1] = ']';
  return DECLINE;
  }

/* Set up a host item */

addr->host_list = h = store_get(sizeof(host_item));

h->next = NULL;
h->address = string_copy(domain+1);
domain[len-1] = ']';   /* restore */
h->name = string_copy(domain);
h->mx = -1;
h->status = hstatus_unknown;
h->why = hwhy_unknown;
h->last_try = 0;

/* Determine whether the host is the local host, and if so, take action
according to the configuration. */

if (host_scan_for_local_hosts(h, h, NULL) == HOST_FOUND_LOCAL)
  {
  int rc = host_self_action(addr, h, rblock->self_code, rblock->self_rewrite,
    rblock->self);
  if (rc != OK) return rc;
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

/* Fill in the transport, queue the address for local or remote delivery, and
yield success. For local delivery, of course, the IP address won't be used. If
just verifying, there need not be a transport, in which case it doesn't matter
which queue we put the address on. This is all now handled by the route_queue()
function. */

if (!route_get_transport(rblock->transport, rblock->expand_transport,
      addr, &(addr->transport), rblock->name, NULL))
  return ERROR;

return route_queue(addr, addr_local, addr_remote, rblock, NULL, "router")?
  OK : ERROR;
}

/* End of routers/ipliteral.c */
