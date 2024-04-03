/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "pipe.h"



/* Options specific to the pipe transport. They must be in alphabetic
order (note that "_" comes before the lower case letters). Those starting
with "*" are not settable by the user but are used by the option-reading
software for alternative value types. Some options are stored in the transport
instance block so as to be publicly visible; these are flagged with opt_public.
*/

optionlist pipe_transport_options[] = {
  { "*expand_group",     opt_stringptr | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, expand_gid)) },
  { "*expand_user",      opt_stringptr | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, expand_uid)) },
  { "*set_group",         opt_bool | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, gid_set)) },
  { "*set_user",          opt_bool | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, uid_set)) },
  { "allow_commands",    opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, allow_commands)) },
  { "batch",             opt_local_batch | opt_public,
      (void *)(offsetof(transport_instance, local_batch)) },
  { "batch_max",         opt_int | opt_public,
      (void *)(offsetof(transport_instance, batch_max)) },
  { "bsmtp",             opt_local_batch | opt_public,
      (void *)(offsetof(transport_instance, local_smtp)) },
  { "bsmtp_helo",        opt_bool | opt_public,
      (void *)(offsetof(transport_instance, bsmtp_helo)) },
  { "check_string",      opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, check_string)) },
  { "command",           opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, cmd)) },
  { "current_directory", opt_stringptr | opt_public,
      (void *)(offsetof(transport_instance, current_dir)) },
  { "environment",       opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, environment)) },
  { "escape_string",     opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, escape_string)) },
  { "freeze_exec_fail",  opt_bool,
      (void *)(offsetof(pipe_transport_options_block, freeze_exec_fail)) },
  { "from_hack",         opt_bool,
      (void *)(offsetof(pipe_transport_options_block, from_hack)) },
  { "group",             opt_expand_gid | opt_public,
      (void *)(offsetof(transport_instance, gid)) },
  { "home_directory",    opt_stringptr | opt_public,
      (void *)(offsetof(transport_instance, home_dir)) },
  { "ignore_status",     opt_bool,
      (void *)(offsetof(pipe_transport_options_block, ignore_status)) },
  { "initgroups",        opt_bool | opt_public,
      (void *)(offsetof(transport_instance, initgroups)) },
  { "log_defer_output",  opt_bool | opt_public,
      (void *)(offsetof(transport_instance, log_defer_output)) },
  { "log_fail_output",   opt_bool | opt_public,
      (void *)(offsetof(transport_instance, log_fail_output)) },
  { "log_output",        opt_bool | opt_public,
      (void *)(offsetof(transport_instance, log_output)) },
  { "max_output",        opt_mkint,
      (void *)(offsetof(pipe_transport_options_block, max_output)) },
  { "path",              opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, path)) },
  { "pipe_as_creator",   opt_bool | opt_public,
      (void *)(offsetof(transport_instance, deliver_as_creator)) },
  { "prefix",            opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, prefix)) },
  { "restrict_to_path",  opt_bool,
      (void *)(offsetof(pipe_transport_options_block, restrict_to_path)) },
  { "retry_use_local_part", opt_bool | opt_public,
      (void *)offsetof(transport_instance, retry_use_local_part) },
  { "return_fail_output",opt_bool | opt_public,
      (void *)(offsetof(transport_instance, return_fail_output)) },
  { "return_output",     opt_bool | opt_public,
      (void *)(offsetof(transport_instance, return_output)) },
  { "suffix",            opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, suffix)) },
  { "temp_errors",       opt_stringptr,
      (void *)(offsetof(pipe_transport_options_block, temp_errors)) },
  { "timeout",           opt_time,
      (void *)(offsetof(pipe_transport_options_block, timeout)) },
  { "umask",             opt_octint,
      (void *)(offsetof(pipe_transport_options_block, umask)) },
  { "use_crlf",          opt_bool,
      (void *)(offsetof(pipe_transport_options_block, use_crlf)) },
  { "use_shell",         opt_bool,
      (void *)(offsetof(pipe_transport_options_block, use_shell)) },
  { "user",              opt_expand_uid | opt_public,
      (void *)(offsetof(transport_instance, uid)) },
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int pipe_transport_options_count =
  sizeof(pipe_transport_options)/sizeof(optionlist);

