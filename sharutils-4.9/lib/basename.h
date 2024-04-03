/* Pathname hacking.
   Copyright (C) 2001-2002, 2007, 2010 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  */

#ifndef _BASENAME_H
#define _BASENAME_H

/* This is where basename() is declared.  */
#include <string.h>

#ifndef PARAMS
# if __STDC__ || defined __GNUC__ || defined __SUNPRO_C || \
     defined __cplusplus || __PROTOTYPES
#  define PARAMS(args) args
# else
#  define PARAMS(args) ()
# endif
#endif

#if !(__GLIBC__ >= 2)
/* When not using the GNU libc we use the basename implementation we
   provide here.  */

extern char *base_name PARAMS ((const char *));
#define basename(Arg) base_name (Arg)

#endif

#endif /* _BASENAME_H */
