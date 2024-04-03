/*
** ypserv.c - YP server main routines.
**
** Copyright (c) 1996, 1997, 1998, 1999, 2000 Thorsten Kukuk
** Copyright (c) 1993 Signum Support AB, Sweden
**
** This file is part of the NYS YP Server.
**
** The NYS YP Server is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** The NYS YP Server is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public
** License along with the NYS YP Server; see the file COPYING.  If
** not, write to the Free Software Foundation, Inc., 675 Mass Ave,
** Cambridge, MA 02139, USA.
**
** Author: Peter Eriksson <pen@signum.se>
** Modified: Thorsten Kukuk <kukuk@suse.de>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "version.h"

#include "yp.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <syslog.h>
#ifndef LOG_DAEMON
#include <sys/syslog.h>
#endif
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#if defined(HAVE_GETOPT_H) && defined(HAVE_GETOPT_LONG)
#include <getopt.h>
#else
#include <compat/getopt.h>
#endif

#ifndef HAVE_GETOPT_LONG
#include <compat/getopt.c>
#include <compat/getopt1.c>
#endif

#ifndef HAVE_STRERROR
#include <compat/strerror.c>
#endif

#include "yp_msg.h"
#include "ypserv.h"

#ifndef SA_RESTART
#define SA_RESTART 0
#endif

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
#ifndef _PATH_VARRUN
#define _PATH_VARRUN "/etc/"
#endif
#define _YPSERV_PIDFILE _PATH_VARRUN"ypserv.pid"

int version_flag = 0;
int debug_flag = 0;
int dns_flag = 0;

#ifndef YPMAPDIR
#define YPMAPDIR   "/var/yp"
#endif
char *path_ypdb = YPMAPDIR;

char *progname;

#if HAVE__RPC_DTABLESIZE
extern int _rpc_dtablesize(void);
#elif HAVE_GETDTABLESIZE
int _rpc_dtablesize()
{
        static int size;

        if (size == 0) {
                size = getdtablesize();
        }
        return (size);
}
#else

#include <sys/resource.h>

int _rpc_dtablesize()
{
    static int size = 0;
    struct rlimit rlb;

    if (size == 0)
    {
        if (getrlimit(RLIMIT_NOFILE, &rlb) >= 0)
            size = rlb.rlim_cur;
    }

    return size;
}
#endif

#ifdef __STDC__
#define SIG_PF void(*)(int)
#endif

static void
ypprog_1 (struct svc_req *rqstp, register SVCXPRT *transp)
{
  union {
    domainname ypoldproc_domain_1_arg;
    domainname ypoldproc_domain_nonack_1_arg;
    yprequest ypoldproc_match_1_arg;
    yprequest ypoldproc_first_1_arg;
    yprequest ypoldproc_next_1_arg;
    yprequest ypoldproc_poll_1_arg;
    yprequest ypoldproc_push_1_arg;
    yprequest ypoldproc_pull_1_arg;
    yprequest ypoldproc_get_1_arg;
  } argument;
  char *result;
  xdrproc_t xdr_argument, xdr_result;
  char *(*local)(char *, struct svc_req *);

  switch (rqstp->rq_proc) {
  case YPOLDPROC_NULL:
    xdr_argument = (xdrproc_t) xdr_void;
    xdr_result = (xdrproc_t) xdr_void;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_null_1_svc;
    break;

  case YPOLDPROC_DOMAIN:
    xdr_argument = (xdrproc_t) xdr_domainname;
    xdr_result = (xdrproc_t) xdr_bool;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_domain_1_svc;
    break;

  case YPOLDPROC_DOMAIN_NONACK:
    xdr_argument = (xdrproc_t) xdr_domainname;
    xdr_result = (xdrproc_t) xdr_bool;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_domain_nonack_1_svc;
    break;

  case YPOLDPROC_MATCH:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_match_1_svc;
    break;

  case YPOLDPROC_FIRST:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_first_1_svc;
    break;

  case YPOLDPROC_NEXT:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_next_1_svc;
    break;

  case YPOLDPROC_POLL:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_poll_1_svc;
    break;

  case YPOLDPROC_PUSH:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_push_1_svc;
    break;

  case YPOLDPROC_PULL:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_pull_1_svc;
    break;

  case YPOLDPROC_GET:
    xdr_argument = (xdrproc_t) xdr_yprequest;
    xdr_result = (xdrproc_t) xdr_ypresponse;
    local = (char *(*)(char *, struct svc_req *)) ypoldproc_get_1_svc;
    break;

  default:
    svcerr_noproc (transp);
    return;
  }
  memset ((char *)&argument, 0, sizeof (argument));
  if (!svc_getargs (transp, xdr_argument, (caddr_t) &argument)) {
    svcerr_decode (transp);
    return;
  }
  result = (*local)((char *)&argument, rqstp);
  if (result != NULL && !svc_sendreply(transp, xdr_result, result)) {
    svcerr_systemerr (transp);
  }
  if (!svc_freeargs (transp, xdr_argument, (caddr_t) &argument)) {
    fprintf (stderr, "unable to free arguments");
    exit (1);
  }
  return;
}

static void
ypprog_2(struct svc_req *rqstp, register SVCXPRT *transp)
{
  union {
    domainname ypproc_domain_2_arg;
    domainname ypproc_domain_nonack_2_arg;
    ypreq_key ypproc_match_2_arg;
    ypreq_key ypproc_first_2_arg;
    ypreq_key ypproc_next_2_arg;
    ypreq_xfr ypproc_xfr_2_arg;
    ypreq_nokey ypproc_all_2_arg;
    ypreq_nokey ypproc_master_2_arg;
    ypreq_nokey ypproc_order_2_arg;
    domainname ypproc_maplist_2_arg;
  } argument;
  char *result;
  xdrproc_t xdr_argument, xdr_result;
  char *(*local)(char *, struct svc_req *);

  switch (rqstp->rq_proc)
    {
    case YPPROC_NULL:
      xdr_argument = (xdrproc_t) xdr_void;
      xdr_result = (xdrproc_t) xdr_void;
      local = (char *(*)(char *, struct svc_req *)) ypproc_null_2_svc;
      break;

    case YPPROC_DOMAIN:
      xdr_argument = (xdrproc_t) xdr_domainname;
      xdr_result = (xdrproc_t) xdr_bool;
      local = (char *(*)(char *, struct svc_req *)) ypproc_domain_2_svc;
      break;

    case YPPROC_DOMAIN_NONACK:
      xdr_argument = (xdrproc_t) xdr_domainname;
      xdr_result = (xdrproc_t) xdr_bool;
      local = (char *(*)(char *, struct svc_req *)) ypproc_domain_nonack_2_svc;
      break;

    case YPPROC_MATCH:
      xdr_argument = (xdrproc_t) xdr_ypreq_key;
      xdr_result = (xdrproc_t) xdr_ypresp_val;
      local = (char *(*)(char *, struct svc_req *)) ypproc_match_2_svc;
      break;

    case YPPROC_FIRST:
#if 0 /* Bug in Sun's yp.x RPC prototype file */
      xdr_argument = (xdrproc_t) xdr_ypreq_key;
