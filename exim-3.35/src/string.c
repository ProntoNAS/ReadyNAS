/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Miscellaneous string-handling functions */


#include "exim.h"

static char  *hex_digits = "0123456789abcdef";


/*************************************************
*            Test for IP address                 *
*************************************************/

/* This used just to be a regular expression, but with IPv6 things are a bit
more complicated. If the address contains a colon, it is assumed to be a v6
address (assuming HAVE_IPV6 is set). If a mask is permitted and one is present,
the '/' in the input gets changed to 0, and maskptr points to its offset.

Arguments:
  s         a string
  maskptr   NULL if no mask is permitted to follow
            otherwise, points to an int where the '/' offset is put, or
            zero if there is no mask given

Returns:    0 if the string is not a textual representation of an IP address
            4 if it is an IPv4 address
            6 if it is an IPv6 address
*/

int
string_is_ip_address(char *s, int *maskptr)
{
int i;
int yield = 4;

/* If an optional mask is permitted, check for it. If found, change the / to
a zero, and pass back the offset where this was done. */

if (maskptr != NULL)
  {
  char *ss = s + (int)strlen(s);
  *maskptr = 0;
  if (s != ss && isdigit((uschar)*(--ss)))
    {
    while (ss > s && isdigit((uschar)ss[-1])) ss--;
    if (ss > s && *(--ss) == '/')
      {
      *ss = 0;
      *maskptr = ss - s;
      }
    }
  }

/* A colon anywhere in the string => IPv6 address */

#if HAVE_IPV6

if (strchr(s, ':') != NULL)
  {
  BOOL had_double_colon = FALSE;
  BOOL v4end = FALSE;
  int count = 0;

  yield = 6;

  /* An IPv6 address must start with hex digit or double colon. A single
  colon is invalid. */

  if (*s == ':' && *(++s) != ':') return 0;

  /* Now read up to 8 components consisting of up to 4 hex digits each. There
  may be one and only one appearance of double colon, which implies any number
  of binary zero bits. The number of preceding components is held in count. */

  for (count = 0; count < 8; count++)
    {
    /* If the end of the string is reached before reading 8 components, the
    address is valid provided a double colon has been read. Otherwise is
    is invalid. */

    if (*s == 0) return had_double_colon? yield : 0;

    /* If a component starts with an additional colon, we have hit a double
    colon. This is permitted to appear once only, and counts as at least
    one component. The final component may be of this form. */

    if (*s == ':')
      {
      if (had_double_colon) return 0;
      had_double_colon = TRUE;
      s++;
      continue;
      }

    /* If the remainder of the string contains a dot but no colons, we
    can expect a trailing IPv4 address. This is valid if either there has
    been no double-colon and this is the 7th component (with the IPv4 address
    being the 7th & 8th components), OR if there has been a double-colon
    and fewer than 6 components. */

    if (strchr(s, ':') == NULL && strchr(s, '.') != NULL)
      {
      if ((!had_double_colon && count != 6) ||
          (had_double_colon && count > 6)) return 0;
      v4end = TRUE;
      yield = 6;
      break;
      }

    /* Check for at least one and not more than 4 hex digits for this
    component. */

    if (!isxdigit((uschar)*s++)) return 0;
    if (isxdigit((uschar)*s) && isxdigit((uschar)*(++s)) &&
      isxdigit((uschar)*(++s))) s++;

    /* If the component is terminated by colon and there is more to
    follow, skip over the colon. If there is no more to follow the address is
    invalid. */

    if (*s == ':' && *(++s) == 0) return 0;
    }

  /* If about to handle a trailing IPv4 address, drop through. Otherwise
  all is well if we are at the end of the string. */

  if (!v4end) return (*s == 0)? yield : 0;
  }

#endif

/* Test for IPv4 address, which may be the tail-end of an IPv6 address. */

for (i = 0; i < 4; i++)
  {
  if (i != 0 && *s++ != '.') return 0;
  if (!isdigit((uschar)*s++)) return 0;
  if (isdigit((uschar)*s) && isdigit((uschar)*(++s))) s++;
  }

return (*s == 0)? yield : 0;
}



/*************************************************
*              Format message size               *
*************************************************/

