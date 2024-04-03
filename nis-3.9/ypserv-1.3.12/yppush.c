/* Copyright (c) 1996, 1997, 1998, 1999 Thorsten Kukuk
   This file is part of the NYS YP Server.
   Author: Thorsten Kukuk <kukuk@suse.de>

   The NYS YP Server is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The NYS YP Server is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with the NYS YP Server; see the file COPYING.  If
   not, write to the Free Software Foundation, Inc., 675 Mass Ave,
   Cambridge, MA 02139, USA. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <rpc/rpc.h>
#include <time.h>
#include "yp.h"
#include <rpcsvc/ypclnt.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <ctype.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#if defined(HAVE_LIBGDBM)
#include <gdbm.h>
#elif defined(HAVE_NDBM)
#include <ndbm.h>
#include <fcntl.h>
#endif
#if defined(HAVE_GETOPT_H) && defined(HAVE_GETOPT_LONG)
#include <getopt.h>
#else
#include <compat/getopt.h>
#endif

#include "version.h"
#include "yp_msg.h"
#include "yppush.h"

#ifndef HAVE_STRDUP
#include <compat/strdup.c>
#endif

#ifndef HAVE_GETOPT_LONG
#include <compat/getopt.c>
#include <compat/getopt1.c>
#endif

#ifndef HAVE_STRERROR
#include <compat/strerror.c>
#endif

#ifndef YPMAPDIR
#define YPMAPDIR "/var/yp"
#endif

struct hostlist
{
  char *hostname;
  struct hostlist *next;
};

struct hostlist *hostliste = NULL;

static char *DomainName = NULL;
int verbose_flag = 0;
int debug_flag = 1; /* Needed for yp_msg */
static char local_hostname[MAXHOSTNAMELEN + 2];
static char *current_map;
static u_int CallbackProg = 0;
static u_int timeout = 90;
static u_int MapOrderNum;
static u_int maxchildren = 1;
static u_int children = 0;

#if HAVE__RPC_DTABLESIZE
extern int _rpc_dtablesize (void);
#elif HAVE_GETDTABLESIZE

int
_rpc_dtablesize ()
{
  static int size;

  if (size == 0)
    {
      size = getdtablesize ();
    }
  return (size);
}
#else

#include <sys/resource.h>

int
_rpc_dtablesize ()
{
  static int size = 0;
  struct rlimit rlb;


  if (size == 0)
    {
      if (getrlimit (RLIMIT_NOFILE, &rlb) >= 0)
	size = rlb.rlim_cur;
    }

  return size;
}
#endif

static char *
yppush_err_string (enum yppush_status status)
{
  switch (status)
    {
    case YPPUSH_SUCC:
      return "Success";
    case YPPUSH_AGE:
      return "Master's version not newer";
    case YPPUSH_NOMAP:
      return "Can't find server for map";
    case YPPUSH_NODOM:
      return "Domain not supported";
    case YPPUSH_RSRC:
      return "Local resource alloc failure";
    case YPPUSH_RPC:
      return "RPC failure talking to server";
    case YPPUSH_MADDR:
      return "Can't get master address";
    case YPPUSH_YPERR:
      return "YP server/map db error";
    case YPPUSH_BADARGS:
      return "Request arguments bad";
    case YPPUSH_DBM:
      return "Local dbm operation failed";
    case YPPUSH_FILE:
      return "Local file I/O operation failed";
    case YPPUSH_SKEW:
      return "Map version skew during transfer";
    case YPPUSH_CLEAR:
      return "Can't send \"Clear\" req to local ypserv";
    case YPPUSH_FORCE:
      return "No local order number in map  use -f flag.";
    case YPPUSH_XFRERR:
      return "ypxfr error";
    case YPPUSH_REFUSED:
      return "Transfer request refused by ypserv";
    }
  return "YPPUSH: Unknown Error, this should not happen!";
}

void *
yppushproc_null_1_svc (void *req __attribute__ ((unused)),
		       struct svc_req *rqstp __attribute__ ((unused)))
{
  static void *resp = NULL;

  if (verbose_flag > 1)
    yp_msg ("yppushproc_null_1_svc\n");

  return &resp;
}

