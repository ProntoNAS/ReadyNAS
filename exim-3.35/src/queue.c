/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions that operate on the input queue. */


#include "exim.h"



/*************************************************
*             Get list of spool files            *
*************************************************/

/* Scan the spool directory and return a list of the relevant file names
therein. Single-character sub-directories are handled as follows:

  If the first argument is > 0, a sub-directory is scanned; the letter is
  taken from the nth entry in subdirs.

  If the first argument is 0, sub-directories are not scanned. However, a
  list of them is returned.

  If the first argument is < 0, sub-directories are scanned for messages,
  and a single, unified list is created. The returned data blocks contain the
  identifying character of the subdirectory, if any. The subdirs vector is
  still required as an argument.

If the randomize argument is TRUE, messages are returned in "randomized" order.
Actually, the order is anything but random, but the algorithm is cheap, and the
point is simply to ensure that the same order doesn't occur every time, in case
a particular message is causing a remote MTA to barf - we would like to try
other messages to that MTA first. If the argument is FALSE, sort the list
according to the file name. This should give the order in which the messages
arrived. It is normally used only for presentation to humans, in which case the
insertion sort that it does is not part of the normal operational code. This
case also has the first argument as NULL, so all messages are included.

However, if the queue_run_in order is set, sorting happens, but for multiple
directories will apply only per-directory.

Arguments:
  subdiroffset   sub-directory character offset, or 0 or -1 (see above)
  subdirs        vector to store list of subdirchars
  subcount       pointer to int in which to store count of subdirs
  randomize      TRUE if the order of the list is to be unpredictable

Returns:         pointer to a chain of queue name items
*/

static queue_filename *
queue_get_spool_list(int subdiroffset, uschar *subdirs, int *subcount,
  BOOL randomize)
{
int i;
int flags = 0;
int resetflags = -1;
int subptr;
queue_filename *yield = NULL;
queue_filename *last = NULL;
struct dirent *ent;
DIR *dd;
char buffer[256];

/* The file names are added onto the start or end of the list according to the
bits of the flags variable. When randomizing, get a collection of bits from the
current time. Use the bottom 16 and just keep re-using them if necessary. */

if (randomize) resetflags = time(NULL) & 0xFFFF;

/* If processing the full queue, or just the top-level, start at the base
directory, and initialize the first subdirectory name (as none). Otherwise,
start at the sub-directory offset. */

if (subdiroffset <= 0)
  {
  i = 0;
  subdirs[0] = 0;
  *subcount = 0;
  }
else i = subdiroffset;

/* Set up prototype for the directory name. */

sprintf(buffer, "%s/input", spool_directory);
subptr = (int)strlen(buffer);
buffer[subptr+2] = 0;               /* terminator for lengthened name */

/* This loop runs at least once, for the main or given directory, and then as
many times as necessary to scan any subdirectories encountered in the main
directory, if they are to be scanned at this time. */

for (; i <= *subcount; i++)
  {
  int count = 0;
  int subdirchar = subdirs[i];      /* 0 for main directory */

  if (subdirchar != 0)
    {
    buffer[subptr] = '/';
    buffer[subptr+1] = subdirchar;
    }

  dd = opendir(buffer);
  if (dd == NULL) continue;

  /* Now scan the directory. */

  while ((ent = readdir(dd)) != NULL)
    {
    char *name = ent->d_name;
    int len = (int)strlen(name);

    /* Count entries */

    count++;

    /* If we find a single alphameric sub-directory in the base directory,
    add it to the list for subsequent scans. */

    if (i == 0 && len == 1 && isalnum((uschar)*name))
      {
      *subcount = *subcount + 1;
      subdirs[*subcount] = *name;
      continue;
      }

    /* Otherwise, if it is a header spool file, add it to the list */

    if (len == SPOOL_NAME_LENGTH &&
        strcmp(name + SPOOL_NAME_LENGTH - 2, "-H") == 0)
      {
      queue_filename *next =
        store_get(sizeof(queue_filename) + (int)strlen(name));
      strcpy(next->text, name);
      next->dir_char = subdirchar;

      /* First item becomes the top and bottom of the list. */

      if (yield == NULL)
        {
        next->next = NULL;
        yield = last = next;
        }

      /* If randomizing, insert at either top or bottom, randomly. This is, I
      argue, faster than doing a sort by allocating a random number to each item,
      and it also saves having to store the number with each item. */

      else if (randomize)
        {
        if (flags == 0) flags = resetflags;
        if ((flags & 1) == 0)
          {
          next->next = yield;
          yield = next;
          }
        else
          {
          next->next = NULL;
          last->next = next;
          last = next;
          }
        flags = flags >> 1;
        }

      /* Otherwise do an insertion sort based on the name. First see if
      it should go before the first item. */

      else if (strcmp(next->text, yield->text) < 0)
        {
        next->next = yield;
        yield = next;
        }

      /* Otherwise find the item it should go after; check the last one
      first, because that will often be the case. */

      else
        {
        queue_filename *this;
        if (strcmp(next->text, last->text) < 0)
          {
          for (this = yield; this != last; this = this->next)
            if (strcmp(next->text, this->next->text) < 0) break;
          }
        else this = last;
        next->next = this->next;
        this->next = next;
        if (this == last) last = next;
        }
      }
    }

  /* Finished with this directory */

  closedir(dd);

  /* If we have just scanned a sub-directory, and it was empty (count == 2
  implies just "." and ".." entries), and Exim is no longer configured to
  use sub-directories, attempt to get rid of it. At the same time, try to
  get rid of any corresponding msglog subdirectory. These are just cosmetic
  tidying actions, so just ignore failures. If we are scanning just a single
  sub-directory, break the loop. */

  if (i != 0)
    {
    if (!split_spool_directory && count <= 2)
      {
      rmdir(buffer);
      sprintf(big_buffer, "%s/msglog/%c", spool_directory, subdirchar);
      rmdir(big_buffer);
      }
    if (subdiroffset > 0) break;    /* Single sub-directory */
    }

  /* If we have just scanned the base directory, and subdiroffset is 0,
  we do not want to continue scanning the sub-directories. */

  else
    {
    if (subdiroffset == 0) break;
    }
  }

/* Pass back the list of file items */

return yield;
}




