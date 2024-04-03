/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Functions for reading spool files. */


#include "exim.h"



/*************************************************
*           Open and lock data file              *
*************************************************/

/* The data file is the one that is used for locking, because the header file
can get replaced during delivery because of header rewriting. The file has
to opened with write access so that we can get an exclusive lock, but in
fact it won't be written to. Just in case there's a major disaster (e.g.
overwriting some other file descriptor with the value of this one), open it
with append.

Argument: the id of the message
Returns:  TRUE if file successfully opened and locked

Side effect: deliver_datafile is set to the fd of the open file.
*/

BOOL
spool_open_datafile(char *id)
{
int i;
struct stat statbuf;
flock_t lock_data;
char spoolname[256];

/* If split_spool_directory is set, first look for the file in the appropriate
sub-directory of the input directory. If it is not found there, try the input
directory itself, to pick up leftovers from before the splitting. If split_
spool_directory is not set, first look in the main input directory. If it is
not found there, try the split sub-directory, in case it is left over from a
splitting state. */

for (i = 0; i < 2; i++)
  {
  int save_errno;
  message_subdir[0] = (split_spool_directory == (i == 0))? id[5] : 0;
  sprintf(spoolname, "%s/input/%s/%s-D", spool_directory, message_subdir, id);
  deliver_datafile = open(spoolname, O_RDWR | O_APPEND);
  if (deliver_datafile >= 0) break;
  save_errno = errno;
  if (errno == ENOENT)
    {
    if (i == 0) continue;
    if (!queue_running)
      log_write(0, LOG_MAIN, "Spool file %s-D not found", id);
    }
  else log_write(0, LOG_MAIN, "Spool error for %s: %s", spoolname,
    strerror(errno));
  errno = save_errno;
  return FALSE;
  }

/* File is open and message_subdir is set. Set the close-on-exec flag, and lock
the file. */

fcntl(deliver_datafile, F_SETFD, fcntl(deliver_datafile, F_GETFD) |
  FD_CLOEXEC);

lock_data.l_type = F_WRLCK;
lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;

if (fcntl(deliver_datafile, F_SETLK, &lock_data) < 0)
  {
  log_write(5, LOG_MAIN,
    "Spool file is locked (another process is handling this message)");
  close(deliver_datafile);
  deliver_datafile = -1;
  errno = 0;
  return FALSE;
  }

/* Get the size of the data; don't include the leading filename line
in the count, but add one for the newline before the data. */

if (fstat(deliver_datafile, &statbuf) == 0)
  {
  message_body_size = statbuf.st_size - MESSAGE_ID_LENGTH - 3;
  message_size = message_body_size + 1;
  }

return TRUE;
}



/*************************************************
*    Read non-recipients tree from spool file    *
*************************************************/

/* The tree of non-recipients is written to the spool file in a form that
makes it easy to read back into a tree. The format is as follows:

   . Each node is preceded by two letter(Y/N) indicating whether it has left
     or right children. There's one space after the two flags, before the name.

   . The left subtree (if any) then follows, then the right subtree (if any).

This function is entered with the next input line in the buffer. Note we must
save the right flag before recursing with the same buffer.

Once the tree is read, we re-construct the balance fields by scanning the tree.
I forgot to write them out originally, and the compatible fix is to do it this
way. This initial local recursing function does the necessary.

Arguments:
  node      tree node

Returns:    maximum depth below the node, including the node itself
*/

static int
count_below(tree_node *node)
{
int nleft, nright;
if (node == NULL) return 0;
nleft = count_below(node->left);
nright = count_below(node->right);
node->balance = (nleft > nright)? 1 : ((nright > nleft)? 2 : 0);
return 1 + ((nleft > nright)? nleft : nright);
}

/* This is the real function...

Arguments:
  connect      pointer to the root of the tree
  f            FILE to read data from
  buffer       contains next input line; further lines read into it
  buffer_size  size of the buffer

Returns:       FALSE on format error
*/

