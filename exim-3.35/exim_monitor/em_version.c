/*************************************************
*                  Exim Monitor                  *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

#include <stdlib.h>
#include <string.h>

extern char *version_string;
extern char *version_date;

void
version_init(void)
{
int i = 0;
char today[20];

version_string = "1.70";

strcpy(today, __DATE__);
if (today[4] == ' ') i = 1;
today[3] = today[6] = '-';

version_date = (char *)malloc(32);
version_date[0] = 0;
strncat(version_date, today+4+i, 3-i);
strncat(version_date, today, 4);
strncat(version_date, today+7, 4);
strcat(version_date, " ");
strcat(version_date, __TIME__);
}

/* End of em_version.c */
