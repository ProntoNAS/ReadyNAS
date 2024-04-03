/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for matching strings */


#include "exim.h"



/*************************************************
*           Generalized string match             *
*************************************************/

/* We are passed the subject and pattern as strings. For
non regular expression, if the first character of the pattern is *, the match
is on the tail of the item. If the pattern starts with <searchtype>; then
do a file lookup, using the remainder as the file name. Keep a list of open
files for efficiency.

Arguments:
  s            the subject string to be checked
  pattern      the pattern to check it against, either literal, starting with *,
               starting with ^ for a r.e., or a lookup specification
  expand_setup if < 0, don't set up any numeric expansion variables;
               if = 0, set $0 to whole subject, and either
                 $1 to what matches * or
                 $1, $2, ... to r.e. bracketed items
               if > 0, don't set $0, but do set either
                 $n to what matches *, or
                 $n, $n+1, ... to r.e. bracketed items
               (n = expand_setup)
  use_partial  if FALSE, override any partial- search types
  caseless     TRUE for caseless matching where possible
  value        if not NULL, and a file lookup was done, return the result
                 here instead of discarding it; else set it to point to NULL

Returns:       TRUE if matched; FALSE if not.
               If the failure is the result of a DEFER in a lookup, then
               search_find_defer will be set.
*/

BOOL
match_check_string(char *s, char *pattern, int expand_setup, BOOL use_partial,
   BOOL caseless, char **value)
{
BOOL yield;
pcre *re;

if (value != NULL) *value = NULL;
search_find_defer = FALSE;          /* In case no call to search_find() */

/* If required to set up $0, initialize the data but don't turn on by setting
expand_nmax until the match is assured. */

expand_nmax = -1;
if (expand_setup == 0)
  {
  expand_nstring[0] = s;
  expand_nlength[0] = (int)strlen(s);
  }
else if (expand_setup > 0) expand_setup--;

/* Pattern is not a regular expression. */

if (pattern[0] != '^')
  {
  int search_type = 0;      /* keep picky compiler happy */
  int partial = -1;
  char *error, *key, *result, *semicolon;
  void *handle;

  /* Tail match */

  if (pattern[0] == '*')
    {
    int patlen, slen;
    patlen = (int)strlen(++pattern);
    slen = (int)strlen(s);
    if (patlen > slen) return FALSE;
    yield = caseless?
      (strncmpic(s + slen - patlen, pattern, patlen) == 0) :
      (strncmp(s + slen - patlen, pattern, patlen) == 0);
    if (yield && expand_setup >= 0)
      {
      expand_nstring[++expand_setup] = s;
      expand_nlength[expand_setup] = slen - patlen;
      expand_nmax = expand_setup;
      }
    return yield;
    }


  /* Exact string match */

  if ((semicolon = strchr(pattern, ';')) == NULL)
    {
    yield = caseless? (strcmpic(s, pattern) == 0) : (strcmp(s, pattern) == 0);
    if (yield && expand_setup >= 0) expand_nmax = expand_setup;
    return yield;
    }


  /* The remaining possibilities are various matches by file lookup; for
  single-key lookups the key is unprocessed, but for query-type lookups
  there has to be some processing to get the key into the query.

  If the pattern starts with "partial-" (with an optional number before
  the -) then set up for doing partial matches by widening and narrowing
  the domain if the original doesn't match. This does not as yet apply to
  NIS+ searches. [Needs thought.] */

  if (strncmp(pattern, "partial", 7) == 0)
    {
    char *orig_pattern = pattern;
    pattern += 7;
    if (isdigit((uschar)*pattern))
      {
      partial = 0;
      while (isdigit((uschar)*pattern))
        partial = partial * 10 + *pattern++ - '0';
      }
    else partial = 2;
    if (*pattern++ != '-')
      log_write(0, LOG_PANIC_DIE, "malformed partial search type in string "
        "match: %s", orig_pattern);
    }

  /* If the lookup type ends in "*" it requests ultimate default lookup.
  This is independent of "partial", but is encoded as part of the same value.
  Temporarily terminate the pattern string for calling search_findtype(). */

  if (semicolon[-1] == '*')
    {
    partial += 1024;
    semicolon[-1] = 0;
    }
  else *semicolon = 0;

  /* Now determine the kind of lookup */

  search_type = search_findtype(pattern, &error);
  if (partial > 1000) semicolon[-1] = '*'; else *semicolon = ';';
  if (search_type < 0) log_write(0, LOG_PANIC_DIE, "%s", error);

  /* For query-style lookups, the query has to be expanded in order
  to get the key inserted into the query. The global lookup_key contains
  the value to replace $key in the expansion. Partial searches are not
  implemented. */

  if (mac_islookup(search_type, lookup_querystyle))
    {
    lookup_key = s;
    key = expand_string(semicolon+1);
    lookup_key = NULL;
    if (key == NULL)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "failed to expand %s query %s: %s",
        lookup_list[search_type].name, semicolon+1, expand_string_message);
    pattern = NULL;
    }
  else key = s;

  /* Now do the actual lookup; throw away the actual data returned unless it
  was asked for; partial matching is all handled inside search_find(). However,
  partial matching is not appropriate for certain lookups (e.g. when looking
  up user@domain for sender rejection). Note that there is no search_close()
  because of the caching arrangements. */

  if (!use_partial) partial = -1;
  if (pattern != NULL)                /* NULL => query type */
    {
    pattern = semicolon + 1;
    while (isspace((uschar)*pattern)) pattern++;
    }
  handle = search_open(pattern, search_type, 0, NULL, NULL, &error);
  if (handle == NULL) log_write(0, LOG_PANIC_DIE, "%s", error);
  result = search_find(handle, pattern, key, partial, &expand_setup, &error);

  if (result == NULL) return FALSE;
  if (value != NULL) *value = result;

  expand_nmax = expand_setup;
  return TRUE;
  }