/* Default private options block for the pipe transport. */

pipe_transport_options_block pipe_transport_option_defaults = {
  NULL,           /* cmd */
  NULL,           /* allow_commands */
  NULL,           /* environment */
  "/usr/bin",     /* path */
  "From ${if def:return_path{$return_path}{MAILER-DAEMON}} ${tod_bsdinbox}\n",
                  /* prefix */
  "\n",           /* suffix */
  mac_expanded_string(EX_TEMPFAIL) ":"    /* temp_errors */
  mac_expanded_string(EX_CANTCREAT),
  NULL,           /* check_string */
  NULL,           /* escape_string */
  022,            /* umask */
  20480,          /* max_output */
  60*60,          /* timeout */
  0,              /* options */
  FALSE,          /* freeze_exec_fail */
  FALSE,          /* from_hack */
  FALSE,          /* ignore_status */
  FALSE,          /* restrict_to_path */
  FALSE,          /* use_shell */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
pipe_transport_init(transport_instance *tblock)
{
pipe_transport_options_block *ob =
  (pipe_transport_options_block *)(tblock->options_block);

/* Retry_use_local_part defaults TRUE if unset */

if (tblock->retry_use_local_part == 2) tblock->retry_use_local_part = TRUE;

/* If pipe_as_creator is set, then uid/gid should not be set. */

if (tblock->deliver_as_creator && (tblock->uid_set || tblock->gid_set ||
  tblock->expand_uid != NULL || tblock->expand_gid != NULL))
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "both pipe_as_creator and an explicit uid/gid are set for the %s "
        "transport", tblock->name);

/* If a fixed uid field is set, then a gid field must also be set. */

if (tblock->uid_set && !tblock->gid_set && tblock->expand_gid == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "user set without group for the %s transport", tblock->name);

/* Temp_errors must consist only of digits and colons, but there can be
spaces round the colons, so allow them too. */

if (ob->temp_errors != NULL)
  {
  size_t p = strspn(ob->temp_errors, "0123456789: ");
  if (ob->temp_errors[p] != 0)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "temp_errors option must be a list of numbers in the %s transport",
        tblock->name);
  }

/* Only one of return_output/return_fail_output or log_output/log_fail_output
should be set. */

if (tblock->return_output && tblock->return_fail_output)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "both return_output and return_fail_output set for %s transport",
    tblock->name);

if (tblock->log_output && tblock->log_fail_output)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "both log_output and log_fail_output set for %s transport",
    tblock->name);

/* If batch SMTP is set, ensure the generic local batch option matches. They
take the same set of values. Also force the check and escape strings, and
arrange that headers are also escaped. */

if (tblock->local_smtp != local_smtp_off)
  {
  tblock->local_batch = tblock->local_smtp;
  ob->check_string = ".";
  ob->escape_string = "..";
  ob->options |= topt_escape_headers;
  }

/* The restrict_to_path  and use_shell options are incompatible */

if (ob->restrict_to_path && ob->use_shell)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "both restrict_to_path and use_shell set for %s transport",
    tblock->name);

/* The allow_commands and use_shell options are incompatible */

if (ob->allow_commands && ob->use_shell)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "both allow_commands and use_shell set for %s transport",
    tblock->name);

/* Set up the bitwise options for transport_write_message from the various
driver options. Only one of body_only and headers_only can be set. */

ob->options |=
  (tblock->body_only? topt_no_headers : 0) |
  (tblock->headers_only? topt_no_body : 0) |
  (tblock->return_path_add? topt_add_return_path : 0) |
  (tblock->delivery_date_add? topt_add_delivery_date : 0) |
  (tblock->envelope_to_add? topt_add_envelope_to : 0) |
  (ob->use_crlf? topt_use_crlf : 0);
}



