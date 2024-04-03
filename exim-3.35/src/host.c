/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for finding hosts, either by gethostbyname, gethostbyaddr, or via
the DNS. Also contains a function for getting a list of interfaces - either
configured by local_interfaces, or from the os-dependent function. Also
contains a random number function, used for randomizing hosts with equal MXs
but available for use in other parts of Exim, and various other functions
concerned with hosts and addresses. */


#include "exim.h"


#ifdef USE_INET_NTOA_FIX
/*************************************************
*         Replacement for broken inet_ntoa()     *
*************************************************/

/* On IRIX systems, gcc uses a different structure passing convention to the
native libraries. This causes inet_ntoa() to always yield 0.0.0.0 or
255.255.255.255. To get round this, we provide a private version of the
function here. It is used only if USE_INET_NTOA_FIX is set, which should happen
only when gcc is in use on an IRIX system. Code send to me by J.T. Breitner,
with these comments:

  code by Stuart Levy
  as seen in comp.sys.sgi.admin

Arguments:  sa  an in_addr structure
Returns:        pointer to static text string
*/

char *
inet_ntoa(struct in_addr sa)
{
static char addr[20];
sprintf(addr, "%d.%d.%d.%d",
        ((unsigned char *)&sa.s_addr)[0],
        ((unsigned char *)&sa.s_addr)[1],
        ((unsigned char *)&sa.s_addr)[2],
        ((unsigned char *)&sa.s_addr)[3]);
  return addr;
}
#endif



/*************************************************
*              Random number generator           *
*************************************************/

/* This is a simple pseudo-random number generator. It does not have to be
very good for the uses to which it is put. When running the regression tests,
start with a fixed seed.

Arguments:
  limit:    one more than the largest number required

Returns:    a pseudo-random number in the range 0 to limit-1
*/

int
random_number(int limit)
{
if (random_seed == 0)
  {
  if (running_in_test_harness) random_seed = 42; else
    {
    int p = (int)getpid();
    random_seed = (int)time(NULL) ^ ((p << 16) | p);
    }
  }
random_seed = 1103515245 * random_seed + 12345;
return (unsigned int)(random_seed >> 16) % limit;
}



/*************************************************
*       Build chain of host items from list      *
*************************************************/

/* This function builds a chain of host items from a textual list of host
names. It does not do any lookups. If randomize is true, the chain is build in
a randomized order.

Arguments:
  anchor      anchor for the chain
  list        text list
  randomize   TRUE for randomizing

Returns:      nothing
*/

void
host_build_hostlist(host_item **anchor, char *list, BOOL randomize)
{
int sep = 0;
char *name;
char buffer[1024];

if (list == NULL) return;

*anchor = NULL;

while ((name = string_nextinlist(&list, &sep, buffer, sizeof(buffer))) != NULL)
  {
  host_item *h = store_get(sizeof(host_item));
  h->name = string_copy(name);
  h->address = NULL;
  h->mx = -1;
  h->sort_key = randomize? random_number(100) : 0;
  h->status = hstatus_unknown;
  h->why = hwhy_unknown;
  h->last_try = 0;

  if (*anchor == NULL)
    {
    h->next = NULL;
    *anchor = h;
    }
  else
    {
    host_item *hh = *anchor;
    if (h->sort_key < hh->sort_key)
      {
      h->next = hh;
      *anchor = h;
      }
    else
      {
      while (hh->next != NULL && h->sort_key >= (hh->next)->sort_key)
        hh = hh->next;
      h->next = hh->next;
      hh->next = h;
      }
    }
  }
}



/*************************************************
*        Extract port from address string        *
*************************************************/

/* In the spool file, and in the -oMa option, a host plus port is given as an
IP address followed by a dot and a port number. This function decodes this.

Argument:
  address    points to the string; if there is a port, the '.' in the string
             is overwritten with zero to terminate the address

Returns:     0 if there is no port, else the port number.
*/

int
host_extract_port(char *address)
{
int port;
int skip = -3;                     /* Skip 3 dots in IPv4 addresses */
char *endptr;

address--;
while (*(++address) != 0)
  {
  int ch = *address;
  if (ch == ':') skip = 0;         /* Skip 0 dots in IPv6 addresses */
    else if (ch == '.' && skip++ >= 0) break;
  }
if (*address == 0) return 0;

port = strtol(address + 1, &endptr, 10);
if (*endptr != 0) return 0;        /* Invalid port; leave invalid address */

*address = 0;
return port;
}



#ifndef STAND_ALONE    /* Omit when standalone testing */

/*************************************************
*     Build sender_fullhost and sender_rcvhost   *
*************************************************/

/* This function is called when sender_host_name and/or sender_helo_name
have been set. Or might have been set - for a local message read off the spool
they won't be. In that case, do nothing. Otherwise, set up the fullhost string
as follows:

(a) No sender_host_name or sender_helo_name: "[ip address]"
(b) Just sender_host_name: "host_name [ip address]"
(c) Just sender_helo_name: "(helo_name) [ip address]"
(d) The two are identical: "host_name [ip address]"
(e) The two are different: "host_name (helo_name) [ip address]"

If log_incoming_port is set, the sending host's port number is added to the IP
address, after a dot. We don't use a colon because it would be confusing
with IPv6 addresses.

This function also builds sender_rcvhost for use in Received: lines, whose
syntax is a bit different. This value also includes the RFC 1413 identity.
There wouldn't be two different variables if I had got all this right in the
first place.

Because this data may survive over more than one incoming SMTP message, it has
to be copied into malloc store.

Arguments:  none
Returns:    nothing
*/

void
host_build_sender_fullhost(void)
{
char *fullhost, *rcvhost, *address;

if (sender_host_address == NULL) return;

/* One block of store holds both values. */

if (sender_fullhost != NULL) store_free(sender_fullhost);

/* Set up address, with or without the port */

address = string_sprintf("%s.%d", sender_host_address, sender_host_port);
if (!log_incoming_port || sender_host_port <= 0) *(strrchr(address, '.')) = 0;

/* This is just messy. Set up the values, then copy them to malloc store
afterwards. */

if (sender_host_name == NULL)
  {
  if (sender_helo_name == NULL)
    {
    fullhost = string_sprintf("[%s]", address);
    rcvhost = (sender_ident == NULL)? fullhost :
      string_sprintf("[%s] (ident=%s)", address, sender_ident);
    }
  else
    {
    fullhost = string_sprintf("(%s) [%s]", sender_helo_name, address);
    rcvhost = (sender_ident == NULL)?
      string_sprintf("[%s] (helo=%s)", address, sender_helo_name) :
      string_sprintf("[%s] (helo=%s ident=%s)", address, sender_helo_name,
        sender_ident);
    }
  }
else
  {
  int len;
  if (sender_helo_name == NULL ||
      strcmpic(sender_host_name, sender_helo_name) == 0 ||
        (sender_helo_name[0] == '[' &&
         sender_helo_name[(len=(int)strlen(sender_helo_name))-1] == ']' &&
         strncmpic(sender_helo_name+1, sender_host_address, len - 2) == 0))
    {
    fullhost = string_sprintf("%s [%s]", sender_host_name, address);
    rcvhost = (sender_ident == NULL)?
      string_sprintf("%s ([%s])", sender_host_name, address) :
      string_sprintf("%s ([%s] ident=%s)", sender_host_name, address,
        sender_ident);
    }
  else
    {
    fullhost = string_sprintf("%s (%s) [%s]", sender_host_name,
      sender_helo_name, address);
    rcvhost = (sender_ident == NULL)?
      string_sprintf("%s ([%s] helo=%s)", sender_host_name,
        address, sender_helo_name) :
      string_sprintf("%s\n\t([%s] helo=%s ident=%s)", sender_host_name,
        address, sender_helo_name, sender_ident);
    }
  }

sender_fullhost =
  store_malloc((int)strlen(fullhost) + (int)strlen(rcvhost) + 2);
sender_rcvhost = sender_fullhost + (int)strlen(fullhost) + 1;
strcpy(sender_fullhost, fullhost);
strcpy(sender_rcvhost, rcvhost);

DEBUG(5) debug_printf("sender_fullhost = %s\n", sender_fullhost);
DEBUG(5) debug_printf("sender_rcvhost = %s\n", sender_rcvhost);
}

