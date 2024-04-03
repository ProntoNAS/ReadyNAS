/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for interfacing with the DNS. */

#include "exim.h"


/* Function declaration needed for mutual recursion when A6 records
are supported. */

#if HAVE_IPV6
#ifdef SUPPORT_A6
static void dns_complete_a6(dns_address ***, dns_answer *, dns_record *,
  int, uschar *);
#endif
#endif



/*************************************************
*        Initialize and configure resolver       *
*************************************************/

/* Initialize the resolver and the storage for holding DNS answers if this is
the first time we have been here, and set the resolver options.

Arguments:
  qualify_single    TRUE to set the RES_DEFNAMES option
  search_parents    TRUE to set the RES_DNSRCH option

Returns:            nothing
*/

void
dns_init(BOOL qualify_single, BOOL search_parents)
{
if ((_res.options & RES_INIT) == 0)
  {
  res_init();
  DEBUG(11) _res.options |= RES_DEBUG;
  }
_res.options &= ~(RES_DNSRCH | RES_DEFNAMES);
_res.options |= (qualify_single? RES_DEFNAMES : 0) |
                (search_parents? RES_DNSRCH : 0);
if (dns_retrans > 0) _res.retrans = dns_retrans;
if (dns_retry > 0) _res.retry = dns_retry;
}





/*************************************************
*       Get next DNS record from answer block    *
*************************************************/

/* Call this with reset == RESET_ANSWERS to scan the answer block, reset ==
RESET_ADDITIONAL to scan the additional records, and reset == RESET_NEXT to
get the next record. The result is in static storage which must be copied if
it is to be preserved.

Arguments:
  dnsa      pointer to dns answer block
  dnss      pointer to dns scan block
  reset     option specifing what portion to scan, as described above

Returns:    next dns record, or NULL when no more
*/

dns_record *
dns_next_rr(dns_answer *dnsa, dns_scan *dnss, int reset)
{
HEADER *h = (HEADER *)dnsa->answer;
int namelen;

/* Reset the saved data when requested to, and skip to the first required RR */

if (reset != RESET_NEXT)
  {
  dnss->rrcount = ntohs(h->qdcount);
  dnss->aptr = dnsa->answer + sizeof(HEADER);

  /* Skip over questions; failure to expand the name just gives up */

  while (dnss->rrcount-- > 0)
    {
    namelen = dn_expand(dnsa->answer, dnsa->answer + dnsa->answerlen,
      dnss->aptr, (DN_EXPAND_ARG4_TYPE) &(dnss->srr.name), DNS_MAXNAME);
    if (namelen < 0) { dnss->rrcount = 0; return NULL; }
    dnss->aptr += namelen + 4;    /* skip name & type & class */
    }

  /* Get the number of answer records. */

  dnss->rrcount = ntohs(h->ancount);

  /* Skip over answers and NS records if wanting to look at the additional
  records. */

  if (reset == RESET_ADDITIONAL)
    {
    dnss->rrcount += ntohs(h->nscount);
    while (dnss->rrcount-- > 0)
      {
      namelen = dn_expand(dnsa->answer, dnsa->answer + dnsa->answerlen,
        dnss->aptr, (DN_EXPAND_ARG4_TYPE) &(dnss->srr.name), DNS_MAXNAME);
      if (namelen < 0) { dnss->rrcount = 0; return NULL; }
      dnss->aptr += namelen + 8;            /* skip name, type, class & TTL */
      GETSHORT(dnss->srr.size, dnss->aptr); /* size of data portion */
      dnss->aptr += dnss->srr.size;         /* skip over it */
      }
    dnss->rrcount = ntohs(h->arcount);
    }
  }


/* The variable dnss->aptr is now pointing at the next RR, and dnss->rrcount
contains the number of RR records left. */

if (dnss->rrcount-- <= 0) return NULL;

/* If expanding the RR domain name fails, behave as if no more records
(something safe). */

namelen = dn_expand(dnsa->answer, dnsa->answer + dnsa->answerlen, dnss->aptr,
  (DN_EXPAND_ARG4_TYPE) &(dnss->srr.name), DNS_MAXNAME);
if (namelen < 0) { dnss->rrcount = 0; return NULL; }

/* Move the pointer past the name and fill in the rest of the data structure
from the following bytes. */

dnss->aptr += namelen;
GETSHORT(dnss->srr.type, dnss->aptr); /* Record type */
dnss->aptr += 6;                      /* Don't want class or TTL */
GETSHORT(dnss->srr.size, dnss->aptr); /* Size of data portion */
dnss->srr.data = dnss->aptr;          /* The record's data follows */
dnss->aptr += dnss->srr.size;         /* Advance to next RR */

/* Return a pointer to the dns_record structure within the dns_answer. This is
for convenience so that the scans can use nice-looking for loops. */

return &(dnss->srr);
}