/*************************************************
*              Perform a queue run               *
*************************************************/

/*The arguments give the messages to start and stop at; NULL means start at the
beginning or stop at the end. If the given start message doesn't exist, we
start at the next lexically greater one, and likewise we stop at the after the
previous lexically lesser one if the given stop message doesn't exist. Because
a queue run can take some time, stat each file before forking, in case it has
been delivered in the meantime by some other means.

The global variables queue_force and queue_local may be set to cause forced
deliveries or local-only deliveries, respectively.

If deliver_selectstring[_sender] is not NULL, skip messages whose recipients do
not contain the string. As this option is typically used when a machine comes
back online, we want to ensure that at least one delivery attempt takes place,
so force the first one. The selecting string can optionally be a regex, or
refer to the sender instead of recipients.

If queue_2stage is set, the queue is scanned twice. The first time, queue_smtp
is set so that routing is done for all messages. Thus in the second run those
that are routed to the same host should go down the same SMTP connection.

Arguments:
  start_id   message id to start at, or NULL for all
  stop_id    message id to end at, or NULL for all
  recurse    TRUE if recursing for 2-stage run

Returns:     nothing
*/

void
queue_run(char *start_id, char *stop_id, BOOL recurse)
{
BOOL force_delivery = queue_run_force || deliver_selectstring != NULL ||
  deliver_selectstring_sender != NULL;
pcre *selectstring_regex = NULL;
pcre *selectstring_regex_sender = NULL;
char *log_detail = NULL;
int subcount = 0;
int i;
uschar subdirs[64];

/* Turn off the command line flag that causes remote processing not to happen,
unless queue_run_local is set, in which case force it. Cancel any specific
remote queue domains. Turn off the flag that causes SMTP deliveries not to
happen, unless doing a 2-stage queue run, when the SMTP flag gets set. Save the
queue_runner's pid and the flag that indicates any deliveries run directly from
this process. Deliveries that are run by handing on TCP/IP channels have
queue_run_pid set, but not queue_running. */

queue_remote = queue_run_local;
queue_remote_domains = NULL;

queue_smtp = queue_2stage;
queue_smtp_domains = NULL;

queue_run_pid = getpid();
queue_running = TRUE;

/* Log the true start of a queue run, and fancy options */

if (!recurse)
  {
  char extras[8];
  char *p = extras;

  if (queue_2stage) *p++ = 'q';
  if (queue_run_force) *p++ = 'f';
  if (deliver_force_thaw) *p++ = 'f';
  if (queue_run_local) *p++ = 'l';
  *p = 0;

  p = big_buffer;
  sprintf(p, "pid=%d", (int)queue_run_pid);
  while (*p != 0) p++;

  if (extras[0] != 0)
    {
    sprintf(p, " -q%s", extras);
    while (*p != 0) p++;
    }

  if (deliver_selectstring != NULL)
    {
    sprintf(p, " -R%s %s", deliver_selectstring_regex? "r" : "",
      deliver_selectstring);
    while (*p != 0) p++;
    }

  if (deliver_selectstring_sender != NULL)
    {
    sprintf(p, " -S%s %s", deliver_selectstring_sender_regex? "r" : "",
      deliver_selectstring_sender);
    while (*p != 0) p++;
    }

  log_detail = string_copy(big_buffer);
  log_write(log_queue_run_level, LOG_MAIN, "Start queue run: %s", log_detail);
  }

/* If deliver_selectstring is a regex, compile it. */

if (deliver_selectstring != NULL && deliver_selectstring_regex)
  selectstring_regex = regex_must_compile(deliver_selectstring, TRUE, FALSE);

if (deliver_selectstring_sender != NULL && deliver_selectstring_sender_regex)
  selectstring_regex_sender =
    regex_must_compile(deliver_selectstring_sender, TRUE, FALSE);

/* If the spool is split into subdirectories, we want to process it one
directory at a time, so as to spread out the directory scanning and the
delivering when there are lots of messages involved, except when
queue_run_in_order is set.

In the random order case, this loop runs once for the main directory (handling
any messages therein), and then repeats for any subdirectories that were found.
When the first argument of queue_get_spool_list() is 0, it scans the top
directory, fills in subdirs, and sets subcount. The order of the directories is
then randomized after the first time through, before they are scanned in
subsqeuent iterations.

When the first argument of queue_get_spool_list() is -1 (for queue_run_in_
order), it scans all directories and makes a single message list. */

for (i  = (queue_run_in_order? -1 : 0);
     i <= (queue_run_in_order? -1 : subcount);
     i++)
  {
  queue_filename *f;
  void *reset_point1 = store_get(0);

  DEBUG(9)
    {
    if (i == 0)
      debug_printf("queue running main directory\n");
    else if (i == -1)
      debug_printf("queue running combined directories\n");
    else
      debug_printf("queue running subdirectory '%c'\n", subdirs[i]);
    }

  for (f = queue_get_spool_list(i, subdirs, &subcount, !queue_run_in_order);
       f != NULL;
       f = f->next)
    {
    pid_t pid;
    int status;
    int pfd[2];
    struct stat statbuf;
    char buffer[256];

    if (stop_id != NULL && strncmp(f->text, stop_id, MESSAGE_ID_LENGTH) > 0)
      continue;
    if (start_id != NULL && strncmp(f->text, start_id, MESSAGE_ID_LENGTH) < 0)
      continue;

    message_subdir[0] = f->dir_char;
    sprintf(buffer, "%s/input/%s/%s", spool_directory, message_subdir, f->text);
    if (stat(buffer, &statbuf) < 0) continue;

    /* Check for a matching address if deliver_selectstring[_sender} is set.
    If so, we do a fully delivery - don't want to omit other addresses since
    their routing might trigger re-writing etc. Ensure the store used in
    reading the header file is scavenged. This logic does mean we have to read
    the header file, and then again when actually delivering, but it's cheaper
    than forking a delivery process for each message even when they aren't all
    going to be delivered. */

    if (deliver_selectstring != NULL || deliver_selectstring_sender != NULL)
      {
      BOOL wanted = TRUE;
      void *reset_point2 = store_get(0);

      if (spool_read_header(f->text, FALSE, TRUE) != spool_read_OK) continue;

      if (deliver_freeze && !deliver_force_thaw)
        {
        log_write(5, LOG_MAIN, "Message is frozen");
        wanted = FALSE;
        }

      /* Sender matching */

      else if (deliver_selectstring_sender != NULL &&
              !(deliver_selectstring_sender_regex?
                (pcre_exec(selectstring_regex_sender, NULL, sender_address,
                  (int)strlen(sender_address), 0, PCRE_EOPT, NULL, 0) >= 0)
                :
                (strstric(sender_address, deliver_selectstring_sender, FALSE)
                  != NULL)))
        {
        DEBUG(9) debug_printf("%s: sender address did not match %s\n",
          f->text, deliver_selectstring_sender);
        wanted = FALSE;
        }

      /* Recipient matching */

      else if (deliver_selectstring != NULL)
        {
        int i;
        for (i = 0; i < recipients_count; i++)
          {
          char *address = recipients_list[i].address;
          if ((deliver_selectstring_regex?
               (pcre_exec(selectstring_regex, NULL, address,
                 (int)strlen(address), 0, PCRE_EOPT, NULL, 0) >= 0)
               :
               (strstric(address, deliver_selectstring, FALSE) != NULL))
              &&
              tree_search_addr(tree_nonrecipients, address, FALSE) == NULL)
            break;
          }

        if (i >= recipients_count)
          {
          DEBUG(9) debug_printf("%s: no recipient address matched %s\n",
            f->text, deliver_selectstring);
          wanted = FALSE;
          }
        }

      store_reset(reset_point2);
      if (!wanted) continue;      /* With next message */
      }

    /* OK, got a message we want to deliver. Create a pipe which will
    serve as a means of detecting when all the processes created by the
    delivery process are finished. This is relevant when the delivery
    process passes one or more SMTP channels on to its own children. The
    pipe gets passed down; by reading on it here we detect when the last
    descendent dies by the unblocking of the read. It's a pity that for
    most of the time the pipe isn't used, but creating a pipe should be
    pretty cheap. */

    if (pipe(pfd) < 0)
      {
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "failed to create pipe in queue "
        "runner process %d: %s", queue_run_pid, strerror(errno));
      }
    queue_run_pipe = pfd[pipe_write];  /* To ensure it gets passed on. */

    /* Make sure it isn't stdin. This seems unlikely, but just to be on the
    safe side... */

    if (queue_run_pipe == 0)
      {
      queue_run_pipe = dup(queue_run_pipe);
      close(0);
      }

    /* Now deliver the message; get the id by cutting the -H off the file
    name. */

    set_process_info("running queue: %s", f->text);
    f->text[SPOOL_NAME_LENGTH-2] = 0;
    if ((pid = fork()) == 0)
      _exit(deliver_message(f->text, force_delivery, FALSE));
    if (pid < 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "fork of delivery process from "
        "queue runner %d failed\n", queue_run_pid);

    /* Close the writing end of the synchronizing pipe in this process,
    then wait for the first level process to terminate. */

    close(pfd[pipe_write]);
    set_process_info("running queue: waiting for %s (%d)", f->text, pid);
    while (wait(&status) != pid);

    /* A successful return means a delivery was attempted; turn off the
    force flag for any subsequent calls unless queue_force is set. */

    if ((status & 0xffff) == 0) force_delivery = queue_run_force;

    /* Otherwise, if the delivery yielded DEFER it means the load average
    was too high to attempt a delivery. Abandon the queue run. A message
    has already been written to the log by the delivery process. */

    else if ((status & 0x00ff) == 0)
      {
      if (((status & 0xff00) >> 8) == DEFER) break;
      }

    /* If the process crashed, tell somebody */

    else
      {
      log_write(0, LOG_MAIN|LOG_PANIC,
        "queue run: process %d crashed with signal %d while delivering %s",
        (int)pid, status & 0x00ff, f->text);
      }

    /* Before continuing, wait till the pipe gets closed at the far end. This
    tells us that any children created by the delivery to re-use any SMTP
    channels have all finished. Since no process actually writes to the pipe,
    the mere fact that read() unblocks is enough. */

    set_process_info("running queue: waiting for children of %d", pid);
    (void)read(pfd[pipe_read], buffer, sizeof(buffer));
    (void)close(pfd[pipe_read]);
    set_process_info("running queue");
    }                                  /* End loop for list of messages */

  store_reset(reset_point1);           /* Scavenge list of messages */

  /* If this was the first time through for random order processing, and
  sub-directories have been found, randomize their order if necessary. */

  if (i == 0 && subcount > 1 && !queue_run_in_order)
    {
    int j;
    for (j = 1; j <= subcount; j++)
      {
      int r = random_number(100);
      if (r >= 50)
        {
        int k = (r % subcount) + 1;
        int x = subdirs[j];
        subdirs[j] = subdirs[k];
        subdirs[k] = x;
        }
      }
    }
  }                                    /* End loop for multiple directories */

