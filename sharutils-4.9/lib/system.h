/* Shared definitions for GNU shar utilities.

   Copyright (C) 1994, 1995, 1996, 2002, 2007 Free Software Foundation, Inc.

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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#ifndef HAVE_INTMAX_T
#define HAVE_INTMAX_T
typedef long intmax_t;
#endif

#if !HAVE_DECL_STRTOIMAX && !defined strtoimax
intmax_t strtoimax ();
#endif

#ifdef STDC_HEADERS
# include <stdlib.h>
#else
char *getenv ();
#endif

#if HAVE_STRING_H
# include <string.h>
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
#else
# include <strings.h>
#endif
#if !HAVE_MEMCPY
# ifndef memcpy
#  define memcpy(D, S, N) bcopy (S, D, N)
# endif
#endif
#if !HAVE_STRCHR
# ifndef strchr
#  define strchr index
# endif
#endif

/* Other header files.  */

#include <stdio.h>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#include <sys/types.h>

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

/* Get definitions for the file permission bits.  */

#include <sys/stat.h>

#ifdef STAT_MACROS_BROKEN
# undef S_ISDIR
# undef S_ISREG
#endif

#ifndef S_IFMT
# define S_IFMT 0170000
#endif
#if !defined(S_ISDIR) && defined(S_IFDIR)
# define S_ISDIR(Mode) (((Mode) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
# define S_ISREG(Mode) (((Mode) & S_IFMT) == S_IFREG)
#endif

#ifndef S_IRWXU
# define S_IRWXU 0700
#endif
#ifndef S_IRUSR
# define S_IRUSR 0400
#endif
#ifndef S_IWUSR
# define S_IWUSR 0200
#endif
#ifndef S_IXUSR
# define S_IXUSR 0100
#endif

#ifndef S_IRWXG
# define S_IRWXG 0070
#endif
#ifndef S_IRGRP
# define S_IRGRP 0040
#endif
#ifndef S_IWGRP
# define S_IWGRP 0020
#endif
#ifndef S_IXGRP
# define S_IXGRP 0010
#endif

#ifndef S_IRWXO
# define S_IRWXO 0007
#endif
#ifndef S_IROTH
# define S_IROTH 0004
#endif
#ifndef S_IWOTH
# define S_IWOTH 0002
#endif
#ifndef S_IXOTH
# define S_IXOTH 0001
#endif

/* System functions.  Even if we usually avoid declaring them, we cannot
   avoid them all.  */

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

FILE *fdopen ();
long ftell ();
FILE *popen ();

#if ! HAVE_FSEEKO
# ifndef fseeko
#  define fseeko fseek
# endif
# ifndef ftello
#  define ftello ftell
# endif
#endif

#ifndef SEEK_SET
# define SEEK_SET 0
#endif

/* GNU small library functions.  */

#ifndef __P
# if __STDC__ || defined __GNUC__ || defined __SUNPRO_C || defined __cplusplus || __PROTOTYPES
#  define __P(Args) Args
# else
#  define __P(Args) ()
# endif
#endif

#ifndef PARAMS
# if __STDC__ || defined __GNUC__ || defined __SUNPRO_C || defined __cplusplus || __PROTOTYPES
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

/* Global functions of the shar package.  */

void copy_file_encoded __P ((FILE *, FILE *));
char *get_submitter __P ((char *));

/* Debugging the memory allocator.  */

#if WITH_DMALLOC
# define MALLOC_FUNC_CHECK
# include <dmalloc.h>
#endif


/* Some gcc specials.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
# define ATTRIBUTE(list) __attribute__ (list)
#else
# define ATTRIBUTE(list)
#endif

#if __CYGWIN__
# include <fcntl.h>
# include <io.h> /* for setmode */
#endif

#if ! defined(O_BINARY) || (O_BINARY == 0)
# define  FOPEN_READ_BINARY   "r"
# define  FOPEN_WRITE_BINARY  "w"
#else
# define  FOPEN_READ_BINARY   "rb"
# define  FOPEN_WRITE_BINARY  "wb"
#endif