#endif   /* STAND_ALONE */




/*************************************************
*        Decode actions for self reference       *
*************************************************/

/* This function is called from a number of routers on receiving
HOST_FOUND_LOCAL when looking up a supposedly remote host. The action is
controlled by a generic configuration option called "self" on each router,
which can be one of:

   . freeze:                       Log the incident, freeze, and return ERROR

   . defer:                        Log the incident and return DEFER

   . send:                         Carry on with the delivery regardless -
                                   this makes sense only if the SMTP
                                   listener on this machine is a differently
                                   configured MTA

   . fail_soft                     The router just fails, and the address
                                   gets passed to the next router, overriding
                                   the setting of no_more

   . reroute:<new-domain>          Change the domain to the given domain
                                   and return ISLOCAL so it gets passed back
                                   to the directors or routers.

   . reroute:rewrite:<new-domain>  The same, but headers containing the
                                   old domain get rewritten.

   . local                         Set af_forced_local and return ISLOCAL,
                                   so the address gets passed to the
                                   directors.

Arguments:
  addr     the address being routed
  host     the host that is local, with MX set (or -1 if MX not used)
  code     the action to be taken (one of the self_xxx enums)
  rewrite  TRUE if rewriting headers required for ISLOCAL
  new      new domain to be used for ISLOCAL

Returns:   ERROR, DEFER, ISLOCAL, PASS, FORCEFAIL, or OK, according
             to the value of code.
*/

int
host_self_action(address_item *addr, host_item *host, int code, BOOL rewrite,
  char *new)
{
char *msg = (host->mx >= 0)?
  "lowest numbered MX record points to local host" :
  "remote host address is the local host";

switch (code)
  {
  case self_freeze:

  /* If there is no message id, this is happening during an address
  verification, so give information about the address that is being verified,
  and where it has come from. Otherwise, during message delivery, the normal
  logging for the address will be sufficient. */

  if (message_id[0] == 0)
    {
    if (sender_fullhost == NULL)
      {
      log_write(0, LOG_MAIN, "%s: %s (while routing <%s>)", msg,
        addr->route_domain, addr->orig);
      }
    else
      {
      log_write(0, LOG_MAIN, "%s: %s (while verifying <%s> from host %s)",
        msg, addr->route_domain, addr->orig, sender_fullhost);
      }
    }
  else
    log_write(0, LOG_MAIN, "%s: %s", msg, addr->route_domain);
  addr->message = msg;
  addr->special_action = SPECIAL_FREEZE;
  return ERROR;

  case self_defer:
  addr->message = msg;
  return DEFER;

  case self_local:
  DEBUG(2)
    {
    debug_printf("%s: %s", msg, addr->route_domain);
    debug_printf("\n  address passed to directors: self_host = %s\n",
      host->name);
    }
  setflag(addr, af_forced_local);
  addr->self_hostname = string_copy(host->name);
  return ISLOCAL;

  case self_reroute:
  DEBUG(2)
    {
    debug_printf("%s: %s", msg, addr->route_domain);
    debug_printf(": domain changed to %s\n", new);
    }
  addr->route_domain = string_copy(new);
  if (rewrite) setflag(addr, af_rewrite_headers);
  return ISLOCAL;

  case self_send:
  DEBUG(2)
    {
    debug_printf("%s: %s", msg, addr->route_domain);
    debug_printf(": configured to try delivery anyway\n");
    }
  return OK;

  case self_pass:    /* This is soft failure; pass to next router */
  DEBUG(2)
    {
    debug_printf("%s: %s", msg, addr->route_domain);
    debug_printf(": router declined (self = pass)\n");
    }
  addr->message = msg;
  addr->self_hostname = string_copy(host->name);
  return PASS;

  case self_forcefail:
  DEBUG(2)
    {
    debug_printf("%s: %s", msg, addr->route_domain);
    debug_printf(": router failed hard (self = fail)\n");
    }
  addr->message = msg;
  return FORCEFAIL;
  }

return ERROR;   /* paranoia */
}



/*************************************************
*         Find addresses on local interfaces     *
*************************************************/

/* This function finds the addresses of local IP interfaces. If the option
local_interfaces is not set, it calls os_find_running_interfaces() in order
to find all the actual interfaces on the host. Otherwise it takes the list in
local_interfaces. Used to detect when apparently remote hosts are really local.
A chain of blocks containing the textual form of the addresses is returned.

Arguments:    none
Returns:      a chain of ip_address_items, each pointing to a textual
              version of an IP address
*/

ip_address_item *
host_find_interfaces(void)
{
int sep = 0;
char *s;
char *listptr = local_interfaces;
char buffer[64];
ip_address_item *yield = NULL;
ip_address_item *last = NULL;
ip_address_item *next;

if (local_interfaces == NULL) return os_find_running_interfaces();

while ((s = string_nextinlist(&listptr, &sep, buffer, sizeof(buffer))) != NULL)
  {
  if (!string_is_ip_address(s, NULL))
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Malformed IP address \"%s\" in "
      "local_interfaces", s);

  /* Data has to survive for ever, so use malloc. */

  next = store_malloc(sizeof(ip_address_item));
  next->next = NULL;
  strcpy(next->address, s);

  if (yield == NULL) yield = last = next; else
    {
    last->next = next;
    last = next;
    }

  DEBUG(8) debug_printf("Configured local interface address is %s\n",
    last->address);
  }

return yield;
}





/*************************************************
*        Convert network IP address to text      *
*************************************************/

/* Given an IPv4 or IPv6 address in binary, convert it to a text
string and return the result in a piece of new store. The address can
either be given directly, or passed over in a sockaddr structure. Note
that this isn't the converse of host_aton() because of byte ordering
differences. See host_nmtoa() below.

Arguments:
  type       if < 0 then arg points to a sockaddr, else
             either AF_INET or AF_INET6
  arg        points to a sockaddr if type is < 0, or
             points to an IPv4 address (32 bits), or
             points to an IPv6 address (128 bits),
             in both cases, in network byte order
  buffer     if NULL, the result is returned in gotten store;
             else points to a buffer to hold the answer
  portptr    points to where to put the port number, if non NULL; only
             used when type < 0

Returns:     pointer to character string
*/

char *
host_ntoa(int type, const void *arg, char *buffer, int *portptr)
{
char *yield;

/* The new world. It is annoying that we have to fish out the address from
different places in the block, depending on what kind of address it is. It
is also a pain that inet_ntop() returns a const char *, whereas the IPv4
function inet_ntoa() returns just char *, and some picky compilers insist
on warning if one assigns a const char * to a char *. Hence the casts. */

#if HAVE_IPV6
char addr_buffer[46];
if (type < 0)
  {
  int family = ((struct sockaddr *)arg)->sa_family;
  if (family == AF_INET6)
    {
    struct sockaddr_in6 *sk = (struct sockaddr_in6 *)arg;
    yield = (char *)inet_ntop(family, &(sk->sin6_addr), addr_buffer,
      sizeof(addr_buffer));
    if (portptr != NULL) *portptr = sk->sin6_port;
    }
  else
    {
    struct sockaddr_in *sk = (struct sockaddr_in *)arg;
    yield = (char *)inet_ntop(family, &(sk->sin_addr), addr_buffer,
      sizeof(addr_buffer));
    if (portptr != NULL) *portptr = sk->sin_port;
    }
  }
else
  {
  yield = (char *)inet_ntop(type, arg, addr_buffer, sizeof(addr_buffer));
  }

/* If the result is a mapped IPv4 address, show it in V4 format. */

if (strncmp(yield, "::ffff:", 7) == 0) yield += 7;

#else  /* HAVE_IPV6 */

/* The old world */

if (type < 0)
  {
  yield = inet_ntoa(((struct sockaddr_in *)arg)->sin_addr);
  if (portptr != NULL) *portptr = ((struct sockaddr_in *)arg)->sin_port;
  }
else
  yield = inet_ntoa(*((struct in_addr *)arg));
#endif

/* If there is no buffer, put the string into some new store. */

if (buffer == NULL) return string_copy(yield);
strcpy(buffer, yield);
return buffer;
}




/*************************************************
*         Convert address text to binary         *
*************************************************/

