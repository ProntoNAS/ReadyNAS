/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "exim.h"
#include <setjmp.h>


/* Functions for accessing Exim's database, which consists of a number
of different DBM files. This module does not contain code for reading DBM
files for (e.g.) alias expansion. That is all contained within the general
search functions. As Exim now has support for the ndbm interface and the native
db 1.85 and 2.x interfaces, all the relevant functions are called as macros.

All the data in Exim's database is in the nature of *hints*. Therefore it
doesn't matter if it gets destroyed by accident. These functions are not
supposed to implement a "safe" database.

Keys are passed in as C strings, and the terminating zero *is* used when
building the dbm files. This just makes life easier when scanning the files
sequentially.

Synchronization is required on the database files, and this is achieved by
means of locking on independent lock files. (Earlier attempts to lock on the
DBM files themselves were never completely successful.) Since callers may in
general want to do more than one read or write while holding the lock, there
are separate open and close functions. However, the calling modules should
arrange to hold the locks for the bare minimum of time. */

/* Environment save for the longjmp() that catches segv errors while reading.
This was implemented in the days when locking was on the DBM files themselves;
this is now believed to have been the cause of such errors, but leave the code
in existence just in case. */

static jmp_buf read_env;


/*************************************************
*        Signal handler for segv while reading   *
*************************************************/

/* This is used only while reading from a dbm file; segv crashes have
occurred and this is a damage limitation exercise. With the change to locking
on a separate file, this is believed not to be likely to happen any more. */

static void
segv_handler(int signal)
{
signal = signal;       /* Keep picky compilers happy */
longjmp(read_env, 1);
}



/*************************************************
*          Open and lock a database file         *
*************************************************/

/* Used for accessing Exim's hints databases.

Arguments:
  name     The single-component name of one of Exim's database files.
  flags    Standard O_xxx flags indicating the type of open required.
  dbblock  Points to an open_db block to be filled in.
  lof      If TRUE, write to the log for actual open failures (locking failures
           are always logged).

Returns:   NULL if the open failed, or the locking failed. After locking
           failures, errno is zero.

           On success, dbblock is returned. This contains the dbm pointer and
           the fd of the locked lock file.
*/

