/* Copyright (c) 2003 SuSE Linux AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ****************************************************************
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "makeboot.h"
#include "proc.h"

#define timerdiff(n,l) (__extension__ ({ (((n).tv_sec-(l).tv_sec)*1000)+(((n).tv_usec-(l).tv_usec)/1000); }))

typedef enum _boolean {false, true} boolean;
extern char *optarg;
extern int optind;

static long int numcpu = -1;
static char *myname;
static struct termios tio;
static struct winsize wz;
static struct {
  char env_row[128];
  char env_col[128];
} sz;
static sig_atomic_t wzok;
static char *arg;
static boolean isstart;
static struct sigaction sa;
static struct timeval glastio;
static struct timeval now;
static struct timeval lastlim;
static char *run_mode = NULL;
static struct makenode **nodevec;

static enum { Unknown, Preload, NoPreload } ispreload = Unknown;

#define PBUF_SIZE	8192
struct prg {
  const char *name;
  const char *arg0;
  int num;
  int fd;
  pid_t pid;
  struct timeval lastio;
  size_t len;
  int splashadd;
  char buf[PBUF_SIZE];
};

static struct prg *prgs;
static int inpar, par;
static int pidpipe[2];
static double iorate = 800.0;

void sighandler_nopreload(int x)
{
    (void)x;
    ispreload = NoPreload;
}


void sighandler_preload(int x)
{
    (void)x;
    ispreload = Preload;
}

void *xcalloc(size_t nmemb, size_t size)
{
  void *r;
  if ((r = (void *)calloc(nmemb, size)) == 0)
    {
      fprintf(stderr, "calloc: out of memory\n");
      exit(1);
    }
  return r;
}

static int splashpos = -1;
static char *splashcfg;

int calcsplash(int x, int n, char *opt)
{
  char *p;
  int i;
  int r, d;

  if (opt == 0)
    return -1;
  for (i = 0; i <= x; i++)
    {
      if ((p = strchr(opt, ':')) == 0)
        break;
      if (i == x)
	return atoi(opt);
      opt = p + 1;
    }
  r = atoi(opt);
  n -= i;
  for (;; i++, n--)
    {
      if (n < 1)
	n = 1;
      d = r / n;
      if (i == x)
	return d;
      r -= d;
    }
}

pid_t splashpid;

void waitsplash()
{
  int status;
  if (!splashpid)
    return;
  do {
    waitpid(splashpid, &status, 0);
  } while (errno == EINTR);
  splashpid = 0;
}

void closeall(void)
{
  int s;

  if (!prgs)
    return;
  for (s = 0; s < par; s++)
    if (prgs[s].fd)
      close(prgs[s].fd);
  close(pidpipe[0]);
  close(pidpipe[1]);
}

void callsplash(int n, const char *path, char *action)
{
  const char *p;
  char sbuf[32];
  char tbuf[256];
  pid_t pid;
  struct stat stb;

  if (n < 0 || splashpos < 0)
    return;
  if (splashpos + n > 65535)
    n = 65535 - splashpos;
  splashpos += n;
  if (stat("/proc/splash", &stb))
     return;
  p = strrchr(path, '/');
  if (p)
    path = p + 1;
  for (p = path; *p; p++)
    if ((*p == 'S' || *p == 'K') && p[1] >= '0' && p[1] <= '9' && p[2] >= '0' && p[2] <= '9' && p[3])
      break;
  if (*p)
    p += 3;
  else
    p = path;
  if (!action)
    action = "";
  if (strlen(p) + strlen(action) + 2 > sizeof(tbuf))
    return;
  sprintf(tbuf, "%s%s%s", p, *action ? " " : "", action);
  sprintf(sbuf, "%d:%d", splashpos - n, n);
  waitsplash();
  pid = fork();
  if (pid == (pid_t)-1)
    return;
  if (pid)
    {
      splashpid = pid;
      return;
    }
  while (dup2(2, 1) < 0 && (errno == EINTR))
    ;
  closeall();
  execl("/sbin/splash", "splash", "-p", sbuf, "-t", tbuf, splashcfg, (char *)0);
  _exit(1);
}

void writebuf(struct prg *p)
{
  char *b = p->buf;
  int r;

  while (p->len > 0)
    {
      r = write(2, b, p->len);
      if (r < 0)
	{
	  perror("write");
	  r = p->len;
	}
      p->len -= r;
      b += r;
    }
  glastio = now;
}

static int checksystem(const int par, const boolean start, const boolean limit)
{
  const      int pg_size       = sysconf(_SC_PAGESIZE);
  const long int minphys_bytes = (sysconf(_SC_LONG_BIT) > 32L) ? (2<<22) : (2<<21);
  const long int avphys_pg     = sysconf(_SC_AVPHYS_PAGES);
  long int minphys_pg;
  unsigned long int prcs_run, prcs_blked;
  int newpar;
  
  if (avphys_pg < 0)
    return 1;

  if (pg_size < 0)
    return par;

  if (!start)
    minphys_pg = avphys_pg;
  else
    minphys_pg = minphys_bytes / pg_size;

  if (avphys_pg < minphys_pg)
    return 1;

  if (numcpu < 1)
    return par;
  
  if (!limit)
    return (par*numcpu);

  if (read_proc(&prcs_run, &prcs_blked))
    return par;

  /* if we have preload running, we expect I/O not to be a problem */
  if (ispreload != NoPreload)
    prcs_blked = 0;

  newpar  = (par*numcpu) - prcs_run + 1; 	/* +1 for startpar its self */
  newpar -= (int)(((double)prcs_blked)*iorate);	/* I/O load reduction */

