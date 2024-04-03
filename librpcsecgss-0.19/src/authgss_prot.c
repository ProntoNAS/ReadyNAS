/*
  authgss_prot.c

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
#include <stdarg.h>
#include <ctype.h>
#include <syslog.h>
#include <string.h>
#include <rpc/rpc.h>
#include <gssapi/gssapi.h>

bool_t
xdr_rpc_gss_buf(XDR *xdrs, gss_buffer_t buf, u_int maxsize) 
{ 
	bool_t xdr_stat;
	u_int tmplen; 
  
	if (xdrs->x_op != XDR_DECODE) {
		if (buf->length > UINT_MAX)
			return (FALSE);
		else
			tmplen = buf->length;
	}
	xdr_stat = xdr_bytes(xdrs, (char **)&buf->value, &tmplen, maxsize);

	if (xdr_stat && xdrs->x_op == XDR_DECODE)
		buf->length = tmplen;

	AUTHGSS_DEBUG(3, ("xdr_rpc_gss_buf: %s %s "
		  "(%p:%d)",
		  (xdrs->x_op == XDR_ENCODE) ? "encode" : "decode",
		  (xdr_stat == TRUE) ? "success" : "failure",
		  buf->value, buf->length));

	return (xdr_stat);
}

bool_t
xdr_rpc_gss_cred(XDR *xdrs, struct rpc_gss_cred *p)
{
	bool_t xdr_stat;

	xdr_stat = (xdr_u_int(xdrs, &p->gc_v) &&
		    xdr_enum(xdrs, (enum_t *)&p->gc_proc) &&
		    xdr_u_int(xdrs, &p->gc_seq) &&
		    xdr_enum(xdrs, (enum_t *)&p->gc_svc) &&
		    xdr_rpc_gss_buf(xdrs, &p->gc_ctx, MAX_AUTH_BYTES));

	AUTHGSS_DEBUG(1, ("xdr_rpc_gss_cred: %s %s "
		  "(v %d, proc %d, seq %d, svc %d, ctx %p:%d)",
		  (xdrs->x_op == XDR_ENCODE) ? "encode" : "decode",
		  (xdr_stat == TRUE) ? "success" : "failure",
		  p->gc_v, p->gc_proc, p->gc_seq, p->gc_svc,
		  p->gc_ctx.value, p->gc_ctx.length));

	return (xdr_stat);
}

bool_t
xdr_rpc_gss_init_args(XDR *xdrs, gss_buffer_desc *p)
{
	bool_t xdr_stat;
	u_int maxlen = p->length + 1024;

	if (xdrs->x_op == XDR_DECODE && p->value == NULL)
		maxlen = -1;

	xdr_stat = xdr_rpc_gss_buf(xdrs, p, maxlen);

	AUTHGSS_DEBUG(1, ("xdr_rpc_gss_init_args: %s %s (token %p:%d)",
		  (xdrs->x_op == XDR_ENCODE) ? "encode" : "decode",
		  (xdr_stat == TRUE) ? "success" : "failure",
		  p->value, p->length));

	return (xdr_stat);
}

bool_t
xdr_rpc_gss_init_res(XDR *xdrs, struct rpc_gss_init_res *p)
{
	bool_t xdr_stat;
	u_int maxctxlen = p->gr_ctx.length + 1024;
	u_int maxtoklen = p->gr_token.length + 1024;

	if (xdrs->x_op == XDR_DECODE && p->gr_ctx.value == NULL)
	    maxctxlen = -1;
	if (xdrs->x_op == XDR_DECODE && p->gr_token.value == NULL)
	    maxtoklen = -1;

	xdr_stat = (xdr_rpc_gss_buf(xdrs, &p->gr_ctx, maxctxlen) &&
		    xdr_u_int(xdrs, &p->gr_major) &&
		    xdr_u_int(xdrs, &p->gr_minor) &&
		    xdr_u_int(xdrs, &p->gr_win) &&
		    xdr_rpc_gss_buf(xdrs, &p->gr_token, maxtoklen));

	AUTHGSS_DEBUG(1, ("xdr_rpc_gss_init_res %s %s "
		  "(ctx %p:%d, maj %d, min %d, win %d, token %p:%d)",
		  (xdrs->x_op == XDR_ENCODE) ? "encode" : "decode",
		  (xdr_stat == TRUE) ? "success" : "failure",
		  p->gr_ctx.value, p->gr_ctx.length,
		  p->gr_major, p->gr_minor, p->gr_win,
		  p->gr_token.value, p->gr_token.length));

	return (xdr_stat);
}

bool_t
xdr_rpc_gss_wrap_data(XDR *xdrs, xdrproc_t xdr_func, caddr_t xdr_ptr,
		      gss_ctx_id_t ctx, gss_qop_t qop,
		      rpc_gss_svc_t svc, u_int seq)
{
	gss_buffer_desc	databuf, wrapbuf;
	OM_uint32	maj_stat, min_stat;
	int		start, end, conf_state;
	bool_t		xdr_stat;
	u_int		databuflen;

	/* Skip databody length. */
	start = XDR_GETPOS(xdrs);
	XDR_SETPOS(xdrs, start + 4);

	memset(&databuf, 0, sizeof(databuf)); 
	memset(&wrapbuf, 0, sizeof(wrapbuf));

	/* Marshal rpc_gss_data_t (sequence number + arguments). */
	if (!xdr_u_int(xdrs, &seq) || !(*xdr_func)(xdrs, xdr_ptr))
		return (FALSE);
	end = XDR_GETPOS(xdrs);

	/* Set databuf to marshalled rpc_gss_data_t. */
	databuflen = end - start - 4;
	XDR_SETPOS(xdrs, start + 4);
	databuf.value = XDR_INLINE(xdrs, databuflen);

	xdr_stat = FALSE;

	if (svc == RPCSEC_GSS_SVC_INTEGRITY) {
		/* Marshal databody_integ length. */
		XDR_SETPOS(xdrs, start);
		if (!xdr_u_int(xdrs, &databuflen))
			return (FALSE);
		databuf.length = databuflen;

		/* Checksum rpc_gss_data_t. */
		maj_stat = gss_get_mic(&min_stat, ctx, qop,
				       &databuf, &wrapbuf);
		if (maj_stat != GSS_S_COMPLETE) {
			authgss_log_status("gss_get_mic failed",
					   maj_stat, min_stat);
			return (FALSE);
		}
		/* Marshal checksum. */
		XDR_SETPOS(xdrs, end);
		xdr_stat = xdr_rpc_gss_buf(xdrs, &wrapbuf, (unsigned int) -1);
		gss_release_buffer(&min_stat, &wrapbuf);
	}
	else if (svc == RPCSEC_GSS_SVC_PRIVACY) {
		/* Encrypt rpc_gss_data_t. */
		databuf.length = databuflen;
		maj_stat = gss_wrap(&min_stat, ctx, TRUE, qop, &databuf,
				    &conf_state, &wrapbuf);
		if (maj_stat != GSS_S_COMPLETE) {
			authgss_log_status("gss_wrap failed",
					   maj_stat, min_stat);
			return (FALSE);
		}
		/* Marshal databody_priv. */
		XDR_SETPOS(xdrs, start);
		xdr_stat = xdr_rpc_gss_buf(xdrs, &wrapbuf, (unsigned int) -1);
		gss_release_buffer(&min_stat, &wrapbuf);
	}
	return (xdr_stat);
}

