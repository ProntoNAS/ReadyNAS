/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for writing log files. */


#include "exim.h"

#define LOG_NAME_SIZE 256
#define MAX_SYSLOG_LEN 1000

#define LOG_MODE_FILE   1
#define LOG_MODE_SYSLOG 2


/*************************************************
*           Local static variables               *
*************************************************/

static char mainlog_name[LOG_NAME_SIZE];

static int mainlogfd = -1;
static int mainlog_inode = -1;
static int processlogfd = -1;
static int rejectlogfd = -1;

static BOOL panic_recurseflag = FALSE;

static BOOL syslog_open = FALSE;
static BOOL path_inspected = FALSE;
static int  logging_mode = LOG_MODE_FILE;
static char *file_path = "";




/*************************************************
*              Write to syslog                   *
*************************************************/

/* The given string is split into sections according to length, or at embedded
newlines, and syslogged as a numbered sequence if there is more than one line.

Arguments:
  priority       syslog priority
  s              the string to be written

Returns:         nothing
*/

static void
write_syslog(int priority, char *s)
{
int len, pass;
int linecount = 0;

if (!syslog_timestamp) s += 20;
len = (int)strlen(s);

#ifndef NO_OPENLOG
if (!syslog_open)
  {
  openlog("exim", LOG_PID|LOG_CONS, LOG_MAIL);
  syslog_open = TRUE;
  }
#endif

/* First do a scan through the message in order to determine how many lines
it is going to end up as. Then rescan to output it. */

for (pass = 0; pass < 2; pass++)
  {
  int i;
  int tlen;
  char *ss = s;
  for (i = 1, tlen = len; tlen > 0; i++)
    {
    int plen = tlen;
    char *nlptr = strchr(ss, '\n');
    if (nlptr != NULL) plen = nlptr - ss;
    if (plen > MAX_SYSLOG_LEN) plen = MAX_SYSLOG_LEN;

    tlen -= plen;
    if (ss[plen] == '\n') tlen--;    /* chars left */

    if (pass == 0) linecount++; else
      {
      if (linecount == 1)
        syslog(priority, "%.*s", plen, ss);
      else
        syslog(priority, "[%d%c%d] %.*s", i,
          (ss[plen] == '\n' && tlen != 0)? '\\' : '/',
          linecount, plen, ss);
      }
    ss += plen;
    if (*ss == '\n') ss++;
    }
  }
}



/*************************************************
*             Die tidily                         *
*************************************************/

/* This is called when Exim is dying as a result of something going wrong in
the logging, or after a log call with LOG_PANIC_DIE set. Optionally write a
message to debug_file or a stderr file, if they exist. Then, if in the middle
of accepting a message, throw it away tidily; this will attempt to send an SMTP
response if appropriate. Otherwise, try to close down an outstanding SMTP call
tidily.

Arguments:
  s1         Error message to write to debug_file and/or stderr and syslog
  s2         Error message for any SMTP call that is in progress
Returns:     The function does not return
*/

static void
die(char *s1, char *s2)
{
if (s1 != NULL)
  {
  write_syslog(LOG_CRIT, s1);
  if (debug_file != NULL) fprintf(debug_file, "%s\n", s1);
  if (log_stderr != NULL && log_stderr != debug_file)
    fprintf(log_stderr, "%s\n", s1);
  }
if (accept_call_bombout) accept_bomb_out(s2);  /* does not return */
if (smtp_input) smtp_closedown(s2);
exim_exit(EXIT_FAILURE);
}



/*************************************************
*                Open a log file                 *
*************************************************/

/* This function opens one of a number of logs, which all reside in the same
directory, creating the directory if it does not exist and an explicit path for
log files has not been specified. This may be called recursively on failure, in
order to open the panic log.

If exim is configured to avoid running as root wherever possible, the log files
must be owned by the non-privileged user. To ensure this, first try an open
without O_CREAT - most of the time this will succeed. If it fails, try to
create the file, and adjust the owner if necessary.

Arguments:
  fd       where to return the resulting file descriptor
  name     log file basic name ("main", "reject", etc.)

Returns:   nothing
*/