/* Convert a message size in bytes to printing form, rounding
according to the magnitude of the number. A value of zero causes
a string of spaces to be returned.

Arguments:
  size        the message size in bytes
  buffer      where to put the answer

Returns:      pointer to the buffer
              a string of exactly 5 characters is normally returned
*/

char *
string_format_size(int size, char *buffer)
{
if (size == 0) strcpy(buffer, "     ");
else if (size < 1024) sprintf(buffer, "%5d", size);
else if (size < 10*1024)
  sprintf(buffer, "%4.1fK", (double)size / 1024.0);
else if (size < 1024*1024)
  sprintf(buffer, "%4dK", (size + 512)/1024);
else if (size < 10*1024*1024)
  sprintf(buffer, "%4.1fM", (double)size / (1024.0 * 1024.0));
else
  sprintf(buffer, "%4dM", (size + 512 * 1024)/(1024*1024));
return buffer;
}



/*************************************************
*       Convert a number to base 62 format       *
*************************************************/

/* Convert a long integer into an ASCII base 62 string.
Always return exactly 6 characters plus zero, in a static area.

Argument: a long integer
Returns:  pointer to base 62 string
*/

char *
string_base62(unsigned long int value)
{
static char yield[7];
static char base62_chars[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char *p = yield + sizeof(yield) - 1;
*p = 0;
while (p > yield)
  {
  *(--p) = base62_chars[value % 62];
  value /= 62;
  }
return yield;
}



/*************************************************
*          Interpret escape sequence             *
*************************************************/

/* This function is called from several places where escape sequences are to be
interpreted in strings.

Arguments:
  pp       points a pointer to the initiating "\" in the string;
           the pointer gets updated to point to the final character
Returns:   the value of the character escape
*/

int
string_interpret_escape(char **pp)
{
int ch;
char *p = *pp;
ch = *(++p);
if (isdigit(ch) && ch != '8' && ch != '9')
  {
  ch -= '0';
  if (isdigit((uschar)p[1]) && p[1] != '8' && p[1] != '9')
    {
    ch = ch * 8 + *(++p) - '0';
    if (isdigit((uschar)p[1]) && p[1] != '8' && p[1] != '9')
      ch = ch * 8 + *(++p) - '0';
    }
  }
else switch(ch)
  {
  case 'n':  ch = '\n'; break;
  case 'r':  ch = '\r'; break;
  case 't':  ch = '\t'; break;
  case 'x':
  ch = 0;
  if (isxdigit((uschar)p[1]))
    {
    ch = ch * 16 +
      strchr(hex_digits, tolower((uschar)*(++p))) - hex_digits;
    if (isxdigit((uschar)p[1])) ch = ch * 16 +
      strchr(hex_digits, tolower((uschar)*(++p))) - hex_digits;
    }
  break;
  }
*pp = p;
return ch;
}



/*************************************************
*          Ensure string is printable            *
*************************************************/

/* This function is called for critical strings. It checks for any
non-printing characters, and if any are found, it makes a new copy
of the string with suitable escape sequences.

Argument:    input string
Returns:     string with non-printers encoded as printing sequences
*/

char *
string_printing(char *s)
{
int nonprintcount = 0;
int length = 0;
uschar *t = (uschar *)s;
char *ss, *tt;

while (*t != 0)
  {
  int c = *t++;
  if (!mac_isprint(c)) nonprintcount++;
  length++;
  }

if (nonprintcount == 0) return s;

/* Get a new block of store guaranteed big enough to hold the
expanded string. */

ss = store_get(length + nonprintcount * 4 + 1);

/* Copy everying, escaping non printers. The unsigned char thing is
for systems that have signed chars by default. */

t = (uschar *)s;
tt = ss;

while (*t != 0)
  {
  int c = *t;
  if (mac_isprint(c)) *tt++ = *t++; else
    {
    *tt++ = '\\';
    switch (*t)
      {
      case '\n': *tt++ = 'n'; break;
      case '\r': *tt++ = 'r'; break;
      case '\b': *tt++ = 'b'; break;
      case '\t': *tt++ = 't'; break;
      case '\v': *tt++ = 'v'; break;
      case '\f': *tt++ = 'f'; break;
      default: sprintf(tt, "%03o", *t); tt += 3; break;
      }
    t++;
    }
  }
*tt = 0;
return ss;
}




/*************************************************
*            Copy and save string                *
*************************************************/

/*
Argument: string to copy
Returns:  copy of string in new store
*/

char *
string_copy(char *s)
{
char *ss = store_get((int)strlen(s) + 1);
strcpy(ss, s);
return ss;
}



/*************************************************
*     Copy and save string in malloc'd store     *
*************************************************/

/*
Argument: string to copy
Returns:  copy of string in new store
*/

char *
string_copy_malloc(char *s)
{
char *ss = store_malloc((int)strlen(s) + 1);
strcpy(ss, s);
return ss;
}



/*************************************************
*       Copy, lowercase and save string          *
*************************************************/

/*
Argument: string to copy
Returns:  copy of string in new store, with letters lowercased
*/

char *
string_copylc(char *s)
{
char *ss = store_get((int)strlen(s) + 1);
char *p = ss;
while (*s != 0) *p++ = tolower(*s++);
*p = 0;
return ss;
}



/*************************************************
*       Copy and save string, given length       *
*************************************************/

/* It is assumed the data contains no zeros. A zero is added
onto the end.

Arguments:
  s         string to copy
  n         number of characters

Returns:    copy of string in new store
*/

char *
string_copyn(char *s, int n)
{
char *ss = store_get(n + 1);
strncpy(ss, s, n);
ss[n] = 0;
return ss;
}


/*************************************************
* Copy, lowercase, and save string, given length *
*************************************************/

/* It is assumed the data contains no zeros. A zero is added
onto the end.

Arguments:
  s         string to copy
  n         number of characters

Returns:    copy of string in new store, with letters lowercased
*/

char *
string_copynlc(char *s, int n)
{
char *ss = store_get(n + 1);
char *p = ss;
while (n-- > 0) *p++ = tolower(*s++);
*p = 0;
return ss;
}


/*************************************************
*     Copy space-terminated or quoted string     *
*************************************************/

/* I should have invented this function at the start; because I didn't, there
are various places in the code that could use it but don't. With luck they may
get converted as time goes by.

This function copies from a string until its end, or until whitespace is
encountered, unless the string begins with a double quote, in which case the
terminating quote is sought, and escaping within the string is done. The length
of a de-quoted string can be no longer than the original, since escaping always
turns n characters into 1 character.

Argument:  pointer to the pointer to the first character, which gets updated
Returns:   the new string
*/

char *
string_dequote(char **sptr)
{
char *s = *sptr;
char *t, *yield;

/* First find the end of the string */

if (*s != '\"')
  {
  while (*s != 0 && !isspace((uschar)*s)) s++;
  }
else
  {
  s++;
  while (*s != 0 && *s != '\"')
    {
    if (*s == '\\') (void)string_interpret_escape(&s);
    s++;
    }
  if (*s != 0) s++;
  }

/* Get enough store to copy into */

t = yield = store_get(s - *sptr + 1);
s = *sptr;

/* Do the copy */

if (*s != '\"')
  {
  while (*s != 0 && !isspace((uschar)*s)) *t++ = *s++;
  }
else
  {
  s++;
  while (*s != 0 && *s != '\"')
    {
    if (*s == '\\') *t++ = string_interpret_escape(&s);
      else *t++ = *s++;
    }
  if (*s != 0) s++;
  }

/* Update the pointer and return the terminated copy */

*sptr = s;
*t = 0;
return yield;
}




/*************************************************
*          Format a string and save it           *
*************************************************/

/* The formatting is done by string_format, which checks the
length of everything.

Arguments:
  format    a printf() format
  ...       arguments for format

Returns:    pointer to fresh piece of store containing sprintf'ed string
*/

char *
string_sprintf(char *format, ...)
{
va_list ap;
char buffer[STRING_SPRINTF_BUFFER_SIZE];
va_start(ap, format);
if (!string_vformat(buffer, sizeof(buffer), format, ap))
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "string_sprintf expansion was longer than %d", sizeof(buffer));
va_end(ap);
return string_copy(buffer);
}



