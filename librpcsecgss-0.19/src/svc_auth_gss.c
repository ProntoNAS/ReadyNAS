/*
  svc_auth_gss.c

  Copyright (c) 2000 The Regents of the University of Michigan.
  All rights reserved.

  Copyright (c) 2000 Dug Song <dugsong@UMICH.EDU>.
  All rights reserved, all wrongs reversed.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the name of the University nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include <gssapi/gssapi.h>

extern SVCAUTH svc_auth_none;

/*
 * from mit-krb5-1.2.1 mechglue/mglueP.h:
 * Array of context IDs typed by mechanism OID
 */
typedef struct gss_union_ctx_id_t {
  gss_OID     mech_type;
  gss_ctx_id_t    internal_ctx_id;
} gss_union_ctx_id_desc, *gss_union_ctx_id_t;



static bool_t	svcauth_gss_destroy();
static bool_t   svcauth_gss_wrap();
static bool_t   svcauth_gss_unwrap();

struct svc_auth_ops svc_auth_gss_ops = {
	svcauth_gss_wrap,
	svcauth_gss_unwrap,
	svcauth_gss_destroy
};

struct svc_rpc_gss_data {
	bool_t			established;	/* context established */
	gss_ctx_id_t		ctx;		/* context id */
	struct rpc_gss_sec	sec;		/* security triple */
	gss_buffer_desc		cname;		/* displayable client name */
	gss_buffer_desc		sname;		/* displayable server name */
	u_int			seq;		/* sequence number */
	u_int			win;		/* sequence window */
	u_int			seqlast;	/* last sequence number */
	u_int32_t		seqmask;	/* bitmask of seqnums */
	gss_name_t		client_name;	/* internal fmt client name */
	gss_name_t		server_name;	/* internal fmt server name */
	gss_cred_id_t		server_cred;	/* cred used for this ctx */
	gss_buffer_desc		checksum;	/* so we can free it */
};

#define SVCAUTH_PRIVATE(auth) \
	((struct svc_rpc_gss_data *)(auth)->svc_ah_private)

/*
 * This structure keeps track of information on all the server
 * identities we can accept connections as.  A new structure
 * is added for each unique invocation of svcauth_gss_set_svc_name()
 */
typedef struct svcauth_server_info {
	struct svcauth_server_info *s_next;	/* Chain pointer */
	gss_OID		s_mech;		/* mechanism for this server name */
	gss_buffer_desc	s_disp_name;	/* displayable service name */
	gss_name_t	s_int_name;	/* internal format service name */
	gss_cred_id_t	s_cred;		/* credentials for this name */
	gss_OID_set	s_cred_mechs;	/* mechs s_cred is valid for */
} svcauth_server_info;

/* Global list of possible server names */
static svcauth_server_info *_svcauth_server_list = NULL;

