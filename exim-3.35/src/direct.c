/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* General functions concerned with directing, and generic director options. */


#include "exim.h"



/* Generic options for directors, all of which live inside director_instance
data blocks and hence have the opt_public flag set. */

optionlist optionlist_directors[] = {
  { "*expand_group",      opt_stringptr | opt_hidden | opt_public,
                 (void *)(offsetof(director_instance, expand_gid)) },
  { "*expand_transport",  opt_stringptr|opt_public|opt_hidden,
                 (void *)offsetof(director_instance, expand_transport) },
  { "*expand_user",       opt_stringptr | opt_hidden | opt_public,
                 (void *)(offsetof(director_instance, expand_uid)) },
  { "*set_group",         opt_bool | opt_hidden | opt_public,
                 (void *)(offsetof(director_instance, gid_set)) },
  { "*set_user",          opt_bool | opt_hidden | opt_public,
                 (void *)(offsetof(director_instance, uid_set)) },
  { "condition",          opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, condition) },
  { "current_directory",  opt_stringptr|opt_public,
                 (void *)(offsetof(director_instance, current_directory)) },
  { "debug_print",        opt_stringptr | opt_public,
                 (void *)offsetof(director_instance, debug_string) },
  { "domains",            opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, domains) },
  { "driver",             opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, driver_name) },
  { "errors_to",          opt_stringptr|opt_public,
                 (void *)(offsetof(director_instance, errors_to)) },
  { "expn",               opt_bool|opt_public,
                 (void *)offsetof(director_instance, expn) },
  { "fail_verify",        opt_bool_verify|opt_hidden|opt_public,
                 (void *)offsetof(director_instance, fail_verify_sender) },
  { "fail_verify_recipient", opt_bool|opt_public,
                 (void *)offsetof(director_instance, fail_verify_recipient) },
  { "fail_verify_sender", opt_bool|opt_public,
                 (void *)offsetof(director_instance, fail_verify_sender) },
  { "fallback_hosts",     opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, fallback_hosts) },
  { "group",              opt_expand_gid | opt_public,
                 (void *)(offsetof(director_instance, gid)) },
  { "headers_add",        opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, extra_headers) },
  { "headers_remove",     opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, remove_headers) },
  { "home_directory",     opt_stringptr|opt_public,
                 (void *)(offsetof(director_instance, home_directory)) },
  { "initgroups",         opt_bool | opt_public,
                 (void *)(offsetof(director_instance, initgroups)) },
  { "local_parts",        opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, local_parts) },
  { "more",               opt_bool|opt_public,
                 (void *)offsetof(director_instance, more) },
  { "new_director",       opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, new_director) },
  { "prefix",             opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, prefix) },
  { "prefix_optional",    opt_bool|opt_public,
                 (void *)offsetof(director_instance, prefix_optional) },
  { "require_files",      opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, require_files) },
  { "senders",            opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, senders) },
  { "suffix",             opt_stringptr|opt_public,
                 (void *)offsetof(director_instance, suffix) },
  { "suffix_optional",    opt_bool|opt_public,
                 (void *)offsetof(director_instance, suffix_optional) },
  { "transport",          opt_transportptr|opt_public,
                 (void *)offsetof(director_instance, transport) },
  { "unseen",             opt_bool|opt_public,
                 (void *)offsetof(director_instance, unseen) },
  { "user",               opt_expand_uid | opt_public,
                 (void *)(offsetof(director_instance, uid)) },
  { "verify",             opt_bool_verify | opt_hidden|opt_public,
                 (void *)offsetof(director_instance, verify_sender) },
  { "verify_only",        opt_bool|opt_public,
                 (void *)offsetof(director_instance, verify_only) },
  { "verify_recipient",   opt_bool|opt_public,
                 (void *)offsetof(director_instance, verify_recipient) },
  { "verify_sender",      opt_bool|opt_public,
                 (void *)offsetof(director_instance, verify_sender) }
};

int optionlist_directors_size =
  sizeof(optionlist_directors)/sizeof(optionlist);




/*************************************************
*      Set uid/gid from block into address       *
*************************************************/

/* This function copies any set uid or gid from a ugid block into an
address.

Arguments:
  addr        the address
  ugid        the ugid block

Returns:      nothing
*/

void
direct_set_ugid(address_item *addr, ugid_block *ugid)
{
if (ugid->uid_set)
  {
  addr->uid = ugid->uid;
  setflag(addr, af_uid_set);
  }

if (ugid->gid_set)
  {
  addr->gid = ugid->gid;
  setflag(addr, af_gid_set);
  }

if (ugid->initgroups) setflag(addr, af_initgroups);
}





