/* server.c - YP server routines.

   Copyright (c) 1996, 1997, 1998, 1999, 2000 Thorsten Kukuk, Germany
   Copyright (c) 1993 Signum Support AB, Sweden

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

   Authors:
   Peter Eriksson <pen@signum.se>
   Thorsten Kukuk <kukuk@suse.de>
*/

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#define _GNU_SOURCE

#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <syslog.h>
#include <sys/param.h>
#include <errno.h>
#include <stdarg.h>
#include <alloca.h>

#include "yp.h"
#include "ypserv.h"
#include "yp_msg.h"
#include "yp_db.h"

volatile int children = 0;
int forked = 0;
int xfr_check_port = 1;

#if !defined(HAVE_STRDUP)
#include <compat/strdup.c>
#endif

#if !defined(HAVE_STRNDUP)
#include <compat/strndup.c>
#endif

#ifndef HAVE_STPCPY
#include <compat/stpcpy.c>
#endif

#if !defined(HAVE_GETHOSTNAME)
#include <sys/utsname.h>

int
gethostname (char *buf, int buflen)
{
  struct utsname nb;

  if (uname (&nb) < 0)
    return -1;

  if (strlen (nb.nodename) > buflen - 1)
    {
      strncpy (buf, nb.nodename, buflen);
      buf[buflen - 1] = '\0';
    }
  else
    strcpy (buf, nb.nodename);

  return 0;
}

#endif

/* repleace "field" with x, if field 3 != user id and user id != 0 */
static int
mangle_field (datum * val, int field)
{
  int i, j, k, anz;
  /* allocate a little bit more memory, it's safer, because the
     field could be empty */
  char *p = alloca (val->dsize + 3);

  if (p == NULL)
    {
      yp_msg ("ERROR: could not allocate enough memory! [%s|%d]\n",
	      __FILE__, __LINE__);
      return -1;
    }

  anz = 0;
  for (i = 0; i < val->dsize; ++i)
    {
      if (val->dptr[i] == ':')
        ++anz;
      if (anz + 1 == field)
        {
          anz = i;
          strncpy (p, val->dptr, anz);
          p[anz] = 0;
          /* if field == 1, we don't need a beginning ":" */
          if (field == 1)
            {
              strcat (p, "x");
              anz += 1;
            }
          else
            {
              strcat (p, ":x");
              anz += 2;
            }
          for (j = anz; j < val->dsize && val->dptr[j] != ':'; ++j);
          for (k = j; k < val->dsize; ++k)
            {
              p[anz] = val->dptr[k];
              ++anz;
            }
          free (val->dptr);
          val->dptr = strdup (p);
          val->dsize = anz;
          return 0;
        }
    }
  return 0;
}


void *
ypproc_null_2_svc (void *dummy __attribute__ ((unused)),
		   struct svc_req *rqstp)
{
  static char *result;
  static char rval = 0;
  struct sockaddr_in *rqhost;

  if (debug_flag)
    {
      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_null() [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));
    }

  if (!is_valid_host (rqstp, NULL, NULL))
    return NULL;

  result = &rval;
  return (void *) &result;
}

bool_t *
ypproc_domain_2_svc (domainname *name, struct svc_req *rqstp)
{
  static bool_t result;
  struct sockaddr_in *rqhost;

  if (debug_flag)
    {
      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_domain(\"%s\") [From: %s:%d]\n",
	      *name,
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));
    }

  if (!is_valid_host (rqstp, NULL, NULL))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid source host)\n");
      result = FALSE;
    }
  else
    {
      if (is_valid_domain (*name))
	result = TRUE;
      else
	result = FALSE;
    }

  if (debug_flag)
    yp_msg ("\t-> %s.\n",
	    (result == TRUE ? "Ok" : "Not served by us"));

  return &result;
}

bool_t *
ypproc_domain_nonack_2_svc (domainname * name,
			    struct svc_req * rqstp)
{
  static bool_t result;
  struct sockaddr_in *rqhost;

  if (debug_flag)
    {
      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_domain_nonack(\"%s\") [From: %s:%d]\n",
	      *name,
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));
    }

  if (!is_valid_host (rqstp, NULL, NULL))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid source host)\n");
      return NULL;
    }
  else if (!is_valid_domain (*name))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid domain)\n");

      /* Bail out and don't return any RPC value */
      return NULL;
    }

  if (debug_flag)
    yp_msg ("\t-> OK.\n");

  result = TRUE;
  return &result;
}

/* Get the DateTimeModified value for a certain map database */
static inline unsigned long
get_dtm (const char *domain, const char *map)
{
  struct stat sbuf;
  char *buf = alloca(strlen (domain) + strlen (map) + 3);

  sprintf (buf, "%s/%s", domain, map);

  if (stat (buf, &sbuf) < 0)
    return 0;
  else
    return (unsigned long) sbuf.st_mtime;
}