#if DEBUG
  fprintf(stderr, "checksystem par=%d newpar=%d (prcs_run=%lu) %ld\n", par, newpar, prcs_run, time(0));
  dump_status();
#endif
  if (newpar <= 0)
    return 1;
  else
    return newpar;
}

static inline int checklimit(const int par, const boolean start)
{
  return checksystem(par, start, true);
}

static inline int checkpar(const int par, const boolean start)
{
  return checksystem(par, start, false);
}

#define SOCK_PATH "/dev/shm/preload_sock"

void run(struct prg *p)
{
  char *m = 0;
  pid_t parent = getpid();

  p->len = 0;
  p->pid = (pid_t)0;
  p->fd = getpt();
  if (p->fd <= 0)
    {
      p->fd = 0;
      perror("getpt");
      fprintf(stderr, "could not get pty for %s\n", p->name);
    }
  else if ((m = ptsname(p->fd)) == 0 || grantpt(p->fd) || unlockpt(p->fd))
    {
      fprintf(stderr, "could not init pty for %s\n", p->name);
      close(p->fd);
      p->fd = 0;
    }
  if ((p->pid = fork()) == (pid_t)-1)
    {
      perror("fork");
      fprintf(stderr, "could not fork %s\n", p->name);
      p->pid = 0;
      if (p->fd)
	{
	  close(p->fd);
	  p->fd = 0;
	}
      return;
    }
  if (p->pid != 0)
    return;

  (void)signal(SIGINT,  SIG_DFL);
  (void)signal(SIGQUIT, SIG_DFL);
  (void)signal(SIGSEGV, SIG_DFL);
  (void)signal(SIGTERM, SIG_DFL);
  (void)signal(SIGCHLD, SIG_DFL);

  if (setpgid(0, 0))
    perror("setpgid");

  if (m && p->fd)
    {
      while (close(1) < 0 && (errno == EINTR))
	;
      if (open(m, O_RDWR) != 1)
	{
	  perror(m);
	  _exit(1);
	}
      while (dup2(1, 2) < 0 && (errno == EINTR))
	;
      tio.c_oflag &= ~OPOST;
      if (tcsetattr(1, TCSANOW, &tio))
	perror("tcsetattr");
      if (wzok)
	ioctl(1, TIOCSWINSZ, &wz);
      putenv(sz.env_row);
      putenv(sz.env_col);
    }
  else
    {
      while (dup2(2, 1) < 0 && (errno == EINTR))
	;
    }

  closeall();

  if (!strcmp(arg, "start")) 
    { 
      int s, t, len;
      pid_t child;
      struct sockaddr_un remote;
      char str[100];

      s = socket(AF_UNIX, SOCK_STREAM, 0);
      if (s != -1) 
	{
	  memset(&remote, 0, sizeof(struct sockaddr_un));
	  remote.sun_family = AF_UNIX;
	  strcpy(remote.sun_path, SOCK_PATH);
	  len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	  t = connect(s, (struct sockaddr *)&remote, len);
	  if (t != -1) 
	    {
	      if (ispreload != Preload)
		kill(parent, SIGUSR1);
	      send(s, p->name, strlen(p->name), 0);
	      recv(s, str, 100, 0);
	    } 
	  else if ( ispreload == Unknown) 
	    {
	      /*
	       * if we connected to preload once, we know it ran.
	       * In case we can't connect to it later, it means it did
	       * its job and we can guess I/O is no longer a problem. 
	       */
	      kill(parent, SIGUSR2);
	    }
	  close(s);
	  /*
	   * if we use preload, we fork again to make bootcharts easier to read.
	   * The reason is that the name of the init script will otherwise be used
	   * when in reality the above code waited for preload. If we fork away
	   * before the exec, the waiting code will be folded into startpar
	   */
	  child = fork();
	  if (child) {
		int status;
		int ret = waitpid(child, &status, 0);
		if (ret == -1)
			perror("waitpid");
		exit(WEXITSTATUS(status));
	  }
	}
    }

  if (run_mode)
    {
      char path[128];
      snprintf(path, sizeof(path), "/etc/init.d/%s", p->name);
      execlp(path, p->arg0, arg, (char *)0);
    }
  else if (arg)
    execlp(p->name, p->arg0, arg, (char *)0);
  else
    execlp(p->name, p->arg0, (char *)0);
  perror(p->name);
  _exit(1);
}

