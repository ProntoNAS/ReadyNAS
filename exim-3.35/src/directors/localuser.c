/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "localuser.h"



/* Options specific to the localuser director. */

optionlist localuser_director_options[] = {
  { "match_directory",   opt_stringptr,
      (void *)(offsetof(localuser_director_options_block, pw_dir)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int localuser_director_options_count =
  sizeof(localuser_director_options)/sizeof(optionlist);

/* Default private options block for the localuser director. */

localuser_director_options_block localuser_director_option_defaults = {
  NULL        /* pw_dir */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void localuser_director_init(director_instance *dblock)
{
/*
localuser_director_options_block *ob =
  (localuser_director_options_block *)(dblock->options_block);
*/

/* There must be a transport unless verifying. */

if (dblock->transport == NULL &&
    dblock->expand_transport == NULL &&
    !dblock->verify_only)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
  "a transport is required by the localuser director driver", dblock->name);
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface description. */

int localuser_director_entry(
  director_instance *dblock,      /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if it's local */
  address_item **addr_remote,     /* add it to this if it's remote */
  address_item **addr_new,        /* put new addresses on here */
  address_item **addr_succeed,    /* put finished with addresses here */
  BOOL verify)                    /* TRUE when verifying */
{
localuser_director_options_block *ob =
  (localuser_director_options_block *)(dblock->options_block);
struct passwd *pw;
BOOL userexists;
int rc;
char *errors_to;
char *remove_headers;
header_line *extra_headers;

addr_new = addr_new;          /* Keep picky compilers happy */
addr_succeed = addr_succeed;

/* Check that the local part is a valid local user account. We have to do this
first in order to get $home available for other checks. However, we can't
return immediately if it isn't, because the condition check logically happens
before the user check, and gives a different result if it fails. The third
argument to direct_finduser() must be NULL here, to prevent a digit string
being taken as a numeric uid. */

userexists = direct_finduser(addr->local_part, &pw, NULL);

/* Set $home if there is a user, and run the condition test. */

if (userexists) deliver_home = pw->pw_dir;
rc = direct_check_fsc(dblock, addr);
if (rc != OK) return rc;

/* Condition succeeded. If the local user check failed, we can now return
DECLINE. */

if (!userexists)
  {
  DEBUG(2) debug_printf("%s director declined for %s: no such user\n",
    dblock->name, addr->local_part);
  return DECLINE;
  }

/* If there is a setting of match_directory, check for a match. */

if (ob->pw_dir != NULL)
  {
  rc = direct_match_directory(addr, pw->pw_dir, ob->pw_dir, dblock->name);
  if (rc != OK) return rc;
  }

/* Set up the errors address, if any. */

rc = direct_get_errors_address(addr, dblock, "director", verify, &errors_to);
if (rc != OK) return rc;

/* Set up the additional and removeable headers for the address. */

rc = direct_get_munge_headers(addr, dblock, "director", &extra_headers,
  &remove_headers);
if (rc != OK) return rc;

/* This local user is OK; set the transport and accept the address; update its
errors address and header munging. Initialization ensures that there is a
transport except when verifying. */

if (!route_get_transport(dblock->transport, dblock->expand_transport,
  addr, &(addr->transport), dblock->name, NULL)) return ERROR;

addr->errors_address = errors_to;
addr->extra_headers = extra_headers;
addr->remove_headers = remove_headers;

/* Default uid/gid are from the passwd file, but can be overridden by
explicit settings, which will be filled in by the route_queue() function. */

addr->uid = pw->pw_uid;
addr->gid = pw->pw_gid;
setflag(addr, af_uid_set|af_gid_set);

if (dblock->home_directory != NULL) addr->home_dir = dblock->home_directory;
  else addr->home_dir = string_copy(pw->pw_dir);

addr->current_dir = dblock->current_directory;

return route_queue(addr, addr_local, addr_remote, (router_instance *)dblock,
  NULL, "director")? OK : ERROR;
}

/* End of director/localuser.c */