/* YP function "MATCH" implementation */
ypresp_val *
ypproc_match_2_svc (ypreq_key *key, struct svc_req *rqstp)
{
  static ypresp_val result;
  struct sockaddr_in *rqhost;
  int valid;
  int mangle = 0;

  if (debug_flag)
    {
      rqhost = svc_getcaller (rqstp->rq_xprt);

      yp_msg ("ypproc_match(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\t\tdomainname = \"%s\"\n",
	      key->domain);
      yp_msg ("\t\tmapname = \"%s\"\n",
	      key->map);
      yp_msg ("\t\tkeydat = \"%.*s\"\n",
	      (int) key->key.keydat_len,
	      key->key.keydat_val);
    }

  result.val.valdat_len = 0;
  if (result.val.valdat_val)
    {
      ypdb_free (result.val.valdat_val);
      result.val.valdat_val = NULL;
    }

  if ((valid = is_valid_host (rqstp, key->map, key->domain)) < 1)
    {
      if (valid == 0)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid source host)\n");

	  result.stat = YP_YPERR;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Error: not a valid domain\n");
	  result.stat = YP_NODOM;
	}
      return &result;
    }
  else if (valid > 200)
    mangle = valid - 200;

  if (key->domain[0] == '\0' || key->map[0] == '\0' ||
      key->key.keydat_len == 0 || key->key.keydat_val[0] == '\0')
    result.stat = YP_BADARGS;
  else
    {
      datum rdat, qdat;

      DB_FILE dbp = ypdb_open (key->domain, key->map);
      if (dbp == NULL)
	result.stat = YP_NOMAP;
      else
	{
	  qdat.dsize = key->key.keydat_len;
	  qdat.dptr = key->key.keydat_val;

	  rdat = ypdb_fetch (dbp, qdat);

          if (rdat.dptr != NULL)
            {
              result.stat = YP_TRUE;
              if (mangle)
                if (mangle_field (&rdat, mangle) < 0)
                  result.stat = YP_YPERR;
              result.val.valdat_len = rdat.dsize;
              result.val.valdat_val = rdat.dptr;
            }
          else
            result.stat = YP_NOKEY;
	  ypdb_close (dbp);
	}
    }

  if (debug_flag)
    {
      if (result.stat == YP_TRUE)
	yp_msg ("\t-> Value = \"%.*s\"\n",
		(int) result.val.valdat_len,
		result.val.valdat_val);
      else
	yp_msg ("\t-> Error #%d\n", result.stat);
    }


  /*
  ** Do the jive thing if we didn't find the host in the YP map
  ** and we have enabled the magic DNS lookup stuff.
  **
  ** DNS lookups are handled in a subprocess so that the server
  ** doesn't block while waiting for requests to complete.
  */
  if (dns_flag && result.stat != YP_TRUE && strstr (key->map, "hosts"))
    {
      struct hostent *he;
      if (children < MAX_CHILDREN)
	{
	  ++children;
	  switch (fork ())
	    {
	    case 0:
#ifdef DEBUG
	      yp_msg ("ypserv has forked: pid=%i\n", getpid ());
#endif
	      ++forked;
	      /* Close all databases ! */
	      ypdb_close_all ();
	      break;
	    case -1:
	      --children;
	      yp_msg ("Cannot fork: %s\n", strerror (errno));
	      result.stat = YP_YPERR;
	      return &result;
	      break;
	    default:
	      return NULL;
	      break;
	    }
	}
      else
	{
	  yp_msg ("WARNING(match): too many running children for DNS requests!\n");
	  result.stat = YP_YPERR;
	  return &result;
	}

      key->key.keydat_val[key->key.keydat_len] = '\0';


      if (debug_flag)
	yp_msg ("Doing DNS lookup of %s\n", key->key.keydat_val);

      if (strcmp (key->map, "hosts.byname") == 0)
	he = res_gethostbyname (key->key.keydat_val);
      else if (strcmp (key->map, "hosts.byaddr") == 0)
	{
	  long a;

	  a = inet_addr (key->key.keydat_val);
	  he = res_gethostbyaddr ((const char *) &a, sizeof (long), AF_INET);
	}
      else
	he = NULL;

      if (he)
	{
	  int len, i, j;
	  char *cp;

	  if (debug_flag)
	    yp_msg ("\t-> OK (%s/%s)\n", he->h_name,
		    inet_ntoa (*(struct in_addr *) he->h_addr));

	  for (j = 0; he->h_addr_list[j]; ++j)
	    ;

	  len = 16;		/* Numerical IP address */
	  len += strlen (he->h_name) + 1;
	  for (i = 0; he->h_aliases[i]; ++i)
	    len += strlen (he->h_aliases[i]) + 1;

	  len *= j;
	  cp = malloc (len + 1);
	  if (cp == NULL)
	    {
	      yp_msg ("ERROR: could not allocate enough memory! [%s|%d]\n",
		      __FILE__, __LINE__);
	      result.stat = YP_YPERR;
	      return &result;
	    }

	  result.val.valdat_val = cp;

	  cp = stpcpy (cp, inet_ntoa (*(struct in_addr *) he->h_addr_list[0]));
	  *cp++ = '\t';
	  cp = stpcpy (cp, he->h_name);

	  for (i = 0; he->h_aliases[i]; ++i)
	    {
	      cp = stpcpy (cp, " ");
	      cp = stpcpy (cp, he->h_aliases[i]);
	    }

	  for (j = 1; he->h_addr_list[j]; ++j)
	    {
	      cp = stpcpy (cp, "\n");
	      cp = stpcpy (cp, inet_ntoa (*(struct in_addr *)
					  he->h_addr_list[j]));
	      *cp++ = '\t';
	      cp = stpcpy (cp, he->h_name);

	      for (i = 0; he->h_aliases[i]; i++)
		{
		  cp = stpcpy (cp, " ");
		  cp = stpcpy (cp, he->h_aliases[i]);
		}
	    }
	  *cp++ = '\n';
	  *cp = '\0';

	  result.val.valdat_len = strlen (result.val.valdat_val);
	  result.stat = YP_TRUE;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Not Found\n");

	  result.stat = YP_NOKEY;
	}
    }

  return &result;
}