/* Regular expression match: compile, match, and set up $ variables if
required. */

re = regex_must_compile(pattern, caseless, FALSE);
return (expand_setup < 0)?
  pcre_exec(re, NULL, s, (int)strlen(s), 0, PCRE_EOPT, NULL, 0) >= 0
  :
  regex_match_and_setup(re, s, 0, expand_setup);
}



/*************************************************
*            Match in colon-separated list       *
*************************************************/

/* If a list item starts with !, then that implies negation if the subject
matches the rest of the item (ignoring white space after the !). The result
when the end of the list is reached is FALSE unless the last item on the list
is negated, in which case it is TRUE. A file name in the list causes its lines
to be interpolated as if items in the list.

This function is used for domain lists and lists of local parts. It is not used
for host lists, which have additional interpretation of the patterns.

Arguments:
  s              string to search for
  listptr        ptr to ptr to colon separated list of patterns, or NULL
  at_is_primary  if TRUE, a list entry of "@" is interpreted as the primary
                 name for the host
  caseless       TRUE for (mostly) caseless matching - passed directly to
                   match_check_string()
  valueptr       pointer to where any lookup data is to be passed back,
                 or NULL (just passed on to match_check_string)

Returns:         TRUE  if matched a non-negated item
                 TRUE  if hit end of list after a negated item
                 FALSE if matched a negated item
                 FALSE if hit end of list after a non-negated item
                 FALSE, with search_find_defer set, if a lookup deferred
*/