bool_t
svcauth_gss_set_svc_name(char *principal, char *mechanism, u_int req_time,
			 u_int program, u_int version)
{
	OM_uint32	maj_stat, min_stat;
	gss_buffer_desc	namebuf;
	gss_name_t	internal_name;
	svcauth_server_info *si = NULL;	/* Server Info */
	svcauth_server_info *tempsi;
	bool_t		retval = FALSE;
	int		names_equal = 0;
	gss_OID_set	req_mechs;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_set_svc_name()"));

	namebuf.value = principal;
	namebuf.length = strlen(principal);

	/* Get internal name */
	maj_stat = gss_import_name(&min_stat, &namebuf,
				   (gss_OID)GSS_C_NT_HOSTBASED_SERVICE,
				   &internal_name);

	if (maj_stat != GSS_S_COMPLETE) {
		authgss_log_err("svcauth_gss_set_svc_name: "
				"error importing name '%s'\n", principal);
		authgss_log_status("gss_import_name", maj_stat, min_stat);
		return (retval);
	}

	/* Check if we already have this name */
	for (si = _svcauth_server_list; si; si = si->s_next) {
		maj_stat = gss_compare_name(&min_stat, internal_name,
					    si->s_int_name, &names_equal);
		if (maj_stat != GSS_S_COMPLETE) {
			authgss_log_status("svcauth_gss_set_svc_name: "
					   "gss_compare_name",
					   maj_stat, min_stat);
			gss_release_name(&min_stat, &internal_name);
			return (retval);
		}
		if (names_equal) {
			authgss_log_err("svcauth_gss_set_svc_name: "
					"'%s' already registered as '%.*s'\n",
					principal, si->s_disp_name.length,
					si->s_disp_name.value);
			gss_release_name(&min_stat, &internal_name);
			return (retval);
		}
	}

	/* Allocate a new server_info structure */
	if ((si = calloc(sizeof(*si), 1)) == NULL) {
		authgss_log_err("svcauth_gss_set_svc_name: "
				"Could not allocate server info structure\n");
		gss_release_name(&min_stat, &internal_name);
		return (retval);
	}

	/* Instead of duplicating and then releasing
	 * internal_name, just keep it
	 */
	si->s_int_name = internal_name;

	/* Get displayable name */
	maj_stat = gss_display_name(&min_stat, si->s_int_name,
				    &si->s_disp_name, &si->s_mech);
	if (maj_stat != GSS_S_COMPLETE) {
		authgss_log_err("svcauth_gss_set_svc_name: "
				"error getting display name for '%s'\n",
				principal);
		authgss_log_status("svcauth_gss_set_svc_name: "
				   "gss_display_name", maj_stat, min_stat);
		goto out_err;
	}

	maj_stat = gss_indicate_mechs(&min_stat, &req_mechs);
	if (maj_stat != GSS_S_COMPLETE) {
		authgss_log_err("svcauth_gss_set_svc_name: "
				"error getting available mechanisms\n");
		authgss_log_status("svcauth_gss_set_svc_name: "
				   "gss_indicate_mechs", maj_stat, min_stat);
		goto out_err;
	}

	/* Get credentials */
	maj_stat = gss_acquire_cred(&min_stat, si->s_int_name, req_time,
				    req_mechs, GSS_C_ACCEPT,
				    &si->s_cred, &si->s_cred_mechs, NULL);

	if (maj_stat != GSS_S_COMPLETE) {
		authgss_log_err("svcauth_gss_set_svc_name: "
				"error getting credentials for '%s' ('%.*s')\n",
				principal, si->s_disp_name.length,
				si->s_disp_name.value);
		authgss_log_status("svcauth_gss_set_svc_name: "
				   "gss_acquire_cred", maj_stat, min_stat);
		goto out_err;
	}

	/* Chain up the new server_info struct */
	if (_svcauth_server_list == NULL)
		_svcauth_server_list = si;
	else {
		for (tempsi = _svcauth_server_list;
		     tempsi->s_next != NULL; tempsi = tempsi->s_next);
		tempsi->s_next = si;
	}
	retval = TRUE;
	return(retval);

out_err:
	if (si->s_int_name)
		gss_release_name(&min_stat, &si->s_int_name);
	gss_release_buffer(&min_stat, &si->s_disp_name);
	free(si);
	return(retval);
}

static bool_t
svcauth_gss_accept_sec_context(struct svc_req *rqst,
			       struct rpc_gss_init_res *gr)
{
	struct svc_rpc_gss_data	*gd;
	struct rpc_gss_cred	*gc;
	gss_buffer_desc		 recv_tok, seqbuf;
	gss_OID			 mech;
	svcauth_server_info	*si = NULL;
	OM_uint32		 maj_stat = 0, min_stat = 0, ret_flags, seq;
	gss_cred_id_t		 server_cred;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_accept_sec_context()"));

	gd = SVCAUTH_PRIVATE(rqst->rq_xprt->xp_auth);
	gc = (struct rpc_gss_cred *)rqst->rq_clntcred;
	memset(gr, 0, sizeof(*gr));

