/* A program to lock a file exactly as Exim would, for investigation of
interlocking problems.

Options:  -fcntl    use fcntl lock only
          -lockfile use lock file only
          -mbx      use mbx-lock only

Default is -fcntl -lockfile.

Argument: the name of the lock file
*/

#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <pwd.h>


#define FALSE 0
#define TRUE  1


/* Flag for timeout signal handler */

static int sigalrm_seen = FALSE;


/* We need to pull in strerror() and os_non_restarting_signal() from the
os.c source, if they are required for this OS. However, we don't need any of
the other stuff in os.c, so force the other macros to omit it. */

#ifndef OS_RESTARTING_SIGNAL
  #define OS_RESTARTING_SIGNAL
#endif

#ifndef OS_STRSIGNAL
  #define OS_STRSIGNAL
#endif

#ifndef OS_STREXIT
  #define OS_STREXIT
#endif

#ifndef OS_LOAD_AVERAGE
  #define OS_LOAD_AVERAGE
#endif

#ifndef FIND_RUNNING_INTERFACES
  #define FIND_RUNNING_INTERFACES
#endif

#include "../src/os.c"



/*************************************************
*             Timeout handler                    *
*************************************************/

static void
sigalrm_handler(int sig)
{
sig = sig;      /* Keep picky compilers happy */
sigalrm_seen = TRUE;
}



/*************************************************
*           Give usage and die                   *
*************************************************/

static void
usage(void)
{
printf("usage: exim_lock [-v] [-q] [-lockfile] [-fcntl | -mbx]\n"
       "       [-retries <n>] [-interval <n>] [-timeout <n>]\n"
       "       <file name> [command]\n");
exit(1);
}



/*************************************************
*         Apply a lock to a file descriptor      *
*************************************************/

static int
apply_lock(int fd, int type, int timeout)
{
int yield;
struct flock lock_data;
lock_data.l_type = type;
lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;

sigalrm_seen = FALSE;

if (timeout > 0)
  {
  int save_errno;
  os_non_restarting_signal(SIGALRM, sigalrm_handler);
  alarm(timeout);
  yield = fcntl(fd, F_SETLKW, &lock_data);
  save_errno = errno;
  alarm(0);
  signal(SIGALRM, SIG_IGN);
  errno = save_errno;
  }
else yield = fcntl(fd, F_SETLK, &lock_data);

return yield;
}



/*************************************************
*           The exim_lock program                *
*************************************************/

