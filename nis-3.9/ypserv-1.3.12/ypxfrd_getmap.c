/*
** Copyright (c) 1996, 1997, 1998, 1999, 2000 Thorsten Kukuk
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
** Author: Thorsten Kukuk <kukuk@suse.de>
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "system.h"

#include <sys/types.h>
#include <time.h>
#include "ypxfrd.h"
#include "yp.h"
#include <rpc/rpc.h>
#if defined (HAVE_RPC_CLNT_SOC_H)
#include <rpc/clnt_soc.h>
#endif
#include <sys/uio.h>
#if defined (HAVE_FCNTL_H)
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <sys/stat.h>
#include <errno.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>

#include "yp_msg.h"

#if !defined (HAVE_GETRPCPORT)
#include <compat/getrpcport.c>
#endif

#if defined (__NetBSD__) || (defined(__GLIBC__) && (__GLIBC__ == 2 && __GLIBC_MINOR__ == 0))
/* <rpc/rpc.h> is missing the prototype */
int getrpcport (char *host, int prognum, int versnum, int proto);
#endif

int ypxfrd_transfer (char *host, char *map, char *domain, char *tmpname);

static int file = 0;
static char *err;
static char ebuf[128];

static bool_t
xdr_ypxfr_xfr (XDR * xdrs, xfr * objp)
{
  while (1)
    {
      if (!xdr_xfr (xdrs, objp))
	return (FALSE);
      if (objp->ok == TRUE)
	{
	  if (write (file, objp->xfr_u.xfrblock_buf.xfrblock_buf_val,
		     objp->xfr_u.xfrblock_buf.xfrblock_buf_len) == -1)
	    {
#if defined(HAVE_SNPRINTF)
	      snprintf(ebuf, sizeof (ebuf), "write failed: %s",
		       strerror(errno));
#else
	      sprintf(ebuf, "write failed: %s",
                      strerror(errno));
#endif
	      err = ebuf;
	      return FALSE;
	    }
	}
      xdr_free ((xdrproc_t) xdr_xfr, (char *) objp);
      if (objp->ok == FALSE)
	{
	  switch (objp->xfr_u.xfrstat)
	    {
	    case XFR_DONE:
	      return TRUE;
	      break;
	    case XFR_DENIED:
	      err = "access to map denied by rpc.ypxfrd";
	      return FALSE;
	      break;
	    case XFR_NOFILE:
	      err = "reqested map does not exist";
	      return FALSE;
	      break;
	    case XFR_ACCESS:
	      err = "rpc.ypxfrd couldn't access the map";
	      return FALSE;
	      break;
	    case XFR_BADDB:
	      err = "file is not a database";
	      return FALSE;
	      break;
	    case XFR_READ_OK:
	      err = "block read successfully";
	      return TRUE;
	      break;
	    case XFR_READ_ERR:
	      err = "got read error from rpc.ypxfrd";
	      return FALSE;
	      break;
	    case XFR_DB_ENDIAN_MISMATCH:
	      err = "rpc.ypxfrd databases have the wrong endian";
	      return FALSE;
	      break;
	    case XFR_DB_TYPE_MISMATCH:
	      err = "rpc.ypxfrd doesn't support the needed database type";
	      return FALSE;
	      break;
	    default:
	      err = "got unknown status from rpc.ypxfrd";
	      return FALSE;
	      break;
	    }
	}
    }
}

int
ypxfrd_transfer (char *host, char *map, char *domain, char *tmpname)
{
  CLIENT *clnt;
  struct ypxfr_mapname req;
  struct xfr resp;
  struct timeval timeout =
  {2, 0};

  err = NULL;

  if (debug_flag)
    yp_msg ("Trying ypxfrd ...");

  if (!getrpcport (host, YPXFRD_FREEBSD_PROG, YPXFRD_FREEBSD_VERS,
		   IPPROTO_TCP))
    {
      if (debug_flag)
	yp_msg (" not running\n");
      return 1;
    }

  req.xfrmap = map;
  req.xfrdomain = domain;
  req.xfrmap_filename = map;
#if defined(HAVE_LIBGDBM)
#if SIZEOF_LONG == 8
  req.xfr_db_type = XFR_DB_GNU_GDBM64;
#else
  req.xfr_db_type = XFR_DB_GNU_GDBM;
#endif
#if defined(WORDS_BIGENDIAN)
  req.xfr_byte_order = XFR_ENDIAN_BIG;
#else
  req.xfr_byte_order = XFR_ENDIAN_LITTLE;
#endif
#elif defined (HAVE_NDBM)
#if defined(__sun__) || defined (sun)
  req.xfr_db_type = XFR_DB_NDBM;
#if defined(WORDS_BIGENDIAN)
  req.xfr_byte_order = XFR_ENDIAN_BIG;
#else
  req.xfr_byte_order = XFR_ENDIAN_LITTLE;
#endif
#else
  req.xfr_db_type = XFR_DB_BSD_NDBM;
  req.xfr_byte_order = XFR_ENDIAN_ANY;
#endif
#endif
  memset (&resp, 0, sizeof (resp));

  if ((clnt = clnt_create (host, YPXFRD_FREEBSD_PROG,
			   YPXFRD_FREEBSD_VERS, "tcp")) == NULL)
    goto error;

  if ((file = open (tmpname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
    {
      clnt_destroy (clnt);
      yp_msg ("couldn't open %s: %s", tmpname, strerror (errno));
      goto error;
    }

  if (clnt_call (clnt, YPXFRD_GETMAP, (xdrproc_t) xdr_ypxfr_mapname,
		 (caddr_t) &req, (xdrproc_t) xdr_ypxfr_xfr,
		 (caddr_t) &resp, timeout) != RPC_SUCCESS)
    {
      yp_msg ("%s", err ? err :
	      clnt_sperror (clnt, "call to rpc.ypxfrd failed"));
      unlink (tmpname);
      clnt_destroy (clnt);
      close (file);
      goto error;
    }

  clnt_destroy (clnt);
  close (file);

  if (debug_flag)
    yp_msg (" success\n");

  return 0;

error:
  if (debug_flag)
    yp_msg (" (failed, fallback to enumeration)\n");
  return 1;
}