/* If queue_2stage is true, we do it all again, with the 2stage flag
turned off. */

if (queue_2stage)
  {
  queue_2stage = FALSE;
  queue_run(start_id, stop_id, TRUE);
  }

/* At top level, if smtp_etrn_serialize is set, remove the serialization
block for this -R text, if any. Then log the end of the run. */

if (!recurse)
  {
  if (smtp_etrn_serialize && deliver_selectstring != NULL)
    transport_end_serialized("etrn-runs", deliver_selectstring);
  log_write(log_queue_run_level, LOG_MAIN, "End queue run: %s", log_detail);
  }
}




/************************************************
*         Count messages on the queue           *
************************************************/

/* Called as a result of -bpc

Arguments:  none
Returns:    nothing
*/

void
queue_count(void)
{
int subcount;
int count = 0;
queue_filename *f = NULL;
uschar subdirs[64];
f = queue_get_spool_list(
        -1,             /* entire queue */
        subdirs,        /* for holding sub list */
        &subcount,      /* for subcount */
        FALSE);         /* not random */
for (; f != NULL; f = f->next) count++;
fprintf(stdout, "%d\n", count);
}



/************************************************
*          List extra deliveries                *
************************************************/

/* This is called from queue_list below to print out all addresses that
have received a message but which were not primary addresses. That is, all
the addresses in the tree of non-recipients that are not primary addresses.
The tree has been scanned and the data field filled in for those that are
primary addresses.

Argument:    points to the tree node
Returns:     nothing
*/

