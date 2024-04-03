/* SASL server API implementation
 * Tim Martin
 */
/* 
 * Copyright (c) 2000 Carnegie Mellon University.  All rights reserved.
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
 * 3. The name "Carnegie Mellon University" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For permission or any other legal
 *    details, please contact  
 *      Office of Technology Transfer
 *      Carnegie Mellon University
 *      5000 Forbes Avenue
 *      Pittsburgh, PA  15213-3890
 *      (412) 268-4387, fax: (412) 268-7395
 *      tech-transfer@andrew.cmu.edu
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Computing Services
 *     at Carnegie Mellon University (http://www.cmu.edu/computing/)."
 *
 * CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <sasl.h>
#include <saslutil.h>
#include "saslint.h"

static int
external_client_new(void *glob_context __attribute__((unused)),
		    sasl_client_params_t *params __attribute__((unused)),
		    void **conn_context)
{
  if (!params
      || !params->utils
      || !params->utils->conn
      || !conn_context)
    return SASL_BADPARAM;
  if (!params->utils->conn->external.auth_id)
    return SASL_NOMECH;
  *conn_context = NULL;
  return SASL_OK;
}

static int
external_client_step(void *conn_context __attribute__((unused)),
		     sasl_client_params_t *params,
		     const char *serverin __attribute__((unused)),
		     int serverinlen,
		     sasl_interact_t **prompt_need,
		     char **clientout,
		     int *clientoutlen,
		     sasl_out_params_t *oparams)
{
  int result;
  sasl_getsimple_t *getuser_cb;
  void *getuser_context;
  const char *user;
  unsigned len;

  if (!params
      || !params->utils
      || !params->utils->conn
      || !params->utils->getcallback
      || !clientout
      || !clientoutlen
      || !oparams)
    return SASL_BADPARAM;

  if (!params->utils->conn->external.auth_id)
    return SASL_BADPROT;

  if (serverinlen != 0)
    return SASL_BADPROT;

  if (prompt_need && *prompt_need) {
    /* Second time through; we used a SASL_INTERACT to get the user. */
    if (! (*prompt_need)[0].result)
      return SASL_BADPARAM;
    user = (*prompt_need)[0].result;
    *clientoutlen = strlen(user);
    params->utils->free(*prompt_need);
    *prompt_need = NULL;
  } else {
    /* We need to get the user. */
    result = params->utils->getcallback(params->utils->conn,
					SASL_CB_USER,
					&getuser_cb,
					&getuser_context);
    switch (result) {
    case SASL_INTERACT:
      /* Set up the interaction... */
      if (prompt_need) {
	*prompt_need = params->utils->malloc(sizeof(sasl_interact_t) * 2);
	if (! *prompt_need)
	  return SASL_FAIL;
	memset(*prompt_need, 0, sizeof(sasl_interact_t) * 2);
	(*prompt_need)[0].id = SASL_CB_USER;
	(*prompt_need)[0].prompt = "Authorization Identity";
	(*prompt_need)[0].defresult = "";
	(*prompt_need)[1].id = SASL_CB_LIST_END;
      }
      return SASL_INTERACT;
    case SASL_OK:
      if (getuser_cb &&
	  (getuser_cb(getuser_context,
		      SASL_CB_USER,
		      &user,
		      &len)
	   == SASL_OK)) {
	*clientoutlen = strlen(user);
	break;
      }
      /* Otherwise, drop through into the default we-lose case. */
    default:
      /* Assume userid == authid. */
      user = NULL;
      *clientoutlen = 0;
    }
  }

  *clientout = params->utils->malloc(*clientoutlen + 1);
  if (! clientout)
    return SASL_FAIL;
  if (user)
    memcpy(*clientout, user, *clientoutlen);
  (*clientout)[*clientoutlen] = '\0';
  
  if (prompt_need)
    *prompt_need = NULL;

  result = _sasl_strdup(params->utils->conn->external.auth_id,
			&oparams->authid,
			NULL);
  if (result == SASL_OK)
  {
    oparams->doneflag = 1;
    oparams->mech_ssf = 0;
    oparams->maxoutbuf = 0;
    oparams->encode_context = NULL;
    oparams->encode = NULL;
    oparams->getmic = NULL;
    oparams->decode_context = NULL;
    oparams->decode = NULL;
    oparams->verifymic = NULL;
    oparams->user = NULL;
    oparams->realm = NULL;
    oparams->param_version = 0;

    return SASL_OK;

  }  /* otherwise */

  sasl_FREE(*clientout);
  *clientout = NULL;

  return result;
}