static BOOL
read_nonrecipients_tree(tree_node **connect, FILE *f, char *buffer,
  int buffer_size)
{
tree_node *node;
int n = (int)strlen(buffer);
BOOL right = buffer[1] == 'Y';

if (n < 5) return FALSE;    /* malformed line */
buffer[n-1] = 0;            /* Remove \n */
node = store_get(sizeof(tree_node) + n - 3);
*connect = node;
strcpy(node->name, buffer + 3);
node->data.ptr = NULL;

if (buffer[0] == 'Y')
  {
  if (fgets(buffer, buffer_size, f) == NULL ||
    !read_nonrecipients_tree(&node->left, f, buffer, buffer_size))
      return FALSE;
  }
else node->left = NULL;

if (right)
  {
  if (fgets(buffer, buffer_size, f) == NULL ||
    !read_nonrecipients_tree(&node->right, f, buffer, buffer_size))
      return FALSE;
  }
else node->right = NULL;

(void) count_below(*connect);
return TRUE;
}




/*************************************************
*             Read spool header file             *
*************************************************/

/* This function reads a spool header file and places the data into the
appropriate global variables. The header portion is always read, but header
structures are built only if read_headers is set true. It isn't, for example,
while generating -bp output.

It may be possible for blocks of nulls (binary zeroes) to get written on the
end of a file if there is a system crash during writing. It was observed on an
earlier version of Exim that omitted to fsync() the files - this is thought to
have been the cause of that incident, but in any case, this code must be robust
against such an event, and if such a file is encountered, it must be treated as
malformed.

Arguments:
  name          name of the header file, including the -H
  read_headers  TRUE if in-store header structures are to be built
  subdir_set    TRUE is message_subdir is already set

Returns:        spool_read_OK        success
                spool_read_notopen   open failed
                spool_read_enverror  error in the envelope portion
                spool_read_hdrdrror  error in the header portion
*/