/*************************************************
*          Get uid/gid for a director/router     *
*************************************************/

/* This function is called by both directors and routers to sort out the
uid/gid values which are passed with an address for use by local transports.
The fields it uses are identical in both director and router blocks.

Arguments:
  dblock       the director/router block
  driver_type  "director" or "router"
  addr         the address being worked on
  ugid         pointer to a ugid block to fill in

Returns:       TRUE if all goes well, else FALSE
*/

BOOL
direct_get_ugid(director_instance *dblock, char *driver_type,
  address_item *addr, ugid_block *ugid)
{
struct passwd *upw = NULL;

/* Initialize from fixed values */

ugid->uid = dblock->uid;
ugid->gid = dblock->gid;
ugid->uid_set = dblock->uid_set;
ugid->gid_set = dblock->gid_set;
ugid->initgroups = dblock->initgroups;

/* If there is no fixed uid set, see if there's a dynamic one that can
be expanded and possibly looked up. */

if (!ugid->uid_set && dblock->expand_uid != NULL)
  {
  if (direct_find_expanded_user(dblock->expand_uid, dblock->name, driver_type,
    &upw, &(ugid->uid), &(addr->message))) ugid->uid_set = TRUE;
  else
    {
    addr->special_action = SPECIAL_FREEZE;
    return FALSE;
    }
  }

/* Likewise for the gid */

if (!ugid->gid_set && dblock->expand_gid != NULL)
  {
  if (direct_find_expanded_group(dblock->expand_gid, dblock->name, driver_type,
    &(ugid->gid), &(addr->message))) ugid->gid_set = TRUE;
  else
    {
    addr->special_action = SPECIAL_FREEZE;
    return FALSE;
    }
  }

/* If a uid is set, then a gid must also be available; use one from the passwd
lookup if it happened. */

if (ugid->uid_set && !ugid->gid_set)
  {
  if (upw != NULL)
    {
    ugid->gid = upw->pw_gid;
    ugid->gid_set = TRUE;
    }
  else
    log_write(0, LOG_PANIC_DIE, "User set without group for %s %s",
      dblock->name, driver_type);
  }

return TRUE;
}




/*************************************************
*    Get errors address for a director/router    *
*************************************************/

/* This function is called by both directors and routers to sort out the errors
address for a particular address. If there is a setting in the director (or
router) block, then expand and verify it, and if it works, use it. Otherwise
use any setting that is in the address itself. This might be NULL, meaning
unset (the message's sender is then used). Verification isn't done when the
original address is just being verified, as otherwise there might be directing
loops if someone sets up a silly configuration.

Arguments:
  addr         the input address
  dblock       the director or router instance (the value used comes from
                 the part of the block that they have in common)
  type         "director" or "router"
  verify       TRUE if verifying
  errors_to    point the errors address here

Returns:       OK if no problem
               DEFER if verifying the address caused a deferment
               ERROR big disaster (e.g. expansion failure)
*/

int
direct_get_errors_address(address_item *addr, director_instance *dblock,
  char *type, BOOL verify, char **errors_to)
{
char *s;

*errors_to = addr->errors_address;
if (dblock->errors_to == NULL) return OK;

s = expand_string(dblock->errors_to);

if (s == NULL)
  {
  addr->message = string_sprintf("%s %s failed to expand \"%s\": %s",
    dblock->name, type, dblock->errors_to, expand_string_message);
  if (search_find_defer) return DEFER;
  addr->special_action = SPECIAL_FREEZE;
  return ERROR;
  }

/* If we are already verifying, or if we are just testing an address, do not
check the errors address. Apart from saving effort, this ensures that the saved
values of the per-address expansion variables for the original address don't
get clobbered by a verify-within-a-verify.

If we do need to verify, set the sender address to null, because that's what
it will be when sending an error message, and there are now configuration
options that control the running of directors and routers by checking
the sender address. When testing an address, there may not be a sender
address. We also need to save and restore the expansion values associated with
an address. */

if (verify || address_test_mode) *errors_to = s; else
  {
  char *snew;
  int save1 = 0;
  if (sender_address != NULL)
    {
    save1 = sender_address[0];
    sender_address[0] = 0;
    }
  deliver_save_expansions();
  DEBUG(2) debug_printf("------ Verifying errors address %s ------\n", s);
  if (verify_address(s, NULL, NULL, &snew, vopt_is_recipient | vopt_local)
    == OK) *errors_to = snew;
  DEBUG(2) debug_printf("------ End verifying errors address %s ------\n", s);
  deliver_restore_expansions();
  if (sender_address != NULL) sender_address[0] = save1;
  }

return OK;
}




