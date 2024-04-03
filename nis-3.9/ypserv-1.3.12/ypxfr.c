/* Copyright (c) 1996, 1997, 1998, 1999, 2000 Thorsten Kukuk
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

#define _GNU_SOURCE

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <time.h>		/* Ultrix needs this */
#include "system.h"

#include <rpc/rpc.h>
#ifdef HAVE_RPC_CLNT_SOC_H
#include <rpc/clnt_soc.h>
#endif
#include <rpc/pmap_clnt.h>
#include "yp.h"
#include "ypxfrd.h"
#include <rpcsvc/ypclnt.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <syslog.h>
#ifndef LOG_DAEMON
#include <sys/syslog.h>		/* for ULTRIX */
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <memory.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>
#if defined(HAVE_GETOPT_H) && defined(HAVE_GETOPT_LONG)
#include <getopt.h>
#else
#include <compat/getopt.h>
#endif

#include "version.h"
#include "yp_msg.h"
#include "ypxfr.h"

#if defined(HAVE_LIBGDBM)
#include <gdbm.h>

#define ypdb_store gdbm_store
#define YPDB_REPLACE GDBM_REPLACE
#define ypdb_close gdbm_close
#define ypdb_fetch gdbm_fetch
static GDBM_FILE dbm;
#elif defined (HAVE_NDBM)
#include <ndbm.h>

#define ypdb_store dbm_store
#define YPDB_REPLACE DBM_REPLACE
#define ypdb_close dbm_close
#define ypdb_fetch dbm_fetch
static DBM *dbm;
#endif

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

#if defined (__NetBSD__) || (defined(__GLIBC__) && (__GLIBC__ == 2 && __GLIBC_MINOR__ == 0))
/* <rpc/rpc.h> is missing the prototype */
int callrpc (char *host, u_long prognum, u_long versnum, u_long procnum,
	     xdrproc_t inproc, char *in, xdrproc_t outproc, char *out);
#endif

static char *path_ypdb = YPMAPDIR;

static char SourceHost[MAXHOSTNAMELEN], *TargetDomain = NULL, *SourceDomain = NULL;
static struct in_addr IpAddress;
static int Force = 0, NoClear = 0, TaskId = 0, ProgramNumber = 0, PortNumber = 0;

int debug_flag = 0;

static char *
ypxfr_err_string (enum ypxfrstat error)
{
  switch (error)
    {
    case YPXFR_SUCC:
      return "Success";
    case YPXFR_AGE:
      return "Master's version not newer";
    case YPXFR_NOMAP:
      return "Can't find server for map";
    case YPXFR_NODOM:
      return "Domain not supported";
    case YPXFR_RSRC:
      return "Local resource alloc failure";
    case YPXFR_RPC:
      return "RPC failure talking to server";
    case YPXFR_MADDR:
      return "Can't get master address";
    case YPXFR_YPERR:
      return "YP server/map db error";
    case YPXFR_BADARGS:
      return "Request arguments bad";
    case YPXFR_DBM:
      return "Local dbm operation failed";
    case YPXFR_FILE:
      return "Local file I/O operation failed";
    case YPXFR_SKEW:
      return "Map version skew during transfer";
    case YPXFR_CLEAR:
      return "Can't send \"Clear\" req to local ypserv";
    case YPXFR_FORCE:
      return "No local order number in map  use -f flag.";
    case YPXFR_XFRERR:
      return "ypxfr error";
    case YPXFR_REFUSED:
      return "Transfer request refused by ypserv";
    }
  return "Unknown Error, should not happen";
}

/* NetBSD has a different prototype in struct ypall_callback */
#if defined(__NetBSD__)
static int
ypxfr_foreach (u_long status, char *key, int keylen,
	       char *val, int vallen, void *data __attribute__ ((unused)))
#else
static int
ypxfr_foreach (int status, char *key, int keylen,
	       char *val, int vallen, char *data __attribute__ ((unused)))