yppushresp_xfr *
yppushproc_xfrresp_1_svc (void *req, struct svc_req * rqstp)
{
  static yppushresp_xfr resp;
  struct sockaddr_in *sin;
  char *h;
  struct hostent *hp;

  if (verbose_flag > 1)
    yp_msg ("yppushproc_xfrresp_1_svc\n");

  sin = svc_getcaller (rqstp->rq_xprt);

  hp = gethostbyaddr ((char *) &sin->sin_addr.s_addr,
		      sizeof (sin->sin_addr.s_addr), AF_INET);
  h = (hp && hp->h_name) ? hp->h_name : inet_ntoa (sin->sin_addr);

  memcpy (&resp, (yppushresp_xfr *) req, sizeof (yppushresp_xfr));
  if (verbose_flag)
    {
      yp_msg ("Status received from ypxfr on %s:\n", h);
      yp_msg ("\tTransfer %sdone: %s\n", resp.status == YPPUSH_SUCC ? "" : "not ",
	      yppush_err_string (resp.status));
    }
  else if (resp.status != YPPUSH_SUCC)
    yp_msg ("%s: %s\n", h, yppush_err_string (resp.status));

  return &resp;
}

void
yppush_xfrrespprog_1 (struct svc_req *rqstp, SVCXPRT * transp)
{
  union
  {
    int fill;
  }
  argument;
  static char *result;
  static bool_t (*xdr_argument) (XDR *, void *), (*xdr_result) (XDR *, void *);
  static char *(*local) (void *, struct svc_req *);

  if (verbose_flag > 1)
    yp_msg ("yppush_xfrrespprog_1\n");

  switch (rqstp->rq_proc)
    {
    case YPPUSHPROC_NULL:
      xdr_argument = (bool_t (*)(XDR *, void *)) xdr_void;
      xdr_result = (bool_t (*)(XDR *, void *)) xdr_void;
      local = (char *(*)(void *, struct svc_req *)) yppushproc_null_1_svc;
      break;

    case YPPUSHPROC_XFRRESP:
      xdr_argument = (bool_t (*)(XDR *, void *)) xdr_yppushresp_xfr;
      xdr_result = (bool_t (*)(XDR *, void *)) xdr_void;
      local = (char *(*)(void *, struct svc_req *)) yppushproc_xfrresp_1_svc;
      break;

    default:
      svcerr_noproc (transp);
      exit (1);
    }
  memset (&argument, '\0', sizeof (argument));
  if (!svc_getargs (transp, (xdrproc_t) xdr_argument, (void *) &argument))
    {
      svcerr_decode (transp);
      exit (1);
    }
  result = (*local) (&argument, rqstp);
  if (result != NULL &&
      !svc_sendreply (transp, (xdrproc_t) xdr_result, result))
    {
      svcerr_systemerr (transp);
    }
  if (!svc_freeargs (transp, (xdrproc_t) xdr_argument, (void *) &argument))
    {
      yp_msg ("unable to free arguments\n");
      exit (1);
    }
  exit (0);
}

static void
yppush_svc_run (char *target)
{
#ifdef FD_SETSIZE
  fd_set readfds;
#else
  int readfds;
#endif /* def FD_SETSIZE */
  struct timeval tr, tb;

  tb.tv_sec = timeout;
  tb.tv_usec = 0;
  tr = tb;

  for (;;)
    {
#ifdef FD_SETSIZE
      readfds = svc_fdset;
#else
      readfds = svc_fds;
#endif /* def FD_SETSIZE */
      switch (select (_rpc_dtablesize (), &readfds, (void *) 0, (void *) 0, &tr))
	{
	case -1:
	  if (errno == EINTR)
	    {
	      tr = tb;		/* Read the Linux select.2 manpage ! */
	      continue;
	    }
	  yp_msg ("svc_run: - select failed (%s)", strerror (errno));
	  return;
	case 0:
	  yp_msg ("%s->%s: Callback timed out\n", current_map, target);
	  exit (0);
	default:
	  svc_getreqset (&readfds);
	  break;
	}
    }
}

/*
 *    Compare 2 hostnames.
 */
static int
hostcmp (char *h1, char *h2)
{
  char buf1[MAXHOSTNAMELEN + 1], buf2[MAXHOSTNAMELEN + 1];
  char *p, *s;

  strncpy (buf1, h1, sizeof (buf1));
  strncpy (buf2, h2, sizeof (buf2));
  s = strchr (buf1, '.');
  p = strchr (buf2, '.');
  if (s && !p)
    *s = 0;
  if (p && !s)
    *p = 0;

  return strcasecmp (buf1, buf2);
}