static const sasl_client_plug_t external_client_mech = {
  "EXTERNAL",			/* mech_name */
  0,				/* max_ssf */
  SASL_SEC_NOPLAINTEXT
  | SASL_SEC_NODICTIONARY,	/* security_flags */
  NULL,				/* required_prompts */
  NULL,				/* glob_context */
  &external_client_new,		/* mech_new */
  &external_client_step,	/* mech_step */
  NULL,				/* mech_dispose */
  NULL,				/* mech_free */
  NULL,				/* auth_create */
  NULL				/* idle */
};

static int
external_client_init(sasl_utils_t *utils,
		     int max_version,
		     int *out_version,
		     const sasl_client_plug_t **pluglist,
		     int *plugcount)
{
  if (!utils || !out_version || !pluglist || !plugcount)
    return SASL_BADPARAM;
  if (max_version != SASL_CLIENT_PLUG_VERSION)
    return SASL_BADVERS;
  *out_version = SASL_CLIENT_PLUG_VERSION;
  *pluglist = &external_client_mech;
  *plugcount = 1;
  return SASL_OK;
}

typedef struct cmechanism
{
  int version;
  const sasl_client_plug_t *plug;
  void *library;

  struct cmechanism *next;  
} cmechanism_t;

typedef struct sasl_client_conn {
  sasl_conn_t base; /* parts common to server + client */

  cmechanism_t *mech;
  sasl_client_params_t *cparams;

  char *serverFQDN;

} sasl_client_conn_t;

typedef struct cmech_list {
  sasl_utils_t *utils; 

  void *mutex;            /* mutex for this data */ 
  cmechanism_t *mech_list; /* list of mechanisms */
  int mech_length;       /* number of mechanisms */

} cmech_list_t;


static cmech_list_t *cmechlist; /* global var which holds the list */

static sasl_global_callbacks_t global_callbacks;

static int init_mechlist()
{
  cmechlist->utils=_sasl_alloc_utils(NULL, &global_callbacks);
  if (cmechlist->utils==NULL)
    return SASL_NOMEM;

  return SASL_OK;
}

static void client_done(void) {
  cmechanism_t *cm;
  cmechanism_t *cprevm;

  cm=cmechlist->mech_list; /* m point to begging of the list */

  while (cm!=NULL)
  {
    cprevm=cm;
    cm=cm->next;
    if (cprevm->library!=NULL)
      _sasl_done_with_plugin(cprevm->library);
    sasl_FREE(cprevm);    
  }
  sasl_MUTEX_DISPOSE(cmechlist->mutex);
  _sasl_free_utils(&cmechlist->utils);
  sasl_FREE(cmechlist);
}

static int add_plugin(void *p, void *library) {
  int plugcount;
  const sasl_client_plug_t *pluglist;
  cmechanism_t *mech;
  sasl_client_plug_init_t *entry_point;
  int result;
  int version;
  int lupe;

  entry_point = (sasl_client_plug_init_t *)p;

  result = entry_point(cmechlist->utils, SASL_CLIENT_PLUG_VERSION, &version,
		       &pluglist, &plugcount);
  if (version != SASL_CLIENT_PLUG_VERSION)
  {
    VL(("Version conflict\n"));
    result = SASL_FAIL;
  }
  if (result != SASL_OK)
  {
    VL(("entry_point failed\n"));
    return result;
  }

  for (lupe=0;lupe< plugcount ;lupe++)
    {
      mech = sasl_ALLOC(sizeof(cmechanism_t));
      if (! mech) return SASL_NOMEM;

      mech->plug=pluglist++;
      if (lupe==0)
	mech->library = library;
      else
	mech->library = NULL;
      mech->version = version;
      mech->next = cmechlist->mech_list;
      cmechlist->mech_list = mech;
      cmechlist->mech_length++;
    }

  return SASL_OK;
}

static int
client_idle(sasl_conn_t *conn)
{
  cmechanism_t *m;
  if (! cmechlist)
    return 0;

  for (m = cmechlist->mech_list;
       m;
       m = m->next)
    if (m->plug->idle
	&&  m->plug->idle(m->plug->glob_context,
			  conn,
			  conn ? ((sasl_client_conn_t *)conn)->cparams : NULL))
      return 1;
  return 0;
}