#else
      xdr_argument = (xdrproc_t) xdr_ypreq_nokey;
#endif
      xdr_result = (xdrproc_t) xdr_ypresp_key_val;
      local = (char *(*)(char *, struct svc_req *)) ypproc_first_2_svc;
      break;

    case YPPROC_NEXT:
      xdr_argument = (xdrproc_t) xdr_ypreq_key;
      xdr_result = (xdrproc_t) xdr_ypresp_key_val;
      local = (char *(*)(char *, struct svc_req *)) ypproc_next_2_svc;
      break;

    case YPPROC_XFR:
      xdr_argument = (xdrproc_t) xdr_ypreq_xfr;
      xdr_result = (xdrproc_t) xdr_ypresp_xfr;
      local = (char *(*)(char *, struct svc_req *)) ypproc_xfr_2_svc;
      break;

    case YPPROC_CLEAR:
      xdr_argument = (xdrproc_t) xdr_void;
      xdr_result = (xdrproc_t) xdr_void;
      local = (char *(*)(char *, struct svc_req *)) ypproc_clear_2_svc;
      break;

    case YPPROC_ALL:
      xdr_argument = (xdrproc_t) xdr_ypreq_nokey;
      xdr_result = (xdrproc_t) xdr_ypresp_all;
      local = (char *(*)(char *, struct svc_req *)) ypproc_all_2_svc;
      break;

    case YPPROC_MASTER:
      xdr_argument = (xdrproc_t) xdr_ypreq_nokey;
      xdr_result = (xdrproc_t) xdr_ypresp_master;
      local = (char *(*)(char *, struct svc_req *)) ypproc_master_2_svc;
      break;

    case YPPROC_ORDER:
      xdr_argument = (xdrproc_t) xdr_ypreq_nokey;
      xdr_result = (xdrproc_t) xdr_ypresp_order;
      local = (char *(*)(char *, struct svc_req *)) ypproc_order_2_svc;
      break;

    case YPPROC_MAPLIST:
      xdr_argument = (xdrproc_t) xdr_domainname;
      xdr_result = (xdrproc_t) xdr_ypresp_maplist;
      local = (char *(*)(char *, struct svc_req *)) ypproc_maplist_2_svc;
      break;

    default:
      if (debug_flag)
	yp_msg("Unknown YPPROC call!\n");
      svcerr_noproc(transp);
      return;
    }

  memset((char *)&argument, 0, sizeof (argument));
  if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument))
    {
      svcerr_decode(transp);
      return;
    }
  result = (*local)((char *)&argument, rqstp);
  if (result != NULL && !svc_sendreply(transp, xdr_result, result))
    {
      svcerr_systemerr(transp);
    }
  if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument))
    {
      yp_msg("unable to free arguments");
      exit(1);
    }
  return;
}