/*************************************************
*         Case-independent strncmp() function    *
*************************************************/

/*
Arguments:
  s         first string
  t         second string
  n         number of characters to compare

Returns:    < 0, = 0, or > 0, according to the comparison
*/

int
strncmpic(char *s, char *t, int n)
{
while (n--)
  {
  int c = tolower(*s++) - tolower(*t++);
  if (c) return c;
  }
return 0;
}


/*************************************************
*         Case-independent strcmp() function     *
*************************************************/

/*
Arguments:
  s         first string
  t         second string

Returns:    < 0, = 0, or > 0, according to the comparison
*/

int
strcmpic(char *s, char *t)
{
while (*s != 0)
  {
  int c = tolower(*s++) - tolower(*t++);
  if (c) return c;
  }
return *t;
}


/*************************************************
*         Case-independent strstr() function     *
*************************************************/

/* The third argument specifies whether whitespace is required
to follow the matched string.

Arguments:
  s              string to search
  t              substring to search for
  space_follows  if TRUE, match only if whitespace follows

Returns:         pointer to substring in string, or NULL if not found
*/

char *
strstric(char *s, char *t, int space_follows)
{
char *p = t;
char *yield = NULL;
int cl = tolower(*p);
int cu = toupper(*p);

while (*s)
  {
  if (*s == cl || *s == cu)
    {
    if (yield == NULL) yield = s;
    if (*(++p) == 0)
      {
      if (!space_follows || s[1] == ' ' || s[1] == '\n' ) return yield;
      yield = NULL;
      p = t;
      }
    cl = tolower(*p);
    cu = toupper(*p);
    s++;
    }
  else if (yield != NULL)
    {
    yield = NULL;
    p = t;
    cl = tolower(*p);
    cu = toupper(*p);
    }
  else s++;
  }
return NULL;
}