/* Given the textual form of an IP address, convert it to binary in an
array of ints. IPv4 addresses occupy one int; IPv6 addresses occupy 4 ints.
The result has the first byte in the most significant byte of the first int. In
other words, the result is not in network byte order, but in host byte order.
As a result, this is not the converse of host_ntoa(), which expects network
byte order. See host_nmtoa() below.

Arguments:
  address    points to the textual address, checked for syntax
  bin        points to an array of 4 ints

Returns:     the number of ints used
*/

int
host_aton(char *address, int *bin)
{
int x[4];
int v4offset = 0;

/* Handle IPv6 address, which may end with an IPv4 address */

#if HAVE_IPV6
if (strchr(address, ':') != NULL)
  {
  char *p = address;
  char *component[8];
  BOOL ipv4_ends = FALSE;
  int ci = 0;
  int nulloffset = 0;
  int v6count = 8;
  int i;

  /* If the address starts with a colon, it will start with two colons.
  Just lose the first one, which will leave a null first component. */

  if (*p == ':') p++;

  /* Split the address into components separated by colons. */

  while (*p != 0)
    {
    int len = strcspn(p, ":");
    if (len == 0) nulloffset = ci;
    if (ci > 7) log_write(0, LOG_MAIN|LOG_PANIC_DIE,
      "Internal error: invalid IPv6 address \"%s\" passed to host_aton()",
      address);
    component[ci++] = p;
    p += len;
    if (*p == ':') p++;
    }

  /* If the final component contains a dot, it is a trailing v4 address.
  As the syntax is known to be checked, just set up for a trailing
  v4 address and restrict the v6 part to 6 components. */

  if (strchr(component[ci-1], '.') != NULL)
    {
    address = component[--ci];
    ipv4_ends = TRUE;
    v4offset = 3;
    v6count = 6;
    }

  /* If there are fewer than 6 or 8 components, we have to insert some
  more empty ones in the middle. */

  if (ci < v6count)
    {
    int insert_count = v6count - ci;
    for (i = v6count-1; i > nulloffset + insert_count; i--)
      component[i] = component[i - insert_count];
    while (i > nulloffset) component[i--] = "";
    }

  /* Now turn the components into binary in pairs and bung them
  into the vector of ints. */

  for (i = 0; i < v6count; i += 2)
    bin[i/2] = (strtol(component[i], NULL, 16) << 16) +
      strtol(component[i+1], NULL, 16);

  /* If there was no terminating v4 component, we are done. */

  if (!ipv4_ends) return 4;
  }
#endif

/* Handle IPv4 address */

sscanf(address, "%d.%d.%d.%d", x, x+1, x+2, x+3);
bin[v4offset] = (x[0] << 24) + (x[1] << 16) + (x[2] << 8) + x[3];
return v4offset+1;
}


/*************************************************
*           Apply mask to an IP address          *
*************************************************/

/* Mask an address held in 1 or 4 ints, with the ms bit in the ms bit of the
first int, etc.

Arguments:
  count        the number of ints
  binary       points to the ints to be masked
  mask         the count of ms bits to leave, or -1 if no masking

Returns:       nothing
*/

void
host_mask(int count, int *binary, int mask)
{
int i;
if (mask < 0) mask = 99999;
for (i = 0; i < count; i++)
  {
  int wordmask;
  if (mask == 0) wordmask = 0;
  else if (mask < 32)
    {
    wordmask = (-1) << (32 - mask);
    mask = 0;
    }
  else
    {
    wordmask = -1;
    mask -= 32;
    }
  binary[i] &= wordmask;
  }
}




/*************************************************
*     Convert masked IP address in ints to text  *
*************************************************/

/* We can't use host_ntoa() because it assumes the binary values are in network
byte order, and these are the result of host_aton(), which puts them in ints in
host byte order. Also, we really want IPv6 addresses to be in a canonical
format, so we output them with no abbreviation. However, we can't use the
normal colon separator in them because it terminates keys in lsearch files, so
use dot instead.

Arguments:
  count       1 or 4 (number of ints)
  binary      points to the ints
  mask        mask value; if < 0 don't add to result
  buffer      big enough to hold the result

Returns:      the number of characters placed in buffer, not counting
              the final nul.
*/

int
host_nmtoa(int count, int *binary, int mask, char *buffer)
{
int i, j;
char *tt = buffer;

if (count == 1)
  {
  j = binary[0];
  for (i = 24; i >= 0; i -= 8)
    {
    sprintf(tt, "%d.", (j >> i) & 255);
    while (*tt) tt++;
    }
  }
else
  {
  for (i = 0; i < 4; i++)
    {
    j = binary[i];
    sprintf(tt, "%04x.%04x.", (j >> 16) & 0xffff, j & 0xffff);
    while (*tt) tt++;
    }
  }

tt--;   /* lose final . */

if (mask < 0)
  *tt = 0;
else
  {
  sprintf(tt, "/%d", mask);
  while (*tt) tt++;
  }

return tt - buffer;
}



/*************************************************
*       Scan host list for local hosts           *
*************************************************/

/* Scan through a chain of addresses and check whether any of them is the
address of an interface on the local machine. If so, remove that address and
any previous ones with the same MX value, and all subsequent ones (which will
have greater or equal MX values) from the chain. Note: marking them as unusable
is NOT the right thing to do because it causes the hosts not to be used for
other domains, for which they may well be correct.

There is also a list of "pseudo-local" host names which are checked against the
host names. Any match causes that host item to be treated the same as one which
matches a local IP address.

If the very first host is a local host, then all MX records had a precedence
greater than or equal to that of the local host. Either there's a problem in
the DNS, or an apparently remote name turned out to be an abbreviation for the
local host. Give a specific return code, and let the caller decide what to do.
Otherwise, give a success code if at least one host address has been found.

Arguments:
  host        the first host in the chain
  last        the last host in the chain
  removed     if not NULL, set TRUE if some local addresses were removed
                from the list

Returns:
  HOST_FOUND       if there is at least one host with an IP address on the chain
                     and an MX value less than any MX value associated with the
                     local host
  HOST_FOUND_LOCAL if a local host is among the lowest-numbered MX hosts; when
                     the host addresses were obtained from A records or
                     gethostbyname(), the MX values are set to -1.
  HOST_FIND_FAILED if no valid hosts with set IP addresses were found
*/

int
host_scan_for_local_hosts(host_item *host, host_item *last, BOOL *removed)
{
int yield = HOST_FIND_FAILED;
host_item *prev = NULL;
host_item *h;

if (removed != NULL) *removed = FALSE;

if (local_interface_data == NULL) local_interface_data = host_find_interfaces();

for (h = host; h != last->next; h = h->next)
  {
  #ifndef STAND_ALONE
  if (hosts_treat_as_local != NULL &&
    match_isinlist(string_copylc(h->name), &hosts_treat_as_local, TRUE, TRUE,
      NULL)) goto FOUND_LOCAL;
  #endif

  if (h->address != NULL)
    {
    ip_address_item *ip;
    for (ip = local_interface_data; ip != NULL; ip = ip->next)
      if (strcmp(h->address, ip->address) == 0) goto FOUND_LOCAL;
    yield = HOST_FOUND;  /* At least one remote address has been found */
    }

  /* Update prev to point to the last host item before any that have
  the same MX value as the one we have just considered. */

  if (h->next == NULL || h->next->mx != h->mx) prev = h;
  }

return yield;  /* No local hosts found: return HOST_FOUND or HOST_FIND_FAILED */


/* A host whose IP address matches a local IP address, or whose name matches
something in hosts_treat_as_local has been found. */

FOUND_LOCAL:

if (prev == NULL)
  {
  DEBUG(8) debug_printf((h->mx >= 0)?
    "local host has lowest MX\n" :
    "local host found for non-MX address\n");
  return HOST_FOUND_LOCAL;
  }

DEBUG(8)
  {
  debug_printf("local host in list - removed hosts:\n");
  for (h = prev->next; h != last->next; h = h->next)
    debug_printf("  %s %s %d\n", h->name, h->address, h->mx);
  }

if (removed != NULL) *removed = TRUE;
prev->next = NULL;
return yield;
}




/*************************************************
*        Find host name for incoming call        *
*************************************************/