BOOL
match_isinlist(char *s, char **listptr, BOOL at_is_primary, BOOL caseless,
  char **valueptr)
{
BOOL yield = TRUE;
int sep = 0;
char *list = *listptr;
char *sss;
char *ot = NULL;
char buffer[1024];

/* Save time by not scanning for the option name when we don't need it, and
don't compile for standalone testing, when readconf isn't present. */

#ifndef STAND_ALONE
HDEBUG(9)
  {
  char *listname = readconf_find_option(listptr);
  if (listname[0] != 0)
    ot = string_sprintf("%s in %s?", s, listname);
  else
    ot = string_sprintf("%s in \"%s\"?", s, *listptr);
  }
#endif

while ((sss = string_nextinlist(&list, &sep, buffer, sizeof(buffer))) != NULL)
  {
  char *ss = sss;

  /* Starting with ! specifies a negative item. It is theoretically possible
  for a local part to start with !. In that case, a regex has to be used. */

  if (*ss == '!')
    {
    yield = FALSE;
    while (isspace((uschar)(*(++ss))));
    }
  else yield = TRUE;

  /* If the item is not a file name, just a single match attempt is
  required. */

  if (*ss != '/')
    {
    if (*ss == '@' && ss[1] == 0 && at_is_primary) ss = primary_hostname_lc;
    if (match_check_string(s, ss, -1, TRUE, caseless, valueptr))
      {
      HDEBUG(9) debug_printf("%s %s (matched %s)\n", ot,
        yield? "yes" : "no", sss);
      return yield;
      }
    }

  /* If the item is a file name, we read the file and do a match attempt
  on each line in the file, including possibly more negation processing. */

  else
    {
    BOOL file_yield = yield;       /* In case empty file */
    FILE *f = fopen(ss, "r");

    /* ot will be null in non-debugging cases, and anyway, we get better
    wording by reworking it. */

    if (f == NULL)
      {
      #ifdef STAND_ALONE
      char *listname = "list";
      #else
      char *listname = readconf_find_option(listptr);
      if (listname[0] == 0)
        listname = string_sprintf("\"%s\"", *listptr);
      #endif
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s",
        string_open_failed(errno,
          "%s when checking for %s in %s", ss, s, listname));
      }

    while (fgets(buffer, sizeof(buffer), f) != NULL)
      {
      if ((ss = strchr(buffer, '#')) != NULL) *ss = 0;  /* trailing comment */

      ss = buffer + (int)strlen(buffer);                /* trailing space */
      while (ss > buffer && isspace((uschar)ss[-1])) ss--;
      *ss = 0;

      ss = buffer;
      while (isspace((uschar)*ss)) ss++;                /* leading space */

      if (*ss == 0) continue;                           /* ignore empty */

      file_yield = yield;
      if (*ss == '!')                                   /* negation */
        {
        file_yield = !file_yield;
        while (isspace((uschar)(*(++ss))));
        }

      if (*ss == '@' && ss[1] == 0 && at_is_primary) ss = primary_hostname_lc;
      if (match_check_string(s, ss, -1, TRUE, caseless, valueptr))
        {
        fclose(f);
        return file_yield;
        }
      if (search_find_defer) break;
      }

    /* At the end of the file, leave the yield setting at the final setting
    for the file, in case this is the last item in the list. */

    yield = file_yield;
    fclose(f);
    }

  /* Deferral of any lookup yields FALSE with search_find_defer set. */

  if (search_find_defer)
    {
    HDEBUG(9) debug_printf("%s lookup deferred\n", ot);
    return FALSE;
    }

  } /* Loop for the next item on the top-level list */

/* End of list reached: if the last item was negated yield TRUE, else FALSE. */

HDEBUG(9) debug_printf("%s %s (end of list)\n", ot, yield? "no" : "yes");
return !yield;
}




#ifndef STAND_ALONE

/*************************************************
*           Do file existence tests              *
*************************************************/

/* This function is given a colon-separated list of files whose existence
is to be tested. The string is first expanded, and the resulting file names
must be absolute, but "!" may appear precede an (absolute) file name to
indicate that non-existence is what is wanted. An empty name, whether preceded
by ! or not, has no effect. Otherwise, if an item in the list contains
no / characters, it is interpreted as a user name or uid to be used when
checking subsequent files. A gid can also be provided with a comma separator.

Argument:
  s        a colon-separated list of files whose existence is to be tested,
           or NULL; a leading "!" causes the test for that file to be for
           non-existence. A leading "+" causes an EACCES from the stat()
           call to be treated as if it were ENOENT.
  error    a pointer to an anchor for an error text in the case of a DEFER

Returns:   OK if s == NULL or all files' existence is as required;
           DEFER if the existence of at least one of the files is
             unclear (an error other than non-existence occurred);
           FAIL otherwise.
           In all cases, errno contains what it was set to by the final
           call to stat(), or 0 if there were no files in the list.
*/