/*************************************************
*       Get next string from separated list      *
*************************************************/

/* Leading and trailing space is removed from each item. The separator in the
list is controlled by the int pointed to by the separator argument as follows:

  If its value is > 0 it is used as the delimiter.
    (If its value is actually > UCHAR_MAX there is only one item in the list.
    This is used for some cases when called via match_isinlist.)
  If its value is <= 0, the string is inspected for a leading <x, where
    x is an ispunct() value. If found, it is used as the delimiter. If not
    found: (a) if separator == 0, ':' is used
           (b) if separator <0, then -separator is used
    In all cases the value of the separator that is used is written back to
      the int so that it is used on subsequent calls as we progress through
      the list.

The separator can always be represented in the string by doubling.

Arguments:
  listptr    points to a pointer to the current start of the list; the
             pointer gets updated to point after the end of the next item
  separator  a pointer to the separator character in an int (see above)
  buffer     where to put a copy of the next string in the list
  buflen     the size of buffer

Returns:     pointer to buffer, containing the next substring,
             or NULL if no more substrings
*/

char *
string_nextinlist(char **listptr, int *separator, char *buffer, int buflen)
{
register int p = 0;
register int sep = *separator;
register uschar *s = (uschar *)(*listptr);

if (s == NULL) return NULL;
while (isspace(*s)) s++;
if (*s == 0) return NULL;

if (sep <= 0)
  {
  if (*s == '<' && ispunct(s[1]))
    {
    sep = s[1];
    s += 2;
    while (isspace(*s)) s++;
    if (*s == 0) return NULL;
    }
  else
    {
    sep = (sep == 0)? ':' : -sep;
    }
  *separator = sep;
  }

for (; *s != 0; s++)
  {
  if (*s == sep && *(++s) != sep) break;
  if (p < buflen - 1) buffer[p++] = *s;
  }

while (p > 0 && isspace((uschar)buffer[p-1])) p--;
buffer[p] = 0;
*listptr = (char *)s;
return buffer;
}



/*************************************************
*             Add chars to string                *
*************************************************/

/* This function is used when building up strings of unknown length.
Room is always left for a terminating zero to be added.

Arguments:
  string   points to the start of the string
  size     the current size of the store (updated if changed)
  ptr      the offset at which to add characters, updated
  s        points to characters to add
  len      count of characters to add

Returns:   pointer to the start of the string, changed if copied for expansion
*/

