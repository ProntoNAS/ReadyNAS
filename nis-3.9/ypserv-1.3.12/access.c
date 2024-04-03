/* Copyright (C) 1997, 1998, 1999 Thorsten Kukuk

   This file is part of the NYS YP Server.

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
   Cambridge, MA 02139, USA.

   Author: Thorsten Kukuk <kukuk@suse.de> */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "yp.h"

#include <netdb.h>
#include <syslog.h>
#ifndef LOG_DAEMON
#include <sys/syslog.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ypserv.h"
#include "yp_msg.h"
#include "yp_db.h"

#ifndef HAVE_LIBWRAP

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/param.h>
/* #include <sys/socket.h> */

#endif

static conffile_t *conf = NULL;

void
load_config (void)
{
  conffile_t *tmp;

  if (conf != NULL)
    {
      yp_msg ("Reloading %s/ypserv.conf\n", CONFDIR);
      while (conf)
	{
	  tmp = conf;
	  conf = conf->next;

	  free (tmp->map);
	  free (tmp);
	}
    }

  conf = load_ypserv_conf (CONFDIR);
}

#ifndef HAVE_LIBWRAP

#ifndef SECURENETS
#define SECURENETS "securenets"
#endif

typedef struct securenet
{
  struct in_addr netmask;
  struct in_addr network;
  struct securenet *next;
}
securenet_t;

static securenet_t *securenets = NULL;

void
load_securenets (void)
{
  char buf1[128], buf2[128], buf3[128];
  FILE *in;
  securenet_t *work, *tmp;
  int line = 0;

  /* If securenets isn't NULL, we should reload the securents file. */
  if (securenets != NULL)
    {
      yp_msg ("Reloading securenets file\n");
      while (securenets != NULL)
	{
	  work = securenets;
	  securenets = securenets->next;
	  free (work);
	}
    }
  securenets = NULL;
  work = NULL;
  tmp = NULL;

  if ((in = fopen (SECURENETS, "r")) == NULL)
    {
      yp_msg ("WARNING: no %s file found!\n", SECURENETS);
      return;
    }

  while (!feof (in))
    {
      int host = 0;

      memset (buf1, 0, sizeof (buf1));
      memset (buf2, 0, sizeof (buf2));
      memset (buf3, 0, sizeof (buf3));
      fgets (buf3, 128, in);
      line++;

      if (buf3[0] == '\0' || buf3[0] == '#' || buf3[0] == '\n')
	continue;

      if (sscanf (buf3, "%s %s", buf1, buf2) != 2)
	{
	  yp_msg ("securenets(%d): malformed line, ignore it\n", line);
	  continue;
	}

      if ((tmp = malloc (sizeof (securenet_t))) == NULL)
	{
	  yp_msg ("ERROR: could not allocate enough memory! [%s|%d]\n", __FILE__, __LINE__);
	  exit (1);
	}

      tmp->next = NULL;

      if (strcmp (buf1, "host") == 0)
	{
	  strcpy (buf1, "255.255.255.255");
	  host = 1;
	}
      else if (strcmp (buf1, "255.255.255.255") == 0)
	host = 1;

#if defined(HAVE_INET_ATON)
      if (!inet_aton (buf1, &tmp->netmask) && !host)
#else
      if ((tmp->netmask.s_addr = inet_addr (buf1)) == (-1) && !host)
#endif
	{
	  yp_msg ("securenets(%d): %s is not a correct netmask!\n", line, buf1);
	  free (tmp);
	  continue;
	}

#if defined(HAVE_INET_ATON)
      if (!inet_aton (buf2, &tmp->network))
#else
      if ((tmp->network.s_addr = inet_addr (buf2)) == (-1))
#endif
	{
	  yp_msg ("securenets(%d): %s is not a correct network address!\n", line, buf2);
	  free (tmp);
	  continue;
	}

      if (work == NULL)
	{
	  work = tmp;
	  securenets = work;
	}
      else
	{
	  work->next = tmp;
	  work = work->next;
	}
    }
  fclose (in);

  if (debug_flag)
    {
      tmp = securenets;
      while (tmp)
	{
	  yp_msg ("Find securenet: %s", inet_ntoa (tmp->netmask));
	  yp_msg (" %s\n", inet_ntoa (tmp->network));
	  tmp = tmp->next;
	}
    }
}

static int
securenet_host (const struct in_addr sin_addr)
{
  securenet_t *ptr;

  ptr = securenets;

  if (ptr == NULL)
    return 1;
  else
    while (ptr != NULL)
      {
	if ((ptr->netmask.s_addr & sin_addr.s_addr) ==
	    ptr->network.s_addr)
	  return 1;
	ptr = ptr->next;
      }
  return 0;
}

#else /* HAVE_LIBWRAP */
#include <tcpd.h>
int allow_severity = LOG_INFO;	/* run-time adjustable */
int deny_severity = LOG_WARNING;	/* ditto */
#endif

/*
   ** Give a string with the DEFINE description back
 */