static void queue_list_extras(tree_node *p)
{
if (p->left != NULL) queue_list_extras(p->left);
if (!p->data.val) printf("       +D %s\n", p->name);
if (p->right != NULL) queue_list_extras(p->right);
}



/************************************************
*          List messages on the queue           *
************************************************/

/* Or a given list of messages. In the "all" case, we get a list of file names
as quickly as possible, then scan each one for information to output. If any
disappear while we are processing, just leave them out, but give an error if an
explicit list was given. This function is a top-level function that is obeyed
as a result of the -bp argument. As there may be a lot of messages on the
queue, we must tidy up the store after reading the headers for each one.

Arguments:
   option     0 => list top-level recipients, with "D" for those delivered
              1 => list only undelivered top-level recipients
              2 => as 0, plus any generated delivered recipients
              If 8 is added to any of these values, the queue is listed in
                random order.
   list       => first of any message ids to list
   count      count of message ids; 0 => all

Returns:      nothing
*/

void
queue_list(int option, char **list, int count)
{
int i;
int subcount;
int now = (int)time(NULL);
void *reset_point;
queue_filename *f = NULL;
uschar subdirs[64];

/* If given a list of messages, build a chain containing their ids. */

if (count > 0)
  {
  queue_filename *last = NULL;
  for (i = 0; i < count; i++)
    {
    queue_filename *next =
      store_get(sizeof(queue_filename) + (int)strlen(list[i]) + 2);
    sprintf(next->text, "%s-H", list[i]);
    next->dir_char = '*';
    next->next = NULL;
    if (i == 0) f = next; else last->next = next;
    last = next;
    }
  }

/* Otherwise get a list of the entire queue, in order if necessary. */

else
  f = queue_get_spool_list(
          -1,             /* entire queue */
          subdirs,        /* for holding sub list */
          &subcount,      /* for subcount */
          option >= 8);   /* randomize if required */

if (option >= 8) option -= 8;

/* Now scan the chain and print information, resetting store used
each time. */

reset_point = store_get(0);

for (; f != NULL; f = f->next)
  {
  int rc, save_errno;
  int size = 0;
  BOOL env_read;

  store_reset(reset_point);
  message_size = 0;
  message_subdir[0] = f->dir_char;
  rc = spool_read_header(f->text, FALSE, count <= 0);
  if (rc == spool_read_notopen && errno == ENOENT && count <= 0) continue;
  save_errno = errno;

  env_read = (rc == spool_read_OK || rc == spool_read_hdrerror);

  if (env_read)
    {
    int ptr;
    FILE *jread;
    struct stat statbuf;

    sprintf(big_buffer, "%s/input/%s/%s", spool_directory, message_subdir,
      f->text);
    ptr = (int)strlen(big_buffer)-1;
    big_buffer[ptr] = 'D';

    if (stat(big_buffer, &statbuf) == 0) size = message_size + statbuf.st_size;
    i = (now - received_time)/60;  /* minutes on queue */
    if (i > 90)
      {
      i = (i + 30)/60;
      if (i > 72) printf("%2dd ", (i + 12)/24); else printf("%2dh ", i);
      }
    else printf("%2dm ", i);

    /* Collect delivered addresses from any J file */

    big_buffer[ptr] = 'J';
    jread = fopen(big_buffer, "r");
    if (jread != NULL)
      {
      while (fgets(big_buffer, big_buffer_size, jread) != NULL)
        {
        int n = (int)strlen(big_buffer);
        big_buffer[n-1] = 0;
        tree_add_nonrecipient(big_buffer+1, big_buffer[0] == 'Y');
        }
      fclose(jread);
      }
    }

  fprintf(stdout, "%s ", string_format_size(size, big_buffer));
  for (i = 0; i < 16; i++) fputc(f->text[i], stdout);

  if (env_read && sender_address != NULL)
    {
    printf(" <%s>", sender_address);
    if (sender_set_untrusted) printf(" (%s)", originator_login);
    }

  if (rc != spool_read_OK)
    {
    printf("\n    ");
    if (save_errno == ERRNO_SPOOLFORMAT)
      {
      struct stat statbuf;
      sprintf(big_buffer, "%s/input/%s/%s", spool_directory, message_subdir,
        f->text);
      if (stat(big_buffer, &statbuf) == 0)
        {
        int size = statbuf.st_size;    /* Because might be a long */
        printf("*** spool format error: size=%d ***", size);
        }
      else printf("*** spool format error ***");
      }
    else printf("*** spool read error: %s ***", strerror(save_errno));
    if (rc != spool_read_hdrerror)
      {
      printf("\n\n");
      continue;
      }
    }

  if (deliver_freeze) printf(" *** frozen ***");

  printf("\n");

  if (recipients_list != NULL)
    {
    for (i = 0; i < recipients_count; i++)
      {
      tree_node *delivered =
        tree_search_addr(tree_nonrecipients, recipients_list[i].address, FALSE);
      if (!delivered || option != 1)
        printf("        %s %s\n", (delivered != NULL)? "D":" ",
          recipients_list[i].address);
      if (delivered != NULL) delivered->data.val = TRUE;
      }
    if (option == 2 && tree_nonrecipients != NULL)
      queue_list_extras(tree_nonrecipients);
    printf("\n");
    }
  }
}



