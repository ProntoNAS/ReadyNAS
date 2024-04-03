/* Copyright (c) 2000 Ingo Luetkebohle, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR OTHER CODE CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* $Id: mod_auth_pam.c,v 1.2.2.2 2002/08/24 14:22:23 netd Exp $
 * mod_auth_pam for apache 2.0
 *
 * Based on: v 2.0-1.1 from 07. July 2002 for APACHE 2.0
 *
 *
 * mod_auth_pam:
 *  basic authentication against pluggable authentication module lib
 *
 * The authoritative homepage for this module is at
 *	http://pam.sourceforge.net/
 *
 * For assistance and support, contact the pam-list
 *	https://listman.redhat.com/mailman/listinfo/pam-list
 * or the Help forum on SourceForge.
 *
 * Written by Ingo Luetkebohle, based upon mod_auth.c, with contributions
 * from Fredrik Ohrn (group performance), Dirk-Willem van Gulik (licensing
 * consultation) and Michael Johnson (example group implementation).
 * Ported to Apache 2.0 by Dirk-Willem van Gulik.
 *
 * Thanks to Andrew Morgan and the rest of the Linux-PAM developers who
 * provided invaluable development help and ideas.
 *
 * Changes:
 *   24-Aug-02: Small bugfixes and build setup improved
 *   09-Jul-02: Ported to apache 2.02 (Dirk-Willem van Gulik,
 *              <dirkx@covalent.net>).
 *   07-Jul-02: License changed to allow redistribution
 *		Performance improvement for group lookup
		(many thanks to Fredrik Ohrn for the patch)
 *   06-Dec-99: Special casing for Solaris 2.6 added
 *              Added versioning message to headers
 *   14-Feb-99: Cleaned up the configuration directives and named them
 *		in a more straightforward way
 *
 *		incorporated getugroups patch from Klaus Wissmann <kw@aw.net>
 *		to look up supplementary groups from /etc/group
 *
 *   22-Jan-99: incorporated changes from Pavel Kankovsky <kan@dcit.cz>
 *		Enabler configuration directive now called AuthPAM_Enabled
 *		Updated for Apache 1.3.x
 *   25-Sep-98: replaced pwdb groups routine with standard C
 *   19-Oct-97: made module fall through (if configured to do so),
 * 		even when a user of the given name is found in the PAM
 *		databases
 *   17-Apr-97: fixed segfault that occured when Apache couldn't look
 *              up the remote host name
 *              removed annoying compiler warnings
 *
 *   05-Apr-97: made fall-through configurable with AuthPAM_Authorative
 *
 *   25-Mar-97: added support for transparent fall-through to other auth
 *              modules with configurable fail delays
 *              added acct_mgmt hook
 *              added group support (through libpwdb)
 *
 * usage information:
 *
 * new-style (DSO and apache 2.0)
 *
 *	compile with
 *		apxs -c -lpam mod_auth_pam.c
 *
 *		apxs -I/usr/local/include -L/usr/local/lib -c -lpam mod_auth_pam.c
 *
 * 	install with
 *		apxs -i -a mod_auth_pam.so
 *
 *	configure PAM by adding
 *		/etc/pam.d/httpd
 *
 *	with the appropriate pam modules (for starters, just copy over ftp)
 *
 *
 * configuration directives:
 * AuthFailDelay <msecs>
 *                              number of mili-seconds to wait after a
 *                              failed authentication attempt. this is
 *                              a minimum value and may have been
 *                              increased by other pam apps.
 *                              defaults to 0
 *				REQUIRES lib_pam SUPPORT
 *
 * AuthPAM_Enabled on|off
 *                              If on, mod_auth_pam will try to authenticate
 *				the user.
 *				If off, mod_auth_pam will DECLINE immediately
 *				instead of trying to authenticate the user.
 *				This will make Apache try other modules.
 *				Defaults to on
 *
 * AuthPAM_FallThrough
 *				If on, makes mod_auth_pam DECLINE if it can't
 *				the username, giving other modules a chance.
 *				Please note that, if it DOES find the username,
 *				and the password doesn't match, it will NOT
 *				fall through but return "access denied" instead
 *				Defaults to off
 *
 * AuthPAM_Authorative on|off   DEPRECATED
 */

#include <sys/types.h>
#include <pwd.h>		/* for getpwnam et.al. */
#include <grp.h>		/* for getpwnam et.al. */
#include <unistd.h>

#include "ap_config.h"
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