static char *
ypproc_name (int proc)
{
  switch (proc)
    {
    case YPPROC_NULL:
      return "ypproc_null";
    case YPPROC_DOMAIN:
      return "ypproc_domain";
    case YPPROC_DOMAIN_NONACK:
      return "ypproc_domain_nonack";
    case YPPROC_MATCH:
      return "ypproc_match";
    case YPPROC_FIRST:
      return "ypproc_first";
    case YPPROC_NEXT:
      return "ypproc_next";
    case YPPROC_XFR:
      return "ypproc_xfr";
    case YPPROC_CLEAR:
      return "ypproc_clear";
    case YPPROC_ALL:
      return "ypproc_all";
    case YPPROC_MASTER:
      return "ypproc_master";
    case YPPROC_ORDER:
      return "ypproc_order";
    case YPPROC_MAPLIST:
      return "ypproc_maplist";
    default:
      return "unknown ?";
    }
}

/*
   ** The is_valid_domain function checks the domain specified bze the
   ** caller to make sure it's actually served by this server.
   **
   ** Return 1 if the name is a valid domain name served by us, else 0.
 */

int
is_valid_domain (const char *domain)
{
  struct stat sbuf;


  if (domain == NULL ||
      strcmp (domain, "binding") == 0 ||
      strcmp (domain, "..") == 0 ||
      strcmp (domain, ".") == 0 ||
      strchr (domain, '/'))
    return 0;

  if (stat (domain, &sbuf) < 0 || !S_ISDIR (sbuf.st_mode))
    return 0;

  return 1;
}

/*
   ** By default, we use the securenet list, to check if the client
   ** is secure. But if you have the libwrap.a library, you could use
   ** them, too. It's much more flexible.
   **
   ** Return 1, if request comes from an authorized host
   ** Return 0, if request comes from an unauthorized host
   ** Return -1, if the domain is not valid
 */

int
is_valid_host (struct svc_req *rqstp, const char *map, const char *domain)
{
#ifdef HAVE_LIBWRAP
  extern int hosts_ctl (char *, char *, char *, char *);
#endif
  struct sockaddr_in *sin;
  int status;
  static unsigned long int oldaddr = 0;		/* so we dont log multiple times */
  static int oldstatus = -1;

  if ((domain != NULL) && (is_valid_domain (domain) == 0))
    return -1;

  sin = svc_getcaller (rqstp->rq_xprt);

#ifndef FROM_UNKNOWN
#define FROM_UNKNOWN STRING_UNKNOWN
#endif

#ifdef HAVE_LIBWRAP
  status = hosts_ctl (progname, FROM_UNKNOWN, inet_ntoa (sin->sin_addr), "");
#else
  status = securenet_host (sin->sin_addr);
#endif

  if ((map != NULL) && status)
    {
      conffile_t *work;

      work = conf;
      while (work)
	{
	  if ((sin->sin_addr.s_addr & work->netmask.s_addr) == work->network.s_addr)
	    if (strcmp (work->map, map) == 0 || strcmp (work->map, "*") == 0)
	      break;
	  work = work->next;
	}

      if (work != NULL)
	switch (work->security)
	  {
	  case SEC_DES:
	    if (rqstp->rq_cred.oa_flavor != AUTH_DES)
	      {
		if (debug_flag)
		  yp_msg ("No AUTH_DES !\n");
		svcerr_weakauth (rqstp->rq_xprt);
		status = 0;
	      }
	    else if (work->mangle)
	      status = 200 + work->field;
	    break;
	  case SEC_NONE:
	    if (work->mangle)
	      status = 200 + work->field;
	    break;
	  case SEC_DENY:
	    status = 0;
	    break;
	  case SEC_PORT:
	    if (ntohs (sin->sin_port) >= IPPORT_RESERVED)
	      {
		if (work->mangle == 0)
		  status = 0;
		else
		  status = 200 + work->field;
	      }
	    break;
	  }
      else if (domain != NULL)
	{
	  /*
	     ** The map is not in the access list, maybe it
	     ** has a YP_SECURE key ?
	   */
	  DB_FILE dbp = ypdb_open (domain, map);
	  if (dbp != NULL)
	    {
	      datum key;

	      key.dsize = sizeof ("YP_SECURE") - 1;
	      key.dptr = "YP_SECURE";
	      if (ypdb_exists (dbp, key))
		if (ntohs (sin->sin_port) >= IPPORT_RESERVED)
		  status = 0;
	      ypdb_close (dbp);
	    }
	}
    }

  if (debug_flag)
    {
      yp_msg ("%sconnect from %s\n", status ? "" : "refused ",
	      inet_ntoa (sin->sin_addr));
    }
  else
    {
      if (!status && ((sin->sin_addr.s_addr != oldaddr)
		      || (status != oldstatus)))
	syslog (LOG_WARNING, "refused connect from %s:%d to procedure %s\n",
		inet_ntoa (sin->sin_addr), ntohs (sin->sin_port),
		ypproc_name (rqstp->rq_proc));
    }
  oldaddr = sin->sin_addr.s_addr;
  oldstatus = status;

  return status;

}
