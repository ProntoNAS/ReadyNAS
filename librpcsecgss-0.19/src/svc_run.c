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
static char *rcsid = "$OpenBSD: svc_run.c,v 1.10 1998/03/19 00:27:24 millert Exp $";
#endif /* LIBC_SCCS and not lint */

/*
 * This is the rpc server side idle loop
 * Wait for input, call server program.
 */
#include <rpc/rpc.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int __svc_fdsetsize;
extern fd_set *__svc_fdset;

void svc_getreqset2 __P((fd_set *, int));

void
svc_run()
{
	fd_set *fds;

	for (;;) {
		if (__svc_fdset) {
			int bytes = howmany(__svc_fdsetsize, NFDBITS) *
			    sizeof(fd_mask);
			fds = (fd_set *)malloc(bytes);
			memcpy(fds, __svc_fdset, bytes);
		} else
			fds = NULL;
		switch (select(svc_maxfd+1, fds, 0, 0, (struct timeval *)0)) {
		case -1:
			if (errno == EINTR) {
				if (fds)
					free(fds);
				continue;
			}
			authgss_perror(errno, "svc_run: select failed");
			if (fds)
				free(fds);
			return;
		case 0:
			if (fds)
				free(fds);
			continue;
		default:
			svc_getreqset2(fds, svc_maxfd+1);
			free(fds);
		}
	}
}
