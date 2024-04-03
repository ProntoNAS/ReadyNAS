/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* Function for setting up the version string. */

#include "exim.h"


#define THIS_VERSION  "3.35"


/* The header file cnumber.h contains a single line containing the
compilation number, making it easy to have it updated automatically.
Hence the fudgery below to get the number turned into a string, since
we can't use #include inside a macro argument list */

void
version_init(void)
{
char today[20];
char *version_cnumber_format;

int cnumber =
#include "cnumber.h"
;

/* The odd magic after each of these is so they can be easily found
for automatic patching to standard values when running regression tests.
The reason that version_cnumber_format isn't just written inline in the
sprintf() call is the gcc -Wall warns about a \0 in a format string. */

version_cnumber = store_malloc(24);
version_cnumber_format = "%d\0<<eximcnumber>>";
sprintf(version_cnumber, version_cnumber_format, cnumber);
version_string = THIS_VERSION "\0<<eximversion>>";

strcpy(today, __DATE__);
if (today[4] == ' ') today[4] = '0';
today[3] = today[6] = '-';

version_date = (char *)store_malloc(32);
version_date[0] = 0;
strncat(version_date, today+4, 3);
strncat(version_date, today, 4);
strncat(version_date, today+7, 4);
strcat(version_date, " ");
strcat(version_date, __TIME__);
}

/* End of version.c */