int main(int argc, char **argv)
{
int lock_retries = 10;
int lock_interval = 3;
int lock_fcntl_timeout = 0;
int use_lockfile = FALSE;
int use_fcntl = FALSE;
int use_mbx = FALSE;
int verbose = FALSE;
int quiet = FALSE;
int i, j, len;
int fd = -1;
int hd = -1;
int md = -1;
int yield = 0;
int now = time(NULL);
char *filename;
char *lockname = NULL, *hitchname = NULL;
char *primary_hostname, *command;
struct utsname s;
char buffer[256];
char tempname[256];

/* Decode options */

for (i = 1; i < argc; i++)
  {
  char *arg = argv[i];
  if (*arg != '-') break;
  if (strcmp(arg, "-fcntl") == 0) use_fcntl = TRUE;
  else if (strcmp(arg, "-lockfile") == 0) use_lockfile = TRUE;
  else if (strcmp(arg, "-mbx") == 0) use_mbx = TRUE;
  else if (strcmp(arg, "-v") == 0) verbose = TRUE;
  else if (strcmp(arg, "-q") == 0) quiet = TRUE;
  else if (++i < argc)
    {
    int value = atoi(argv[i]);
    if (strcmp(arg, "-retries") == 0) lock_retries = value;
    else if (strcmp(arg, "-interval") == 0) lock_interval = value;
    else if (strcmp(arg, "-timeout") == 0) lock_fcntl_timeout = value;
    else usage();
    }
  else usage();
  }

if (quiet) verbose = 0;

/* Default is to use both non-MBX kinds of lock; fcntl and mbx are exclusive */

if (!use_lockfile && !use_fcntl && !use_mbx) use_lockfile = use_fcntl = TRUE;
if (use_fcntl && use_mbx) usage();

/* A file name is required */

if (i >= argc) usage();

filename = argv[i++];

/* Expand file names starting with ~ */

if (*filename == '~')
  {
  struct passwd *pw;

  if (*(++filename) == '/')
    pw = getpwuid(getuid());
  else
    {
    char *s = buffer;
    while (*filename != 0 && *filename != '/')
      *s++ = *filename++;
    *s = 0;
    pw = getpwnam(buffer);
    }

  if (pw == NULL)
    {
    printf("exim_lock: unable to expand file name %s\n", argv[i-1]);
    exit(1);
    }

  if ((int)strlen(pw->pw_dir) + (int)strlen(filename) + 1 > sizeof(buffer))
    {
    printf("exim_lock: expanded file name %s%s is too long", pw->pw_dir,
      filename);
    exit(1);
    }

  strcpy(buffer, pw->pw_dir);
  strcat(buffer, filename);
  filename = buffer;
  }

/* If using a lock file, prepare by creating the lock file name and
the hitching post name. */

if (use_lockfile)
  {
  if (uname(&s) < 0)
    {
    printf("exim_lock: failed to find host name using uname()\n");
    exit(1);
    }
  primary_hostname = s.nodename;

  len = (int)strlen(filename);
  lockname = malloc(len + 8);
  sprintf(lockname, "%s.lock", filename);
  hitchname = malloc(len + 32 + (int)strlen(primary_hostname));
  sprintf(hitchname, "%s.%s.%08x.%08x", lockname, primary_hostname,
    now, (int)getpid());

  if (verbose)
    printf("exim_lock: lockname =  %s\n           hitchname = %s\n", lockname,
      hitchname);
  }

/* Locking retry loop */

for (j = 0; j < lock_retries; j++)
  {
  int sleep_before_retry = TRUE;
  struct stat statbuf;

  /* Try to build a lock file if so configured */

  if (use_lockfile)
    {
    int rc;
    if (verbose) printf("exim_lock: creating lock file\n");
    hd = open(hitchname, O_WRONLY | O_CREAT | O_EXCL, 0440);
    if (hd < 0)
      {
      printf("exim_lock: failed to create hitching post %s: %s\n", hitchname,
        strerror(errno));
      exit(1);
      }

    /* Apply hitching post algorithm. */

    if ((rc = link(hitchname, lockname)) != 0) fstat(hd, &statbuf);
    close(hd);
    unlink(hitchname);

    if (rc != 0 && statbuf.st_nlink != 2)
      {
      printf("exim_lock: failed to link hitching post to lock file\n");
      hd = -1;
      goto RETRY;
      }

    if (!quiet) printf("exim_lock: lock file successfully created\n");
    }

  /* We are done if no other locking required. */

  if (!use_fcntl && !use_mbx) break;

  /* Open the file for writing. */

  fd = open(filename, O_RDWR + O_APPEND);
  if (fd < 0)
    {
    printf("exim_lock: failed to open %s for writing: %s\n", filename,
      strerror(errno));
    yield = 1;
    goto CLEAN_UP;
    }

  /* If there is a timeout, implying blocked locking, we don't want to
  sleep before any retries after this. */

  if (lock_fcntl_timeout > 0) sleep_before_retry = FALSE;

  /* Lock using fcntl. There are pros and cons to using a blocking call vs
  a non-blocking call and retries. Exim is non-blocking by default, but setting
  a timeout changes it to blocking. */

  if (!use_mbx)
    {
    if (apply_lock(fd, F_WRLCK, lock_fcntl_timeout) >= 0)
      {
      if (!quiet) printf("exim_lock: fcntl() lock successfully applied\n");
      break;
      }
    else
      {
      printf("exim_lock: fcntl() failed: %s\n", strerror(errno));
      goto RETRY;
      }
    }

  /* Lock using MBX rules. This is complicated and is documented with the
  source of the c-client library that goes with Pine and IMAP. What has to
  be done to interwork correctly is to take out a shared lock on the mailbox,
  and an exclusive lock on a /tmp file. */

  else
    {
    if (apply_lock(fd, F_RDLCK, lock_fcntl_timeout) >= 0)
      {
      if (!quiet) printf("exim_lock: fcntl() read lock successfully applied\n");
      }
    else
      {
      printf("exim_lock: fcntl() read lock failed: %s\n", strerror(errno));
      goto RETRY;
      }

    if (fstat(fd, &statbuf) < 0)
      {
      printf("exim_lock: fstat() of %s failed: %s\n", filename,
        strerror(errno));
      yield = 1;
      goto CLEAN_UP;
      }

    /* Set up file in /tmp and check its state if already existing. */

    sprintf(tempname, "/tmp/.%lx.%lx", (long)statbuf.st_dev,
      (long)statbuf.st_ino);

    if (lstat(tempname, &statbuf) >= 0)
      {
      if ((statbuf.st_mode & S_IFMT) == S_IFLNK)
        {
        printf("exim_lock: symbolic link on lock name %s\n", tempname);
        yield = 1;
        goto CLEAN_UP;
        }
      if (statbuf.st_nlink > 1)
        {
        printf("exim_lock: hard link to lock name %s\n", tempname);
        yield = 1;
        goto CLEAN_UP;
        }
      }

    md = open(tempname, O_RDWR | O_CREAT, 0600);
    if (md < 0)
      {
      printf("exim_lock: failed to create mbx lock file %s: %s\n",
        tempname, strerror(errno));
      goto CLEAN_UP;
      }

    chmod (tempname, 0600);

    if (apply_lock(md, F_WRLCK, lock_fcntl_timeout) >= 0)
      {
      if (!quiet) printf("exim_lock: fcntl() lock successfully applied to mbx "
        "lock file %s\n", tempname);
      break;
      }
    else
      {
      printf("exim_lock: fcntl() failed for %s: %s\n", tempname,
        strerror(errno));
      goto RETRY;
      }
    }

  /* Clean up before retrying */

  RETRY:

  if (md >= 0)
    {
    if (close(md) < 0)
      printf("exim_lock: close %s failed: %s\n", tempname, strerror(errno));
    else
      if (!quiet) printf("exim_lock: %s closed\n", tempname);
    md = -1;
    }

  if (fd >= 0)
    {
    if (close(fd) < 0)
      printf("exim_lock: close failed: %s\n", strerror(errno));
    else
      if (!quiet) printf("exim_lock: file closed\n");
    fd = -1;
    }

  if (hd >= 0)
    {
    if (unlink(lockname) < 0)
      printf("exim_lock: unlink of %s failed: %s\n", lockname, strerror(errno));
    else
      if (!quiet) printf("exim_lock: lock file removed\n");
    hd = -1;
    }

  /* If a blocking call timed out, break the retry loop if the total time
  so far is not less than than retries * interval. */

  if (sigalrm_seen &&
      (j + 1) * lock_fcntl_timeout >= lock_retries * lock_interval)
    j = lock_retries;

  /* Wait a bit before retrying, except when it was a blocked fcntl() that
  caused the problem. */

  if (j < lock_retries && sleep_before_retry)
    {
    printf(" ... waiting\n");
    sleep(lock_interval);
    }
  }

if (j >= lock_retries)
  {
  printf("exim_lock: locking failed too many times\n");
  yield = 1;
  goto CLEAN_UP;
  }

if (!quiet) printf("exim_lock: locking %s succeeded: ", filename);

/* If there are no further arguments, run the user's shell; otherwise
the next argument is a command to run. */

if (i >= argc)
  {
  command = getenv("SHELL");
  if (command == NULL || *command == 0) command = "/bin/sh";
  if (!quiet) printf("running %s ...\n", command);
  }
else
  {
  command = argv[i];
  if (!quiet) printf("running the command ...\n");
  }

/* Run the command */

system(command);

/* Remove the locks and exit. Unlink the /tmp file if we can get an exclusive
lock on the mailbox. This should be a non-blocking lock call, as there is no
point in waiting. */

CLEAN_UP:

if (md >= 0)
  {
  struct flock lock_data;
  lock_data.l_type = F_WRLCK;
  lock_data.l_whence = lock_data.l_start = lock_data.l_len = 0;
  if (fcntl(fd, F_SETLK, &lock_data) >= 0)
    {
    if (!quiet) printf("exim_lock: %s unlinked - no sharers\n", tempname);
    unlink(tempname);
    }
  else if (!quiet)
    printf("exim_lock: %s not unlinked - unable to get exclusive mailbox lock\n",
      tempname);
  if (close(md) < 0)
    printf("exim_lock: close %s failed: %s\n", tempname, strerror(errno));
  else
    if (!quiet) printf("exim_lock: %s closed\n", tempname);
  }

if (fd >= 0)
  {
  if (close(fd) < 0)
    printf("exim_lock: close %s failed: %s\n", filename, strerror(errno));
  else
    if (!quiet) printf("exim_lock: %s closed\n", filename);
  }

if (hd >= 0)
  {
  if (unlink(lockname) < 0)
    printf("exim_lock: unlink %s failed: %s\n", lockname, strerror(errno));
  else
    if (!quiet) printf("exim_lock: lock file removed\n");
  }

return yield;
}

/* End */