/* initialize the SASL client drivers
 *  callbacks      -- base callbacks for all client connections
 * returns:
 *  SASL_OK        -- Success
 *  SASL_NOMEM     -- Not enough memory
 *  SASL_BADVERS   -- Mechanism version mismatch
 *  SASL_BADPARAM  -- error in config file
 *  SASL_NOMECH    -- No mechanisms available
 *  ...
 */

int sasl_client_init(const sasl_callback_t *callbacks)
{
  int ret;

  _sasl_allocation_locked++;

  _sasl_client_cleanup_hook = &client_done;
  _sasl_client_idle_hook = &client_idle;

  global_callbacks.callbacks = callbacks;
  global_callbacks.appname = NULL;

  cmechlist=sasl_ALLOC(sizeof(cmech_list_t));
  if (cmechlist==NULL) return SASL_NOMEM;

  /* create mutex*/
  cmechlist->mutex=sasl_MUTEX_NEW();

  /* load plugins */
  ret=init_mechlist();  
  if (ret!=SASL_OK)
    return ret;

  cmechlist->mech_list=NULL;
  cmechlist->mech_length=0;

  add_plugin((void *) &external_client_init, NULL);

  ret = _sasl_common_init();

  if (ret == SASL_OK)
      ret=_sasl_get_mech_list("sasl_client_plug_init",
			      _sasl_find_getpath_callback(callbacks),
			      _sasl_find_verifyfile_callback(callbacks),
			      &add_plugin);
  
  return ret;
}

static void client_dispose(sasl_conn_t *pconn)
{
  sasl_client_conn_t *c_conn=(sasl_client_conn_t *) pconn;

  if (c_conn->mech && c_conn->mech->plug->mech_dispose)
    c_conn->mech->plug->mech_dispose(c_conn->base.context,
				     c_conn->cparams->utils);

  _sasl_free_utils(&c_conn->cparams->utils);

  if (c_conn->serverFQDN!=NULL)
    sasl_FREE(c_conn->serverFQDN);

  sasl_FREE(c_conn->cparams);

  _sasl_conn_dispose(pconn);
}

/* initialize a client exchange based on the specified mechanism
 *  service       -- registered name of the service using SASL (e.g. "imap")
 *  serverFQDN    -- the fully qualified domain name of the server
 *  prompt_supp   -- list of client interactions supported
 *                   may also include sasl_getopt_t context & call
 *                   NULL prompt_supp = user/pass via SASL_INTERACT only
 *                   NULL proc = interaction supported via SASL_INTERACT
 *  secflags      -- security flags (see above)
 * in/out:
 *  pconn         -- connection negotiation structure
 *                   pointer to NULL => allocate new
 *                   non-NULL => recycle storage and go for next available mech
 *
 * Returns:
 *  SASL_OK       -- success
 *  SASL_NOMECH   -- no mechanism meets requested properties
 *  SASL_NOMEM    -- not enough memory
 */

int sasl_client_new(const char *service,
		    const char *serverFQDN,
		    const sasl_callback_t *prompt_supp,
		    int secflags,
		    sasl_conn_t **pconn)
{
  int result;
  sasl_client_conn_t *conn;
  if (!pconn || !service || !serverFQDN)
    return SASL_BADPARAM;

  *pconn=sasl_ALLOC(sizeof(sasl_client_conn_t));
  if (*pconn==NULL) return SASL_NOMEM;

  (*pconn)->destroy_conn = &client_dispose;
  result = _sasl_conn_init(*pconn, service, secflags,
			   &client_idle, NULL,
			   prompt_supp, &global_callbacks);
  if (result != SASL_OK) return result;
  
  conn = (sasl_client_conn_t *)*pconn;

  conn->mech = NULL;

  conn->cparams=sasl_ALLOC(sizeof(sasl_client_params_t));
  if (conn->cparams==NULL) return SASL_NOMEM;

  conn->cparams->utils=_sasl_alloc_utils(*pconn, &global_callbacks);
  if (conn->cparams->utils==NULL)
    return SASL_NOMEM;

  conn->cparams->utils->conn= *pconn;

  result = _sasl_strdup(serverFQDN, &conn->serverFQDN, NULL);

  if (result == SASL_OK) return SASL_OK;

  /* result isn't SASL_OK */

  _sasl_conn_dispose(*pconn);
  sasl_FREE(*pconn);
  *pconn = NULL;
  return result;
}