/* For the moment, just use gethostbyaddr, and pass back a single string, in
permanent store. The incoming address is in sender_host_address, either in
dotted-quad form for IPv4 or in colon-separated form for IPv6.

Arguments:    none
Returns:      TRUE on success, the answer being placed in the global variable
              sender_host_name, with any aliases in a list hung off
              sender_host_aliases

The variable host_lookup_msg is set to an empty string on sucess, or to a
reason for the failure otherwise, in a form suitable for tagging onto an error
message, and also host_lookup_failed is set TRUE if the lookup failed. Any
dynamically constructed string for host_lookup_msg must be in permanent store,
because it might be used for several incoming messages on the same SMTP
connection. */

BOOL
host_name_lookup(void)
{
char *s, *t;
int len;
struct hostent *hosts;
struct in_addr addr;

HDEBUG(2) debug_printf("looking up host name for %s\n", sender_host_address);

/* Lookup on IPv6 system */

#if HAVE_IPV6
if (strchr(sender_host_address, ':') != NULL)
  {
  struct in6_addr addr6;
  if (inet_pton(AF_INET6, sender_host_address, &addr6) != 1)
    log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", sender_host_address);
  hosts = gethostbyaddr((char *)(&addr6), sizeof(addr6), AF_INET6);
  }
else
  {
  if (inet_pton(AF_INET, sender_host_address, &addr) != 1)
    log_write(0, LOG_PANIC_DIE, "unable to parse \"%s\"", sender_host_address);
  hosts = gethostbyaddr((char *)(&addr), sizeof(addr), AF_INET);
  }

/* Do lookup on IPv4 system */

#else
addr.s_addr = (S_ADDR_TYPE)inet_addr(sender_host_address);
hosts = gethostbyaddr((char *)(&addr), sizeof(addr), AF_INET);
#endif

/* Failed to look up the host. NB host_lookup_msg must be in permant store. */

if (hosts == NULL)
  {
  HDEBUG(2) debug_printf("IP address lookup failed\n");
  host_lookup_msg = " (failed to find host name from IP address)";
  host_lookup_failed = TRUE;
  return FALSE;
  }

/* Copy and lowercase the name, which is in static storage in many systems. */

s = (char *)hosts->h_name;
len = (int)strlen(s) + 1;
t = sender_host_name = store_malloc(len);
while (*s != 0) *t++ = tolower(*s++);
*t = 0;

HDEBUG(2) debug_printf("IP address lookup yielded %s\n", sender_host_name);

/* If the host has aliases, build a copy of the alias list */

if (hosts->h_aliases != NULL)
  {
  int count = 1;
  char **aliases, **ptr;
  for (aliases = hosts->h_aliases; *aliases != NULL; aliases++) count++;
  ptr = sender_host_aliases = store_malloc(count * sizeof(char *));
  for (aliases = hosts->h_aliases; *aliases != NULL; aliases++)
    {
    char *s = *aliases;
    int len = (int)strlen(s) + 1;
    char *t = *ptr++ = store_malloc(len);
    while (*s != 0) *t++ = tolower(*s++);
    *t = 0;
    HDEBUG(2) debug_printf("Alias %s\n", ptr[-1]);
    }
  *ptr = NULL;
  }

/* At this point we used to verify that a forward lookup did indeed correspond
to the address we first had, but it seems that gethostbyaddr() does this check
itself. So rather than waste time checking the name and all its aliases, we now
rely on that. */

return TRUE;
}




/*************************************************
*    Find IP address(es) for host by name        *
*************************************************/

/* The input is a host_item structure with the name filled in and the address
field set to NULL. We use gethostbyname(). Of course, gethostbyname() may use
the DNS, but it doesn't do MX processing. If more than one address is given,
chain on additional host items, with other relevant fields copied. The type of
store required for them is set by use_malloc.

The second argument provides a host list (usually an IP list) of hosts to
ignore. This makes it possible to ignore IPv6 link-local addresses or loopback
addresses in unreasonable places.

The lookup may result in a change of name. For compatibility with the dns
lookup, return this via fully_qualified_name as well as updating the host item.
The lookup may also yield more than one IP address, in which case chain on
subsequent host_item structures.

Arguments:
  host                   a host item with the name filled in; the address is
                         to be filled in; multiple IP addresses cause other
                         host items to be chained on.
  ignore_target_hosts    a list of hosts to ignore
  fully_qualified_name   if not NULL, set to point to host name for
                         compatibility with host_find_bydns
  use_malloc             TRUE if additional items must be in permanent store

Returns:                 HOST_FIND_FAILED  Failed to find the host or domain
                         HOST_FOUND        Host found
                         HOST_FOUND_LOCAL  Host found and is the local host
*/

int
host_find_byname(host_item *host, char *ignore_target_hosts,
  char **fully_qualified_name, BOOL use_malloc)
{
int i, yield, times;
char **addrlist;
host_item *last = NULL;

/* In an IPv6 world, we need to scan for both kinds of address, so go round the
loop twice. Note that we have ensured that AF_INET6 is defined even in an IPv4
world, which makes for slightly tidier code. However, if ipv4_address_lookup is
set (originally to disable IPv6 DNS lookups), we also just do IPv4 lookups
here. */

#if HAVE_IPV6
int af;
if (ipv4_address_lookup)
  { af = AF_INET; times = 1; }
else
  { af = AF_INET6; times = 2; }
#else
times = 1;
#endif

/* Initialize the flag that gets set for DNS syntax check errors, so that the
interface to this function can be the same as host_find_bydns. */

host_find_failed_syntax = FALSE;

/* Loop to look up both kinds of address in an IPv6 world */

for (i = 1; i <= times; i++)
  {
  BOOL ipv4_addr;
  struct hostent *hostdata;

  #if HAVE_IPV6
    #if HAVE_GETIPNODEBYNAME
    int error_num;
    hostdata = getipnodebyname(host->name, af, 0, &error_num);
    #else
    hostdata = gethostbyname2(host->name, af);
    #endif
  af = AF_INET;   /* For next time round the loop */
  #else
  hostdata = gethostbyname(host->name);
  #endif

  if (hostdata == NULL || (hostdata->h_addr_list)[0] == NULL) continue;

  /* Replace the name with the fully qualified one if necessary, and fill in
  the fully_qualified_name pointer. */

  if (strcmp(host->name, hostdata->h_name) != 0)
    host->name = use_malloc? string_copy_malloc((char *)hostdata->h_name) :
      string_copy((char *)hostdata->h_name);
  if (fully_qualified_name != NULL) *fully_qualified_name = host->name;

  /* Get the list of addresses. IPv4 and IPv6 addresses can be distinguished
  by their different lengths. Scan the list, ignoring any that are to be
  ignored, and build a chain from the rest. */

  ipv4_addr = hostdata->h_length == sizeof(struct in_addr);

  for (addrlist = hostdata->h_addr_list; *addrlist != NULL; addrlist++)
    {
    char *text_address =
      host_ntoa(ipv4_addr? AF_INET:AF_INET6, *addrlist, NULL, NULL);

    if (ignore_target_hosts != NULL &&
        verify_check_this_host(&ignore_target_hosts, FALSE, host->name,
          text_address, NULL))
      {
      DEBUG(8) debug_printf("ignored host %s [%s]\n", host->name, text_address);
      continue;
      }

    /* If this is the first address, last == NULL and we put the data in the
    original block. */

    if (last == NULL)
      {
      host->address = text_address;
      host->mx = -1;
      host->status = hstatus_unknown;
      host->why = hwhy_unknown;
      last = host;
      }

    /* Else add further host item blocks for any other addresses, keeping
    the order. */

    else
      {
      host_item *next = use_malloc?
        store_malloc(sizeof(host_item)) : store_get(sizeof(host_item));
      next->name = host->name;
      next->ident_string = host->ident_string;
      next->address = text_address;
      next->status = hstatus_unknown;
      next->why = hwhy_unknown;
      next->last_try = 0;
      next->mx = -1;
      next->next = last->next;
      last->next = next;
      last = next;
      }
    }
  }

/* If no hosts were found, the address field in the original host block will be
NULL. */

if (host->address == NULL) return HOST_FIND_FAILED;

/* Now check to see if this is the local host. This also checks for the
presence of a host with an IP address, and returns HOST_FOUND, HOST_FOUND_LOCAL
or HOST_FOUND_FAILED. */

yield = host_scan_for_local_hosts(host, last, NULL);

DEBUG(8)
  {
  host_item *h;
  if (fully_qualified_name != NULL)
    debug_printf("fully qualified name = %s\n", *fully_qualified_name);
  for (h = host; h != last->next; h = h->next)
    debug_printf("%s %s mx=%d %s\n", h->name,
      (h->address == NULL)? "<null>" : h->address, h->mx,
      (h->status >= hstatus_unusable)? "*" : "");
  }

/* Return the found status. */

return yield;
}