#include <security/pam_appl.h>

/* change this to 0 on RedHat 4.x */
#define PAM_STRE_NEEDS_PAMH 1
#define VERSION "2.0-1.1"

module auth_pam_module;

static const char
	*pam_servicename = "httpd",
	*valid_user 	 = "valid-user";

typedef struct {
    char *name, *pw;
}      auth_pam_userinfo;

/*
 * Solaris 2.6.x has a broken conversation function and needs this
 * as a global variable
 * I refused to pollute code for other platforms with this,
 * so all Solaris 2.6 specific stuff is if'd like the following
 */
#if SOLARIS2 == 260
auth_pam_userinfo *global_userinfo;
#endif

/*
 * the pam_strerror function has different parameters in early PAM
 * versions
 */
#ifndef PAM_STRE_NEEDS_PAMH
#define compat_pam_strerror(pamh, res) pam_strerror(res)
#else
#define compat_pam_strerror(pamh, res) pam_strerror(pamh, res)
#endif

/*
 * configuration directive handling
 */

typedef struct {
    int
        fail_delay,		/* fail delay in ms -- needs library support */
        fall_through,		/* 1 to DECLINE instead of
				 * HTTP_UNAUTHORIZEDif we can't find the
				 * username (defaults to 0) */
        enabled;		/* 1 to use mod_auth_pam, 0 otherwise
				 * (defaults to 1) */
}      auth_pam_dir_config;

static
int auth_pam_init(
		      apr_pool_t * p,
		      apr_pool_t * plog,
		      apr_pool_t * ptemp,
		      server_rec * s
)
{
    ap_log_error(APLOG_MARK, APLOG_INFO, 0, s, "PAM: mod_auth_pam/" VERSION);

    return OK;
}

static
void *create_auth_pam_dir_config(apr_pool_t * p, char *dummy)
{
    auth_pam_dir_config *new =
    (auth_pam_dir_config *) apr_palloc(p, sizeof(auth_pam_dir_config));

    new->fail_delay = 0;	/* 0 ms */
    new->fall_through = 0;	/* off */
    new->enabled = 1;		/* on */
    return new;
}

static command_rec auth_pam_cmds[] = {

    AP_INIT_TAKE1("AuthPAM_FailDelay",
	ap_set_int_slot, (void *) APR_OFFSETOF(auth_pam_dir_config, fail_delay),
	OR_AUTHCFG,
	"number of micro seconds to wait after failed authentication "
	"attempt. (default is 0.)"),

    AP_INIT_FLAG("AuthPAM_FallThrough",
	 ap_set_flag_slot, (void *) APR_OFFSETOF(auth_pam_dir_config, fall_through),
	 OR_AUTHCFG,
	"on|off - determines if other authentication methods are attempted "
	 "if this one fails; (default is off.)"),

    AP_INIT_FLAG("AuthPAM_Enabled",
	 ap_set_flag_slot, (void *) APR_OFFSETOF(auth_pam_dir_config, enabled),
	 OR_AUTHCFG,
	 "on|off - determines if PAM authentication is enabled. "
	 "(default is on.)"),

    {NULL}
};

/*
 * auth_pam_talker: supply authentication information to PAM when asked
 *
 * Assumptions:
 *   A password is asked for by requesting input without echoing
 *   A username is asked for by requesting input _with_ echoing
 *
 */
static
int auth_pam_talker(int num_msg,
		        const struct pam_message ** msg,
		        struct pam_response ** resp,
		        void *appdata_ptr)
{
    unsigned short i = 0;
    auth_pam_userinfo *userinfo = (auth_pam_userinfo *) appdata_ptr;
    struct pam_response *response = 0;

#if SOLARIS2 == 260
    if (!userinfo)
	userinfo = global_userinfo;
    /* fprintf(stderr,"%s : %s", userinfo->name, userinfo->pw); */
#endif

    /* parameter sanity checking */
    if (!resp || !msg || !userinfo)
	return PAM_CONV_ERR;

    /* allocate memory to store response */
    response = malloc(num_msg * sizeof(struct pam_response));
    if (!response)
	return PAM_CONV_ERR;

    /* copy values */
    for (i = 0; i < num_msg; i++) {
	/* initialize to safe values */
	response[i].resp_retcode = 0;
	response[i].resp = 0;

	/* select response based on requested output style */
	switch (msg[i]->msg_style) {
	case PAM_PROMPT_ECHO_ON:
	    /* on memory allocation failure, auth fails */
	    response[i].resp = strdup(userinfo->name);
	    break;
	case PAM_PROMPT_ECHO_OFF:
	    response[i].resp = strdup(userinfo->pw);
	    break;
	default:
	    if (response)
		free(response);
	    return PAM_CONV_ERR;
	}
    }
    /* everything okay, set PAM response values */
    *resp = response;
    return PAM_SUCCESS;
}

