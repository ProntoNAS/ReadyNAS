/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include "exim.h"


/*************************************************
*           Attempt to create a directory        *
*************************************************/

/* All the directories that Exim ever creates for itself are within the spool
directory as defined by spool_directory. We are prepared to create as many as
necessary from that directory downwards, inclusive. However, directory creation
can also be required in appendfile. The making function therefore has a parent
argument, below which the new directories are to go. It can be NULL if the name
is absolute.

If a non-root uid has been specified for exim, and we are currently running as
root, ensure the directory is owned by the non-root id if the parent is the
spool directory.

Arguments:
  parent    parent directory name; if NULL the name must be absolute
  name      directory name within the parent that we want
  mode      mode for the new directory
  panic     if TRUE, panic on failure

Returns:    panic on failure if panic is set; otherwise return FALSE;
            TRUE on success.
*/

BOOL
directory_make(char *parent, char *name, int mode, BOOL panic)
{
BOOL use_chown =
  parent == spool_directory && exim_uid_set && geteuid() == root_uid;
char *p, *slash;
int c = 1;
struct stat statbuf;
char buffer[256];

if (parent == NULL)
  {
  p = buffer + 1;
  slash = parent = "";
  }
else
  {
  p = buffer + (int)strlen(parent);
  slash = "/";
  }

if (!string_format(buffer, sizeof(buffer), "%s%s%s", parent, slash, name))
  log_write(0, LOG_PANIC_DIE, "name too long in directory_make");

while (c != 0 && *p != 0)
  {
  while (*p != 0 && *p != '/') p++;
  c = *p;
  *p = 0;
  if (stat(buffer, &statbuf) != 0)
    {
    if (mkdir(buffer, mode) < 0 && errno != EEXIST)
      {
      if (!panic) return FALSE;
      log_write(0, LOG_PANIC_DIE,
        "Failed to create directory \"%s\": %s\n", buffer, strerror(errno));
      }

    /* Set the ownership if necessary. */

    if (use_chown) chown(buffer, exim_uid, exim_gid);

    /* It appears that any mode bits greater than 0777 are ignored by
    mkdir(), at least on some operating systems. Therefore, if the mode
    contains any such bits, do an explicit mode setting. */

    if ((mode & 0777000) != 0) chmod(buffer, mode);
    }
  *p++ = c;
  }

return TRUE;
}

/* End of directory.c */