/*************************************************
*  Get additional headers for a director/router  *
*************************************************/

/* This function is called by both directors and routers to sort out the
additional headers and header remove list for a particular address.

Arguments:
  addr           the input address
  dblock         the director or router instance (the value used comes from
                   the part of the block that they have in common)
  type           "director" or "router"
  extra_headers  points to where to hang the header chain
  remove_headers points to where to hang the remove list

Returns:         OK if no problem
                 DEFER if expanding a string caused a deferment
                 ERROR big disaster (e.g. expansion failure)
*/

int
direct_get_munge_headers(address_item *addr, director_instance *dblock,
  char *type, header_line **extra_headers, char **remove_headers)
{
/* Default is to retain existing headers */

*extra_headers = addr->extra_headers;

if (dblock->extra_headers != NULL)
  {
  header_line *h;
  char *s = expand_string(dblock->extra_headers);

  if (s == NULL)
    {
    if (!expand_string_forcedfail)
      {
      addr->message = string_sprintf("%s %s failed to expand \"%s\": %s",
        dblock->name, type, dblock->extra_headers, expand_string_message);
      if (search_find_defer) return DEFER;
      addr->special_action = SPECIAL_FREEZE;
      return ERROR;
      }
    }

  /* Expand succeeded. Put extra header at the start of the chain because
  further down it may point to headers from other directors, which may be
  shared with other addresses. The output function outputs them in reverse
  order. */

  else
    {
    char *addnl = "";
    int slen = (int)strlen(s);
    if (slen > 0)
      {
      if (s[slen-1] != '\n')
        {
        addnl = "\n";
        slen++;
        }
      h = store_get(sizeof(header_line));
      h->text = string_sprintf("%s%s", s, addnl);
      h->next = addr->extra_headers;
      h->type = htype_other;
      h->slen = slen;
      *extra_headers = h;
      }
    }
  }

/* Default is to retain existing removes */

*remove_headers = addr->remove_headers;

if (dblock->remove_headers != NULL)
  {
  char *s = expand_string(dblock->remove_headers);
  if (s == NULL)
    {
    if (!expand_string_forcedfail)
      {
      addr->message = string_sprintf("%s %s failed to expand \"%s\": %s",
        dblock->name, type, dblock->remove_headers, expand_string_message);
      if (search_find_defer) return DEFER;
      addr->special_action = SPECIAL_FREEZE;
      return ERROR;
      }
    }
  else if (*s != 0)
    {
    if (addr->remove_headers == NULL)
      *remove_headers = s;
    else
      *remove_headers = string_sprintf("%s:%s", addr->remove_headers, s);
    }
  }

return OK;
}




/*************************************************
*    Check home directory for a director         *
*************************************************/

/* This is a utility function called by directors to check that a home
directory matches a specific pattern. On catastrophic errors, a message
is attached to the address

Arguments:
  addr        the address being directed
  home_dir    the home directory path
  pattern     the pattern to be matched
  dname       the director name, for error messages

Returns:      OK, DECLINE, DEFER, or ERROR
*/

