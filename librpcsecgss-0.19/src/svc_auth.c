/*
 * Copyright (c) 2009, Sun Microsystems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sun Microsystems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: svc_auth.c,v 1.4 1998/03/19 00:27:22 millert Exp $";
#endif /* LIBC_SCCS and not lint */

/*
 * svc_auth_nodes.c, Server-side rpc authenticator interface,
 * *WITHOUT* DES authentication.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#include <rpc/rpc.h>

/*
 * svcauthsw is the bdevsw of server side authentication.
 *
 * Server side authenticators are called from authenticate by
 * using the client auth struct flavor field to index into svcauthsw.
 * The server auth flavors must implement a routine that looks
 * like:
 *
 *	enum auth_stat
 *	flavorx_auth(rqst, msg)
 *		register struct svc_req *rqst;
 *		register struct rpc_msg *msg;
 *
 */

enum auth_stat _svcauth_none();		/* no authentication */
enum auth_stat _svcauth_unix();		/* unix style (uid, gids) */
enum auth_stat _svcauth_short();	/* short hand unix style */
enum auth_stat _svcauth_gss();		/* RPCSEC_GSS */

static struct {
	enum auth_stat (*authenticator)();
} svcauthsw[] = {
	{ _svcauth_none },		/* AUTH_NONE */
	{ _svcauth_unix },		/* AUTH_UNIX */
	{ _svcauth_short },		/* AUTH_SHORT */
	{ _svcauth_none },		/* AUTH_DES - does not exist */
	{ _svcauth_none },
	{ _svcauth_none },
	{ _svcauth_gss }		/* RPCSEC_GSS */
};
#define	AUTH_MAX	6		/* HIGHEST AUTH NUMBER */

/*
 * The call rpc message, msg has been obtained from the wire.  The msg contains
 * the raw form of credentials and verifiers.  authenticate returns AUTH_OK
 * if the msg is successfully authenticated.  If AUTH_OK then the routine also
 * does the following things:
 * set rqst->rq_xprt->verf to the appropriate response verifier;
 * sets rqst->rq_clntcred to the "cooked" form of the credentials.
 *
 * NB: rqst->rq_xprt->verf must be pre-alloctaed;
 * its length is set appropriately.
 *
 * The caller still owns and is responsible for msg->u.cmb.cred and
 * msg->u.cmb.verf.  The authentication system retains ownership of
 * rqst->rq_clntcred, the cooked credentials.
 *
 * There is an assumption that any flavour less than AUTH_NONE is
 * invalid.
 */
enum auth_stat
_authenticate(rqst, msg, no_dispatch)
	register struct svc_req *rqst;
	struct rpc_msg *msg;
	bool_t *no_dispatch;
{
	register int cred_flavor;

	rqst->rq_cred = msg->rm_call.cb_cred;
	rqst->rq_xprt->xp_verf.oa_flavor = _null_auth.oa_flavor;
	rqst->rq_xprt->xp_verf.oa_length = 0;
	cred_flavor = rqst->rq_cred.oa_flavor;
	*no_dispatch = FALSE;

	if ((cred_flavor <= AUTH_MAX) && (cred_flavor >= AUTH_NONE)) {
		return ((*(svcauthsw[cred_flavor].authenticator))(rqst, msg, no_dispatch));
	}

	return (AUTH_REJECTEDCRED);
}