/*
 * These functions return 0 if client is OK, and proper error status
 * if not... either AUTH_REQUIRED, if we made a check, and it failed, or
 * SERVER_ERROR, if things are so totally confused that we couldn't
 * figure out how to tell if the client is authorized or not.
 *
 * If they return DECLINED, and all other modules also decline, that's
 * treated by the server core as a configuration error, logged and
 * reported as such.
 */

/*
 * Determine user ID, and check if it really is that user
 */
static
int pam_auth_basic_user(request_rec * r)
{
    int res = 0;
    /* mod_auth_pam specific */
    auth_pam_userinfo userinfo = {NULL, NULL};
    auth_pam_dir_config *conf = (auth_pam_dir_config *)
    ap_get_module_config(r->per_dir_config, &auth_pam_module);
    /* PAM specific  */
    struct pam_conv conv_info = {&auth_pam_talker, (void *) &userinfo};
    pam_handle_t *pamh = NULL;

#if SOLARIS2 == 260
    global_userinfo = &userinfo;
#endif

    /* enabled? */
    if (!conf->enabled)
	return DECLINED;

    /* read sent pw */
    if ((res = ap_get_basic_auth_pw(r, (const char **) &(userinfo.pw))))
	return res;

    /* this is only set after get_basic_auth_pw was called */
    userinfo.name = r->user;

    /* initialize pam */
    if ((res = pam_start(pam_servicename,
			 userinfo.name,
			 &conv_info,
			 &pamh)) != PAM_SUCCESS) {

	ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
		      "PAM: Could not start pam service: %s",
		      compat_pam_strerror(pamh, res));
	return HTTP_INTERNAL_SERVER_ERROR;
    }

    /* set fail delay */
#ifdef PAM_FAIL_DELAY
    pam_fail_delay(pamh, conf->fail_delay);
#endif

    /* set remote user information */
    /*
     * this seems to cause segfaults in lots of cases -- disabled for now
     * pam_set_item(pamh, PAM_USER, userinfo.name); pam_set_item(pamh,
     * PAM_RHOST, get_remote_host(r->connection, conf, REMOTE_NAME));
     */

    /* try to authenticate user, log error on failure */
    if ((res = pam_authenticate(pamh, PAM_DISALLOW_NULL_AUTHTOK)) !=
	PAM_SUCCESS) {
	ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
		      "PAM: user '%s' - not authenticated: %s", r->user, compat_pam_strerror(pamh, res));

	if (conf->fall_through && (res == PAM_USER_UNKNOWN)) {
	    /* we don't know about the user, but other auth modules might do */
	    pam_end(pamh, PAM_SUCCESS);
	    return DECLINED;
	}
	else {
	    pam_end(pamh, PAM_SUCCESS);
	    ap_note_basic_auth_failure(r);
	    return HTTP_UNAUTHORIZED;
	}			/* endif fall_through */
    }				/* endif authenticate */

    /* check that the account is healthy */
    if ((res = pam_acct_mgmt(pamh, PAM_DISALLOW_NULL_AUTHTOK)) != PAM_SUCCESS) {
	ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
		      "PAM: user '%s'  - invalid account: %s", r->user, compat_pam_strerror(pamh, res));
	pam_end(pamh, PAM_SUCCESS);
	return HTTP_UNAUTHORIZED;
    }

    pam_end(pamh, PAM_SUCCESS);
    return OK;
}

static void pam_register_hooks(apr_pool_t * p)
{
    ap_hook_post_config(auth_pam_init, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_check_user_id(pam_auth_basic_user, NULL, NULL, APR_HOOK_MIDDLE);
}

module AP_MODULE_DECLARE_DATA auth_pam_module = {
    STANDARD20_MODULE_STUFF,
    create_auth_pam_dir_config,	/* dir config creater */
    NULL,			/* dir merger --- default is to override */
    NULL,			/* server config */
    NULL,			/* merge server config */
    auth_pam_cmds,		/* command table */
    pam_register_hooks,		/* register hooks */
};
