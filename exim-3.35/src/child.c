/*************************************************
*     Exim - an Internet mail transport agent    *
*************************************************/

/* Copyright (c) University of Cambridge 1995 - 2001 */
/* See the file NOTICE for conditions of use and distribution. */


#include "exim.h"

static void (*oldsignal)(int);


/*************************************************
*          Ensure an fd has a given value        *
*************************************************/

/* This function is called when we want to ensure that a certain fd has a
specific value (one of 0, 1, 2). If it hasn't got it already, close the value
we want, duplicate the fd, then close the old one.

Arguments:
  oldfd        original fd
  newfd        the fd we want

Returns:       nothing
*/

static void
force_fd(int oldfd, int newfd)
{
if (oldfd == newfd) return;
close(newfd);
dup2(oldfd, newfd);
close(oldfd);
}



/*************************************************
*          Create a child Exim process           *
*************************************************/

/* This function creates a child process and runs Exim in it. It sets up a pipe
to the standard input of the new process, and returns that to the caller via
fdptr. The function returns the pid of the new process, or -1 if things go
wrong. If debug_fd is non-negative, it is passed as stderr.

Argument: fdptr   pointer to int for the stdin fd
Returns:          pid of the created process or -1 if anything has gone wrong
*/

pid_t
child_open_exim(int *fdptr)
{
int pfd[2];
int save_errno;
pid_t pid;

/* Create the pipe and fork the process. Ensure that SIGCHLD is set to
SIG_DFL before forking, so that the child process can be waited for. We
sometimes get here with it set otherwise. Save the old state for resetting
on the wait. */

if (pipe(pfd) != 0) return (pid_t)(-1);
oldsignal = signal(SIGCHLD, SIG_DFL);
pid = fork();

/* Child process: make the reading end of the pipe into the standard input and
close the writing end. If debugging, pass debug_fd as stderr. Then re-exec
Exim. Failure is signalled with EX_UNAVAILABLE, but this shouldn't occur! */

if (pid == 0)
  {
  force_fd(pfd[pipe_read], 0);
  close(pfd[pipe_write]);
  if (debug_fd > 0) force_fd(debug_fd, 2);
  execv(mailer_argv[0], mailer_argv);
  _exit(EX_UNAVAILABLE);      /* Note: must be _exit(), NOT exit() */
  }

/* Parent process. Save fork() errno and close the reading end of the stdin
pipe. */

save_errno = errno;
close(pfd[pipe_read]);

/* Fork succeeded */

if (pid > 0)
  {
  *fdptr = pfd[pipe_write];   /* return writing end of stdin pipe */
  return pid;                 /* and pid of new process */
  }

/* Fork failed */

close(pfd[pipe_write]);
errno = save_errno;
return (pid_t)(-1);
}




/*************************************************
*         Create a non-Exim child process        *
*************************************************/

/* This function creates a child process and runs the given command in it. It
sets up pipes to the standard input and output of the new process, and returns
them to the caller. The standard error is cloned to the output. If there are
any file descriptors "in the way" in the new process, they are closed. A new
umask is supplied for the process, and an optional new uid and gid are also
available. These are used by the queryprogram router to set an unprivileged id.
The function returns the pid of the new process, or -1 if things go wrong.

Arguments:
  argv        the argv for exec in the new process
  envp        the envp for exec in the new process
  newumask    umask to set in the new process
  newuid      point to uid for the new process or NULL for no change
  newgid      point to gid for the new process or NULL for no change
  infdptr     pointer to int into which the fd of the stdin of the new process
                is placed
  outfdptr    pointer to int into which the fd of the stdout/stderr of the new
                process is placed
  wd          if not NULL, a path to be handed to chdir() in the new process
  make_leader if TRUE, make the new process a process group leader

Returns:      the pid of the created process or -1 if anything has gone wrong
*/

