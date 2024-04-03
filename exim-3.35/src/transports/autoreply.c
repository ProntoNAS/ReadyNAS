/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "../exim.h"
#include "autoreply.h"



/* Options specific to the autoreply transport. They must be in alphabetic
order (note that "_" comes before the lower case letters). Those starting
with "*" are not settable by the user but are used by the option-reading
software for alternative value types. Some options are publicly visible and so
are stored in the driver instance block. These are flagged with opt_public. */

optionlist autoreply_transport_options[] = {
  { "*expand_group",     opt_stringptr | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, expand_gid)) },
  { "*expand_user",      opt_stringptr | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, expand_uid)) },
  { "*set_group",         opt_bool | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, gid_set)) },
  { "*set_user",          opt_bool | opt_hidden | opt_public,
      (void *)(offsetof(transport_instance, uid_set)) },
  { "bcc",               opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, bcc)) },
  { "cc",                opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, cc)) },
  { "file",              opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, file)) },
  { "file_expand",     opt_bool,
      (void *)(offsetof(autoreply_transport_options_block, file_expand)) },
  { "file_optional",     opt_bool,
      (void *)(offsetof(autoreply_transport_options_block, file_optional)) },
  { "from",              opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, from)) },
  { "group",             opt_expand_gid | opt_public,
      (void *)(offsetof(transport_instance, gid)) },
  { "headers",           opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, headers)) },
  { "initgroups",        opt_bool | opt_public,
      (void *)(offsetof(transport_instance, initgroups)) },
  { "log",               opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, logfile)) },
  { "mode",              opt_octint,
      (void *)(offsetof(autoreply_transport_options_block, mode)) },
  { "once",              opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, oncelog)) },
  { "once_file_size",    opt_int,
      (void *)(offsetof(autoreply_transport_options_block, once_file_size)) },
  { "once_repeat",       opt_time,
      (void *)(offsetof(autoreply_transport_options_block, once_repeat)) },
  { "reply_to",          opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, reply_to)) },
  { "return_message",    opt_bool,
      (void *)(offsetof(autoreply_transport_options_block, return_message)) },
  { "subject",           opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, subject)) },
  { "text",              opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, text)) },
  { "to",                opt_stringptr,
      (void *)(offsetof(autoreply_transport_options_block, to)) },
  { "user",              opt_expand_uid | opt_public,
      (void *)(offsetof(transport_instance, uid)) },
};

/* Size of the options list. An extern variable has to be used so that its
address can appear in the tables drtables.c. */

int autoreply_transport_options_count =
  sizeof(autoreply_transport_options)/sizeof(optionlist);

/* Default private options block for the autoreply transport. */

autoreply_transport_options_block autoreply_transport_option_defaults = {
  NULL,           /* from */
  NULL,           /* reply_to */
  NULL,           /* to */
  NULL,           /* cc */
  NULL,           /* bcc */
  NULL,           /* subject */
  NULL,           /* headers */
  NULL,           /* text */
  NULL,           /* file */
  NULL,           /* logfile */
  NULL,           /* oncelog */
  0600,           /* mode */
  0,              /* once_file_size */
  0,              /* once_repeat */
  FALSE,          /* file_expand */
  FALSE,          /* file_optional */
  FALSE           /* return message */
};



/*************************************************
*          Initialization entry point            *
*************************************************/

/* Called for each instance, after its options have been read, to
enable consistency checks to be done, or anything else that needs
to be set up. */

void
autoreply_transport_init(transport_instance *tblock)
{
/*
autoreply_transport_options_block *ob =
  (autoreply_transport_options_block *)(tblock->options_block);
*/

/* If a fixed uid field is set, then a gid field must also be set. */

if (tblock->uid_set && !tblock->gid_set && tblock->expand_gid == NULL)
  log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
    "user set without group for the %s transport", tblock->name);
}




/*************************************************
*          Expand string and check               *
*************************************************/

/* If the expansion fails, the error is set up in the address. Expanded
strings must be checked to ensure they contain only printing characters
and white space. If not, the function fails.

Arguments:
   s         string to expand
   addr      address that is being worked on
   name      transport name, for error text
   text      if TRUE, don't check for non-printing characters

Returns:     expanded string if expansion succeeds;
             NULL otherwise
*/