int
match_exists(char *s, char **error)
{
char *ss, *file, *listptr;
char buffer[1024];
int yield = OK;
int save_errno = 0;
int sep = 0;
uid_t saved_euid = (uid_t)0;
gid_t saved_egid = (uid_t)0;
BOOL restore_uid = FALSE;
struct stat statbuf;

if (s == NULL) return OK;
ss = expand_string(s);
if (ss == NULL)
  {
  *error = string_sprintf("expansion failure: %s", expand_string_message);
  return DEFER;
  }

DEBUG(9) debug_printf("require_files = %s\n", ss);

listptr = ss;

while ((file = string_nextinlist(&listptr, &sep, buffer, sizeof(buffer)))
        != NULL)
  {
  int rc;
  BOOL invert = FALSE;
  BOOL eperm_is_absence = FALSE;

  if (*file == '!')
    {
    invert = TRUE;
    while (isspace((uschar)(*(++file))));
    }

  if (*file == '+')
    {
    eperm_is_absence = TRUE;
    while (isspace((uschar)(*(++file))));

    if (*file == '!')
      {
      invert = TRUE;
      while (isspace((uschar)(*(++file))));
      }
    }

  /* Empty items are just skipped */

  if (*file == 0) continue;

  /* If the first character is not '/' then either we have a user name
  or uid (with optional group/gid), or else it's a non-absolute file name,
  which is a disaster. */

  if (*file != '/')
    {
    uid_t uid;
    gid_t gid;
    BOOL ok;
    struct passwd *pw;
    char *comma = strchr(file, ',');

    /* The presence of a / further in the string is taken as implying
    a non-absolute path. */

    if (strchr(file, '/') != NULL)
      {
      *error = string_sprintf("file name for existence test is "
        "not fully qualified: %s", file);
      return DEFER;
      }

    /* If this is the first change of uid/gid, remember what we have to
    go back to. */

    if (!restore_uid)
      {
      saved_euid = geteuid();
      saved_egid = getegid();
      restore_uid = TRUE;
      }

    /* If there's a comma, temporarily terminate the user name/number
    at that point. Then set the uid. */

    if (comma != NULL) *comma = 0;
    ok = direct_finduser(file, &pw, &uid);
    if (comma != NULL) *comma = ',';

    if (!ok)
      {
      DEBUG(1) debug_printf("user \"%s\" for file existence test not found\n",
        file);
      return FAIL;
      }

    /* If there was no comma, the gid is either that associated with the
    user, or unchanged. */

    if (comma == NULL)
      {
      gid = (pw == NULL)? saved_egid : pw->pw_gid;
      }
    else
      {
      if (!direct_findgroup(comma + 1, &gid))
        {
        DEBUG(1) debug_printf("group \"%s\" for file existence test not found\n",
          comma + 1);
        return FAIL;
        }
      }

    /* Change the effective uid/gid, and proceed to the next file. */

    if (saved_euid != root_uid) mac_seteuid(root_uid);
    mac_setegid(gid);
    mac_seteuid(uid);
    DEBUG(9) debug_print_ids("require files");
    continue;
    }

  /* Do the test */

  errno = 0;
  rc = stat(file, &statbuf);
  save_errno = errno;

  DEBUG(9) debug_printf("test existence of %s\n  "
    "required %s, EACCES => %s\n  %s\n",
    file,
    invert? "absent" : "present",
    eperm_is_absence? "absent" : "unknown",
    strerror(errno));

  if (rc < 0 && save_errno != ENOENT && save_errno != ENOTDIR &&
      (!eperm_is_absence || save_errno != EACCES))
    {
    *error = string_sprintf("%s", strerror(save_errno));
    yield = DEFER;
    break;
    }

  if ((rc < 0) != invert)
    {
    yield = FAIL;
    break;
    }
  }

/* Put back the uid/gid if we changed it. */

if (restore_uid)
  {
  mac_seteuid(root_uid);
  mac_setegid(saved_egid);
  mac_seteuid(saved_euid);
  DEBUG(9) debug_print_ids("restored");
  }

return yield;
}

#endif  /* STAND_ALONE */




/*************************************************
*    Match address to single address-list item   *
*************************************************/

/* This function matches a domain and possibly local part to a single
item from an address list.

Arguments:
  ss             the pattern to match
  address        the start of the address
  llen           length of local part
  expand_setup   controls setting up of $n variables
  caseless       TRUE for caseless local part matching

Returns:         am_yes    for a positive match
                 am_no     for a negative match
                 am_pos    for no match, last one was positive
                 am_neg    for no match, last one was negative
                 am_ld     if a lookup deferred
*/

enum { am_yes, am_no, am_pos, am_neg, am_ld };