static char *
get_dbm_entry (char *key)
{
  static char mappath[MAXPATHLEN + 2];
  char *val;
  datum dkey, dval;
#if defined(HAVE_LIBGDBM)
  GDBM_FILE dbm;
#elif defined (HAVE_NDBM)
  DBM *dbm;
#endif

#if defined(HAVE_SNPRINTF)
  if (snprintf (mappath, MAXPATHLEN, "%s/%s/%s", YPMAPDIR, DomainName, current_map) == MAXPATHLEN)
#else
  if (strlen (YPMAPDIR) + strlen (DomainName) + strlen (current_map) + 3 < MAXPATHLEN)
    sprintf (mappath, "%s/%s/%s", YPMAPDIR, DomainName, current_map);
  else
#endif
    {
      yp_msg ("ERROR: Path to long: %s/%s/%s\n", YPMAPDIR, DomainName, current_map);
      exit (-1);
    }

#if defined(HAVE_LIBGDBM)
  dbm = gdbm_open (mappath, 0, GDBM_READER, 0600, NULL);
#elif defined(HAVE_NDBM)
  dbm = dbm_open (mappath, O_CREAT | O_RDWR, 0600);
#endif
  if (dbm == NULL)
    {
      yp_msg ("YPPUSH: Cannot open %s\n", mappath);
      exit (-1);
    }

  dkey.dptr = key;
  dkey.dsize = strlen (dkey.dptr);
#if defined(HAVE_LIBGDBM)
  dval = gdbm_fetch (dbm, dkey);
#elif defined(HAVE_NDBM)
  dval = dbm_fetch (dbm, dkey);
#endif
  if (dval.dptr == NULL)
    val = NULL;
  else
    {
      val = malloc (dval.dsize + 1);
      strncpy (val, dval.dptr, dval.dsize);
      val[dval.dsize] = 0;
    }
#if defined(HAVE_LIBGDBM)
  gdbm_close (dbm);
#elif defined(HAVE_NDBM)
  dbm_close (dbm);
#endif
  return val;
}

static u_int
getordernum (void)
{
  char *val;
  u_int i;

  val = get_dbm_entry ("YP_LAST_MODIFIED");

  if (val == NULL)
    {
      if (verbose_flag > 1)
	yp_msg ("YPPUSH: %s: Cannot determine order number\n", current_map);
      return -1;
    }

  for (i = 0; i < strlen (val); i++)
    {
      if (!isdigit (val[i]))
	{
	  yp_msg ("YPPUSH: %s: Invalid order number '%s'\n",
		  current_map, val);
	  return -1;
	}
    }

  i = atoi (val);

  free (val);

  return i;
}

/*
** Create with the ypservers or slaves.hostname map a list with all
** slave servers we should send the new map
*/
/* NetBSD has a different prototype in struct ypall_callback */
#if defined(__NetBSD__)
static int
add_slave_server (u_long status, char *key, int keylen,
		  char *val, int vallen, void *data __attribute__ ((unused)))
#else
static int
add_slave_server (int status, char *key, int keylen,
		  char *val, int vallen, char *data __attribute__ ((unused)))
#endif
{
  char host[YPMAXPEER + 2];
  struct hostlist *tmp;

  if (verbose_flag > 1)
    yp_msg ("add_slave_server: Key=%.*s, Val=%.*s, status=%d\n", keylen, key, vallen, val, status);

  if (status != YP_TRUE)
    return status;

#if defined(HAVE_SNPRINTF)
  if (snprintf (host, YPMAXPEER, "%.*s", vallen, val) == YPMAXPEER)
#else
  if (vallen < YPMAXPEER)
    sprintf (host, "%.*s", vallen, val);
  else
#endif
    {
      yp_msg ("ERROR: add_slave_server: %.*s to long\n", vallen, val);
      exit (-1);
    }

  /* Do not add ourself! */
  if (hostcmp (local_hostname, host) == 0)
    {
      if (verbose_flag > 1)
	yp_msg ("add_slave_server: skipping %s\n", host);
      return 0;
    }

  if ((tmp = (struct hostlist *) malloc (sizeof (struct hostlist))) == NULL)
    {
      yp_msg ("malloc() failed: %s", strerror (errno));
      return -1;
    }
  tmp->hostname = strdup (host);
  tmp->next = hostliste;
  hostliste = tmp;

  return 0;
}

static void
child_sig_int (int sig __attribute__ ((unused)))
{
  if (CallbackProg != 0)
    svc_unregister (CallbackProg, 1);
  exit (-1);
}