ypresp_key_val *
ypproc_first_2_svc (ypreq_nokey *key, struct svc_req *rqstp)
{
  static ypresp_key_val result;
  struct sockaddr_in *rqhost;
  int valid;
  int mangle = 0;

  if (debug_flag)
    {
      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_first(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\tdomainname = \"%s\"\n", key->domain);
      yp_msg ("\tmapname = \"%s\"\n", key->map);
    }

  result.key.keydat_len = 0;
  if (result.key.keydat_val)
    {
      ypdb_free (result.key.keydat_val);
      result.key.keydat_val = NULL;
    }

  result.val.valdat_len = 0;
  if (result.val.valdat_val)
    {
      ypdb_free (result.val.valdat_val);
      result.val.valdat_val = NULL;
    }

  if ((valid = is_valid_host (rqstp, key->map, key->domain)) < 1)
    {
      if (valid == 0)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid source host)\n");
	  result.stat = YP_YPERR;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid domain)\n");
	  result.stat = YP_NODOM;
	}
      return &result;
    }
  else if (valid > 200)
    mangle = valid - 200;

  if (key->map[0] == '\0' || key->domain[0] == '\0')
    result.stat = YP_BADARGS;
  else
    {
      DB_FILE dbp = ypdb_open (key->domain, key->map);
      if (dbp == NULL)
        result.stat = YP_NOMAP;
      else
        {
	  datum dkey = ypdb_firstkey (dbp);

	  /* XXX Replace strncmp */
	  while (dkey.dptr != NULL && strncmp (dkey.dptr, "YP_", 3) == 0)
	    {
#if defined(HAVE_NDBM) /* This is much more faster then ypdb_nextkey, but
			  it is terrible to port to other databases */
	      dkey = dbm_nextkey (dbp);
#else
	      datum tkey = dkey;
	      dkey = ypdb_nextkey (dbp, tkey);
	      ypdb_free (tkey.dptr);
#endif
	    }

	  if (dkey.dptr != NULL)
	    {
	      datum dval = ypdb_fetch (dbp, dkey);
	      result.stat = YP_TRUE;
	      if (mangle)
		if (mangle_field (&dval, mangle) < 0)
		  result.stat = YP_YPERR;

	      result.key.keydat_len = dkey.dsize;
	      result.key.keydat_val = dkey.dptr;

	      result.val.valdat_len = dval.dsize;
	      result.val.valdat_val = dval.dptr;
	    }
	  else
	    result.stat = YP_NOKEY;
	  ypdb_close (dbp);
	}
    }

  if (debug_flag)
    {
      if (result.stat == YP_TRUE)
	yp_msg ("\t-> Key = \"%.*s\", Value = \"%.*s\"\n",
		(int) result.key.keydat_len,
		result.key.keydat_val,
		(int) result.val.valdat_len,
		result.val.valdat_val);

      else if (result.stat == YP_NOMORE)
	yp_msg ("\t-> No more entry's\n");
      else
	yp_msg ("\t-> Error #%d\n", result.stat);
    }

  return &result;
}

