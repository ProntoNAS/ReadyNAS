/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "queryprogram.h"



/* Options specific to the queryprogram router. */

optionlist queryprogram_router_options[] = {
  { "*set_command_group",   opt_bool | opt_hidden,
      (void *)(offsetof(queryprogram_router_options_block, gid_set)) },
  { "*set_command_user",    opt_bool | opt_hidden,
      (void *)(offsetof(queryprogram_router_options_block, uid_set)) },
  { "command",      opt_stringptr,
      (void *)(offsetof(queryprogram_router_options_block, command)) },
  { "command_group",opt_gid,
      (void *)(offsetof(queryprogram_router_options_block, gid)) },
  { "command_user", opt_uid,
      (void *)(offsetof(queryprogram_router_options_block, uid)) },
  { "current_directory", opt_stringptr,
      (void *)(offsetof(queryprogram_router_options_block, current_directory)) },
  { "timeout",      opt_time,
      (void *)(offsetof(queryprogram_router_options_block, timeout)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int queryprogram_router_options_count =
  sizeof(queryprogram_router_options)/sizeof(optionlist);

/* Default private options block for the queryprogram router. */

queryprogram_router_options_block queryprogram_router_option_defaults = {
  NULL,         /* command */
  60*60,        /* timeout */
  (uid_t)(-1),  /* uid */
  (gid_t)(-1),  /* gid */
  FALSE,        /* uid_set */
  FALSE,        /* gid_set */
  NULL          /* current_directory */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
queryprogram_router_init(router_instance *rblock)
{
queryprogram_router_options_block *ob =
  (queryprogram_router_options_block *)(rblock->options_block);

/* A command must be given, starting with a slash. */

if (ob->command == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "a command specification is required", rblock->name);

if (ob->command[0] != '/')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s router:\n  "
    "command is not an absolute path", rblock->name);
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface details */

int
queryprogram_router_entry(
  router_instance *rblock,        /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if successful & local */
  address_item **addr_remote,     /* add it to this if successful & remote */
  BOOL verify,                    /* TRUE if verifying */
  char *ignore_target_hosts)      /* ignore host list (expanded) */
{
int fd_in, fd_out, i, len, rc;
pid_t pid;
uid_t uid;
gid_t gid;
char *s, *expanded;
char *argv[20];
char *returned[5];
char buffer[256];
transport_instance *transport = NULL;
queryprogram_router_options_block *ob =
  (queryprogram_router_options_block *)(rblock->options_block);
char *current_directory = (ob->current_directory == NULL)? "/" :
  ob->current_directory;

DEBUG(2) debug_printf("%s router called for %s: route_domain = %s\n",
  rblock->name, addr->orig, addr->route_domain);

/* If one of uid/gid is not set, we need to have "nobody"s values available. */

if (!ob->uid_set || !ob->gid_set)
  {
  if (!nobody_uid_set)
    {
    struct passwd *pw;
    if (direct_finduser("nobody", &pw, NULL))
      {
      nobody_uid = pw->pw_uid;
      nobody_gid = pw->pw_gid;
      nobody_uid_set = nobody_gid_set = TRUE;
      }
    }

  if (!nobody_uid_set)
    {
    log_write(0, LOG_MAIN, "%s router: "
      "nobody_user is not defined and \"nobody\" is not a login id",
      rblock->name);
    addr->message = string_sprintf("%s router: \"nobody\" unavailable",
      rblock->name);
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  }

/* Now set the uid/gid */

uid = (ob->uid_set)? ob->uid : nobody_uid;
gid = (ob->gid_set)? ob->gid : nobody_gid;

DEBUG(2) debug_printf("uid=%ld gid=%ld current_directory=%s\n",
  (long int)uid, (long int)gid, current_directory);

/* Expand the command string, making the domain available as a variable,
and ensuring a copy of the text is made. Then set up the arguments for the
command. */

deliver_domain = addr->route_domain;
s = expanded = expand_string_copy(ob->command);
deliver_domain = NULL;

if (expanded == NULL)
  {
  addr->message = string_sprintf("%s router: failed to expand %s: %s",
    rblock->name, ob->command, expand_string_message);
  if (search_find_defer) return DEFER;
  addr->special_action = SPECIAL_FREEZE;
  return ERROR;
  }

DEBUG(2) debug_printf("command is: %s\n", expanded);

/* Split on white space to make the arguments. */

for (i = 0; *s != 0 && i < 19; i++)
  {
  while (isspace((uschar)*s)) s++;
  argv[i] = s;
  while (*s != 0 && !isspace((uschar)*s)) s++;
  if (*s != 0) *s++ = 0;
  }
argv[i] = NULL;


/* Create the child process, making it a group leader. */

pid = child_open(argv, NULL, 0077, &uid, &gid, &fd_in, &fd_out,
  current_directory, TRUE);

if (pid < 0)
  {
  addr->message = string_sprintf("%s router couldn't create child process: %s",
    rblock->name, strerror(errno));
  log_write(0, LOG_MAIN, "%s", addr->message);
  return DEFER;
  }

/* Nothing is written to the standard input. */

close(fd_in);

/* Wait for the process to finish, applying the timeout, and inspect its return
code. */

if ((rc = child_close(pid, ob->timeout)) != 0)
  {
  if (rc > 0)
    addr->message = string_sprintf("%s router: command returned non-zero "
      "code %d", rblock->name, rc);

  else if (rc == -256)
    {
    uid_t euid = geteuid();

    addr->message = string_sprintf("%s router: command timed out",
      rblock->name);

    /* Kill the whole process group; we have to get back the privilege in
    order to do this. */

    if (euid != root_uid) mac_seteuid(root_uid);
    killpg(pid, SIGKILL);
    if (euid != root_uid) mac_seteuid(euid);
    }

  else if (rc == -257)
    addr->message = string_sprintf("%s router: wait() failed: %s",
      rblock->name, strerror(errno));

  else
    addr->message = string_sprintf("%s router: command killed by signal %d",
      rblock->name, -rc);

  addr->special_action = SPECIAL_FREEZE;
  return ERROR;
  }


/* Read the pipe to get the command's output, and then close it. */

len = read(fd_out, buffer, sizeof(buffer));
close(fd_out);


/* Failure to return any data is an error. */

if (len <= 0)
  {
  addr->message = string_sprintf("%s router: command failed to return data",
    rblock->name);
  addr->special_action = SPECIAL_FREEZE;
  return ERROR;
  }


/* The returned data may contain up to five fields, separated by white space.
The final field goes on to the end of the string and may contain white space.
Ignore leading and trailing white space. */

while (len > 0 && isspace((uschar)buffer[len-1])) len--;
buffer[len] = 0;

DEBUG(2) debug_printf("command wrote: %s\n", buffer);

s = buffer;
for (i = 0; i < 4; i++)
  {
  while (isspace((uschar)*s)) s++;
  returned[i] = s;
  while (*s != 0 && !isspace((uschar)*s)) s++;
  if (*s != 0) *s++ = 0;
  }
returned[4] = s;

/* The first returned field must be a known yield name. If it is not
"OK", then the rest of the line is an error message. */

if (strcmpic(returned[0], "OK") != 0)
  {
  addr->message = string_sprintf("%s router: %s %s %s %s", rblock->name,
    returned[1], returned[2], returned[3], returned[4]);

  if (strcmpic(returned[0], "decline") == 0) return DECLINE;
  if (strcmpic(returned[0], "fail") == 0) return DECLINE;     /* Obsolete */
  if (strcmpic(returned[0], "forcefail") == 0) return FORCEFAIL;
  if (strcmpic(returned[0], "defer") == 0) return DEFER;

  addr->special_action = SPECIAL_FREEZE;

  if (strcmpic(returned[0], "error") == 0) return ERROR;

  addr->message = string_sprintf("%s router: bad command yield: %s %s %s %s %s",
    rblock->name, returned[0], returned[1], returned[2], returned[3],
      returned[4]);
  log_write(0, LOG_MAIN, "%s", addr->message);
  return ERROR;
  }


/* The command said OK. The second field is a transport, or "+" or null,
meaning take the transport from the router, or if there is none, pass
on the address. Otherwise, look up the transport. */

if (returned[1][0] != 0 && strcmp(returned[1], "+") != 0)
  {
  for (transport = transports; transport != NULL; transport = transport->next)
    {
    if (strcmp(transport->name, returned[1]) == 0) break;
    }

  if (transport == NULL)
    {
    log_write(0, LOG_MAIN, "%s router: unknown transport name %s yielded by "
      "command", rblock->name, returned[1]);
    addr->message = string_sprintf("%s router: unknown transport name %s "
      "yielded by command", rblock->name, returned[1]);
    return ERROR;
    }
  }

/* Second field was empty or "+"; get the transport for this router, if
any. It might be expanded, or it might be unset (which is not an error). */

else if (!route_get_transport(rblock->transport, rblock->expand_transport,
      addr, &transport, rblock->name, NULL))
  return ERROR;

/* If the third field is non-null and not "+", it is a new route_address to
replace the current one. If the fourth field is non-null and not "+", it gives
a method for looking up the name. This is ignored if no transport is available.
*/

if (returned[2][0] != 0 && strcmp(returned[2], "+") != 0)
  {
  char *new_domain = string_copy(returned[2]);

  /* Look up an address if requested. */

  if (transport != NULL && returned[3][0] != 0 && strcmp(returned[3], "+") != 0)
    {
    int lookup_type, rc;
    char *canonical_name;
    host_item *h;

    if (strcmp(returned[3], "byname") == 0) lookup_type = lk_byname;
    else if (strcmp(returned[3], "bydns") == 0) lookup_type = lk_bydns;
    else if (strcmp(returned[3], "bydns_a") == 0) lookup_type = lk_bydns_a;
    else if (strcmp(returned[3], "bydns_mx") == 0) lookup_type = lk_bydns_mx;
    else
      {
      log_write(0, LOG_MAIN, "%s router: bad lookup type %s yielded by command",
        rblock->name, returned[3]);
      addr->message = string_sprintf("%s router: bad lookup type %s yielded "
        "by command", rblock->name, returned[3]);
      addr->special_action = SPECIAL_FREEZE;
      return ERROR;
      }

    addr->host_list = h = store_get(sizeof(host_item));
    h->name = new_domain;
    h->address = NULL;
    h->mx = -1;
    h->status = hstatus_unknown;
    h->why = hwhy_unknown;
    h->last_try = 0;
    h->next = NULL;

    if (lookup_type == lk_byname)
      rc = host_find_byname(h, ignore_target_hosts, &canonical_name, FALSE);
    else
      {
      BOOL removed;
      rc = host_find_bydns(h, ignore_target_hosts, (lookup_type == lk_bydns_mx),
        (lookup_type == lk_bydns_a), FALSE, FALSE, &canonical_name, &removed);
      if (removed) setflag(addr, af_local_host_removed);
      }

    if (rc == HOST_FIND_AGAIN)
      {
      if (rblock->pass_on_timeout)
        {
        DEBUG(2) debug_printf("%s router timed out & pass_on_timeout set\n",
          rblock->name);
        return PASS;
        }
      return DEFER;
      }

    if (rc == HOST_FIND_FAILED)
      {
      log_write(0, LOG_MAIN, "Error in %s router: lookup of host %s failed",
        rblock->name, h->name);
      addr->message = "error in router: host lookup failed";
      addr->special_action = SPECIAL_FREEZE;
      return ERROR;
      }

    if (rc == HOST_FOUND_LOCAL)
      {
      rc = host_self_action(addr, h, rblock->self_code, rblock->self_rewrite,
        rblock->self);
      if (rc != OK) return rc;
      }
    }

  /* Replace routing domain. */

  addr->route_domain = new_domain;
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


/* If no transport is set, simply pass on the address (possibly with a modified
route_domain) to the next router. */

if (transport == NULL) return PASS;

/* Else fill in the transport and route option string, queue the address for
local or remote delivery, and yield success. This is all handled by the
route_queue() function. */

addr->transport = transport;
if (returned[4][0] != 0) addr->route_option = string_copy(returned[4]);
return route_queue(addr, addr_local, addr_remote, rblock, NULL, "router")?
  OK : ERROR;
}

/* End of routers/queryprogram.c */