pid_t
child_open(char **argv, char **envp, int newumask, uid_t *newuid, gid_t *newgid,
  int *infdptr, int *outfdptr, char *wd, BOOL make_leader)
{
int save_errno;
int inpfd[2], outpfd[2];
pid_t pid;

/* Create the pipes. */

if (pipe(inpfd) != 0) return (pid_t)(-1);
if (pipe(outpfd) != 0)
  {
  close(inpfd[pipe_read]);
  close(inpfd[pipe_write]);
  return (pid_t)(-1);
  }

/* Fork the process. Ensure that SIGCHLD is set to SIG_DFL before forking, so
that the child process can be waited for. We sometimes get here with it set
otherwise. Save the old state for resetting on the wait. */

oldsignal = signal(SIGCHLD, SIG_DFL);
pid = fork();

/* The child process becomes a process group leader if requested, and then
organizes the pipes. Any unexpected failure is signalled with EX_UNAVAILBLE;
these are all "should never occur" failures, except perhaps for exec failing
because the command doesn't exist. */

if (pid == 0)
  {
  if (make_leader && setpgid(0,0) < 0) goto CHILD_FAILED;

  close(inpfd[pipe_write]);
  force_fd(inpfd[pipe_read], 0);

  close(outpfd[pipe_read]);
  force_fd(outpfd[pipe_write], 1);

  close(2);
  dup2(1, 2);

  /* Recover the power to setuid if necessary. We shouldn't be
  here not as root if it isn't available. */

  if (geteuid() != root_uid) mac_seteuid(root_uid);

  /* Set the required environment. If changing uid, ensure that
  SIGUSR1 is ignored, as the process won't have the privilege to
  write to the process log. */

  if (newgid != NULL && setgid(*newgid) < 0) goto CHILD_FAILED;
  if (newuid != NULL)
    {
    signal(SIGUSR1, SIG_IGN);
    if (setuid(*newuid) < 0) goto CHILD_FAILED;
    }
  (void)umask(newumask);

  /* Set the working directory if required */

  if (wd != NULL && chdir(wd) < 0) goto CHILD_FAILED;

  /* Now do the exec */

  if (envp == NULL) execv(argv[0], argv);
    else execve(argv[0], argv, envp);

  /* Failed to execv. Signal this failure using EX_UNAVAILABLE. We are
  losing the actual errno we got back, because there is no way to return
  this. */

  CHILD_FAILED:
  _exit(EX_UNAVAILABLE);      /* Note: must be _exit(), NOT exit() */
  }

/* Parent process. Save any fork failure code, and close the reading end of the
stdin pipe, and the writing end of the stdout pipe. */

save_errno = errno;
close(inpfd[pipe_read]);
close(outpfd[pipe_write]);

/* Fork succeeded; return the input/output pipes and the pid */

if (pid > 0)
  {
  *infdptr = inpfd[pipe_write];
  *outfdptr = outpfd[pipe_read];
  return pid;
  }

/* Fork failed; reset fork errno before returning */

close(inpfd[pipe_write]);
close(outpfd[pipe_read]);
errno = save_errno;
return (pid_t)(-1);
}


/*************************************************
*           Close down child process             *
*************************************************/

/* Wait for the given process to finish, with optional timeout.

Arguments
  pid:      the pid to wait for
  timeout:  maximum time to wait; 0 means for as long as it takes

Returns:    >= 0          process terminated by exiting; value is process
                            ending status
            < 0 & > -256  process was terminated by a signal; value is the
                            negation of the signal number
            -256          timed out
            -257          other error in wait(); errno still set
*/

int
child_close(pid_t pid, int timeout)
{
int yield;

if (timeout > 0)
  {
  sigalrm_seen = FALSE;
  os_non_restarting_signal(SIGALRM, sigalrm_handler);
  alarm(timeout);
  }

for(;;)
  {
  int status;
  pid_t rc = waitpid(pid, &status, 0);
  if (rc == pid)
    {
    int lowbyte = status & 255;
    if (lowbyte == 0) yield = (status >> 8) & 255;
      else yield = -lowbyte;
    break;
    }
  if (rc < 0)
    {
    yield = (errno == EINTR && sigalrm_seen)? -256 : -257;
    break;
    }
  }

if (timeout > 0)
  {
  alarm(0);
  signal(SIGALRM, SIG_IGN);
  }

signal(SIGCHLD, oldsignal);   /* restore */
return yield;
}

/* End of child.c */