ypresp_key_val *
ypproc_next_2_svc (ypreq_key *key, struct svc_req *rqstp)
{
  static ypresp_key_val result;
  struct sockaddr_in *rqhost;
  int valid;
  int mangle = 0;

  if (debug_flag)
    {
      rqhost = svc_getcaller (rqstp->rq_xprt);

      yp_msg ("ypproc_next(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\tdomainname = \"%s\"\n", key->domain);
      yp_msg ("\tmapname = \"%s\"\n", key->map);
      yp_msg ("\tkeydat = \"%.*s\"\n",
	      (int) key->key.keydat_len,
	      key->key.keydat_val);
    }

  result.key.keydat_len = 0;
  if (result.key.keydat_val)
    {
      ypdb_free (result.key.keydat_val);
      result.key.keydat_val = NULL;
    }

  result.val.valdat_len = 0;
  if (result.val.valdat_val)
    {
      ypdb_free (result.val.valdat_val);
      result.val.valdat_val = NULL;
    }

  if ((valid = is_valid_host (rqstp, key->map, key->domain)) < 1)
    {
      if (valid == 0)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid source host)\n");
	  result.stat = YP_YPERR;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid domain)\n");
	  result.stat = YP_NODOM;
	}
      return &result;
    }
  else if (valid > 200)
    mangle = valid - 200;

  if (key->map[0] == '\0' || key->domain[0] == '\0')
    result.stat = YP_BADARGS;
  else if (!is_valid_domain (key->domain))
    result.stat = YP_NODOM;
  else
    {
      DB_FILE dbp = ypdb_open (key->domain, key->map);
      if (dbp == NULL)
        result.stat = YP_NOMAP;
      else
        {
          datum oldkey, dkey;

          oldkey.dsize = key->key.keydat_len;
          oldkey.dptr = strndup (key->key.keydat_val, oldkey.dsize);

          dkey = ypdb_nextkey (dbp, oldkey);
	  while (dkey.dptr != NULL && strncmp (dkey.dptr, "YP_", 3) == 0)
	    {
	      free (oldkey.dptr);
	      oldkey.dsize = dkey.dsize;
	      oldkey.dptr = strndup (dkey.dptr, dkey.dsize);
	      ypdb_free (dkey.dptr);
	      dkey = ypdb_nextkey (dbp, oldkey);
	    }

          free (oldkey.dptr);

          if (dkey.dptr == NULL)
            result.stat = YP_NOMORE;
          else
            {
              datum dval = ypdb_fetch (dbp, dkey);

              result.stat = YP_TRUE;
              if (mangle)
                if (mangle_field (&dval, mangle) < 0)
                  result.stat = YP_YPERR;

              result.key.keydat_len = dkey.dsize;
              result.key.keydat_val = dkey.dptr;

              result.val.valdat_len = dval.dsize;
              result.val.valdat_val = dval.dptr;
            }
          ypdb_close (dbp);
        }
    }

  if (debug_flag)
    {
      if (result.stat == YP_TRUE)
	yp_msg ("\t-> Key = \"%.*s\", Value = \"%.*s\"\n",
		(int) result.key.keydat_len,
		result.key.keydat_val,
		(int) result.val.valdat_len,
		result.val.valdat_val);
      else if (result.stat == YP_NOMORE)
	yp_msg ("\t-> No more entry's\n");
      else
	yp_msg ("\t-> Error #%d\n", result.stat);
    }

  return &result;
}

static inline void
print_ypmap_parms (const struct ypmap_parms *pp)
{
  yp_msg ("\t\tdomain   = \"%s\"\n", pp->domain);
  yp_msg ("\t\tmap      = \"%s\"\n", pp->map);
  yp_msg ("\t\tordernum = %u\n", pp->ordernum);
  yp_msg ("\t\tpeer     = \"%s\"\n", pp->peer);
}

