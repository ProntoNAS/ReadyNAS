/* Copyright (C) 19911993 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, write to the Free Software Foundation, Inc., 675 Mass Ave,
   Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <string.h>

extern char *sys_errlist[];
extern int sys_nerr;

/* Return a string describing the errno code in ERRNUM.  */
char *
strerror (int errnum)
{
  static char buf[1024];

  if (errnum < 0 || errnum > sys_nerr)
    {
      sprintf (buf, "Unknown error %d", errnum);
      return buf;
    }

  return (char *) sys_errlist[errnum];
}
