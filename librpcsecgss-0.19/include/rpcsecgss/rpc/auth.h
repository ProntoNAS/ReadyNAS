/*	$OpenBSD: auth.h,v 1.2 1997/09/21 10:46:09 niklas Exp $	*/
/*	$NetBSD: auth.h,v 1.7 1995/04/29 05:27:55 cgd Exp $	*/

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
 *
 *	from: @(#)auth.h 1.17 88/02/08 SMI
 *	@(#)auth.h	2.3 88/08/07 4.0 RPCSRC
 */

/*
 * auth.h, Authentication interface.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * The data structures are completely opaque to the client.  The client
 * is required to pass a AUTH * to routines that create rpc
 * "sessions".
 */

#ifndef _RPC_AUTH_H
#define _RPC_AUTH_H
#include <sys/cdefs.h>

#define MAX_AUTH_BYTES	400
#define MAXNETNAMELEN	255	/* maximum length of network user's name */

/*
 * Status returned from authentication check
 */
enum auth_stat {
	AUTH_OK=0,
	/*
	 * failed at remote end
	 */
	AUTH_BADCRED=1,			/* bogus credentials (seal broken) */
	AUTH_REJECTEDCRED=2,		/* client should begin new session */
	AUTH_BADVERF=3,			/* bogus verifier (seal broken) */
	AUTH_REJECTEDVERF=4,		/* verifier expired or was replayed */
	AUTH_TOOWEAK=5,			/* rejected due to security reasons */
	/*
	 * failed locally
	*/
	AUTH_INVALIDRESP=6,		/* bogus response verifier */
	AUTH_FAILED=7,			/* some unknown reason */
	/*
	 * RPCSEC_GSS errors
	 */
	RPCSEC_GSS_CREDPROBLEM = 13,
	RPCSEC_GSS_CTXPROBLEM = 14
};

typedef u_int32_t u_int32;	/* 32-bit unsigned integers */

union des_block {
	struct {
		u_int32 high;
		u_int32 low;
	} key;
	char c[8];
};
typedef union des_block des_block;
__BEGIN_DECLS
extern bool_t xdr_des_block __P((XDR *, des_block *));
__END_DECLS

/*
 * Authentication info.  Opaque to client.
 */
struct opaque_auth {
	enum_t	oa_flavor;		/* flavor of auth */
	caddr_t	oa_base;		/* address of more auth stuff */
	u_int	oa_length;		/* not to exceed MAX_AUTH_BYTES */
};


/*
 * Auth handle, interface to client side authenticators.
 */
typedef struct __rpc_auth {
	struct	opaque_auth	ah_cred;
	struct	opaque_auth	ah_verf;
	union	des_block	ah_key;
	struct auth_ops {
		void	(*ah_nextverf) __P((struct __rpc_auth *));
		/* nextverf & serialize */
		int	(*ah_marshal) __P((struct __rpc_auth *, XDR *));
		/* validate verifier */
		int	(*ah_validate) __P((struct __rpc_auth *,
			    struct opaque_auth *));
		/* refresh credentials */
		int	(*ah_refresh) __P((struct __rpc_auth *));
		/* destroy this structure */
		void	(*ah_destroy) __P((struct __rpc_auth *));
		/* prepare to refresh credentials by destroying gss context */
		void	(*ah_prep_refresh) __P((struct __rpc_auth *));
		/* returns number of retries remaining for this rpc session */
		int	(*ah_get_retries) __P((struct __rpc_auth *));
		/* encode data for wire */
		int	(*ah_wrap) __P((struct __rpc_auth *, XDR *, xdrproc_t, caddr_t));
		/* decode data for wire */
		int	(*ah_unwrap) __P((struct __rpc_auth *, XDR *, xdrproc_t, caddr_t));

	} *ah_ops;
	caddr_t ah_private;
} AUTH;


/*
 * Authentication ops.
 * The ops and the auth handle provide the interface to the authenticators.
 *
 * AUTH	*auth;
 * XDR	*xdrs;
 * struct opaque_auth verf;
 */
#define AUTH_NEXTVERF(auth)		\
		((*((auth)->ah_ops->ah_nextverf))(auth))
#define auth_nextverf(auth)		\
		((*((auth)->ah_ops->ah_nextverf))(auth))

#define AUTH_MARSHALL(auth, xdrs)	\
		((*((auth)->ah_ops->ah_marshal))(auth, xdrs))
#define auth_marshall(auth, xdrs)	\
		((*((auth)->ah_ops->ah_marshal))(auth, xdrs))

#define AUTH_VALIDATE(auth, verfp)	\
		((*((auth)->ah_ops->ah_validate))((auth), verfp))
#define auth_validate(auth, verfp)	\
		((*((auth)->ah_ops->ah_validate))((auth), verfp))

#define AUTH_REFRESH(auth)		\
		((*((auth)->ah_ops->ah_refresh))(auth))
#define auth_refresh(auth)		\
		((*((auth)->ah_ops->ah_refresh))(auth))

#define AUTH_DESTROY(auth)		\
		((*((auth)->ah_ops->ah_destroy))(auth))
#define auth_destroy(auth)		\
		((*((auth)->ah_ops->ah_destroy))(auth))

#define AUTH_PREP_REFRESH(auth)		\
		((*((auth)->ah_ops->ah_prep_refresh))(auth))
#define auth_prep_refresh(auth)		\
		((*((auth)->ah_ops->ah_prep_refresh))(auth))

#define AUTH_GET_RETRIES(auth)		\
		((*((auth)->ah_ops->ah_get_retries))(auth))
#define auth_get_retries(auth)		\
		((*((auth)->ah_ops->ah_get_retries))(auth))

#define AUTH_WRAP(auth, xdrs, xfunc, xwhere)            \
                ((*((auth)->ah_ops->ah_wrap))(auth, xdrs, \
                                              xfunc, xwhere))
#define auth_wrap(auth, xdrs, xfunc, xwhere)            \
                ((*((auth)->ah_ops->ah_wrap))(auth, xdrs, \
                                              xfunc, xwhere))

#define AUTH_UNWRAP(auth, xdrs, xfunc, xwhere)          \
                ((*((auth)->ah_ops->ah_unwrap))(auth, xdrs, \
                                              xfunc, xwhere))
#define auth_unwrap(auth, xdrs, xfunc, xwhere)          \
                ((*((auth)->ah_ops->ah_unwrap))(auth, xdrs, \
                                              xfunc, xwhere))


extern struct opaque_auth _null_auth;

/*
 * Any style authentication.  These routines can be used by any
 * authentication style that does not use the wrap/unwrap functions.
 */
int authany_wrap(struct __rpc_auth *, XDR *, xdrproc_t, caddr_t);
int authany_unwrap(struct __rpc_auth *, XDR *, xdrproc_t, caddr_t);

/*
 * These are the various implementations of client side authenticators.
 */

/*
 * Unix style authentication
 * AUTH *authunix_create(machname, uid, gid, len, aup_gids)
 *	char *machname;
 *	int uid;
 *	int gid;
 *	int len;
 *	int *aup_gids;
 */
__BEGIN_DECLS
struct sockaddr_in;
extern AUTH *authunix_create		__P((char *, int, int, int, int *));
extern AUTH *authunix_create_default	__P((void));
extern AUTH *authnone_create		__P((void));
extern AUTH *authdes_create		__P((char *, u_int,
					    struct sockaddr_in *, des_block *));
extern bool_t xdr_opaque_auth		__P((XDR *, struct opaque_auth *));
__END_DECLS

#define AUTH_NONE	0		/* no authentication */
#define	AUTH_NULL	0		/* backward compatibility */
#define	AUTH_UNIX	1		/* unix style (uid, gids) */
#define	AUTH_SHORT	2		/* short hand unix style */
#define AUTH_DES	3		/* des style (encrypted timestamps) */
#define RPCSEC_GSS	6		/* RPCSEC_GSS */

#include <rpc/auth_gss.h>	/* Include here so we don't have to change rpc.h */

#endif /* !_RPC_AUTH_H */