bool_t
xdr_rpc_gss_unwrap_data(XDR *xdrs, xdrproc_t xdr_func, caddr_t xdr_ptr,
			gss_ctx_id_t ctx, gss_qop_t qop,
			rpc_gss_svc_t svc, u_int seq)
{
	XDR		tmpxdrs;
	gss_buffer_desc	databuf, wrapbuf;
	OM_uint32	maj_stat, min_stat;
	u_int		seq_num, conf_state, qop_state;
	bool_t		xdr_stat;

	if (xdr_func == (xdrproc_t) xdr_void || xdr_ptr == NULL)
		return (TRUE);

	memset(&databuf, 0, sizeof(databuf));
	memset(&wrapbuf, 0, sizeof(wrapbuf));

	if (svc == RPCSEC_GSS_SVC_INTEGRITY) {
		/* Decode databody_integ. */
		if (!xdr_rpc_gss_buf(xdrs, &databuf, (unsigned int) -1)) {
			authgss_log_err("xdr decode databody_integ failed");
			return (FALSE);
		}
		/* Decode checksum. */
		if (!xdr_rpc_gss_buf(xdrs, &wrapbuf, (unsigned int) -1)) {
			gss_release_buffer(&min_stat, &databuf);
			authgss_log_err("xdr decode checksum failed");
			return (FALSE);
		}
		/* Verify checksum and QOP. */
		maj_stat = gss_verify_mic(&min_stat, ctx, &databuf,
					  &wrapbuf, &qop_state);
		gss_release_buffer(&min_stat, &wrapbuf);

		if (maj_stat != GSS_S_COMPLETE || qop_state != qop) {
			gss_release_buffer(&min_stat, &databuf);
			authgss_log_status("gss_verify_mic",
					   maj_stat, min_stat);
			return (FALSE);
		}
	}
	else if (svc == RPCSEC_GSS_SVC_PRIVACY) {
		/* Decode databody_priv. */
		if (!xdr_rpc_gss_buf(xdrs, &wrapbuf, (unsigned int) -1)) {
			authgss_log_err("xdr decode databody_priv failed");
			return (FALSE);
		}
		/* Decrypt databody. */
		maj_stat = gss_unwrap(&min_stat, ctx, &wrapbuf, &databuf,
				      &conf_state, &qop_state);

		gss_release_buffer(&min_stat, &wrapbuf);

		/* Verify encryption and QOP. */
		if (maj_stat != GSS_S_COMPLETE || qop_state != qop ||
			conf_state != TRUE) {
			gss_release_buffer(&min_stat, &databuf);
			authgss_log_status("gss_unwrap", maj_stat, min_stat);
			return (FALSE);
		}
	}
	/* Decode rpc_gss_data_t (sequence number + arguments). */
	xdrmem_create(&tmpxdrs, databuf.value, databuf.length, XDR_DECODE);
	xdr_stat = (xdr_u_int(&tmpxdrs, &seq_num) &&
		    (*xdr_func)(&tmpxdrs, xdr_ptr));
	XDR_DESTROY(&tmpxdrs);
	gss_release_buffer(&min_stat, &databuf);

	/* Verify sequence number. */
	if (xdr_stat == TRUE && seq_num != seq) {
		authgss_log_err("wrong sequence number in databody");
		return (FALSE);
	}
	return (xdr_stat);
}