ypresp_xfr *
ypproc_xfr_2_svc (ypreq_xfr *xfr, struct svc_req *rqstp)
{
  static ypresp_xfr result;
  struct sockaddr_in *rqhost;
  DB_FILE dbp;

  rqhost = svc_getcaller (rqstp->rq_xprt);

  if (debug_flag)
    {
      yp_msg ("ypproc_xfr_2_svc(): [From: %s:%d]\n\tmap_parms:\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      print_ypmap_parms (&xfr->map_parms);
      yp_msg ("\t\ttransid = %u\n", xfr->transid);
      yp_msg ("\t\tprog = %u\n", xfr->prog);
      yp_msg ("\t\tport = %u\n", xfr->port);
    }

  result.transid = xfr->transid;

  if (!is_valid_host (rqstp, NULL, NULL))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid source host)\n");
      else
	yp_msg ("ypproc_xfr: refuse to transfer map from %s",
		inet_ntoa (rqhost->sin_addr));

      result.xfrstat = YPXFR_REFUSED;
      return &result;
    }

  if (!is_valid_domain (xfr->map_parms.domain))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid domain)\n");
      else
	yp_msg ("ypproc_xfr: refuse to transfer map from %s, no valid domain",
		inet_ntoa (rqhost->sin_addr));
      result.xfrstat = YPXFR_NODOM;
      return &result;
    }

  if (xfr->map_parms.map == NULL || strchr (xfr->map_parms.map ,'/'))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (map contains \"/\"!)\n");
      else
	yp_msg ("ypproc_xfr: refuse to transfer map from %s, no valid mapname",
		inet_ntoa (rqhost->sin_addr));
      result.xfrstat = YPXFR_REFUSED;
      return &result;
    }

  if (xfr_check_port)
    {
      if(ntohs(rqhost->sin_port) >= IPPORT_RESERVED)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (no reserved port!)\n");
	  else
	    yp_msg ("ypproc_xfr: refuse to transfer %s from %s, no valid port",
		    xfr->map_parms.map, inet_ntoa (rqhost->sin_addr));

	  result.xfrstat = YPXFR_REFUSED;
	  return &result;
	}
    }

  /* If we have the map, check, if the master name is the same as in
     the ypreq_xfr struct. If we doesn't have the map, refuse. */
  dbp = ypdb_open(xfr->map_parms.domain, xfr->map_parms.map);
  if (dbp != NULL)
    {
      datum key;

      key.dsize = sizeof("YP_MASTER_NAME")-1;
      key.dptr = "YP_MASTER_NAME";
      if(ypdb_exists(dbp, key))
	{
	  datum val = ypdb_fetch (dbp, key);

	  if ((size_t)val.dsize != strlen (xfr->map_parms.peer) ||
	      strncmp (val.dptr, xfr->map_parms.peer, val.dsize) != 0)
	    {
	      if (debug_flag)
		yp_msg ("\t->Ignored (%s is not the master!)\n",
			xfr->map_parms.peer);
	      else
		yp_msg ("ypproc_xfr: refuse to transfer %s from %s, not master",
			xfr->map_parms.map, inet_ntoa (rqhost->sin_addr));

	      ypdb_close (dbp);
	      result.xfrstat = YPXFR_NODOM;
	      return &result;
	    }
	}
      else
	{
	  /* If we do not have a YP_MASTER_NAME key, we don't have a
	     master/slave NIS system */
	  if (debug_flag)
	    yp_msg ("\t->Ignored (no YP_MASTER_NAME key in local map)\n");

	  ypdb_close (dbp);
	  result.xfrstat = YPXFR_REFUSED;
	  return &result;
	}
      ypdb_close(dbp);
    }
  /* If you wish to allow the transfer of new maps, change the next
     #if 1 statement to #if 0 */
#if 1
  else
    {
      /* We doesn't have the map, refuse the transfer */
      if (debug_flag)
	yp_msg ("\t->Ignored (I don't have this map)\n");
      else
	yp_msg ("ypproc_xfr: refuse to transfer %s from %s, map doesn't exist",
		xfr->map_parms.map, inet_ntoa (rqhost->sin_addr));

      result.xfrstat = YPXFR_REFUSED;
      return &result;
    }
#endif

  ++children;
  switch (fork ())
    {
    case 0:
      {
	char *ypxfr_command = alloca(strlen (YPBINDIR) + 8);
	char g[30], t[30], p[30];
	int i;

	umask(0);
	i = open("/dev/null", O_RDWR);
	dup(i);
	dup(i);

	sprintf (ypxfr_command, "%s/ypxfr", YPBINDIR);
	sprintf (t, "%u", xfr->transid);
	sprintf (g, "%u", xfr->prog);
	sprintf (p, "%u", xfr->port);
	if (debug_flag)
	  execl (ypxfr_command, "ypxfr", "--debug", "-d",
		 xfr->map_parms.domain, "-h", xfr->map_parms.peer, "-C", t, g,
		 inet_ntoa (rqhost->sin_addr), p, xfr->map_parms.map, NULL);
	else
	  execl (ypxfr_command, "ypxfr", "-d", xfr->map_parms.domain, "-h",
		 xfr->map_parms.peer, "-C", t, g,
		 inet_ntoa (rqhost->sin_addr), p, xfr->map_parms.map, NULL);

	yp_msg ("ypxfr execl(): %s", strerror (errno));
	exit (0);
      }
    case -1:
      --children;
      yp_msg ("Cannot fork: %s", strerror (errno));
      result.xfrstat = YPXFR_XFRERR;
    default:
      result.xfrstat = YPXFR_SUCC;
      break;
    }

  return &result;
}


/* Close all cached filehandles */
void *
ypproc_clear_2_svc (void *dummy __attribute__ ((unused)),
		    struct svc_req *rqstp)
{
  static int *result;
  static int rval;

  if (debug_flag)
    {
      struct sockaddr_in *rqhost;

      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_clear_2_svc() [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));
    }

  if (!is_valid_host (rqstp, NULL, NULL))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid source host)\n");

      return NULL;
    }

  rval = ypdb_close_all ();
  result = &rval;
  return (void *) &result;
}

/* We need the struct for giving ypall_encode the DB_FILE
   handle and the mangle field. */

typedef struct ypall_data
{
  DB_FILE dbm;
  int field;
} *ypall_data_t;

static int
ypall_close (void *data)
{
  if (debug_flag && data == NULL)
    {
      yp_msg ("ypall_close() called with NULL pointer.\n");
      return 0;
    }

  ypdb_close (((ypall_data_t) data)->dbm);
  ypdb_free (data);
  return 0;
}