open_db *
dbfn_open(char *name, int flags, open_db *dbblock, BOOL lof)
{
int rc;
BOOL read_only = flags == O_RDONLY;
flock_t lock_data;
char buffer[256];

/* The first thing to do is to open a separate file on which to lock. This
ensures that Exim has exclusive use of the database before it even tries to
open it. Previous versions tried to lock on the open database itself, but that
gave rise to mysterious problems from time to time - it was suspected that some
DB libraries "do things" on their open() calls which break the interlocking.
The lock file is never written to, but we open it for writing to ensure it gets
created, and we also try to make the directory if the initial open fails. */

sprintf(buffer, "%s/db/%s.lockfile", spool_directory, name);
if ((dbblock->lockfd = open(buffer, O_RDWR|O_CREAT, DB_LOCKFILE_MODE)) < 0)
  {
  (void)directory_make(spool_directory, "db", DB_DIRECTORY_MODE, TRUE);
  dbblock->lockfd = open(buffer, O_RDWR|O_CREAT, DB_LOCKFILE_MODE);
  }

/* If the open for writing fails with permission denied, and this is a
read-only call, try to open the file for reading instead. This allows group
read access the databases for utilities like exim_dumpdb to work without having
to set up write access to the lock file. */

if (dbblock->lockfd < 0)
  {
  if (errno == EACCES && read_only)
    dbblock->lockfd = open(buffer, O_RDONLY, DB_LOCKFILE_MODE);
  if (dbblock->lockfd < 0)
    {
    log_write(0, LOG_MAIN, "%s",
      string_open_failed(errno, "database lock file %s", buffer));
    errno = 0;      /* Indicates locking failure */
    return NULL;
    }
  }

/* Now we must get a lock on the opened lock file; do this with a blocking
lock that times out. */

lock_data.l_type = read_only? F_RDLCK : F_WRLCK;
lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;

sigalrm_seen = FALSE;
os_non_restarting_signal(SIGALRM, sigalrm_handler);
alarm(DB_LOCK_TIMEOUT_EXIM);
rc = fcntl(dbblock->lockfd, F_SETLKW, &lock_data);
alarm(0);
signal(SIGALRM, SIG_IGN);

if (sigalrm_seen) errno = ETIMEDOUT;
if (rc < 0)
  {
  log_write(0, LOG_MAIN, "Failed to get %s lock for %s: %s",
    ((flags & O_RDONLY) != 0)? "read" : "write", buffer,
    (errno == ETIMEDOUT)? "timed out" : strerror(errno));
  close(dbblock->lockfd);
  errno = 0;       /* Indicates locking failure */
  return NULL;
  }

DEBUG(9) debug_printf("locked %s\n", buffer);

/* At this point we have an opened and locked separate lock file, that is,
exclusive access to the database, so we can go ahead and open it. */

sprintf(buffer, "%s/db/%s", spool_directory, name);
EXIM_DBOPEN(buffer, flags, DB_MODE, &(dbblock->dbptr));

/* If we're using GDBM, then opening for reading doesn't create a new file.
Arguably, this is correct behaviour. If this happens, we just create the
file, leaving it open for both reading and writing. */

if (dbblock->dbptr == NULL && errno == ENOENT && flags == O_RDONLY)
  EXIM_DBOPEN(buffer, O_RDWR|O_CREAT, DB_MODE, &(dbblock->dbptr));

/* If the open has failed, return NULL, leaving errno set. If lof is TRUE,
log the event - also for debugging - but not if the file just doesn't exist. */

if (dbblock->dbptr == NULL)
  {
  int save_errno = errno;
  if ((lof || debug_level >= 9) && errno != ENOENT)
    log_write(0, LOG_MAIN, "%s",
      string_open_failed(errno, "DB file %s", buffer));
  close(dbblock->lockfd);
  errno = save_errno;
  return NULL;
  }

/* All is well */

DEBUG(9) debug_printf("opened DB file %s: flags=%x\n", buffer, flags);
return dbblock;
}




/*************************************************
*         Unlock and close a database file       *
*************************************************/

/* Closing a file automatically unlocks it, so after closing the database, just
close the lock file.

Argument: a pointer to an open database block
Returns:  nothing
*/

void
dbfn_close(open_db *dbblock)
{
EXIM_DBCLOSE(dbblock->dbptr);
close(dbblock->lockfd);
}




/*************************************************
*             Read from database file            *
*************************************************/

/* Passing back the pointer unchanged is useless, because there is
no guarantee of alignment. Since all the records used by Exim need
to be properly aligned to pick out the timestamps, etc., we might as
well do the copying centrally here.

Most calls don't need the length, so there is a macro called dbfn_read which
has two arguments; it calls this function adding NULL as the third.

Arguments:
  dbblock   a pointer to an open database block
  key       the key of the record to be read
  length    a pointer to an int into which to return the length, if not NULL

Returns: a pointer to the retrieved record, or
         NULL if the record is not found
*/