/*************************************************
*        Fill in a host address from the DNS     *
*************************************************/

/* Given a host item, with its name and mx fields set, and its address field
set to NULL, fill in its IP address from the DNS. If it is multi-homed, create
additional host items for the additional addresses, copying all the other
fields, and randomizing the order.

On IPv6 systems, A6 records are sought first (but only if support for A6 is
configured - they may never become mainstream), then AAAA records are sought,
and finally A records are sought as well.

The host name may be changed if the DNS returns a different name - e.g. fully
qualified or changed via CNAME. If fully_qualified_name is not NULL, dns_lookup
ensures that it points to the fully qualified name. However, this is the fully
qualified version of the original name; if a CNAME is involved, the actual
canonical host name may be different again, and so we get it directly from the
relevant RR. Note that we do NOT change the mx field of the host item in this
function as it may be called to set the addresses of hosts taken from MX
records.

Arguments:
  host                  points to initial host item
  ignore_target_hosts   list of hosts to ignore
  allow_ip              if TRUE, recognize an IP address and return it
  fully_qualified_name  if not NULL, return fully qualified name here if
                          the contents are different (i.e. it must be preset
                          to something)

Returns:       HOST_FIND_FAILED     couldn't find A record
               HOST_FIND_AGAIN      try again later
               HOST_FOUND           found AAAA and/or A record(s)
               HOST_IGNORED         found, but all IPs ignored
*/

static int
set_address_from_dns(host_item *host, char *ignore_target_hosts, BOOL allow_ip,
  char **fully_qualified_name)
{
dns_record *rr;
host_item *last = NULL;
BOOL v6_find_again = FALSE;
int i;

/* If allow_ip is set, a name which is an IP address returns that value
as its address. This is used for MX records when allow_mx_to_ip is set, for
those sites that feel they have to flaunt the RFC rules. */

if (allow_ip && string_is_ip_address(host->name, NULL) != 0)
  {
  if (ignore_target_hosts != NULL &&
        verify_check_this_host(&ignore_target_hosts, FALSE, host->name,
        host->name, NULL))
    {
    return HOST_IGNORED;
    }
  host->address = host->name;
  return HOST_FOUND;
  }

/* On an IPv6 system, go round the loop up to three times, looking for A6 and
AAAA records the first two times, unless ipv4_address_lookup is set. However,
since A6 records look like being abandoned, support them only if explicitly
configured to do so. On an IPv4 system, go round the loop once only, looking
only for A records. */

#if HAVE_IPV6
  #ifdef SUPPORT_A6
  i = ipv4_address_lookup? 0 : 2;
  #else
  i = ipv4_address_lookup? 0 : 1;
  #endif  /* SUPPORT_A6 */
#else   /* HAVE_IPV6 */
i = 0;
#endif  /* HAVE_IPV6 */

for (; i >= 0; i--)
  {
  static int types[] = { T_A, T_AAAA, T_A6 };
  int type = types[i];
  dns_answer dnsa;
  dns_scan dnss;

  int rc = dns_lookup(&dnsa, host->name, type, fully_qualified_name);

  /* We want to return HOST_FIND_AGAIN if one of the A, A6, or AAAA lookups
  fails or times out, but not if another one succeeds. (In the early
  IPv6 days there are name servers that always fail on AAAA, but are happy
  to give out an A record. We want to proceed with that A record.) */

  if (rc != DNS_SUCCEED)
    {
    if (i == 0)  /* Just tried for an A record, i.e. end of loop */
      {
      if (host->address != NULL) return HOST_FOUND;  /* A6 or AAAA was found */
      if (rc == DNS_AGAIN || rc == DNS_FAIL || v6_find_again)
        return HOST_FIND_AGAIN;
      return HOST_FIND_FAILED;
      }

    /* Tried for an A6 or AAAA record: remember if this was a temporary
    error, and look for the next record type. */

    if (rc != DNS_NOMATCH) v6_find_again = TRUE;
    continue;
    }

  /* Lookup succeeded: fill in the given host item with the first non-ignored
  address found; create additional items for any others. A single A6 record
  may generate more than one address. */

  for (rr = dns_next_rr(&dnsa, &dnss, RESET_ANSWERS);
       rr != NULL;
       rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT))
    {
    if (rr->type == type)
      {
      dns_address *da;
      da = dns_address_from_rr(&dnsa, rr);

      DEBUG(9)
        {
        if (da == NULL)
          debug_printf("no addresses extracted from A6 RR for %s\n",
            host->name);
        }

      /* This loop runs only once for A and AAAA records, but may run
      several times for an A6 record that generated multiple addresses. */

      for (; da != NULL; da = da->next)
        {
        if (ignore_target_hosts != NULL &&
              verify_check_this_host(&ignore_target_hosts, FALSE, host->name,
              CS da->address, NULL))
          {
          DEBUG(8)
            debug_printf("ignored host %s [%s]\n", host->name, da->address);
          continue;
          }

        /* If this is the first address, stick it in the given host block,
        and change the name if the returned RR has a different name. */

        if (last == NULL)
          {
          if (strcmpic(host->name, rr->name) != 0)
            host->name = string_copy(rr->name);
          host->address = CS da->address;
          host->sort_key = host->mx * 100 + random_number(100);
          host->status = hstatus_unknown;
          host->why = hwhy_unknown;
          last = host;
          }

        /* Not the first address. Check for, and ignore, duplicates. Then
        insert in the chain at a random point. */

        else
          {
          int new_sort_key;
          host_item *next;

          /* End of our local chain is specified by "last". */

          for (next = host;; next = next->next)
            {
            if (strcmp(CS da->address, next->address) == 0) break;
            if (next == last) { next = NULL; break; }
            }
          if (next != NULL) continue;  /* With loop for next address */

          /* Not a duplicate */

          new_sort_key = host->mx * 100 + random_number(100);
          next = store_get(sizeof(host_item));

          /* New address goes first: insert the new block after the first one
          (so as not to disturb the original pointer) but put the new address
          in the original block. */

          if (new_sort_key < host->sort_key)
            {
            *next = *host;
            host->next = next;
            host->address = CS da->address;
            host->sort_key = new_sort_key;
            if (last == host) last = next;
            }

          /* Otherwise scan down the addresses for this host to find the
          one to insert after. */

          else
            {
            host_item *h = host;
            while (h != last)
              {
              if (new_sort_key < h->next->sort_key) break;
              h = h->next;
              }
            *next = *h;
            h->next = next;
            next->address = CS da->address;
            next->sort_key = new_sort_key;
            if (h == last) last = next;      /* Inserted after last */
            }
          }
        }
      }
    }
  }

/* Control gets here only if the final lookup (the A record) succeeded.
However, the address may not be filled in if it was ignored. */

return (host->address == NULL)? HOST_IGNORED : HOST_FOUND;
}




/*************************************************
*  Find IP addresses and names for host via DNS  *
*************************************************/

/* The input is a host_item structure with the name filled in and the address
field set to NULL. The lookup may result in more than one IP address, in which
case chain on subsequent host_item structures. The names of the hosts are in
general different to the original domain name, but in addition, the original
name may not be fully qualified. Use the fully_qualified_name argument to
return the official name, as returned by the resolver.

Arguments:
  host                  point to initial host item
  ignore_target_hosts   a list of hosts to ignore
  mx_only               if TRUE, require MX record(s) to exist
  a_only                if TRUE, don't look for MX records
  qualify_single        if TRUE, ask DNS to qualify single-component names
  search_parents        if TRUE, asd DNS to search parent domains
  fully_qualified_name  if not NULL, return fully-qualified name
  removed               set TRUE if local host was removed from the list

Returns:                HOST_FIND_FAILED  Failed to find the host or domain;
                                          if there was a syntax error,
                                          host_find_failed_syntax is set.
                        HOST_FIND_AGAIN   Could not resolve at this time
                        HOST_FOUND        Host found
                        HOST_FOUND_LOCAL  The lowest MX record points to this
                                          machine, if MX records were found, or
                                          an A record that was found contains
                                          an address of the local host
*/

