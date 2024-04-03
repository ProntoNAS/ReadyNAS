/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for writing spool files, and moving them about. */


#include "exim.h"



/*************************************************
*       Deal with header writing errors          *
*************************************************/

/* This function is called immediately after errors in writing the spool, with
errno still set. It creates and error message, depending on the circumstances.
If errmsg is NULL, it logs the message and panic-dies. Otherwise errmsg is set
to point to the message, and -1 is returned. This function makes the code of
spool_write_header() a bit neater.

Arguments:
   where      SW_RECEIVING, SW_DELIVERING, or SW_MODIFYING
   errmsg     where to put the message; NULL => panic-die
   s          text to add to log string
   temp_name  name of temp file to unlink
   f          FILE to close, if not NULL

Returns:      -1 if errmsg is not NULL; otherwise doesn't return
*/

static int
spool_write_error(int where, char **errmsg, char *s, char *temp_name, FILE *f)
{
char *msg = (where == SW_RECEIVING)?
  string_sprintf("spool file %s error while receiving from %s: %s", s,
    (sender_fullhost != NULL)? sender_fullhost : sender_ident,
    strerror(errno))
  :
  string_sprintf("spool file %s error while %s: %s", s,
    (where == SW_DELIVERING)? "delivering" : "modifying",
    strerror(errno));

if (temp_name != NULL) unlink(temp_name);
if (f != NULL) fclose(f);

if (errmsg == NULL)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s", msg);
else
  *errmsg = msg;

return -1;
}



/*************************************************
*            Open file under temporary name      *
*************************************************/

/* This is used for opening spool files under a temporary name,
with a single attempt at deleting if they already exist.

Argument: temporary name for spool header file
Returns:  file descriptor of open file, or < 0 on failure, with errno unchanged
*/

int
spool_open_temp(char *temp_name)
{
int fd = open(temp_name, O_RDWR|O_CREAT|O_EXCL, SPOOL_MODE);

/* If the file already exists, something has gone wrong. This process may well
have previously created the file if it is delivering more than one address, but
it should have renamed it almost immediately. A file could, however, be left
around as a result of a system crash, and by coincidence this process might
have the same pid. We therefore have one go at unlinking it before giving up.
*/

if (fd < 0 && errno == EEXIST)
  {
  DEBUG(2) debug_printf("%s exists: unlinking\n", temp_name);
  unlink(temp_name);
  fd = open(temp_name, O_RDWR|O_CREAT|O_EXCL, SPOOL_MODE);
  }

/* If the file has been opened, make sure the file's group is the Exim gid
if exim_uid exists (can't have exim_uid set without exim_gid), and double-check
the mode because the group setting doesn't always get set automatically. */

if (fd >= 0 && exim_uid_set)
  {
  fchown(fd, exim_uid, exim_gid);
  fchmod(fd, SPOOL_MODE);
  }

return fd;
}



/*************************************************
*          Write the header spool file           *
*************************************************/

/* Returns the size of the file for success; zero for failure. The file is
written under a temporary name, and then renamed. It's done this way so that it
works with re-writing the file on message deferral as well as for the initial
write. Whenever this function is called, the data file for the message should
be open and locked, thus preventing any other exim process from working on this
message.

Argument:
  id      the message id
  where   SW_RECEIVING, SW_DELIVERING, or SW_MODIFYING
  errmsg  where to put an error message; if NULL, panic-die on error

Returns:  the size of the header texts on success;
          negative on writing failure, unless errmsg == NULL
*/