void *
dbfn_read_with_length(open_db *dbblock, char *key, int *length)
{
void *yield;
EXIM_DATUM key_datum, result_datum;

DEBUG(9) debug_printf("dbfn_read: key=%s\n", key);

EXIM_DATUM_INIT(key_datum);         /* Some DBM libraries require the datum */
EXIM_DATUM_INIT(result_datum);      /* to be cleared before use. */
EXIM_DATUM_DATA(key_datum) = key;
EXIM_DATUM_SIZE(key_datum) = (int)strlen(key) + 1;

/* There have been problems on various systems that manifest themselves
as SEGV crashes inside DBM libraries. As yet it is unknown whether these
are the fault of Exim or not. However, in order to limit the amount of damage
that such crashes cause, we attempt to catch SEGV signals here, and if one
happens, log the incident and behave as if the record did not exist in the
database. To do this we have to use the longjmp() mechanism. */

if (setjmp(read_env) == 0)
  {
  BOOL found;
  signal(SIGSEGV, segv_handler);
  found = EXIM_DBGET(dbblock->dbptr, key_datum, result_datum);
  signal(SIGSEGV, SIG_DFL);
  if (!found) return NULL;
  yield = store_get(EXIM_DATUM_SIZE(result_datum));
  memcpy(yield, EXIM_DATUM_DATA(result_datum), EXIM_DATUM_SIZE(result_datum));
  if (length != NULL) *length = EXIM_DATUM_SIZE(result_datum);
  EXIM_DATUM_FREE(result_datum);    /* Some DBM libs require freeing */
  return yield;
  }

/* Caught a SEGV. Returning NULL from here is often interperted as "no retry
data for this domain", and that might cause frequent subsequent attempts to
read the same data. That in turn could cause a heck of a lot of data to get
written to the logs. Therefore, insert a short delay. */

else
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "SEGV while reading \"%s\" from "
    "dbm file: record assumed not to exist", key);
  sleep(1);
  return NULL;
  }
}



/*************************************************
*             Write to database file             *
*************************************************/

/*
Arguments:
  dbblock   a pointer to an open database block
  key       the key of the record to be written
  ptr       a pointer to the record to be written
  length    the length of the record to be written

Returns:    the yield of the underlying dbm or db "write" function. If this
            is dbm, the value is zero for OK.
*/

int
dbfn_write(open_db *dbblock, char *key, void *ptr, int length)
{
EXIM_DATUM key_datum, value_datum;
dbdata_generic *gptr = (dbdata_generic *)ptr;
gptr->time_stamp = time(NULL);

DEBUG(9) debug_printf("dbfn_write: key=%s\n", key);

EXIM_DATUM_INIT(key_datum);         /* Some DBM libraries require the datum */
EXIM_DATUM_INIT(value_datum);       /* to be cleared before use. */
EXIM_DATUM_DATA(key_datum) = key;
EXIM_DATUM_SIZE(key_datum) = (int)strlen(key) + 1;
EXIM_DATUM_DATA(value_datum) = (char *)ptr;
EXIM_DATUM_SIZE(value_datum) = length;
return EXIM_DBPUT(dbblock->dbptr, key_datum, value_datum);
}



/*************************************************
*           Delete record from database file     *
*************************************************/

/*
Arguments:
  dbblock    a pointer to an open database block
  key        the key of the record to be deleted

Returns: the yield of the underlying dbm or db "delete" function.
*/

int
dbfn_delete(open_db *dbblock, char *key)
{
EXIM_DATUM key_datum;
EXIM_DATUM_INIT(key_datum);         /* Some DBM libraries require clearing */
EXIM_DATUM_DATA(key_datum) = key;
EXIM_DATUM_SIZE(key_datum) = (int)strlen(key) + 1;
return EXIM_DBDEL(dbblock->dbptr, key_datum);
}



/*************************************************
*         Scan the keys of a database file       *
*************************************************/

/*
Arguments:
  dbblock  a pointer to an open database block
  start    TRUE if starting a new scan
           FALSE if continuing with the current scan
  cursor   a pointer to a pointer to a cursor anchor, for those dbm libraries
           that use the notion of a cursor

Returns:   the next record from the file, or
           NULL if there are no more
*/

char *
dbfn_scan(open_db *dbblock, BOOL start, EXIM_CURSOR **cursor)
{
EXIM_DATUM key_datum, value_datum;
char *yield;
value_datum = value_datum;    /* dummy; not all db libraries use this */

/* Some dbm require an initialization */

if (start) EXIM_DBCREATE_CURSOR(dbblock->dbptr, cursor);

EXIM_DATUM_INIT(key_datum);         /* Some DBM libraries require the datum */
EXIM_DATUM_INIT(value_datum);       /* to be cleared before use. */

yield = (EXIM_DBSCAN(dbblock->dbptr, key_datum, value_datum, start, *cursor))?
  EXIM_DATUM_DATA(key_datum) : NULL;

/* Some dbm require a termination */

if (!yield) EXIM_DBDELETE_CURSOR(*cursor);
return yield;
}