#endif
{
  datum outKey, outData;

  if (debug_flag > 1)
    {
      if (keylen != 0 && vallen != 0)
	yp_msg ("ypxfr_foreach: key=%.*s, val=%.*s\n", keylen, key, vallen, val);
      else
	yp_msg ("ypxfr_foreach: empty key/value pair!\n");
    }

  if (status == YP_NOMORE)
    return 0;

  if (status != YP_TRUE)
    {
      int s = ypprot_err (status);
      yp_msg ("%s\n", yperr_string (s));
      return 1;
    }

  if (keylen != 0)
    {
      /* Hack for broken maps: If keylen/vallen includes the trailing 0,
	 decrement it. */
      if (keylen > 0 && key[keylen - 1] == '\0')
	--keylen;
      if (vallen > 0 && val[vallen - 1] == '\0')
	--vallen;
      /* Now save it */
      outKey.dptr = keylen ? key : "";
      outKey.dsize = keylen;
      outData.dptr = vallen ? val : "";
      outData.dsize = vallen;
      if (ypdb_store (dbm, outKey, outData, YPDB_REPLACE) != 0)
	return 1;
    }

  return 0;
}

/*
** Collects all the answers for our query for hosts running ypserv
*/
static char *ypserver = NULL;
static bool_t
eachresult (bool_t * resp, struct sockaddr_in *addr)
{
  if (*resp)
    {
      struct hostent *h;
      h = gethostbyaddr ((char *) &addr->sin_addr.s_addr,
			 sizeof (addr->sin_addr.s_addr), AF_INET);

      if (ypserver == NULL)
	ypserver = strdup (h->h_name);

      if (debug_flag)
	{
	  yp_msg ("Find running ypserv on %s\n", h->h_name);
	}
      return 1;
    }
  else
    {
      return 0;
    }
}

/* At first, use the yp lib routines. If we are not bound to a server,
   do a broadcast and search one. */
static int
__yp_master (char *domain, char *map, char **master)
{
  static ypresp_master res;
  ypreq_nokey req;
  int status;

  if ((status = yp_master (domain, map, master)) != 0)
    {
      bool_t out;

      status = clnt_broadcast (YPPROG, YPVERS, YPPROC_DOMAIN_NONACK,
			       (xdrproc_t) xdr_domainname, (void *) &domain,
			       (xdrproc_t) xdr_bool, (void *) &out,
			       (resultproc_t) eachresult);
      if (status != RPC_SUCCESS)
	{
	  yp_msg ("broadcast: %s.\n", clnt_sperrno (status));
	}
      else
	{
	  req.domain = domain;
	  req.map = map;
	  if ((status = callrpc (ypserver, YPPROG, YPVERS, YPPROC_MASTER,
				 (xdrproc_t) xdr_ypreq_nokey, (void *) &req,
			      (xdrproc_t) xdr_ypresp_master, (void *) &res))
	      != RPC_SUCCESS)
	    {
	      yp_msg ("failed to send 'ypproc_master' to %s's ypserv: %s\n",
		      ypserver, clnt_sperrno ((enum clnt_stat) status));

	    }
	  else
	    {
	      if ((res.peer != NULL) && (strlen (res.peer) > 0))
		*master = strdup (res.peer);
	      else
		return 1;
	    }
	}
    }
  return status;
}

extern struct ypall_callback *xdr_ypall_callback;