int
spool_write_header(char *id, int where, char **errmsg)
{
int fd;
int i;
int size_correction;
FILE *f;
header_line *h;
struct stat statbuf;
char name[256];
char temp_name[256];

sprintf(temp_name, "%s/input/%s/hdr.%d", spool_directory, message_subdir,
  (int)getpid());
fd = spool_open_temp(temp_name);
if (fd < 0) return spool_write_error(where, errmsg, "open", NULL, NULL);
f = fdopen(fd, "w");
DEBUG(9) debug_printf("Writing spool header file\n");

/* We now have an open file to which the header data is to be written. Start
with the file's leaf name, to make the file self-identifying. Continue with the
identity of the submitting user, followed by the sender's address. The sender's
address is enclosed in <> because it might be the null address. Then write the
received time and the number of warning messages that have been sent. */

fprintf(f, "%s-H\n", message_id);
fprintf(f, "%.63s %ld %ld\n", originator_login, (long int)originator_uid,
  (long int)originator_gid);
fprintf(f, "<%s>\n", sender_address);
fprintf(f, "%d %d\n", received_time, warning_count);

/* If there is information about a sending host, remember it. The HELO
data can be set for local SMTP as well as remote. */

if (sender_helo_name != NULL)
  fprintf(f, "-helo_name %s\n", sender_helo_name);

if (sender_host_address != NULL)
  {
  fprintf(f, "-host_address %s.%d\n", sender_host_address, sender_host_port);
  if (sender_host_name != NULL)
    fprintf(f, "-host_name %s\n", sender_host_name);
  if (sender_host_authenticated != NULL)
    fprintf(f, "-host_auth %s\n", sender_host_authenticated);
  }

/* Also about the interface a message came in on */

if (interface_address != NULL)
  fprintf(f, "-interface_address %s\n", interface_address);

/* Likewise for any ident information; for local messages this is
likely to be the same as originator_login, but will be different if
the originator was root, forcing a different ident. */

if (sender_ident != NULL) fprintf(f, "-ident %s\n", sender_ident);

/* Ditto for the received protocol */

if (received_protocol != NULL)
  fprintf(f, "-received_protocol %s\n", received_protocol);

/* Now any other data that needs to be remembered. */

fprintf(f, "-body_linecount %d\n", body_linecount);

if (authenticated_id != NULL)
  fprintf(f, "-auth_id %s\n", authenticated_id);
if (authenticated_sender != NULL)
  fprintf(f, "-auth_sender %s\n", authenticated_sender);
if (deliver_firsttime) fprintf(f, "-deliver_firsttime\n");
if (deliver_freeze) fprintf(f, "-frozen %d\n", deliver_frozen_at);
if (dont_deliver) fprintf(f, "-N\n");
if (host_lookup_failed) fprintf(f, "-host_lookup_failed\n");
if (sender_local) fprintf(f, "-local\n");
if (local_error_message) fprintf(f, "-localerror\n");
if (deliver_manual_thaw) fprintf(f, "-manual_thaw\n");
if (header_names != header_names_normal) fprintf(f, "-resent\n");
if (sender_set_untrusted) fprintf(f, "-sender_set_untrusted\n");

#ifdef SUPPORT_TLS
if (tls_cipher != NULL) fprintf(f, "-tls_cipher %s\n", tls_cipher);
if (tls_peerdn != NULL) fprintf(f, "-tls_peerdn %s\n", tls_peerdn);
#endif

#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is
left here just in case. Nothing is documented. */
if (dsn_envid != NULL) fprintf(f, "-dsn_envid %s\n", dsn_envid);
if (dsn_ret != 0) fprintf(f, "-dsn_ret %d\n", dsn_ret);
#endif

/* To complete the envelope, write out the tree of non-recipients, followed by
the list of recipients. These won't be disjoint the first time, when no
checking has been done. */

tree_write(tree_nonrecipients, f);
fprintf(f, "%d\n", recipients_count);
for (i = 0; i < recipients_count; i++)
  {
  recipient_item *r = recipients_list + i;
  if (r->flags == 0
      #ifdef SUPPORT_DSN
      && r->orcpt == NULL
      #endif
     ) fprintf(f, "%s\n", r->address);

  else
    {
    #ifdef SUPPORT_DSN
    if (r->orcpt != NULL && r->orcpt[0] != 0)
      fprintf(f, "%s %s %d,%d,%d\n", r->address, r->orcpt, r->flags, r->pno,
        (int)strlen(r->orcpt));
    else
    #endif

    fprintf(f, "%s %d,%d,0\n", r->address, r->flags, r->pno);
    }
  }

/* Put a blank line before the headers */

fprintf(f, "\n");

/* Save the size of the file so far so we can subtract it from the final length
to get the actual size of the headers. */

fflush(f);
fstat(fd, &statbuf);
size_correction = statbuf.st_size;

/* Finally, write out the message's headers. To make it easier to read them
in again, precede each one with the count of its length. Make the count fixed
length to aid human eyes when debugging and arrange for it not be included in
the size. It is followed by a space for normal headers, a flagging letter for
various other headers, or an asterisk for old headers that have been rewritten.
These are saved as a record for debugging. Don't included them in the message's
size. */

for (h = header_list; h != NULL; h = h->next)
  {
  fprintf(f, "%03d%c %s", h->slen, h->type, h->text);
  size_correction += 5;
  if (h->type == '*') size_correction += h->slen;
  }

/* Flush and check for any errors while writing */

if (fflush(f) != 0 || ferror(f))
  return spool_write_error(where, errmsg, "write", temp_name, f);

/* Force the file's contents to be written to disc. Note that fflush()
just pushes it out of C, and fclose() doesn't guarantee to do the write
either. That's just the way Unix works... */

if (fsync(fileno(f)) < 0)
  return spool_write_error(where, errmsg, "sync", temp_name, f);

/* Get the size of the file, and close it. */

fstat(fd, &statbuf);
if (fclose(f) != 0)
  return spool_write_error(where, errmsg, "close", temp_name, NULL);

/* Rename the file to its correct name, thereby replacing any previous
incarnation. */

sprintf(name, "%s/input/%s/%s-H", spool_directory, message_subdir, id);

if (rename(temp_name, name) < 0)
  return spool_write_error(where, errmsg, "rename", temp_name, NULL);

/* Return the number of characters in the headers, which is the file size, less
the prelimary stuff, less the additional count fields on the headers. */

DEBUG(2)
  debug_printf("Size of headers = %d\n", statbuf.st_size - size_correction);

return statbuf.st_size - size_correction;
}