static void
open_log(int *fd, char *name)
{
uid_t euid;
char buffer[LOG_NAME_SIZE];

/* For all except the process log, get the log file name from the spool
directory or file_path. The process "log" is a different kind of beast, and
is always written to spool/exim-process.info. If the name is too long, it is a
total and utter disaster. */

if (! (
   (strcmp(name, "process") == 0)?
   string_format(buffer,sizeof(buffer),"%s/exim-process.info",spool_directory) :
   (file_path[0] == 0)?
   string_format(buffer,sizeof(buffer),"%s/log/%slog", spool_directory, name) :
   string_format(buffer,sizeof(buffer), file_path, name)))
  {
  die("exim: log file path too long: aborting",
    "Logging failure; please try later");
  }

/* Save the name of the main log so that we can stat() it later to see if it
has been cycled, then open the file. The static slot for saving it is the same
size as buffer, and the text has been checked above to fit. */

if (strcmp(name, "main") == 0) strcpy(mainlog_name, buffer);

/* After a successful open, arrange for automatic closure on exec(). */

*fd = open(buffer, O_APPEND|O_WRONLY, LOG_MODE);
if (*fd >= 0)
  {
  fcntl(*fd, F_SETFD, fcntl(*fd, F_GETFD) | FD_CLOEXEC);
  return;
  }

/* Try creating the file */

*fd = open(buffer, O_CREAT|O_APPEND|O_WRONLY, LOG_MODE);

/* If creation failed and LOG_FILE_PATH was undefined, causing file_path to be
an empty string, attempt to build a log directory in the spool directory. This
can also happen for settings such as "log_file_path = syslog : :". */

if (*fd < 0 && errno == ENOENT && file_path[0] == 0)
  {
  (void)directory_make(spool_directory, "log", LOG_DIRECTORY_MODE, TRUE);
  *fd = open(buffer, O_CREAT|O_APPEND|O_WRONLY, LOG_MODE);
  }

/* Creation succeeded; change owner if we are currently root, and ensure
the mode is as specified. Set the close-on-exec flag. */

euid = geteuid();

if (*fd >= 0)
  {
  if (exim_uid_set && euid == root_uid)
    chown(buffer, exim_uid, exim_gid);
  chmod(buffer, LOG_MODE);
  fcntl(*fd, F_SETFD, fcntl(*fd, F_GETFD) | FD_CLOEXEC);
  return;
  }

/* Creation failed. There are some circumstances in which we get here when
the effective uid is not root or exim, which is the problem. (For example, a
non-setuid binary with log_arguments set, called in certain ways.) Rather than
just bombing out, force the log to stderr and carry on if stderr is available.
*/

if (euid != root_uid && (!exim_uid_set || euid != exim_uid) &&
    log_stderr != NULL)
  {
  *fd = fileno(log_stderr);
  return;
  }

/* Otherwise this is a disaster */

log_write(0, LOG_PANIC_DIE, "Cannot open %s log file \"%s\": %s: "
  "euid=%d egid=%d", name, buffer, strerror(errno), euid, getegid());
/* Never returns */
}



/*************************************************
*            Write message to log file           *
*************************************************/

/* Exim can be configured to log to local files, or use syslog, or both. This
is controlled by the setting of log_file_path. The following cases are
recognized:

  log_file_path = ""               write files in the spool/log directory
  log_file_path = "xxx"            write files in the xxx directory
  log_file_path = "syslog"         write to syslog
  log_file_path = "syslog : xxx"   write to syslog and to files (any order)

The one exception to this is messages containing LOG_PROCESS. These are always
written to exim-process.info in the spool directory. They aren't really log
messages in the same sense as the others.

The message always gets '\n' added on the end of it, since more than one
process may be writing to the log at once and we don't want intermingling to
happen in the middle of lines. To be absolutely sure of this we write the data
into a private buffer and then put it out in a single write() call.

The flags determine which log(s) the message is written to, or for syslogging,
which priority to use, and in the case of the panic log, whether the process
should die afterwards.

The variable really_exim is TRUE only when exim is running in privileged state
(i.e. not with a changed configuration or with testing options such as -brw).
If it is not, don't try to write to the log because permission will probably be
denied.

Avoid actually writing to the logs when exim is called with -bv or -bt to
test an address, but take other actions, such as panicing.

In Exim proper, the buffer for building the message is got at start-up, so that
nothing gets done if it can't be got. However, some functions that are also
used in utilities occasionally obey log_write calls in error situations, and it
is simplest to put a single malloc() here rather than put one in each utility.
Malloc is used directly because the store functions may call log_write().

If a message_id exists, we include it after the timestamp.

Arguments:
  flags     each bit indicates some independent action:
              LOG_SENDER      add raw sender to the message
              LOG_RECIPIENTS  add raw recipients list to message
              LOG_CONFIG      add "Exim configuration error:\n  "
              LOG_CONFIG2     add "Exim configuration error for "
              LOG_MAIN        write to main log or syslog LOG_INFO
              LOG_REJECT      write to reject log or syslog LOG_NOTICE
              LOG_PANIC       write to panic log or syslog LOG_ALERT
              LOG_PANIC_DIE   write to panic log or LOG_ALERT and then crash
              LOG_PROCESS     write to process log (always a file)
  level     write to main log or LOG_INFO only if log_level is >= this value
  format    a printf() format
  ...       arguments for format

Returns:    nothing
*/

