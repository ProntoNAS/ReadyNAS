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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include "system.h"

#include <string.h>
#include <memory.h>

#include "yp.h"
#include "ypxfr.h"

static struct timeval TIMEOUT = {25, 0};

ypresp_val *ypproc_match_2(ypreq_key *argp, CLIENT *clnt)
{
  static ypresp_val res;

  memset((char *)&res, 0, sizeof(res));
  if (clnt_call(clnt, YPPROC_MATCH, (xdrproc_t) xdr_ypreq_key,
		(void *)argp, (xdrproc_t) xdr_ypresp_val,
		(void *)&res, TIMEOUT) != RPC_SUCCESS)
    return NULL;

  return &res;
}

ypresp_key_val *ypproc_first_2( ypreq_nokey *argp, CLIENT *clnt)
{
  static ypresp_key_val res;

  memset((char*)&res, 0, sizeof(res));
  if (clnt_call(clnt, YPPROC_FIRST, (xdrproc_t) xdr_ypreq_key,
		(void *) argp, (xdrproc_t) xdr_ypresp_key_val,
		(void *) &res, TIMEOUT) != RPC_SUCCESS)
    return NULL;

  return &res;
}

void *ypproc_clear_2( void *argp, CLIENT *clnt)
{
  static char res;

  memset((char*)&res, 0, sizeof(res));
  if (clnt_call(clnt, YPPROC_CLEAR, (xdrproc_t) xdr_void, argp,
		(xdrproc_t) xdr_void, &res, TIMEOUT) != RPC_SUCCESS)
    return NULL;

  return (void *)&res;
}

ypresp_all *ypproc_all_2( ypreq_nokey *argp, CLIENT *clnt)
{
  static ypresp_all res;

  memset(&res, 0, sizeof(res));
  if (clnt_call(clnt, YPPROC_ALL, (xdrproc_t) xdr_ypreq_nokey,
		(void *) argp, (xdrproc_t) ypxfr_xdr_ypresp_all,
		(void *) &res, TIMEOUT) != RPC_SUCCESS)
    return NULL;

  return &res;
}

ypresp_master *ypproc_master_2( ypreq_nokey *argp, CLIENT *clnt)
{
  static ypresp_master res;

  memset(&res, 0, sizeof(res));
  if (clnt_call(clnt, YPPROC_MASTER, (xdrproc_t) xdr_ypreq_nokey,
		(void *) argp, (xdrproc_t) xdr_ypresp_master,
		(void *) &res, TIMEOUT) != RPC_SUCCESS)
    return NULL;

  return &res;
}

ypresp_order *ypproc_order_2(ypreq_nokey *argp, CLIENT *clnt)
{
  static ypresp_order clnt_res;

  memset((char *)&clnt_res, 0, sizeof(clnt_res));
  if (clnt_call(clnt, YPPROC_ORDER, (xdrproc_t) xdr_ypreq_nokey,
		(void *) argp, (xdrproc_t) xdr_ypresp_order,
		(void *) &clnt_res, TIMEOUT) != RPC_SUCCESS)
    return NULL;

  return &clnt_res;
}