static int
check_address(char *ss, char *address, int llen, int expand_setup,
  BOOL caseless)
{
int expand_inc = 0;
BOOL matchyield;
char *sdomain;

/* Starting with ! specifies a negative address item. It is theoretically
possible for a local part to start with !; in that case, a regex has to be
used. */

if (*ss == '!')
  {
  matchyield = am_no;
  while (isspace((uschar)(*(++ss))));
  }
else matchyield = am_yes;

/* Handle a regular expression, which must match the entire incoming address. */

if (*ss == '^')
  {
  if (match_check_string(address, ss, expand_setup, TRUE, caseless, NULL))
    return matchyield;
  goto NO_MATCH;
  }

/* If not a regular expression, either part may begin with an asterisk, and
both parts must match. If there's no '@' in the pattern, then it is just a
domain and treated as if it had *@ on the front. */

sdomain = strrchr(ss, '@');

/* No @ => assume user matches; set domain = whole thing */

if (sdomain == NULL) sdomain = ss;

/* Check the local part if one is given in the list. A null local part
is treated as '*'. */

else
  {
  int sllen = sdomain - ss;
  sdomain += 1;

  /* If the domain in the pattern is apparently null, and the local part in
  the pattern ends in "@", then we have a pattern of the form <something>@@,
  which is to be interpreted as <something>@primary_hostname. */

  if (*sdomain == 0 && ss[sllen - 1] == '@')
    {
    sdomain = primary_hostname_lc;
    sllen--;
    }

  /* Else if the local part in the pattern is precisely "@", and a local part
  is given in the source, and the domain part of the pattern is a lookup,
  then lookup the domain, and use the data returned as a list of local
  parts. If the last item in the list is ">something" it gives another key to
  look up, thus providing a chaining facility. */

  else if (llen > 0 && sllen == 1 && ss[0] == '@' &&
      strchr(sdomain, ';') != NULL)
    {
    int watchdog = 50;
    char *result, *list, *ss;
    char *key = address + llen + 1;
    char buffer[1024];

    /* Loop for handling chains */

    while (key != NULL && watchdog-- > 0)
      {
      int sep = 0;

      if (!match_check_string(key, sdomain, -1, TRUE, FALSE, &result))
        goto NO_MATCH;

      /* Temporarily terminate after the local part */

      address[llen] = 0;
      list = result;

      /* Check for chaining from the last item */

      ss = strrchr(list, ':');
      if (ss == NULL) ss = list; else ss++;
      while (isspace((uschar)*ss)) ss++;
      if (*ss == '>')
        {
        *ss++ = 0;
        while (isspace((uschar)*ss)) ss++;
        key = string_copy(ss);
        }
      else key = NULL;

      /* Look up the local parts provided by the list; negation is permitted.
      If a local part has to begin with !, a regex can be used. */

      while ((ss = string_nextinlist(&list, &sep, buffer, sizeof(buffer)))
             != NULL)
        {
        BOOL localyield = matchyield;

        if (*ss == '!')
          {
          localyield = (localyield == am_yes)? am_no : am_yes;
          while (isspace((uschar)(*(++ss))));
          }

        if (match_check_string(address, ss, -1, TRUE, caseless, NULL))
          {
          address[llen] = '@';
          return localyield;
          }

        if (search_find_defer) break;
        }

      /* Reconnect local part to the address */

      address[llen] = '@';
      }

    /* End of chain loop; panic if too many times */

    if (watchdog <= 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Loop detected in lookup of "
        "local part of %s in %s", address, sdomain);

    /* Otherwise the local part check has failed, so the whole match
    fails. */

    goto NO_MATCH;
    }

  /* If it's not an "@@" style lookup, if there is a local part in the pattern,
  check it against the subject local part; if there is no subject local part,
  the match will fail unless the pattern's local part is "*". */

  if (sllen > 0)
    {
    if (ss[0] == '*')
      {
      int cllen = sllen - 1;
      if (llen < cllen) goto NO_MATCH;
      if (caseless)
        {
        if (strncmpic(address+llen-cllen, ss + 1, cllen) != 0)
          goto NO_MATCH;
        }
      else
        {
        if (strncmp(address+llen-cllen, ss + 1, cllen) != 0)
          goto NO_MATCH;
        }
      if (expand_setup > 0)
        {
        expand_nstring[expand_setup] = address;
        expand_nlength[expand_setup] = llen - cllen;
        expand_inc = 1;
        }
      }
    else
      {
      if (llen != sllen) goto NO_MATCH;
      if (caseless)
        {
        if (strncmpic(address, ss, llen) != 0) goto NO_MATCH;
        }
      else
        {
        if (strncmp(address, ss, llen) != 0) goto NO_MATCH;
        }
      }
    }
  }

/* If the local part matched, or was not being checked, check the domain using
the generalized function, which supports file lookups. */

if (match_check_string(address + llen + 1, sdomain,
    expand_setup + expand_inc, TRUE, caseless, NULL))
  return matchyield;

/* If we have no match and the pattern is a single lookup pattern
without a local part, then try the entire address, but do not do any
partial matching, which won't be appropriate. */

if (sdomain == ss && strchr(ss, ';') != NULL &&
  match_check_string(address, ss, -1, FALSE, caseless, NULL))
    return matchyield;

/* Match failed: if a lookup deferred, return am_ld. Otherwise return whether
the item was positive or negative. */

NO_MATCH:
if (search_find_defer) return am_ld;
return (matchyield == am_yes)? am_pos : am_neg;
}




