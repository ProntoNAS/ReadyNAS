/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "dfunctions.h"
#include "forwardfile.h"



/* Options specific to the forwardfile director. */

optionlist forwardfile_director_options[] = {
  { "*expand_directory2_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(forwardfile_director_options_block, expand_directory2_transport) },
  { "*expand_directory_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(forwardfile_director_options_block, expand_directory_transport) },
  { "*expand_file_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(forwardfile_director_options_block, expand_file_transport) },
  { "*expand_pipe_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(forwardfile_director_options_block, expand_pipe_transport) },
  { "*expand_reply_transport",  opt_stringptr|opt_hidden,
      (void *)offsetof(forwardfile_director_options_block, expand_reply_transport) },
  { "allow_system_actions", opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, allow_system_actions)) },
  { "check_ancestor",     opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, check_ancestor)) },
  { "check_group",        opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, check_group)) },
  { "check_local_user",   opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, check_local_user)) },
  { "data",               opt_stringptr,
      (void *)offsetof(forwardfile_director_options_block, data) },
  { "directory2_transport",opt_transportptr,
      (void *)(offsetof(forwardfile_director_options_block, directory2_transport)) },
  { "directory_transport",opt_transportptr,
      (void *)(offsetof(forwardfile_director_options_block, directory_transport)) },
  { "file",               opt_stringptr,
      (void *)(offsetof(forwardfile_director_options_block, file)) },
  { "file_directory",     opt_stringptr,
      (void *)(offsetof(forwardfile_director_options_block, file_dir)) },
  { "file_transport",     opt_transportptr,
      (void *)(offsetof(forwardfile_director_options_block, file_transport)) },
  { "filter",             opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, filter)) },
  { "forbid_file",        opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_file)) },
  { "forbid_filter_existstest",  opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_filter_exists)) },
  { "forbid_filter_logwrite",opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_filter_logwrite)) },
  { "forbid_filter_lookup",      opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_filter_lookup)) },
  #ifdef EXIM_PERL
  { "forbid_filter_perl",        opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_filter_perl)) },
  #endif
  { "forbid_filter_reply",opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_reply)) },
  { "forbid_include",     opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_include)) },
  { "forbid_pipe",        opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, forbid_pipe)) },
  { "forbid_reply",        opt_bool|opt_hidden,
      (void *)(offsetof(forwardfile_director_options_block, forbid_reply)) },
  { "freeze_missing_include", opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, freeze_missing_include)) },
  { "hide_child_in_errmsg", opt_bool,
      (void *)offsetof(forwardfile_director_options_block, hide_child_in_errmsg) },
  { "ignore_eacces",      opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, ignore_eacces)) },
  { "ignore_enotdir",     opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, ignore_enotdir)) },
  { "match_directory",    opt_stringptr,
      (void *)(offsetof(forwardfile_director_options_block, pw_dir)) },
  { "modemask",           opt_octint,
      (void *)(offsetof(forwardfile_director_options_block, modemask)) },
  { "one_time",           opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, one_time)) },
  { "owners",             opt_uidlist,
      (void *)(offsetof(forwardfile_director_options_block, owners)) },
  { "owngroups",          opt_gidlist,
      (void *)(offsetof(forwardfile_director_options_block, owngroups)) },
  { "pipe_transport",     opt_transportptr,
      (void *)(offsetof(forwardfile_director_options_block, pipe_transport)) },
  { "qualify_preserve_domain", opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, qualify_preserve_domain)) },
  { "reply_transport",    opt_transportptr,
      (void *)(offsetof(forwardfile_director_options_block, reply_transport)) },
  { "rewrite",            opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, rewrite)) },
  { "seteuid",            opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, use_seteuid)) },
  { "skip_syntax_errors", opt_bool,
      (void *)(offsetof(forwardfile_director_options_block, skip_syntax_errors)) },
  { "syntax_errors_text", opt_stringptr,
      (void *)(offsetof(forwardfile_director_options_block, syntax_errors_text)) },
  { "syntax_errors_to",   opt_stringptr,
      (void *)(offsetof(forwardfile_director_options_block, syntax_errors_to)) }
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int forwardfile_director_options_count =
  sizeof(forwardfile_director_options)/sizeof(optionlist);

/* Default private options block for the forwardfile director. */

