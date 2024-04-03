/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "dnsdb.h"



/* Ancient systems (e.g. SunOS4) don't appear to have T_TXT defined in their
header files. */

#ifndef T_TXT
#define T_TXT 16
#endif

/* Table of recognized DNS record types and their integer values. */

static char *type_names[] = {
  "a",
#if HAVE_IPV6
  "aaaa",
  #ifdef SUPPORT_A6
  "a6",
  #endif
#endif
  "cname",
  "mx",
  "ns",
  "ptr",
  "txt" };

static int type_values[] = {
  T_A,
#if HAVE_IPV6
  T_AAAA,
  #ifdef SUPPORT_A6
  T_A6,
  #endif
#endif
  T_CNAME,
  T_MX,
  T_NS,
  T_PTR,
  T_TXT };


/*************************************************
*              Open entry point                  *
*************************************************/

/* See local README for interface description. */

void *
dnsdb_open(char *filename, char **errmsg)
{
filename = filename;   /* Keep picky compilers happy */
errmsg = errmsg;       /* Ditto */
return (void *)(-1);   /* Any non-0 value */
}



/*************************************************
*           Find entry point for dnsdb           *
*************************************************/

/* See local README for interface description. */

int
dnsdb_find(void *handle, char *filename, char *keystring, int length,
  char **result, char **errmsg)
{
int rc;
int size = 0;
int ptr = 0;
int type = T_TXT;
char *yield = NULL;
char *equals = strchr(keystring, '=');
char buffer[256];

dns_record *rr;
dns_answer dnsa;
dns_scan dnss;

handle = handle;           /* Keep picky compilers happy */
filename = filename;
length = length;

/* If the keystring contains an = this is preceded by a type name. */

if (equals != NULL)
  {
  int i;
  int len = equals - keystring;
  for (i = 0; i < sizeof(type_names)/sizeof(char *); i++)
    {
    if (len == (int)strlen(type_names[i]) &&
        strncmpic(keystring, type_names[i], len) == 0)
      {
      type = type_values[i];
      break;
      }
    }
  if (i >= sizeof(type_names)/sizeof(char *))
    {
    *errmsg = "unsupported DNS record type";
    return DEFER;
    }
  keystring += len + 1;
  }

/* If the type is PTR, we have to construct the relevant magic lookup
key. */

if (type == T_PTR && string_is_ip_address(keystring, NULL))
  {
  char *p = keystring + (int)strlen(keystring);
  char *pp = buffer;

  /* Handle IPv4 address */

  #if HAVE_IPV6
  if (strchr(keystring, ':') == NULL)
  #endif
    {
    int i;
    for (i = 0; i < 4; i++)
      {
      char *ppp = p;
      while (ppp > keystring && ppp[-1] != '.') ppp--;
      strncpy(pp, ppp, p - ppp);
      pp += p - ppp;
      *pp++ = '.';
      p = ppp - 1;
      }
    strcpy(pp, "in-addr.arpa");
    }

  /* Handle IPv6 address; convert to binary so as to fill out any
  abbreviation in the textual form. */

  #if HAVE_IPV6
  else
    {
    int i;
    int v6[4];
    (void)host_aton(keystring, v6);

    /* The original specification for IPv6 reverse lookup, to go with
    AAAA records, was to invert each nibble, and look in the ip6.int
    domain. This code does that. */

    for (i = 3; i >= 0; i--)
      {
      int j;
      for (j = 0; j < 32; j += 4)
        {
        sprintf(pp, "%x.", (v6[i] >> j) & 15);
        pp += 2;
        }
      }
    strcpy(pp, "ip6.int.");

    /* The revised way of doind IPv6 reverse lookups is to construct
    a binary key, and look in ip6.arpa. This code does that, but I have
    not been able to make it work on Solairs 8. The resolver seems to
    lose the initial backslash somehow. */

    /* For the moment, therefore, just leave this out. It's a minor point -
    how many people actually do PTR lookups using dnsdb? */

    /**************************************************
    strcpy(pp, "\\[x");
    pp += 3;

    for (i = 0; i < 4; i++)
      {
      sprintf(pp, "%08X", v6[i]);
      pp += 8;
      }
    strcpy(pp, "].ip6.arpa.");
    **************************************************/

    }
  #endif

  /* Point at constructed key */

  keystring = buffer;
  }

DEBUG(9) debug_printf("dnsdb key: %s\n", keystring);

/* Initialize the resolver, in case this is the first time it is used
in this run. Then do the lookup and sort out the result. */

dns_init(FALSE, FALSE);
rc = dns_lookup(&dnsa, keystring, type, NULL);

if (rc == DNS_NOMATCH) return FAIL;
if (rc != DNS_SUCCEED) return DEFER;

for (rr = dns_next_rr(&dnsa, &dnss, RESET_ANSWERS);
     rr != NULL;
     rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT))
  {
  int len = 0;        /* Stop picky compilers warning */
  char *s = NULL;

  if (rr->type != type) continue;

  /* There may be several addresses from an A6 record. Put newlines between
  them, just as for between several records. */

  if (type == T_A ||
      #ifdef SUPPORT_A6
      type == T_A6 ||
      #endif
      type == T_AAAA)
    {
    dns_address *da;

    /* This loop runs only once for A and AAAA records */

    for (da = dns_address_from_rr(&dnsa, rr); da != NULL; da = da->next)
      {
      len = (int)strlen(CS da->address);
      if (yield == NULL)
        {
        yield = CS da->address;
        ptr = len;
        size = ptr + 1;
        }
      else
        {
        yield = string_cat(yield, &size, &ptr, "\n", 1);
        yield = string_cat(yield, &size, &ptr, CS da->address, len);
        }
      }
    continue;
    }

  /* Other kinds of record just have one piece of data each. */

  if (type == T_TXT)
    {
    len = (rr->data)[0];
    s = string_copyn((char *)(rr->data+1), len);
    }
  else   /* T_CNAME, T_MX, T_NS, T_PTR */
    {
    uschar *p = (uschar *)(rr->data);
    int ssize = 264;
    s = store_get(ssize);
    if (type == T_MX)
      {
      int number;
      GETSHORT(number, p);      /* pointer is advanced */
      sprintf(s, "%d ", number);
      len = (int)strlen(s);
      }
    p += dn_expand(dnsa.answer, dnsa.answer + dnsa.answerlen, p,
      (DN_EXPAND_ARG4_TYPE)(s + len), ssize - len);
    len += (int)strlen(s+len);
    store_reset(s + len + 1);
    }

  if (yield == NULL)
    {
    yield = s;
    ptr = len;
    size = ptr + 1;
    }
  else
    {
    yield = string_cat(yield, &size, &ptr, "\n", 1);
    yield = string_cat(yield, &size, &ptr, s, len);
    }
  }

yield[ptr] = 0;
*result = yield;

return OK;
}

/* End of lookups/dnsdb.c */