int
host_find_bydns(host_item *host, char *ignore_target_hosts,  BOOL mx_only,
  BOOL a_only, BOOL qualify_single, BOOL search_parents,
  char **fully_qualified_name, BOOL *removed)
{
host_item *h, *last;
dns_record *rr;
int rc = DNS_FAIL;
int yield;
dns_answer dnsa;
dns_scan dnss;

/* Set the default fully qualified name to the incoming name, initialize the
resolver if necessary, and set up the relevant options. */

if (fully_qualified_name != NULL) *fully_qualified_name = host->name;
dns_init(qualify_single, search_parents);

/* Search the DNS for MX records, possibly via a CNAME. First, initialize the
flag that gets set for DNS syntax check errors. */

host_find_failed_syntax = FALSE;

if (!a_only)
  {
  rc = dns_lookup(&dnsa, host->name, T_MX, fully_qualified_name);
  if (rc == DNS_FAIL || rc == DNS_AGAIN) return HOST_FIND_AGAIN;
  }

/* If there were no MX records and mx_only is FALSE, or if a_only is TRUE,
try for an A record, unless there was a syntax error in the name that was used
for the MX lookup. If we find it (or them) check to see that it isn't the local
host. */

if (rc != DNS_SUCCEED)
  {
  if (host_find_failed_syntax) return HOST_FIND_FAILED;

  if (!mx_only || a_only)
    {
    host_item *next = host->next;
    last = host;
    host->mx = -1;
    rc = set_address_from_dns(host, ignore_target_hosts, FALSE,
      fully_qualified_name);

    /* If one or more A records have been found, find the last one and
    check that none of them are local. Since we know the host items all
    have their IP addresses inserted, host_scan_for_local_hosts() can only
    return HOST_FOUND or HOST_FOUND_LOCAL. */

    if (rc == HOST_FOUND)
      {
      while (last->next != next) last = last->next;
      rc = host_scan_for_local_hosts(host, last, removed);
      }
    else
      if (rc == HOST_IGNORED) rc = HOST_FIND_FAILED;  /* No special action */

    DEBUG(8)
      {
      host_item *h;
      if (host->address != NULL)
        {
        if (fully_qualified_name != NULL)
          debug_printf("fully qualified name = %s\n", *fully_qualified_name);
        for (h = host; h != next; h = h->next)
          debug_printf("%s %s mx=%d sort=%d %s\n", h->name,
            (h->address == NULL)? "<null>" : h->address, h->mx, h->sort_key,
            (h->status >= hstatus_unusable)? "*" : "");
        }
      }

    return rc;
    }

  DEBUG(8) debug_printf("No MX records, and mx_only is TRUE\n");
  return HOST_FIND_FAILED;
  }

/* We have found one or more MX records. Sort them according to precedence, and
create host_item blocks for them. For equal precedences one is supposed to
randomize the order. To make this happen, the sorting is actually done on the
MX value * 100 + a random number. This is put into a host field called
sort_key. Remove any duplicates that point to the same host, retaining only the
one with the lowest precedence. We cannot yet check for precedence greater than
that of the local host, because that test cannot be properly done until the
addresses have been found - an MX record may point to a name for this host
which is not the primary hostname. */

last = NULL;

for (rr = dns_next_rr(&dnsa, &dnss, RESET_ANSWERS);
     rr != NULL;
     rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT))
  {
  int precedence;
  uschar *s;             /* MUST be unsigned for GETSHORT */
  char data[256];

  if (rr->type != T_MX) continue;
  s = rr->data;
  GETSHORT(precedence, s);      /* Pointer s is advanced */

  /* Get the name of the host pointed to. */

  (void)dn_expand(dnsa.answer, dnsa.answer + dnsa.answerlen, s,
    (DN_EXPAND_ARG4_TYPE)data, sizeof(data));

  /* Check that we haven't already got this host on the chain; if we have,
  keep only the lower precedence. This situation shouldn't occur, but you
  never know what junk might get into the DNS (and this case has been seen on
  more than one occasion). */

  if (last != NULL)
    {
    host_item *prev = NULL;

    for (h = host; h != last->next; prev = h, h = h->next)
      {
      if (strcmpic(h->name, data) == 0)
        {
        if (precedence >= h->mx) goto NEXT_MX_RR; /* Skip greater precedence */
        if (h == host)                            /* Override first item */
          {
          h->mx = precedence;
          host->sort_key = precedence * 100 + random_number(100);
          goto NEXT_MX_RR;
          }

        /* Unwanted host item is not the first in the chain, so we can get
        get rid of it by cutting it out. */

        prev->next = h->next;
        if (h == last) last = prev;
        break;
        }
      }
    }

  /* If this is the first MX record, put the data into the existing host
  block. Otherwise, add a new block in the correct place; it it has to be
  before the first block, copy the first block's data to a new second block. */

  if (last == NULL)
    {
    host->name = string_copy(data);
    host->address = NULL;
    host->mx = precedence;
    host->sort_key = precedence * 100 + random_number(100);
    host->status = hstatus_unknown;
    host->why = hwhy_unknown;
    last = host;
    }

  /* Make a new host item and seek the correct insertion place */

  else
    {
    int sort_key = precedence * 100 + random_number(100);

    host_item *next = store_get(sizeof(host_item));
    next->name = string_copy(data);
    next->address = NULL;
    next->mx = precedence;
    next->sort_key = sort_key;
    next->status = hstatus_unknown;
    next->why = hwhy_unknown;
    next->last_try = 0;

    /* Handle the case when we have to insert before the first item. */

    if (sort_key < host->sort_key)
      {
      host_item htemp;
      htemp = *host;
      *host = *next;
      *next = htemp;
      host->next = next;
      if (last == host) last = next;
      }

    /* Else scan down the items we have inserted as part of this exercise;
    don't go further. */

    else
      {
      for (h = host; h != last; h = h->next)
        {
        if (sort_key < h->next->sort_key)
          {
          next->next = h->next;
          h->next = next;
          break;
          }
        }

      /* Join on after the last host item that's part of this
      processing if we haven't stopped sooner. */

      if (h == last)
        {
        next->next = last->next;
        last->next = next;
        last = next;
        }
      }
    }

  NEXT_MX_RR: continue;
  }


/* Now we have to ensure addresses exist for all the hosts. We have ensured
above that the names in the host items are all unique. The addresses
may have been returned in the additional data section of the DNS query.
Because it is more expensive to scan the returned DNS records (because you
have to expand the names) we do a single scan over them, and multiple scans
of the chain of host items (which is typically only 3 or 4 long anyway.)
Add extra host items for multi-homed hosts. */