/*************************************************
*    Test whether address matches address list   *
*************************************************/

/* This function is given an address and a string list of things to match it
against. The list may contain individual addresses, regular expressions, lookup
specifications, and indirection via bare files. Negation is supported. The
address to check can consist of just a domain, which will then match only
domain items or items specified as *@domain. The value of the second argument
must then be given as 0.

Domains are always lower cased before the match. Local parts are also lower
cased if "caseless" is set, until an item of the form +caseful is encountered
in the list. The casefulness of other matching (where possible) is also
controlled by this.

Arguments:
  origaddress   address to test
  domain_offset offset to the domain in the address
  caseless      TRUE if caseless lower part matching initially
  list          string list to check against
  expand_setup  controls setting up of $n variables - passed through
                to match_check_string (q.v.)
  separator     pointer to separator character for the list;
                may be 0 to get separator from the list;
                may be -1 for one-item list
  ldr           what to return if a lookup defers

Returns:        TRUE for a positive match, or end list after a negation;
                FALSE for a negative match, or end list after non-negation;
                ldr if a lookup deferred; search_find_defer will be set
*/

BOOL
match_address_list(char *origaddress, int domain_offset, BOOL caseless,
  char **listptr, int expand_setup, int *separator, BOOL ldr)
{
int llen;
BOOL endyield = FALSE;
char *ss, *p;
char *list = *listptr;
char *ot = NULL;
char *address = big_buffer;
char buffer[1024];

/* Set up the option name text, if we need it for debugging output. This
function is called for lists (actually, single addresses) that aren't options,
in which case the name comes back as an empty string. This is cut out for
standalone testing, which doesn't have the readconf module included. */

#ifndef STAND_ALONE
HDEBUG(1)
  {
  char *opt_name = readconf_find_option(listptr);
    ot = string_sprintf("%s in %s?", origaddress,
      (opt_name[0] != 0)? opt_name : list);
  }
#endif

search_find_defer = FALSE;       /* In case no call to match_check_string() */

/* If the list is empty, the answer is no. This case happens only when checking
named options. */

if (list == NULL)
  {
  HDEBUG(2) debug_printf("%s no (option unset)\n", ot);
  return FALSE;
  }

/* Compute the length of the local part; if domain_offset == 0 the length is
negative. */

llen = domain_offset - 1;

/* If caseless is set, ensure the domain is lower-cased before doing any
comparisons. The mail RFCs specify that local parts are caseful things, and
Exim always preserves the case for remote local parts. However, RFC 2505
recommends that for spam checking, local parts should be caselessly compared.
Therefore, Exim now forces the local part into lower case here for most calls
to this function (caseless == TRUE), but if it encounters a magic item
"+caseful" in the list, it restores a caseful copy from the original address.
*/

sprintf(address, "%.*s", big_buffer_size-1, origaddress);
for (p = address + ((caseless || llen < 0)? 0 : llen); *p != 0; p++)
  *p = tolower(*p);

/* If expand_setup is zero, we need to set up $0 to the whole thing, in
case there is a match. Can't use the built-in facilities of match_check_string,
as we may just be calling that for part of the address (the domain). */

if (expand_setup == 0)
  {
  expand_nstring[0] = address;
  expand_nlength[0] = (int)strlen(address);
  expand_setup++;
  }

/* Loop for each address in the list. */

while ((ss = string_nextinlist(&list, separator, buffer, sizeof(buffer)))
       != NULL)
  {
  /* Handle the magic entry which restores caseful checking of the
  local part. */

  if (strcmp(ss, "+caseful") == 0)
    {
    if (llen > 0) strncpy(address, origaddress, llen);
    caseless = FALSE;
    continue;
    }

  /* If the item does not start with / or !/ it is a single item (but allow
  for spaces after the !) */

  if (ss[strspn(ss, "! \t")] != '/')
    {
    switch(check_address(ss, address, llen, expand_setup, caseless))
      {
      case am_yes:         /* matched a positive item */
      HDEBUG(2) debug_printf("%s yes (%s)\n", ot, ss);
      return TRUE;

      case am_no:          /* matched a negative item */
      HDEBUG(2) debug_printf("%s no (%s)\n", ot, ss);
      return FALSE;

      case am_pos:       /* failed; last item was positive */
      endyield = FALSE;
      break;

      case am_neg:       /* failed; last item was negative */
      endyield = TRUE;
      break;

      case am_ld:        /* lookup deferred */
      HDEBUG(2) debug_printf("%s %s (lookup deferred: %s)\n", ot,
        ldr? "yes" : "no", ss);
      return ldr;
      }
    }

  /* Handle the case of an inline file, possible negated. */

  else
    {
    FILE *f;
    BOOL yes;
    char fbuffer[1024];

    if (*ss == '!')
      {
      yes = FALSE;
      while (isspace((uschar)(*(++ss))));
      }
    else yes = TRUE;

    f = fopen(ss, "r");

    /* ot will be unset unless debugging; in any case, we get better
    wording by not using it. */

    if (f == NULL)
      {
      #ifdef STAND_ALONE
      char *opt_name = "list";
      #else
      char *opt_name = readconf_find_option(listptr);
      if (opt_name[0] == 0) opt_name = string_sprintf("\"%s\"", *listptr);
      #endif
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s",
        string_open_failed(errno, "%s when checking for %s in %s",
          ss, origaddress, opt_name));
      }

    while (fgets(fbuffer, sizeof(fbuffer), f) != NULL)
      {
      /* Because # characters may appear in local parts, we insist on
      whitespace or eol before treating one as comment */

      if ((ss = strchr(fbuffer, '#')) != NULL &&
          (ss[1] == 0 || isspace((uschar)ss[1])))
        *ss = 0;

      ss = fbuffer + (int)strlen(fbuffer);              /* trailing space */
      while (ss > fbuffer && isspace((uschar)ss[-1])) ss--;
      *ss = 0;

      ss = fbuffer;                                     /* leading space */
      while (isspace((uschar)*ss)) ss++;

      if (*ss == 0) continue;                           /* ignore empty */

      switch(check_address(ss, address, llen, expand_setup, caseless))
        {
        case am_yes:         /* matched a positive item */
        HDEBUG(2) debug_printf("%s %s (%s in %s)\n", ot, yes? "yes" : "no",
          ss, buffer);
        fclose(f);
        return yes;

        case am_no:          /* matched a negative item */
        HDEBUG(2) debug_printf("%s %s (%s in %s)\n", ot, yes? "no" : "yes",
          ss, buffer);
        fclose(f);
        return !yes;

        case am_pos:       /* failed; last item was positive */
        endyield = !yes;
        break;

        case am_neg:       /* failed; last item was negative */
        endyield = yes;
        break;

        case am_ld:        /* lookup deferred */
        HDEBUG(2) debug_printf("%s %s (lookup deferred: %s)\n", ot,
          ldr? "yes" : "no", ss);
        fclose(f);
        return ldr;
        }
      }
    fclose(f);
    }
  }