/*************************************************
*          Set up direct (non-shell) command     *
*************************************************/

/* This function is called when a command line is to be parsed by the transport
and executed directly, without the use of /bin/sh.

Arguments:
  argvptr            pointer to anchor for argv vector
  cmd                points to the command string
  expand_arguments   true if expansion is to occur
  addr               chain of addresses
  tname              the transport name
  ob                 the transport options block

Returns:             TRUE if all went well; otherwise an error will be
                     set in the first address and FALSE returned
*/

static BOOL
set_up_direct_command(char ***argvptr, char *cmd, BOOL expand_arguments,
  address_item *addr, char *tname, pipe_transport_options_block *ob)
{
BOOL permitted = FALSE;
char **argv;
char buffer[64];

/* Set up "transport <name>" to be put in any error messages, and then
call the common function for creating an argument list and expanding
the items if necessary. If it fails, this function fails (error information
is in the addresses). */

sprintf(buffer, "%.50s transport", tname);
if (!transport_set_up_command(argvptr, cmd, expand_arguments,
      (addr->local_part[0]== '|')? FAIL : PANIC,
      addr, buffer, NULL))
  return FALSE;

/* Point to the set-up arguments. */

argv = *argvptr;

/* If allow_commands is set, see if the command is in the permitted list. */

if (ob->allow_commands != NULL)
  {
  int sep = 0;
  char *s, *p;
  char buffer[256];

  s = expand_string(ob->allow_commands);
  if (s == NULL)
    {
    addr->transport_return = DEFER;
    addr->message = string_sprintf("failed to expand string \"%s\" "
      "for %s transport: %s", ob->allow_commands, tname, expand_string_message);
    return FALSE;
    }

  while ((p = string_nextinlist(&s, &sep, buffer, sizeof(buffer))) != NULL)
    {
    if (strcmp(p, argv[0]) == 0) { permitted = TRUE; break; }
    }
  }

/* If permitted is TRUE it means the command was found in the allowed list, and
no further checks are done. If permitted = FALSE, it either means
allow_commands wasn't set, or that the command didn't match anything in the
list. In both cases, if restrict_to_path is set, we fail if the command
contains any slashes, but if restrict_to_path is not set, we must fail the
command only if allow_commands is set. */

if (!permitted)
  {
  if (ob->restrict_to_path)
    {
    if (strchr(argv[0], '/') != NULL)
      {
      addr->transport_return = FAIL;
      addr->message = string_sprintf("\"/\" found in \"%s\" (command for %s "
        "transport) - failed for security reasons", cmd, tname);
      return FALSE;
      }
    }

  else if (ob->allow_commands != NULL)
    {
    addr->transport_return = FAIL;
    addr->message = string_sprintf("\"%s\" command not permitted by %s "
      "transport", argv[0], tname);
    return FALSE;
    }
  }

/* If the command is not an absolute path, search the PATH directories
for it. */

if (argv[0][0] != '/')
  {
  int sep = 0;
  char *p;
  char *listptr = ob->path;
  char buffer[1024];

  while ((p = string_nextinlist(&listptr, &sep, buffer, sizeof(buffer))) != NULL)
    {
    struct stat statbuf;
    sprintf(big_buffer, "%.256s/%.256s", p, argv[0]);
    if (stat(big_buffer, &statbuf) == 0)
      {
      argv[0] = string_copy(big_buffer);
      break;
      }
    }
  if (p == NULL)
    {
    addr->transport_return = FAIL;
    addr->message = string_sprintf("\"%s\" command not found for %s transport",
      argv[0], tname);
    return FALSE;
    }
  }

return TRUE;
}


/*************************************************
*               Set up shell command             *
*************************************************/

/* This function is called when a command line is to be passed to /bin/sh
without parsing inside the transport.

Arguments:
  argvptr            pointer to anchor for argv vector
  cmd                points to the command string
  expand_arguments   true if expansion is to occur
  addr               chain of addresses
  tname              the transport name

Returns:             TRUE if all went well; otherwise an error will be
                     set in the first address and FALSE returned
*/