char *
string_cat(char *string, int *size, int *ptr, char *s, int len)
{
int p = *ptr;
if (p + len >= *size)
  {
  int oldsize = *size;
  while (*size <= p + len) *size += 50;

  /* We must *not* use store_release() here as is done in a
  similar bit of code in accept.c because we cannot guarantee
  that there are no other calls to the store_ functions between
  calls to string_cat(). */

  if (!store_extend(string, oldsize, *size))
    {
    char *newstring = store_get(*size);
    memcpy(newstring, string, p);
    string = newstring;
    }
  }
strncpy(string + p, s, len);
*ptr = p + len;
return string;
}



/*************************************************
*        Format a string with length checks      *
*************************************************/

/* This function is used to format a string with checking of the
length of the output for all conversions. It protects Exim from absent-
mindedness when calling functions like debug_printf and string_sprintf,
and elsewhere. There are two different entry points to what is actually
the same function, depending on whether the variable length list of
data arguments are given explicitly or as a va_list item.

Arguments:
  buffer       a buffer in which to put the formatted string
  buflen       the length of the buffer
  format       the format string
  ... or ap    variable list of supplementary arguments

Returns:       TRUE if the result fitted in the buffer
*/

BOOL
string_format(char *buffer, int buflen, char *format, ...)
{
BOOL yield;
va_list ap;
va_start(ap, format);
yield = string_vformat(buffer, buflen, format, ap);
va_end(ap);
return yield;
}


BOOL
string_vformat(char *buffer, int buflen, char *format, va_list ap)
{
BOOL yield = TRUE;
int width, precision;
char *p = buffer;
char *last = buffer + buflen - 1;

/* Scan the format and handle the insertions */

while (*format != 0)
  {
  int *nptr;
  int slen;
  char *item_start, *s;
  char newformat[16];

  /* Non-% characters just get copied verbatim */

  if (*format != '%')
    {
    if (p >= last) { yield = FALSE; break; }
    *p++ = *format++;
    continue;
    }

  /* Deal with % characters. Pick off the width and precision, for checking
  strings, skipping over the flag and modifier characters. */

  item_start = format;
  width = precision = -1;

  if (strchr("-+ #0", *(++format)) != NULL) format++;

  if (isdigit((uschar)*format))
    {
    width = *format++ - '0';
    while (isdigit((uschar)*format)) width = width * 10 + *format++ - '0';
    }
  else if (*format == '*')
    {
    width = va_arg(ap, int);
    format++;
    }

  if (*format == '.')
    {
    if (*(++format) == '*')
      {
      precision = va_arg(ap, int);
      format++;
      }
    else
      {
      precision = 0;
      while (isdigit((uschar)*format))
        precision = precision*10 + *format++ - '0';
      }
    }

  if (strchr("hlL", *format) != NULL) format++;

  /* Handle each specific format type. */

  switch (*format++)
    {
    case 'n':
    nptr = va_arg(ap, int *);
    *nptr = p - buffer;
    break;

    case 'd':
    case 'o':
    case 'u':
    case 'x':
    case 'X':
    if (p >= last - 12) { yield = FALSE; goto END_FORMAT; }
    strncpy(newformat, item_start, format - item_start);
    newformat[format - item_start] = 0;
    sprintf(p, newformat, va_arg(ap, int));
    while (*p) p++;
    break;

    /* %f format is inherently insecure if the numbers that it may be
    handed are unknown (e.g. 1e300). However, in Exim, the only use of %f
    is for printing load averages, and these are actually stored as integers
    (load average * 1000) so the size of the numbers is constrained. */

    case 'f':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    if (precision < 0) precision = 6;
    if (p >= last - precision - 8) { yield = FALSE; goto END_FORMAT; }
    strncpy(newformat, item_start, format - item_start);
    newformat[format-item_start] = 0;
    sprintf(p, newformat, va_arg(ap, double));
    while (*p) p++;
    break;

    /* String types */

    case '%':
    if (p >= last) { yield = FALSE; goto END_FORMAT; }
    *p++ = '%';
    break;

    case 'c':
    if (p >= last) { yield = FALSE; goto END_FORMAT; }
    *p++ = va_arg(ap, int);
    break;

    case 's':
    case 'S':                   /* Forces *lower* case */
    s = va_arg(ap, char *);
    if (s == NULL) s = "NULL";
    slen = (int)strlen(s);

    /* If the width is specified, check that there is a precision
    set; if not, set it to the width to prevent overruns of long
    strings. */

    if (width >= 0)
      {
      if (precision < 0) precision = width;
      }

    /* If a width is not specified and the precision is specified, set
    the width to the precision, or the string length if shorted. */

    else if (precision >= 0)
      {
      width = (precision < slen)? precision : slen;
      }

    /* If neither are specified, set them both to the string length. */

    else width = precision = slen;

    /* Check string space, and add the string to the buffer if ok. If
    not OK, add part of the string (debugging uses this to show as
    much as possible). */

    if (p >= last - width)
      {
      yield = FALSE;
      width = precision = last - p - 1;
      }
    sprintf(p, "%*.*s", width, precision, s);
    if (format[-1] == 'S')
      while (*p) { *p = tolower(*p); p++; }
    else
      while (*p) p++;
    if (!yield) goto END_FORMAT;
    break;

    /* Some things are never used in Exim; also catches junk. */

    default:
    strncpy(newformat, item_start, format - item_start);
    newformat[format-item_start] = 0;
    log_write(0, LOG_PANIC_DIE, "string_format: unsupported type in \"%s\"",
      newformat);
    break;
    }
  }

/* Ensure string is complete; return TRUE if got to the end of the format */

END_FORMAT:

*p = 0;
return yield;
}




