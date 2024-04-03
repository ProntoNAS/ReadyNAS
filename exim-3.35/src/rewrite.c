/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions concerned with rewriting headers */


#include "exim.h"

/* Names for testing rewriting */

static char *rrname[] = {
  "  sender",
  "    from",
  "      to",
  "      cc",
  "     bcc",
  "reply-to",
  "env-from",
  "  env-to"
};



/*************************************************
*            Ensure an address is qualified      *
*************************************************/

/*

Arguments:
  s              address to check
  is_recipient   TRUE if a recipient address; FALSE if a sender address

Returns:         fully-qualified address
*/

char *
rewrite_address_qualify(char *s, BOOL is_recipient)
{
return (parse_find_at(s) != NULL)? s :
  string_sprintf("%s@%s", s,
    is_recipient? qualify_domain_recipient : qualify_domain_sender);
}



/*************************************************
*               Rewrite a single address         *
*************************************************/

/* The yield is the input address if there is no rewriting to be done. Assume
the input is a valid address, except in the case of SMTP-time rewriting, which
is handled specially. When this function is called while processing filter and
forward files, the uid may be that of the user. Ensure it is reset while
expanding a replacement, in case that involves file lookups.

Arguments:
  s              address to rewrite
  flag           indicates which header this address comes from; it must
                   match the flags in the rewriting rule
  whole          if not NULL, set TRUE if any rewriting rule contained the
                   "whole" bit and it is a header that is being rewritten
  add_header     if TRUE and rewriting occurs, add an "X-rewrote-xxx" header
                   if headers are in existence; this should be TRUE only when
                   a message is being received, not during delivery
  name           name of header, for use when adding X-rewrote-xxxx
  rewrite_rules  chain of rewriting rules

Returns:         new address if rewritten; the input address if no change;
                 for a header rewrite, if the "whole" bit is set, the entire
                 rewritten address is returned, not just the active bit.
*/