static int
yppush_foreach (const char *host)
{
  SVCXPRT *CallbackXprt;
  CLIENT *PushClient = NULL;
  struct ypreq_xfr req;
  struct timeval tv = {10, 0};
  u_int transid;
  char server[YPMAXPEER + 2];
  int sock;
  struct sigaction sa;
#if HAVE_WAIT4
  struct rusage r;
#endif

  if (verbose_flag > 1)
    yp_msg ("yppush_foreach: host=%s\n", host);

  sa.sa_handler = child_sig_int;
  sigemptyset (&sa.sa_mask);
#if defined(linux) || (defined(sun) && defined(__srv4__))
  sa.sa_flags = SA_NOMASK;
  /* Do  not  prevent  the  signal   from   being
     received from within its own signal handler. */
#endif
  sigaction (SIGINT, &sa, NULL);

#if defined(HAVE_SNPRINTF)
  if (snprintf (server, YPMAXPEER, "%s", host) == YPMAXPEER)
#else
  if (strlen (host) < YPMAXPEER)
    sprintf (server, "%s", host);
  else
#endif
    {
      yp_msg ("ERROR: yppush_foreach: %.*s to long\n", host);
      exit (-1);
    }

  PushClient = clnt_create (server, YPPROG, YPVERS, "udp");
  if (PushClient == NULL)
    {
      yp_msg ("%s", host);
      clnt_pcreateerror ("");
      return 1;
    }

  sock = RPC_ANYSOCK;
  CallbackXprt = svcudp_create (sock);
  if (CallbackXprt == NULL)
    {
      yp_msg ("YPPUSH: Cannot create callback transport.\n");
      return 1;
    }
  for (CallbackProg = 0x40000000; CallbackProg < 0x5fffffff; CallbackProg++)
    {
      if (svc_register (CallbackXprt, CallbackProg, 1,
			yppush_xfrrespprog_1, IPPROTO_UDP))
	break;
    }

  switch (transid = fork ())
    {
    case -1:
      perror ("Cannot fork");
      exit (-1);
    case 0:
      yppush_svc_run (server);
      exit (0);
    default:
      close (CallbackXprt->xp_sock);
      req.map_parms.domain = (char *) DomainName;
      req.map_parms.map = (char *) current_map;
      /* local_hostname is correct since we have compared it with YP_MASTER_NAME */
      req.map_parms.peer = local_hostname;
      req.map_parms.ordernum = MapOrderNum;
      req.transid = transid;
      req.prog = CallbackProg;
      req.port = CallbackXprt->xp_port;

      if (verbose_flag)
	{
	  yp_msg ("%s has been called.\n", server);
	  if (verbose_flag > 1)
	    {
	      yp_msg ("\t->target: %s\n", server);
	      yp_msg ("\t->domain: %s\n", req.map_parms.domain);
	      yp_msg ("\t->map: %s\n", req.map_parms.map);
	      yp_msg ("\t->tarnsid: %d\n", req.transid);
	      yp_msg ("\t->prog: %d\n", req.prog);
	      yp_msg ("\t->master: %s\n", req.map_parms.peer);
	      yp_msg ("\t->ordernum: %d\n", req.map_parms.ordernum);
	    }
	}

      if (clnt_call (PushClient, YPPROC_XFR, (xdrproc_t) xdr_ypreq_xfr,
		     (caddr_t) &req, (xdrproc_t) xdr_void, NULL,
		     tv) != RPC_SUCCESS)
	{
	  clnt_perror (PushClient, "YPPUSH: Cannot call YPPROC_XFR");
	  kill (transid, SIGTERM);
	}

#if HAVE_WAIT4
      wait4 (transid, &sock, 0, &r);
#else
      waitpid (transid, &sock, 0);
#endif
      svc_unregister (CallbackProg, 1);
      CallbackProg = 0;
      if (PushClient != NULL)
	{
	  clnt_destroy (PushClient);
	  PushClient = NULL;
	}
    }

  return 0;
}

static void
sig_child (int sig __attribute__ ((unused)))
{
  int status;

#ifdef HAVE_WAIT3
  while (wait3 (&status, WNOHANG, NULL) > 0)
#else
  while (waitpid (-1, &status, WNOHANG) > 0)
#endif
    {
      if (verbose_flag > 1)
	yp_msg ("Child %d exists\n", WEXITSTATUS (status));
      children--;
    }
}

static inline void
Usage (int exit_code)
{
  yp_msg ("Usage: yppush [-d domain] [-t timeout] [-p #] [-h host] [-v] mapname ...\n");
  yp_msg ("       yppush --version\n");
  exit (exit_code);
}