static BOOL
set_up_shell_command(char ***argvptr, char *cmd, BOOL expand_arguments,
  address_item *addr, char *tname)
{
char **argv;

*argvptr = argv = store_get((4)*sizeof(char *));

argv[0] = "/bin/sh";
argv[1] = "-c";

/* We have to take special action to handle the special "variable" called
$pipe_addresses, which is not recognized by the normal expansion function. */

DEBUG(7) debug_printf("shell pipe command before expansion:\n  %s\n", cmd);

if (expand_arguments)
  {
  char *s = cmd;
  char *p = strstr(cmd, "pipe_addresses");

  if (p != NULL && (
         (p > cmd && p[-1] == '$') ||
         (p > cmd + 1 && p[-2] == '$' && p[-1] == '{' && p[14] == '}')))
    {
    address_item *ad;
    char *q = p + 14;
    int size = (int)strlen(cmd) + 64;
    int offset;

    if (p[-1] == '{') { q++; p--; }

    s = store_get(size);
    offset = p - cmd - 1;
    strncpy(s, cmd, offset);

    for (ad = addr; ad != NULL; ad = ad->next)
      {
      if (ad != addr) string_cat(s, &size, &offset, " ", 1);
      string_cat(s, &size, &offset, ad->orig, (int)strlen(ad->orig));
      }

    string_cat(s, &size, &offset, q, (int)strlen(q));
    s[offset] = 0;
    }

  /* Allow $recipients in the expansion iff it comes from a system filter */

  enable_dollar_recipients = addr != NULL &&
    addr->parent != NULL && strcmp(addr->parent->orig, "message filter") == 0;
  argv[2] = expand_string(s);
  enable_dollar_recipients = FALSE;

  if (argv[2] == NULL)
    {
    addr->transport_return = search_find_defer? DEFER :
      (addr->local_part[0] == '|')? FAIL : PANIC;
    addr->message = string_sprintf("Expansion of command \"%s\" "
      "in %s transport failed: %s",
      cmd, tname, expand_string_message);
    return FALSE;
    }

  DEBUG(7) debug_printf("shell pipe command after expansion:\n  %s\n", argv[2]);
  }
else argv[2] = cmd;

argv[3] = (char *)0;
return TRUE;
}




/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface details. This transport always returns FALSE,
indicating that the status in the first address is the status for all addresses
in a batch. */