forwardfile_director_options_block forwardfile_director_option_defaults = {
  /************** Common block ****************/
  NULL,     /* directory_transport */
  NULL,     /* expand_directory_transport */
  NULL,     /* directory2_transport */
  NULL,     /* expand_directory2_transport */
  NULL,     /* file_transport */
  NULL,     /* expand_file_transport */
  NULL,     /* pipe_transport */
  NULL,     /* expand_pipe_transport */
  NULL,     /* reply_transport */
  NULL,     /* expand_reply_transport */
  NULL,     /* file */
  NULL,     /* queries */
  NULL,     /* query */
  022,      /* modemask */
  NULL,     /* owners */
  NULL,     /* owngroups */
  -1,       /* partial_match */
  -1,       /* search_type */
  FALSE,    /* check_ancestor */
  TRUE,     /* check_local_user */
  FALSE,    /* expand */
  FALSE,    /* forbid_file */
  FALSE,    /* forbid_pipe */
  FALSE,    /* forbid_reply */
  FALSE,    /* include_domain */
  FALSE,    /* hide_child_in_errmsg */
  FALSE,    /* one_time */
  FALSE,    /* optional */
  /********************************************/
  NULL,     /* data */
  NULL,     /* file_dir */
  NULL,     /* pw_dir */
  NULL,     /* syntax_errors_to */
  NULL,     /* syntax_errors_text */
  FALSE,    /* check_group */
  FALSE,    /* filter */
  FALSE,    /* allow_system_actions */
  FALSE,    /* forbid_filter_exists */
  FALSE,    /* forbid_filter_logwrite */
  FALSE,    /* forbid_filter_lookup */
  FALSE,    /* forbid_filter_perl */
  FALSE,    /* forbid_include */
  FALSE,    /* ignore_eacces */
  FALSE,    /* ignore_enotdir */
  FALSE,    /* qualify_preserve_domain */
  FALSE,    /* use_seteuid */
  TRUE,     /* freeze_missing_include */
  TRUE,     /* rewrite */
  FALSE     /* skip_syntax_errors */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void forwardfile_director_init(director_instance *dblock)
{
forwardfile_director_options_block *ob =
  (forwardfile_director_options_block *)(dblock->options_block);

/* The one_continue flag is turned off for forwardfile */

dblock->one_continue = FALSE;

/* Either file or data must be set, but not both */

if (ob->file == NULL && ob->data == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "one of \"file\" or \"data\" must be specified", dblock->name);

if (ob->file != NULL && ob->data != NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "only one of \"file\" or \"data\" must be specified", dblock->name);

/* A directory setting is optional, but if it exists it must be absolute,
though we can't check for certain until it has been expanded. */

if (ob->file_dir != NULL && ob->file_dir[0] != '/' &&
    ob->file_dir[0] != '$')
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "absolute path name required for the 'file_directory' option",
    dblock->name);

/* Permit relative paths only if local user checking is set, or if the
directory option (which must be absolute) is set. Permit match_directory
only if local user checking is set. */

if (!ob->check_local_user)
  {
  if (ob->file != NULL && ob->file[0] != '/' && ob->file[0] != '$' &&
      ob->file_dir == NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "absolute file path required when check_local_user and directory are not set",
      dblock->name);
  if (ob->pw_dir != NULL)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
      "check_local_user is required when match_directory is set",
      dblock->name);
  }

/* Permit seteuid only if it is configured to be available, and either local
user checking is set, or uid/gid are supplied. */

if (ob->use_seteuid && !have_seteuid)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "seteuid requested but not configured as available",
    dblock->name);

if (ob->use_seteuid && !ob->check_local_user && !dblock->uid_set &&
    dblock->expand_uid == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "seteuid requested without check_local_user or explicit uid", dblock->name);

/* A transport must *not* be specified */

if (dblock->transport != NULL || dblock->expand_transport != NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "a transport is not allowed for this director", dblock->name);

/* Onetime aliases can only be real addresses. If filter is not set,
force forbid_reply, since a non-filter file can't generate replies. */

if (!ob->filter) ob->forbid_reply = TRUE;

if (ob->one_time && (!ob->forbid_pipe || !ob->forbid_file ||
      !ob->forbid_reply))
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG2, "%s director:\n  "
    "forbid_pipe, forbid_file, and forbid_filter_reply must be set when "
    "one_time is set",
    dblock->name);
}



/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface description. */