int run_single(const char *prg, const char *arg0, int spl)
{
  pid_t pid;
  int r;

  if ((pid = fork()) == (pid_t)-1)
    {
      perror("fork");
      fprintf(stderr, "could not fork %s\n", prg);
      return 1;
    }

  if (pid == 0)
    {
      (void)signal(SIGINT,  SIG_DFL);
      (void)signal(SIGQUIT, SIG_DFL);
      (void)signal(SIGSEGV, SIG_DFL);
      (void)signal(SIGTERM, SIG_DFL);
      (void)signal(SIGCHLD, SIG_DFL);

      while (dup2(2, 1) < 0 && (errno == EINTR))
	;
      closeall();
      if (run_mode)
	{
	  char path[128];
	  snprintf(path, sizeof(path), "/etc/init.d/%s", prg);
	  execlp(path, arg0 ? arg0 : path, arg, (char *)0);
	}
      else if (arg)
	execlp(prg, arg0 ? arg0 : prg, arg, (char *)0);
      else
	execlp(prg, arg0 ? arg0 : prg, (char *)0);
      perror(prg);
      _exit(1);
    }

   while ((waitpid(pid, &r, 0) == (pid_t)-1) && (errno == EINTR))
     ;
   callsplash(spl, prg, arg);
   return WIFEXITED(r) ? WEXITSTATUS(r) : (WIFSIGNALED(r) ? 1 : 255);
}

void do_forward(void)
{
  char buf[4096], *b;
  int r, rr;
  setsid();
  while ((r = read(0, buf, sizeof(buf))) > 0)
    {
      b = buf;
      while (r > 0)
	{
	  rr = write(1, b, r);
	  if (rr == -1)
	    {
	      perror("write");
	      rr = r;
	    }
	  r -= rr;
	}
    }
  _exit(0);
}