BOOL
pipe_transport_entry(
  transport_instance *tblock,      /* data for this instantiation */
  address_item *addr)              /* address(es) we are working on */
{
pid_t pid, outpid;
int fd_in, fd_out, rc;
int envcount = 0;
int envsep = 0;
pipe_transport_options_block *ob =
  (pipe_transport_options_block *)(tblock->options_block);
int timeout = ob->timeout;
BOOL written_ok = FALSE;
BOOL expand_arguments;
char **argv;
char *envp[40];
char *envlist = ob->environment;
char *env_local_part;
char *cmd, *ss;
char *eol = (ob->use_crlf)? "\r\n" : "\n";

DEBUG(2) debug_printf("%s transport entered\n", tblock->name);

/* Set up for the good case */

addr->transport_return = OK;
addr->basic_errno = 0;

/* Pipes are not accepted as general addresses, but they can be generated from
.forward files or alias files. In those cases, the command to be obeyed is
pointed to by addr->local_part; it starts with the pipe symbol. In other cases,
the command is supplied as one of the pipe transport's options. */

if (testflag(addr,af_pfr) && addr->local_part[0] == '|')
  {
  cmd = addr->local_part + 1;
  while (isspace((uschar)*cmd)) cmd++;
  expand_arguments = testflag(addr, af_expand_pipe);
  }
else
  {
  cmd = ob->cmd;
  expand_arguments = TRUE;
  }

/* When a pipe is set up by a filter file, there may be numerical variables
in existence. These are passed in addr->pipe_expandn for use here. */

if (expand_arguments && addr->pipe_expandn != NULL)
  {
  char **ss = addr->pipe_expandn;
  expand_nmax = -1;
  while (*ss != NULL)
    {
    expand_nstring[++expand_nmax] = *ss;
    expand_nlength[expand_nmax] = (int)strlen(*ss++);
    }
  }

/* The default way of processing the command is to split it up into arguments
here, and run it directly. This offers some security advantages. However, there
are installations that want by default to run commands under /bin/sh always, so
there is an option to do that. */

if (ob->use_shell)
  {
  if (!set_up_shell_command(&argv, cmd, expand_arguments, addr, tblock->name))
    return FALSE;
  }
else if (!set_up_direct_command(&argv, cmd, expand_arguments, addr,
  tblock->name, ob)) return FALSE;

expand_nmax = -1;   /* Reset */

/* Set up the environment for the command. */

env_local_part = (deliver_localpart == NULL)? "" : deliver_localpart;

envp[envcount++] = string_sprintf("LOCAL_PART=%s", env_local_part);
envp[envcount++] = string_sprintf("LOGNAME=%s", env_local_part);
envp[envcount++] = string_sprintf("USER=%s", env_local_part);

envp[envcount++] = string_sprintf("DOMAIN=%s", (deliver_domain == NULL)?
  "" : deliver_domain);

envp[envcount++] = string_sprintf("HOME=%s", (deliver_home == NULL)?
  "" : deliver_home);

envp[envcount++] = string_sprintf("MESSAGE_ID=%s", message_id_external);
envp[envcount++] = string_sprintf("PATH=%s", ob->path);
envp[envcount++] = string_sprintf("QUALIFY_DOMAIN=%s", qualify_domain_sender);
envp[envcount++] = string_sprintf("SENDER=%s", sender_address);
envp[envcount++] = "SHELL=/bin/sh";

if (addr->host_list != NULL)
  envp[envcount++] = string_sprintf("HOST=%s", addr->host_list->name);

/* Add any requested items */

if (envlist != NULL)
  {
  envlist = expand_string(envlist);
  if (envlist == NULL)
    {
    addr->transport_return = DEFER;
    addr->message = string_sprintf("failed to expand string \"%s\" "
      "for %s transport: %s", ob->environment, tblock->name,
      expand_string_message);
    return FALSE;
    }
  }

while ((ss = string_nextinlist(&envlist, &envsep, big_buffer, big_buffer_size))
       != NULL)
   {
   if (envcount > sizeof(envp)/sizeof(char *) - 2)
     log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Too many environment settings for "
       "%s transport", tblock->name);
   envp[envcount++] = string_copy(ss);
   }

envp[envcount] = NULL;


/* If the -N option is set, can't do any more. */

if (dont_deliver)
  {
  debug_printf("*** delivery by %s transport bypassed by -N option",
    tblock->name);
  return FALSE;
  }


/* Handling the output from the pipe is tricky. If a file for catching this
output is provided, we could in theory just hand that fd over to the process,
but this isn't very safe because it might loop and carry on writing for
ever (which is exactly what happened in early versions of Exim). Therefore we
use the standard child_open() function, which creates pipes. We can then read
our end of the output pipe and count the number of bytes that come through,
chopping the sub-process if it exceeds some limit.

However, this means we want to run a sub-process with both its input and output
attached to pipes. We can't handle that easily from a single parent process
using straightforward code such as the transport_write_message() function
because the subprocess might not be reading its input because it is trying to
write to a full output pipe. The complication of redesigning the world to
handle this is too great - simpler just to run another process to do the
reading of the output pipe. */


/* As this is a local transport, we are already running with the required
uid/gid and current directory, so pass NULL to child_open to indicate no
change. Request that the new process be a process group leader, so we
can kill it and all its children on a timeout. */

if ((pid = child_open(argv, envp, ob->umask, NULL, NULL, &fd_in, &fd_out,
    NULL, TRUE)) < 0)
  {
  addr->transport_return = DEFER;
  addr->message = string_sprintf(
    "Failed to create child process for %s transport: %s", tblock->name,
      strerror(errno));
  return FALSE;
  }