extern int forked;

void
ypserv_svc_run (void)
{
#ifdef FD_SETSIZE
  fd_set readfds;
#else
  int readfds;
#endif /* def FD_SETSIZE */

  for (;;)
    {
#ifdef FD_SETSIZE
      readfds = svc_fdset;
#else
      readfds = svc_fds;
#endif /* def FD_SETSIZE */
      switch (select(_rpc_dtablesize(), &readfds, NULL, NULL,
		     (struct timeval *)0))
	{
	case -1:
	  if (errno == EINTR)
	    continue;
	  syslog (LOG_ERR, "svc_run: - select failed: %m");
	  return;
	case 0:
	  continue;
	default:
	  svc_getreqset (&readfds);
	  if (forked)
	    exit (0);
	}
    }
}

/* Create a pidfile on startup */
static void
create_pidfile(void)
{
  int fd, left, written, flags;
  pid_t pid;
  char pbuf[10], *ptr;
  struct flock lock;

  fd = open(_YPSERV_PIDFILE, O_CREAT | O_RDWR,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0)
    {
      yp_msg("cannot create pidfile %s", _YPSERV_PIDFILE);
      if(debug_flag) yp_msg("\n");
    }

  /* Make sure file gets correctly closed when process finished.  */
  flags = fcntl (fd, F_GETFD, 0);
  if (flags == -1)
    {
      /* Cannot get file flags.  */
      close (fd);
      return;
    }
  flags |= FD_CLOEXEC;          /* Close on exit.  */
  if (fcntl (fd, F_SETFD, flags) < 0)
    {
      /* Cannot set new flags.  */
      close (fd);
      return;
    }

  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  /* Is the pidfile locked by another ypserv ? */
  if (fcntl(fd, F_GETLK, &lock) < 0)
    {
      yp_msg("fcntl error");
      if(debug_flag) yp_msg("\n");
    }
  if (lock.l_type == F_UNLCK)
    pid = 0;          /* false, region is not locked by another proc */
  else
    pid = lock.l_pid; /* true, return pid of lock owner */

  if (0 != pid)
    {
      yp_msg("ypserv already running (pid %d) - exiting", pid);
      if(debug_flag) yp_msg("\n");
      exit(1);
    }

  /* write lock */
  lock.l_type = F_WRLCK;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;
  if (0 != fcntl(fd, F_SETLK, &lock))
    yp_msg("cannot lock pidfile");
  sprintf(pbuf, "%ld\n", (long)getpid());
  left = strlen(pbuf);
  ptr = pbuf;
  while (left > 0)
    {
      if ((written = write(fd, ptr, left)) <= 0)
	return;               /* error */
      left -= written;
      ptr   += written;
    }

  return;
}

/* Clean up after child processes signal their termination. */
static void
sig_child (int sig)
{
  int st;
  pid_t pid;

  if (debug_flag)
    yp_msg ("sig_child: got signal %i\n", sig);

  while ((pid = waitpid (-1, &st, WNOHANG)) > 0)
    {
      if (debug_flag)
	yp_msg ("pid=%d\n", pid);
      --children;
    }

  if (children < 0)
    {
      yp_msg ("children is lower 0 (%i)!\n", children);
    }
  else if (debug_flag)
    yp_msg ("children = %i\n", children);
}