/*************************************************
*            Turn DNS type into text             *
*************************************************/

/* Turn the coded record type into a string for printing.

Argument:   record type
Returns:    pointer to string
*/

char *
dns_text_type(int t)
{
switch(t)
  {
  case T_A:    return "A";
  case T_MX:   return "MX";
  case T_AAAA: return "AAAA";
  case T_A6:   return "A6";
  case T_TXT:  return "TXT";
  case T_PTR:  return "PTR";
  default:     return "?";
  }
}



/*************************************************
*        Cache a failed DNS lookup result        *
*************************************************/

/* We cache failed lookup results so as not to experience timeouts many
times for the same domain. For successful lookups, we rely on resolver and/or
name server caching.

Arguments:
  name       the domain name
  type       the lookup type
  rc         the return code

Returns:     the return code
*/

static int
dns_return(char *name, int type, int rc)
{
tree_node *node = store_malloc(sizeof(tree_node) + 280);
sprintf(node->name, "%.255s-%s", name, dns_text_type(type));
node->data.val = rc;
(void)tree_insertnode(&tree_dns_fails, node);
return rc;
}



/*************************************************
*              Do basic DNS lookup               *
*************************************************/

/* Call the resolver to look up the given domain name, using the given type,
and check the result. The error code TRY_AGAIN is documented as meaning "non-
Authoritive Host not found, or SERVERFAIL". Sometimes there are badly set
up nameservers that produce this error continually, so there is the option of
providing a list of domains for which this is treated as a non-existent
host.

Arguments:
  dnsa      pointer to dns_answer structure
  name      name to look up
  type      type of DNS record required (T_A, T_MX, etc)

Returns:    DNS_SUCCEED   successful lookup
            DNS_NOMATCH   name not found, or no data found for the given type,
                          or name contains illegal characters (if checking)
            DNS_AGAIN     soft failure, try again later
            DNS_FAIL      DNS failure
*/

