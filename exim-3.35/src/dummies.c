/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* This file is not part of the main Exim code. The Exim monitor and some other
utilities use modules from the main code for certain functions, and those
modules may call other main Exim functions which are inappropriate in a
utility. The classic case is log_write(). This module contains dummy versions
of such functions - well not really dummies, more like alternates. */

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/* We don't have the full Exim headers dragged in, but this function
is used for debugging output. */

extern int  string_vformat(char *, int, char *, va_list);


/*************************************************
*         Handle calls to write the log          *
*************************************************/

/* The message gets written to stderr when log_write() is called from a
utility. The message always gets '\n' added on the end of it.

Arguments:
  flags     not relevant when running a utility
  level     not relevant when running a utility
  format    a printf() format
  ...       arguments for format

Returns:    nothing
*/

void
log_write(int level, int flags, char *format, ...)
{
va_list ap;
va_start(ap, format);
vfprintf(stderr, format, ap);
fprintf(stderr, "\n");
va_end(ap);
level = level;     /* Keep picky compilers happy */
flags = flags;
}


/*************************************************
*      Handle calls to print debug output        *
*************************************************/

/* The message just gets written to stderr

Arguments:
  format    a printf() format
  ...       arguments for format

Returns:    nothing
*/

void
debug_printf(char *format, ...)
{
va_list ap;
char buffer[1024];

va_start(ap, format);

if (!string_vformat(buffer, sizeof(buffer), format, ap))
  {
  char *s = "**** debug string overflowed buffer ****\n";
  char *p = buffer + (int)strlen(buffer);
  int maxlen = sizeof(buffer) - (int)strlen(s) - 3;
  if (p > buffer + maxlen) p = buffer + maxlen;
  if (p > buffer && p[-1] != '\n') *p++ = '\n';
  strcpy(p, s);
  }

fprintf(stderr, "%s", buffer);
fflush(stderr);
va_end(ap);
}



/*************************************************
*              SIGALRM handler                   *
*************************************************/

extern int sigalrm_seen;

void
sigalrm_handler(int sig)
{
sig = sig;            /* Keep picky compilers happy */
sigalrm_seen = 1;
}



/*************************************************
*                  Dummies                       *
*************************************************/

/* These are functions that either aren't called when the Exim functions are
invoked from a utility, or aren't relevant. */

int
header_checkname(void *h, char *name, int len)
{
h = h;            /* Keep picky compilers happy */
name = name;
len = len;
return 0;
}

void
directory_make(char *parent, char *name, int mode, int panic)
{
parent = parent;  /* Keep picky compilers happy */
name = name;
mode = mode;
panic = panic;
}

void
host_build_sender_fullhost(void) { }

/* This one isn't needed for test_host */

#ifndef TEST_HOST
char *
host_ntoa(int type, const void *arg, char *buffer, int *portptr)
{
type = type;      /* Keep picky compilers happy */
arg = arg;
buffer = buffer;
portptr = portptr;
return NULL;
}
#endif


/* End of dummies.c */