/*************************************************
*       Generate an "open failed" message        *
*************************************************/

/* This function creates a message after failure to open a file. It includes a
string supplied as data, adds the strerror() text, and if the failure was
"Permission denied", reads and includes the euid and egid.

Arguments:
  eno           the value of errno after the failure
  format        a text format string
  ...           arguments for the format string

Returns:        a message, in dynamic store
*/

char *
string_open_failed(int eno, char *format, ...)
{
va_list ap;
char buffer[1024];

strcpy(buffer, "failed to open ");
va_start(ap, format);

/* Use the checked formatting routine to ensure that the buffer
does not overflow. It should not, since this is called only for internally
specified messages. If it does, the message just gets truncated, and there
doesn't seem much we can do about that. */

(void)string_vformat(buffer+15, sizeof(buffer) - 15, format, ap);

return (eno == EACCES)?
  string_sprintf("%s: %s (euid=%ld egid=%ld)", buffer, strerror(eno),
    (long int)geteuid(), (long int)getegid()) :
  string_sprintf("%s: %s", buffer, strerror(eno));
}



/*************************************************
*          Generate log address list             *
*************************************************/

/* This function generates a list consisting of an address and its parents, for
use in logging lines. It is passed the address and its ultimate parent, and a
flag indicating a directed local address. For saved onetime aliased addresses,
the onetime parent field is used.

Arguments:
  addr          bottom (ultimate) address
  topaddr       original parent
  dlocal        TRUE if a directed local address
  all_parents   if TRUE, include all parents

Returns:        a string in dynamic store
*/