int
dns_basic_lookup(dns_answer *dnsa, char *name, int type)
{
tree_node *previous;
char node_name[280];

/* DNS lookup failures of any kind are cached in a tree. This is mainly so that
a timeout on one domain doesn't happen time and time again for messages that
have many addresses in the same domain. We rely on the resolver and name server
caching for successful lookups. */

sprintf(node_name, "%.255s-%s", name, dns_text_type(type));
previous = tree_search(tree_dns_fails, node_name);
if (previous != NULL)
  {
  DEBUG(8) debug_printf("DNS lookup of %s: using cached value %s\n",
    node_name,
      (previous->data.val == DNS_NOMATCH)? "DNS_NOMATCH" :
      (previous->data.val == DNS_AGAIN)? "DNS_AGAIN" :
      (previous->data.val == DNS_FAIL)? "DNS_FAIL" : "??");
  return previous->data.val;
  }

/* If we are running in the test harness, recognize a couple of special
names that always give error returns. This makes it straightforward to
test the handling of DNS errors. */

if (running_in_test_harness)
  {
  if (strcmp(name, "test.again.dns") == 0)
    {
    DEBUG(8) debug_printf("DNS lookup of %s (%s) bypassed for testing\n",
      name, dns_text_type(type));
    DEBUG(8) debug_printf("returning DNS_AGAIN\n");
    return dns_return(name, type, DNS_AGAIN);
    }
  if (strcmp(name, "test.fail.dns") == 0)
    {
    DEBUG(8) debug_printf("DNS lookup of %s (%s) bypassed for testing\n",
      name, dns_text_type(type));
    DEBUG(8) debug_printf("returning DNS_FAIL\n");
    return dns_return(name, type, DNS_FAIL);
    }
  }

/* If configured, check the hygene of the name passed to lookup. Otherwise,
although DNS lookups may give REFUSED at the lower level, some resolvers
turn this into TRY_AGAIN, which is silly. Give a NOMATCH return, since such
domains cannot be in the DNS. The check is now done by a regular expression;
give it space for substring storage to save it having to get its own if the
regex has substrings that are used - the default uses a conditional.

This test is omitted for PTR records. These occur only in calls from the dnsdb
lookup, which constructs the names itself, so they should be OK. Besides,
bitstring labels don't conform to normal name syntax. */

#ifndef STAND_ALONE   /* Omit this for stand-alone tests */

if (check_dns_names && type != T_PTR)
  {
  int ovector[3*(EXPAND_MAXN+1)];

  if (regex_check_dns_names == NULL)
    regex_check_dns_names =
      regex_must_compile(check_dns_names_pattern, FALSE, TRUE);

  if (pcre_exec(regex_check_dns_names, NULL, name, (int)strlen(name), 0,
      PCRE_EOPT, ovector, sizeof(ovector)/sizeof(int)) < 0)
    {
    DEBUG(3)
      debug_printf("DNS name syntax check failed: %s (%s)\n", name,
        dns_text_type(type));
    host_find_failed_syntax = TRUE;
    return DNS_NOMATCH;
    }
  }

#endif /* STAND_ALONE */

/* Call the resolver */

dnsa->answerlen = res_search(name, C_IN, type, dnsa->answer, MAXPACKET);

if (dnsa->answerlen < 0) switch (h_errno)
  {
  case HOST_NOT_FOUND:
  DEBUG(8) debug_printf("DNS lookup of %s (%s) gave HOST_NOT_FOUND\n"
    "returning DNS_NOMATCH\n", name, dns_text_type(type));
  return dns_return(name, type, DNS_NOMATCH);

  case TRY_AGAIN:
  DEBUG(8) debug_printf("DNS lookup of %s (%s) gave TRY_AGAIN\n",
    name, dns_text_type(type));

  /* Cut this out for various test programs */
  #ifndef STAND_ALONE
  if (!match_isinlist(name, &dns_again_means_nonexist, FALSE, TRUE, NULL))
    {
    DEBUG(8) debug_printf("returning DNS_AGAIN\n");
    return dns_return(name, type, DNS_AGAIN);
    }
  DEBUG(8) debug_printf("%s is in dns_again_means_nonexist: returning "
    "DNS_NOMATCH\n", name);
  #endif

  return dns_return(name, type, DNS_NOMATCH);

  case NO_RECOVERY:
  DEBUG(8) debug_printf("DNS lookup of %s (%s) gave NO_RECOVERY\n"
    "returning DNS_FAIL\n", name, dns_text_type(type));
  return dns_return(name, type, DNS_FAIL);

  case NO_DATA:
  DEBUG(8) debug_printf("DNS lookup of %s (%s) gave NO_DATA\n"
    "returning DNS_NOMATCH\n", name, dns_text_type(type));
  return dns_return(name, type, DNS_NOMATCH);

  default:
  DEBUG(8) debug_printf("DNS lookup of %s (%s) gave unknown DNS error %d\n"
    "returning DNS_FAIL\n", name, dns_text_type(type), h_errno);
  return dns_return(name, type, DNS_FAIL);
  }

DEBUG(8) debug_printf("DNS lookup of %s (%s) succeeded\n",
  name, dns_text_type(type));

return DNS_SUCCEED;
}




/************************************************
*        Do a DNS lookup and handle CNAMES      *
************************************************/

/* Look up the given domain name, using the given type. Follow CNAMEs if
necessary, but only so many times. There aren't supposed to be CNAME chains in
the DNS, but you are supposed to cope with them if you find them.

The assumption is made that if the resolver gives back records of the
requested type *and* a CNAME, we don't need to make another call to look up
the CNAME. I can't see how it could return only some of the right records. If
it's done a CNAME lookup in the past, it will have all of them; if not, it
won't return any.

If fully_qualified_name is not NULL, set it to point to the full name
returned by the resolver, if this is different to what it is given, unless
the returned name starts with "*" as some nameservers seem to be returning
wildcards in this form.

Arguments:
  dnsa                  pointer to dns_answer structure
  name                  domain name to look up
  type                  DNS record type (T_A, T_MX, etc)
  fully_qualified_name  if not NULL, return the returned name here if its
                          contents are different (i.e. it must be preset)

Returns:                DNS_SUCCEED   successful lookup
                        DNS_NOMATCH   name not found, or no data found
                        DNS_AGAIN     soft failure, try again later
                        DNS_FAIL      DNS failure
*/