#ifdef SUPPORT_MOVE_FROZEN_MESSAGES

/************************************************
*              Make a hard link                 *
************************************************/

/* Used by spool_move_message() below. Note re the use of sprintf(): the value
of spool_directory is checked to ensure that it is less than 200 characters at
start-up time.

Arguments:
  dir        base directory name
  subdir     subdirectory name
  id         message id
  suffix     suffix to add to id
  from       source directory prefix
  to         destination directory prefix
  noentok    if TRUE, absence of file is not an error

Returns:     TRUE if all went well
             FALSE, having panic logged if not
*/

static BOOL
make_link(char *dir, char *subdir, char *id, char *suffix, char *from,
  char *to, BOOL noentok)
{
char f[256], t[256];
sprintf(f, "%s/%s%s/%s/%s%s", spool_directory, from, dir, subdir, id, suffix);
sprintf(t, "%s/%s%s/%s/%s%s", spool_directory, to, dir, subdir, id, suffix);
if (link(f, t) < 0 && (!noentok || errno != ENOENT))
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "link(\"%s\", \"%s\") failed while moving "
    "message: %s", f, t, strerror(errno));
  return FALSE;
  }
return TRUE;
}



/************************************************
*                Break a link                   *
************************************************/

/* Used by spool_move_message() below. Note re the use of sprintf(): the value
of spool_directory is checked to ensure that it is less than 200 characters at
start-up time.

Arguments:
  dir        base directory name
  subdir     subdirectory name
  id         message id
  suffix     suffix to add to id
  from       source directory prefix
  noentok    if TRUE, absence of file is not an error

Returns:     TRUE if all went well
             FALSE, having panic logged if not
*/

static BOOL
break_link(char *dir, char *subdir, char *id, char *suffix, char *from,
  BOOL noentok)
{
char f[256];
sprintf(f, "%s/%s%s/%s/%s%s", spool_directory, from, dir, subdir, id, suffix);
if (unlink(f) < 0 && (!noentok || errno != ENOENT))
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "unlink(\"%s\") failed while moving "
    "message: %s", f, strerror(errno));
  return FALSE;
  }
return TRUE;
}



/************************************************
*	     Move message files			*
************************************************/

/* Move the files for a message (-H, -D, and msglog) from one directory (or
hierarchy) to another. It is assume that there is no -J file in existence when
this is done. At present, this is used only when move_frozen_messages is set,
so compile it only when that support is configured.

Arguments:
  id          the id of the message to be delivered
  subdir      the subdirectory name, or an empty string
  from        a prefix for "input" or "msglog" for where the message is now
  to          a prefix for "input" or "msglog" for where the message is to go

Returns:      TRUE if all is well
              FALSE if not, with error logged in panic and main logs
*/

BOOL
spool_move_message(char *id, char *subdir, char *from, char *to)
{
/* Create any output directories that do not exist. */

sprintf(big_buffer, "%sinput/%s", to, subdir);
(void)directory_make(spool_directory, big_buffer, INPUT_DIRECTORY_MODE, TRUE);
sprintf(big_buffer, "%smsglog/%s", to, subdir);
(void)directory_make(spool_directory, big_buffer, INPUT_DIRECTORY_MODE, TRUE);

/* Move the message by first creating new hard links for all the files, and
then removing the old links. When moving messages onto the main spool, the -H
file should be set up last, because that's the one that tells Exim there is a
message to be delivered, so we create its new link last and remove its old link
first. Programs that look at the alternate directories should follow the same
rule of waiting for a -H file before doing anything. When moving messsages off
the mail spool, the -D file should be open and locked at the time, thus keeping
Exim's hands off. */

if (!make_link("msglog", subdir, id, "", from, to, TRUE) ||
    !make_link("input", subdir, id, "-D", from, to, FALSE) ||
    !make_link("input", subdir, id, "-H", from, to, FALSE))
  return FALSE;

if (!break_link("input", subdir, id, "-H", from, FALSE) ||
    !break_link("input", subdir, id, "-D", from, FALSE) ||
    !break_link("msglog", subdir, id, "", from, TRUE))
  return FALSE;

log_write(0, LOG_MAIN, "moved from %sinput, %smsglog to %sinput, %smsglog",
   from, from, to, to);

return TRUE;
}

#endif

/* End of spool_out.c */