HDEBUG(2) debug_printf("%s %s (end of list)\n", ot, endyield? "yes" : "no");
return endyield;
}




/*************************************************
*              Match sender to list              *
*************************************************/

/* This function is given a colon-separated list to be matched against the
sender of the message.

Argument:
  s          a colon-separated list (after expansion)

Returns:   OK    if the sender matches
           FAIL  if sender does not match
           DEFER if the check could not be completed
*/

int
match_sender(char *s)
{
BOOL yield;
int sep = 0;
char *address = (sender_address == NULL)? "" : sender_address;

if (s == NULL) return FAIL;

yield = match_address_list(
  address,
  (address[0] == 0)? 0 : strchr(address, '@') - address + 1,
  TRUE,
  &s,
  FALSE,
  &sep,
  FALSE);

return yield? OK : search_find_defer? DEFER : FAIL;
}




/*************************************************
**************************************************
*             Stand-alone test program           *
**************************************************
*************************************************/

#ifdef STAND_ALONE

BOOL
regex_match_and_setup(pcre *re, char *subject, int options, int setup)
{
int ovector[3*(EXPAND_MAXN+1)];
int n = pcre_exec(re, NULL, subject, (int)strlen(subject), 0, PCRE_EOPT|options,
  ovector, sizeof(ovector)/sizeof(int));
BOOL yield = n >= 0;
if (n == 0) n = EXPAND_MAXN + 1;
if (yield)
  {
  int nn;
  expand_nmax = (setup < 0)? 0 : setup + 1;
  for (nn = (setup < 0)? 0 : 2; nn < n*2; nn += 2)
    {
    expand_nstring[expand_nmax] = subject + ovector[nn];
    expand_nlength[expand_nmax++] = ovector[nn+1] - ovector[nn];
    }
  expand_nmax--;
  }
return yield;
}