/* Clean up if we quit the program. */
static void
sig_quit (int sig __attribute__ ((unused)))
{
  pmap_unset (YPPROG, YPVERS);
  pmap_unset (YPPROG, YPOLDVERS);
  unlink (_YPSERV_PIDFILE);

  exit (0);
}

/* Reload securenets and config file */
static void
sig_hup (int sig __attribute__ ((unused)))
{
#ifndef HAVE_LIBWRAP
  load_securenets ();
#endif
  load_config ();
}

static void
Usage (int exitcode)
{
  fputs ("usage: ypserv [-d [path]] [-b] [-p port]\n", stderr);
  fputs ("       ypserv --version\n", stderr);

  exit (exitcode);
}

int
main (int argc, char **argv)
{
  SVCXPRT *main_transp;
  int my_port = -1, my_socket, result;
  struct sockaddr_in socket_address;
  struct sigaction sa;

  progname = strrchr (argv[0], '/');
  if (progname == (char *) NULL)
    progname = argv[0];
  else
    progname++;

  openlog(progname, LOG_PID, LOG_DAEMON);

  while(1)
    {
      int c;
      int option_index = 0;
      static struct option long_options[] =
      {
        {"version", no_argument, NULL, 'v'},
        {"debug", no_argument, NULL, 'd'},
        {"port", required_argument, NULL, 'p'},
        {"dns", no_argument, NULL, 'b'},
        {"usage", no_argument, NULL, 'u'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, '\0'}
      };

      c=getopt_long(argc, argv, "vdp:buh",long_options, &option_index);
      if (c==EOF) break;
      switch (c)
        {
        case 'v':
	  debug_flag=1;
	  yp_msg("ypserv - NYS YP Server version %s (with %s)\n", version,
#ifdef HAVE_LIBWRAP
		 "tcp wrapper"
#else
		 "securenets"
#endif
		 );
	  exit(0);
        case 'd':
          debug_flag++;
          break;
        case 'p':
          my_port = atoi(optarg);
          if (debug_flag)
            yp_msg("Using port %d\n", my_port);
          break;
	case 'b':
	  dns_flag = 1;
	  break;
        case 'u':
        case 'h':
          Usage(0);
          break;
        default:
          Usage(1);
          break;
        }
    }

  argc-=optind;
  argv+=optind;

  if (debug_flag)
    yp_msg("[Welcome to the NYS YP Server, version %s (with %s)]\n",
	   version,
#ifdef HAVE_LIBWRAP
	   "tcp wrapper"
#else
	   "securenets"
#endif
	   );
  else
    {
      int i;

      if ((i = fork()) > 0)
	exit(0);

      if (i < 0)
	{
	  yp_msg ("Cannot fork: %s\n", strerror (errno));
	  exit (-1);
	}

      if (setsid() == -1)
	{
	  yp_msg ("Cannot setsid: %s\n", strerror (errno));
	  exit (-1);
	}

      if ((i = fork()) > 0)
	exit(0);

      if (i < 0)
	{
	  yp_msg ("Cannot fork: %s\n", strerror (errno));
	  exit (-1);
	}

      for (i = 0; i < getdtablesize(); ++i)
	close(i);
      errno = 0;

      umask(0);
      i = open("/dev/null", O_RDWR);
      dup(i);
      dup(i);
    }

  if (argc > 0)
    {
      path_ypdb = argv[0];
      if (debug_flag)
	yp_msg("Using database directory: %s\n", path_ypdb);
    }

  /* Change current directory to database location */
  if (chdir(path_ypdb) < 0)
    {
      yp_msg("ypserv: chdir: %", strerror(errno));
      exit(1);
    }

  create_pidfile();

#ifndef HAVE_LIBWRAP
  load_securenets();
#endif
  load_config();

  /*
   * Ignore SIGPIPEs. They can hurt us if someone does a ypcat
   * and then hits CTRL-C before it terminates.
   */
  sigaction(SIGPIPE, NULL, &sa);
  sa.sa_handler = SIG_IGN;
#if !defined(sun) || (defined(sun) && defined(__svr4__))
  sa.sa_flags |= SA_RESTART;
  /*
   * The opposite to SA_ONESHOT, do  not  restore
   * the  signal  action.  This provides behavior
   * compatible with BSD signal semantics.
   */
#endif
  sigemptyset(&sa.sa_mask);
  sigaction(SIGPIPE, &sa, NULL);
  /*
   * Clear up if child exists
   */
  sigaction(SIGCHLD, NULL, &sa);
#if !defined(sun) || (defined(sun) && defined(__svr4__))
  sa.sa_flags |= SA_RESTART;
#endif
  sa.sa_handler = sig_child;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGCHLD, &sa, NULL);
  /*
   * If program quits, give ports free.
   */
  sigaction(SIGTERM, NULL, &sa);