static int have_prompts(sasl_conn_t *conn,
			const sasl_client_plug_t *mech)
{
  static const long default_prompts[] = {
    SASL_CB_AUTHNAME,
    SASL_CB_PASS,
    SASL_CB_LIST_END
  };

  const long *prompt;
  int (*pproc)();
  void *pcontext;
  int result;

  for (prompt = (mech->required_prompts
		 ? mech->required_prompts :
		 default_prompts);
       *prompt != SASL_CB_LIST_END;
       prompt++) {
    result = _sasl_getcallback(conn, *prompt, &pproc, &pcontext);
    if (result != SASL_OK && result != SASL_INTERACT)
      return 0;			/* we don't have this required prompt */
  }

  return 1; /* we have all the prompts */
}

/* select a mechanism for a connection
 *  mechlist      -- mechanisms server has available (punctuation ignored)
 *  secret        -- optional secret from previous session
 * output:
 *  prompt_need   -- on SASL_INTERACT, list of prompts needed to continue
 *  clientout     -- the initial client response to send to the server
 *  mech          -- set to mechanism name
 *
 * Returns:
 *  SASL_OK       -- success
 *  SASL_NOMEM    -- not enough memory
 *  SASL_NOMECH   -- no mechanism meets requested properties
 *  SASL_INTERACT -- user interaction needed to fill in prompt_need list
 */

/* xxx confirm this with rfc 2222
 * SASL mechanism allowable characters are "AZaz-_"
 * seperators can be any other characters and of any length
 * even variable lengths between
 *
 * Apps should be encouraged to simply use space or comma space
 * though
 */

int sasl_client_start(sasl_conn_t *conn,
		      const char *list,
		      sasl_secret_t *secret,
		      sasl_interact_t **prompt_need,
		      char **clientout,
		      unsigned *clientoutlen,
		      const char **mech)
{
    sasl_client_conn_t *c_conn= (sasl_client_conn_t *) conn;
    char name[SASL_MECHNAMEMAX + 1];
    cmechanism_t *m=NULL,*bestm=NULL;
    size_t pos=0,place;
    size_t list_len;
    sasl_ssf_t bestssf = 0, minssf = 0;
    int result;

    /* verify parameters */
    if (list == NULL)
    {
	return SASL_BADPARAM;
    }

    VL(("in sasl_client_start\n"));

    /* if prompt_need != NULL we've already been here
       and just need to do the continue step again */

    /* do a step */
    if (prompt_need && *prompt_need != NULL) {
	result = c_conn->mech->plug->mech_step(conn->context,
					       c_conn->cparams,
					       NULL,
					       0,
					       prompt_need,
					       clientout, (int *) clientoutlen,
					       &conn->oparams);

	return result;
    }

    /* set secret */
    conn->secret=secret;

    /* Get app's desired sec props */
    if (conn->props.min_ssf < conn->external.ssf) {
	minssf = 0;
    } else {
	minssf = conn->props.min_ssf - conn->external.ssf;
    }

    /* parse mechlist */
    VL(("mech list from server is %s\n", list));
    list_len = strlen(list);
    while (pos<list_len)
    {
	place=0;
	while ((pos<list_len) && (isalnum((unsigned char)list[pos])
				  || list[pos] == '_'
				  || list[pos] == '-')) {
	    name[place]=list[pos];
	    pos++;
	    place++;
	    if (SASL_MECHNAMEMAX <= place) {
		place--;
		while(pos<list_len && (isalnum((unsigned char)list[pos])
				       || list[pos] == '_'
				       || list[pos] == '-'))
		    pos++;
	    }
	}
	pos++;
	name[place]=0;

	if (! place) continue;

	VL(("Considering mech %s\n",name));

	/* foreach in server list */
	for (m = cmechlist->mech_list; m != NULL; m = m->next) {
	    /* is this the mechanism the server is suggesting? */
	    if (strcasecmp(m->plug->mech_name, name))
		continue; /* no */

	    /* do we have the prompts for it? */
	    if (!have_prompts(conn, m->plug))
		break;

	    /* is it strong enough? */
	    if (minssf > m->plug->max_ssf)
		break;

	    /* does it meet our security properties? */
	    if (((conn->props.security_flags ^ m->plug->security_flags)
		 & conn->props.security_flags) != 0) {
		break;
	    }

#ifdef PREFER_MECH
	    if (strcasecmp(m->plug->mech_name, PREFER_MECH) &&
		bestm && m->plug->max_ssf <= bestssf) {
		/* this mechanism isn't our favorite, and it's no better
		   than what we already have! */
		break;
	    }
#else
	    if (bestm && m->plug->max_ssf <= bestssf) {
		/* this mechanism is no better than what we already have! */
		break;
	    }
#endif

	    VL(("Best mech so far: %s\n", m->plug->mech_name));
	    if (mech) {
		*mech = m->plug->mech_name;
	    }
	    bestssf = m->plug->max_ssf;
	    bestm = m;
	    break;
	}
    }

    if (bestm == NULL) {
	VL(("No worthy mechs found\n"));
	result = SASL_NOMECH;
	goto done;
    }

    /* make cparams */
    c_conn->cparams->serverFQDN = c_conn->serverFQDN; 
    c_conn->cparams->service = conn->service;
    c_conn->cparams->external_ssf = conn->external.ssf;
    c_conn->cparams->props = conn->props;
    c_conn->mech = bestm;

    /* init that plugin */
    c_conn->mech->plug->mech_new(NULL,
				 c_conn->cparams,
				 &(conn->context));

    /* do a step */
    result = c_conn->mech->plug->mech_step(conn->context,
					 c_conn->cparams,
					 NULL,
					 0,
					 prompt_need,
					 clientout, (int *) clientoutlen,
					 &conn->oparams);    
 done:
    return result;
}

