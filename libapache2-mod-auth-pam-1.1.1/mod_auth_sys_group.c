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

/* $Id: mod_auth_sys_group.c,v 1.1.2.2 2002/08/31 11:09:52 netd Exp $
 * mod_auth_sys_group for apache 2.0
 *
 * v2.0-11 from 24-Aug-02 (APACHE 2.0)
 * 
 * mod_auth_sys_group:
 * 	'require group' functionality against system group database
 *
 * This code was originally part of mod_auth_pam:
 * Written by Ingo Luetkebohle, based upon mod_auth.c, with contributions 
 * from Fredrik Ohrn (group performance), Dirk-Willem van Gulik (licensing
 * consultation) and Michael Johnson (example group implementation).
 * 
 * Changes:
 *   24-Aug-02: Renamed to 'sys_group' because it will check using
 *              the systems getgrent functions. Depending on NSS setup,
 *              those might use something different from /etc/group
 *		Ported supplemental group check fix from 1.3er version
 *   09-Jul-02: Group part split off from mod_auth_pam 2.02 (Dirk-Willem 
 *	        van Gulik, <dirkx@covalent.net>).
 *
 * usage information:
 *
 * new-style (DSO and apache 2.0)
 * 
 *	compile with 
 *		apxs -c mod_auth_sys_group.c
 *
 *		apxs -c mod_auth_sys_group.c
 *
 * 	install with
 *		apxs -i -a mod_auth_sys_group.so
 *
 * Directives
 *
 * AuthGROUP_Enabled on|off       
 *                              If on, mod_auth_sys_group will try to verify
 *				any 'require group <list of groups>' against
 *				the /etc/group file (and also check that 
 *				the user is in /etc/passwd to avoid surpizes).
 *
 *				If off, mod_auth_sys_group will DECLINE immediately.
 *
 *				This will make Apache try other modules.
 *
 *				Defaults to on
 *
 * AuthGROUP_FallThrough on|off
 *
 *				If on, makes mod_auth_sys_group DECLINE if it can't
 *				find the username in the group as pulled from /etc/group
 *			  	or if it cannot find the group.
 *
 *				Please note that, if it DOES find the group, and
 *				the user is NOT in it; it will deny acces.
 *
 *				Defaults to off
 *
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

#define VERSION "2.0-1.1"

module auth_sys_group_module;

/*
 * configuration directive handling
 */

typedef struct {
  int
    fall_through, 	/* 1 to DECLINE instead of HTTP_UNAUTHORIZEDif we
			   can't find the username in a group. 
			   (default to 0) */
    enabled;		/* 1 to use mod_auth_sys_group, 0 otherwise
		   	  (defaults to 1) */
} auth_sys_group_dir_config;

static
int auth_sys_group_init(
	apr_pool_t *p,
	apr_pool_t *plog,
	apr_pool_t *ptemp,
	server_rec *s
)
{
  ap_log_error(APLOG_MARK, APLOG_INFO, 0, s,"GROUP: mod_auth_sys_group/" VERSION);

  return OK;
}

static
void* create_auth_sys_group_dir_config(apr_pool_t *p, char *dummy)
{
  auth_sys_group_dir_config *new =
    (auth_sys_group_dir_config*) apr_palloc (p, sizeof(auth_sys_group_dir_config));

  new->fall_through   = 0; /* off */
  new->enabled	      = 1; /* on */
  return new;
}

static command_rec auth_sys_group_cmds[] = {

  AP_INIT_FLAG("AuthGROUP_FallThrough", 
	ap_set_flag_slot, 
     (void *)APR_OFFSETOF(auth_sys_group_dir_config, fall_through), 
	OR_AUTHCFG,
    "on|off - determines if other authentication methods are attempted "
    "if this one fails; (default is off.)"),

  AP_INIT_FLAG("AuthGROUP_Enabled", 
	ap_set_flag_slot, 
     (void *)APR_OFFSETOF(auth_sys_group_dir_config, enabled), 
	OR_AUTHCFG,
    "on|off - determines if GROUP authentication is enabled; ("
    "default is on.)" ),

  { NULL }
};

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



static
int sys_group_check_auth (request_rec *r)
{
  register int i = 0;
  char method_restricted = 0, *line = 0;
  auth_sys_group_dir_config *conf = 
	(auth_sys_group_dir_config*) ap_get_module_config(
		r->per_dir_config, &auth_sys_group_module);
  struct passwd *pwent;
  
  /* check for allowed users/group */
  const apr_array_header_t *reqs_arr = ap_requires (r);
  require_line *reqs = 0;

  /* enabled? */
  if (!conf->enabled)
    return DECLINED;

  /* retrieve user info from passwd and use that info instead of
   * server supplied info */
  pwent = getpwnam(r->user);
  if(pwent == NULL)
    return DECLINED;

  /* if any valid user suffices return success */
  if (!reqs_arr)
    return (OK);

  /* otherwise */
  reqs = (require_line*)reqs_arr->elts;

  /*  loop over requirement lines */
  for( i = 0; i < reqs_arr->nelts; i++) {
    /* if method of this requirement matches current method */
    if (reqs[i].method_mask & (1 << r->method_number)) {
      char* type;
      method_restricted = 1;

      line = reqs[i].requirement;
      type = ap_getword(r->pool, (const char**)&line, ' ');

      if(!strcmp(type, "group") && (r->user)) {
	struct group *grent;
	char **members;

	while (*line) {
	  char* groupname = ap_getword_conf(r->pool, (const char**)&line);
	   
	  if((grent = getgrnam(groupname)) && grent->gr_mem) {
	    members = grent->gr_mem;

	    /* maybe its the primary group? saves the comparisons */
	    if(pwent->pw_gid == grent->gr_gid)
	      return OK;
			
	    while (*members) {
	      if (strcmp (*members, pwent->pw_name) == 0)
		  return OK;

	      members ++;
	    }
	  }
	}
      } /* end if group */
    } /* method mask */
  }

  if (!method_restricted)
    return OK;

  ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "GROUP: %s not in required group(s).",r->user);

  ap_note_basic_auth_failure (r);
  return HTTP_UNAUTHORIZED;
}

static void sys_group_register_hooks(apr_pool_t *p)
{  
    ap_hook_post_config(auth_sys_group_init, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_auth_checker(sys_group_check_auth,NULL,NULL,APR_HOOK_MIDDLE);

}   

module AP_MODULE_DECLARE_DATA auth_sys_group_module = {
  STANDARD20_MODULE_STUFF,
  create_auth_sys_group_dir_config,  /* dir config creater */
  NULL,                        /* dir merger --- default is to override */
  NULL,                        /* server config */
  NULL,                        /* merge server config */
  auth_sys_group_cmds,               /* command table */
  sys_group_register_hooks,  	       /* register hooks */
};