static char *
checkexpand(char *s, address_item *addr, char *name, BOOL text)
{
char *t;
char *ss = expand_string(s);

if (ss == NULL)
  {
  addr->transport_return = FAIL;
  addr->message = string_sprintf("Expansion of \"%s\" failed in %s transport: "
    "%s", s, name, expand_string_message);
  return NULL;
  }

if (!text) for (t = ss; *t != 0; t++)
  {
  int c = (uschar)*t;
  if (!mac_isprint(c) && c != '\t')
    {
    s = string_printing(s);
    addr->transport_return = FAIL;
    addr->message = string_sprintf("Expansion of \"%s\" in %s transport "
      "contains non-printing character %d", s, name, c);
    return NULL;
    }
  }

return ss;
}




/*************************************************
*              Main entry point                  *
*************************************************/

/* See local README for interface details. This transport always returns
FALSE, indicating that the top address has the status for all - though in fact
this transport can handle only one address at at time anyway. */

BOOL
autoreply_transport_entry(
  transport_instance *tblock,      /* data for this instantiation */
  address_item *addr)              /* address we are working on */
{
int fd, pid, rc;
int cache_fd = -1;
int log_fd = -1;
int cache_size = 0;
int add_size = 0;
EXIM_DB *dbm_file = NULL;
BOOL file_expand, return_message;
char *from, *reply_to, *to, *cc, *bcc, *subject, *headers, *text, *file;
char *logfile, *oncelog;
char *cache_buff = NULL;
char *cache_time = NULL;
header_line *h;
time_t now = time(NULL);
time_t once_repeat;
FILE *f;
FILE *ff = NULL;

autoreply_transport_options_block *ob =
  (autoreply_transport_options_block *)(tblock->options_block);

DEBUG(2) debug_printf("%s transport entered\n", tblock->name);

/* Set up for the good case */

addr->transport_return = OK;
addr->basic_errno = 0;

/* If the address is pointing to a reply block, then take all the data
from that block. It has typically been set up by a mail filter processing
director. Otherwise, the data must be supplied by this transport, and
it has to be expanded here. */

if (addr->reply != NULL)
  {
  DEBUG(9) debug_printf("taking data from address\n");
  from = addr->reply->from;
  reply_to = addr->reply->reply_to;
  to = addr->reply->to;
  cc = addr->reply->cc;
  bcc = addr->reply->bcc;
  subject = addr->reply->subject;
  headers = addr->reply->headers;
  text = addr->reply->text;
  file = addr->reply->file;
  logfile = addr->reply->logfile;
  oncelog = addr->reply->oncelog;
  once_repeat = addr->reply->once_repeat;
  file_expand = addr->reply->file_expand;
  expand_forbid_exists = addr->reply->forbid_exists;
  expand_forbid_lookup = addr->reply->forbid_lookup;
  expand_forbid_perl = addr->reply->forbid_perl;
  return_message = addr->reply->return_message;
  }
else
  {
  DEBUG(9) debug_printf("taking data from transport\n");
  from = ob->from;
  reply_to = ob->reply_to;
  to = ob->to;
  cc = ob->cc;
  bcc = ob->bcc;
  subject = ob->subject;
  headers = ob->headers;
  text = ob->text;
  file = ob->file;
  logfile = ob->logfile;
  oncelog = ob->oncelog;
  once_repeat = ob->once_repeat;
  file_expand = ob->file_expand;
  return_message = ob->return_message;

  if ((from  != NULL &&
        (from = checkexpand(from, addr, tblock->name, FALSE)) == NULL) ||
      (reply_to    != NULL &&
        (reply_to = checkexpand(reply_to, addr, tblock->name, FALSE)) == NULL) ||
      (to    != NULL &&
        (to = checkexpand(to, addr, tblock->name, FALSE)) == NULL) ||
      (cc    != NULL &&
        (cc = checkexpand(cc, addr, tblock->name, FALSE)) == NULL) ||
      (bcc   != NULL &&
        (bcc = checkexpand(bcc, addr, tblock->name, FALSE)) == NULL) ||
      (subject   != NULL &&
        (subject = checkexpand(subject, addr, tblock->name, FALSE)) == NULL) ||
      (headers != NULL &&
        (headers = checkexpand(headers, addr, tblock->name, TRUE)) == NULL) ||
      (text  != NULL &&
        (text = checkexpand(text, addr, tblock->name, TRUE)) == NULL) ||
      (file  != NULL &&
        (file = checkexpand(file, addr, tblock->name, FALSE)) == NULL) ||
      (logfile != NULL &&
        (logfile = checkexpand(logfile, addr, tblock->name, FALSE)) == NULL) ||
      (oncelog != NULL &&
        (oncelog = checkexpand(oncelog, addr, tblock->name, FALSE)) == NULL))
    return FALSE;
  }


/* If the -N option is set, can't do any more. */

if (dont_deliver)
  {
  debug_printf("*** delivery by %s transport bypassed by -N option\n",
    tblock->name);
  return FALSE;
  }


/* If the oncelog field is set, we send want to send only one message to the
given recipient(s). This works only on the "To" field. If there is no "To"
field, the message is always sent. If the To: field contains more than one
recipient, the effect might not be quite as envisaged. If once_file_size is
set, instead of a dbm file, we use a regular file containing a circular buffer
recipient cache. */

if (oncelog != NULL && to != NULL)
  {
  time_t then = 0;

  /* Handle fixed-size cache file. */

  if (ob->once_file_size > 0)
    {
    char *p;
    struct stat statbuf;
    cache_fd = open(oncelog, O_CREAT|O_RDWR, ob->mode);

    if (cache_fd < 0 || fstat(cache_fd, &statbuf) != 0)
      {
      addr->transport_return = DEFER;
      addr->message = string_sprintf("Failed to %s \"once\" file %s when "
        "sending message from %s transport: %s",
        (cache_fd < 0)? "open" : "stat", oncelog, tblock->name,
          strerror(errno));
      goto END_OFF;
      }

    /* Get store in the temporary pool and read the entire file into it. We get
    an amount of store that is big enough to add the new entry on the end if we
    need to do that. */

    cache_size = statbuf.st_size;
    add_size = sizeof(time_t) + (int)strlen(to) + 1;
    store_pool = POOL_TEMP;
    cache_buff = store_get(cache_size + add_size);
    store_pool = POOL_MAIN;

    if (read(cache_fd, cache_buff, cache_size) != cache_size)
      {
      addr->transport_return = DEFER;
      addr->basic_errno = errno;
      addr->message = "error while reading \"once\" file";
      goto END_OFF;
      }

    DEBUG(2) debug_printf("%d bytes read from %s\n", cache_size, oncelog);

    /* Scan the data for this recipient. Each entry in the file starts with
    a time_t sized time value, followed by the address, followed by a binary
    zero. If we find a match, put the time into "then", and the place where it
    was found into "cache_time". Otherwise, "then" is left at zero. */

    p = cache_buff;
    while (p < cache_buff + cache_size)
      {
      char *s = p + sizeof(time_t);
      char *nextp = s + (int)strlen(s) + 1;
      if (strcmp(to, s) == 0)
        {
        memcpy(&then, p, sizeof(time_t));
        cache_time = p;
        break;
        }
      p = nextp;
      }
    }

  /* Use a DBM file for the list of previous recipients. */

  else
    {
    EXIM_DATUM key_datum, result_datum;
    EXIM_DBOPEN(oncelog, O_RDWR|O_CREAT, ob->mode, &dbm_file);
    if (dbm_file == NULL)
      {
      addr->transport_return = DEFER;
      addr->message = string_sprintf("Failed to open %s file %s when sending "
        "message from %s transport: %s", EXIM_DBTYPE, oncelog, tblock->name,
        strerror(errno));
      goto END_OFF;
      }

    EXIM_DATUM_INIT(key_datum);        /* Some DBM libraries need datums */
    EXIM_DATUM_INIT(result_datum);     /* to be cleared */
    EXIM_DATUM_DATA(key_datum) = to;
    EXIM_DATUM_SIZE(key_datum) = (int)strlen(to) + 1;

    if (EXIM_DBGET(dbm_file, key_datum, result_datum))
      {
      /* If the datum size is that of a binary time, we are in the new world
      where messages are sent periodically. Otherwise the file is an old one,
      where the datum was filled with a tod_log time, which is assumed to be
      different in size. For that, only one message is ever sent. This change
      introduced at Exim 3.00. In a couple of years' time the test on the size
      can be abolished. */

      if (EXIM_DATUM_SIZE(result_datum) == sizeof(time_t))
        {
        memcpy(&then, EXIM_DATUM_DATA(result_datum), sizeof(time_t));
        }
      else then = now;
      }
    }

  /* Either "then" is set zero, if no message has yet been sent, or it
  is set to the time of the last sending. */

  if (then != 0 && (once_repeat <= 0 || now - then < once_repeat))
    {
    DEBUG(9) debug_printf("message previously sent to %s%s\n", to,
      (once_repeat > 0)? " and repeat time not reached" : "");
    log_fd = open(logfile, O_WRONLY|O_APPEND|O_CREAT, ob->mode);
    if (log_fd >= 0)
      {
      char *ptr = log_buffer;
      sprintf(ptr, "%s\n  previously sent to %.200s\n", tod_stamp(tod_log), to);
      while(*ptr) ptr++;
      write(log_fd, log_buffer, ptr - log_buffer);
      close(log_fd);
      }
    goto END_OFF;
    }

  DEBUG(9) debug_printf("%s %s\n", (then <= 0)?
    "no previous message sent to" : "repeat time reached for", to);
  }

/* We are going to send a message. Ensure any requested file is available. */

if (file != NULL)
  {
  ff = fopen(file, "r");
  if (ff == NULL && !ob->file_optional)
    {
    addr->transport_return = DEFER;
    addr->message = string_sprintf("Failed to open file %s when sending "
      "message from %s transport: %s", file, tblock->name, strerror(errno));
    return FALSE;
    }
  }

/* Make a subprocess to send the message */

pid = child_open_exim(&fd);

/* Creation of child failed; defer this delivery. */

if (pid < 0)
  {
  addr->transport_return = DEFER;
  addr->message = string_sprintf("Failed to create child process to send "
    "message from %s transport: %s", tblock->name, strerror(errno));
  DEBUG(2) debug_printf("%s\n", addr->message);
  return FALSE;
  }

/* Create the message to be sent - recipients are taken from the headers,
as the -t option is used. The "headers" stuff *must* be last in case there
are newlines in it which might, if placed earlier, screw up other headers. */

f = fdopen(fd, "w");

if (from != NULL) fprintf(f, "From: %s\n", from);
if (reply_to != NULL) fprintf(f, "Reply-To: %s\n", reply_to);
if (to != NULL) fprintf(f, "To: %s\n", to);
if (cc != NULL) fprintf(f, "Cc: %s\n", cc);
if (bcc != NULL) fprintf(f, "Bcc: %s\n", bcc);
if (subject != NULL) fprintf(f, "Subject: %s\n", subject);

/* Generate In-Reply-To from the message_id header; there should
always be one, but code defensively. */

for (h = header_list; h != NULL; h = h->next)
  if (h->type == htype_id) break;

if (h != NULL)
  {
  char *s = strchr(h->text, ':') + 1;
  while (isspace((uschar)*s)) s++;
  fprintf(f, "In-Reply-To: %s", s);
  }

/* Add any specially requested headers */

if (headers != NULL) fprintf(f, "%s\n", headers);
fprintf(f, "\n");

if (text != NULL)
  {
  fprintf(f, "%s", text);
  if (text[(int)strlen(text)-1] != '\n') fprintf(f, "\n");
  }

if (ff != NULL)
  {
  while (fgets(big_buffer, big_buffer_size, ff) != NULL)
    {
    if (file_expand)
      {
      char *s = expand_string(big_buffer);
      DEBUG(9)
        {
        if (s == NULL)
          debug_printf("error while expanding line from file:\n  %s\n  %s\n",
            big_buffer, expand_string_message);
        }
      fprintf(f, "%s", (s == NULL)? big_buffer : s);
      }
    else fprintf(f, "%s", big_buffer);
    }
  }

/* Copy the original message if required, observing the return size
limit. */

if (return_message)
  {
  if (return_size_limit > 0)
    {
    struct stat statbuf;
    int max = (return_size_limit/DELIVER_IN_BUFFER_SIZE + 1) *
      DELIVER_IN_BUFFER_SIZE;
    if (fstat(deliver_datafile, &statbuf) == 0 && statbuf.st_size > max)
      {
      int size = statbuf.st_size;  /* Because might be a long */
      fprintf(f, "\n"
"------ This is a copy of the message, including all the headers.\n"
"------ The body of the message is %d characters long; only the first\n"
"------ %d or so are included here.\n\n", size, (max/1000)*1000);
      }
    else fprintf(f, "\n"
"------ This is a copy of the message, including all the headers. ------\n\n");
    }
  else fprintf(f, "\n"
"------ This is a copy of the message, including all the headers. ------\n\n");

  fflush(f);
  transport_write_message(addr, fileno(f),
    (tblock->body_only? topt_no_headers : 0) |
    (tblock->headers_only? topt_no_body : 0) |
    (tblock->return_path_add? topt_add_return_path : 0) |
    (tblock->delivery_date_add? topt_add_delivery_date : 0) |
    (tblock->envelope_to_add? topt_add_envelope_to : 0),
    return_size_limit, tblock->add_headers, tblock->remove_headers,
    NULL, NULL, tblock->rewrite_rules, tblock->rewrite_existflags);
  }

/* End the message and wait for the child process to end; no timeout. */

fclose(f);
rc = child_close(pid, 0);

/* Update the "sent to" log whatever the yield. This errs on the side of
missing out a message rather than risking sending more than one. We either have
cache_fd set to a fixed size, circular buffer file, or dbm_file set to an open
DBM file (or neither, if "once" is not set). */

/* Update fixed-size cache file. If cache_time is set, we found a previous
entry; that is the spot into which to put the current time. Otherwise we have
to add a new record; remove the first one in the file if the file is too big.
We always rewrite the entire file in a single write operation. This is
(hopefully) going to be the safest thing because there is no interlocking
between multiple simultaneous deliveries. */

if (cache_fd >= 0)
  {
  char *from = cache_buff;
  int size = cache_size;
  (void)lseek(cache_fd, 0, SEEK_SET);

  if (cache_time == NULL)
    {
    cache_time = from + size;
    memcpy(cache_time + sizeof(time_t), to, add_size - sizeof(time_t));
    size += add_size;

    if (cache_size > 0 && size > ob->once_file_size)
      {
      from += sizeof(time_t) + (int)strlen(from + sizeof(time_t)) + 1;
      size -= (from - cache_buff);
      }
    }

  memcpy(cache_time, &now, sizeof(time_t));
  write(cache_fd, from, size);
  }

/* Update DBM file */

else if (dbm_file != NULL)
  {
  EXIM_DATUM key_datum, value_datum;
  EXIM_DATUM_INIT(key_datum);          /* Some DBM libraries need to have */
  EXIM_DATUM_INIT(value_datum);        /* cleared datums. */
  EXIM_DATUM_DATA(key_datum) = to;
  EXIM_DATUM_SIZE(key_datum) = (int)strlen(to) + 1;

  /* Many OS define the datum value, sensibly, as a void *. However, there
  are some which still have char *. By casting this address to a char * we
  can avoid warning messages from the char * systems. */

  EXIM_DATUM_DATA(value_datum) = (char *)(&now);
  EXIM_DATUM_SIZE(value_datum) = (int)sizeof(time_t);
  EXIM_DBPUT(dbm_file, key_datum, value_datum);
  }

/* If sending failed, defer to try again - but if once is set the next
try will skip, of course. However, if there were no recipients in the
message, we do not fail. */

if (rc != 0)
  {
  if (rc == EXIT_NORECIPIENTS)
    {
    DEBUG(1) debug_printf("%s transport: message contained no recipients\n",
      tblock->name);
    }
  else
    {
    addr->transport_return = DEFER;
    addr->message = string_sprintf("Failed to send message from %s "
      "transport (%d)", tblock->name, rc);
    goto END_OFF;
    }
  }

/* Log the sending of the message if successful and required. If the file
fails to open, it's hard to know what to do. We cannot write to the Exim
log from here, since we may be running under an unprivileged uid. We don't
want to fail the delivery, since the message has been successfully sent. For
the moment, ignore open failures. Write the log entry as a single write() to a
file opened for appending, in order to avoid interleaving of output from
different processes. The log_buffer can be used exactly as for main log
writing. */

if (logfile != NULL)
  {
  int log_fd = open(logfile, O_WRONLY|O_APPEND|O_CREAT, ob->mode);
  if (log_fd >= 0)
    {
    char *ptr = log_buffer;
    DEBUG(9) debug_printf("logging message details\n");
    sprintf(ptr, "%s\n", tod_stamp(tod_log));
    while(*ptr) ptr++;
    if (from != NULL)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer),
        "  From: %s\n", from);
      while(*ptr) ptr++;
      }
    if (to != NULL)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer),
        "  To: %s\n", to);
      while(*ptr) ptr++;
      }
    if (cc != NULL)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer),
        "  Cc: %s\n", cc);
      while(*ptr) ptr++;
      }
    if (bcc != NULL)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer),
        "  Bcc: %s\n", bcc);
      while(*ptr) ptr++;
      }
    if (subject != NULL)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer),
        "  Subject: %s\n", subject);
      while(*ptr) ptr++;
      }
    if (headers != NULL)
      {
      (void)string_format(ptr, LOG_BUFFER_SIZE - (ptr-log_buffer),
        "  %s\n", headers);
      while(*ptr) ptr++;
      }
    write(log_fd, log_buffer, ptr - log_buffer);
    close(log_fd);
    }
  else DEBUG(2) debug_printf("Failed to open log file %s for %s "
    "transport: %s\n", logfile, tblock->name, strerror(errno));
  }


END_OFF:
if (cache_buff != NULL)
  {
  store_pool = POOL_TEMP;
  store_reset(cache_buff);
  store_pool = POOL_MAIN;
  }

if (dbm_file != NULL) EXIM_DBCLOSE(dbm_file);
if (cache_fd > 0) close(cache_fd);

DEBUG(2) debug_printf("%s transport succeeded\n", tblock->name);

return FALSE;
}

/* End of transport/autoreply.c */