int
dns_lookup(dns_answer *dnsa, char *name, int type, char **fully_qualified_name)
{
int i;
char *orig_name = name;

/* Loop to follow CNAME chains so far, but no further... */

for (i = 0; i < 10; i++)
  {
  char data[256];
  dns_record *rr, cname_rr, type_rr;
  dns_scan dnss;
  int datalen, rc;

  /* DNS lookup failures get passed straight back. */

  if ((rc = dns_basic_lookup(dnsa, name, type)) != DNS_SUCCEED) return rc;

  /* We should have either records of the required type, or a CNAME record,
  or both. We need to know whether both exist for getting the fully qualified
  name, but avoid scanning more than necessary. Note that we must copy the
  contents of any rr blocks returned by dns_next_rr() as they use the same
  area in the dnsa block. */

  cname_rr.data = type_rr.data = NULL;
  for (rr = dns_next_rr(dnsa, &dnss, RESET_ANSWERS);
       rr != NULL;
       rr = dns_next_rr(dnsa, &dnss, RESET_NEXT))
    {
    if (rr->type == type)
      {
      if (type_rr.data == NULL) type_rr = *rr;
      if (cname_rr.data != NULL) break;
      }
    else if (rr->type == T_CNAME) cname_rr = *rr;
    }

  /* If a CNAME was found, take the fully qualified name from it; otherwise
  from the first data record, if present. */

  if (fully_qualified_name != NULL)
    {
    if (cname_rr.data != NULL)
      {
      if (strcmp(cname_rr.name, *fully_qualified_name) != 0 &&
          cname_rr.name[0] != '*')
        *fully_qualified_name = string_copy(cname_rr.name);
      }
    else if (type_rr.data != NULL)
      {
      if (strcmp(type_rr.name, *fully_qualified_name) != 0 &&
          type_rr.name[0] != '*')
        *fully_qualified_name = string_copy(type_rr.name);
      }
    }

  /* If any data records of the correct type were found, we are done. */

  if (type_rr.data != NULL) return DNS_SUCCEED;

  /* If there are no data records, we need to re-scan the DNS using the
  domain given in the CNAME record, which should exist (otherwise we should
  have had a failure from dns_lookup). However code against the possibility of
  its not existing. */

  if (cname_rr.data == NULL) return DNS_FAIL;
  datalen = dn_expand(dnsa->answer, dnsa->answer + dnsa->answerlen,
    cname_rr.data, (DN_EXPAND_ARG4_TYPE)data, 256);
  if (datalen < 0) return DNS_FAIL;
  name = data;
  }       /* Loop back to do another lookup */

/*Control reaches here after 10 times round the CNAME loop. Something isn't
right... */

log_write(0, LOG_MAIN, "CNAME loop for %s encountered", orig_name);
return DNS_FAIL;
}


#if HAVE_IPV6 && defined(SUPPORT_A6)

/*************************************************
*        Search DNS block for prefix RRs         *
*************************************************/

/* Called from dns_complete_a6() to search an additional section or a main
answer section for required prefix records to complete an IPv6 address obtained
from an A6 record. For each prefix record, a recursive call to dns_complete_a6
is made, with a new copy of the address so far.

Arguments:
  dnsa       the DNS answer block
  which      RESET_ADDITIONAL or RESET_ANSWERS
  name       name of prefix record
  yptrptr    pointer to the pointer that points to where to hang the next
               dns_address structure
  bits       number of bits we have already got
  bitvec     the bits we have already got

Returns:     TRUE if any records were found
*/

static BOOL
dns_find_prefix(dns_answer *dnsa, int which, uschar *name, dns_address
  ***yptrptr, int bits, uschar *bitvec)
{
BOOL yield = FALSE;
dns_record *rr;
dns_scan dnss;

for (rr = dns_next_rr(dnsa, &dnss, which);
     rr != NULL;
     rr = dns_next_rr(dnsa, &dnss, RESET_NEXT))
  {
  uschar cbitvec[16];
  if (rr->type != T_A6 || strcmpic(rr->name, name) != 0) continue;
  yield = TRUE;
  memcpy(cbitvec, bitvec, sizeof(cbitvec));
  dns_complete_a6(yptrptr, dnsa, rr, bits, cbitvec);
  }

return yield;
}



/*************************************************
*            Follow chains of A6 records         *
*************************************************/

/* A6 records may be incomplete, with pointers to other records containing more
bits of the address. There can be a tree structure, leading to a number of
addresses originating from a single initial A6 record.

Arguments:
  yptrptr    pointer to the pointer that points to where to hang the next
               dns_address structure
  dnsa       the current DNS answer block
  rr         the RR we have at present
  bits       number of bits we have already got
  bitvec     the bits we have already got

Returns:     nothing
*/