/* do a single authentication step.
 *  serverin    -- the server message received by the client, MUST have a NUL
 *                 sentinel, not counted by serverinlen
 * output:
 *  prompt_need -- on SASL_INTERACT, list of prompts needed to continue
 *  clientout   -- the client response to send to the server
 *
 * returns:
 *  SASL_OK        -- success
 *  SASL_INTERACT  -- user interaction needed to fill in prompt_need list
 *  SASL_BADPROT   -- server protocol incorrect/cancelled
 *  SASL_BADSERV   -- server failed mutual auth
 */

int sasl_client_step(sasl_conn_t *conn,
		     const char *serverin,
		     unsigned serverinlen,
		     sasl_interact_t **prompt_need,
		     char **clientout,
		     unsigned *clientoutlen)
{
  sasl_client_conn_t *c_conn= (sasl_client_conn_t *) conn;
  int result;

  /* check parameters */
  if ((serverin==NULL) && (serverinlen>0))
    return SASL_BADPARAM;

  /* do a step */
  result = c_conn->mech->plug->mech_step(conn->context,
					 c_conn->cparams,
					 serverin,
					 serverinlen,
					 prompt_need,
					 clientout, (int *)clientoutlen,
					 &conn->oparams);
  return result;
}

/* Set connection secret based on passphrase
 *  may be used in SASL_CB_PASS callback
 * input:
 *  user          -- username
 *  pass          -- plaintext passphrase with NUL sentinel
 *  passlen       -- 0 = strlen(pass)
 * out:
 *  prompts       -- if non-NULL, SASL_CB_PASS item filled in
 *  keepcopy      -- set to copy of secret if non-NULL
 * returns:
 *  SASL_OK       -- success
 *  SASL_NOMEM    -- failure
 */

int sasl_client_auth(sasl_conn_t *conn __attribute__((unused)),
		     const char *user __attribute__((unused)),
		     const char *pass __attribute__((unused)), 
		     unsigned passlen __attribute__((unused)),
		     sasl_interact_t *prompts __attribute__((unused)),
		     sasl_secret_t **keepcopy __attribute__((unused)))
{
  /* XXX needs to be filled in */
  return SASL_FAIL;
}

/* erase & dispose of a sasl_secret_t
 *  calls free utility last set by sasl_set_alloc
 */

void sasl_free_secret(sasl_secret_t **secret)
{
  size_t lup;

  VL(("trying to free secret\n"));

  if (secret==NULL) return;
  if (*secret==NULL) return;

  /* overwrite the memory */
  for (lup=0;lup<(*secret)->len;lup++)
    (*secret)->data[lup]='\0';

  sasl_FREE(*secret);

  *secret=NULL;
}