/*************************************************
*             Act on a specific message          *
*************************************************/

/* Actions that require a list of addresses make use of
argv/argc/recipients_arg. Other actions do not. This function does its
own authority checking.

Arguments:
  id              id of the message to work on
  action          which action is required (MSG_xxx)
  argv            the original argv for Exim
  argc            the original argc for Exim
  recipients_arg  offset to the list of recipients in argv

Returns:          FALSE if there was any problem
*/

BOOL
queue_action(char *id, int action, char **argv, int argc, int recipients_arg)
{
int i, j;
BOOL yield = TRUE;
BOOL removed = FALSE;
struct passwd *pw;
char *doing = NULL;
char *username;
char *errmsg;
char spoolname[256];

/* Set the global message_id variable, used when re-writing spool files. This
also causes message ids to be added to log messages. */

strcpy(message_id, id);

/* The "actions" that just list the files do not require any locking to be
done. Only admin users may read the spool files. */

if (action >= MSG_SHOW_BODY)
  {
  int fd, i, rc;
  char *subdirectory, *suffix;

  if (!admin_user)
    {
    printf("Permission denied\n");
    return FALSE;
    }

  if (recipients_arg < argc)
    {
    printf("*** Only one message can be listed at once\n");
    return FALSE;
    }

  if (action == MSG_SHOW_BODY)
    {
    subdirectory = "input";
    suffix = "-D";
    }
  else if (action == MSG_SHOW_HEADER)
    {
    subdirectory = "input";
    suffix = "-H";
    }
  else
    {
    subdirectory = "msglog";
    suffix = "";
    }

  for (i = 0; i < 2; i++)
    {
    message_subdir[0] = (split_spool_directory == (i == 0))? id[5] : 0;
    sprintf(spoolname, "%s/%s/%s/%s%s", spool_directory, subdirectory,
      message_subdir, id, suffix);
    fd = open(spoolname, O_RDONLY);
    if (fd >= 0) break;
    if (i == 0) continue;
    printf("Failed to open %s file for %s%s: %s\n", subdirectory, id, suffix,
      strerror(errno));
    return FALSE;
    }

  while((rc = read(fd, big_buffer, big_buffer_size)) > 0)
    write(fileno(stdout), big_buffer, rc);

  close(fd);
  return TRUE;
  }

/* For actions that actually act, open and lock the data file to ensure that no
other process is working on this message. If the file does not exist, continue
only if the action is remove and the user is an admin user, to allow for
tidying up broken states. */

if (!spool_open_datafile(id))
  {
  if (errno == ENOENT)
    {
    yield = FALSE;
    printf("Spool data file for %s does not exist\n", id);
    if (action != MSG_REMOVE || !admin_user) return FALSE;
    printf("Continuing, to ensure all files removed\n");
    }
  else
    {
    if (errno == 0) printf("Message %s is locked\n", id);
      else printf("Couldn't open spool file for %s: %s\n", id,
        strerror(errno));
    return FALSE;
    }
  }

/* Read the spool header file for the message. Again, continue after an
error only in the case of deleting by an administrator. Setting the third
argument false causes it to look both in the main spool directory and in
the appropriate subdirectory, and set message_subdir according to where it
found the message. */

sprintf(spoolname, "%s-H", id);
if (spool_read_header(spoolname, TRUE, FALSE) != spool_read_OK)
  {
  yield = FALSE;
  if (errno != ERRNO_SPOOLFORMAT)
    printf("Spool read error for %s: %s\n", spoolname, strerror(errno));
  else
    printf("Spool format error for %s\n", spoolname);
  if (action != MSG_REMOVE || !admin_user)
    {
    close(deliver_datafile);
    deliver_datafile = -1;
    return FALSE;
    }
  printf("Continuing to ensure all files removed\n");
  }

/* Check that the user running this process is entitled to operate on this
message. Only admin users may freeze/thaw, add/cancel recipients, or otherwise
mess about, but the original sender is permitted to remove a message. That's
why we leave this check until after the headers are read. */

if (!admin_user && (action != MSG_REMOVE || real_uid != originator_uid))
  {
  printf("Permission denied\n");
  close(deliver_datafile);
  deliver_datafile = -1;
  return FALSE;
  }

/* Set up the user name for logging. */

pw = getpwuid(real_uid);
username = (pw != NULL)?
  pw->pw_name : string_sprintf("uid %ld", (long int)real_uid);

/* Take the necessary action. */

printf("Message %s ", id);

switch(action)
  {
  case MSG_FREEZE:
  if (deliver_freeze)
    {
    yield = FALSE;
    printf("is already frozen\n");
    }
  else
    {
    deliver_freeze = TRUE;
    deliver_manual_thaw = FALSE;
    deliver_frozen_at = time(NULL);
    if (spool_write_header(id, SW_MODIFYING, &errmsg) >= 0)
      {
      printf("is now frozen\n");
      log_write(0, LOG_MAIN, "frozen by %s", username);
      }
    else
      {
      yield = FALSE;
      printf("could not be frozen: %s\n", errmsg);
      }
    }
  break;


  case MSG_THAW:
  if (!deliver_freeze)
    {
    yield = FALSE;
    printf("is not frozen\n");
    }
  else
    {
    deliver_freeze = FALSE;
    deliver_manual_thaw = TRUE;
    if (spool_write_header(id, SW_MODIFYING, &errmsg) >= 0)
      {
      printf("is no longer frozen\n");
      log_write(0, LOG_MAIN, "unfrozen by %s", username);
      }
    else
      {
      yield = FALSE;
      printf("could not be unfrozen: %s\n", errmsg);
      }
    }
  break;


  /* We must ensure all files are removed from both the input directory
  and the appropriate subdirectory, to clean up cases when there are odd
  files left lying around in odd places. In the normal case message_subdir
  will have been set correctly by spool_read_header, but as this is a rare
  operation, just run everything twice. */

  case MSG_REMOVE:
  message_subdir[0] = id[5];
  for (j = 0; j < 2; message_subdir[0] = 0, j++)
    {
    sprintf(spoolname, "%s/msglog/%s/%s", spool_directory, message_subdir, id);
    if (unlink(spoolname) < 0)
      {
      if (errno != ENOENT)
        {
        yield = FALSE;
        printf("Error while removing %s: %s\n", spoolname,
          strerror(errno));
        }
      }
    else removed = TRUE;

    for (i = 0; i < 3; i++)
      {
      sprintf(spoolname, "%s/input/%s/%s-%c", spool_directory, message_subdir,
        id, "DHJ"[i]);
      if (unlink(spoolname) < 0)
        {
        if (errno != ENOENT)
          {
          yield = FALSE;
          printf("Error while removing %s: %s\n", spoolname,
            strerror(errno));
          }
        }
      else removed = TRUE;
      }
    }

  /* In the common case, the datafile is open (and locked), so give the
  obvious message. Otherwise be more specific. */

  if (deliver_datafile >= 0) printf("has been removed\n");
    else printf("has been removed or did not exist\n");
  if (removed) log_write(0, LOG_MAIN, "removed by %s", username);
  break;


  case MSG_MARK_ALL_DELIVERED:
  for (i = 0; i < recipients_count; i++)
    {
    tree_add_nonrecipient(recipients_list[i].address, FALSE);
    }
  if (spool_write_header(id, SW_MODIFYING, &errmsg) >= 0)
    {
    printf("has been modified\n");
    for (i = 0; i < recipients_count; i++)
      log_write(0, LOG_MAIN, "address <%s> marked delivered by %s",
        recipients_list[i].address, username);
    }
  else
    {
    yield = FALSE;
    printf("- could not mark all delivered: %s\n", errmsg);
    }
  break;


  case MSG_EDIT_SENDER:
  if (recipients_arg < argc - 1)
    {
    yield = FALSE;
    printf("- only one sender address can be specified\n");
    break;
    }
  doing = "editing sender";
  /* Fall through */

  case MSG_ADD_RECIPIENT:
  if (doing == NULL) doing = "adding recipient";
  /* Fall through */

  case MSG_MARK_DELIVERED:
  if (doing == NULL) doing = "marking as delivered";

  /* Common code for EDIT_SENDER, ADD_RECIPIENT, & MARK_DELIVERED */

  if (recipients_arg >= argc)
    {
    yield = FALSE;
    printf("- error while %s: no address given\n", doing);
    }
  else
    {
    for (; recipients_arg < argc; recipients_arg++)
      {
      int start, end, domain;
      char *errmess;
      char *receiver =
        parse_extract_address(argv[recipients_arg], &errmess, &start, &end,
          &domain, (action == MSG_EDIT_SENDER));

      if (receiver == NULL)
        {
        yield = FALSE;
        printf("- error while %s:\n  bad address %s: %s\n",
          doing, argv[recipients_arg], errmess);
        }
      else if (receiver[0] != 0 && domain == 0)
        {
        yield = FALSE;
        printf("- error while %s:\n  bad address %s: "
          "domain missing\n", doing, argv[recipients_arg]);
        }
      else
        {
        if (action == MSG_ADD_RECIPIENT)
          {
          accept_add_recipient(receiver, NULL, 0, 0);
          log_write(0, LOG_MAIN, "recipient <%s> added by %s",
            receiver, username);
          }
        else if (action == MSG_MARK_DELIVERED)
          {
          for (i = 0; i < recipients_count; i++)
            if (strcmpic(recipients_list[i].address, receiver) == 0) break;
          if (i >= recipients_count)
            {
            printf("- error while %s:\n  %s is not a recipient:"
              " message not updated\n", doing, receiver);
            yield = FALSE;
            }
          else
            {
            tree_add_nonrecipient(recipients_list[i].address, FALSE);
            log_write(0, LOG_MAIN, "address <%s> marked delivered by %s",
              receiver, username);
            }
          }
        else  /* MSG_EDIT_SENDER */
          {
          sender_address = receiver;
          log_write(0, LOG_MAIN, "sender address changed to <%s> by %s",
            receiver, username);
          }
        }
      }


    if (yield)
      {
      if (spool_write_header(id, SW_MODIFYING, &errmsg) >= 0)
        printf("has been modified\n");
      else
        {
        yield = FALSE;
        printf("- while %s: %s\n", doing, errmsg);
        }
      }
    }
  break;


  case MSG_EDIT_BODY:
  if (recipients_arg < argc)
    {
    yield = FALSE;
    printf("- only one message can be edited at once\n");
    }

  /* Make a copy of the body, and let the editor work on that. If the editor
  returns successfully, replace the body with the new text. This is the only
  way to allow the editor to do intermittent saves while preserving the right
  of the human to abort the whole thing. As soon as the rename() is done, the
  message becomes available for some other process to work on, since the new
  file is not locked, but that's OK because the next thing this process does is
  to close the old file anyway. */

  else
    {
    int copy_fd;
    int rc;
    char copyname[256];

    /* Make a temporary file to copy to */

    sprintf(copyname, "%s/input/%s/%s-D-%d", spool_directory, message_subdir,
      id, (int)getpid());
    copy_fd = open(copyname, O_WRONLY|O_CREAT, SPOOL_MODE);
    if (copy_fd < 0)
      {
      printf("not modified: opening copy file failed: %s",
        strerror(errno));
      yield = FALSE;
      break;
      }

    /* Make sure it has the same characteristics as the -D file */

    if (exim_uid_set)
      {
      fchown(copy_fd, exim_uid, exim_gid);
      fchmod(copy_fd, SPOOL_MODE);
      }

    /* Copy the contents of the -D file */

    while((rc = read(deliver_datafile, big_buffer, big_buffer_size)) > 0)
      {
      if (write(copy_fd, big_buffer, rc) != rc)
        {
        printf("not modified: copying failed (write): %s\n",
          strerror(errno));
        yield = FALSE;
        break;
        }
      }

    if (rc < 0)
      {
      printf("not modified: copying failed (read): %s\n",
        strerror(errno));
      yield = FALSE;
      }

    close(copy_fd);

    /* Now call the editor and act according to its yield */

    if (yield)
      {
      sprintf(spoolname, "/bin/sh -c \"${VISUAL:-${EDITOR:-vi}} %s\"",
        copyname);

      if ((system(spoolname) & 0xffff) == 0)
        {
        sprintf(spoolname, "%s/input/%s/%s-D", spool_directory, message_subdir,
          id);
        if ((rc = rename(copyname, spoolname)) == 0)
          {
          printf("has been modified\n");
          log_write(0, LOG_MAIN, "body edited by %s", username);
          }
        else
          {
          printf("not modified: rename failed: %s\n",
            strerror(errno));
          yield = FALSE;
          }
        }
      else
        {
        printf("not modified: editing failed\n");
        yield = FALSE;
        }
      }

    /* Get rid of the copy file if something went wrong */

    if (!yield) unlink(copyname);
    }
  break;
  }


/* Closing the datafile releases the lock and permits other processes
to operate on the message (if it still exists). */

close(deliver_datafile);
deliver_datafile = -1;
return yield;
}



/*************************************************
*       Check the queue_only_file condition      *
*************************************************/

/* The queue_only_file option forces certain kinds of queueing if a given file
exists.

Arguments:  none
Returns:    nothing
*/

void
queue_check_only(void)
{
BOOL *set;
int sep = 0;
struct stat statbuf;
char *s, *ss, *name;
char buffer[1024];

if (queue_only_file == NULL) return;

s = queue_only_file;
while ((ss = string_nextinlist(&s, &sep, buffer, sizeof(buffer))) != NULL)
  {
  if (strncmp(ss, "remote", 6) == 0)
    {
    name = "queue_remote";
    set = &queue_remote;
    ss += 6;
    }
  else if (strncmp(ss, "smtp", 4) == 0)
    {
    name = "queue_smtp";
    set = &queue_smtp;
    ss += 4;
    }
  else
    {
    name = "queue_only";
    set = &queue_only;
    }

  if (stat(ss, &statbuf) == 0)
    {
    *set = TRUE;
    DEBUG(2) debug_printf("%s set because %s exists\n", name, ss);
    }
  }
}

/* End of queue.c */