for (rr = dns_next_rr(&dnsa, &dnss, RESET_ADDITIONAL);
     rr != NULL;
     rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT))
  {
  dns_address *da;
  int status = hstatus_unknown;
  int why = hwhy_unknown;

  if (rr->type != T_A
  #if HAVE_IPV6
    && rr->type != T_AAAA
    #ifdef SUPPORT_A6
    && rr->type != T_A6
    #endif
  #endif
    ) continue;

  /* Find the first host that matches this record's name. If there isn't
  one, move on to the next RR. */

  for (h = host; h != last->next; h = h->next)
    { if (strcmpic(h->name, rr->name) == 0) break; }
  if (h == last->next) continue;

  /* Get the list of textual addresses for this RR. There may be more than one
  if it is an A6 RR. Then loop to handle multiple addresses from an A6 record.
  If there are none, nothing will get done - the record is ignored. */

  for (da = dns_address_from_rr(&dnsa, rr); da != NULL; da = da->next)
    {
    /* Set status for an ignorable host. */

    if (ignore_target_hosts != NULL &&
          verify_check_this_host(&ignore_target_hosts, FALSE, h->name,
          CS da->address, NULL))
      {
      DEBUG(8) debug_printf("ignored host %s [%s]\n", h->name, da->address);
      status = hstatus_unusable;
      why = hwhy_ignored;
      }

    /* If the address is already set for this host, it may be that
    we just have a duplicate DNS record. Alternatively, this may be
    a multi-homed host. Search all items with the same host name
    (they will all be together) and if this address is found, skip
    to the next RR. */

    if (h->address != NULL)
      {
      int new_sort_key;
      host_item *thishostlast;
      host_item *hh = h;

      do
        {
        if (hh->address != NULL && strcmp(CS da->address, hh->address) == 0)
          goto DNS_NEXT_RR;         /* Need goto to escape from inner loop */
        thishostlast = hh;
        hh = hh->next;
        }
      while (hh != last->next && strcmpic(hh->name, rr->name) == 0);

      /* We have a multi-homed host, since we have a new address for
      an existing name. Create a copy of the current item, and give it
      the new address. RRs can be in arbitrary order, but one is supposed
      to randomize the addresses of multi-homed hosts, so compute a new
      sorting key and do that. [Latest SMTP RFC says not to randomize multi-
      homed hosts, but to rely on the resolver. I'm not happy about that -
      caching in the resolver will not rotate as often as the name server
      does.] */

      new_sort_key = h->mx * 100 + random_number(100);
      hh = store_get(sizeof(host_item));

      /* New address goes first: insert the new block after the first one
      (so as not to disturb the original pointer) but put the new address
      in the original block. */

      if (new_sort_key < h->sort_key)
        {
        *hh = *h;
        h->next = hh;
        h->address = CS da->address;
        h->sort_key = new_sort_key;
        h->status = status;
        h->why = why;
        }

      /* Otherwise scan down the addresses for this host to find the
      one to insert after. */

      else
        {
        while (h != thishostlast)
          {
          if (new_sort_key < h->next->sort_key) break;
          h = h->next;
          }
        *hh = *h;
        h->next = hh;
        hh->address = CS da->address;
        hh->sort_key = new_sort_key;
        hh->status = status;
        hh->why = why;
        }

      if (h == last) last = hh;       /* Inserted after last */
      }

    /* The existing item doesn't have its address set yet, so just set it. */

    else
      {
      h->address = CS da->address;
      h->status = status;
      h->why = why;
      }
    }    /* Loop for addresses extracted from one RR */

  /* Carry on to the next RR. It would be nice to be able to be able to stop
  when every host on the list has an address, but we can't be sure there won't
  be an additional address for a multi-homed host further down the list, so
  we have to continue to the end. */

  DNS_NEXT_RR: continue;
  }

/* Set the default yield to failure */

yield = HOST_FIND_FAILED;

/* If we haven't found all the addresses in the additional section, we
need to search for A or AAAA records explicitly. The names shouldn't point to
CNAMES, but we use the general lookup function that handles them, just
in case. If any lookup gives a soft error, change the default yield. */

for (h = host; h != last->next; h = h->next)
  {
  if (h->address != NULL || h->status == hstatus_unusable) continue;
  rc = set_address_from_dns(h, ignore_target_hosts, allow_mx_to_ip, NULL);
  if (rc != HOST_FOUND)
    {
    h->status = hstatus_unusable;
    if (rc == HOST_FIND_AGAIN)
      {
      yield = rc;
      h->why = hwhy_deferred;
      }
    else
      h->why = (rc == HOST_IGNORED)? hwhy_ignored : hwhy_failed;
    }
  }

/* Scan the list for any hosts that are marked unusable because they have
been explicitly ignored, and remove them from the list, as if they did not
exist. If we end up with just a single, ignored host, flatten its fields as if
nothing was found. */

if (ignore_target_hosts != NULL)
  {
  host_item *prev = NULL;
  for (h = host; h != last->next; h = h->next)
    {
    REDO:
    if (h->why != hwhy_ignored)   /* Non ignored host, just continue */
      prev = h;
    else if (prev == NULL)        /* First host is ignored */
      {
      if (h != last)              /* First is not last, overwrite it */
        {
        if (h->next == last) last = h;
        *h = *(h->next);          /* and reprocess it. */
        goto REDO;                /* C should have redo, like Perl */
        }
      }
    else                          /* Ignored host is not first - cut it out */
      {
      prev->next = h->next;
      if (h == last) last = prev;
      }
    }

  if (host->why == hwhy_ignored)
    {
    host->address = NULL;
    }
  }

/* Scan the list of hosts for any whose IP addresses are on the local host.
If any are found, all hosts with the same or higher MX values are removed.
However, if the local host has the lowest numbered MX, then HOST_FOUND_LOCAL
is returned. Otherwise, if at least one host with an IP address is on the list,
HOST_FOUND is returned. Otherwise, HOST_FIND_FAILED is returned, but in this
case do not update the yield, as it might have been set to HOST_FIND_AGAIN
just above here. If not, it will already be HOST_FIND_FAILED. */

rc = host_scan_for_local_hosts(host, last, removed);
if (rc != HOST_FIND_FAILED) yield = rc;

DEBUG(8)
  {
  if (fully_qualified_name != NULL)
    debug_printf("fully qualified name = %s\n", *fully_qualified_name);
  debug_printf("host_find_bydns yield = %s (%d); returned hosts:\n",
    (yield == HOST_FOUND)? "HOST_FOUND" :
    (yield == HOST_FOUND_LOCAL)? "HOST_FOUND_LOCAL" :
    (yield == HOST_FIND_AGAIN)? "HOST_FIND_AGAIN" :
    (yield == HOST_FIND_FAILED)? "HOST_FIND_FAILED" : "?",
    yield);
  for (h = host; h != last->next; h = h->next)
    debug_printf("  %s %s %d %d %s\n", h->name,
      (h->address == NULL)? "<null>" : h->address, h->mx, h->sort_key,
      (h->status >= hstatus_unusable)? "*" : "");
  }

return yield;
}



/*************************************************
*        Check an IP address against an RBL      *
*************************************************/

/* Anti-spamming "realtime blocking lists" are appearing on the DNS. This
function, given an IP address and the domain of one such list, looks up the
address in the blocking domain. If a list of IP addresses is given, the
returned A record is compared to them, and ignored unless one of them matches.
This allows for handling different categories of entry in some RBL lists.

Arguments:
  address      the string version of the address
  domain       the rbl domain to use
  iplist       a list of IP addreses to match against, or NULL
  buffer       somewhere to point an error message

Returns:    DNS_SUCCEED   successful lookup (i.e. the address is blocked)
            DNS_NOMATCH   name not found, or no data found for the given type
            DNS_AGAIN     soft failure, try again later
            DNS_FAIL      DNS failure
*/