char *
rewrite_one(char *s, int flag, BOOL *whole, BOOL add_header, char *name,
  rewrite_rule *rewrite_rules)
{
rewrite_rule *rule;
char *yield = s;
char *subject = s;
char *domain = NULL;
BOOL uid_checked = FALSE;
BOOL uid_set = FALSE;
BOOL done = FALSE;
uid_t saved_euid = 0;
gid_t saved_egid = 0;
int rule_number = 1;
int sep = UCHAR_MAX + 1;
int yield_start = 0, yield_end = 0;

if (whole != NULL) *whole = FALSE;

/* Scan the rewriting rules */

for (rule = rewrite_rules;
     rule != NULL && !done;
     rule_number++, rule = rule->next)
  {
  int start, end, pdomain;
  int count = 0;
  char *save_localpart, *save_domain;
  char *error, *new, *newparsed;

  /* Ensure that the flag matches the flags in the rule. */

  if ((rule->flags & flag) == 0) continue;

  /* Come back here for a repeat after a successful rewrite. We do this
  only so many times. */

  REPEAT_RULE:

  /* If this is an SMTP-time rewrite, the pattern must be a regex and
  the subject may have any structure. No local part or domain variables
  can be set for the expansion. */

  if ((flag & rewrite_smtp) != 0)
    {
    if (!match_check_string(subject, rule->key, 0, TRUE, FALSE, NULL)) continue;
    new = expand_string(rule->replacement);
    }

  /* All other rewrites expect the input to be a valid address, so local part
  and domain variables can be set for expansion. For the first rule, to be
  applied to this address, domain will be NULL and needs to be set. */

  else
    {
    if (domain == NULL) domain = strrchr(subject, '@') + 1;

    /* Use the general function for matching an address against a list (here
    just one item, so set the separator to 0). */

    if (!match_address_list(subject, domain-subject, FALSE, &(rule->key), 0,
      &sep, FALSE)) continue;

    /* The source address matches, and numerical variables have been
    set up. If the replacement string consists of precisely "*" then no
    rewriting is required for this address - the behaviour is as for "fail"
    in the replacement expansion, but assuming the quit flag. */

    if (strcmp(rule->replacement, "*") == 0) break;

    /* Otherwise, expand the replacement string, changing the uid/gid first if
    necessary. While doing the expansion, set $local_part and $domain to the
    appropriate values, restoring whatever value they previously had afterwards.
    */

    if (!uid_checked)
      {
      uid_t uid = exim_uid_set? exim_uid : root_uid;
      uid_checked = TRUE;
      saved_euid = geteuid();
      saved_egid = getegid();
      if (saved_euid != uid)
        {
        if (saved_euid != root_uid) mac_seteuid(root_uid);
        if (exim_gid_set) mac_setegid(exim_gid);
        mac_seteuid(uid);
        uid_set = TRUE;
        DEBUG(2) debug_print_ids("rewrite_one");
        }
      }

    save_localpart = deliver_localpart;
    save_domain = deliver_domain;

    /* We have subject pointing to "localpart@domain" and domain pointing to
    the domain. Temporarily terminate the local part so that it can be
    set up as an expansion variable */

    domain[-1] = 0;
    deliver_localpart = subject;
    deliver_domain = domain;

    new = expand_string(rule->replacement);

    domain[-1] = '@';
    deliver_localpart = save_localpart;
    deliver_domain = save_domain;
    }

  /* If the expansion failed with the "forcedfail" flag, don't generate
  an error - just give up on this rewriting rule. If the "q" flag is set,
  give up altogether. For other expansion failures we have a configuration
  error. */

  if (new == NULL)
    {
    if (expand_string_forcedfail)
      { if ((rule->flags & rewrite_quit) != 0) break; else continue; }
    log_write(0, LOG_MAIN|LOG_PANIC, "Expansion of %s failed while rewriting: "
      "%s", rule->replacement, expand_string_message);
    break;
    }

  /* Validate what has been generated, qualifying if permitted to do so.
  Only envelope from is permitted to be rewritten as <>.*/

  if ((rule->flags & rewrite_qualify) != 0)
    new = rewrite_address_qualify(new, TRUE);

  newparsed = parse_extract_address(new, &error, &start, &end, &pdomain,
    flag == rewrite_envfrom);

  if (newparsed == NULL)
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "Rewrite of %s yielded unparseable "
      "address: %s in address %s", subject, error, new);
    }
  else if (pdomain == 0 && (flag != rewrite_envfrom || *newparsed != 0))
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "Rewrite of %s yielded unqualified "
      "address \"%s\"", subject, new);
    }
  else
    {
    DEBUG(2) debug_printf("\"%s\" rewritten as ", yield);

    if (log_rewrites)
      log_write(0, LOG_MAIN, "\"%s\" rewritten as \"%s\" by rule %d",
        yield, new, rule_number);

    /* A header will only actually be added if header_last is non-NULL,
    i.e. during message reception or delivery, but add_header should not
    be set TRUE during delivery, as otherwise multiple instances of the header
    can fill up the -H file and make it embarrassingly large. We don't need
    to set header_changed because the -H file always gets written at the end of
    message reception. */

    if (add_header)
      header_add(htype_old, "X-rewrote-%s: %s\n", name, subject);

    /* Handle the case when replacement of the whole address is possible.
    This happens only when whole is not NULL and we are rewriting a header.
    If *whole is already TRUE it means that a previous rule had the w
    flag set and so we must preserve the non-active portion of the current
    subject unless the current rule also has the w flag set. */

    if (whole != NULL && (flag & rewrite_all_headers) != 0)
      {
      /* Current rule has the w flag set */

      if ((rule->flags & rewrite_whole) != 0)
        {
        yield = new;
        yield_start = start;
        yield_end = end;
        subject = newparsed;
        *whole = TRUE;
        }

      /* Current rule does not have the w flag set; if not previously
      done any whole rewriting, behave in non-whole manner. */

      else if (!*whole) goto NEVER_WHOLE;

      /* Current rule does not have the w flag set, but a previous
      rule did rewrite the whole address. Thus yield and subject will be
      different. Preserve the previous non-active part of the address. */

      else
        {
        subject = newparsed;
        new = string_sprintf("%.*s%s%n%s",
           yield_start, yield, subject, &end, yield + yield_end);
        yield_end = end;
        yield = new;
        }
      }

    /* Rule just rewrites active part, or handling an envelope. This
    code is obeyed only when all rules so far have not done "whole"
    replacement. */

    else
      {
      NEVER_WHOLE:
      subject = yield = newparsed;
      }

    DEBUG(2) debug_printf("\"%s\"\n", yield);
    domain = NULL;    /* Reset for next rule */
    }

  /* If no further rewrites are to be done, set the done flag. This does
  allow repeats of the current rule if configured. */

  if ((rule->flags & rewrite_quit) != 0) done = TRUE;

  /* Allow the current rule to be applied up to 10 times if
  requested. */

  if ((rule->flags & rewrite_repeat) != 0)
    {
    if (count++ < 10) goto REPEAT_RULE;
    log_write(0, LOG_MAIN|LOG_PANIC, "rewrite rule repeat ignored after 10 "
      "times");
    }
  }

