/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


/* Utility program to tidy the contents of an exim database file. The
arguments are two optional flags, followed by the spool directory name and the
database name. The flags are

   -f         full tidy - for the waiting and retry databases, checks are made
              on the existence of messages

   -t <time>  expiry time for old records - default 30 days

The final argument is the name of the database file. The available names are:

  retry:      retry delivery information
  wait-<t>:   message waiting information - <t> is transport name
  reject:     message rejection information
  serialize:  host serialization information

This argument is required. */


#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
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



typedef struct key_item {
  struct key_item *next;
  char   key[1];
} key_item;



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



static void usage(void)
{
fprintf(stderr,"Usage: exim_tidydb [-f] [-t <time>] <spool-directory> <database-name>\n");
fprintf(stderr, "<database-name> = retry | wait-<transport> | reject | "
  "serialize-<transport>\n");
exit(1);
}


int main(int argc, char **argv)
{
struct stat statbuf;
int  dbdata_type = 0;
int  maxkeep = 30 * 24 * 60 * 60;
int  i, oldest, path_len;
int dbname_arg = 0;
key_item *keychain = NULL;
void *reset_point;
BOOL full = FALSE;
open_db dbblock;
open_db *dbm;
EXIM_CURSOR *cursor;
char buffer[256];
char *key;


/* Scan the arguments */

for (i = 1; i < argc; i++)
  {
  if (argv[i][0] != '-')
    {
    spool_directory = argv[i++];
    if (i != argc-1) usage();
    dbname_arg = i;
    if (strcmp(argv[i], "retry") == 0) dbdata_type = type_retry;
    else if (strncmp(argv[i], "wait-", 5) == 0) dbdata_type = type_wait;
    else if (strcmp(argv[i], "reject") == 0) dbdata_type = type_reject;
    else if (strncmp(argv[i], "serialize-", 10) == 0) dbdata_type = type_serialize;
    else usage();
    }
  else if (strcmp(argv[i], "-f") == 0) full = TRUE;
  else if (strcmp(argv[i], "-t") == 0)
    {
    char *s;
    s = argv[++i];
    maxkeep = 0;
    while (*s != 0)
      {
      int value, count;
      if (!isdigit((uschar)*s)) usage();
      (void)sscanf(s, "%d%n", &value, &count);
      s += count;
      switch (*s)
        {
        case 'w': value *= 7;
        case 'd': value *= 24;
        case 'h': value *= 60;
        case 'm': value *= 60;
        case 's': s++;
        break;
        default: usage();
        }
      maxkeep += value;
      }
    }
  else usage();
  }


/* Compute the oldest keep time */

oldest = time(NULL) - maxkeep;

/* Database name missing */

if (dbdata_type == 0) usage();

/* Verify what we are working on */

printf("Exim %s database in spool %s\n", argv[dbname_arg], spool_directory);


/* Open the database - this gets a write lock as well. After a locking failure
(which is logged) errno will be 0. */

dbm = dbfn_open(argv[dbname_arg], O_RDWR|O_CREAT, &dbblock, TRUE);
if (dbm == NULL) exit(1);

/* Prepare for building file names */

sprintf(buffer, "%s/input/", spool_directory);
path_len = (int)strlen(buffer);


/* It appears, by experiment, that it is a bad idea to make changes
to the file while scanning it. Pity the man page doesn't warn you about that.
Therefore, we scan and build a list of all the keys. Then we use that to
read the records and possibly update them. */

key = dbfn_scan(dbm, TRUE, &cursor);
while (key != NULL)
  {
  key_item *k = store_get(sizeof(key_item) + (int)strlen(key));
  k->next = keychain;
  keychain = k;
  strcpy(k->key, key);
  key = dbfn_scan(dbm, FALSE, &cursor);
  }

/* Now scan the collected keys and operate on the records, resetting
the store each time round. */

reset_point = store_get(0);

while (keychain != NULL)
  {
  dbdata_generic *value;

  store_reset(reset_point);
  key = keychain->key;
  keychain = keychain->next;
  value = dbfn_read(dbm, key);

  /* A continuation record may have been deleted or renamed already, so
  non-existence is not serious. */

  if (value == NULL) continue;

  /* Delete if too old */

  if (value->time_stamp < oldest)
    {
    fprintf(stdout,"deleted %s (too old)\n", key);
    dbfn_delete(dbm, key);
    continue;
    }

  /* If full processing not requested, do no more. */

  if (!full) continue;

  /* Otherwise do database-specific tidying for wait databases, and message-
  specific tidying for the retry database. */

  if (dbdata_type == type_wait)
    {
    dbdata_wait *wait = (dbdata_wait *)value;
    BOOL update = FALSE;

    /* Leave corrupt records alone */

    if (wait->count > WAIT_NAME_MAX)
      {
      fprintf(stderr,"**** Data for %s corrupted\n  count=%d=0x%x max=%d\n",
        key, wait->count, wait->count, WAIT_NAME_MAX);
      continue;
      }

    /* Loop for renamed continuation records. For each message id,
    check to see if the message exists, and if not, remove its entry
    from the record. Because of the possibility of split input directories,
    we must look in both possible places for a -D file. */

    for (;;)
      {
      int offset;
      int length = wait->count * MESSAGE_ID_LENGTH;

      for (offset = length - MESSAGE_ID_LENGTH;
           offset >= 0; offset -= MESSAGE_ID_LENGTH)
        {
        strncpy(buffer+path_len, wait->text + offset, MESSAGE_ID_LENGTH);
        sprintf(buffer+path_len + MESSAGE_ID_LENGTH, "-D");

        if (stat(buffer, &statbuf) != 0)
          {
          buffer[path_len] = wait->text[offset+5];
          buffer[path_len+1] = '/';
          strncpy(buffer+path_len+2, wait->text + offset, MESSAGE_ID_LENGTH);
          sprintf(buffer+path_len+2 + MESSAGE_ID_LENGTH, "-D");

          if (stat(buffer, &statbuf) != 0)
            {
            int left = length - offset - MESSAGE_ID_LENGTH;
            if (left > 0) strncpy(wait->text + offset,
              wait->text + offset + MESSAGE_ID_LENGTH, left);
            wait->count--;
            length -= MESSAGE_ID_LENGTH;
            update = TRUE;
            }
          }
        }

      /* If record is empty and the main record, either delete it or rename
      the next continuation, repeating if that is also empty. */

      if (wait->count == 0 && strchr(key, ':') == NULL)
        {
        while (wait->count == 0 && wait->sequence > 0)
          {
          char newkey[256];
          dbdata_generic *newvalue;
          sprintf(newkey, "%s:%d", key, wait->sequence - 1);
          newvalue = dbfn_read(dbm, newkey);
          if (newvalue != NULL)
            {
            value = newvalue;
            wait = (dbdata_wait *)newvalue;
            dbfn_delete(dbm, newkey);
            printf("renamed %s\n", newkey);
            update = TRUE;
            }
          else wait->sequence--;
          }

        /* If we have ended up with an empty main record, delete it
        and break the loop. Otherwise the new record will be scanned. */

        if (wait->count == 0 && wait->sequence == 0)
          {
          dbfn_delete(dbm, key);
          printf("deleted %s (empty)\n", key);
          update = FALSE;
          break;
          }
        }

      /* If not an empty main record, break the loop */

      else break;
      }

    /* Re-write the record if required */

    if (update)
      {
      printf("updated %s\n", key);
      dbfn_write(dbm, key, wait, sizeof(dbdata_wait) +
        wait->count * MESSAGE_ID_LENGTH);
      }
    }

  /* If a retry record's key ends with a message-id, check that that message
  still exists; if not, remove this record. */

  else if (dbdata_type == type_retry)
    {
    char *id;
    int len = (int)strlen(key);

    if (len < MESSAGE_ID_LENGTH + 1) continue;
    id = key + len - MESSAGE_ID_LENGTH - 1;
    if (*id++ != ':') continue;

    for (i = 0; i < MESSAGE_ID_LENGTH; i++)
      {
      if (i == 6 || i == 13)
        { if (id[i] != '-') break; }
      else
        { if (!isalnum((uschar)id[i])) break; }
      }
    if (i < MESSAGE_ID_LENGTH) continue;

    strncpy(buffer+path_len, id, MESSAGE_ID_LENGTH);
    sprintf(buffer+path_len + MESSAGE_ID_LENGTH, "-D");

    if (stat(buffer, &statbuf) != 0)
      {
      dbfn_delete(dbm, key);
      printf("deleted %s (no message)\n", key);
      }
    }
  }

dbfn_close(dbm);
return 0;
}

/* End of exim_tidydb.c */