int
host_check_rbl(char *address, char *domain, char *iplist, char **bufptr)
{
int bin[4];
int rc;
int type = T_A;
char name[256];
char *t = name;
dns_answer dnsa;
dns_scan dnss;

/* In case this is the first time the DNS resolver is being used. */

dns_init(FALSE, FALSE);

/* If this is an IPv4 address mapped into IPv6 format, adjust the pointer
to the IPv4 part only. */

if (strncmp(address, "::ffff:", 7) == 0) address += 7;

/* Handle IPv4 address: if HAVE_IPV6 is false, the result of
host_aton() is always 1. */

if (host_aton(address, bin) == 1)
  {
  int i;
  int x = bin[0];
  for (i = 0; i < 4; i++)
    {
    sprintf(t, "%d.", x & 255);
    while (*t) t++;
    x >>= 8;
    }
  }

/* Handle IPv6 address */

#if HAVE_IPV6
else
  {
  int i, j;
  for (j = 3; j >= 0; j--)
    {
    int x = bin[j];
    for (i = 0; i < 8; i++)
      {
      sprintf(t, "%x.", x & 15);
      while (*t) t++;
      x >>= 4;
      }
    }
  type = T_AAAA;
  }
#endif

/* Now do the DNS lookup */

strcpy(t, domain);
rc = dns_basic_lookup(&dnsa, name, type);

/* An entry was found in the DNS; check against the IP address list if
there is one. */

if (rc == DNS_SUCCEED)
  {
  if (iplist != NULL)
    {
    BOOL ok = FALSE;
    int sep = ',';             /* fixed comma separator */
    char *ptr = iplist;        /* save original for msg */
    char ip[46];               /* allow for IPv6 */

    while (!ok &&
	  (string_nextinlist(&ptr, &sep, ip, sizeof(ip)) != NULL))
      {
      dns_record *rr;
      for (rr = dns_next_rr(&dnsa, &dnss, RESET_ANSWERS);
	   rr != NULL && !ok;
	   rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT))
	{
        #if HAVE_IPV6
          #ifdef SUPPORT_A6
          if (rr->type == T_A || rr->type == T_A6 || rr->type == T_AAAA)
          #else
          if (rr->type == T_A || rr->type == T_AAAA)
          #endif
        #else
        if (rr->type == T_A)
        #endif
          {
          dns_address *da;
          for (da = dns_address_from_rr(&dnsa, rr);
               da != NULL && !ok;
               da = da->next)
            {
	    ok = strcmp(CS da->address, ip) == 0;
            if (ok) break;
            }
          }
	}
      }

    /* No IP address matched any of those in the list. In that case,
    behave as if the RBL lookup had not succeeded. */

    if (!ok)
      {
      HDEBUG(8)
	{
	debug_printf("RBL lookup for %s suceeded\n", name);
	debug_printf("=> that means it is black listed at %s,\n", domain);
	debug_printf("=> but we are not accepting this block class because\n");
        debug_printf("=> there was no match for %s\n", iplist);
	}
      return DNS_NOMATCH;
      }
    }

  /* Either there was no IP list, or the record matched. Set the buffer
  pointer to point to the null string, or to a TXT record's data if present
  after success. */

  *bufptr = "";
  if (dns_basic_lookup(&dnsa, name, T_TXT) == DNS_SUCCEED)
    {
    dns_record *rr;
    for (rr = dns_next_rr(&dnsa, &dnss, RESET_ANSWERS);
         rr != NULL;
         rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT))
      if (rr->type == T_TXT) break;
    if (rr != NULL)
      {
      int len = (rr->data)[0];
      if (len > 511) len = 127;
      *bufptr = string_sprintf("%.*s", len, (const char *)(rr->data + 1));
      }
    }

  HDEBUG(8)
    {
    debug_printf("RBL lookup for %s succeeded\n", name);
    debug_printf("=> that means it is black listed at %s\n", domain);
    if (**bufptr != 0) debug_printf("%s\n", *bufptr);
    }
  }

/* No entry was found in the DNS */

else HDEBUG(8)
  {
  debug_printf("RBL lookup for %s failed\n", name);
  debug_printf("=> that means it's not black listed at %s\n", domain);
  }

return rc;
}




/*************************************************
**************************************************
*             Stand-alone test program           *
**************************************************
*************************************************/

#ifdef STAND_ALONE

/* Note: an address for testing RBL is 192.203.178.39 */
/* Note: an address for testing DUL is 192.203.178.4  */

/* This is a dummy; won't be called in standalone testing */

BOOL
verify_check_this_host(char **listptr, BOOL find_failed_return,
   char *host_name, char *host_address, char *host_ident)
{
listptr = listptr;
find_failed_return = find_failed_return;
host_name = host_name;
host_address = host_address;
host_ident = host_ident;
return FALSE;
}


int main(int argc, char **argv)
{
host_item h;
BOOL byname = FALSE;
BOOL mx_only = FALSE;
BOOL a_only = FALSE;
BOOL qualify_single = TRUE;
BOOL search_parents = FALSE;
char buffer[256];

primary_hostname = "";
debug_level = 9;
debug_file = stdout;
debug_fd = fileno(debug_file);

printf("Exim stand-alone host functions test\n");

host_find_interfaces();
debug_level = 8;

if (argc > 1) primary_hostname = argv[1];

/* So that debug level changes can be done first */

dns_init(qualify_single, search_parents);

printf("Testing host lookup\n");
printf("> ");
while (fgets(buffer, 256, stdin) != NULL)
  {
  int rc;
  int len = (int)strlen(buffer);
  char *fully_qualified_name;

  while (len > 0 && isspace((uschar)buffer[len-1])) len--;
  buffer[len] = 0;

  if (strcmp(buffer, "q") == 0) break;

  if (strcmp(buffer, "byname") == 0) byname = TRUE;
  else if (strcmp(buffer, "no_byname") == 0) byname = FALSE;
  else if (strcmp(buffer, "a_only") == 0) a_only = TRUE;
  else if (strcmp(buffer, "no_a_only") == 0) a_only = FALSE;
  else if (strcmp(buffer, "mx_only") == 0) mx_only = TRUE;
  else if (strcmp(buffer, "no_mx_only") == 0) mx_only = FALSE;
  else if (strcmp(buffer, "qualify_single") == 0) qualify_single = TRUE;
  else if (strcmp(buffer, "no_qualify_single") == 0) qualify_single = FALSE;
  else if (strcmp(buffer, "search_parents") == 0) search_parents = TRUE;
  else if (strcmp(buffer, "no_search_parents") == 0) search_parents = FALSE;
  else if (strncmp(buffer, "retrans", 7) == 0)
    {
    sscanf(buffer+8, "%d", &dns_retrans);
    _res.retrans = dns_retrans;
    }
  else if (strncmp(buffer, "retry", 5) == 0)
    {
    sscanf(buffer+6, "%d", &dns_retry);
    _res.retry = dns_retry;
    }
  else if (isdigit((uschar)buffer[0]))
    {
    sscanf(buffer, "%d", &debug_level);
    _res.options &= ~RES_DEBUG;
    DEBUG(11) _res.options |= RES_DEBUG;
    }
  else
    {
    h.name = buffer;
    h.next = NULL;
    h.mx = -1;
    h.status = hstatus_unknown;
    h.why = hwhy_unknown;
    h.address = NULL;

    rc = byname? host_find_byname(&h, NULL, &fully_qualified_name, FALSE) :
      host_find_bydns(&h, NULL, mx_only, a_only, qualify_single, search_parents,
        &fully_qualified_name, NULL);

    if (rc == HOST_FIND_FAILED) printf("Failed\n");
      else if (rc == HOST_FIND_AGAIN) printf("Again\n");
        else if (rc == HOST_FOUND_LOCAL) printf("Local\n");
    }

  printf("\n> ");
  }

printf("Testing host_aton\n");
printf("> ");
while (fgets(buffer, 256, stdin) != NULL)
  {
  int i;
  int x[4];
  int len = (int)strlen(buffer);

  while (len > 0 && isspace((uschar)buffer[len-1])) len--;
  buffer[len] = 0;

  if (strcmp(buffer, "q") == 0) break;

  len = host_aton(buffer, x);
  printf("length = %d ", len);
  for (i = 0; i < len; i++)
    {
    printf("%04x ", (x[i] >> 16) & 0xffff);
    printf("%04x ", x[i] & 0xffff);
    }
  printf("\n> ");
  }

printf("\n");

printf("Testing host_name_lookup\n");
printf("> ");
while (fgets(buffer, 256, stdin) != NULL)
  {
  int len = (int)strlen(buffer);
  while (len > 0 && isspace((uschar)buffer[len-1])) len--;
  buffer[len] = 0;
  if (strcmp(buffer, "q") == 0) break;
  sender_host_address = buffer;
  sender_host_name = NULL;
  sender_host_aliases = NULL;
  host_lookup_msg = "";
  host_lookup_failed = FALSE;
  if (!host_name_lookup())  /* Debug causes printing */
    printf("Lookup failed:%s\n", host_lookup_msg);
  printf("\n> ");
  }

printf("\n");

printf("Testing host_check_rbl\n");
printf("> ");
while (fgets(buffer, 256, stdin) != NULL)
  {
  char *bufptr;
  int len = (int)strlen(buffer);
  while (len > 0 && isspace((uschar)buffer[len-1])) len--;
  buffer[len] = 0;
  if (strcmp(buffer, "q") == 0) break;
  (void) host_check_rbl(buffer, "rbl.maps.vix.com", NULL, &bufptr);
  printf("\n> ");
  }

printf("\n");
return 0;
}
#endif

/* End of host.c */
