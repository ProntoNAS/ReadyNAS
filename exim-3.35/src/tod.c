/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */

/* A function for returning the time of day in various formats */


#include "exim.h"


static char timebuf[sizeof("www, dd-mmm-yyyy hh:mm:ss +zzzz")];


/*************************************************
*                Return timestamp                *
*************************************************/

/* The log timestamp format is dd-mmm-yy so as to be non-confusing on both
sides of the Atlantic. We calculate an explicit numerical offset from GMT for
the full datestamp and BSD inbox datestamp. Note that on some systems
localtime() and gmtime() re-use the same store, so we must save the local time
values before calling gmtime(). If timestamps_utc is set, don't use
localtime(); alll times are then in UTC (with offset +0000).

There are also some contortions to get the day of the month without
a leading zero for the full stamp, since strftime() doesn't provide this
option.

Argument:  type of timestamp required:
             tod_log    log file format
             tod_bsdin  BSD inbox format
             tod_mbx    MBX inbox format
             tod_full   full date and time

Returns:   pointer to fixed buffer containing the timestamp
*/

char *
tod_stamp(int type)
{
time_t now = time(NULL);
struct tm *t = timestamps_utc? gmtime(&now) : localtime(&now);

/* Format used in logging */

if (type == tod_log)
  {
  (void) sprintf(timebuf, "%04d-%02d-%02d %02d:%02d:%02d",
    1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday,
    t->tm_hour, t->tm_min, t->tm_sec);
  }

/* Format used in BSD inbox separator lines. Sort-of documented in RFC 976
("UUCP Mail Interchange Format Standard") but only by example, not by explicit
definition. The examples show no timezone offsets, and some MUAs appear to be
sensitive to this, so Exim has been changed to remove the timezone offsets that
originally appeared. */

else if (type == tod_bsdin)
  {
  int len = strftime(timebuf, sizeof(timebuf), "%a %b %d %H:%M:%S", t);
  strftime(timebuf + len, sizeof(timebuf) - len, " %Y", t);
  }

/* Other types require the GMT offset to be calculated, or just set up in the
case of UTC timestamping. */

else
  {
  int diff_hour, diff_min;
  struct tm local;
  memcpy(&local, t, sizeof(struct tm));

  if (timestamps_utc)
    {
    diff_hour = diff_min = 0;
    }
  else
    {
    struct tm *gmt = gmtime(&now);
    diff_min = 60*(local.tm_hour - gmt->tm_hour) + local.tm_min - gmt->tm_min;
    if (local.tm_year != gmt->tm_year)
      diff_min += (local.tm_year > gmt->tm_year)? 1440 : -1440;
    else if (local.tm_yday != gmt->tm_yday)
      diff_min += (local.tm_yday > gmt->tm_yday)? 1440 : -1440;
    diff_hour = diff_min/60;
    diff_min  = abs(diff_min - diff_hour*60);
    }

  /* tod_mbx: format used in MBX mailboxes - subtly different to tod_full */

  #ifdef SUPPORT_MBX
  if (type == tod_mbx)
    {
    int len;
    (void) sprintf(timebuf, "%02d-", local.tm_mday);
    len = strlen(timebuf);
    len += strftime(timebuf + len, sizeof(timebuf) - len, "%b-%Y %H:%M:%S", &local);
    (void) sprintf(timebuf + len, " %+03d%02d", diff_hour, diff_min);
    }
  else
  #endif

  /* tod_full: format used in Received: headers. */

    {
    int len = strftime(timebuf, sizeof(timebuf), "%a, ", &local);
    (void) sprintf(timebuf + len, "%02d ", local.tm_mday);
    len += strlen(timebuf + len);
    len += strftime(timebuf + len, sizeof(timebuf) - len, "%b %Y %H:%M:%S", &local);
    (void) sprintf(timebuf + len, " %+03d%02d", diff_hour, diff_min);
    }
  }

return timebuf;
}

/* End of tod.c */