/* Now fork a process to handle the output that comes down the pipe. */

if ((outpid = fork()) < 0)
  {
  addr->basic_errno = errno;
  addr->transport_return = DEFER;
  addr->message = string_sprintf(
    "Failed to create process for handling output in %s transport",
      tblock->name);
  close(fd_in);
  close(fd_out);
  return FALSE;
  }

/* This is the code for the output-handling subprocess. Read from the pipe
in chunks, and write to the return file if one is provided. Keep track of
the number of bytes handled. If the limit is exceeded, try to kill the
subprocess group, and in any case close the pipe and exit, which should cause
the subprocess to fail. */

if (outpid == 0)
  {
  int count = 0;
  close(fd_in);
  set_process_info("reading output from |%s", cmd);
  while ((rc = read(fd_out, big_buffer, big_buffer_size)) > 0)
    {
    if (addr->return_file >= 0)
      write(addr->return_file, big_buffer, rc);
    count += rc;
    if (count > ob->max_output)
      {
      char *message = "\n\n*** Too much output - remainder discarded ***\n";
      DEBUG(2) debug_printf("Too much output from pipe - killed\n");
      if (addr->return_file >= 0)
        write(addr->return_file, message, (int)strlen(message));
      killpg(pid, SIGKILL);
      break;
      }
    }
  close(fd_out);
  _exit(0);
  }

close(fd_out);  /* Not used in this process */


/* Carrying on now with the main parent process. Attempt to write the message
to it down the pipe. It is a fallacy to think that you can detect write errors
when the sub-process fails to read the pipe. The parent process may complete
writing and close the pipe before the sub-process completes. We could sleep a
bit here to let the sub-process get going, but it may still not complete. So we
ignore all writing errors. */

DEBUG(2) debug_printf("Writing message to pipe\n");

/* Arrange to time out writes if there is a timeout set. */

if (timeout > 0)
  {
  sigalrm_seen = FALSE;
  os_non_restarting_signal(SIGALRM, sigalrm_handler);
  transport_write_timeout = timeout;
  }

/* If the local_smtp option is not unset, we need to write SMTP prefix
information. The various different values for batching are handled outside; if
there is more than one address available here, all must be included. Force
SMTP dot-handling. */

if (tblock->local_smtp != local_smtp_off)
  {
  address_item *a;

  if (tblock->bsmtp_helo &&
      !transport_write_string(fd_in, "HELO %s%s", primary_hostname, eol))
    goto END_WRITE;

  if (!transport_write_string(fd_in, "MAIL FROM:<%s>%s", return_path, eol))
    goto END_WRITE;

  for (a = addr; a != NULL; a = a->next)
    {
    if (!transport_write_string(fd_in,
        "RCPT TO:<%s@%s>%s", a->local_part, a->domain, eol))
      goto END_WRITE;
    }

  if (!transport_write_string(fd_in, "DATA%s", eol)) goto END_WRITE;
  }

/* Now any other configured prefix. */

if (ob->prefix != NULL)
  {
  char *prefix = expand_string(ob->prefix);
  if (prefix == NULL)
    {
    addr->transport_return = search_find_defer? DEFER : PANIC;
    addr->message = string_sprintf("Expansion of \"%s\" (prefix for %s "
      "transport) failed: %s", ob->prefix, tblock->name, expand_string_message);
    return FALSE;
    }
  if (!transport_write_block(fd_in, prefix, (int)strlen(prefix)))
    goto END_WRITE;
  }

/* Now the actual message - the options were set at initialization time */

if (!transport_write_message(addr, fd_in, ob->options, 0, tblock->add_headers,
  tblock->remove_headers, ob->check_string, ob->escape_string,
  tblock->rewrite_rules, tblock->rewrite_existflags))
    goto END_WRITE;

/* Now any configured suffix */