int
main (int argc, char **argv)
{
  struct hostlist *tmp;

  struct ypall_callback f;
  enum ypstat y;
  struct sigaction sig;

  sig.sa_handler = sig_child;
  sigemptyset (&sig.sa_mask);
#if defined(linux) || (defined(sun) && defined(__srv4__))
  sig.sa_flags = SA_NOMASK;
  /* Do  not  prevent  the  signal   from   being
     received from within its own signal handler. */
#endif
  sigaction (SIGCHLD, &sig, NULL);

  while (1)
    {
      int c;
      int option_index = 0;
      static struct option long_options[] =
      {
	{"version", no_argument, NULL, '\255'},
	{"verbose", no_argument, NULL, 'v'},
	{"host", required_argument, NULL, 'h'},
	{"help", no_argument, NULL, 'u'},
	{"usage", no_argument, NULL, 'u'},
	{"parallel", required_argument, NULL, 'p'},
	{"timeout", required_argument, NULL, 't'},
	{NULL, 0, NULL, '\0'}
      };

      c = getopt_long (argc, argv, "d:vh:ut:p:j:", long_options, &option_index);
      if (c == EOF)
	break;
      switch (c)
	{
	case 'd':
	  DomainName = optarg;
	  break;
	case 'v':
	  verbose_flag++;
	  break;
	case 't':
	  timeout = atoi (optarg);
	  break;
	case 'j':
	case 'p':
	  maxchildren = atoi (optarg);
	  break;
	case 'h':
	  /* we can handle multiple hosts */
	  tmp = (struct hostlist *) malloc (sizeof (struct hostlist));
	  if (tmp == NULL)
	    {
	      yp_msg ("malloc() failed: %s", strerror (errno));
	      return 1;
	    }
	  tmp->hostname = strdup (optarg);
	  tmp->next = hostliste;
	  hostliste = tmp;
	  break;
	case 'u':
	  Usage (0);
	  break;
	case '\255':
	  fprintf (stderr, "yppush - NYS YP Server version %s\n", version);
	  exit (0);
	default:
	  Usage (1);
	}
    }

  argc -= optind;
  argv += optind;

  if (argc < 1)
    Usage (1);

  if (DomainName == NULL)
    {
      if (yp_get_default_domain (&DomainName) != 0)
	{
	  yp_msg ("YPPUSH: Cannot get default domain\n");
	  return 1;
	}
    }

  if (gethostname (local_hostname, MAXHOSTNAMELEN) != 0)
    {
      perror ("YPPUSH: gethostname");
      yp_msg ("YPPUSH: Cannot determine local hostname\n");
      return 1;
    }
#if USE_FQDN
  else
    {
      struct hostent *hp;

      if (!(hp = gethostbyname (local_hostname)))
	{
	  perror ("YPPUSH: gethostbyname()");
	  yp_msg ("YPPUSH: using not FQDN name\n");
	}
      else
	{
	  strncpy (local_hostname, hp->h_name, MAXHOSTNAMELEN);
	  local_hostname[MAXHOSTNAMELEN] = '\0';
	}
    }
#endif

  if (hostliste == NULL)
    {
      f.foreach = add_slave_server;
#ifdef OSF_KLUDGE
      y = yp_all (DomainName, "ypservers", f);
#else
      y = yp_all (DomainName, "ypservers", &f);
#endif
      if (y && y != YP_NOMORE)
	{
	  yp_msg ("Could not read ypservers map: %d %s\n",
		  y, yperr_string (y));
	}
    }

  while (*argv)
    {
      char *val;

      current_map = *argv++;
      val = get_dbm_entry ("YP_MASTER_NAME");
      if (val && strcasecmp (val, local_hostname) != 0)
	{
	  yp_msg ("YPPUSH: %s is not the master for %s, try it from %s.\n",
		  local_hostname, current_map, val);
	  free (val);
	  continue;
	}
      else if (val)
	free (val);

      MapOrderNum = getordernum ();
#if 0
      if (MapOrderNum == 0xffffffff)
	continue;
#endif
      tmp = hostliste;
      while (tmp != NULL)
	{
	  while (children >= maxchildren)
	    sleep (1);
	  children++;
	  switch (fork ())
	    {
	    case -1:
	      perror ("YPPUSH: Cannot fork");
	      exit (1);
	    case 0:
	      yppush_foreach (tmp->hostname);
	      exit (children);
	    default:
	      if (verbose_flag > 1)
		yp_msg ("Start new child (%d)\n", children);
	      break;
	    }
	  tmp = tmp->next;
	}
      while (children != 0)
	{
	  sleep (10);
	  if (verbose_flag > 1)
	    yp_msg ("Running Children: %d\n", children);
	}
    }

  if (verbose_flag > 1)
    yp_msg ("all done (%d running childs)\n", children);

  return 0;
}