static char *gtimo_buf;
static size_t gtimo_bufsize;
static size_t gtimo_buflen;

void storebuf(struct prg *p)
{
  if ((gtimo_buflen + p->len) > gtimo_bufsize)
    {
      writebuf(p);				/* In case of overflow or memory shortage */
      return;
    }

  (void)memcpy(gtimo_buf + gtimo_buflen, p->buf, p->len);
  gtimo_buflen += p->len;
  p->len = 0;
  glastio = now;
}

void flushbuf(void)
{
  size_t len = gtimo_buflen;
  char * buf = gtimo_buf;

  if (!buf)
	return;					/* In case of memory shortage */

  while (len > 0)
    {
      int r = write(2, buf, len);
      if (r < 0)
	{
	  perror("write");
	  r = len;
	}
      len -= r;
      buf += r;
    }
  gtimo_buflen = 0;
  *gtimo_buf = 0;
}

#define GTIMO_OFFL	0
#define GTIMO_USED	1

void detach(struct prg *p, const int store)
{
  int r;
  int flags = fcntl(p->fd, F_GETFL);

  if (flags > 0)
    flags |= FNONBLOCK;
  else
    flags = FNONBLOCK;

  fcntl(p->fd, F_SETFL, flags);
  while ((r = read(p->fd, p->buf, sizeof(p->buf))) > 0)
    {
      p->len = r;
      if (store)
	storebuf(p);
      else
	writebuf(p);
    }
  flags &= ~FNONBLOCK;
  fcntl(p->fd, F_SETFL, flags);
  if (r == -1 && errno == EWOULDBLOCK)
    {
      if ((r = fork()) == 0)
	{
	  while (dup2(p->fd, 0) < 0 && (errno == EINTR))
	    ;
	  while (dup2(2, 1) < 0 && (errno == EINTR))
	    ;
	  closeall();
	  execlp(myname, myname, "-f", "--", p->name, NULL);
	  do_forward();
	}
      if (r == -1)
	perror("fork");
    }
  close(p->fd);
  p->fd = 0;
}

static void sigchld(int sig __attribute__ ((unused)))
{
  char c = 0;
  write(pidpipe[1], &c, 1);
}

static void sigwinch(int sig __attribute__ ((unused)))
{
  if (ioctl(0, TIOCGWINSZ, &wz) < 0)
    {
      wzok = 0;
      return;
    }
  if (wz.ws_row == 0) wz.ws_row = 24;
  if (wz.ws_col == 0) wz.ws_col = 80;
  snprintf(sz.env_row, sizeof(sz.env_row), "LINES=%d",   wz.ws_row);
  snprintf(sz.env_col, sizeof(sz.env_col), "COLUMNS=%d", wz.ws_col);
}

void usage(int status)
{
  fprintf(stderr, "usage: startpar [options] [-a arg] prgs\n");
  fprintf(stderr, "           run given programs parallel\n");
  fprintf(stderr, "       startpar [options] [-P prev] [-R run] [-S <start>:<num>] -M mode\n");
  fprintf(stderr, "           run parallel with Makefile\n");
  fprintf(stderr, "       startpar -v\n");
  fprintf(stderr, "           show version number\n");
  fprintf(stderr, "general options:\n");
  fprintf(stderr, "       -p parallel tasks\n");
  fprintf(stderr, "       -t I/O timeout\n");
  fprintf(stderr, "       -T global I/O timeout\n");
  exit(status);
}