	/* Deserialize arguments. */
	memset(&recv_tok, 0, sizeof(recv_tok));

	if (!svc_getargs(rqst->rq_xprt, (xdrproc_t)xdr_rpc_gss_init_args,
			 (caddr_t)&recv_tok)) {
		authgss_log_err("svcauth_gss_accept_sec_context: "
			"svc_getargs failed\n");
		return (FALSE);
	}

	/*
	 * Loop trying each registered name/credential.  If gd->server_cred
	 * is already set, then we've already found the right credential
	 * to use (in a previous trip through here) and we use that
	 * credential and never loop.
	 */
	for (si = _svcauth_server_list; si; si = si->s_next) {
		/*
		 * If we've already figured out which credential to
		 * use for this client, then use it.  Otherwise, we're
		 * looping trying to find a credential that works.
		 */
		if (gd->server_cred != NULL) {
			AUTHGSS_DEBUG(1, ("svcauth_gss_accept_sec_context: "
				      "using existing server_cred"));
			server_cred = gd->server_cred;
		} else {
			AUTHGSS_DEBUG(1, ("svcauth_gss_accept_sec_context: "
				      "trying cred for service '%.*s'",
				      si->s_disp_name.length,
				      si->s_disp_name.value));
			server_cred = si->s_cred;
		}

		gr->gr_major = gss_accept_sec_context(&gr->gr_minor,
						      &gd->ctx,
						      server_cred,
						      &recv_tok,
						      GSS_C_NO_CHANNEL_BINDINGS,
						      &gd->client_name,
						      &mech,
						      &gr->gr_token,
						      &ret_flags,
						      NULL,
						      NULL);

		/* If we were using an existing credential, we never loop */
		if (server_cred == gd->server_cred)
			break;

		/*
		 * If this cred worked remember it.  Otherwise, if we got
		 * any error other than GSS_S_FAILURE, then don't bother
		 * looping.  (Assume that all creds will fail.)
		 */
		if (gr->gr_major == GSS_S_COMPLETE ||
		    gr->gr_major == GSS_S_CONTINUE_NEEDED) {

			gd->server_cred = server_cred;
			/* Copy server info pointers, don't free on destroy! */
			gd->server_name = si->s_int_name;
			gd->sname.length = si->s_disp_name.length;
			gd->sname.value = si->s_disp_name.value; 
			AUTHGSS_DEBUG(1, ("svcauth_gss_accept_sec_context: "
				      "cred for '%.*s' seems to have worked!",
				      gd->sname.length,
				      gd->sname.value));
			
			break;
		} else if (gr->gr_major != GSS_S_FAILURE
#ifdef HAVE_KRB5_STUFF
			   || (gr->gr_minor != KRB5KRB_AP_WRONG_PRINC)
#endif
			   ) {
			break;
		}
		gss_release_buffer(&min_stat, &gr->gr_token);
	}

	svc_freeargs(rqst->rq_xprt, (xdrproc_t)xdr_rpc_gss_init_args, (caddr_t)&recv_tok);

	AUTHGSS_DEBUG(1, ("svcauth_gss_accept_sec_context: "
		      "gss_accept_sec_context returned %x:%x",
		      gr->gr_major, gr->gr_minor));
	
	if (gr->gr_major != GSS_S_COMPLETE &&
	    gr->gr_major != GSS_S_CONTINUE_NEEDED) {
		authgss_log_status("accept_sec_context",
				   gr->gr_major, gr->gr_minor);
		gd->ctx = GSS_C_NO_CONTEXT;
		goto errout;
	}

	/*
	 * mechglue returns ctx containing two pointers,
	 * one to the mechanism oid, one to the internal_ctx_id
	 */
	if ((gr->gr_ctx.value = mem_alloc(sizeof(gss_union_ctx_id_desc))) == NULL) {
		authgss_log_err("svcauth_gss_accept_sec_context: out of memory\n");
		goto errout;
	}
	memcpy(gr->gr_ctx.value, gd->ctx, sizeof(gss_union_ctx_id_desc));
	gr->gr_ctx.length = sizeof(gss_union_ctx_id_desc);

