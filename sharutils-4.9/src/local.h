/*
   Handle so called 'shell archives'.

   Copyright (C) 1994, 1995, 1996, 2002, 2005, 2006, 2007, 2010
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see:  http://www.gnu.org/licenses.
*/

#ifndef SHARUTILS_LOCAL_H
#define SHARUTILS_LOCAL_H 1

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef USE_UNLOCKED_IO
#  define USE_UNLOCKED_IO 1
#endif

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "basename.h"
#include "error.h"
#include "gettext.h"
#include "locale.h"
#include "system.h"
#include "xstrtol.h"
#include "unlocked-io.h"

#ifdef BINARY_MODE_POPEN
static char const freadonly_mode[]  = "rb";
static char const fwriteonly_mode[] = "wb";
#else
static char const freadonly_mode[]  = "r";
static char const fwriteonly_mode[] = "w";
#endif

#endif /* SHARUTILS_LOCAL_H */
