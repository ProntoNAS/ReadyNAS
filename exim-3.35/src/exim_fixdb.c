/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/stat.h>

#include <pwd.h>                  /* Because used in includes below */
#include <pcre.h>	            /* Ditto */
#define  MAXPACKET 1024           /* Ditto */

#include "macros.h"
#include "config.h"
#include "dbfn.h"
#include "structs.h"
#include "functions.h"

#define type_retry 1
#define type_wait 2
#define type_reject 3
#define type_serialize 4


/* Utility program to interrogate the contents of an exim database file, with
possible updating. The first argument is the spool directory. The second
argument is the name of the database file. The available names are:

  retry                  retry delivery information
  wait-<transport>       message waiting information
  reject                 message rejection information
  serialize-<transport>  host serialization information

In order not to hold the database lock any longer than is necessary, each
operation on the database uses a separate open/close call. This is expensive,
but then using this utility is not expected to be very common.

Syntax of commands:

(1) <record name>
    This causes the data from the given record to be displayed, or "not found"
    to be output. Note that in the retry database, destination names are
    preceded by D: R: or T: for director, router, or transport retry info.

(2) <record name> d
    This causes the given record to be deleted or "not found" to be output.

(3) <record name> <field number> <value>
    This sets the given value into the given field, identified by a number
    which is output by the display command. Not all types of record can
    be changed.

(4) q
    This exits from exim_fixdb.

If the record name is omitted from (2) or (3), the previously used record name
is re-used. */


/* Need a few globals that the dbfn and string functions use. */

extern char *spool_directory;
char *spool_directory;

extern volatile int sigalrm_seen;
volatile int sigalrm_seen;

extern int store_pool;
int store_pool = POOL_MAIN;

extern int debug_level;
int debug_level = 0;

extern int debug_trace_memory;
int debug_trace_memory = 0;

extern BOOL print_topbitchars;
BOOL print_topbitchars = FALSE;


/* Functions for handling time */

static char time_buffer[sizeof("09-xxx-1999 hh:mm:ss  ")];

char *print_time(time_t t)
{
struct tm *tmstr = localtime(&t);
strftime(time_buffer, sizeof(time_buffer), "%d-%b-%Y %H:%M:%S", tmstr);
return time_buffer;
}


static time_t read_time(char *s)
{
char *t = s;
int field = 0;
int value;
time_t now = time(NULL);
struct tm *tm = localtime(&now);

tm->tm_sec = 0;
tm->tm_isdst = -1;

for (t = s + (int)strlen(s) - 1; t >= s; t--)
  {
  if (*t == ':') continue;
  if (!isdigit((uschar)*t)) return -1;

  value = *t - '0';
  if (--t >= s)
    {
    if (!isdigit((uschar)*t)) return -1;
    value = value + (*t - '0')*10;
    }

  switch (field++)
    {
    case 0: tm->tm_min = value; break;
    case 1: tm->tm_hour = value; break;
    case 2: tm->tm_mday = value; break;
    case 3: tm->tm_mon = value - 1; break;
    case 4: tm->tm_year = (value < 90)? value + 100 : value; break;
    default: return -1;
    }
  }

return mktime(tm);
}