	gr->gr_win = sizeof(gd->seqmask) * 8;

	/* Save client info. */
	gd->sec.mech = mech;
	gd->sec.qop = GSS_C_QOP_DEFAULT;
	gd->sec.svc = gc->gc_svc;

	switch(gd->sec.svc) {
        case RPCSEC_GSS_SVC_PRIVACY: 
        	AUTHGSS_DEBUG(1, ("PRIVACY SERVICE\n")); break; 
        case RPCSEC_GSS_SVC_INTEGRITY:
        	AUTHGSS_DEBUG(1, ("INTEGRITY SERVICE\n")); break; 
        case RPCSEC_GSS_SVC_NONE:
        	AUTHGSS_DEBUG(1, ("NO SERVICE\n")); break; 
        }

	gd->seq = gc->gc_seq;
	gd->win = gr->gr_win;

	if (gr->gr_major == GSS_S_COMPLETE) {
		maj_stat = gss_display_name(&min_stat, gd->client_name,
					    &gd->cname, &gd->sec.mech);
		if (maj_stat != GSS_S_COMPLETE) {
			authgss_log_status("display_name", maj_stat, min_stat);
			goto errout;
		}
#ifdef HAVE_KRB5
		{
			gss_buffer_desc mechname;

			gss_oid_to_str(&min_stat, mech, &mechname);

			AUTHGSS_DEBUG(1, ("accepted context for %.*s with "
				  "<mech %.*s, qop %d, svc %d>",
				  gd->cname.length, (char *)gd->cname.value,
				  mechname.length, (char *)mechname.value,
				  gd->sec.qop, gd->sec.svc));

			gss_release_buffer(&min_stat, &mechname);
		}
#elif HAVE_HEIMDAL
		AUTHGSS_DEBUG(1, ("accepted context for %.*s with "
			  "<mech {}, qop %d, svc %d>",
			  gd->cname.length, (char *)gd->cname.value,
			  gd->sec.qop, gd->sec.svc));
#endif
		seq = htonl(gr->gr_win);
		seqbuf.value = &seq;
		seqbuf.length = sizeof(seq);

		gss_release_buffer(&min_stat, &gd->checksum);
		maj_stat = gss_sign(&min_stat, gd->ctx, GSS_C_QOP_DEFAULT,
				    &seqbuf, &gd->checksum);

		if (maj_stat != GSS_S_COMPLETE)
			goto errout;

		rqst->rq_xprt->xp_verf.oa_flavor = RPCSEC_GSS;
		rqst->rq_xprt->xp_verf.oa_base = gd->checksum.value;
		rqst->rq_xprt->xp_verf.oa_length = gd->checksum.length;
	}
	return (TRUE);
errout:
	gss_release_buffer(&min_stat, &gr->gr_token);
	return (FALSE);
}