if (ob->suffix != NULL)
  {
  char *suffix = expand_string(ob->suffix);
  if (suffix == NULL)
    {
    addr->transport_return = search_find_defer? DEFER : PANIC;
    addr->message = string_sprintf("Expansion of \"%s\" (suffix for %s "
      "transport) failed: %s", ob->suffix, tblock->name, expand_string_message);
    return FALSE;
    }
  if (!transport_write_block(fd_in, suffix, (int)strlen(suffix)))
    goto END_WRITE;
  }

/* If local_smtp, write the terminating dot. */

if (tblock->local_smtp != local_smtp_off &&
    !transport_write_string(fd_in, ".%s", eol))
  goto END_WRITE;

/* Flag all writing completed successfully. */

written_ok = TRUE;

/* Come here if there are errors during writing. */

END_WRITE:

/* OK, the writing is now all done. Turn off the alarm handler and close the
pipe. */

signal(SIGALRM, SIG_IGN);
(void) close(fd_in);

/* Handle errors during writing. For timeouts, set the timeout for waiting for
the child process to 1 second. If the process at the far end of the pipe died
without reading all of it, we expect an EPIPE error, which should be ignored.
We used also to ignore WRITEINCOMPLETE but the writing function is now cleverer
at handling OS where the death of a pipe doesn't give EPIPE immediately. See
comments therein. This change made 04-Sep-98. Clean up this code in a year or
so. */

if (!written_ok)
  {
  if (errno == ETIMEDOUT)
    timeout = 1;
  else if (errno == EPIPE /* || errno == ERRNO_WRITEINCOMPLETE */ )
    {
    debug_printf("transport error %s ignored\n",
      (errno == EPIPE)? "EPIPE" : "WRITEINCOMPLETE");
    }
  else
    {
    addr->transport_return = PANIC;
    addr->basic_errno = errno;
    if (errno == ERRNO_CHHEADER_FAIL)
      addr->message =
        string_sprintf("Failed to expand headers_add or headers_remove: %s",
          expand_string_message);
    else if (errno == ERRNO_FILTER_FAIL)
      addr->message = string_sprintf("Filter process failure");
    else if (errno == ERRNO_WRITEINCOMPLETE)
      addr->message = string_sprintf("Failed repeatedly to write data");
    else
      addr->message = string_sprintf("Error %d", errno);
    return FALSE;
    }
  }

/* Wait for the child process to complete and take action if the returned
status is nonzero. The timeout will be just 1 second if any of the writes
above timed out. */