/* Reset the uid/gid if necessary; free the original subject if it
has been rewritten; unset expansion numeric variables, and that's it. */

if (uid_set)
  {
  mac_seteuid(root_uid);
  mac_setegid(saved_egid);
  mac_seteuid(saved_euid);
  DEBUG(2) debug_print_ids("restored");
  }
expand_nmax = -1;
return yield;
}



/*************************************************
*         Ensure qualification and rewrite       *
*************************************************/

/* This function is called for envelope addresses, the boolean specifying
whether a recipient or a sender. It must first of all ensure the address is
fully qualified, and then apply any relevant re-writing rules. The add-header
flag causes a header to be added, recording the old address. This is marked
"old", so that it is never transported anywhere; it exists for local checking
and debugging purposes.

Arguments:
  s              the address to be considered
  is_recipient   TRUE for recipient addresses; FALSE otherwise
  add_header     add "X-rewrote-xxx" header when rewriting; this is
                   set TRUE only for calls from the reception functions
  rewrite_rules  points to chain of rewrite rules
  existflags     bits indicating which headers there are rewrites for
                 (just an optimisation)

Returns:         possibly rewritten address
*/

char *
rewrite_address(char *s, BOOL is_recipient, BOOL add_header,
  rewrite_rule *rewrite_rules, int existflags)
{
int flag = is_recipient? rewrite_envto : rewrite_envfrom;
s = rewrite_address_qualify(s, is_recipient);
if ((existflags & flag) != 0)
  {
  char *new = rewrite_one(s, flag, NULL, add_header, is_recipient?
    "original-recipient" : "sender", rewrite_rules);
  if (new != s) s = new;
  }
return s;
}



/*************************************************
*    Qualify and possibly rewrite one header     *
*************************************************/

/* This is called only from rewrite_header() below, either when reading a
message. or when routing, in order to rewrite addresses that get changed by a
router. This is normally the addition of full qualification to a partial
domain. The first rewriting rule in this case is "change routed_old into
routed_new", and it applies to all header lines that contain addresses. Then
header-specific rewriting rules are applied.

Before rewriting can be done, addresses without domains have to be qualified.
This should only be done for messages from "local" senders. This is a difficult
concept to pin down, what with the use of SMTP both as a submission and as a
transmission protocol. Exim normally requires incoming SMTP to contain fully-
qualified addresses, but there are options to permit unqualified ones from
certain hosts. For those hosts only, addresses in headers can also be
qualified. For other hosts, unqualified addresses in headers do not get touched
in any way. For locally sourced messages, unqualified addresses always get
qualified.

Arguments:
  h              pointer to header line block
  flag           indicates which header this is
  routed_old     if not NULL, this is a rewrite caused by a router, changing
                   this domain into routed_new
  routed_new     new routed domain if routed_old is not NULL
  rewrite_rules  points to chain of rewriting rules
  existflags     bits indicating which rewrites exist
  replace        if TRUE, insert the new header in the chain after the old
                   one, and mark the old one "replaced"

Returns:         NULL if header unchanged; otherwise the rewritten header
*/