int
direct_match_directory(address_item *addr, char *home_dir, char *pattern,
  char *dname)
{
BOOL match;
char *expat = pattern;
int match_return = OK;
int nomatch_return = DECLINE;

/* The pattern gets expanded if it contains any expansion items. In this case,
we mustn't use the regex anchor block that is passed in, because we have to
re-compile the regex each time. */

if (strchr(expat, '$') != NULL)
  {
  expat = expand_string(expat);

  if (expat == NULL)
    {
    if (expand_string_forcedfail)
      {
      DEBUG(2) debug_printf("%s director expansion of \"%s\" forced "
        "failure\n", dname, pattern);
      return DECLINE;
      }
    addr->message = string_sprintf("%s director: failed to expand "
      "match_directory string \"%s\": %s", dname, pattern,
      expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  }

/* If the pattern starts with ! it negates the outcome */

while (*expat == ' ') expat++;
if (*expat == '!')
  {
  match_return = DECLINE;
  nomatch_return = OK;
  while (*(++expat) == ' ');
  }

/* Now do the match */

match = match_check_string(home_dir, expat, -1, TRUE, FALSE, NULL);

DEBUG(2)
  {
  debug_printf("%s director: home directory for %s is \"%s\"\n  "
    "match against pattern \"%s\" yielded %d\n", dname, addr->local_part,
    home_dir, pattern, match);
  if (strcmp(expat, pattern) != 0)
    debug_printf("  expanded string was \"%s\"\n", expat);
  }

if (match) return match_return;

if (search_find_defer)
  {
  addr->message = string_sprintf("%s director: lookup defer while matching "
    "directory %s against %s", dname, home_dir, pattern);
  return DEFER;
  }

return nomatch_return;
}





/*************************************************
*  Check files, sender, condition for a director *
*************************************************/

/* This is a utility function for use by directors, and called from within
them. It carries out file existence, sender checking, and general condition
tests, in that order, because the latter may be using the files mentioned in
the former. There are two failure yields, partly for historical reasons.
DECLINE causes the directing not to continue if the director has no_more set;
FAIL_CONDITION does not. Retaining DECLINE is for backwards compatibility, but
it may be that changing them all to FAIL_CONDITION one day might be reasonable.

Argument:
  dblock    points to the director instance
  addr      address item (for defer message)

Returns:    OK               no conditions failed
            DEFER            some condition couldn't be checked
            DECLINE          a condition failed; no_more means no more
            FAIL_CONDITION   a condition failed; ignore no_more
*/

int
direct_check_fsc(director_instance *dblock, address_item *addr)
{
int rc;
char *error;

debug_print_string(dblock->debug_string);

switch (match_exists(dblock->require_files, &error))
  {
  case FAIL:
  DEBUG(9) debug_printf("%s director skipped: file existence failure\n",
    dblock->name);
  return DECLINE;

  case DEFER:
  addr->message = string_sprintf("file existence defer in %s director: %s",
    dblock->name, error);
  return DEFER;
  }

/* Arrange not to replace $key in the senders string. This allows users to miss
off the \ without causing trouble. */

if (dblock->senders != NULL)
  {
  char *senders;
  lookup_key = "$key";
  senders = expand_string_panic(dblock->senders, dblock->name, "director");
  lookup_key = NULL;
  if ((rc = match_sender(senders)) != OK)
    {
    if (rc == DEFER)
      {
      addr->message = "sender check lookup defer";
      return DEFER;
      }
    DEBUG(9) debug_printf("%s director: sender match failure\n",
      dblock->name);
    return DECLINE;
    }
  }

if (dblock->condition != NULL &&
    !expand_check_condition(dblock->condition, dblock->name, "director"))
  {
  if (search_find_defer)
    {
    addr->message = "condition check lookup defer";
    DEBUG(9) debug_printf("%s\n", addr->message);
    return DEFER;
    }
  DEBUG(9) debug_printf("%s director skipped: condition failure\n",
    dblock->name);
  return FAIL_CONDITION;
  }

return OK;
}




/*************************************************
*           Find a local user                    *
*************************************************/

/* Try several times (if configured) to find a local user, in case delays in
NIS or NFS whatever cause an incorrect refusal. It's a pity that getpwnam()
doesn't have some kind of indication as to why it has failed. If the string
given consists entirely of digits, and the third argument is not NULL, assume
the string is the numerical value of the uid. Otherwise it is looked up using
getpwnam(). The uid is passed back via return_uid, if not NULL, and the
pointer to a passwd structure, if found, is passed back via pw, if not NULL.

Because this may be called several times in succession for the same user for
different directors, cache the result of the previous getpwnam call so that it
can be re-used. Note that we can't just copy the structure, as the store it
points to can get trashed.

Arguments:
  s           the login name or textual form of the numerical uid of the user
  pw          if not NULL, return the result of getpwnam here, or set NULL
                if no call to getpwnam is made (s numeric, return_uid != NULL)
  return_uid  if not NULL, return the uid via this address

Returns:      TRUE if s is numerical or was looked up successfully

*/

static struct passwd pwcopy;
static struct passwd *lastpw = NULL;
static char lastname[48] = { 0 };
static char lastdir[128];

BOOL
direct_finduser(char *s, struct passwd **pw, uid_t *return_uid)
{
if (strcmp(lastname, s) != 0)
  {
  int i = 0;

  if (return_uid != NULL && (isdigit((uschar)*s) || *s == '-') &&
       s[strspn(s+1, "0123456789")+1] == 0)
    {
    *return_uid = (uid_t)atoi(s);
    if (pw != NULL) *pw = NULL;
    return TRUE;
    }

  strncpy(lastname, s, sizeof(lastname));
  lastname[sizeof(lastname)-1] = 0;          /* just in case */

  /* Force failure if string length is greater than given maximum */

  if (max_username_length > 0 && (int)strlen(lastname) > max_username_length)
    {
    DEBUG(9) debug_printf("forced failure of finduser(): string "
      "length of %s is greater than %d\n", lastname, max_username_length);
    lastpw = NULL;
    }

  /* Try a few times if so configured; this handles delays in NIS etc. */

  else for (;;)
    {
    if ((lastpw = getpwnam(s)) != NULL) break;
    if (++i > finduser_retries) break;
    sleep(1);
    }

  if (lastpw != NULL)
    {
    pwcopy.pw_uid = lastpw->pw_uid;
    pwcopy.pw_gid = lastpw->pw_gid;
    strncpy(lastdir, lastpw->pw_dir, sizeof(lastdir));
    lastdir[sizeof(lastdir)-1] = 0;          /* just in case */
    pwcopy.pw_name = lastname;
    pwcopy.pw_dir = lastdir;
    lastpw = &pwcopy;
    }
  }

if (lastpw == NULL) return FALSE;

if (return_uid != NULL) *return_uid = lastpw->pw_uid;
if (pw != NULL) *pw = lastpw;

return TRUE;
}




/*************************************************
*           Find a local group                   *
*************************************************/

/* Try several times (if configured) to find a local group, in case delays in
NIS or NFS whatever cause an incorrect refusal. It's a pity that getgrnam()
doesn't have some kind of indication as to why it has failed.

Arguments:
  s           the group namd or textual form of the numerical gid
  return_gid  return the gid via this address

Returns:      TRUE if the group was found; FALSE otherwise

*/

BOOL
direct_findgroup(char *s, gid_t *return_gid)
{
int i = 0;
struct group *gr;

if ((isdigit((uschar)*s) || *s == '-') && s[strspn(s+1, "0123456789")+1] == 0)
  {
  *return_gid = (gid_t)atoi(s);
  return TRUE;
  }

for (;;)
  {
  if ((gr = getgrnam(s)) != NULL)
    {
    *return_gid = gr->gr_gid;
    return TRUE;
    }
  if (++i > finduser_retries) break;
  sleep(1);
  }

return FALSE;
}




/*************************************************
*          Find user by expanding string         *
*************************************************/

/* Expands a string, and then looks up the result in the passwd file.

Arguments:
  string       the string to be expanded, yielding a login name or a numerical
                 uid value (to be passed to direct_finduser())
  driver_name  caller name for panic error message (only)
  driver_type  caller type for panic error message (only)
  pw           return passwd entry via this pointer
  uid          return uid via this pointer
  errmsg       where to point a message on failure

Returns:       TRUE if user found, FALSE otherwise
*/

BOOL
direct_find_expanded_user(char *string, char *driver_name,
  char *driver_type, struct passwd **pw, uid_t *uid, char **errmsg)
{
char *user = expand_string(string);

if (user == NULL)
  {
  *errmsg = string_sprintf("Failed to expand user string \"%s\" from the "
    "%s %s: %s", string, driver_name, driver_type, expand_string_message);
  log_write(0, LOG_MAIN|LOG_PANIC, "%s", *errmsg);
  return FALSE;
  }

if (direct_finduser(user, pw, uid)) return TRUE;

*errmsg = string_sprintf("Failed to find user \"%s\" from expanded string "
  "\"%s\" from the %s %s", user, string, driver_name, driver_type);
log_write(0, LOG_MAIN|LOG_PANIC, "%s", *errmsg);
return FALSE;
}



/*************************************************
*          Find group by expanding string        *
*************************************************/

/* Expands a string and then looks up the result in the group file.

Arguments:
  string       the string to be expanded, yielding a group name or a numerical
                 gid value (to be passed to direct_findgroup())
  driver_name  caller name for panic error message (only)
  driver_type  caller type for panic error message (only)
  gid          return gid via this pointer
  errmsg       return error message via this pointer

Returns:       TRUE if found group, FALSE otherwise
*/

BOOL
direct_find_expanded_group(char *string, char *driver_name, char *driver_type,
  gid_t *gid, char **errmsg)
{
BOOL yield = TRUE;
char *group = expand_string(string);

if (group == NULL)
  {
  *errmsg = string_sprintf("Failed to expand group string \"%s\" from the "
    "%s %s: %s", string, driver_name, driver_type, expand_string_message);
  log_write(0, LOG_MAIN|LOG_PANIC, "%s", *errmsg);
  return FALSE;
  }

if (!direct_findgroup(group, gid))
  {
  *errmsg = string_sprintf("Failed to find group \"%s\" from expanded string "
    "\"%s\" from the %s %s", group, string, driver_name, driver_type);
  log_write(0, LOG_MAIN|LOG_PANIC, "%s", *errmsg);
  yield = FALSE;
  }

return yield;
}



/*************************************************
*             Initialize director list           *
*************************************************/

/* Read the directors configuration file, and set up a chain of director
instances according to its contents. Each director has generic options and may
also have its own private options. This function is only ever called when
directors == NULL. We use generic code in readconf to do the work. */

void
direct_init(void)
{
director_instance *d;
readconf_driver_init("director",
  (driver_instance **)(&directors),     /* chain anchor */
  (driver_info *)directors_available,   /* available drivers */
  sizeof(director_info),                /* size of info blocks */
  &director_defaults,                   /* default values for generic options */
  sizeof(director_instance),            /* size of instance block */
  optionlist_directors,                 /* generic options */
  optionlist_directors_size);

/* Check various options for consistency */

for (d = directors; d != NULL; d = d->next)
  {
  if (d->unseen && !d->more )
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "the combination of \"unseen\" and \"no_more\" is not permitted on "
      "a director,\n  but was set for the %s director", d->name);

  /* Check new_director is valid, and set actual pointers */

  if (d->new_director != NULL)
    {
    director_instance *dd;
    for (dd = directors; dd != NULL; dd = dd->next)
      {
      if (strcmp(d->new_director, dd->name) == 0)
        {
        d->new = dd;
        break;
        }
      }
    if (dd == NULL)
      log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
        "new_director \"%s\" not found for \"%s\" director", d->new_director,
          d->name);
    }

  /* If a fixed uid field is set, then a gid field must also be set. */

  if (d->uid_set && !d->gid_set && d->expand_gid == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "user set without group for the %s director", d->name);

  /* Build a host list if fallback hosts is set */

  host_build_hostlist(&(d->fallback_hostlist), d->fallback_hosts, FALSE);
  }
}