void
log_write(int level, int flags, char *format, ...)
{
char *ptr;
BOOL testing = verify_only || address_test_mode || host_checking;
int paniclogfd;
va_list ap;

/* If panic_recurseflag is set, we have failed to open the panic log.
This is the ultimate disaster. First try to write the message to a debug file
and/or stderr and also to syslog. Then expire. */

if (panic_recurseflag)
  {
  if (debug_file != NULL) fprintf(debug_file, "%s", log_buffer);
  if (log_stderr != NULL && log_stderr != debug_file)
    fprintf(log_stderr, "%s", log_buffer);
  write_syslog(LOG_CRIT, log_buffer);
  die("exim: could not open panic log - aborting: original error above",
    "Unexpected log failure, please try later");
  }

/* Ensure we have a buffer (see comment above); this should never be obeyed
when running Exim proper, only when running utilities. */

if (log_buffer == NULL)
  {
  log_buffer = (char *)malloc(LOG_BUFFER_SIZE);
  if (log_buffer == NULL)
    {
    fprintf(stderr, "exim: failed to get store for log buffer\n");
    exim_exit(EXIT_FAILURE);
    }
  }

/* If we haven't already done so, inspect the setting of log_file_path to
determine whether to log to files and/or to syslog. Bits in logging_mode
control this, and for file logging, the path must end up in file_path. If that
is NULL (the default) with logging_mode containing LOG_MODE_FILE (the default)
then logs are written in the spool/log directory (except for process log). */

if (!path_inspected)
  {
  BOOL multiple = FALSE;
  if (log_file_path[0] != 0)
    {
    int sep = ':';              /* Fixed separator - outside use */
    char *s;
    char *ss = log_file_path;
    logging_mode = 0;
    while ((s = string_nextinlist(&ss,&sep,log_buffer,LOG_BUFFER_SIZE)) != NULL)
      {
      if (strcmp(s, "syslog") == 0)
        logging_mode |= LOG_MODE_SYSLOG;
      else if ((logging_mode & LOG_MODE_FILE) != 0) multiple = TRUE;
      else
        {
        logging_mode |= LOG_MODE_FILE;
        file_path = string_copy_malloc(s);
        }
      }
    }
  path_inspected = TRUE;

  /* If no modes have been selected, it is a major disaster */

  if (logging_mode == 0)
    die("Neither syslog nor file logging set in log_file_path",
        "Unexpected logging failure");

  /* If more than one file path was given, log a complaint. This recursive call
  should work since we have now set up the routing. */

  if (multiple)
    {
    log_write(0, LOG_MAIN|LOG_PANIC,
      "More than one path given in log_file_path: using %s", file_path);
    }
  }

/* If debugging, show all log entries, but don't show headers. Do it all
in one go so that it doesn't get split when multi-processing. */

DEBUG(1)
  {
  ptr = log_buffer;
  sprintf(ptr, "LOG: %d%s%s%s%s%s\n  ", level,
    ((flags & LOG_MAIN) != 0)?    " MAIN"   : "",
    ((flags & LOG_PANIC) != 0)?   " PANIC"  : "",
    ((flags & LOG_PANIC_DIE) == LOG_PANIC_DIE)? " DIE" : "",
    ((flags & LOG_PROCESS) != 0)? " PROCESS": "",
    ((flags & LOG_REJECT) != 0)?  " REJECT" : "");
  if ((flags & LOG_CONFIG) != 0)
    strcat(ptr, "Exim configuration error\n  ");
  else if ((flags & LOG_CONFIG2) != 0)
  strcat(ptr, "Exim configuration error for ");
  while(*ptr) ptr++;

  va_start(ap, format);
  if (!string_vformat(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer)-1, format, ap))
    strcpy(ptr, "**** log string overflowed log buffer ****\n");
  va_end(ap);

  while(*ptr) ptr++;
  strcat(ptr, "\n");
  fprintf(debug_file, "%s", log_buffer);
  fflush(debug_file);
  }