int
spool_read_header(char *name, BOOL read_headers, BOOL subdir_set)
{
FILE *f;
int n;
long int uid, gid;
BOOL inheader = FALSE;
char originator[64];

/* Ensure all pointers to store that will be obtained are NULL, just
in case. */

authenticated_id = NULL;
authenticated_sender = NULL;
header_list = header_last = NULL;
sender_address = NULL;
sender_host_address = NULL;
sender_host_name = NULL;
sender_host_port = 0;
sender_host_authenticated = NULL;
interface_address = NULL;
sender_helo_name = NULL;
sender_fullhost = NULL;
sender_ident = NULL;
recipients_list = NULL;
tree_nonrecipients = NULL;

#ifdef SUPPORT_TLS
tls_cipher = NULL;
tls_peerdn = NULL;
#endif

/* Generate the full name and open the file. If message_subdir is already
set, just look in the given directory. Otherwise, look in both the split
and unsplit directories, as for the data file above. */

for (n = 0; n < 2; n++)
  {
  if (!subdir_set)
    message_subdir[0] = (split_spool_directory == (n == 0))? name[5] : 0;
  sprintf(big_buffer, "%s/input/%s/%s", spool_directory, message_subdir, name);
  f = fopen(big_buffer, "r");
  if (f != NULL) break;
  if (n != 0 || subdir_set || errno != ENOENT) return spool_read_notopen;
  }

errno = 0;

DEBUG(5) debug_printf("Opened spool file %s\n", name);

/* The first line of a spool file contains the message id followed by -H (i.e.
the file name), in order to make the file self-identifying. */

if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
if ((int)strlen(big_buffer) != MESSAGE_ID_LENGTH + 3 ||
    strncmp(big_buffer, name, MESSAGE_ID_LENGTH + 2) != 0)
  goto SPOOL_FORMAT_ERROR;

/* The next three lines in the header file are in a fixed format. The first
contains the login, uid, and gid of the user who caused the file to be written.
The second contains the mail address of the message's sender, enclosed in <>.
The third contains the time the message was received, and the number of warning
messages for delivery delays that have been sent. */

if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;

if (sscanf(big_buffer, "%s %ld %ld", originator, &uid, &gid) != 3)
  goto SPOOL_FORMAT_ERROR;
originator_login = string_copy(originator);
originator_uid = (uid_t)uid;
originator_gid = (gid_t)gid;

if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
n = (int)strlen(big_buffer);
if (n < 3 || big_buffer[0] != '<' || big_buffer[n-2] != '>')
  goto SPOOL_FORMAT_ERROR;

sender_address = store_get(n-2);
strncpy(sender_address, big_buffer+1, n-3);
sender_address[n-3] = 0;

if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
if (sscanf(big_buffer, "%d %d", &received_time, &warning_count) != 2)
  goto SPOOL_FORMAT_ERROR;

message_age = time(NULL) - received_time;

DEBUG(5) debug_printf("user=%s uid=%ld gid=%ld sender=%s\n",
  originator_login, (long int)originator_uid, (long int)originator_gid,
  sender_address);

/* Now there may be a number of optional lines, each starting with "-".
First set default values for the options. */

sender_local = FALSE;
header_names = header_names_normal;
body_linecount = 0;
message_linecount = 0;
deliver_freeze = FALSE;
deliver_frozen_at = 0;
host_lookup_failed = FALSE;
local_error_message = FALSE;
sender_set_untrusted = FALSE;

#ifdef SUPPORT_DSN
/* This partial attempt at doing DSN was abandoned. This obsolete code is
left here just in case. Nothing is documented. */
dsn_ret = 0;
dsn_envid = NULL;
#endif

for (;;)
  {
  if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
  if (big_buffer[0] != '-') break;

  big_buffer[(int)strlen(big_buffer) - 1] = 0;
  if (strcmp(big_buffer, "-local") == 0) sender_local = TRUE;
    else if (strcmp(big_buffer, "-localerror") == 0)
      local_error_message = TRUE;
    else if (strcmp(big_buffer, "-host_lookup_failed") == 0)
      host_lookup_failed = TRUE;
    else if (strcmp(big_buffer, "-resent") == 0)
      header_names = header_names_resent;
    else if (strncmp(big_buffer, "-body_linecount", 15) == 0)
      body_linecount = atoi(big_buffer + 15);
    else if (strncmp(big_buffer, "-frozen", 7) == 0)
      {
      deliver_freeze = TRUE;
      deliver_frozen_at = atoi(big_buffer + 7);
      }
    else if (strcmp(big_buffer, "-deliver_firsttime") == 0)
      deliver_firsttime = TRUE;
    else if (strcmp(big_buffer, "-manual_thaw") == 0)
      deliver_manual_thaw = TRUE;
    else if (strncmp(big_buffer, "-auth_id", 8) == 0)
      authenticated_id = string_copy(big_buffer + 9);
    else if (strncmp(big_buffer, "-auth_sender", 12) == 0)
      authenticated_sender = string_copy(big_buffer + 13);
    else if (strncmp(big_buffer, "-sender_set_untrusted", 21) == 0)
      sender_set_untrusted = TRUE;

    #ifdef SUPPORT_TLS
    else if (strncmp(big_buffer, "-tls_cipher", 11) == 0)
      tls_cipher = string_copy(big_buffer + 12);
    else if (strncmp(big_buffer, "-tls_peerdn", 11) == 0)
      tls_peerdn = string_copy(big_buffer + 12);
    #endif

    #ifdef SUPPORT_DSN
    /* This partial attempt at doing DSN was abandoned. This obsolete code is
    left here just in case. Nothing is documented. */
    else if (strncmp(big_buffer, "-dsn_ret", 8) == 0)
      dsn_ret= atoi(big_buffer + 8);

    else if (strncmp(big_buffer, "-dsn_envid", 10) == 0)
      dsn_envid = string_copy(big_buffer + 11);
    #endif

    /* We now record the port number after the address, separated by a
    dot. For compatibility during upgrading, do nothing if there
    isn't a value (it gets left at zero). */

    else if (strncmp(big_buffer, "-host_address", 13) == 0)
      {
      sender_host_port = host_extract_port(big_buffer + 14);
      sender_host_address = string_copy(big_buffer + 14);
      }

    else if (strncmp(big_buffer, "-host_auth", 10) == 0)
      sender_host_authenticated = string_copy(big_buffer + 11);
    else if (strncmp(big_buffer, "-host_name", 10) == 0)
      sender_host_name = string_copy(big_buffer + 11);
    else if (strncmp(big_buffer, "-interface_address", 18) == 0)
      interface_address = string_copy(big_buffer + 19);
    else if (strncmp(big_buffer, "-helo_name", 10) == 0)
      sender_helo_name = string_copy(big_buffer + 11);
    else if (strncmp(big_buffer, "-ident", 6) == 0)
      sender_ident = string_copy(big_buffer + 7);
    else if (strncmp(big_buffer, "-received_protocol", 18) == 0)
      received_protocol = string_copy(big_buffer + 19);
    else if (strncmp(big_buffer, "-N", 2) == 0)
      dont_deliver = TRUE;

  /* To allow new versions of Exim that add additional flags to interwork
  with older versions that do not understand them, just ignore any flagged
  lines that we don't recognize. Otherwise it wouldn't be possible to back
  off a new version that left new-style flags written on the spool. That's
  why the following line is commented out. */

    /* else goto SPOOL_FORMAT_ERROR; */
  }

/* Build sender_fullhost if required */

host_build_sender_fullhost();

DEBUG(5)
  debug_printf("sender_local=%d resent=%s ident=%s\n", sender_local,
    (header_names == header_names_normal)? "no" : "yes",
    (sender_ident == NULL)? "unset" : sender_ident);

/* We now have the tree of addresses NOT to deliver to, or a line
containing "XX", indicating no tree. */

if (strncmp(big_buffer, "XX\n", 3) != 0 &&
  !read_nonrecipients_tree(&tree_nonrecipients, f, big_buffer, big_buffer_size))
    goto SPOOL_FORMAT_ERROR;

DEBUG(5)
  {
  debug_printf("Non-recipients:\n");
  tree_print(tree_nonrecipients, debug_file);
  }

/* After reading the tree, the next line has not yet been read into the
buffer. It contains the count of recipients which follow on separate lines. */

if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
if (sscanf(big_buffer, "%d", &recipients_count) != 1) goto SPOOL_FORMAT_ERROR;

DEBUG(5) debug_printf("recipients_count=%d\n", recipients_count);

recipients_list_max = recipients_count;
recipients_list = store_get(recipients_count * sizeof(recipient_item));

for (n = 0; n < recipients_count; n++)
  {
  int nn;
  int flags = 0;
  int pno = 0;
  char *orcpt = NULL;
  char *p;

  if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
  nn = (int)strlen(big_buffer);
  if (nn < 2) goto SPOOL_FORMAT_ERROR;

  /* Look back from the end for digits/commas and a space, indicating the
  presence of flags, pno data, and possibly the DSN orcpt field.
  The partial attempt at doing DSN was abandoned. This obsolete code is
  left here just in case. Nothing is documented.

  Jan, 2002: Added code to make it read the Exim 4 version of this, which
  is just a single number.
  */

  p = big_buffer + nn - 2;
  while (isdigit((uschar)*p) || *p == ',') p--;
  if (*p == ' ')
    {
    int orc;
    *p++ = 0;
    if (sscanf(p, "%d,%d,%d", &flags, &pno, &orc) == 1)
      {
      pno = flags;                 /* Convert Exim 4 one-number */
      flags = rf_onetime;          /* to Exim 3 setting plus a  */
      orc = 0;                     /* bit in the flags */
      }
    if (orc > 0)
      {
      p -= orc + 1;
      p[-1] = 0;
      orcpt = string_copy(p);
      }
    }
  else big_buffer[nn-1] = 0;

  recipients_list[n].address = string_copy(big_buffer);
  recipients_list[n].flags = flags;
  recipients_list[n].pno = pno;

  #ifdef SUPPORT_DSN
  /* This partial attempt at doing DSN was abandoned. This obsolete code is
  left here just in case. Nothing is documented. */
  recipients_list[n].orcpt = orcpt;
  #else
  orcpt = orcpt;   /* Keep picky compilers happy */
  #endif
  }

/* The remainder of the spool header file contains the headers for the message,
separated off from the previous data by a blank line. Each header is preceded
by a count of its length and either a certain letter (for various identified
headers), space (for a miscellaneous live header) or an asterisk (for a header
that has been rewritten). Count the Received: headers. We read the headers
always, in order to check on the format of the file, but only create a header
list if requested to do so. */

inheader = TRUE;
if (fgets(big_buffer, big_buffer_size, f) == NULL) goto SPOOL_READ_ERROR;
if (big_buffer[0] != '\n') goto SPOOL_FORMAT_ERROR;

while ((n = fgetc(f)) != EOF)
  {
  header_line *h;
  uschar flag[4];
  int i;

  if (!isdigit(n)) goto SPOOL_FORMAT_ERROR;
  ungetc(n, f);
  fscanf(f, "%d%c ", &n, flag);
  if (!mac_isprint(flag[0])) goto SPOOL_FORMAT_ERROR;
  if (flag[0] != '*') message_size += n;  /* Omit non-transmitted headers */

  if (read_headers)
    {
    h = store_get(sizeof(header_line));
    h->next = NULL;
    h->type = flag[0];
    h->slen = n;
    h->text = store_get(n+1);

    if (h->type == htype_received) received_count++;

    if (header_list == NULL) header_list = h;
      else header_last->next = h;
    header_last = h;

    for (i = 0; i < n; i++)
      {
      int c = fgetc(f);
      if (c == 0 || c == EOF) goto SPOOL_FORMAT_ERROR;
      if (c == '\n' && h->type != htype_old) message_linecount++;
      h->text[i] = c;
      }
    h->text[i] = 0;

    /* Make the Precedence value available in a variable. This isn't
    a standard RFC 822 header, but it is in widespread use. */

    if (header_checkname(h, "Precedence", 10))
      {
      char *s = h->text + 10;
      while (isspace((uschar)*s)) s++;
      s++;
      while (isspace((uschar)*s)) s++;
      message_precedence = string_copy(s);
      }
    }

  /* Not requiring header data, just skip through the bytes */

  else for (i = 0; i < n; i++)
    {
    int c = fgetc(f);
    if (c == 0 || c == EOF) goto SPOOL_FORMAT_ERROR;
    }
  }

/* We have successfully read the data in the header file. Update the message
line count by adding the body linecount to the header linecount. Close the file
and give a positive response. */

DEBUG(5) debug_printf("body_linecount=%d message_linecount=%d\n",
  body_linecount, message_linecount);

message_linecount += body_linecount;

fclose(f);
return spool_read_OK;


/* There was an error reading the spool or there was missing data,
or there was a format error. A "read error" with no errno means an
unexpected EOF, which we treat as a format error. */

SPOOL_READ_ERROR:
if (errno != 0)
  {
  n = errno;
  DEBUG(1) debug_printf("Error while reading spool file %s\n", name);
  fclose(f);
  errno = n;
  return inheader? spool_read_hdrerror : spool_read_enverror;
  }

SPOOL_FORMAT_ERROR:
DEBUG(1) debug_printf("Format error in spool file %s\n", name);
fclose(f);
errno = ERRNO_SPOOLFORMAT;
return inheader? spool_read_hdrerror : spool_read_enverror;
}

/* End of spool_in.c */