static enum ypxfrstat
ypxfr (char *mapName)
{
  int localOrderNum = 0;
  int masterOrderNum = 0;
  char *masterName = NULL;
  struct ypall_callback callback;
  char dbName[MAXPATHLEN + 1];
  char dbName2[MAXPATHLEN + 1];
  CLIENT *clnt_tcp, *clnt_udp;
  struct sockaddr_in sockaddr, sockaddr_udp;
  struct ypreq_nokey req_nokey;
  struct ypreq_key req_key;
  struct ypresp_order *resp_order;
  struct ypresp_master *resp_master;
  struct ypresp_val *resp_val;
  struct ypresp_all *resp_all;
  datum outKey, outData;
  char orderNum[12];
  int result;
  int sock;
  struct timeval TIMEOUT = {25, 0};

  /*
  ** Get the Master hostname for the map, if no explicit
  ** sourcename is given.
   */
  if (SourceHost[0] == '\0')
    {
      if (__yp_master (SourceDomain, mapName, &masterName))
	return YPXFR_MADDR;
      strcpy (SourceHost, masterName);
    }

  /*
  ** Build a connection to the host with the master map, not the
  ** the nearest ypserv, because this map could be out of date.
   */
  memset (&sockaddr, '\0', sizeof (sockaddr));
  sockaddr.sin_family = AF_INET;
  {
    struct hostent *h = gethostbyname (SourceHost);
    if (!h)
      return YPXFR_RSRC;

    memcpy (&sockaddr.sin_addr, h->h_addr, sizeof sockaddr.sin_addr);
  }
  memcpy (&sockaddr_udp, &sockaddr, sizeof (sockaddr));

  sock = RPC_ANYSOCK;
  clnt_tcp = clnttcp_create (&sockaddr, YPPROG, YPVERS, &sock, 0, 0);
  if (clnt_tcp == NULL)
    {
      clnt_pcreateerror ("YPXFR");
      return YPXFR_RPC;
    }

  sock = RPC_ANYSOCK;
  clnt_udp = clntudp_create (&sockaddr_udp, YPPROG, YPVERS, TIMEOUT, &sock);
  if (clnt_udp == NULL)
    {
      clnt_pcreateerror ("YPXFR");
      return YPXFR_RPC;
    }

  req_nokey.domain = SourceDomain;
  req_nokey.map = mapName;
  if ((resp_order = ypproc_order_2 (&req_nokey, clnt_udp)) == NULL)
    {
      clnt_perror (clnt_udp, "masterOrderNum");
      masterOrderNum = time (NULL); /* We set it to the current time.
                                       So a new map will be always newer. */

    }
  else
    {
      masterOrderNum = resp_order->ordernum;
      xdr_free ((xdrproc_t) xdr_ypresp_order, (char *) resp_order);
    }

  /*
   *	Not sure why, but sometimes resp_order->ordernum gets
   *	set to INT_MAX ? I don't know if this happens in glibc or
   *	that the remote server returns this, but it seems safest
   *	to bail out in this case ....
   */
#ifndef INT_MAX
#  define INT_MAX 2147483647
#endif
  if (masterOrderNum == INT_MAX)
    {
      yp_msg("ERROR: order number of map %s == INT_MAX (%d), impossible",
		mapName, masterOrderNum);
      return YPXFR_YPERR;
    }

  if (masterName == NULL)
    {
      req_nokey.domain = SourceDomain;
      req_nokey.map = mapName;
      if ((resp_master = ypproc_master_2 (&req_nokey, clnt_udp)) == NULL)
	{
	  clnt_perror (clnt_udp, "ypmaster");
	}
      else
	{
	  masterName = strdup (resp_master->peer);
	  xdr_free ((xdrproc_t) xdr_ypresp_master, (char *) resp_master);
	}
    }
  /* If we doesn't force the map, look, if the new map is really newer */
  if (!Force)
    {
      datum inKey, inVal;

      if (strlen (path_ypdb) + strlen (TargetDomain) + strlen (mapName) + 3 < MAXPATHLEN)
	sprintf (dbName, "%s/%s/%s", path_ypdb, TargetDomain, mapName);
      else
	{
	  yp_msg ("ERROR: Path to long: %s/%s/%s\n", path_ypdb, TargetDomain, mapName);
	  return YPXFR_RSRC;
	}

#if defined(HAVE_LIBGDBM)
      dbm = gdbm_open (dbName, 0, GDBM_READER, 0600, NULL);
#elif defined(HAVE_NDBM)
      dbm = dbm_open (dbName, O_CREAT|O_RDWR, 0600);
#endif
      if (dbm == NULL)
	{
	  yp_msg ("Cannot open old %s - ignored.\n", dbName);
	  localOrderNum = 0;
	}
      else
	{
	  inKey.dptr = "YP_LAST_MODIFIED";
	  inKey.dsize = strlen (inKey.dptr);
	  inVal = ypdb_fetch (dbm, inKey);
	  if (inVal.dptr)
	    {
	      int i;
	      char *d = inVal.dptr;
	      for (i = 0; i < inVal.dsize; i++, d++)
		{
		  if (!isdigit (*d))
		    {
		      ypdb_close (dbm);
		      return YPXFR_SKEW;
		    }
		}
	      localOrderNum = atoi (inVal.dptr);
	    }
	  ypdb_close (dbm);
	}
      if (debug_flag > 1)
	yp_msg ("masterOrderNum=%d, localOrderNum=%d\n",
		masterOrderNum, localOrderNum);
      if (localOrderNum >= masterOrderNum)
	return YPXFR_AGE;
    }

  if (strlen (path_ypdb) + strlen (TargetDomain) + strlen (mapName) + 4 < MAXPATHLEN)
    sprintf (dbName, "%s/%s/%s~", path_ypdb, TargetDomain, mapName);
  else
    {
      yp_msg ("ERROR: Path to long: %s/%s/%s~\n", path_ypdb, TargetDomain, mapName);
      return YPXFR_RSRC;
    }

  /*
  ** Try to use ypxfrd for getting the new map. If it fails, use the old
  ** method.
  */
  if ((result = ypxfrd_transfer (masterName, mapName, TargetDomain, dbName)) != 0)
    {

#if defined(HAVE_LIBGDBM)
      dbm = gdbm_open (dbName, 0, GDBM_NEWDB, 0600, NULL);
#elif defined(HAVE_NDBM)
      dbm = dbm_open (dbName, O_CREAT|O_RDWR, 0600);
#endif
       if (dbm == NULL)
	{
	  yp_msg ("Cannot open %s\n", dbName);
	  return YPXFR_DBM;
	}

      outKey.dptr = "YP_MASTER_NAME";
      outKey.dsize = strlen (outKey.dptr);
      outData.dptr = masterName;
      outData.dsize = strlen (outData.dptr);
      if (ypdb_store (dbm, outKey, outData, YPDB_REPLACE) != 0)
	{
	  ypdb_close (dbm);
	  unlink (dbName);
	  return YPXFR_DBM;
	}
      sprintf (orderNum, "%d", masterOrderNum);
      outKey.dptr = "YP_LAST_MODIFIED";
      outKey.dsize = strlen (outKey.dptr);
      outData.dptr = orderNum;
      outData.dsize = strlen (outData.dptr);
      if (ypdb_store (dbm, outKey, outData, YPDB_REPLACE) != 0)
	{
	  ypdb_close (dbm);
	  unlink (dbName);
	  return YPXFR_DBM;
	}

      /*
         ** Get the YP_INTERDOMAIN field. This is needed from the SunOS ypserv.
         ** We ignore this, since we have the "dns" option. But a Sun could be
         ** a NIS slave server and request the map from us.
       */
      req_key.domain = SourceDomain;
      req_key.map = mapName;
      req_key.key.keydat_val = "YP_INTERDOMAIN";
      req_key.key.keydat_len = strlen ("YP_INTERDOMAIN");
      if ((resp_val = ypproc_match_2 (&req_key, clnt_udp)) == NULL)
	{
	  clnt_perror (clnt_udp, "yproc_match");
	}
      else
	{
	  if (resp_val->stat == YP_TRUE)
	    {
	      outKey.dptr = "YP_INTERDOMAIN";
	      outKey.dsize = strlen (outKey.dptr);
	      outData.dptr = "";
	      outData.dsize = 0;
	      if (ypdb_store (dbm, outKey, outData, YPDB_REPLACE) != 0)
		{
		  ypdb_close (dbm);
		  unlink (dbName);
		  return YPXFR_DBM;
		}
	    }
	  xdr_free ((xdrproc_t) xdr_ypresp_val, (char *) resp_val);
	}

      /* Get the YP_SECURE field. */
      req_key.domain = SourceDomain;
      req_key.map = mapName;
      req_key.key.keydat_val = "YP_SECURE";
      req_key.key.keydat_len = strlen ("YP_SECURE");
      if ((resp_val = ypproc_match_2 (&req_key, clnt_udp)) == NULL)
	{
	  clnt_perror (clnt_udp, "yproc_match");
	}
      else
	{
	  if (resp_val->stat == YP_TRUE)
	    {
	      outKey.dptr = "YP_SECURE";
	      outKey.dsize = strlen (outKey.dptr);
	      outData.dptr = "";
	      outData.dsize = 0;
	      if (ypdb_store (dbm, outKey, outData, YPDB_REPLACE) != 0)
		{
		  ypdb_close (dbm);
		  unlink (dbName);
		  return YPXFR_DBM;
		}
	    }
	  xdr_free ((xdrproc_t) xdr_ypresp_val, (char *) resp_val);
	}

      callback.foreach = ypxfr_foreach;
      callback.data = NULL;
      {

	req_nokey.domain = SourceDomain;
	req_nokey.map = mapName;
	xdr_ypall_callback = &callback;
	if ((resp_all = ypproc_all_2 (&req_nokey, clnt_tcp)) == NULL)
	  {
	    clnt_perror (clnt_tcp, "ypall");
	  }
	else
	  {
	    switch (resp_all->ypresp_all_u.val.stat)
	      {
	      case YP_TRUE:
	      case YP_NOMORE:
		result = 0;
		break;
	      default:
		result = ypprot_err (resp_all->ypresp_all_u.val.stat);
	      }
	    clnt_freeres (clnt_tcp, (xdrproc_t) ypxfr_xdr_ypresp_all, (void *) resp_all);
	  }
      }

      clnt_destroy (clnt_tcp);

      ypdb_close (dbm);
    }

  if (strlen (path_ypdb) + strlen (TargetDomain) + strlen (mapName) + 3 < MAXPATHLEN)
    sprintf (dbName2, "%s/%s/%s", path_ypdb, TargetDomain, mapName);
  else
    {
      yp_msg ("ERROR: Path to long: %s/%s/%s\n", path_ypdb, TargetDomain, mapName);
      return YPXFR_RSRC;
    }

  if (result == 0)
    {
      unlink (dbName2);
      rename (dbName, dbName2);
    }
  else
    unlink(dbName);

  if (!NoClear)
    {
      char in = 0;
      char *out = NULL;
      int stat;

      if ((stat = callrpc ("localhost", YPPROG, YPVERS, YPPROC_CLEAR,
			   (xdrproc_t) xdr_void, &in,
			   (xdrproc_t) xdr_void, out)) != RPC_SUCCESS)
	{
	  yp_msg ("failed to send 'clear' to local ypserv: %s\n",
		  clnt_sperrno ((enum clnt_stat) stat));
	  return YPXFR_CLEAR;
	}
    }
  return result == 0 ? YPXFR_SUCC : YPXFR_YPERR;
}