int main(int argc, char **argv)
{
  int gtimo = -1;
  int timo = -1;
  int isgtimo;
  int forw = 0;
  int c, i, num;
  int limit;
  int *resvec;
  fd_set rset;
  struct timeval tv;
  struct prg *p;
  char pipebuf[16];
  struct prg *gtimo_running = 0;
  struct prg *interactive_task = NULL;
  char *prev_level = getenv("PREVLEVEL");
  char *run_level = getenv("RUNLEVEL");
  char *splashopt = 0;

  (void)signal(SIGUSR1, sighandler_preload);
  (void)signal(SIGUSR2, sighandler_nopreload);

  (void)signal(SIGCHLD, SIG_DFL);
  numcpu = sysconf(_SC_NPROCESSORS_ONLN);
  myname = argv[0];

  while ((c = getopt(argc, argv, "fhp:t:T:a:M:P:R:S:vi:")) != EOF)
    {
      switch(c)
        {
	case 'p':
	  par = atoi(optarg);
	  break;
	case 't':
	  timo = atoi(optarg);
	  break;
	case 'T':
	  gtimo = atoi(optarg);
	  break;
	case 'f':
	  forw = 1;
	  break;
	case 'a':
	  arg = optarg;
	  break;
	case 'M':
	  run_mode = optarg;
	  break;
	case 'P':
	  prev_level = optarg;
	  break;
	case 'R':
	  run_level = optarg;
	  break;
	case 'S':
	  splashopt = optarg;
	  break;
	case 'v':
	  printf("startpar version %s\n", VERSION);
	  exit(0);
	case 'h':
	  usage(0);
	  break;
	case 'i':
	  iorate = atof(optarg);
	  if (iorate < 0.0)
	    iorate = 800.0;
	  break;
	default:
	  usage(1);
	  break;
        }
    }
  if (forw)
    do_forward();
  argc -= optind;
  argv += optind;

  if (splashopt)
    {
      char *so = strchr(splashopt, ':');
      if (!so)
	splashopt = 0;
      else
	{
	  splashpos = atoi(splashopt);
	  splashopt = so + 1;
	}
      splashcfg = getenv("SPLASHCFG");
      if (!splashcfg)
	{
	  splashpos = -1;
	  splashopt = 0;
	}
    }
  if (run_mode)
    {
      char makefile[64];
      if (!strcmp(run_mode, "boot"))
	arg = "start";
      else if (!strcmp(run_mode, "halt"))
	arg = "stop";
      else if (!strcmp(run_mode, "start") || !strcmp(run_mode, "stop"))
	{
	  arg = run_mode;
	  if (!prev_level || !run_level)
	    {
	      fprintf(stderr, "You must specify previous and next runlevels\n");
	      exit(1);
	    }
	}
      else
	{
	  fprintf(stderr, "invalid run mode %s\n", run_mode);
	  exit(1);
	}
      snprintf(makefile, sizeof(makefile), "/etc/init.d/.depend.%s", run_mode);
      parse_makefile(makefile);
      check_run_files(run_mode, prev_level, run_level);

      argc = tree_entries;			/* number of handled scripts */
      isstart = !strcmp(arg, "start");

      if (argc == 0)
	exit(0);

      if (par == 0)
	par = 4;
      if (par > argc)				/* not more than the number of all scripts */
	par = argc;

      inpar = par;				/* the original argument of parallel procs per cpu */

      par = checkpar(inpar, isstart);		/* the number of parallel procs on all cpu's */

      if (par > argc)				/* not more than the number of all scripts */
	par = argc;

      nodevec = xcalloc(argc, sizeof(*nodevec));
    }
  else
    {
      if (par < 0)
	usage(1);

      if (arg)
	isstart = !strcmp(arg, "start");

      if (argc == 0)
	exit(0);

      if (par == 0)
	par = argc;
      if (par > argc)				/* not more than the number of all scripts */
	par = argc;

      inpar = par;				/* the original argument of parallel procs per cpu */

      par = checkpar(inpar, isstart);		/* the number of parallel procs on all cpu's */

      if (par > argc)				/* not more than the number of all scripts */
	par = argc;
    }

  num = 0;
  resvec = (int *)xcalloc(argc, sizeof(int));
  for (i = 0; i < argc; i++)
    resvec[i] = 255;

  if (argc == 1)
    {
      if (run_mode)
	{
	  if ((*nodevec = pickup_task()))
	  {
	    *resvec = run_single((*nodevec)->name, (*nodevec)->arg0, calcsplash(0, 1, splashopt));
	    finish_task(*nodevec);
	  }
      } else
	*resvec = run_single(*argv, *argv, calcsplash(0, 1, splashopt));
      goto finished;
    }

  prgs = (struct prg *)xcalloc(par, sizeof *prgs);
  gtimo_bufsize = par * PBUF_SIZE;
  gtimo_buf = (char *) calloc(gtimo_bufsize, sizeof(char));
  if (!gtimo_buf)
    gtimo_bufsize = 0;				/* Accept error due memory shortage */

  sa.sa_handler = sigwinch;
  sa.sa_flags = SA_RESTART|SA_NODEFER;
  (void)sigemptyset(&sa.sa_mask);
  if (sigaction(SIGWINCH, &sa, 0))
    {
      perror("sigwinch sigaction");
      exit(1);
    }

  if (tcgetattr(0, &tio))
    {
      perror("tcgetattr");
      exit(1);
    }
  if (ioctl(0, TIOCGWINSZ, &wz) == 0)
    wzok = 1;
  if (wz.ws_row == 0) wz.ws_row = 24;
  if (wz.ws_col == 0) wz.ws_col = 80;

  strcat(&sz.env_row[0], "LINES=");
  strcat(&sz.env_col[0], "COLUMNS=");
  snprintf(sz.env_row, sizeof(sz.env_row), "LINES=%d",   wz.ws_row);
  snprintf(sz.env_col, sizeof(sz.env_col), "COLUMNS=%d", wz.ws_col);

  if (pipe(pidpipe))
    {
      perror("pipe");
      exit(1);
    }
  fcntl(pidpipe[0], F_SETFL, FNONBLOCK);
  fcntl(pidpipe[1], F_SETFL, FNONBLOCK);
  sa.sa_handler = sigchld;
  sa.sa_flags = SA_RESTART;
  (void)sigemptyset(&sa.sa_mask);
  if (sigaction(SIGCHLD, &sa, 0))
    {
      perror("sigchld sigaction");
      exit(1);
    }

  gettimeofday(&glastio, 0);
  limit = checklimit(inpar, isstart);
  lastlim = glastio;
  for (;;)
    {
      int active = 0;
      int maxfd = -1;
      int last = -1;
      pid_t pid = 0;
      int r = 0, s;
      long diff;

      gettimeofday(&now, 0);
      FD_ZERO(&rset);
      tv = now;

      if ((diff = timerdiff(now, lastlim)) >= 300 || diff < 0)
	{
#if DEBUG
	  fprintf(stderr, "%d: doing checklimit after %ldms %ld\n", getpid(), diff, time(0));
#endif
	  if ((limit = checklimit(inpar, isstart)) > argc)
	    limit = argc;			/* not more than the number of all scripts */
	  lastlim = now;
	  diff = 0;
	} 
#if DEBUG
      fprintf(stderr, "par=%d, inpar=%d, limit=%d (diff=%ld)\n", par, inpar, limit, diff);
#endif
      for (s = 0; s < par; s++)			/* never leave this with break!! */
	{
	account:				/* for the new process below */
	  p = prgs + s;
	  if (p == interactive_task)
	    continue;				/* don't count this here */
	  if (p->fd || p->pid)
	    active++;				/* count all running procs */
	  if (p->fd == 0)
	    {
	      if (interactive_task)
		continue;			/* dont't start new processes */
	      if (num >= argc)
		continue;			/* nothing to do */
	      if (p->pid == 0)
		{
		  if (active >= limit)
		    continue;			/* load balancing */
		  if (run_mode)
		    {
		      if ((nodevec[num] = pickup_task()) == NULL)
			continue;
		      if (nodevec[num]->interactive)
			interactive_task = p;
		      p->name = nodevec[num]->name;
		      p->arg0 = nodevec[num]->arg0 ? nodevec[num]->arg0 : nodevec[num]->name;
		    }
		  else {
		    p->name = *argv++;
		    p->arg0 = p->name;
		  }
		  p->splashadd = calcsplash(num, argc, splashopt);
		  p->num = num++;
		  if (interactive_task)
		    continue;			/* don't start this here */
		  run(p);
		  if (p->pid == 0)
		    {
		      resvec[p->num] = 1;
		      if (run_mode)
			finish_task(nodevec[p->num]);
		    }
		  gettimeofday(&now, 0);
		  tv = now;
		  goto account;			/* take the new process into account */
		}
	      continue;
	    }
	  FD_SET(p->fd, &rset);
	  if (p->fd > maxfd)
	    maxfd = p->fd;
	  if (p->len == 0)
	    continue;
          if ((last < 0) || timercmp(&tv,&p->lastio,>))
	    {
	      last = s;
	      tv = p->lastio;
	    }
	} /* for (s = 0; s < par; s++) */

      if (interactive_task)
	{
	  if (active == 0)
	    {
	      p = interactive_task;
	      resvec[p->num] = run_single(p->name, p->arg0, p->splashadd);
	      if (run_mode)
		finish_task(nodevec[p->num]);
	      p->pid = 0;
	      p->fd = 0;
	      interactive_task = NULL;
	      continue;
	    }
	}

      if (active == 0)
	{
	  if (num < argc)
	    fprintf(stderr, "ERROR: not all processed (%d of %d)\n", num, argc);
#if DEBUG
	  if ((pid = waitpid(-1, &r, (maxfd < 0 ? 0 : WNOHANG)|WUNTRACED)) > 0)
	    fprintf(stderr, "ERROR: not all processes are checked\n");
#endif
	  break;
	}
#if DEBUG
      fprintf(stderr, "active = %d\n", active);
#endif
      if (active == 1 && last >= 0)
	{
	  p = prgs + last;
	  if ((pid = waitpid(p->pid, &r, (maxfd < 0 ? 0 : WNOHANG)|WUNTRACED)) == 0)
	    {
	      writebuf(p);
	      continue;
	    }
	}

      FD_SET(pidpipe[0], &rset);
      /* drain the pidpipe */
      while ((c = read(pidpipe[0], pipebuf, sizeof pipebuf)) > 0)
	;

      if (pid == 0)
	pid = waitpid(-1, &r, (maxfd < 0 ? 0 : WNOHANG)|WUNTRACED);

      if (pid > 0)
	{
	  if (pid == splashpid)
	    splashpid = (pid_t)0;
	  for (s = 0; s < par; s++)
	    {
	      p = prgs + s;
	      if (p->pid == pid)
		{
		  if (WIFSTOPPED(r))
		    {
		      if (WSTOPSIG(r) == SIGTTIN)
			{
			  pid_t pg = getpgid(pid);
			  if (pg > 0)
			    killpg(pg, SIGCONT);
			}
		      continue;
		    }
		  callsplash(p->splashadd, p->name, arg);
		  resvec[p->num] = WIFEXITED(r) ? WEXITSTATUS(r) : (WIFSIGNALED(r) ? 1 : 255);
		  if (run_mode)
		    finish_task(nodevec[p->num]);
		  p->pid = 0;
		  if (gtimo_running == p)
		    {
		      writebuf(p);
		      if (p->fd)
			detach(p, GTIMO_OFFL);
		      flushbuf();
		      gtimo_running = 0;
		    }
		  else if (gtimo_running)
		    {
		      storebuf(p);
		      if (p->fd)
			detach(p, GTIMO_USED);
		    }
		  else
		    {
		      writebuf(p);
		      if (p->fd)
			detach(p, GTIMO_OFFL);
		    }
		  break;
		}
	    } /* for (s = 0; s < par; s++) */
	  continue;
	}

      if (maxfd < 0)
	continue;				/* start new processes? */

      if (timo >= 0)
        tv.tv_sec += timo;

      isgtimo = 0;
      if (gtimo >= 0 && !gtimo_running && last >= 0 && prgs[last].pid)
	{
	  struct timeval gl = glastio;
	  gl.tv_sec += gtimo;
	  if ((timo < 0) || timercmp(&tv,&gl,>))
	    {
	      tv = glastio;
	      tv.tv_sec += gtimo;
	      isgtimo = 1;
	    }
	}

      r = 0;
      if (timo >= 0 || isgtimo)
	{
	  int setfd = (pidpipe[0] > maxfd) ? pidpipe[0] : maxfd;
	  struct timeval wait;

	  timersub(&tv, &now, &wait);
	  if (wait.tv_usec < 0)
	    {
	      wait.tv_usec += 1000000;
	      wait.tv_sec--;
	    }
	  if (wait.tv_sec >= 0)
	    {
	      int check = limit < par && num < argc;

	      if (check)			/* shorten timeout for new limit and procs  ... */
		{
		  wait.tv_sec = 0;
		  wait.tv_usec = (300 - diff) * 1000;
		}
#if DEBUG
	      fprintf(stderr, "going into select1 %d %ld %ld\n", last, wait.tv_sec, wait.tv_usec);
#endif
	      r = select(setfd + 1, &rset, 0, 0, (last >= 0 || check) ? &wait : 0);

	      if (check && (r == 0))		/* ... but do not throw out messages to early!!! */
		continue;
	    }
	  else
	    {
	      wait.tv_sec  = 0;			/* Avoid looping around (does this ever happen?) */
	      wait.tv_usec = 20*1000;
	      r = select(setfd + 1, &rset, 0, 0, last >= 0 ? &wait : 0);
	    }
	}
      else
	{
	  int setfd = (pidpipe[0] > maxfd) ? pidpipe[0] : maxfd;
	  r = select(setfd + 1, &rset, 0, 0, 0);
	}

      if (r == -1)
	{
	  if (errno == EINTR)
	    continue;
	  perror("select");
	  exit(1);
	}
      if (r == 0)
	{
	  if (last < 0)		/* just in case... */
	    continue;
	  p = prgs + last;
	  writebuf(p);
	  if (isgtimo && p->pid)
	    gtimo_running = p;
	}
      else
	{
	  for (s = 0; s < par; s++)
	    {
	      p = prgs + s;
	      if (p->fd == 0)
		continue;
	      if (!FD_ISSET(p->fd, &rset))
		continue;
	      r = read(p->fd, p->buf + p->len, sizeof(p->buf) - p->len);
	      if (r <= 0)
		{
		  if (!gtimo_running || p == gtimo_running)
		    writebuf(p);
		  close(p->fd);
		  p->fd = 0;
		  break;
		}
	      p->len += r;
	      if (p->len == sizeof(p->buf))
		{
		  for (i = p->len - 1; i >= 0; i--)
		    {
		      if (p->buf[i] == '\n')
			break;
		    }
		  if (++i <= 0)
		    i = p->len;
		  p->len = i;
		  writebuf(p);
		  p->len = i;	/* writebuf clears p->len */
		  if (p->len < sizeof(p->buf))
		    memmove(p->buf, p->buf + p->len, sizeof(p->buf) - p->len);
		  p->len = sizeof(p->buf) - p->len;
		}
	      p->lastio = now;
	    } /* for (s = 0; s < par; s++) */
	}
    } /* for (;;) */

 finished:
  waitsplash();
  if (run_mode)
    print_run_result(resvec, nodevec, run_mode);
  else
    {
      for (i = 0; i < argc; i++)
	{
#if DEBUG
	  if (resvec[i] == 255)
	    {
	      fprintf(stderr, "ERROR: forgotten process??\n");
	      exit (1);
	    }
#endif
#if VERBOSE
	  printf(i ? " %d" : "%d", resvec[i]);
#endif /* VERBOSE */
	}
#if VERBOSE
      printf("\n");
#endif /* VERBOSE */
    }
  return 0;
}