static header_line *
rewrite_one_header(header_line *h, int flag, char *routed_old,
  char *routed_new, rewrite_rule *rewrite_rules, int existflags, BOOL replace)
{
int lastnewline = 0;
header_line *newh = NULL;
char *s = strchr(h->text, ':') + 1;
while (isspace((uschar)*s)) s++;

DEBUG(5) debug_printf("rewrite_one_header: type=%c:\n  %s", h->type, h->text);

parse_allow_group = TRUE;     /* Allow group syntax */

/* Loop for multiple addresses in the header. We have to go through them all
in case any need qualifying, even if there's no rewriting. Pathological headers
may have thousands of addresses in them, so cause the store to be reset for
any that don't actually get rewritten. */

while (*s != 0)
  {
  char *sprev;
  char *ss = parse_find_address_end(s, FALSE);
  char *recipient, *new, *errmess;
  void *reset_point = store_get(0);
  BOOL changed = FALSE;
  int terminator = *ss;
  int start, end, domain;

  /* Temporarily terminate the string at this point, and extract the
  operative address within. Then put back the terminator and prepare for
  the next address, saving the start of the old one. */

  *ss = 0;
  recipient = parse_extract_address(s,&errmess,&start,&end,&domain,FALSE);
  *ss = terminator;
  sprev = s;
  s = ss + (terminator? 1:0);
  while (isspace((uschar)*s)) s++;

  /* There isn't much we can do for syntactic disasters at this stage.
  Pro tem (possibly for ever) ignore them. */

  if (recipient == NULL) continue;

  /* If routed_old is not NULL, this is a rewrite caused by a router,
  consisting of changing routed_old into routed_new, and applying to all
  headers. If the header address has no domain, it is excluded, since a router
  rewrite affects domains only. The new value should always be fully qualified,
  but it may be something that has an explicit re-write rule set, so we need to
  check the configured rules subsequently as well. (Example: there's an
  explicit rewrite turning *.foo.com into foo.com, and an address is supplied
  as abc@xyz, which the DNS lookup turns into abc@xyz.foo.com). However, if no
  change is made here, don't bother carrying on. */

  if (routed_old != NULL)
    {
    if (domain <= 0 || strcmpic(recipient+domain, routed_old) != 0) continue;
    recipient[domain-1] = 0;
    new = string_sprintf("%s@%s", recipient, routed_new);
    DEBUG(2)
      {
      recipient[domain-1] = '@';
      debug_printf("%s rewritten by router as %s\n", recipient, new);
      }
    recipient = new;
    changed = TRUE;
    }

  /* This is not a router-inspired rewrite. Ensure the address is fully
  qualified if that is permitted. If an unqualified address was received
  from a host that isn't listed, do not continue rewriting this address.
  Sender, From or Reply-To headers are treated as senders, the rest as
  recipients. This matters only when there are different qualify strings. */

  else
    {
    BOOL is_recipient =
      (flag & (rewrite_sender | rewrite_from | rewrite_replyto)) == 0;
    new = rewrite_address_qualify(recipient, is_recipient);
    changed = (new != recipient);
    recipient = new;

    /* Can only qualify if permitted; if not, no rewrite. */

    if (changed && ((is_recipient && !allow_unqualified_recipient) ||
                    (!is_recipient && !allow_unqualified_sender)))
      continue;
    }

  /* If there are rewrite rules for this type of header, apply
  them. This test is just for efficiency, to save scanning the rules
  in cases when nothing is going to change. If any rewrite rule had the
  "whole" flag set, adjust the pointers so that the whole address gets
  replaced, except possibly a final \n. */

  if ((existflags & flag) != 0)
    {
    BOOL whole;
    new = rewrite_one(recipient, flag, &whole, FALSE, NULL, rewrite_rules);
    if (new != recipient)
      {
      changed = TRUE;
      if (whole)
        {
        start = 0;
        end = ss - sprev;
        if (sprev[end-1] == '\n') end--;
        }
      }
    }

  /* If the address has changed, create a new header containing the
  rewritten address. We do not need to set the chain pointers at this
  stage. */

  if (changed)
    {
    int remlen;
    int newlen = (int)strlen(new);
    int oldlen = end - start;

    header_line *prev = (newh == NULL)? h : newh;
    header_line *newnewh =
      store_get(sizeof(header_line) + prev->slen - oldlen + newlen + 4);
    char *newt = store_get(prev->slen - oldlen + newlen + 4);

    newnewh->text = newt;
    newnewh->type = prev->type;
    newnewh->slen = prev->slen - oldlen + newlen;

    /* Build the new header text by copying the old and putting in the
    replacement. This process may make the header substantially longer
    than it was before - qualification of a list of bare addresses can
    often do this - so we stick in a newline after the re-written address
    if it has increased in length and ends more than 40 characters in. In
    fact, the code is not perfect, since it does not scan for existing
    newlines in the header, but it doesn't seem worth going to that
    amount of trouble. */

    strncpy(newt, prev->text, sprev - prev->text + start);
    newt += sprev - prev->text + start;
    *newt = 0;
    strcat(newt, new);
    newt += newlen;
    remlen = s - (sprev + end);
    if (remlen > 0)
      {
      strncpy(newt, sprev + end, remlen);
      newt += remlen;
      *newt = 0;
      }

    /* Must check that there isn't a newline here anyway; in particular, there
    will be one at the very end of the header, where we DON'T want to insert
    another one! The pointer s has been skipped over white space, so just
    look back to see if the last non-space-or-tab was a newline. */

    if (newlen > oldlen && newt - newnewh->text - lastnewline > 40)
      {
      char *p = s - 1;
      while (p >= prev->text && (*p == ' ' || *p == '\t')) p--;
      if (*p != '\n')
        {
        lastnewline = newt - newnewh->text;
        strcat(newt, "\n\t");
        newnewh->slen += 2;
        }
      }

    /* Finally, the remaining unprocessed addresses, if any. */

    strcat(newt, s);

    DEBUG(5) debug_printf("newlen=%d newtype=%c newtext:\n%s",
      newnewh->slen, newnewh->type, newnewh->text);

    s = newnewh->text + (s - prev->text) - oldlen + newlen;

    DEBUG(5) debug_printf("remainder: %s", (*s == 0)? "\n" : s);

    newh = newnewh;
    }

  /* If the address was not changed, reset so as to reuse the store it
  was extracted into. */

  else store_reset(reset_point);
  }

parse_allow_group = FALSE;  /* Reset group flags */
parse_found_group = FALSE;

/* If a rewrite happened and "replace" is true, put the new header into the
chain following the old one, and mark the old one as replaced. */

if (newh != NULL && replace)
  {
  newh->next = h->next;
  if (newh->next == NULL) header_last = newh;
  h->type = htype_old;
  h->next = newh;
  }

return newh;
}