static bool_t
svcauth_gss_validate(struct svc_rpc_gss_data *gd, struct rpc_msg *msg)
{
	struct opaque_auth	*oa;
	gss_buffer_desc		 rpcbuf, checksum;
	OM_uint32		 maj_stat, min_stat, qop_state;
	int32_t			 bufsize;
	int32_t			*buf;
	u_char			*rpchdr;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_validate()"));

	oa = &msg->rm_call.cb_cred;
	if (oa->oa_length > MAX_AUTH_BYTES)
		return (FALSE);

	bufsize = 8 * BYTES_PER_XDR_UNIT + RNDUP(oa->oa_length);
	buf = calloc(bufsize, 1);
	if (buf == NULL)
		return (FALSE);
	rpchdr = (u_char *) buf;

	IXDR_PUT_LONG(buf, msg->rm_xid);
	IXDR_PUT_ENUM(buf, msg->rm_direction);
	IXDR_PUT_LONG(buf, msg->rm_call.cb_rpcvers);
	IXDR_PUT_LONG(buf, msg->rm_call.cb_prog);
	IXDR_PUT_LONG(buf, msg->rm_call.cb_vers);
	IXDR_PUT_LONG(buf, msg->rm_call.cb_proc);
	IXDR_PUT_ENUM(buf, oa->oa_flavor);
	IXDR_PUT_LONG(buf, oa->oa_length);
	if (oa->oa_length) {
		memcpy((caddr_t)buf, oa->oa_base, oa->oa_length);
		buf += RNDUP(oa->oa_length) / sizeof(int32_t);
	}
	rpcbuf.value = rpchdr;
	rpcbuf.length = (u_char *)buf - rpchdr;

	checksum.value = msg->rm_call.cb_verf.oa_base;
	checksum.length = msg->rm_call.cb_verf.oa_length;

	maj_stat = gss_verify_mic(&min_stat, gd->ctx, &rpcbuf, &checksum,
				  &qop_state);

	free(rpchdr);

	if (maj_stat != GSS_S_COMPLETE) {
		authgss_log_status("gss_verify_mic", maj_stat, min_stat);
		return (FALSE);
	}
	return (TRUE);
}

bool_t
svcauth_gss_nextverf(struct svc_req *rqst, u_int num)
{
	struct svc_rpc_gss_data	*gd;
	gss_buffer_desc		 signbuf;
	OM_uint32		 maj_stat, min_stat;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_nextverf()"));

	if (rqst->rq_xprt->xp_auth == NULL)
		return (FALSE);

	gd = SVCAUTH_PRIVATE(rqst->rq_xprt->xp_auth);

	gss_release_buffer(&min_stat, &gd->checksum);

	signbuf.value = &num;
	signbuf.length = sizeof(num);

	maj_stat = gss_get_mic(&min_stat, gd->ctx, gd->sec.qop,
			       &signbuf, &gd->checksum);

	if (maj_stat != GSS_S_COMPLETE) {
		authgss_log_status("gss_get_mic", maj_stat, min_stat);
		return (FALSE);
	}
	rqst->rq_xprt->xp_verf.oa_flavor = RPCSEC_GSS;
	rqst->rq_xprt->xp_verf.oa_base = (caddr_t)gd->checksum.value;
	rqst->rq_xprt->xp_verf.oa_length = (u_int)gd->checksum.length;

	return (TRUE);
}