bool_t
xdr_rpc_gss_data(XDR *xdrs, xdrproc_t xdr_func, caddr_t xdr_ptr,
		 gss_ctx_id_t ctx, gss_qop_t qop,
		 rpc_gss_svc_t svc, u_int seq)
{
	switch (xdrs->x_op) {

	case XDR_ENCODE:
		return (xdr_rpc_gss_wrap_data(xdrs, xdr_func, xdr_ptr,
					      ctx, qop, svc, seq));
	case XDR_DECODE:
		return (xdr_rpc_gss_unwrap_data(xdrs, xdr_func, xdr_ptr,
						ctx, qop,svc, seq));
	case XDR_FREE:
		return (TRUE);
	}
	return (FALSE);
}

/* library global debug level */
int authgss_debug_level = 0;

/*
 * Set the debug level for the rpcsec_gss code.
 * Return the old value so that it can be set
 * and restored easily.
 */
int
authgss_set_debug_level(int new_level)
{
	int old_level;

	if (new_level < 0)
		new_level = 0;
	old_level = authgss_debug_level;
	authgss_debug_level = new_level;
	AUTHGSS_DEBUG(1, ("rpcsec_gss: debug level is %d",
			authgss_debug_level));
	return (old_level);
}

/* Log messages to syslog */
void
authgss_log_err(char *format, ...)
{
	va_list vap;

	va_start(vap, format);
	vsyslog(LOG_ERR, format, vap);
	va_end(vap);
}

/* Log debug messages to syslog */
void
authgss_log_dbg(char *format, ...)
{
	va_list vap;

	va_start(vap, format);
	vsyslog(LOG_DEBUG, format, vap);
	va_end(vap);
}

/* Format gss return value status message, and log it */
void
authgss_log_status(char *m, OM_uint32 maj_stat, OM_uint32 min_stat)
{
	OM_uint32 min;
	gss_buffer_desc maj_msg, min_msg;
	int msg_ctx = 0;
	char msgbuff[1024];

	gss_display_status(&min, maj_stat, GSS_C_GSS_CODE, GSS_C_NULL_OID,
			   &msg_ctx, &maj_msg);
	gss_display_status(&min, min_stat, GSS_C_MECH_CODE, GSS_C_NULL_OID,
			   &msg_ctx, &min_msg);
	snprintf(msgbuff, sizeof(msgbuff),
		 "rpcsec_gss: %s: (major) %s - (minor) %s\n",
		 m, maj_msg.value, min_msg.value);

	gss_release_buffer(&min, &maj_msg);
	gss_release_buffer(&min, &min_msg);

	authgss_log_err("%s", msgbuff);
}

/* Format error message and log it */
void
authgss_perror(int e, char *m)
{
	char msgbuff[1024];
	char *p;

	if (e == 0 || m == NULL)
		return;

	p = strerror(e);
	snprintf(msgbuff, sizeof(msgbuff), "%s: %s\n", m, p);
	authgss_log_err("%s", msgbuff);
}

void
authgss_log_hexdump(const u_char *buf, int len, int offset)
{
	u_int i, j, jm;
	int c;
	char msgbuff[256];
	char *m = msgbuff;
	int written;

	for (i = 0; i < len; i += 0x10) {
		written = sprintf(m, "  %04x: ", (u_int)(i + offset));
		m += written;
		jm = len - i;
		jm = jm > 16 ? 16 : jm;

		for (j = 0; j < jm; j++) {
			if ((j % 2) == 1)
				written = sprintf(m, "%02x ", (u_int) buf[i+j]);
			else
				written = sprintf(m, "%02x", (u_int) buf[i+j]);
			m += written;
		}
		for (; j < 16; j++) {
			if ((j % 2) == 1)
				written = sprintf(m, "   ");
			else
				written = sprintf(m, "  ");
			m += written;
		}
		sprintf(m, " ");
		m++;

		for (j = 0; j < jm; j++) {
			c = buf[i+j];
			c = isprint(c) ? c : '.';
			sprintf(m, "%c", c);
			m++;
		}
		sprintf(m, "\n");

		authgss_log_dbg("%s", msgbuff);
		m = msgbuff;
		memset(msgbuff, '\0', sizeof(msgbuff));
	}
}
