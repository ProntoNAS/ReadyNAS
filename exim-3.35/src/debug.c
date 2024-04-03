/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "exim.h"



/*************************************************
*      Expand and print debugging string         *
*************************************************/

/* The string is expanded and written as debugging output. If
expansion fails, a message is written instead.

Argument:    the string
Returns:     nothing
*/

void
debug_print_string(char *debug_string)
{
if (debug_level >= 1 && debug_string != NULL)
  {
  char *s = expand_string(debug_string);
  if (s == NULL)
    debug_printf("failed to expand debug_output \"%s\": %s\n", debug_string,
      expand_string_message);
  else if (s[0] != 0)
    debug_printf("%s%s", s, (s[(int)strlen(s)-1] == '\n')? "" : "\n");
  }
}



/*************************************************
*      Print current uids and gids               *
*************************************************/

/*
Argument:   an introductory string
Returns:    nothing
*/

void
debug_print_ids(char *s)
{
debug_printf("%s uid=%ld gid=%ld euid=%ld egid=%ld\n", s,
  (long int)getuid(), (long int)getgid(), (long int)geteuid(),
  (long int)getegid());
}




/*************************************************
*           Print debugging message              *
*************************************************/

/* There are two entries, one for use when being called directly from a
function with a variable argument list.

If debug_pid is nonzero, print the pid at the start of each message and add
a newline if there isn't one. This is for tidier output when running parallel
remote deliveries with debugging turned on. Must do the whole thing with a
single printf and flush, as otherwise output may get interleaved. */

void
debug_printf(char *format, ...)
{
va_list ap;
va_start(ap, format);
debug_vprintf(format, ap);
va_end(ap);
}

void
debug_vprintf(char *format, va_list ap)
{
char *ptr;
char buffer[1024];

if (debug_file == NULL) return;

ptr = buffer;

/* Set up prefix if outputting for host checking and not debugging */

if (host_checking && debug_level <= 0)
  {
  strcpy(buffer, ">>> ");
  ptr += 4;
  }

/* Set up the pid if required */

if (debug_pid > 0)
  {
  sprintf(ptr, "%6d ", (int)debug_pid);
  while(*ptr) ptr++;
  }

/* Use the checked formatting routine to ensure that the buffer
does not overflow. Ensure there's space for a newline at the end. */

if (!string_vformat(ptr, sizeof(buffer) - (ptr - buffer) - 1, format, ap))
  {
  char *s = "**** debug string overflowed buffer ****\n";
  char *p = buffer + (int)strlen(buffer);
  int maxlen = sizeof(buffer) - (int)strlen(s) - 3;
  if (p > buffer + maxlen) p = buffer + maxlen;
  if (p > buffer && p[-1] != '\n') *p++ = '\n';
  strcpy(p, s);
  }

if (debug_pid > 0)
  {
  while(*ptr) ptr++;
  if (ptr > buffer && ptr[-1] != '\n') strcat(ptr, "\n");
  }

fprintf(debug_file, "%s", buffer);
fflush(debug_file);
}

/* End of debug.c */
