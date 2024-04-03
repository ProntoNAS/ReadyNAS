/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "iplookup.h"


/* IP connection types */

#define ip_udp 0
#define ip_tcp 1


/* Options specific to the iplookup router. */

optionlist iplookup_router_options[] = {
  { "hosts",    opt_stringptr,
      (void *)(offsetof(iplookup_router_options_block, hosts)) },
  { "optional", opt_bool,
      (void *)(offsetof(iplookup_router_options_block, optional)) },
  { "port",     opt_int,
      (void *)(offsetof(iplookup_router_options_block, service)) },
  { "protocol", opt_stringptr,
      (void *)(offsetof(iplookup_router_options_block, protocol_name)) },
  { "query",    opt_stringptr,
      (void *)(offsetof(iplookup_router_options_block, query)) },
  { "reroute",  opt_stringptr,
      (void *)(offsetof(iplookup_router_options_block, reroute)) },
  { "response_pattern", opt_stringptr,
      (void *)(offsetof(iplookup_router_options_block, response_pattern)) },
  { "service",  opt_int | opt_hidden,
      (void *)(offsetof(iplookup_router_options_block, service)) },
  { "timeout",  opt_time,
      (void *)(offsetof(iplookup_router_options_block, timeout)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int iplookup_router_options_count =
  sizeof(iplookup_router_options)/sizeof(optionlist);

/* Default private options block for the iplookup router. */

iplookup_router_options_block iplookup_router_option_defaults = {
  -1,       /* service */
  ip_udp,   /* protocol */
  5,        /* timeout */
  NULL,     /* protocol_name */
  NULL,     /* hosts */
  NULL,     /* query; NULL => local_part@domain */
  NULL,     /* response_pattern; NULL => don't apply regex */
  NULL,     /* reroute; NULL => just used returned data */
  NULL,     /* re_response_pattern; compiled pattern */
  FALSE     /* optional */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
iplookup_router_init(router_instance *rblock)
{
iplookup_router_options_block *ob =
  (iplookup_router_options_block *)(rblock->options_block);

/* Don't rewrite sender domains as a result of this router */

rblock->sender_rewrite = FALSE;

/* A service and a host list must be given */

if (ob->service < 0)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "a service (port) must be specified", rblock->name);

if (ob->hosts == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "a host list must be specified", rblock->name);

/* A transport must NOT be given. */

if (rblock->transport != NULL || rblock->expand_transport != NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "a transport must not be given for the %s driver",
      rblock->name, rblock->info->driver_name);

/* Translate protocol name into value */

if (ob->protocol_name != NULL)
  {
  if (strcmp(ob->protocol_name, "udp") == 0) ob->protocol = ip_udp;
  else if (strcmp(ob->protocol_name, "tcp") == 0) ob->protocol = ip_tcp;
  else log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "protocol not specified as udp or tcp", rblock->name);
  }

/* If a response pattern is given, compile it now to get the error early. */

if (ob->response_pattern != NULL)
  ob->re_response_pattern =
    regex_must_compile(ob->response_pattern, FALSE, TRUE);
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface details */

int
iplookup_router_entry(
  router_instance *rblock,        /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if successful & local */
  address_item **addr_remote,     /* add it to this if successful & remote */
  BOOL verify,                    /* TRUE if verifying */
  char *ignore_target_hosts)      /* ignore host list (expanded) */
{
u_short net_port;
char *query = NULL;
char reply[256];
char *hostname, *reroute, *domain, *listptr;
char host_buffer[256];
host_item *host = store_get(sizeof(host_item));
iplookup_router_options_block *ob =
  (iplookup_router_options_block *)(rblock->options_block);
pcre *re = ob->re_response_pattern;
int count, query_len, rc;
int error_yield = (ob->optional)? PASS : DEFER;
int yield = PASS;
int sep = 0;

addr_local = addr_local;    /* Keep picky compilers happy */
addr_remote = addr_remote;
ignore_target_hosts = ignore_target_hosts;

DEBUG(2) debug_printf("%s router called for %s: route_domain = %s\n",
  rblock->name, addr->orig, addr->route_domain);

/* Build the query string to send. If not explicitly given, a default of
"user@domain user@domain" is used. */

if (ob->query == NULL)
  query = string_sprintf("%s@%s %s@%s", addr->local_part, addr->domain,
    addr->local_part, addr->domain);
else
  {
  query = expand_string(ob->query);
  if (query == NULL)
    {
    addr->message = string_sprintf("%s router: failed to expand %s: %s",
      rblock->name, ob->query, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  }

query_len = (int)strlen(query);
DEBUG(9) debug_printf("%s router query is %s\n", rblock->name, query);

/* Now connect to the required port for each of the hosts in turn, until a
response it received. Initialization insists on the port being set and there
being a host list. */

net_port = htons(ob->service);
listptr = ob->hosts;

while ((hostname = string_nextinlist(&listptr, &sep, host_buffer,
       sizeof(host_buffer))) != NULL)
  {
  host_item *h;

  DEBUG(9) debug_printf("calling host %s\n", hostname);

  host->name = hostname;
  host->address = NULL;
  host->next = NULL;

  if (string_is_ip_address(host->name, NULL)) host->address = host->name;
    else if (host_find_byname(host, NULL, NULL, FALSE) == HOST_FIND_FAILED)
      continue;

  /* Loop for possible multiple IP addresses for the given name. */

  for (h = host; h != NULL; h = h->next)
    {
    int rc, save_errno, host_af, query_socket;

    #if HAVE_IPV6
    struct sockaddr_in6 sin;
    #else
    struct sockaddr_in sin;
    #endif

    /* Skip any hosts for which we have no address */

    if (h->address == NULL) continue;

    /* Create a socket, for UDP or TCP, as configured. IPv6 addresses are
    detected by checking for a colon in the address. */

    host_af = (strchr(h->address, ':') != NULL)? AF_INET6 : AF_INET;
    query_socket = socket(host_af,
      (ob->protocol == ip_udp)? SOCK_DGRAM:SOCK_STREAM, 0);
    if (query_socket < 0)
      {
      log_write(0, LOG_MAIN, "socket creation failed in %s router: %s",
        rblock->name, strerror(errno));
      return error_yield;
      }

    /* Clear the socket block, whichever type it is. */

    memset(&sin, 0, sizeof(sin));

    /* Set up the address and port on an IPv6 system. You wouldn't have thought
    it would take much to put the IPv6 address in the same place as the IPv4
    one, but no... */

    #if HAVE_IPV6
    sin.sin6_family = host_af;
    sin.sin6_port = net_port;

    if (host_af == AF_INET6)
      {
      if (inet_pton(host_af, h->address, &sin.sin6_addr) != 1)
        log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", h->address);
      }
    else
      {
      if (inet_pton(host_af, h->address, &sin.sin6_flowinfo) != 1)
        log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", h->address);
      }

    /* Set up the address and port on an IPv4 system */

    #else
    sin.sin_family = AF_INET;
    sin.sin_port = net_port;
    sin.sin_addr.s_addr = (S_ADDR_TYPE)inet_addr(h->address);
    #endif

    /* Just try the next one if connection or sending fails; use the
    common timeout handler code for timing out the connection. In fact,
    timeouts can occur only for TCP calls; for a UDP socket, "connect" always
    works (the router will timeout later on the read call). */

    sigalrm_seen = FALSE;
    os_non_restarting_signal(SIGALRM, sigalrm_handler);
    alarm(ob->timeout);

    rc = connect(query_socket, (struct sockaddr *)&sin, sizeof(sin));
    save_errno = errno;

    alarm(0);
    signal(SIGALRM, SIG_IGN);

    if (rc < 0)
      {
      DEBUG(9)
        {
        if (save_errno == EINTR && sigalrm_seen) save_errno = ETIMEDOUT;
        debug_printf("connection to %s failed: %s\n", h->address,
          strerror(save_errno));
        }
      close(query_socket);
      continue;
      }

    if (send(query_socket, query, query_len, 0) < 0)
      {
      DEBUG(9) debug_printf("send to %s failed\n", h->address);
      close(query_socket);
      continue;
      }

    /* The recv() function call is timed. There is a loop to cover the
    possibility of select() getting interrupted (rare, but can happen with,
    e.g. the SIGUSR1 signal from exiwhat, or returning with a positive result
    but no ready descriptor). Is this in fact possible? */

    for (;;)
      {
      int rc;
      fd_set select_inset;
      struct timeval tv;

      FD_ZERO (&select_inset);
      FD_SET (query_socket, &select_inset);
      tv.tv_sec = ob->timeout;
      tv.tv_usec = 0;
      rc = select(query_socket + 1, (SELECT_ARG2_TYPE *)&select_inset,
        NULL, NULL, &tv);

      /* Handle an interrupt. */

      if (rc < 0 && errno == EINTR) continue;

      /* Handle a timeout or any other error while select() was waiting. Treat
      the latter as if a timeout had occurred. */

      if (rc <= 0)
        {
        errno = ETIMEDOUT;
        count = -1;
        break;
        }

      /* If the socket is ready, initialize empty buffer in case nothing gets
      read, then read the response and break out of this select retry loop. */

      if (FD_ISSET(query_socket, &select_inset))
        {
        *reply = 0;
        count = recv(query_socket, reply, 255, 0);
        break;
        }
      }

    /* Finished with the socket */

    close(query_socket);

    /* recv() failed; try next IP address */

    if (count < 0)
      {
      DEBUG(9) debug_printf("%s from %s\n", (errno == ETIMEDOUT)?
        "timed out" : "recv failed", h->address);
      continue;
      }

    /* Success; break the loop */

    reply[count] = 0;
    DEBUG(9) debug_printf("%s router received %s from %s\n",
      rblock->name, reply, h->address);
    break;
    }

  /* If h == NULL we have tried all the IP addresses and failed on all of them,
  so we must continue to try more host names. Otherwise we have succeeded. */

  if (h != NULL) break;
  }


/* If hostname is NULL, we have failed to find any host, or failed to
connect to any of the IP addresses, or timed out while reading or writing to
those we have connected to. In all cases, we must pass if optional and
defer otherwise. */

if (hostname == NULL)
  {
  DEBUG(9) debug_printf("%s router failed to get anything\n", rblock->name);
  return error_yield;
  }


/* If a response pattern was supplied, match the returned string against it. A
failure to match causes the router to decline. After a successful match, the
numerical variables for expanding the rerouted address are set up. */

if (re != NULL)
  {
  if (!regex_match_and_setup(re, reply, 0, -1))
    {
    DEBUG(9) debug_printf("%s router: %s failed to match response %s\n",
      rblock->name, ob->response_pattern, reply);
    return DECLINE;
    }
  }

/* If no response pattern was supplied, set up ${0} as the response up to the
first white space (if any). Also, if no query was specified, check that what
follows the white space matches user@domain. */

else
  {
  int n = 0;
  while (reply[n] != 0 && !isspace((uschar)reply[n])) n++;
  expand_nmax = 0;
  expand_nstring[0] = reply;
  expand_nlength[0] = n;

  if (ob->query == NULL)
    {
    int nn = n;
    while (isspace((uschar)reply[nn])) nn++;
    if (strcmp(query + query_len/2 + 1, reply+nn) != 0)
      {
      DEBUG(9) debug_printf("%s router: failed to match identification "
        "in response %s\n", rblock->name, reply);
      return DECLINE;
      }
    }

  reply[n] = 0;  /* Terminate for the default case */
  }

/* If an explicit rerouting string is specified, expand it. Otherwise, use
what was sent back verbatim. */

if (ob->reroute != NULL)
  {
  reroute = expand_string(ob->reroute);
  if (reroute == NULL)
    {
    addr->message = string_sprintf("%s router: failed to expand %s: %s",
      rblock->name, ob->reroute, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  }
else reroute = reply;

/* We should now have a new address in the form user@domain. This must be
used to update the local part, the domain that is being routed, and the
actual address domain, except when verifying. */

domain = strchr(reroute, '@');
if (domain == NULL)
  {
  log_write(0, LOG_MAIN, "%s router: reroute string %s is not of the form "
    "user@domain", rblock->name, reroute);
  addr->message = string_sprintf("%s router: reroute string %s is not of the "
    "form user@domain", rblock->name, reroute);
  addr->special_action = SPECIAL_FREEZE;
  return ERROR;
  }

/* Update the domain and local part. If verifying, we update only the
route_domain, which is the domain being worked on, since we do not want
to change the original in this case. */

addr->local_part = string_copyn(reroute, domain - reroute);
addr->route_domain = string_copy(++domain);
if (!verify) addr->domain = addr->route_domain;

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

/* Check whether the new domain is actually a local domain. If so, we have to
pass back this address to the directors. */

if (match_isinlist(domain, &local_domains, TRUE, TRUE, NULL))
  yield = ISLOCAL;
else if (search_find_defer)
  {
  yield = DEFER;
  addr->message = "cannot check for local domain";
  }

/* Cancel numerical expansion variables, and return */

expand_nmax = -1;
return yield;
}

/* End of routers/iplookup.c */