pcre *
regex_must_compile(char *pattern, BOOL caseless, BOOL use_malloc)
{
int offset;
int options = PCRE_COPT;
pcre *yield;
const char *error;
if (caseless) options |= PCRE_CASELESS;
yield = pcre_compile(pattern, options, &error, &offset, NULL);
if (yield == NULL)
  log_write(0, LOG_PANIC_DIE, "regular expression error: %s at offset %d "
    "while compiling %s", error, offset, pattern);
return yield;
}

char *
parse_extract_address(char *mailbox, char **errorptr, int *start, int *end,
  int *domain, BOOL allow_null)
{
return NULL;
}





int main(int argc, char **argv)
{
char listbuffer[256];
char subjbuffer[256];
debug_level = 1;
debug_file = stderr;
debug_fd = fileno(debug_file);
big_buffer = malloc(big_buffer_size);

primary_hostname_lc = "local.host";

if (argc > 1) debug_level = atoi(argv[1]);

printf("Testing match functions: debug_level = %d\n\n", debug_level);

printf("Testing match_isinlist()\n");

for (;;)
  {
  int len;

  printf("List: ");
  if (fgets(listbuffer, 256, stdin) == NULL) break;

  len = (int)strlen(listbuffer);
  listbuffer[len-1] = 0;

  for(;;)
    {
    int rc;
    char *value;
    char *list = listbuffer;
    printf("Subj: ");
    if (fgets(subjbuffer, 256, stdin) == NULL) break;
    len = (int)strlen(subjbuffer);
    subjbuffer[len-1] = 0;
    rc = match_isinlist(subjbuffer, &list, TRUE, FALSE, &value);
    if (rc)
      printf("OK %s\n", (value == NULL)? "NULL" : value);
    else printf("%s\n", search_find_defer? "DEFER" : "FAIL");
    }
  printf("\n");
  }
printf("\n");

search_tidyup();


printf("Testing match_sender()\n");

for (;;)
  {
  int len;

  printf("List: ");
  if (fgets(listbuffer, 256, stdin) == NULL) break;

  len = (int)strlen(listbuffer);
  listbuffer[len-1] = 0;

  for(;;)
    {
    int rc;
    printf("Subj: ");
    if (fgets(subjbuffer, 256, stdin) == NULL) break;
    len = (int)strlen(subjbuffer);
    subjbuffer[len-1] = 0;
    sender_address = subjbuffer;
    rc = match_sender(listbuffer);
    printf("%s\n", (rc == OK)? "OK" : (rc == DEFER)? "DEFER" : "FAIL");
    }
  printf("\n");
  }
printf("\n");

search_tidyup();


printf("Testing match_address_list()\n");

for (;;)
  {
  int len;
  int sep = 0;
  char *list = listbuffer;

  printf("List: ");
  if (fgets(listbuffer, 256, stdin) == NULL) break;

  len = (int)strlen(listbuffer);
  listbuffer[len-1] = 0;

  for(;;)
    {
    int domain_offset;
    char *s;
    printf("Subj: ");
    if (fgets(subjbuffer, 256, stdin) == NULL) break;
    len = (int)strlen(subjbuffer);
    subjbuffer[len-1] = 0;
    s = strchr(subjbuffer, '@');
    domain_offset = (s == NULL)? 0 : s - subjbuffer + 1;
    if (match_address_list(subjbuffer, domain_offset, TRUE, &list, -1, &sep,
      FALSE)) printf("OK\n");
    else
      printf("Failed%s\n", search_find_defer? " (lookup deferred)" : "");
    }
  printf("\n");
  }
printf("\n");

search_tidyup();


return 0;
}

#endif

/* End of match.c */