enum auth_stat
_svcauth_gss(struct svc_req *rqst, struct rpc_msg *msg, bool_t *no_dispatch)
{
	enum auth_stat		retstat;
	XDR	 		 xdrs;
	SVCAUTH			*auth;
	struct svc_rpc_gss_data	*gd;
	struct rpc_gss_cred	*gc;
	struct rpc_gss_init_res	 gr;
	int			 call_stat, offset;
	OM_uint32		 min_stat;

	AUTHGSS_DEBUG(1, ("in svcauth_gss()"));

	/* Initialize reply. */
	rqst->rq_xprt->xp_verf = _null_auth;

	/* Allocate and set up server auth handle. */
	if (rqst->rq_xprt->xp_auth == NULL ||
	    rqst->rq_xprt->xp_auth == &svc_auth_none) {
		if ((auth = calloc(sizeof(*auth), 1)) == NULL) {
			authgss_log_err("svcauth_gss: out_of_memory\n");
			return (AUTH_FAILED);
		}
		if ((gd = calloc(sizeof(*gd), 1)) == NULL) {
			authgss_log_err("svcauth_gss: out_of_memory\n");
			return (AUTH_FAILED);
		}
		auth->svc_ah_ops = &svc_auth_gss_ops;
		auth->svc_ah_private = (caddr_t) gd;
		rqst->rq_xprt->xp_auth = auth;
	}
	else gd = SVCAUTH_PRIVATE(rqst->rq_xprt->xp_auth);

	/* Deserialize client credentials. */
	if (rqst->rq_cred.oa_length <= 0)
		return (AUTH_BADCRED);

	gc = (struct rpc_gss_cred *)rqst->rq_clntcred;
	memset(gc, 0, sizeof(*gc));

	xdrmem_create(&xdrs, rqst->rq_cred.oa_base,
		      rqst->rq_cred.oa_length, XDR_DECODE);

	if (!xdr_rpc_gss_cred(&xdrs, gc)) {
		XDR_DESTROY(&xdrs);
		return (AUTH_BADCRED);
	}
	XDR_DESTROY(&xdrs);

	retstat = AUTH_FAILED;

#define ret_freegc(code) do { retstat = code; goto freegc; } while (0)

	/* Check version. */
	if (gc->gc_v != RPCSEC_GSS_VERSION)
		ret_freegc (AUTH_BADCRED);

	/* Check RPCSEC_GSS service. */
	if (gc->gc_svc != RPCSEC_GSS_SVC_NONE &&
	    gc->gc_svc != RPCSEC_GSS_SVC_INTEGRITY &&
	    gc->gc_svc != RPCSEC_GSS_SVC_PRIVACY)
		ret_freegc (AUTH_BADCRED);

	/* Check sequence number. */
	if (gd->established) {
		if (gc->gc_seq > MAXSEQ)
			ret_freegc (RPCSEC_GSS_CTXPROBLEM);

		if ((offset = gd->seqlast - gc->gc_seq) < 0) {
			gd->seqlast = gc->gc_seq;
			offset = 0 - offset;
			gd->seqmask <<= offset;
			offset = 0;
		}
		else if (offset >= gd->win || (gd->seqmask & (1 << offset))) {
			*no_dispatch = 1;
			ret_freegc (RPCSEC_GSS_CTXPROBLEM);
		}
		gd->seq = gc->gc_seq;
		gd->seqmask |= (1 << offset);
	}

	if (gd->established) {
		rqst->rq_clntname = (char *)gd->client_name;
		rqst->rq_svcname = (char *)gd->ctx;
	}

	/* Handle RPCSEC_GSS control procedure. */
	switch (gc->gc_proc) {

	case RPCSEC_GSS_INIT:
	case RPCSEC_GSS_CONTINUE_INIT:
		if (rqst->rq_proc != NULLPROC)
			ret_freegc (AUTH_FAILED);		/* XXX ? */

		if (_svcauth_server_list == NULL) {
			authgss_log_err("_svcauth_gss: called without any "
					"server names set!\n");
			if (svcauth_gss_set_svc_name("nfs", "krb5", 0, 0, 0)
								== FALSE) {
				authgss_log_err("_svcauth_gss: unable to "
					"set default service name!\n");
				ret_freegc (AUTH_FAILED);
			}
		}

		if (!svcauth_gss_accept_sec_context(rqst, &gr))
			ret_freegc (AUTH_REJECTEDCRED);

		if (gr.gr_major == GSS_S_COMPLETE &&
		    !svcauth_gss_nextverf(rqst, htonl(gr.gr_win))) {
			gss_release_buffer(&min_stat, &gr.gr_token);
			ret_freegc (AUTH_FAILED);
		}

		*no_dispatch = TRUE;

		call_stat = svc_sendreply(rqst->rq_xprt, (xdrproc_t)xdr_rpc_gss_init_res,
					  (caddr_t)&gr);

		gss_release_buffer(&min_stat, &gr.gr_token);
		gss_release_buffer(&min_stat, &gr.gr_ctx);

		if (!call_stat)
			ret_freegc (AUTH_FAILED);

		if (gr.gr_major == GSS_S_COMPLETE)
			gd->established = TRUE;

		break;

	case RPCSEC_GSS_DATA:
		if (!svcauth_gss_validate(gd, msg))
			ret_freegc (RPCSEC_GSS_CREDPROBLEM);

		if (!svcauth_gss_nextverf(rqst, htonl(gc->gc_seq)))
			ret_freegc (AUTH_FAILED);
		break;

	case RPCSEC_GSS_DESTROY:
		if (rqst->rq_proc != NULLPROC)
			ret_freegc (AUTH_FAILED);		/* XXX ? */

		if (!svcauth_gss_validate(gd, msg))
			ret_freegc (RPCSEC_GSS_CREDPROBLEM);

		if (!svcauth_gss_nextverf(rqst, htonl(gc->gc_seq)))
			ret_freegc (AUTH_FAILED);

		SVCAUTH_DESTROY(rqst->rq_xprt->xp_auth);
		rqst->rq_xprt->xp_auth = &svc_auth_none;

		break;

	default:
		ret_freegc (AUTH_REJECTEDCRED);
		break;
	}
	retstat =  AUTH_OK;
freegc:
	xdr_free((xdrproc_t)xdr_rpc_gss_cred, (char *)gc);
	AUTHGSS_DEBUG(1, ("svcauth_gss: returning %d", retstat));
	return (retstat);
}