/*************************************************
*           Tidy up after directing              *
*************************************************/

/* Directors are entitled to keep hold of certain resources in their instance
blocks so as to save setting them up each time. An example is the open file for
the aliasfile director. Such directors must provide a tidyup entry point which
is called when all directing is finished via this function. */

void
direct_tidyup(void)
{
director_instance *d;
for (d = directors; d != NULL; d = d->next)
  if (d->info->tidyup != NULL) (d->info->tidyup)(d);
}





/*************************************************
*         Check local part for prefix            *
*************************************************/

/* This functionis handed a local part and a list of possible prefixes;
if any one matches, return the prefix length. A prefix beginning with '*'
is a wildcard.

Arguments:
  local_part    the local part to check
  prefixes      the list of prefixes

Returns:        length of matching prefix or zero
*/

int
direct_check_prefix(char *local_part, char *prefixes)
{
int sep = 0;
char *prefix;
char *listptr = prefixes;
char prebuf[64];

while ((prefix = string_nextinlist(&listptr, &sep, prebuf, sizeof(prebuf)))
       != NULL)
  {
  int plen = (int)strlen(prefix);
  if (prefix[0] == '*')
    {
    char *p;
    prefix++;
    for (p = local_part + (int)strlen(local_part) - (--plen);
         p >= local_part; p--)
      if (strncmpic(prefix, p, plen) == 0) return plen + p - local_part;
    }
  else
    if (strncmpic(prefix, local_part, plen) == 0) return plen;
  }

return 0;
}



