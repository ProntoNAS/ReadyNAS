/* A C wrapper to ldconfig for libreadline2_2.1-2.1 and later (?) */
/* Copyright (C) 1997, James Troup <jjtroup@comp.brad.ac.uk> */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

int doc_link(void)
{
  struct stat buf;
  int rv;

  /* [ -d /usr/doc ] */
  if (lstat("/usr/doc", &buf) == -1)
    return 1;
  if (!S_ISDIR(buf.st_mode))
    return 0;

  /* [ -d /usr/share/doc/<pkg> ] */
  if (stat("/usr/share/doc/" PKG_NAME, &buf) == -1)
    return 1;
  if (!S_ISDIR(buf.st_mode))
    return 0;

  /* [ ! -e /usr/doc/<pkg> ] */
  if (stat("/usr/doc/" PKG_NAME, &buf) == -1) {
    if (errno != ENOENT)
      return 1;
  }
  else
    return 0;

  /* link it */
  unlink("/usr/doc/" PKG_NAME);
  errno = 0;
  rv = symlink("../share/doc/" PKG_NAME, "/usr/doc/" PKG_NAME);
  return rv;
}

int
main (int argc, char **argv)
{
  if (argv[1] && !strcmp(argv[1], "configure")) {

    if (doc_link()) {
      fprintf(stderr, "Error: %s\n", strerror (errno));
      fprintf(stderr, "Symbolic link (/usr/doc --> /usr/share/doc) not created\n");
    }
    execlp("ldconfig", "ldconfig", 0);
    fprintf(stderr, "Error: exec ldconfig: %s\n", strerror (errno));
    fprintf(stderr, "Run ldconfig by hand, or bash might be unusable !\n");
    exit(1);
  }
  exit(0);
}