int forwardfile_director_entry(
  director_instance *dblock,      /* data for this instantiation */
  address_item *addr,             /* address we are working on */
  address_item **addr_local,      /* add it to this if it's local */
  address_item **addr_remote,     /* add it to this if it's remote */
  address_item **addr_new,        /* put new addresses on here */
  address_item **addr_succeed,    /* put old address here on success */
  BOOL verify)                    /* TRUE when verifying */
{
forwardfile_director_options_block *ob =
  (forwardfile_director_options_block *)(dblock->options_block);
address_item *generated = NULL;
char *save_qualify_domain_recipient = qualify_domain_recipient;
char *directory = NULL;
char *errors_to;
char *filename;
char *filebuf = NULL;
char *error;
char *s, *tag;
char *remove_headers;
void *reset_point = NULL;
header_line *extra_headers;
struct stat statbuf;
struct passwd *pw = NULL;
error_block *eblock = NULL;
ugid_block ugid;
FILE *fwd = NULL;
int   yield = OK;
uid_t saved_euid = 0;
gid_t saved_egid = 0;
int   rc;
int   filter_action = SPECIAL_NONE;
BOOL  usercheckfailed = FALSE;
BOOL  uid_ok = FALSE;
BOOL  gid_ok = !ob->check_group;
BOOL  restore_uid = FALSE;
BOOL  stat_directory = TRUE;
BOOL  delivered = FALSE;
BOOL  is_filter = FALSE;
BOOL  filter_header = FALSE;

addr_local = addr_local;     /* Keep picky compilers happy */
addr_remote = addr_remote;

/* If the check_local_user option is set, check that the local_part is
the login of a local user. Note: the third argument to direct_finduser() must
be NULL here, to prevent a numeric string being taken as a numeric uid. If the
user is found, set directory to the home directory, and the home expansion
variable as well, so that it can be used while expanding match_directory and
ob->file_dir. The resetting of deliver_home happens at the outer level. If the
user is not found, we don't decline just yet, because the condition test might
fail, and it may return FAIL_CONDITION, which should take precedence. */

if (ob->check_local_user)
  {
  usercheckfailed = !direct_finduser(addr->local_part, &pw, NULL);
  if (!usercheckfailed) deliver_home = directory = pw->pw_dir;
  }

/* Perform file existence and sender verification checks now that we
potentially have $home available. */

yield = direct_check_fsc(dblock, addr);
if (yield != OK) return yield;

/* The condition check succeeded; we can now return DECLINE if the
user check failed. */

if (usercheckfailed)
  {
  DEBUG(2) debug_printf("%s director declined for %s (not a user)\n",
    dblock->name, addr->local_part);
  return DECLINE;
  }

/* If there is a user and a setting of match_directory, check for a match. */

if (pw != NULL && ob->pw_dir != NULL)
  {
  rc = direct_match_directory(addr, pw->pw_dir, ob->pw_dir, dblock->name);
  if (rc != OK) return rc;
  }

/* If the file_directory option is set expand the string, and set it as the
"home" directory. The expansion can contain $home if check_local_user
is set. */

if (ob->file_dir != NULL)
  {
  directory = expand_string(ob->file_dir);
  if (directory == NULL)
    {
    addr->message = string_sprintf("failed to expand \"%s\": %s",
      ob->file_dir, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  deliver_home = directory;
  }

/* The text of the forward/filter file is either a complete file, or is
obtained by expanding the "data" option. This is an interim state, working
towards amalgamating forwardfile and aliasfile. */

/* In the case of a file, sort out the file name. */

if (ob->file != NULL)
  {
  /* Get the required file name and expand it. If the expansion fails, log the
  incident and indicate an internal error. */

  filename = expand_string(ob->file);

  if (filename == NULL)
    {
    addr->message = string_sprintf("failed to expand \"%s\": %s",
      ob->file, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }

  DEBUG(2) debug_printf("%s director: file = %s\n", dblock->name, filename);

  /* If a directory is set and the file name is not absolute, construct the
  complete file name. Otherwise set a flag to prevent an attempt at statting the
  directory below. */

  if (directory != NULL && filename[0] != '/')
    filename = string_sprintf("%s/%s", directory, filename);
  else stat_directory = FALSE;

  /* Check that the file name is absolute. Simple checks are done in the
  init function, but expansions mean that we have to do a final check here. */

  if (filename[0] != '/')
    {
    log_write(0, LOG_MAIN, "%s is not an absolute path for the %s director",
      filename, dblock->name);
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  }

/* If not using a file, the text is obtained by expanding data. We do that now,
before changing uid, so that lookups run as the exim user. If the expansion is
forced to fail, we give up (cf non-existent file). The variable "filename" is
set appopriately for use in messages about syntax errors. */

else
  {
  filename = "inline forwarding data";
  filebuf = expand_string(ob->data);
  if (filebuf == NULL)
    {
    if (expand_string_forcedfail)
      {
      DEBUG(2) debug_printf("forced failure for expansion of \"%s\"\n",ob->data);
      return DECLINE;
      }
    addr->message = string_sprintf("failed to expand \"%s\": %s",
      ob->data, expand_string_message);
    if (search_find_defer) return DEFER;
    addr->special_action = SPECIAL_FREEZE;
    return ERROR;
    }
  }

/* Sort out the fixed or dynamic uid/gid. This uid is used (a) for reading the
file (and interpreting a filter) if seteuid is on and (b) for running the
transports for generated file and pipe addresses. It is not (necessarily) the
same as the uids that may own the file. Exim panics if an expanded string is
not a number and can't be found in the password file. Other errors set the
freezing bit. */

if (!direct_get_ugid(dblock, "director", addr, &ugid)) return ERROR;

/* If the seteuid option is on and a uid is available (either explicit or as a
result of check_local_user), use seteuid() to become the local user. This is
necessary in order to read .forward files that are in NFS-mounted home
directories. We also need to do it for running filter files. The macros expand
to -1 on systems without setuid() configured, but in those cases this code is
never obeyed, as the option is locked out in the init check.

Even if the seteuid option is not on, we need to do this when the security
level is 1 or 3, because in those cases this code is running seteuid to exim.
But only when a uid is available. */

if ((ugid.uid_set || ob->check_local_user) &&
    (ob->use_seteuid || security_level == 1 || security_level == 3))
  {
  saved_euid = geteuid();
  saved_egid = getegid();
  if (saved_euid != root_uid) mac_seteuid(root_uid);

  if (ugid.uid_set)
    {
    if (dblock->initgroups)
      {
      struct passwd *ppw = getpwuid(ugid.uid);
      if (ppw != NULL) initgroups(ppw->pw_name, ugid.gid);
      }
    mac_setegid(ugid.gid);
    mac_seteuid(ugid.uid);
    }
  else
    {
    if (dblock->initgroups) initgroups(pw->pw_name, pw->pw_gid);
    mac_setegid(pw->pw_gid);
    mac_seteuid(pw->pw_uid);
    }

  DEBUG(2) debug_print_ids("set");
  restore_uid = TRUE;
  }

/* Handle the testing and reading of a complete file. You might think we could
just test for the existence of the required file by attempting to open it, but
life isn't that simple. In many large systems, .forward files in users' home
directories are used, with the home directories possibly NFS-mounted from some
remote place. It doesn't seem possible to detect the state of "NFS mount
inaccessible" just by trying to open a file.

The common case is specified with a relative path name (relative to the home
directory or to a specified directory), and in that case we try to do a bit
better by statting the directory first. If it cannot be statted, assume there
is some mounting problem, and defer the delivery.

It seems that in Solaris 2.6, statting an entry in an indirect map that is
currently unmounted does not cause the mount to happen. Instead, dummy data is
returned, which defeats the whole point of this test. However, if a stat() is
done on some object inside the directory, such as the "." back reference to
itself, then the mount does occur. The stat() may, however, fail as a result
of a lack of search permission for the directory. This of course means that
opening .forward is also going to fail, but what that failure means is
controlled by an option below, so it seems that the safest plan is to do the
"mounting" stat, and then the directory stat as before if the first one fails
with EACCES.

If an NFS host is taken offline, it is possible for the stat() to get stuck
until it comes back. To guard against this, stick a timer round it. */

if (ob->file != NULL)
  {
  if (directory != NULL && stat_directory)
    {
    int rc, saved_errno;
    sigalrm_seen = FALSE;
    os_non_restarting_signal(SIGALRM, sigalrm_handler);
    alarm(30);
    sprintf(big_buffer, "%.*s/.", big_buffer_size, directory);
    rc = stat(big_buffer, &statbuf);
    if (rc == 0)
      {
      DEBUG(2) debug_printf("successful stat of %s\n", big_buffer);
      }
    else if (errno == EACCES && !sigalrm_seen)
      {
      rc = stat(directory, &statbuf);
      if (rc == 0)
        {
        DEBUG(2) debug_printf("successful stat of %s\n", directory);
        }
      }
    saved_errno = errno;
    alarm(0);
    signal(SIGALRM, SIG_DFL);

    if (sigalrm_seen || rc != 0)
      {
      addr->message = string_sprintf("failed to stat %s (%s)", directory,
        sigalrm_seen? "timeout" : strerror(saved_errno));
      yield = DEFER;
      goto RESTORE_UID;             /* skip forward */
      }

    }

  /* Now try to open the file for reading. If this fails with a non-existence
  error, we have no option but to believe that the file does not exist, so the
  director gives up on this address. Some other cases are more dubious, and may
  indicate configuration errors. For this reason, their handling is controlled by
  options.

  ENOTDIR means that something along the line is not a directory: there are
  installations that set home directories to be /dev/null for non-login accounts
  but in normal circumstances this indicates some kind of configuration error.

  EACCES means there's a permissions failure. Some users turn off read
  permission on a .forward file to suspend forwarding, but this is probably an
  error in any kind of mailing list processing. */

  fwd = fopen(filename, "r");
  if (fwd == NULL)
    {
    switch(errno)
      {
      case ENOENT:          /* File does not exist */
      DEBUG(2) debug_printf("%s not found\n", filename);
      yield = DECLINE;
      break;

      case ENOTDIR:         /* Something on the path isn't a directory */
      if (ob->ignore_enotdir)
        {
        DEBUG(2) debug_printf("non-directory on path %s: file assumed not to "
          "exist\n", filename);
        yield = DECLINE;
        break;
        }
      goto DEFAULT_ERROR;

      case EACCES:           /* Permission denied */
      if (ob->ignore_eacces)
        {
        DEBUG(2) debug_printf("permission denied for %s: file assumed not to "
          "exist\n", filename);
        yield = DECLINE;
        break;
        }
      /* Else fall through */

      DEFAULT_ERROR:
      default:
      addr->message = string_open_failed(errno, "%s", filename);
      yield = ERROR;
      break;
      }

    goto RESTORE_UID;               /* skip forward */
    }


  /* Now check up on the mode of the file. It is tempting to do this stat before
  opening the file, and use it as an existence check. However, doing that opens a
  small security loophole in that the status could be changed before the file is
  opened. Can't quite see what problems this might lead to, but you can't be too
  careful where security is concerned. Fstat() on an open file can normally be
  expected to succeed, but there are some NFS states where it does not. */

  if (fstat(fileno(fwd), &statbuf) != 0)
    {
    yield = DEFER;
    goto CLOSE_RESTORE_UID;         /* skip forward */
    }

  if ((statbuf.st_mode & S_IFMT) != S_IFREG)
    {
    addr->basic_errno = ERRNO_BADFORWARD;
    addr->message = string_sprintf("%s is not a regular file", filename);
    yield = DEFER;
    goto CLOSE_RESTORE_UID;         /* skip forward */
    }

  if ((statbuf.st_mode & ob->modemask) != 0)
    {
    addr->basic_errno = ERRNO_BADFORWARD;
    addr->message =
      string_sprintf("bad mode (0%o) for %s: 0%o bit(s) unexpected",
        statbuf.st_mode, filename, statbuf.st_mode & ob->modemask);
    yield = DEFER;
    goto CLOSE_RESTORE_UID;         /* skip forward */
    }

  /* Check the owner and group, including the current user if check_local_user
  was set. If there is nothing to check, let it go. */

  if (ob->check_local_user)
    { if (statbuf.st_uid == pw->pw_uid) uid_ok = TRUE; }
  else if (ob->owners == NULL || (int)(ob->owners[0]) == 0) uid_ok = TRUE;

  if (!uid_ok && ob->owners != NULL)
    {
    int i;
    for (i = 1; i <= (int)(ob->owners[0]); i++)
      if (ob->owners[i] == statbuf.st_uid) { uid_ok = TRUE; break; }
    }

  /* If gid is to be checked, the local user's gid is always permissible. */

  if (!gid_ok)
    {
    if (ob->check_local_user)
      { if (statbuf.st_gid == pw->pw_gid) gid_ok = TRUE; }
    if (!gid_ok && ob->owngroups != NULL && (int)(ob->owngroups[0]) != 0)
      {
      int i;
      for (i = 1; i <= (int)(ob->owngroups[0]); i++)
        if (ob->owngroups[i] == statbuf.st_gid) { gid_ok = TRUE; break; }
      }
    }

  if (!uid_ok || !gid_ok)
    {
    addr->basic_errno = ERRNO_BADFORWARD;
    addr->message =
      string_sprintf("bad %s for %s", uid_ok? "group" : "owner", filename);
    yield = DEFER;
    goto CLOSE_RESTORE_UID;         /* skip forward */
    }

  /* Processing a .forward file could use a fair bit of store, especially if it
  is a filter file. Therefore it is a good idea to recover the store used
  afterwards. In order to do this, we switch to the temporary stacking pool, and
  recover that. Any new addresses that are created are put in the main pool, so
  as to remain around afterwards. */

  store_pool = POOL_TEMP;
  reset_point = store_get(0);

  /* Read the .forward file. We read the file in one go in order to minimize
  the time we have it open. */

  filebuf = store_get(statbuf.st_size + 1);
  if (fread(filebuf, 1, statbuf.st_size, fwd) != statbuf.st_size)
    {
    store_pool = POOL_MAIN;
    addr->basic_errno = errno;
    addr->message = "error while reading forward file";
    yield = DEFER;
    goto CLOSE_RESTORE_UID;         /* skip forward */
    }
  filebuf[statbuf.st_size] = 0;

  /* Don't pass statbuf.st_size directly to debug_printf. On some systems it is a
  long, which may not be the same as an int. */

  DEBUG(2)
    {
    int size = statbuf.st_size;
    debug_printf("%d bytes read from %s\n", size, filename);
    }
  }

/* If we already have the text from a database lookup, all we need to do here
is to change to the temporary storage pool. */

else
  {
  store_pool = POOL_TEMP;
  reset_point = store_get(0);
  }

/* If the filter option is set, the file is to be interpreted as a filter
file instead of a straight list of addresses, if it starts with
"# Exim filter ..." (any capitilization, spaces optional). Check for this
in all cases, so as to give a warning message if it is found when filtering
is not enabled. */

s = filebuf;
tag = "# exim filter";
while (isspace((uschar)*s)) s++;     /* Skips initial blank lines */
for (; *tag != 0; s++, tag++)
  {
  if (*tag == ' ')
    {
    while (*s == ' ' || *s == '\t') s++;
    s--;
    }
  else if (tolower(*s) != tolower(*tag)) break;
  }
if (*tag == 0) filter_header = TRUE;

/* If qualify_preserve_domain is set, temporarily reset qualify_domain_
recipient to the current domain so that any unqualified addresses will get
qualified with the same domain as the incoming address. */

if (ob->qualify_preserve_domain) qualify_domain_recipient = addr->domain;

/* Filter interpretation is done by a general function that is also called from
the filter testing option (-bf). Several features of string expansion may be
locked out at sites that don't trust users. This is done by setting global
flags that the expander inspects. The second-last argument specifies whether
the log command should be locked out or skipped; this is done when verifying or
testing addresses, or if we have not set the uid to a local user, or when
explicitly configured. */

if (filter_header && ob->filter)
  {
  BOOL extracted;
  BOOL oldforbid_exists = expand_forbid_exists;
  BOOL oldforbid_lookup = expand_forbid_lookup;
  BOOL oldforbid_perl = expand_forbid_perl;

  DEBUG(2) debug_printf("file is a filter file\n");
  is_filter = TRUE;

  expand_forbid_exists = ob->forbid_filter_exists;
  expand_forbid_lookup = ob->forbid_filter_lookup;
  expand_forbid_perl = ob->forbid_filter_perl;

  extracted = filter_interpret(filebuf, &generated, &delivered,
    (ob->allow_system_actions)? &filter_action : NULL,
    &error,
    (verify || address_test_mode)? 1 : /* skip logging if verifying/testing */
      ob->forbid_filter_logwrite? 2 :  /* lock out if forbidden */
      restore_uid? 0 : 3,              /* else permit only if uid set */
    ob->rewrite);

  expand_forbid_exists = oldforbid_exists;
  expand_forbid_lookup = oldforbid_lookup;
  expand_forbid_perl = oldforbid_perl;

  store_pool = POOL_MAIN;              /* Reset for error strings */

  /* If extraction from a filter file failed, it is a "probable user error", to
  use a good old IBM term. By default, just defer delivery and let the user
  clean things up in due course. However, users are very lax about this. The
  skip_syntax_errors option now applies to filter files as well as to normal
  forward files. It can be used to carry on delivering, while sending an
  error message if syntax_errors_to is set. */

  if (!extracted)
    {
    if (ob->skip_syntax_errors)
      {
      eblock = store_get(sizeof(error_block));
      eblock->next = NULL;
      eblock->text1 = error;
      eblock->text2 = NULL;
      yield = OK;
      }
    else
      {
      addr->basic_errno = ERRNO_BADFORWARD;
      addr->message = string_sprintf("error in filter file: %s", error);
      yield = DEFER;
      }
    }
  }

/* Otherwise it's a vanilla .forward file; call parse_extract_addresses()
to get the values. The yield is 0=>OK, -1=>error, +1=> failed to open an
:include: file. */

else
  {
  int extracted;
  DEBUG(2) debug_printf("file is not a filter file\n");

  /* There is a common function for use by forwarding and aliasing
  directors that extracts a list of addresses from a text string.
  Setting the fourth argument TRUE says that generating no addresses
  (from valid syntax) is no error.

  The forward file may include :include: items, and we have to be
  careful about permissions for reading them. The extracting function
  will check that include files begin with a specified string, unless
  NULL is supplied. Supplying "*" locks out :include: files, since they
  must be absolute paths. We lock them out if (a) requested to do so or
  (b) we haven't used seteuid and there's no directory to check. If
  seteuid has been used, just try to read anything; otherwise restrict
  to the directory or lock out if none. */

  extracted = parse_extract_addresses(filebuf,
    addr->domain,                      /* to qualify \name */
    &generated,                        /* where to hang them */
    &error,                            /* for errors */
    TRUE,                              /* no addresses => no error */
    FALSE,                             /* don't recognize :blackhole: etc. */
    ob->rewrite,                       /* rewrite if configured */
    ob->forbid_include? "*" :          /* includes forbidden */
    restore_uid? NULL :                /* if seteuid, try all */
    (directory == NULL)? "*" :         /* if no directory, lock out */
    directory,                         /* else restrain to directory */
    ob->skip_syntax_errors? &eblock : NULL);

  store_pool = POOL_MAIN;                      /* Reset for error strings */

  /* If extraction from a .forward file failed, freeze and yield ERROR if
  it was a missing :include: file and freeze_missing_include is TRUE. Other-
  wise just DEFER and hope things get fixed eventually. */

  if (extracted != 0)
    {
    addr->basic_errno = ERRNO_BADFORWARD;
    addr->message =
      string_sprintf("error in forward file%s: %s",
        filter_header? " (filtering not enabled)" : "", error);
    if (extracted > 0 && ob->freeze_missing_include)
      {
      addr->special_action = SPECIAL_FREEZE;
      yield = ERROR;
      }
    else yield = DEFER;
    }
  }

/* Reset the general qualify domain. */

qualify_domain_recipient = save_qualify_domain_recipient;

/* At this point we are finished with the .forward file. Close it, and, if
seteuid was used above, restore the previous effective uid and gid. The dreaded
goto is used above to skip to this code when errors are detected. */

CLOSE_RESTORE_UID:

if (fwd != NULL) fclose(fwd);

RESTORE_UID:

if (restore_uid)
  {
  mac_seteuid(root_uid);
  if (dblock->initgroups) setgroups(0, NULL);
  mac_setegid(saved_egid);
  mac_seteuid(saved_euid);
  DEBUG(2) debug_print_ids("restored");
  }


/* If yield is ok, but skip_syntax_errors was set and there were syntax errors
in the list, error messages will be present in eblock. Log them and send a
message if so configured, using a function that is common to aliasfile and
forwardfile. We cannot do this earlier, because the error message must be sent
as Exim, not as the local user. */

if (yield == OK && eblock != NULL &&
    !moan_skipped_syntax_errors(
      dblock->name,                           /* For message content */
      filename,                               /* Ditto */
      eblock,                                 /* Ditto */
      (verify || address_test_mode)?
        NULL : ob->syntax_errors_to,          /* Who to mail */
      generated != NULL,                      /* True if not all failed */
      ob->syntax_errors_text))                /* Custom message */
  {
  addr->special_action = SPECIAL_FREEZE;
  yield = ERROR;
  }


/* Now we are finished with the temporary store, if it was in use. */

if (reset_point != NULL)
  {
  store_pool = POOL_TEMP;
  store_reset(reset_point);
  store_pool = POOL_MAIN;
  }


/* If there has been an error, return the error value now. Subsequently we
can just return directly on error, since there is no further need to mess with
the uid or close the file. */

if (yield != OK) return yield;

/* Handle the generated addresses; set up the errors address and the additional
and removeable headers, and then add the addresses to the addr_new chain, using
the common function. We save the errors_address in the original parent, in case
any of the children are routed/directed in an "unseen" manner. */

if (generated != NULL)
  {
  rc = direct_get_errors_address(addr, dblock, "director", verify, &errors_to);
  if (rc != OK) return rc;
  addr->errors_address = errors_to;


  rc = direct_get_munge_headers(addr, dblock, "director", &extra_headers,
    &remove_headers);
  if (rc != OK) return rc;

  df_generated(dblock, addr_new, addr, generated, errors_to, extra_headers,
    remove_headers, &ugid, pw, directory);
  }

/* Otherwise, there are no generated addresses. If this was a .forward file,
just decline. */

else if (!is_filter) return DECLINE;

/* It was a filter file. If "delivered" is not set, and no special action was
requested, just decline. This also happens if the action is "freeze" but there
has been a manual thaw. Otherwise, if not a special action, a filter is just
discarding deliveries. */

else
  {
  if (filter_action == SPECIAL_FREEZE && deliver_manual_thaw)
    filter_action = SPECIAL_NONE;
  if (!delivered && filter_action == SPECIAL_NONE) return DECLINE;

  /* Either "delivered" or a special action is required */

  if (filter_action == SPECIAL_NONE)
    {
    if (!verify && !address_test_mode)
      log_write(0, LOG_MAIN, "=> discarded <%s> D=%s", addr->orig,
        dblock->name);
    return OK;
    }
  }

/* If a filter returned a special action (freeze or fail - these can only occur
if allow_special_actions is set) deal with them now. Like the system filter,
freeze does not happen after a manual thaw. In case there were deliveries
set up by the filter (which will be done), we set the child count high so that
their completion does not mark the original address done. */

if (filter_action == SPECIAL_FREEZE && !deliver_manual_thaw)
  {
  addr->message = (filter_fmsg == NULL)?
    "frozen by filter" : string_copy(filter_fmsg);
  addr->special_action = SPECIAL_FREEZE;
  addr->child_count = 9999999;
  return ERROR;
  }
else if (filter_action == SPECIAL_FAIL)
  {
  addr->message = (filter_fmsg == NULL)?
    "rejected by filter" : string_copy(filter_fmsg);
  return FORCEFAIL;
  }

/* If the filter interpreter returned not "delivered", we have to arrange for
this address to be passed on to subsequent directors. Returning DECLINE would
appear to be the answer, but it isn't, because successful delivery of the base
address gets it marked "done", so deferred generated addresses never get tried
again. We have to generate a new version of the base address, as if there were
a "deliver" command in the filter file, with the original address as parent. */

if (is_filter && !delivered)
  {
  address_item *next;

  next = deliver_make_addr(addr->orig, FALSE);
  next->parent = addr;
  copyflag(next, addr, (af_pfr|af_forced_local|af_ignore_error));
  addr->child_count++;
  next->next = *addr_new;
  *addr_new = next;

  next->errors_address = errors_to;
  next->extra_headers = extra_headers;
  next->remove_headers = remove_headers;

  DEBUG(2) debug_printf("%s director generated %s\n%s%s%s%s",
    dblock->name,
    next->orig,
    testflag(next, af_pfr)? "  pipe, file, or autoreply\n" : "",
    (errors_to != NULL)? "  errors to " : "",
    (errors_to != NULL)? errors_to : "",
    (errors_to != NULL)? "\n" : "");
  }

/* Put the original address onto the succeed queue so that any retry items that
get attached to it get processed. */

addr->next = *addr_succeed;
*addr_succeed = addr;

return OK;
}

/* End of directors/forwardfile.c */