/* If no log file is specified, we are in a mess. */

if ((flags & (LOG_MAIN|LOG_PANIC|LOG_REJECT|LOG_PROCESS)) == 0)
  log_write(0, LOG_PANIC_DIE, "log_write called with no log flags set");

/* Create the main message in the log buffer, including the message
id except for the process log and when called by a utility. */

ptr = log_buffer;
if (really_exim && (flags & LOG_PROCESS) == 0 && message_id[0] != 0)
  sprintf(ptr, "%s %s ", tod_stamp(tod_log), message_id);
else sprintf(ptr, "%s ", tod_stamp(tod_log));

if ((flags & LOG_CONFIG) != 0)
  strcat(ptr, "Exim configuration error\n  ");
else if ((flags & LOG_CONFIG2) != 0)
  strcat(ptr, "Exim configuration error for ");

while(*ptr) ptr++;

va_start(ap, format);
if (!string_vformat(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer)-1, format, ap))
  strcpy(ptr, "**** log string overflowed log buffer ****\n");
while(*ptr) ptr++;
va_end(ap);

/* Add the raw, unrewritten, sender to the message if required. This is done
this way because it kind of fits with LOG_RECIPIENTS. */

if ((flags & LOG_SENDER) != 0 &&
    ptr < log_buffer + LOG_BUFFER_SIZE - 8 - (int)strlen(raw_sender))
  {
  sprintf(ptr, " from <%s>", raw_sender);
  while (*ptr) ptr++;
  }

/* Add list of recipients to the message if required; the raw list,
before rewriting, was saved in raw_recipients. */

if ((flags & LOG_RECIPIENTS) != 0 && ptr < log_buffer + LOG_BUFFER_SIZE - 6)
  {
  int i;
  sprintf(ptr, " for");
  while (*ptr) ptr++;
  for (i = 0; i < recipients_count; i++)
    {
    char *s = raw_recipients[i];
    if (log_buffer + LOG_BUFFER_SIZE - ptr < (int)strlen(s) + 3) break;
    sprintf(ptr, " %s", s);
    while (*ptr) ptr++;
    }
  }

sprintf(ptr, "\n");
while(*ptr) ptr++;

/* Handle loggable errors when running a utility, or when address testing.
Write to log_stderr unless debugging (when it will already have been written),
or unless there is no log_stderr (expn called from daemon, for example). */

if (!really_exim || testing)
  {
  if (debug_level <= 0 && log_stderr != NULL)
    {
    if (host_checking)
      fprintf(log_stderr, "LOG: %s", log_buffer + 20);  /* no timestamp */
    else
      fprintf(log_stderr, "%s", log_buffer);
    }
  if ((flags & LOG_PANIC_DIE) == LOG_PANIC_DIE) exim_exit(EXIT_FAILURE);
  return;
  }

/* Handle the main log. We know that either syslog or file logging (or both) is
set up. A real file gets left open during reception or delivery once it has
been opened, but we don't want to keep on writing to it for too long after it
has been renamed. Therefore, do a stat() and see if the inode has changed, and
if so, re-open. */

if ((flags & LOG_MAIN) != 0 && log_level >= level)
  {
  if ((logging_mode & LOG_MODE_SYSLOG) != 0)
    {
    write_syslog(LOG_INFO, log_buffer);
    }
  if ((logging_mode & LOG_MODE_FILE) != 0)
    {
    struct stat statbuf;
    if (mainlogfd >= 0)
      {
      if (stat(mainlog_name, &statbuf) < 0 || statbuf.st_ino != mainlog_inode)
        {
        DEBUG(1) debug_printf("Closing main log after inode change\n");
        close(mainlogfd);
        mainlogfd = -1;
        mainlog_inode = -1;
        }
      }
    if (mainlogfd < 0)
      {
      open_log(&mainlogfd, "main");     /* No return on error */
      if (fstat(mainlogfd, &statbuf) >= 0) mainlog_inode = statbuf.st_ino;
      }
    write(mainlogfd, log_buffer, ptr - log_buffer);
    }
  }

