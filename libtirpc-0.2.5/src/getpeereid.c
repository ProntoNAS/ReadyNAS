/*
 * Copyright (c) 2001 Dima Dorfman.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#ifdef HAVE_SYS_USER_H
#  include <sys/user.h>
#endif /* HAVE_SYS_USER_H */

#include <errno.h>
#include <unistd.h>

int
getpeereid(int s, uid_t *euid, gid_t *egid)
{
#ifndef HAVE_SYS_USER_H
        return(-1);
#else
#ifdef XUCRED_VERSION
	struct xucred uc;
#define uid  cr_uid
#define gid  cr_gid
#else
	struct ucred uc;
#endif
	socklen_t uclen;
	int error;

	uclen = sizeof(uc); 
#ifdef XUCRED_VERSION
	error = getsockopt(s, 0, LOCAL_PEERCRED, &uc, &uclen);
#else
	error = getsockopt(s, SOL_SOCKET, SO_PEERCRED, &uc, &uclen); /*  SCM_CREDENTIALS */
#endif
	if (error != 0)
		return (error);
	//	if (uc.cr_version != XUCRED_VERSION)
	//	return (EINVAL);
	*euid = uc.uid;
	*egid = uc.gid;
	return (0);
#endif /* HAVE_SYS_USER_H */
 }