int main(int argc, char **argv)
{
int dbdata_type = 0;
char buffer[256];
char name[256];
void *reset_point = store_get(0);

name[0] = 0;  /* No name set */

/* Check up on the number of arguments */

if (argc != 3)
  {
  printf("Usage: exim_fixdb <spool-directory> <database-name>\n");
  printf("<database-name> = retry | wait-<transport> | reject\n");
  exit(1);
  }

if (strcmp(argv[2], "retry") == 0) dbdata_type = type_retry;
  else if (strcmp(argv[2], "reject") == 0) dbdata_type = type_reject;
  else if (strncmp(argv[2], "wait-", 5) == 0) dbdata_type = type_wait;
  else if (strncmp(argv[2], "serialize-", 10) == 0) dbdata_type = type_serialize;

if (dbdata_type == 0)
  {
  printf("Usage: exim_fixdb <spool-directory> <database-name>\n");
  printf("<database-name> = retry | wait-<transport> | reject | "
    "serialize-<transport>\n");
  exit(1);
  }

/* Set up the spool directory in the global place. */

spool_directory = argv[1];

/* Verify what we are working on */

printf("Exim %s database in spool %s\n", argv[2], spool_directory);

/* Process user requests */

for(;;)
  {
  open_db dbblock;
  open_db *dbm;
  void *record;
  dbdata_retry *retry;
  dbdata_wait *wait;
  dbdata_reject *reject;
  dbdata_serialize *serialize;
  int i, length;
  char *t;
  char field[256], value[256];

  store_reset(reset_point);

  printf("> ");
  if (fgets(buffer, 256, stdin) == NULL) break;

  buffer[(int)strlen(buffer)-1] = 0;
  field[0] = value[0] = 0;

  /* If the buffer contains just one digit, or just consists of "d", use the
  previous name for an update. */

  if ((isdigit((uschar)buffer[0]) && !isdigit((uschar)buffer[1])) ||
       strcmp(buffer, "d") == 0)
    {
    if (name[0] == 0)
      {
      printf("No previous record name is set\n");
      continue;
      }
    sscanf(buffer, "%s %s", field, value);
    }
  else
    {
    name[0] = 0;
    sscanf(buffer, "%s %s %s", name, field, value);
    }

  /* Handle an update request */

  if (field[0] != 0)
    {
    int verify = 1;
    dbm = dbfn_open(argv[2], O_RDWR, &dbblock, TRUE);
    if (dbm == NULL) continue;

    if (strcmp(field, "d") == 0)
      {
      if (value[0] != 0) printf("unexpected value after \"d\"\n");
        else printf("%s\n", (dbfn_delete(dbm, name) < 0)?
          "not found" : "deleted");
      dbfn_close(dbm);
      continue;
      }

    else if (isdigit((uschar)field[0]))
      {
      int fieldno = atoi(field);
      if (value[0] == 0)
        {
        printf("value missing\n");
        dbfn_close(dbm);
        continue;
        }
      else
        {
        record = dbfn_read(dbm, name);
        if (record == NULL) printf("not found\n"); else
          {
          time_t tt;
          int length = 0;     /* Stops compiler warning */

          switch(dbdata_type)
            {
            case type_retry:
            retry = (dbdata_retry *)record;
            length = sizeof(dbdata_retry) + (int)strlen(retry->text);

            switch(fieldno)
              {
              case 0:
              retry->basic_errno = atoi(value);
              break;

              case 1:
              retry->more_errno = atoi(value);
              break;

              case 2:
              if ((tt = read_time(value)) > 0) retry->first_failed = tt;
                else printf("bad time value\n");
              break;

              case 3:
              if ((tt = read_time(value)) > 0) retry->last_try = tt;
                else printf("bad time value\n");
              break;

              case 4:
              if ((tt = read_time(value)) > 0) retry->next_try = tt;
                else printf("bad time value\n");
              break;

              case 5:
              if (strcmp(value, "yes") == 0) retry->expired = TRUE;
              else if (strcmp(value, "no") == 0) retry->expired = FALSE;
              else printf("\"yes\" or \"no\" expected=n");
              break;

              default:
              printf("unknown field number\n");
              verify = 0;
              break;
              }
            break;

            case type_wait:
            printf("Can't change contents of wait database record\n");
            break;

            case type_reject:
            printf("Can't change contents of reject database record\n");
            break;

            case type_serialize:
            printf("Can't change contents of serialization database record\n");
            break;
            }

          dbfn_write(dbm, name, record, length);
          }
        }
      }

    else
      {
      printf("field number or d expected\n");
      verify = 0;
      }

    dbfn_close(dbm);
    if (!verify) continue;
    }

  /* The "name" q causes an exit */

  else if (strcmp(name, "q") == 0) return 0;

  /* Handle a read request, or verify after an update. */

  dbm = dbfn_open(argv[2], O_RDONLY, &dbblock, TRUE);
  if (dbm == NULL) continue;

  record = dbfn_read_with_length(dbm, name, &length);
  if (record == NULL)
    {
    printf("record %s not found\n", name);
    name[0] = 0;
    }
  else
    {
    int count_bad = 0;
    switch(dbdata_type)
      {
      case type_retry:
      retry = (dbdata_retry *)record;
      printf("0 error number: %d %s\n", retry->basic_errno, retry->text);
      printf("1 extra data:   %d\n", retry->more_errno);
      printf("2 first failed: %s\n", print_time(retry->first_failed));
      printf("3 last try:     %s\n", print_time(retry->last_try));
      printf("4 next try:     %s\n", print_time(retry->next_try));
      printf("5 expired:      %s\n", (retry->expired)? "yes" : "no");
      break;

      case type_wait:
      wait = (dbdata_wait *)record;
      t = wait->text;
      printf("Sequence: %d\n", wait->sequence);
      if (wait->count > WAIT_NAME_MAX)
        {
        printf("**** Data corrupted: count=%d=0x%x max=%d ****\n", wait->count,
          wait->count, WAIT_NAME_MAX);
        wait->count = WAIT_NAME_MAX;
        count_bad = 1;
        }
      for (i = 1; i <= wait->count; i++)
        {
        strncpy(value, t, MESSAGE_ID_LENGTH);
        value[MESSAGE_ID_LENGTH] = 0;
        if (count_bad && value[0] == 0) break;
        if ((int)strlen(value) != MESSAGE_ID_LENGTH ||
            strspn(value, "0123456789"
                          "abcdefghijklmnopqrstuvwxyz"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ-") != MESSAGE_ID_LENGTH)
          {
          int j;
          printf("\n**** Data corrupted: bad character in message id ****\n");
          for (j = 0; j < MESSAGE_ID_LENGTH; j++)
            printf("%02x ", value[j]);
          printf("\n");
          break;
          }
        printf("%s ", value);
        t += MESSAGE_ID_LENGTH;
        }
      printf("\n");
      break;

      case type_reject:
      reject = (dbdata_reject *)record;
      if (length < sizeof(dbdata_reject))
        printf("%s %c  0.0\n", print_time(reject->time_stamp),
          reject->rejected_mail_from? 'T':'F');
      else
        {
        printf("%s %c %5.1f\n", print_time(reject->time_stamp),
          reject->rejected_mail_from? 'T':'F', reject->temp_rate);
        }
      break;

      case type_serialize:
      serialize = (dbdata_serialize *)record;
      printf("%s\n", print_time(serialize->time_stamp));
      break;
      }
    }

  /* The database is closed after each request */

  dbfn_close(dbm);
  }

printf("\n");
return 0;
}

/* End of exim_fixdb.c */