/*************************************************
**************************************************
*             Stand-alone test program           *
**************************************************
*************************************************/

#ifdef STAND_ALONE


void
stat_file(char *name)
{
struct stat statbuf;
if (stat(name, &statbuf) != 0)
  {
  printf("Failed to stat %s: %s\n", name, strerror(errno));
  return;
  }
printf("%s: uid = %ld gid = %ld mode = %04o size=%d blksize=%d\n",
  name, (long int)(statbuf.st_uid), (long int)(statbuf.st_gid),
  (int)statbuf.st_mode & 07777,
  (int)statbuf.st_size, (int)statbuf.st_blksize);
}


int
main(int argc, char **argv)
{
open_db dbblock[8];
int max_db = sizeof(dbblock)/sizeof(open_db);
int current = -1;
int showtime = 0;
int i;
dbdata_wait *dbwait = NULL;
char buffer[256];
char structbuffer[1024];

if (argc != 2)
  {
  printf("Usage: test_dbfn directory\n");
  printf("The subdirectory called \"db\" in the given directory is used for\n");
  printf("the files used in this test program.\n");
  return 1;
  }

/* Initialize */

spool_directory = argv[1];
debug_level = 10;
debug_file = stderr;
big_buffer = malloc(big_buffer_size);

for (i = 0; i < max_db; i++) dbblock[i].dbptr = NULL;

printf("\nExim's db functions tester: interface type is %s\n", EXIM_DBTYPE);
printf("DBM library: ");

#ifdef DB_VERSION_STRING
printf("Berkeley DB: %s\n", DB_VERSION_STRING);
#elif defined(BTREEVERSION) && defined(HASHVERSION)
  #ifdef USE_DB
  printf("probably Berkeley DB version 1.8x (native mode)\n");
  #else
  printf("probably Berkeley DB version 1.8x (compatibility mode)\n");
  #endif
#elif defined(_DBM_RDONLY) || defined(dbm_dirfno)
printf("probably ndbm\n");
#elif defined(USE_TDB)
printf("using tdb\n");
#else
printf("probably GDBM\n");
#endif

/* Show the effects of certain macros */

#ifdef USE_DB
printf("USE_DB is defined\n");
#else
printf("USE_DB is not defined\n");
#endif


/* Test the functions */

printf("\nTest the functions\n> ");

while (fgets(buffer, 256, stdin) != NULL)
  {
  int len = (int)strlen(buffer);
  int count = 1;
  clock_t start = 1;
  clock_t stop = 0;
  char *cmd = buffer;
  while (len > 0 && isspace((uschar)buffer[len-1])) len--;
  buffer[len] = 0;

  if (isdigit((uschar)*cmd))
    {
    count = atoi(cmd);
    while (isdigit((uschar)*cmd)) cmd++;
    while (isspace((uschar)*cmd)) cmd++;
    }

  if (strncmp(cmd, "open", 4) == 0)
    {
    int i;
    open_db *odb;
    char *s = cmd + 4;
    while (isspace((uschar)*s)) s++;

    for (i = 0; i < max_db; i++)
      if (dbblock[i].dbptr == NULL) break;

    if (i >= max_db)
      {
      printf("Too many open databases\n> ");
      continue;
      }

    start = clock();
    odb = dbfn_open(s, O_RDWR|O_CREAT, dbblock + i, TRUE);
    stop = clock();

    if (odb != NULL)
      {
      current = i;
      printf("opened %d\n", current);
      }
    }

  else if (strncmp(cmd, "write", 5) == 0)
    {
    int rc = 0;
    char *key = cmd + 5;
    char *data;

    if (current < 0)
      {
      printf("No current database\n");
      continue;
      }

    while (isspace((uschar)*key)) key++;
    data = key;
    while (*data != 0 && !isspace((uschar)*data)) data++;
    *data++ = 0;
    while (isspace((uschar)*data)) data++;

    dbwait = (dbdata_wait *)(&structbuffer);
    strcpy(dbwait->text, data);

    start = clock();
    while (count-- > 0)
      rc = dbfn_write(dbblock + current, key, dbwait,
        (int)strlen(data) + sizeof(dbdata_wait));
    stop = clock();
    if (rc != 0) printf("Failed: %s\n", strerror(errno));
    }

  else if (strncmp(cmd, "read", 4) == 0)
    {
    char *key = cmd + 4;
    if (current < 0)
      {
      printf("No current database\n");
      continue;
      }
    while (isspace((uschar)*key)) key++;
    start = clock();
    while (count-- > 0)
      dbwait = (dbdata_wait *)dbfn_read_with_length(dbblock+ current, key, NULL);
    stop = clock();
    printf("%s\n", (dbwait == NULL)? "<not found>" : dbwait->text);
    }

  else if (strncmp(cmd, "delete", 6) == 0)
    {
    char *key = cmd + 6;
    if (current < 0)
      {
      printf("No current database\n");
      continue;
      }
    while (isspace((uschar)*key)) key++;
    dbfn_delete(dbblock + current, key);
    }

  else if (strncmp(cmd, "scan", 4) == 0)
    {
    EXIM_CURSOR *cursor;
    BOOL startflag = TRUE;
    char *key;
    char keybuffer[256];
    if (current < 0)
      {
      printf("No current database\n");
      continue;
      }
    start = clock();
    while ((key = dbfn_scan(dbblock + current, startflag, &cursor)) != NULL)
      {
      startflag = FALSE;
      strcpy(keybuffer, key);
      dbwait = (dbdata_wait *)dbfn_read_with_length(dbblock + current,
        keybuffer, NULL);
      printf("%s: %s\n", keybuffer, dbwait->text);
      }
    stop = clock();
    printf("End of scan\n");
    }

  else if (strncmp(cmd, "close", 5) == 0)
    {
    char *s = cmd + 5;
    while (isspace((uschar)*s)) s++;
    i = atoi(s);
    if (i >= max_db || dbblock[i].dbptr == NULL) printf("Not open\n"); else
      {
      start = clock();
      dbfn_close(dbblock + i);
      stop = clock();
      dbblock[i].dbptr = NULL;
      if (i == current) current = -1;
      }
    }

  else if (strncmp(cmd, "file", 4) == 0)
    {
    char *s = cmd + 4;
    while (isspace((uschar)*s)) s++;
    i = atoi(s);
    if (i >= max_db || dbblock[i].dbptr == NULL) printf("Not open\n");
      else current = i;
    }

  else if (strncmp(cmd, "time", 4) == 0) showtime = ~showtime;

  else if (strcmp(cmd, "q") == 0 || strncmp(cmd, "quit", 4) == 0) break;

  else if (strncmp(cmd, "help", 4) == 0)
    {
    printf("close <number>\n");
    printf("file  <number>\n");
    printf("open  <name>\n");
    printf("quit\n");
    printf("read  <key>\n");
    printf("write <key> <rest-of-line>\n");
    }

  else printf("Eh?\n");

  if (showtime && stop >= start)
    printf("start=%d stop=%d difference=%d\n", (int)start, (int)stop,
     ( int)(stop - start));

  printf("> ");
  }

for (i = 0; i < max_db; i++)
  {
  if (dbblock[i].dbptr != NULL)
    {
    printf("\nClosing %d", i);
    dbfn_close(dbblock + i);
    }
  }

printf("\n");
return 0;
}

#endif

/* End of dbfn.c */