static int
ypall_encode (ypresp_key_val *val, void *data)
{
  datum dkey;

#if defined(HAVE_NDBM)

  dkey = dbm_nextkey (((ypall_data_t) data)->dbm);
  /* XXX Replace strncmp */
  while (dkey.dptr != NULL && strncmp (dkey.dptr, "YP_", 3) == 0)
    dkey = dbm_nextkey (((ypall_data_t) data)->dbm);

#else
  datum oldkey;

  oldkey.dsize = val->key.keydat_len;
  oldkey.dptr = strndup (val->key.keydat_val, oldkey.dsize);

  dkey = ypdb_nextkey (((ypall_data_t) data)->dbm, oldkey);

  /* XXX Replace strncmp */
  while (dkey.dptr != NULL && strncmp (dkey.dptr, "YP_", 3) == 0)
    {
      free (oldkey.dptr);
      oldkey.dsize = dkey.dsize;
      oldkey.dptr = strndup (dkey.dptr, dkey.dsize);
      ypdb_free (dkey.dptr);

      dkey = ypdb_nextkey (((ypall_data_t) data)->dbm, oldkey);
    }

  free (oldkey.dptr);
#endif

  if (dkey.dptr == NULL)
    val->stat = YP_NOMORE;
  else
    {
      datum dval = ypdb_fetch (((ypall_data_t) data)->dbm, dkey);

      val->stat = YP_TRUE;
      if (((ypall_data_t) data)->field)
        if (mangle_field (&dval, ((ypall_data_t) data)->field) < 0)
          val->stat = YP_YPERR;

      val->key.keydat_val = dkey.dptr;
      val->key.keydat_len = dkey.dsize;

      val->val.valdat_val = dval.dptr;
      val->val.valdat_len = dval.dsize;
    }
  return val->stat;
}

extern xdr_ypall_cb_t xdr_ypall_cb;

ypresp_all *
ypproc_all_2_svc (ypreq_nokey *nokey, struct svc_req *rqstp)
{
  static ypresp_all result;
  int valid;
  int mangle = 0;

  if (debug_flag)
    {
      struct sockaddr_in *rqhost;

      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_all_2_svc(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\t\tdomain = \"%s\"\n", nokey->domain);
      yp_msg ("\t\tmap = \"%s\"\n", nokey->map);
    }


  if ((valid = is_valid_host (rqstp, nokey->map, nokey->domain)) < 1)
    {
      if (valid == 0)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid source host)\n");
	  result.ypresp_all_u.val.stat = YP_YPERR;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid domain)\n");
	  result.ypresp_all_u.val.stat = YP_NODOM;
	}
      return &result;
    }
  if (valid > 200)
    mangle = valid - 200;

  if (children >= MAX_CHILDREN)
    {
      int wait = 0;

      while (wait < 5)
	{
	  sleep (1);

	  if (children < MAX_CHILDREN)
	    break;
	  ++wait;
	}
    }

  if (children < MAX_CHILDREN)
    {
      ++children;
      switch (fork ())
	{
	case 0:
	  ++forked;
#ifdef DEBUG
	  yp_msg ("ypserv has forked for ypproc_all(): pid=%i\n", getpid ());
	  if (!forked)
	    abort ();
#endif
	  /* Close all databases ! */
	  ypdb_close_all ();
	  break;
	case -1:
	  --children;
	  yp_msg ("WARNING(ypproc_all_2_svc): cannot fork: %s\n",
		  strerror (errno));
	  result.ypresp_all_u.val.stat = YP_YPERR;
	  return &result;
	default:
	  return NULL;
	  break;
	}
    }
  else
    {
      yp_msg ("WARNING(ypproc_all_2_svc): too many running children!\n");
      result.ypresp_all_u.val.stat = YP_YPERR;
      return &result;
    }

  xdr_ypall_cb.u.encode = NULL;
  xdr_ypall_cb.u.close = NULL;
  xdr_ypall_cb.data = NULL;

  result.more = TRUE;

  if (nokey->map[0] == '\0' || nokey->domain[0] == '\0')
    result.ypresp_all_u.val.stat = YP_BADARGS;
  else
    {
      ypall_data_t data;

      if ((data = malloc (sizeof (struct ypall_data))) == NULL)
	{
	  yp_msg ("ERROR: could not allocate enough memory! [%s|%d]\n",
		  __FILE__, __LINE__);
	  result.ypresp_all_u.val.stat = YP_YPERR;
	  return &result;
	}
      data->dbm = ypdb_open (nokey->domain, nokey->map);

      if (data->dbm == NULL)
	result.ypresp_all_u.val.stat = YP_NOMAP;
      else
	{
          datum dkey = ypdb_firstkey (data->dbm);

	  /* XXX Replace strncmp */
	  while (dkey.dptr != NULL && strncmp (dkey.dptr, "YP_", 3) == 0)
	    {
	      datum tkey = dkey;
	      dkey = ypdb_nextkey (data->dbm, tkey);
	      ypdb_free (tkey.dptr);
	    }

          if (dkey.dptr != NULL)
            {
              datum dval = ypdb_fetch (data->dbm, dkey);

              result.ypresp_all_u.val.stat = YP_TRUE;

              if (mangle)
                if (mangle_field (&dval, mangle) < 0)
                  result.ypresp_all_u.val.stat = YP_YPERR;

              result.ypresp_all_u.val.key.keydat_len = dkey.dsize;
              result.ypresp_all_u.val.key.keydat_val = dkey.dptr;

              result.ypresp_all_u.val.val.valdat_len = dval.dsize;
              result.ypresp_all_u.val.val.valdat_val = dval.dptr;

              data->field = mangle;

              xdr_ypall_cb.u.encode = ypall_encode;
              xdr_ypall_cb.u.close = ypall_close;
              xdr_ypall_cb.data = (void *) data;

              if (debug_flag)
                yp_msg ("\t -> First value returned.\n");

              if (result.ypresp_all_u.val.stat == YP_TRUE)
		return &result;
            }
	  else
	    result.ypresp_all_u.val.stat = YP_NOMORE;

	  ypdb_close (data->dbm);
	}
      free (data);
    }

  if (debug_flag)
    yp_msg ("\t -> Exit from ypproc_all without sending data.\n");

  return &result;
}