/* Handle the log for rejected messages: log recipients and the headers if any,
but watch out for overflowing the buffer. Stick a separator between messages. */

if ((flags & LOG_REJECT) != 0)
  {
  header_line *h;

  if (recipients_count > 0)
    {
    int i;

    /* The first recipient */

    string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer), "Recipients: %s\n",
      recipients_list[0].address);
    while (*ptr) ptr++;

    /* And up to 2 additional recipients */

    for (i = 1; i < recipients_count && i < 3; i++)
      {
      string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer), "    %s\n",
        recipients_list[i].address);
      while (*ptr) ptr++;
      }

    if (i < recipients_count)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer), "    ...\n");
      while (*ptr) ptr++;
      }
    }

  for (h = header_list; h != NULL; h = h->next)
    {
    if (!string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer), "%c %s",
      h->type, h->text))
        break;
    while(*ptr) ptr++;
    }

  while(*ptr) ptr++;

  if ((logging_mode & LOG_MODE_SYSLOG) != 0)
    {
    write_syslog(LOG_NOTICE, log_buffer);
    }

  /* Add a separator line. If the headers etc. have filled up the log buffer,
  they will just have been chopped by string_format(). Ensure there is enough
  space for the separator and insert an indication of chopping if necessary. */

  if ((logging_mode & LOG_MODE_FILE) != 0)
    {
    char *separator = "----------------------------------------"
                      "--------------------------------------\n";
    char *tmsg = "\n*** truncated ***\n";
    int seplen = (int)strlen(separator);
    int spaceleft = LOG_BUFFER_SIZE - (ptr-log_buffer);
    if (spaceleft < seplen + 1)
      {
      spaceleft = seplen + 1;
      ptr = log_buffer + LOG_BUFFER_SIZE - spaceleft;
      strcpy(ptr - (int)strlen(tmsg), tmsg);
      }
    (void)string_format(ptr, spaceleft, separator);
    while(*ptr) ptr++;
    if (rejectlogfd < 0)
      open_log(&rejectlogfd, "reject"); /* No return on error */
    write(rejectlogfd, log_buffer, ptr - log_buffer);
    }
  }


/* Handle the process log file, where exim processes can be made to dump
details of what they are doing by sending them a USR1 signal. Note that
a message id is not automatically added above. This information is always
written to a file - never to syslog. */

if ((flags & LOG_PROCESS) != 0)
  {
  if (processlogfd < 0)
    open_log(&processlogfd, "process");  /* No return on error */
  write(processlogfd, log_buffer, ptr - log_buffer);
  }


/* Handle the panic log, which is not kept open like the others. If it fails to
open, there will be a recursive call to log_write(). We detect this above and
attempt to write to the system log as a last-ditch try at telling somebody. In
all cases, try to write to log_stderr. */

if ((flags & LOG_PANIC) != 0)
  {
  if (log_stderr != NULL && log_stderr != debug_file)
    fprintf(log_stderr, "%s", log_buffer);

  if ((logging_mode & LOG_MODE_SYSLOG) != 0)
    {
    write_syslog(LOG_ALERT, log_buffer);
    }

  if ((logging_mode & LOG_MODE_FILE) != 0)
    {
    panic_recurseflag = TRUE;
    open_log(&paniclogfd, "panic");  /* Won't return on failure */
    panic_recurseflag = FALSE;
    write(paniclogfd, log_buffer, ptr - log_buffer);
    close(paniclogfd);
    }

  /* Give up if the DIE flag is set */

  if ((flags & LOG_PANIC_DIE) != LOG_PANIC)
    die(NULL, "Unexpected failure, please try later");
  }
}



/*************************************************
*            Close any open log files            *
*************************************************/

void
log_close_all(void)
{
if (mainlogfd >= 0)
  { close(mainlogfd); mainlogfd = -1; }
if (processlogfd >= 0)
  { close(processlogfd); processlogfd = -1; }
if (rejectlogfd >= 0)
  { close(rejectlogfd); rejectlogfd = -1; }
closelog();
syslog_open = FALSE;
}

/* End of log.c */
