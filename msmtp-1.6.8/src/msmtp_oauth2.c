/*
 * OAUTH2 authentication module for MSMTP
 *
 * This overrides SASL.
 *
 * Copyright 2017 Hiro Sugawara <hiro.sugawara@netgear.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

# include "base64.h"

#include "gettext.h"
#define _(string) gettext(string)

#include "xalloc.h"
#include "list.h"
#include "readbuf.h"
#include "net.h"
#include "smtp.h"
#include "stream.h"

#include "base64.c"

#include "oauth2.h"

extern
int smtp_send_cmd(smtp_server_t *srv, char **errstr, const char *format, ...);
extern
int smtp_get_msg(smtp_server_t *srv, list_t **msg, char **errstr);

int smtp_auth_oauth2(smtp_server_t *srv, const char *user,
        const char *password,
        list_t **error_msg, char **errstr)
{
    char *ubuf = NULL;
    list_t *msg = NULL;
    int e = SMTP_EOK;

    char *token = oauth2_access_token(password, errstr);
    if (!token)
    {
	   return SMTP_EUNAVAIL;
    }

    int u_len = asprintf(&ubuf, "user=%s%cauth=Bearer %s%c%c",
			user, 01, token, 01, 01);
    free(token);
    if (u_len <= 0)
        return SMTP_ELIBFAILED;

    size_t b64_len = BASE64_LENGTH(u_len);
    char *b64 = xmalloc(strlen("AUTH XOAUTH2 ") + b64_len + 1);
    if (!b64)
        return SMTP_ELIBFAILED;

    strcpy(b64, "AUTH XOAUTH2 ");
    base64_encode(ubuf, u_len, b64 + strlen(b64), b64_len + 1);

    *error_msg = NULL;
    e = smtp_send_cmd(srv, errstr, b64);
    free(b64);
    free(ubuf);

    if (e != SMTP_EOK)
    {
        return e;
    }

    if ((e = smtp_get_msg(srv, &msg, errstr)) != SMTP_EOK)
    {
        return e;
    }

    if (smtp_msg_status(msg) != 235) {
        *error_msg = msg;
        *errstr = xasprintf(_("authentication failed (method %s)"), "XOAUTH2");
        return SMTP_EAUTHFAIL;
    }
    list_xfree(msg, free);

    return SMTP_EOK;
}