/*************************************************
*              Rewrite a header line             *
*************************************************/

/* This function may be passed any old header line. It must detect those which
contain addresses, then then apply any rewriting rules that apply. If
routed_old is NULL, only the configured rewriting rules are consulted.
Otherwise, the rewriting rule is "change routed_old into routed_new", and it
applies to all header lines that contain addresses. Then header-specific
rewriting rules are applied.

The old header line is flagged as "old". Old headers are saved on the spool for
debugging but are never sent to any recipients.

Arguments:
  h               header line to rewrite
  routed_old     if not NULL, this is a rewrite caused by a router, changing
                   this domain into routed_new
  routed_new     new routed domain if routed_old is not NULL
  rewrite_rules  points to chain of rewrite rules
  existflags     bits indicating which rewrites exist
  replace        if TRUE, the new header is inserted into the header chain
                    after the old one, and the old one is marked replaced

Returns:         NULL if header unchanged; otherwise the rewritten header
*/

header_line *
rewrite_header(header_line *h, char *routed_old, char *routed_new,
  rewrite_rule *rewrite_rules, int existflags, BOOL replace)
{
switch (h->type)
  {
  case htype_sender:
  return rewrite_one_header(h, rewrite_sender, routed_old, routed_new,
    rewrite_rules, existflags, replace);

  case htype_from:
  return rewrite_one_header(h, rewrite_from, routed_old, routed_new,
    rewrite_rules, existflags, replace);

  case htype_to:
  return rewrite_one_header(h, rewrite_to, routed_old, routed_new,
    rewrite_rules, existflags, replace);

  case htype_cc:
  return rewrite_one_header(h, rewrite_cc, routed_old, routed_new,
    rewrite_rules, existflags, replace);

  case htype_bcc:
  return rewrite_one_header(h, rewrite_bcc, routed_old, routed_new,
    rewrite_rules, existflags, replace);

  case htype_replyto:
  return rewrite_one_header(h, rewrite_replyto, routed_old, routed_new,
    rewrite_rules, existflags, replace);
  }

return NULL;
}