static void
Usage (int exit_code)
{
  fprintf (stderr, "usage: ypxfr [-f] [-c] [-d target domain] [-h source host] [-s source domain]\n");
  fprintf (stderr, "             [-C taskid program-number ipaddr port] [-p yp_path] mapname ...\n");
  fprintf (stderr, "       ypxfr --version\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "where\n");
  fprintf (stderr, "\t-f forces transfer even if the master's copy is not newer.\n");
  fprintf (stderr, "\thost may be either a name or an internet\n");
  fprintf (stderr, "\t     address of form ww.xx.yy.zz\n");
  fprintf (stderr, "\t-c inhibits sending a \"Clear map\" message to the local ypserv.\n");
  fprintf (stderr, "\t-C is used by ypserv to pass callback information.\n");
  exit (exit_code);
}

int
main (int argc, char **argv)
{
  static char res;

  if (argc < 2)
    Usage (1);

  if (!isatty (fileno (stderr)))
    openlog ("ypxfr", LOG_PID, LOG_DAEMON);
  else
    debug_flag = 1;

  memset (SourceHost, '\0', sizeof (SourceHost));

  while (1)
    {
      int c;
      int option_index = 0;
      static struct option long_options[] =
      {
	{"version", no_argument, NULL, '\255'},
	{"debug", no_argument, NULL, '\254'},
	{"help", no_argument, NULL, 'u'},
	{"usage", no_argument, NULL, 'u'},
	{"path", required_argument, NULL, 'p'},
	{NULL, 0, NULL, '\0'}
      };

      c = getopt_long (argc, argv, "ufcd:h:p:s:C:S", long_options, &option_index);
      if (c == EOF)
	break;
      switch (c)
	{
	case 'p':
	  path_ypdb = optarg;
	  if (debug_flag)
	    yp_msg ("Using database directory: %s\n", path_ypdb);
	  break;
	case 'f':
	  Force++;
	  break;
	case 'c':
	  NoClear++;
	  break;
	case 'd':
	  TargetDomain = optarg;
	  break;
	case 'h':
	  strncpy (SourceHost, optarg, sizeof (SourceHost) - 1);
	  SourceHost[sizeof (SourceHost) - 1] = '\0';
	  break;
	case 's':
	  SourceDomain = optarg;
	  break;
	case 'C':
	  if (optind + 3 > argc)
	    {
	      Usage (1);
	      break;
	    }
	  TaskId = atoi (optarg);
	  ProgramNumber = atoi (argv[optind++]);
	  IpAddress.s_addr = inet_addr (argv[optind++]);
	  PortNumber = atoi (argv[optind++]);
	  break;
	case 'u':
	  Usage (0);
	  break;
	case '\254':
	  debug_flag = 2;
	  break;
	case '\255':
	  fprintf (stderr, "ypxfr - NYS YP Server version %s\n", version);
	  exit (0);
	default:
	  Usage (1);
	  break;
	}
    }
  argc -= optind;
  argv += optind;

  if (!TargetDomain)
    yp_get_default_domain (&TargetDomain);

  if (!SourceDomain)
    SourceDomain = TargetDomain;


  for (; *argv; argv++)
    {
      enum ypxfrstat y;

      /*
         ** Start the map Transfer
       */
      if ((y = ypxfr (*argv)) != YPXFR_SUCC)
	{
	  /*
	     ** Don't syslog "Master's version not newer" as that is
	     ** the common case.
	   */
	  if (debug_flag || y != YPXFR_AGE)
	    yp_msg ("ypxfr: %s\n", ypxfr_err_string (y));
	}

      /*
         ** Now send the status to the yppush program, so it can display a
         ** message for the sysop and do not timeout.
       */
      if (TaskId)
	{
	  struct sockaddr_in addr;
	  struct timeval wait;
	  CLIENT *clnt;
	  int s;
	  ypresp_xfr resp;
	  static struct timeval tv =
	  {10, 0};

	  memset (&addr, '\0', sizeof addr);
	  addr.sin_addr = IpAddress;
	  addr.sin_port = htons (PortNumber);
	  addr.sin_family = AF_INET;
	  wait.tv_sec = 25;
	  wait.tv_usec = 0;
	  s = RPC_ANYSOCK;

	  clnt = clntudp_create (&addr, ProgramNumber, 1, wait, &s);
	  if (!clnt)
	    {
	      clnt_pcreateerror ("ypxfr_callback");
	      continue;
	    }

	  resp.transid = TaskId;
	  resp.xfrstat = y;

	  if (clnt_call (clnt, 1, (xdrproc_t) xdr_ypresp_xfr, (void *) &resp,
			 (xdrproc_t) xdr_void, &res, tv) != RPC_SUCCESS)
	    clnt_perror (clnt, "ypxfr_callback");

	  clnt_destroy (clnt);
	}
    }
  return 0;
}