bool_t
svcauth_gss_destroy(SVCAUTH *auth)
{
	struct svc_rpc_gss_data	*gd;
	OM_uint32		 min_stat;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_destroy()"));

	gd = SVCAUTH_PRIVATE(auth);

	gss_delete_sec_context(&min_stat, &gd->ctx, GSS_C_NO_BUFFER);
	gss_release_buffer(&min_stat, &gd->cname);
	/* This (checksum) needs to be freed elsewhere, or we
	 * need to make a copy in svcauth_gss_nextverf instead
	 * of referencing this */
	/* gss_release_buffer(&min_stat, &gd->checksum); */
	/* gd->sname points to server info data, don't release it */

	if (gd->client_name)
		gss_release_name(&min_stat, &gd->client_name);
	/* gd->server_name points to server info data, don't release it */

	mem_free(gd, sizeof(*gd));
	mem_free(auth, sizeof(*auth));

	return (TRUE);
}

bool_t
svcauth_gss_wrap(SVCAUTH *auth, XDR *xdrs, xdrproc_t xdr_func, caddr_t xdr_ptr)
{
	struct svc_rpc_gss_data	*gd;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_wrap()"));

	gd = SVCAUTH_PRIVATE(auth);

	if (!gd->established || gd->sec.svc == RPCSEC_GSS_SVC_NONE) {
		return ((*xdr_func)(xdrs, xdr_ptr));
	}
	return (xdr_rpc_gss_data(xdrs, xdr_func, xdr_ptr,
				 gd->ctx, gd->sec.qop,
				 gd->sec.svc, gd->seq));
}

bool_t
svcauth_gss_unwrap(SVCAUTH *auth, XDR *xdrs, xdrproc_t xdr_func, caddr_t xdr_ptr)
{
	struct svc_rpc_gss_data	*gd;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_unwrap()"));

	gd = SVCAUTH_PRIVATE(auth);

	if (!gd->established || gd->sec.svc == RPCSEC_GSS_SVC_NONE) {
		return ((*xdr_func)(xdrs, xdr_ptr));
	}
	return (xdr_rpc_gss_data(xdrs, xdr_func, xdr_ptr,
				 gd->ctx, gd->sec.qop,
				 gd->sec.svc, gd->seq));
}

char *
svcauth_gss_get_principal(SVCAUTH *auth)
{
	struct svc_rpc_gss_data *gd;
	char *pname;

	AUTHGSS_DEBUG(1, ("in svcauth_gss_get_principal()"));

	gd = SVCAUTH_PRIVATE(auth);

	if (gd->cname.length == 0)
		return (NULL);

	if ((pname = malloc(gd->cname.length + 1)) == NULL)
		return (NULL);

	memcpy(pname, gd->cname.value, gd->cname.length);
	pname[gd->cname.length] = '\0';

	return (pname);
}