if ((rc = child_close(pid, timeout)) != 0)
  {
  /* The process did not complete in time; kill its process group and fail
  the delivery. It appears to be necessary to kill the output process too, as
  otherwise it hangs on for some time if the actual pipe process is sleeping.
  (At least, that's what I observed on Solaris 2.5.1.) Since we are failing
  the delivery, that shouldn't cause any problem. */

  if (rc == -256)
    {
    killpg(pid, SIGKILL);
    kill(outpid, SIGKILL);
    addr->transport_return = FAIL;
    addr->message = string_sprintf("pipe delivery process timed out",
      tblock->name);
    }

  /* Wait() failed. */

  else if (rc == -257)
    {
    addr->transport_return = PANIC;
    addr->message = string_sprintf("Wait() failed for child process of %s "
      "transport: %s", tblock->name, strerror(errno));
    }

  /* Either the process completed, but yielded a non-zero (necessarily
  positive) status, or the process was terminated by a signal (rc will contain
  the negation of the signal number). Treat killing by signal as failure unless
  status is being ignored. */

  else if (rc < 0)
    {
    if (!ob->ignore_status)
      {
      addr->transport_return = FAIL;
      addr->message = string_sprintf("Child process of %s transport (running "
        "command \"%s\") was terminated by signal %d (%s)", tblock->name, cmd,
        -rc, os_strsignal(-rc));
      }
    }

  /* For positive values (process terminated with non-zero status), we need a
  status code to request deferral. A number of systems contain the following
  line in sysexits.h:

      #define EX_TEMPFAIL 75

  with the description

      EX_TEMPFAIL -- temporary failure, indicating something that
	 is not really an error.  In sendmail, this means
	 that a mailer (e.g.) could not create a connection,
	 and the request should be reattempted later.

  Based on this, we use exit code EX_TEMPFAIL as a default to mean "defer" when
  not ignoring the returned status. However, there is now an option that
  contains a list of temporary codes, with TEMPFAIL and CANTCREAT as defaults.

  Another case that needs special treatment is if execve() failed (typically
  the command that was given is a non-existent path). By default this is
  treated as just another failure, but if freeze_exec_fail is set, the reaction
  is to freeze the message rather than bounce the address. This failure is
  signalled by EX_UNAVAILABLE which is definined in many systems as

      #define EX_UNAVAILABLE  69

  with the description

      EX_UNAVAILABLE -- A service is unavailable.  This can occur
	    if a support program or file does not exist.  This
	    can also be used as a catchall message when something
	    you wanted to do doesn't work, but you don't know why.

  so this is used by the code in child.c to signal execve() failure and
  other unexpected failures such as setuid() not working - though that won't
  be the case here because we aren't changing uid. */

  else
    {
    /* Always handle execve() failure specially if requested to */

    if (ob->freeze_exec_fail && (rc == EX_UNAVAILABLE))
      {
      addr->transport_return = DEFER;
      addr->special_action = SPECIAL_FREEZE;
      addr->message = string_sprintf("pipe process failed to exec \"%s\"",
        cmd);
      }

    /* Otherwise take action only if not ignoring status */

    else if (!ob->ignore_status)
      {
      char *s = ob->temp_errors;
      char *ss, *p;
      int size, ptr, i;
      int sep = 0;
      char buffer[64];

      addr->transport_return = FAIL;

      /* Scan the list of temporary failure codes; if any match, the
      result becomes DEFER. */

      while ((p = string_nextinlist(&s, &sep, buffer, sizeof(buffer))) != NULL)
        {
        if (rc == atoi(p)) { addr->transport_return = DEFER; break; }
        }

      /* Ensure the message contains the expanded command and arguments. This
      doesn't have to be brilliantly efficient - it is an error situation. */

      addr->message = string_sprintf("Child process of %s transport returned "
        "%d", tblock->name, rc);

      ptr = (int)strlen(addr->message);
      size = ptr + 1;

      /* If the return code is > 128, it often means that a shell command
      was terminated by a signal. */

      ss = (rc > 128)?
        string_sprintf("(could mean shell command ended by signal %d (%s))",
          rc-128, os_strsignal(rc-128)) :
        os_strexit(rc);

      if (*ss != 0)
        {
        addr->message = string_cat(addr->message, &size, &ptr, " ", 1);
        addr->message = string_cat(addr->message, &size, &ptr,
          ss, (int)strlen(ss));
        }

      /* Now add the command and arguments */

      addr->message = string_cat(addr->message, &size, &ptr,
        " from command:", 14);

      for (i = 0; i < sizeof(argv)/sizeof(int *) && argv[i] != NULL; i++)
        {
        BOOL quote = FALSE;
        addr->message = string_cat(addr->message, &size, &ptr, " ", 1);
        if (strpbrk(argv[i], " \t") != NULL)
          {
          quote = TRUE;
          addr->message = string_cat(addr->message, &size, &ptr, "\"", 1);
          }
        addr->message = string_cat(addr->message, &size, &ptr, argv[i],
          (int)strlen(argv[i]));
        if (quote)
          addr->message = string_cat(addr->message, &size, &ptr, "\"", 1);
        }
      addr->message[ptr] = 0;  /* Ensure concatenated string terminated */
      }
    }
  }

/* Ensure all subprocesses (in particular, the output handling process)
are complete before we pass this point. */

while (wait(&rc) >= 0);

DEBUG(2) debug_printf("%s transport yielded %d\n", tblock->name,
  addr->transport_return);

return FALSE;
}

/* End of transport/pipe.c */
