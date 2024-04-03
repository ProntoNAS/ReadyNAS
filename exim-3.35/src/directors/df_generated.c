/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "../exim.h"
#include "dfunctions.h"



/*************************************************
*    Process a set of generated new addresses    *
*************************************************/

/* This function sets up a set of newly generated child addresses and puts them
on the new address chain. Copy in the uid, gid and permission flags for use by
pipes and files, set the parent, and "or" its af_ignore_error flag. Also record
the setting for any starting director.

If the generated address is the same as one of its ancestors, and the
check_ancestor flag is set, do not use this generated address, but replace it
with a copy of the input address. This is to cope with cases where A is aliased
to B and B has a .forward file pointing to A, though it is usually set on the
forwardfile rather than the aliasfile. We can't just pass on the old
address by returning FAIL, because it must act as a general parent for
generated addresses, and only get marked "done" when all its children are
delivered.

Arguments:
  dblock                  director block
  addr_new                new address chain
  addr                    original address
  generated               list of generated addresses
  errors_to               return path from director configuration
  extra_headers           headers to add
  remove_headers          headers to remove
  ugidptr                 points to uid/gid data for files, pipes, autoreplies
  pw                      password entry, set if ob->check_local_user is TRUE
  default_home_directory  used only if dblock->home_directory is NULL

Returns:         nothing
*/

void
df_generated(director_instance *dblock, address_item **addr_new,
  address_item *addr, address_item *generated, char *errors_to,
  header_line *extra_headers, char *remove_headers, ugid_block *ugidptr,
  struct passwd *pw, char *default_home_directory)
{
common_director_options_block *ob =
  (common_director_options_block *)(dblock->options_block);

while (generated != NULL)
  {
  address_item *parent;
  address_item *next = generated;

  generated = next->next;
  next->parent = addr;
  orflag(next, addr, af_ignore_error);
  next->start_director = dblock->new;
  addr->child_count++;

  next->next = *addr_new;
  *addr_new = next;

  if (ob->one_time && !queue_2stage)
    {
    for (parent = addr; parent->parent != NULL; parent = parent->parent);
    next->onetime_parent = parent->orig;
    }

  if (ob->hide_child_in_errmsg) setflag(next, af_hide_child);

  if (ob->check_ancestor)
    {
    for (parent = addr; parent != NULL; parent = parent->parent)
      {
      if (strcmp(next->orig, parent->orig) == 0)
        {
        DEBUG(2) debug_printf("generated parent replaced by child\n");
        next->orig = string_copy(addr->orig);
        break;
        }
      }
    }

  /* A user filter may, under some circumstances, set up an errors address.
  If so, leave it alone. */

  if (next->errors_address == NULL) next->errors_address = errors_to;

  /* Add in header additions/deletions. */

  next->extra_headers = extra_headers;
  next->remove_headers = remove_headers;

  /* For pipes, files, and autoreplies, record this director as handling them,
  because they don't go through the directing process again. Then set up uid,
  gid, home and current directories. */

  if (testflag(next, af_pfr))
    {
    next->director = dblock;

    /* If there are no explicit uid/gid settings on the driver,
    use those for the local user if we've looked one up. */

    direct_set_ugid(next, ugidptr);

    if (ob->check_local_user)
      {
      if (!ugidptr->uid_set)
        {
        next->uid = pw->pw_uid;
        setflag(next, af_uid_set);
        }
      if (!ugidptr->gid_set)
        {
        next->gid = pw->pw_gid;
        setflag(next, af_gid_set);
        }
      }

    /* There's a magic setting for the home directory that allows for it to
    be set to the user's home directory (when a user has been looked up),
    overriding any other home directory value that might be assumed from
    some other option setting. */

    if (dblock->home_directory != NULL)
      {
      next->home_dir =
        (ob->check_local_user &&
        strcmpic(dblock->home_directory, "check_local_user") == 0)?
          string_copy(pw->pw_dir) : dblock->home_directory;
      }
    else
      if (default_home_directory != NULL)
        next->home_dir = string_copy(default_home_directory);

    /* Same magic for current directory */

    next->current_dir =
        (dblock->current_directory != NULL &&
        ob->check_local_user &&
        strcmpic(dblock->current_directory, "check_local_user") == 0)?
      string_copy(pw->pw_dir) : dblock->current_directory;

    /* Permission options */

    if (!ob->forbid_pipe) setflag(next, af_allow_pipe);
    if (!ob->forbid_file) setflag(next, af_allow_file);
    if (!ob->forbid_reply) setflag(next, af_allow_reply);

    /* Aliasfile can produce only pipes or files, but forwardfile can in
    addition produce autoreplies. If the transport setting fails, the error
    gets picked up at the outer level from the setting of basic_errno in the
    address. */

    if (next->orig[0] == '|')
      {
      (void)route_get_transport(ob->pipe_transport, ob->expand_pipe_transport,
        next, &(next->transport), dblock->name, "pipe_transport");
      }
    else if (next->orig[0] == '>')
      {
      (void)route_get_transport(ob->reply_transport, ob->expand_reply_transport,
        next, &(next->transport), dblock->name, "reply_transport");
      }
    else
      {
      int len = (int)strlen(next->orig);
      if (next->orig[len-1] == '/')
        {
        if (len > 1 && next->orig[len-2] == '/' &&
            ob->directory2_transport != NULL)
          (void)route_get_transport(ob->directory2_transport,
            ob->expand_directory2_transport,
            next, &(next->transport), dblock->name, "directory2_transport");
        else
          (void)route_get_transport(ob->directory_transport,
            ob->expand_directory_transport,
            next, &(next->transport), dblock->name, "directory_transport");
        }
      else
        {
        (void)route_get_transport(ob->file_transport, ob->expand_file_transport,
          next, &(next->transport), dblock->name, "file_transport");
        }
      }
    }

  DEBUG(2)
    {
    debug_printf("%s director generated %s\n  %serrors_to=%s transport=%s\n",
      dblock->name,
      next->orig,
      testflag(next, af_pfr)? "pipe, file, or autoreply\n  " : "",
      next->errors_address,
      (next->transport == NULL)? "NULL" : next->transport->name);

    if (testflag(next, af_uid_set))
      debug_printf("  uid=%ld ", (long int)(next->uid));
    else
      debug_printf("  uid=unset ");

    if (testflag(next, af_gid_set))
      debug_printf("gid=%ld ", (long int)(next->gid));
    else
      debug_printf("gid=unset ");

    debug_printf("home=%s\n", next->home_dir);
    }
  }
}

/* End of df_generated.c */