static void
dns_complete_a6(dns_address ***yptrptr, dns_answer *dnsa, dns_record *rr,
  int bits, uschar *bitvec)
{
static uschar bitmask[] = { 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80 };
uschar *p = (uschar *)(rr->data);
int prefix_len, suffix_len;
int i, j, k;
uschar *chainptr;
uschar chain[264];
dns_answer cdnsa;

/* The prefix length is the first byte. It defines the prefix which is missing
from the data in this record as a number of bits. Zero means this is the end of
a chain. The suffix is the data in this record; only sufficient bytes to hold
it are supplied. There may be zero bytes. We have to ignore trailing bits that
we have already obtained from earlier RRs in the chain. */

prefix_len = *p++;                      /* bits */
suffix_len = (128 - prefix_len + 7)/8;  /* bytes */

/* If the prefix in this record is greater than the prefix in the previous
record in the chain, we have to ignore the record (RFC 2874). */

if (prefix_len > 128 - bits) return;

/* In this little loop, the number of bits up to and including the current byte
is held in k. If we have none of the bits in this byte, we can just or it into
the current data. If we have all of the bits in this byte, we skip it.
Otherwise, some masking has to be done. */

for (i = suffix_len - 1, j = 15, k = 8; i >= 0; i--)
  {
  int required = k - bits;
  if (required >= 8) bitvec[j] |= p[i];
    else if (required > 0) bitvec[j] |= p[i] & bitmask[required];
  j--;     /* I tried putting these in the "for" statement, but gcc muttered */
  k += 8;  /* about computed values not being used. */
  }

/* If the prefix_length is zero, we are at the end of a chain. Build a
dns_address item with the current data, hang it onto the end of the chain,
adjust the hanging pointer, and we are done. */

if (prefix_len == 0)
  {
  dns_address *new = store_get(sizeof(dns_address) + 50);
  inet_ntop(AF_INET6, bitvec, new->address, 50);
  new->next = NULL;
  **yptrptr = new;
  *yptrptr = &(new->next);
  return;
  }

/* Prefix length is not zero. Reset the number of bits that we have collected
so far, and extract the chain name. */

bits = 128 - prefix_len;
p += suffix_len;

chainptr = chain;
while ((i = *p++) != 0)
  {
  if (chainptr != chain) *chainptr++ = '.';
  memcpy(chainptr, p, i);
  chainptr += i;
  p += i;
  }
*chainptr = 0;
chainptr = chain;

/* Now scan the current DNS response record to see if the additional section
contains the records we want. This processing can be cut out for testing
purposes. */

if (dns_find_prefix(dnsa, RESET_ADDITIONAL, chainptr, yptrptr, bits, bitvec))
  return;

/* No chain records were found in the current DNS response block. Do a new DNS
lookup to try to find these records. This opens up the possibility of DNS
failures. We ignore them at this point; if all branches of the tree fail, there
will be no addresses at the end. */

if (dns_lookup(&cdnsa, chainptr, T_A6, NULL) == DNS_SUCCEED)
  (void)dns_find_prefix(&cdnsa, RESET_ANSWERS, chainptr, yptrptr, bits, bitvec);
}
#endif  /* HAVE_IPV6 && defined(SUPPORT_A6) */




/*************************************************
*          Get address(es) from DNS record       *
*************************************************/

/* The record type is either T_A for an IPv4 address or T_AAAA (or T_A6 when
supported) for an IPv6 address. In the A6 case, there may be several addresses,
generated by following chains. A recursive function does all the hard work.
A6 records now look like passing into history, so the code is only included
when explicitly asked for.

Argument:
  dnsa       the DNS answer block
  rr         the RR

Returns:     pointer a chain of dns_address items
*/

dns_address *
dns_address_from_rr(dns_answer *dnsa, dns_record *rr)
{
dns_address *yield = NULL;

#if HAVE_IPV6 && defined(SUPPORT_A6)
dns_address **yieldptr = &yield;
uschar bitvec[16];
#else
dnsa = dnsa;    /* Stop picky compilers warning */
#endif

if (rr->type == T_A)
  {
  uschar *p = (uschar *)(rr->data);
  yield = store_get(sizeof(dns_address) + 20);
  (void)sprintf(CS yield->address, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
  yield->next = NULL;
  }

#if HAVE_IPV6

#ifdef SUPPORT_A6
else if (rr->type == T_A6)
  {
  memset(bitvec, 0, sizeof(bitvec));
  dns_complete_a6(&yieldptr, dnsa, rr, 0, bitvec);
  }
#endif  /* SUPPORT_A6 */

else
  {
  yield = store_get(sizeof(dns_address) + 50);
  inet_ntop(AF_INET6, (uschar *)(rr->data), yield->address, 50);
  yield->next = NULL;
  }
#endif  /* HAVE_IPV6 */

return yield;
}

/* End of dns.c */