/*************************************************
*         Check local part for suffix            *
*************************************************/

/* This functionis handed a local part and a list of possible suffixes;
if any one matches, return the suffix length. A suffix ending with '*'
is a wildcard.

Arguments:
  local_part    the local part to check
  suffixes      the list of suffixes

Returns:        length of matching suffix or zero
*/

int
direct_check_suffix(char *local_part, char *suffixes)
{
int sep = 0;
int alen = (int)strlen(local_part);
char *suffix;
char *listptr = suffixes;
char sufbuf[64];

while ((suffix = string_nextinlist(&listptr, &sep, sufbuf, sizeof(sufbuf)))
     != NULL)
  {
  int slen = (int)strlen(suffix);
  if (suffix[slen-1] == '*')
    {
    char *p, *pend;
    pend = local_part + alen - (--slen) + 1;
    for (p = local_part; p < pend; p++)
      if (strncmpic(suffix, p, slen) == 0) return alen - (p - local_part);
    }
  else
    if (alen > slen && strncmpic(suffix, local_part + alen - slen, slen) == 0)
      return slen;
  }

return 0;
}




/*************************************************
*                 Direct one address             *
*************************************************/

/* This function is passed in one address item, for processing by the
directors. It has been determined that the address is for one of the local
domains. The verify flag is set if this is being called for verification rather
than delivery. If the director doesn't have its "verify" flag set, it is
skipped. Otherwise, the flag is passed to the director in case it needs to
know.

If the director has the "more" flag set false (i.e. "no_more" has been
specified) then, if it fails to match a name, no further directors are tried.
This includes the case when the director would have been called, but for the
verify flag's being FALSE.

If the new_director field of the address is set, then directing must start
at the director it points to. This has been checked to exist at start-up
time.

The generic options "domain", "prefix", and "suffix" are handled at this top
level.

Arguments
  addr         the address to direct
  addr_local   the chain onto which to hang the address for local delivery
  addr_remote  the chain onto which to hang the address for remote delivery
  addr_new     the chain onto which to hang newly-generated addresses
  addr_succeed the chain onto which to hang addresses that we are finished
                 with (e.g. they've been expanded into something else)
  verify       v_none if delivering rather than verifying,
               v_sender if verifying a sender address
               v_recipient if verifying a recipient address
               v_expn if processing an EXPN address

Returns:       OK           => address was successfully directed
               FAIL         => address could not be directed
               DEFER        => temporary problem
               ERROR        => serious configuration or internal problem
*/