ypresp_master *
ypproc_master_2_svc (ypreq_nokey * nokey, struct svc_req * rqstp)
{
  static ypresp_master result;
  char hostbuf[MAXHOSTNAMELEN + 2];
  int valid;

  if (debug_flag)
    {
      struct sockaddr_in *rqhost;

      rqhost = svc_getcaller (rqstp->rq_xprt);
      yp_msg ("ypproc_master_2_svc(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\t\tdomain = \"%s\"\n", nokey->domain);
      yp_msg ("\t\tmap = \"%s\"\n", nokey->map);
    }

  if (result.peer)
    {
      free (result.peer);
      result.peer = NULL;
    }

  if ((valid = is_valid_host (rqstp, NULL, NULL)) < 1)
    {
      if (valid == 0)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid source host)\n");
	  result.stat = YP_YPERR;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a domain)\n");
	  result.stat = YP_NODOM;
	}
      result.peer = strdup ("");
      return &result;
    }

  if (nokey->map[0] == '\0')
    result.stat = YP_BADARGS;
  else
    {
      DB_FILE dbp = ypdb_open (nokey->domain, nokey->map);
      if (dbp == NULL)
	result.stat = YP_NOMAP;
      else
	{
	  datum key, val;

	  key.dsize = sizeof ("YP_MASTER_NAME") - 1;
	  key.dptr = "YP_MASTER_NAME";

	  val = ypdb_fetch (dbp, key);
	  if (val.dptr == NULL)
	    {
	      /* No YP_MASTER_NAME record in map? Assume we are Master */
	      if(gethostname (hostbuf, sizeof (hostbuf) - 1) != 0)
		{
		  perror ("gethostname");
		  exit (1);
		}
#if USE_FQDN
	      else
		{
		  struct hostent *hp;

		  if (!(hp = res_gethostbyname (hostbuf)))
		    perror ("gethostbyname()");
		  else
		    {
		      strncpy (hostbuf, hp->h_name, sizeof (hostbuf) - 1);
		      hostbuf[sizeof (hostbuf) - 1] = '\0';
	}
    }
#endif

	      if ((result.peer = strdup (hostbuf)) == NULL)
		result.stat = YP_YPERR;
	      else
		result.stat = YP_TRUE;
	    }
	  else
	    {
	      int i;

	      /* put the eof string mark at the end of the string */
	      for (i = 0; i < val.dsize; ++i)
		hostbuf[i] = val.dptr[i];
	      hostbuf[val.dsize] = '\0';
	      ypdb_free (val.dptr);

	      if ((result.peer = strdup (hostbuf)) == NULL)
		result.stat = YP_YPERR;
	      else
		result.stat = YP_TRUE;
	    }

	  ypdb_close (dbp);
	}
    }

  if (result.peer == NULL)
    result.peer = strdup ("");

  if (debug_flag)
    yp_msg ("\t-> Peer = \"%s\"\n", result.peer);

  return &result;
}