#if !defined(sun) || (defined(sun) && defined(__svr4__))
  sa.sa_flags |= SA_RESTART;
#endif
  sa.sa_handler = sig_quit;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGTERM, &sa, NULL);

  sigaction(SIGINT, NULL, &sa);
#if !defined(sun) || (defined(sun) && defined(__svr4__))
  sa.sa_flags |= SA_RESTART;
#endif
  sa.sa_handler = sig_quit;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  /*
   * If we get a SIGHUP, reload the securenets and config file.
   */
  sigaction(SIGHUP, NULL, &sa);
#if !defined(sun) || (defined(sun) && defined(__svr4__))
  sa.sa_flags |= SA_RESTART;
#endif
  sa.sa_handler = sig_hup;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGHUP, &sa, NULL);

  pmap_unset(YPPROG, YPVERS);
  pmap_unset(YPPROG, YPOLDVERS);

  if (my_port >= 0)
    {
      my_socket = socket (AF_INET, SOCK_DGRAM, 0);
      if (my_socket < 0)
	{
	  yp_msg("can not create UDP: %s",strerror(errno));
	  exit (1);
	}

      memset((char *) &socket_address, 0, sizeof(socket_address));
      socket_address.sin_family = AF_INET;
      socket_address.sin_addr.s_addr = htonl (INADDR_ANY);
      socket_address.sin_port = htons (my_port);

      result = bind (my_socket, (struct sockaddr *) &socket_address,
		     sizeof (socket_address));
      if (result < 0)
	{
	  yp_msg("%s: can not bind UDP: %s ",
		 progname,strerror(errno));
	  exit (1);
	}
    }
  else
    my_socket = RPC_ANYSOCK;

  main_transp = svcudp_create (my_socket);
  if (main_transp == NULL)
    {
      yp_msg ("cannot create udp service.");
      exit (1);
    }

  if (!svc_register (main_transp, YPPROG, YPVERS, ypprog_2, IPPROTO_UDP))
    {
      yp_msg ("unable to register (YPPROG, YPVERS, udp).");
      exit (1);
    }

  if (!svc_register(main_transp, YPPROG, YPOLDVERS, ypprog_1, IPPROTO_UDP))
    {
      yp_msg("unable to register (YPPROG, YPOLDVERS, udp).");
      exit(1);
    }

  if (my_port >= 0)
    {
      my_socket = socket (AF_INET, SOCK_STREAM, 0);
      if (my_socket < 0)
	{
	  yp_msg ("%s: can not create TCP: %s", progname, strerror (errno));
	  exit (1);
	}

      memset (&socket_address, 0, sizeof (socket_address));
      socket_address.sin_family = AF_INET;
      socket_address.sin_addr.s_addr = htonl (INADDR_ANY);
      socket_address.sin_port = htons (my_port);

      result = bind (my_socket, (struct sockaddr *) &socket_address,
		     sizeof (socket_address));
      if (result < 0)
	{
	  yp_msg("%s: can not bind TCP ", progname, strerror (errno));
	  exit (1);
	}
    }
  else
    my_socket = RPC_ANYSOCK;

  main_transp = svctcp_create(my_socket, 0, 0);
  if (main_transp == NULL)
    {
      yp_msg("%s: cannot create tcp service\n", progname);
      exit(1);
    }

  if (!svc_register(main_transp, YPPROG, YPVERS, ypprog_2, IPPROTO_TCP))
    {
      yp_msg("%s: unable to register (YPPROG, YPVERS, tcp)\n", progname);
      exit(1);
    }

  if (!svc_register(main_transp, YPPROG, YPOLDVERS, ypprog_1, IPPROTO_TCP))
    {
      yp_msg("%s: unable to register (YPPROG, YPOLDVERS, tcp)\n", progname);
      exit(1);
    }

  ypserv_svc_run();
  yp_msg("svc_run returned");
  unlink(_YPSERV_PIDFILE);
  exit(1);
  /* NOTREACHED */
}