int
direct_address(address_item *addr,
  address_item **addr_local,
  address_item **addr_remote,
  address_item **addr_new,
  address_item **addr_succeed,
  int verify)
{
director_instance *d;
address_item *parent;
BOOL more = TRUE;

DEBUG(9) debug_printf(">>>>>>>>>>>>>>>>>>>>>>>>\ndirecting %s\n", addr->orig);

/* Loop through all director instances, possibly starting at a specific
point. */

for (d = (addr->start_director == NULL)? directors : addr->start_director;
     more && d != NULL;
     d = d->next)
  {
  int loopcount = 0;
  int yield;
  int suffixchar = 0;
  int suffixend = -1;
  char *oldlocal_part = NULL;
  BOOL loop_detected = FALSE;

  /* Check the conditions that are common to both routers and directors. The
  relevant items in the instance block are carefully put in the same position,
  which is why we can coerce the director instance pointer to a router instance
  pointer here. We need the expansions set up before doing this. */

  deliver_set_expansions(addr);

  if (route_skip_driver((router_instance *)d, addr->local_part, addr->domain,
       verify, &more, "director"))
    {
    if (!search_find_defer) continue;
    addr->message = "lookup defer for domains or local_parts";
    addr->director = d;
    return DEFER;
    }

  /* Skip this director if processing EXPN and it doesn't have expn set */

  if (verify == v_expn && !d->expn)
    {
    DEBUG(9) debug_printf("%s director skipped: no_expn set\n", d->name);
    continue;
    }

  /* Handle any configured prefix by replacing the local_part address,
  saving it for restoration if the director fails. Skip the director
  if the prefix doesn't match, unless the prefix is optional. */

  if (d->prefix != NULL)
    {
    int plen = direct_check_prefix(addr->local_part, d->prefix);
    if (plen > 0)
      {
      oldlocal_part = addr->local_part;
      addr->local_part += plen;
      addr->prefix = string_copyn(oldlocal_part, plen);
      DEBUG(9) debug_printf("stripped prefix %s\n", addr->prefix);
      }
    else if (!d->prefix_optional)
      {
      DEBUG(9) debug_printf("%s director skipped: prefix mismatch\n",
        d->name);
      continue;
      }
    }

  /* Handle any configured suffix likewise, but for this we have to
  fudge the end of the address; save the character so it can be put
  back if the director fails. */

  if (d->suffix != NULL)
    {
    int slen = direct_check_suffix(addr->local_part, d->suffix);
    if (slen > 0)
      {
      suffixend = (int)strlen(addr->local_part) - slen;
      suffixchar = addr->local_part[suffixend];
      addr->suffix = string_copy(addr->local_part+suffixend);
      addr->local_part[suffixend] = 0;
      DEBUG(9) debug_printf("stripped suffix %s\n", addr->suffix);
      }

    /* If failed to match the suffix, restore any change the prefix
    handler made. */

    else if (!d->suffix_optional)
      {
      DEBUG(9) debug_printf("%s director skipped: suffix mismatch\n",
        d->name);
      if (oldlocal_part != NULL)
        {
        addr->local_part = oldlocal_part;
        addr->prefix = NULL;
        }
      continue;
      }
    }


  /* Loop protection: If this address has a parent with the same address that
  was directed by this director, we skip this director. This prevents a variety
  of possibly looping states, and saves us having to do anything special for
  the forwardfile director. Just in case someone does put it into a loop
  (possible with a smartuser director just adding to an address, for example),
  put a long stop counter on the number of parents. */

  for (parent = addr->parent; parent != NULL; parent = parent->parent)
    {
    if (parent->director == d &&
        strcmpic(parent->local_part, addr->local_part) == 0 &&
        strcmpic(parent->domain, addr->domain) == 0
        &&
        ((parent->prefix == NULL && addr->prefix == NULL) ||
         (parent->prefix != NULL && addr->prefix != NULL &&
          strcmpic(parent->prefix, addr->prefix) == 0))
        &&
        ((parent->suffix == NULL && addr->suffix == NULL) ||
         (parent->suffix != NULL && addr->suffix != NULL &&
          strcmpic(parent->suffix, addr->suffix) == 0)))
      {
      DEBUG(9) debug_printf("%s director skipped: previously directed "
        "%s%s%s@%s\n",
        d->name,
        (addr->prefix == NULL)? "" : addr->prefix,
        addr->local_part,
        (addr->suffix == NULL)? "" : addr->suffix,
        addr->domain);
      loop_detected = TRUE;
      break;
      }

    if (loopcount++ > 100)
      log_write(0, LOG_PANIC_DIE, "directing loop for %s", addr->orig);
    }

  if (loop_detected)
    {
    yield = DECLINE;            /* Behave as if director failed */
    goto DIRECTOR_DECLINED;
    }

  /* Prefix and suffix (if any) have been stripped; ready to call the
  director. Set the expansions again, in case prefix/suffix have
  changed things. Reset them afterwards. */

  deliver_set_expansions(addr);
  DEBUG(9) debug_printf("calling %s director\n", d->name);
  yield = (d->info->code)(d, addr, addr_local, addr_remote, addr_new,
    addr_succeed, verify != v_none && verify != v_expn);
  deliver_set_expansions(NULL);

  /* If yield = FAIL_CONDITION it means that a condition that has to be tested
  from within the director (because of setting up variables like $home or
  whatever) has failed, and the condition is one that should logically be
  tested before the "more" flag. We therefore force "more" back to be TRUE, and
  convert the yield to DECLINE. */

  if (yield == FAIL_CONDITION) { more = TRUE; yield = DECLINE; }

  /* Success or deferral means we are finished with this address, as does an
  internal or configuration failure. If we succeed on a director that has
  "fail_verify" set, convert the result into a fail. If we succeed on a
  director that has "unseen" set on it, we must make a copy of the address to
  hand on to the subsequent directors. Actually, we can't pass it on directly;
  we have to put it on the new queue, but since it has the same address as
  the current one, it will be passed by all previous directors and also the
  one that has just succeeded, by the loop-avoidance code. This is all handled
  by a function common to both directors and routers. */

  if (yield == OK || yield == DEFER || yield == ERROR)
    {
    addr->director = d;

    if (yield == OK)
      {
      if ((verify == v_sender && d->fail_verify_sender) ||
          (verify == v_recipient && d->fail_verify_recipient))
        {
        yield = FAIL;
        addr->message = string_sprintf("%s director forced verify failure",
          d->name);
        if (*addr_remote == addr) *addr_remote = addr->next;
        if (*addr_local == addr) *addr_local = addr->next;
        }
      else if (d->unseen)
        route_unseen(d->name, addr, addr_local, addr_remote, addr_new);
      }

    DEBUG(2)
      {
      if (yield == OK)
        {
        debug_printf("%s director succeeded for %s%s\n", d->name,
          addr->local_part, (d->unseen)? " (unseen)" : "");
        }
      else if (yield == DEFER)
        {
        debug_printf("%s director deferred %s\n", d->name, addr->local_part);
        debug_printf("  message: %s\n", (addr->message == NULL)?
          "<none>" : addr->message);
        }
      else
        {
        debug_printf("%s director: error for %s\n", d->name, addr->local_part);
        debug_printf("  message: %s\n", (addr->message == NULL)?
          "<none>" : addr->message);
        }
      }

    return yield;
    }

  /* The only remaining yield values are DECLINE or FORCEFAIL. */

  if (yield != DECLINE && yield != FORCEFAIL)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s director returned unknown "
      "value %d", d->name, yield);

  /* The director has declined to handle this address. */

  DIRECTOR_DECLINED:

  /* Restore prefix & suffix for the next director. */

  if (suffixend >= 0) addr->local_part[suffixend] = suffixchar;
  if (oldlocal_part != NULL) addr->local_part = oldlocal_part;
  addr->prefix = addr->suffix = NULL;

  /* Break the loop if a director forced a failure. */

  if (yield == FORCEFAIL) break;
  }

/* No directors accepted this address; fail it, setting a default message
if there is nothing already set. */

if (addr->message == NULL)
  addr->message = string_sprintf("unknown local-part \"%s\" in domain \"%s\"",
    addr->local_part, addr->domain);

deliver_set_expansions(NULL);
return FAIL;
}

/* End of direct.c */