/************************************************
*            Test rewriting rules               *
************************************************/

/* Called from the mainline as a result of the -brw option. Test the
address for all possible cases.

Argument: the address to test
Returns:  nothing
*/

void rewrite_test(char *s)
{
char *recipient, *error;
int i, start, end, domain;
BOOL done_smtp = FALSE;

if (rewrite_existflags == 0)
  {
  printf("No rewrite rules are defined\n");
  return;
  }

/* Do SMTP rewrite only if a rule with the S flag exists. */

if ((rewrite_existflags & rewrite_smtp) != 0)
  {
  char *new = rewrite_one(s, rewrite_smtp, NULL, FALSE, "",
    global_rewrite_rules);
  if (new != s)
    {
    printf("    SMTP: %s\n", new);
    done_smtp = TRUE;
    }
  }

/* Do the other rewrites only if a rule without the S flag exists */

if ((rewrite_existflags & ~(rewrite_smtp|rewrite_X)) == 0) return;

/* Qualify if necessary before extracting the address */

if (parse_find_at(s) == NULL)
  s = string_sprintf("%s@%s", s, qualify_domain_recipient);

recipient = parse_extract_address(s, &error, &start, &end, &domain, FALSE);

if (recipient == NULL)
  {
  if (!done_smtp)
    printf("Syntax error in %s\n%c%s\n", s, toupper(error[0]), error+1);
  return;
  }

for (i = 0; i < 8; i++)
  {
  BOOL whole = FALSE;
  int flag = 1 << i;
  char *new = rewrite_one(recipient, flag, &whole, FALSE, "",
    global_rewrite_rules);
  printf("%s: ", rrname[i]);
  if (whole || (flag & rewrite_all_headers) == 0) printf("%s\n", new);
    else printf("%.*s%s%s\n", start, s, new, s+end);
  }

/* Only show the X case if there is a rewrite rule for it. */

if ((rewrite_existflags & rewrite_X) != 0)
  {
  char *new = rewrite_one(recipient, rewrite_X, NULL, FALSE, "",
    global_rewrite_rules);
  if (new != recipient) printf("       X: %s\n", new);
  }
}

/* End of rewrite.c */
