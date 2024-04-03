/* ypserv_v1.c - YP server routines for V1

   Copyright (c) 1998, 1999 Thorsten Kukuk

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

   Author: Thorsten Kukuk <kukuk@suse.de>
*/


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "system.h"

#include <string.h>

#include "yp.h"
#include "yp_msg.h"

void *
ypoldproc_null_1_svc (void *argp, struct svc_req *rqstp)
{
  return ypproc_null_2_svc (argp, rqstp);
}

bool_t *
ypoldproc_domain_1_svc (domainname *argp, struct svc_req *rqstp)
{
  return ypproc_domain_2_svc (argp, rqstp);
}

bool_t *
ypoldproc_domain_nonack_1_svc (domainname *argp, struct svc_req *rqstp)
{
  return ypproc_domain_nonack_2_svc (argp, rqstp);
}

ypresponse *
ypoldproc_match_1_svc (yprequest *argp, struct svc_req *rqstp)
{
  static ypresponse result;
  ypresp_val *v2_result;

  result.yp_resptype = YPRESP_VAL;
  result.ypresponse_u.yp_resp_valtype.val.valdat_val = "";
  result.ypresponse_u.yp_resp_valtype.val.valdat_len = 0;

  if (argp->yp_reqtype != YPREQ_KEY)
    {
      result.ypresponse_u.yp_resp_valtype.stat = YP_BADARGS;
      return &result;
    }

  v2_result = ypproc_match_2_svc (&argp->yprequest_u.yp_req_keytype, rqstp);
  if (v2_result == NULL)
    return NULL;

  memcpy (&result.ypresponse_u.yp_resp_valtype, v2_result,
	  sizeof (ypresp_val));

  return &result;
}

ypresponse *
ypoldproc_first_1_svc (yprequest * argp, struct svc_req * rqstp)
{
  static ypresponse result;
  ypresp_key_val *v2_result;

  result.yp_resptype = YPRESP_KEY_VAL;
  result.ypresponse_u.yp_resp_key_valtype.val.valdat_val =
    result.ypresponse_u.yp_resp_key_valtype.key.keydat_val = "";
  result.ypresponse_u.yp_resp_key_valtype.val.valdat_len =
    result.ypresponse_u.yp_resp_key_valtype.key.keydat_len = 0;

  if (argp->yp_reqtype != YPREQ_NOKEY)
    {
      result.ypresponse_u.yp_resp_key_valtype.stat = YP_BADARGS;
      return &result;
    }

  v2_result = ypproc_first_2_svc (&argp->yprequest_u.yp_req_nokeytype,
				  rqstp);
  if (v2_result == NULL)
    return NULL;

  memcpy (&result.ypresponse_u.yp_resp_key_valtype, v2_result,
	  sizeof (ypresp_key_val));

  return &result;
}

ypresponse *
ypoldproc_next_1_svc (yprequest * argp, struct svc_req * rqstp)
{
  static ypresponse result;
  ypresp_key_val *v2_result;

  result.yp_resptype = YPRESP_KEY_VAL;
  result.ypresponse_u.yp_resp_key_valtype.val.valdat_val =
    result.ypresponse_u.yp_resp_key_valtype.key.keydat_val = "";
  result.ypresponse_u.yp_resp_key_valtype.val.valdat_len =
    result.ypresponse_u.yp_resp_key_valtype.key.keydat_len = 0;

  if (argp->yp_reqtype != YPREQ_KEY)
    {
      result.ypresponse_u.yp_resp_key_valtype.stat = YP_BADARGS;
      return &result;
    }

  v2_result = ypproc_next_2_svc (&argp->yprequest_u.yp_req_keytype, rqstp);
  if (v2_result == NULL)
    return NULL;

  memcpy (&result.ypresponse_u.yp_resp_key_valtype, v2_result,
	  sizeof (ypresp_key_val));

  return &result;
}

ypresponse *
ypoldproc_poll_1_svc (yprequest * argp, struct svc_req * rqstp)
{
  static ypresponse result;
  ypresp_master *v2_result1;
  ypresp_order *v2_result2;

  result.yp_resptype = YPRESP_MAP_PARMS;
  result.ypresponse_u.yp_resp_map_parmstype.domain =
    argp->yprequest_u.yp_req_nokeytype.domain;
  result.ypresponse_u.yp_resp_map_parmstype.map =
    argp->yprequest_u.yp_req_nokeytype.map;
  /*
   * Hmm... there is no 'status' value in the
   * yp_resp_map_parmstype structure, so I have to
   * guess at what to do to indicate a failure.
   * I hope this is right.
   */
  result.ypresponse_u.yp_resp_map_parmstype.ordernum = 0;
  result.ypresponse_u.yp_resp_map_parmstype.peer = "";

  if (argp->yp_reqtype != YPREQ_MAP_PARMS)
    return &result;

  v2_result1 = ypproc_master_2_svc (&argp->yprequest_u.yp_req_nokeytype,
				    rqstp);
  if (v2_result1 == NULL)
    return (NULL);
  if (v2_result1->stat != YP_TRUE)
    return &result;

  v2_result2 = ypproc_order_2_svc (&argp->yprequest_u.yp_req_nokeytype,
				   rqstp);
  if (v2_result2 == NULL)
    return NULL;

  if (v2_result2->stat != YP_TRUE)
    return &result;

  result.ypresponse_u.yp_resp_map_parmstype.peer = v2_result1->peer;
  result.ypresponse_u.yp_resp_map_parmstype.ordernum = v2_result2->ordernum;

  return &result;
}

ypresponse *
ypoldproc_push_1_svc (yprequest *argp __attribute__ ((unused)),
		      struct svc_req *rqstp __attribute__ ((unused)))
{
  static ypresponse result;

  /*
   * Not implemented.
   */

  return &result;
}

ypresponse *
ypoldproc_pull_1_svc (yprequest *argp __attribute__ ((unused)),
		      struct svc_req *rqstp __attribute__ ((unused)))
{
  static ypresponse result;

  /*
   * Not implemented.
   */

  return &result;
}

ypresponse *
ypoldproc_get_1_svc (yprequest *argp __attribute__ ((unused)),
		     struct svc_req *rqstp __attribute__ ((unused)))
{
  static ypresponse result;

  /*
   * Not implemented.
   */

  return &result;
}
