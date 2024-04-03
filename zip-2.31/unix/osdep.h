/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2004-May-22 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#include <sys/types.h>
#include <sys/stat.h>


#ifdef __CYGWIN__

/* File operations:
 *   use "b" for binary;
 *   use "S" for sequential access on NT to prevent the NT file cache
 *   eating up memory with large .zip files.
 */
#  define FOPR "rb"
#  define FOPM "r+b"
#  define FOPW "wbS"

/* Cygwin is not Win32. */
#  undef WIN32

#endif /* ?__CYGWIN__ */


/* Enable the "UT" extra field (time info) */
#if !defined(NO_EF_UT_TIME) && !defined(USE_EF_UT_TIME)
#  define USE_EF_UT_TIME
#endif