ypresp_order *
ypproc_order_2_svc (ypreq_nokey * nokey, struct svc_req * rqstp)
{
  static ypresp_order result;
  int valid;

  if (debug_flag)
    {
      struct sockaddr_in *rqhost;

      rqhost = svc_getcaller (rqstp->rq_xprt);

      yp_msg ("ypproc_order_2_svc(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\t\tdomain = \"%s\"\n", nokey->domain);
      yp_msg ("\t\tmap = \"%s\"\n", nokey->map);
    }

  if ((valid = is_valid_host (rqstp, nokey->map, nokey->domain)) < 1)
    {
      if (valid == 0)
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid source host)\n");
	  result.stat = YP_YPERR;
	}
      else
	{
	  if (debug_flag)
	    yp_msg ("\t-> Ignored (not a valid domain)\n");
	  result.stat = YP_NODOM;
	}
      return &result;
    }

  result.ordernum = 0;

  if (nokey->map[0] == '\0')
    result.stat = YP_BADARGS;
  else
    {
      DB_FILE dbp = ypdb_open (nokey->domain, nokey->map);
      if (dbp == NULL)
	result.stat = YP_NOMAP;
      else
	{
	  datum key, val;

	  key.dsize = sizeof ("YP_LAST_MODIFIED") - 1;
	  key.dptr = "YP_LAST_MODIFIED";

	  val = ypdb_fetch (dbp, key);
	  if (val.dptr == NULL)
	    {
	      /* No YP_LAST_MODIFIED record in map? Use DTM timestamp.. */
	      result.ordernum = get_dtm (nokey->domain, nokey->map);
	    }
	  else
	    {
	      char *buf = alloca(val.dsize + 2);

	      memcpy (buf, val.dptr, val.dsize);
	      buf[val.dsize + 1] = '\0';
	      result.ordernum = atoi (buf);
	      ypdb_free (val.dptr);
	    }

	  result.stat = YP_TRUE;
	  ypdb_close (dbp);
	}
    }

  if (debug_flag)
    yp_msg ("-> Order # %u\n", result.ordernum);

  return &result;
}

static void
free_maplist (ypmaplist * mlp)
{
  ypmaplist *next;

  while (mlp != NULL)
    {
      next = mlp->next;
      free (mlp->map);
      free (mlp);
      mlp = next;
    }
}

static int
add_maplist (ypmaplist ** mlhp, char *map)
{
  ypmaplist *mlp;
#if defined(HAVE_NDBM)
#if defined(sun) || defined(__sun__)
  int len = strlen (map);

  /* We have all maps twice: with .dir and with .pag. Ignore
     .pag */
  if (len > 3 && map[len - 4] == '.' && map[len - 3] == 'p' &&
      map[len - 2] == 'a' && map[len - 1] == 'g')
    return 0;

  if (len > 3 && map[len - 4] == '.' && map[len - 3] == 'd' &&
      map[len - 2] == 'i' && map[len - 1] == 'r')
    map[len - 4] = '\0';
#else
  int len = strlen (map);

  if (len > 2 && map[len - 3] == '.' && map[len - 2] == 'd' &&
      map[len - 1] == 'b')
    map[len - 3] = '\0';
#endif
#endif

  if ((mlp = malloc (sizeof (*mlp))) == NULL)
    return -1;

  if ((mlp->map = strdup (map)) == NULL)
    {
      free (mlp);
      return -1;
    }

  mlp->next = *mlhp;
  *mlhp = mlp;

  return 0;
}


ypresp_maplist *
ypproc_maplist_2_svc (domainname * name, struct svc_req * rqstp)
{
  static ypresp_maplist result;

  if (debug_flag)
    {
      struct sockaddr_in *rqhost;

      rqhost = svc_getcaller (rqstp->rq_xprt);

      yp_msg ("ypproc_maplist_2_svc(): [From: %s:%d]\n",
	      inet_ntoa (rqhost->sin_addr),
	      ntohs (rqhost->sin_port));

      yp_msg ("\t\tdomain = \"%s\"\n", *name);
    }

  if (!is_valid_host (rqstp, NULL, NULL))
    {
      if (debug_flag)
	yp_msg ("\t-> Ignored (not a valid source host)\n");

      return NULL;
    }

  if (result.maps)
    free_maplist (result.maps);

  result.maps = NULL;

  if ((*name)[0] == '\0')
    result.stat = YP_BADARGS;
  else if (!is_valid_domain (*name))
    result.stat = YP_NODOM;
  else
    {
      DIR *dp;

      /* open domain directory */
      dp = opendir (*name);
      if (dp == NULL)
	{
	  if (debug_flag)
	    {
	      yp_msg ("%s: opendir: %s", progname, strerror (errno));
	    }

	  result.stat = YP_BADDB;
	}
      else
	{
	  struct dirent *dep;

	  while ((dep = readdir (dp)) != NULL)
	    {
	      /* ignore files . and .. */
	      if (strcmp (".", dep->d_name) == 0 ||
		  strcmp ("..", dep->d_name) == 0)
		continue;
	      if (add_maplist (&result.maps, dep->d_name) < 0)
		{
		  result.stat = YP_YPERR;
		  break;
		}
	    }
	  closedir (dp);
	  result.stat = YP_TRUE;
	}
    }

  if (debug_flag)
    {
      if (result.stat == YP_TRUE)
	{
	  ypmaplist *p;

	  p = result.maps;
	  yp_msg ("-> ");
	  while (p)
	    {
	      yp_msg ("%s", p->map);
	      if (p->next)
		{
		  yp_msg (", ");
		}
	      p = p->next;
	    }
	  putc ('\n', stderr);
	}
      else
	yp_msg ("\t-> Error #%d\n", result.stat);
    }

  return &result;
}