char *
string_log_address(address_item *addr, address_item *topaddr, BOOL dlocal,
  BOOL all_parents)
{
int size = 64;
int ptr = 0;
BOOL add_topaddr = TRUE;
char *yield = store_get(size);

/* A local delivery that was directed starts with just the local part. */

if (dlocal)
  yield = string_cat(yield, &size, &ptr, addr->local_part,
    (int)strlen(addr->local_part));

/* A routed local delivery or any remote delivery starts with the full
address. If it is the same as the original
address, and no parents are to be shown, unset the flag for adding the
top address. */

else
  {
  yield = string_cat(yield, &size, &ptr, addr->local_part,
    (int)strlen(addr->local_part));
  yield = string_cat(yield, &size, &ptr, "@", 1);
  yield = string_cat(yield, &size, &ptr, addr->domain,
    (int)strlen(addr->domain) );
  yield[ptr] = 0;

  /* If the address we are going to print is the same as the top address,
  don't add on the top address. First of all, do a caseless comparison; if
  this succeeds, do a caseful comparison on the local parts. */

  if (strcmpic(yield, topaddr->orig) == 0 &&
      strncmp(yield, topaddr->orig, strchr(yield, '@') - yield) == 0 &&
      addr->onetime_parent == NULL &&
      (!all_parents || addr->parent == NULL || addr->parent == topaddr))
    add_topaddr = FALSE;
  }

/* If all parents are requested, or this is a local pipe/file/reply, and
there is at least one intermediate parent, show it in brackets, and continue
with all of them if all are wanted. */

if ((all_parents || testflag(addr, af_pfr)) &&
    addr->parent != NULL &&
    addr->parent != topaddr)
  {
  char *s = " (";
  address_item *addr2;
  for (addr2 = addr->parent; addr2 != topaddr; addr2 = addr2->parent)
    {
    yield = string_cat(yield, &size, &ptr, s, 2);
    yield = string_cat(yield, &size, &ptr, addr2->orig,
      (int)strlen(addr2->orig));
    if (!all_parents) break;
    s = ", ";
    }
  yield = string_cat(yield, &size, &ptr, ")", 1);
  }

/* Add the top address if it is required */

if (add_topaddr)
  {
  yield = string_cat(yield, &size, &ptr, " <", 2);

  if (addr->onetime_parent == NULL)
    yield = string_cat(yield, &size, &ptr, topaddr->orig,
      (int)strlen(topaddr->orig));
  else
    yield = string_cat(yield, &size, &ptr, addr->onetime_parent,
      (int)strlen(addr->onetime_parent));

  yield = string_cat(yield, &size, &ptr, ">", 1);
  }

yield[ptr] = 0;  /* string_cat() leaves space */
return yield;
}





/*************************************************
**************************************************
*             Stand-alone test program           *
**************************************************
*************************************************/

#ifdef STAND_ALONE
int main(void)
{
char buffer[256];

printf("Testing is_ip_address\n");

while (fgets(buffer, 256, stdin) != NULL)
  {
  int offset;
  buffer[(int)strlen(buffer) - 1] = 0;
  printf("%d\n", string_is_ip_address(buffer, NULL));
  printf("%d %d %s\n", string_is_ip_address(buffer, &offset), offset, buffer);
  }

/* This is a horrible lash-up, but it serves its purpose. */

printf("Testing string_format\n");

while (fgets(buffer, 256, stdin) != NULL)
  {
  void *args[3];
  double dargs[3];
  int dflag = 0;
  int n = 0;
  int count;
  int countset = 0;
  char format[256];
  char outbuf[256];
  char *s;
  buffer[(int)strlen(buffer) - 1] = 0;

  s = strchr(buffer, ',');
  if (s == NULL) s = buffer + (int)strlen(buffer);

  strncpy(format, buffer, s - buffer);
  format[s-buffer] = 0;

  if (*s == ',') s++;

  while (*s != 0)
    {
    char *ss = s;
    s = strchr(ss, ',');
    if (s == NULL) s = ss + (int)strlen(ss);

    if (isdigit((uschar)*ss))
      {
      strncpy(outbuf, ss, s-ss);
      if (strchr(outbuf, '.') != NULL)
        {
        dflag = 1;
        dargs[n++] = strtod(outbuf, NULL);
        }
      else
        {
        args[n++] = (void *)atoi(outbuf);
        }
      }

    else if (strcmp(ss, "*") == 0)
      {
      args[n++] = (void *)(&count);
      countset = 1;
      }

    else
      {
      char *sss = malloc(s - ss + 1);
      strncpy(sss, ss, s-ss);
      args[n++] = sss;
      }

    if (*s == ',') s++;
    }

  if (!dflag) printf("%s\n", string_format(outbuf, sizeof(outbuf), format,
    args[0], args[1], args[2])? "True" : "False");

  else printf("%s\n", string_format(outbuf, sizeof(outbuf), format,
    dargs[0], dargs[1], dargs[2])? "True" : "False");

  printf("%s\n", outbuf);
  if (countset) printf("count=%d\n", count);
  }

return 0;
}
#endif

/* End of string.c */
