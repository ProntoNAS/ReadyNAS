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


/* checkpw stuff */

#include <config.h>
#include <sasl.h>
#include <saslint.h>
#include <saslutil.h>
#include <assert.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_KRB
#include <krb.h>
#endif

#include <stdlib.h>

#ifndef WIN32
#include <strings.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/un.h>
#else
#include <string.h>
#endif

#include <sys/types.h>
#include <ctype.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif /* HAVE_PWD_H */
#ifdef HAVE_CRYPT_H
#ifndef HAVE_KRB
#include <crypt.h>
#endif /* HAVE_KRB */
#endif /* HAVE_CRYPT_H */
#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif /* HAVE_SHADOW_H */

#ifdef HAVE_PAM
#include <security/pam_appl.h>
#endif

#if defined(HAVE_PWCHECK) || defined(HAVE_SASLAUTHD)
# include <errno.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

extern int errno;
#endif

#ifdef HAVE_KRB

/* This defines the Andrew string_to_key function.  It accepts a
 * password string as input and converts its via a one-way encryption
 * algorithm to a DES encryption key.  It is compatible with the
 * original Andrew authentication service password database.  
 */

static int
afs_cmu_StringToKey (str, cell, key)
char *str;
char *cell;                  /* cell for password */
des_cblock *key;
{   char  password[8+1];              /* crypt is limited to 8 chars anyway */
    int   i;
    int   passlen;

    memset(key, 0, sizeof(des_cblock));
    memset((void *)password, 0, sizeof(password));

    strncpy (password, cell, 8);
    passlen = strlen (str);
    if (passlen > 8) passlen = 8;

    for (i=0; i<passlen; i++)
        password[i] = str[i] ^ cell[i];

    for (i=0;i<8;i++)
        if (password[i] == '\0') password[i] = 'X';

    /* crypt only considers the first 8 characters of password but for some
       reason returns eleven characters of result (plus the two salt chars). */
    strncpy((void *)key,(const char *) (crypt(password, "p1") + 2), sizeof(des_cblock));

    /* parity is inserted into the LSB so leftshift each byte up one bit.  This
       allows ascii characters with a zero MSB to retain as much significance
       as possible. */
    {   char *keybytes = (char *)key;
        unsigned int temp;

        for (i = 0; i < 8; i++) {
            temp = (unsigned int) keybytes[i];
            keybytes[i] = (unsigned char) (temp << 1);
        }
    }
    des_fixup_key_parity (key);

    return SASL_OK;
}

static int
afs_transarc_StringToKey (str, cell, key)
char *str;
char *cell;                  /* cell for password */
des_cblock *key;
{   des_key_schedule schedule;
    des_cblock temp_key;
    des_cblock ivec;
    char password[BUFSIZ];
    int  passlen;

    strncpy (password, str, sizeof(password));
    if ((passlen = strlen (password)) < (int) sizeof(password)-1)
        strncat (password, cell, sizeof(password)-passlen);
    if ((passlen = strlen(password)) > (int) sizeof(password)) passlen = sizeof(password);

    memcpy (&ivec, "kerberos", 8);
    memcpy (&temp_key, "kerberos", 8);
    des_fixup_key_parity (&temp_key);
    des_key_sched (&temp_key, schedule);
    des_cbc_cksum ((des_cblock *)password, &ivec, passlen, schedule, &ivec);

    memcpy (&temp_key, &ivec, sizeof temp_key);
    des_fixup_key_parity (&temp_key);
    des_key_sched (&temp_key, schedule);
    des_cbc_cksum ((des_cblock *)password, key, passlen, schedule, &ivec);

    des_fixup_key_parity (key);

    return SASL_OK;
}

static int krb_afs_string_to_key(str, key, cell)
char *str;
des_cblock *key;
char *cell;                  /* cell for password */
{
    if (strlen(str) > 8) {
	afs_transarc_StringToKey (str, cell, key);
    }
    else {
	afs_cmu_StringToKey (str, cell, key);
    }
    return SASL_OK;
}

/* convert string to all lower case
 */
char *lcase(char* str)
{
    char *scan = str;
    
    while (*scan) {
	*scan = tolower((int) *scan);
	scan++;
    }

    return (str);
}

static int use_key(char *user __attribute__((unused)), 
		   char *instance __attribute__((unused)), 
		   char *realm __attribute__((unused)), 
		   void *key, des_cblock *returned_key)
{
    memcpy (returned_key, key, sizeof(des_cblock));
    return 0;
}

/*
 * Securely verify the plaintext password 'passwd' for user 'user'
 * against the Kerberos database.  "service" is the name of a service
 * we can verify the returned ticket against.  Returns 1 for success,
 * 0 for failure.  On failure, 'reply' is filled in with a pointer to
 * the reason.
 */
static int kerberos_verify_password(sasl_conn_t *conn,
				    const char *user, 
				    const char *passwd,
				    const char *service, 
				    const char *user_realm 
				               __attribute__((unused)), 
				    const char **reply)
{
    int result;
    des_cblock key;
    char tfname[40];
    char realm[REALM_SZ+1];
    char cell[REALM_SZ+1];
    char hostname[MAXHOSTNAMELEN+1];
    char phost[MAXHOSTNAMELEN+1];
    char oldtkfile[MAXPATHLEN];
    KTEXT_ST authent;
    char instance[INST_SZ];
    AUTH_DAT kdata;
    char *srvtab = "";
    char *user_inst = "";
    sasl_getopt_t *getopt;
    void *context;

    if (!user|| !passwd) {
	return SASL_BADPARAM;
    }
    if (reply) { *reply = NULL; }

    /* check to see if the user configured a srvtab */
    if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
	== SASL_OK) {
	getopt(context, NULL, "srvtab", (const char **) &srvtab, NULL);
	getopt(context, NULL, "plain_inst", (const char **) &user_inst, NULL);

	if (!srvtab) srvtab = "";
	if (!user_inst) user_inst = "";
    }


    if (krb_get_lrealm(realm, 1)) return SASL_FAIL;

    /* after this point, we need to "goto fini" on failure */
    strcpy(oldtkfile, tkt_string()); /* krb gaurantees only MAXPATHLEN-1 length */
    snprintf(tfname,sizeof(tfname), "/tmp/tkt_%d", getpid());
    krb_set_tkt_string(tfname);

    /* First try Kerberos string-to-key */
    des_string_to_key((char *) passwd, &key);
    
    result = krb_get_in_tkt((char *)user, user_inst, realm,
			    "krbtgt", realm, 1, use_key, NULL, &key);

    if (result == INTK_BADPW) {
	/* Now try andrew string-to-key */
	realm[REALM_SZ] = '\0';
	strcpy(cell, realm);
	lcase(cell);
	krb_afs_string_to_key(passwd, key, cell);
    
	result = krb_get_in_tkt((char *)user, user_inst, realm,
				"krbtgt", realm, 1, use_key, NULL, &key);
    }

    memset(key, 0, sizeof(key));

    if (result != 0) {
	if (reply) *reply = krb_err_txt[result];
	result = SASL_FAIL;
	goto fini;
    }

    /* Check validity of returned ticket */
    gethostname(hostname, sizeof(hostname));
    strncpy(phost, krb_get_phost(hostname), sizeof(phost)-1);

    result = krb_mk_req(&authent, (char *) service, phost, realm, 0);

    if (result != 0) {
	memset(&authent, 0, sizeof(authent));
	if (reply) *reply = krb_err_txt[result];
	result = SASL_FAIL;
	goto fini;
    }
    strcpy(instance, "*");

    result = krb_rd_req(&authent, (char *) service, instance, 0L, &kdata, 
			srvtab); 

    memset(&authent, 0, sizeof(authent));
    memset(kdata.session, 0, sizeof(kdata.session));
    if (result != 0 || strcmp(kdata.pname, user) != 0 || kdata.pinst[0] ||
	strcmp(kdata.prealm, realm) != 0) {
	if (result != 0) {
	    if (reply) *reply = krb_err_txt[result];
	}
	else {
	    if (reply) *reply = "Kerberos ID does not match user name";
	}
	result = SASL_FAIL;
    }
    else result = SASL_OK;

 fini:
    /* destroy any tickets we managed to acquire here */
    dest_tkt();
    /* restore the old ticket file */
    krb_set_tkt_string(oldtkfile);
    return result;
}

#endif /* HAVE_KRB */

#ifdef HAVE_GSSAPI_H 
/* ok, this is  wrong but the most convenient way of doing 
 * it for now. We assume (possibly incorrectly) that if GSSAPI exists then 
 * the Kerberos 5 headers and libraries exist.   
 * What really should be done is a configure.in check for krb5.h and use 
 * that since none of this code is GSSAPI but rather raw Kerberos5.
 *
 * This function also has a bug where an alternate realm can't be
 * specified.  
 */

#include <krb5.h>

/* returns 0 for failure, 1 for success */
static int k5support_verify_tgt(krb5_context context, 
				krb5_ccache ccache) 
{
  krb5_principal server;
  krb5_data packet;
  krb5_keyblock *keyblock = NULL;
  krb5_auth_context auth_context = NULL;
  krb5_error_code k5_retcode;
  char thishost[BUFSIZ];
  int result = 0;

  if (krb5_sname_to_principal(context, NULL, NULL,
			      KRB5_NT_SRV_HST, &server)) {
    return 0;
  }

  if (krb5_kt_read_service_key(context, NULL, server, 0,
			       0, &keyblock)) {
    goto fini;
  }

  if (keyblock) {
    free(keyblock);
  }

  /* this duplicates work done in krb5_sname_to_principal
   * oh well.
   */
  if (gethostname(thishost, BUFSIZ) < 0) {
    goto fini;
  }
  thishost[BUFSIZ-1] = '\0';
  
  krb5_data_zero(&packet);
  k5_retcode = krb5_mk_req(context, &auth_context, 0, "host", 
			   thishost, NULL, ccache, &packet);

  if (auth_context) {
    krb5_auth_con_free(context, auth_context);
    auth_context = NULL;
  }

  if (k5_retcode) {
    goto fini;
  }

  if (krb5_rd_req(context, &auth_context, &packet, 
		  server, NULL, NULL, NULL)) {
    goto fini;
  }

  
  /* all is good now */
  result = 1;
 fini:
  krb5_free_principal(context, server);

  return result;
}

static int kerberos5_verify_password(sasl_conn_t *conn 
				     __attribute__((unused)),
				    const char *user,
				    const char *passwd,
				    const char *service 
				     __attribute__((unused)),
				    const char *user_realm 
				               __attribute__((unused)), 
				    const char **reply)
{
  krb5_context context;
  krb5_ccache ccache = NULL;
  krb5_principal auth_user;
  krb5_creds creds;
  krb5_get_init_creds_opt opts;
  int result = SASL_FAIL;
  char tfname[40];

  if (!user|| !passwd) {
    return SASL_BADPARAM;
  }

  if (reply) { 
    *reply = NULL; 
  }
  
  if (krb5_init_context(&context)) {
    return SASL_FAIL;
  }
    
  if (krb5_parse_name (context, user, &auth_user)) {
    krb5_free_context(context);
    return SASL_FAIL;
  }

  /* create a new CCACHE so we don't stomp on anything */
  snprintf(tfname,sizeof(tfname), "/tmp/k5cc_%d", getpid());
  if (krb5_cc_resolve(context, tfname, &ccache)) {
    krb5_free_principal(context, auth_user);
    krb5_free_context(context);
    return SASL_FAIL;
  }

  if (krb5_cc_initialize (context, ccache, auth_user)) {
    krb5_free_principal(context, auth_user);
    krb5_free_context(context);
    return SASL_FAIL;
  }

  krb5_get_init_creds_opt_init(&opts);
  /* 15 min should be more than enough */
  krb5_get_init_creds_opt_set_tkt_life(&opts, 900); 
  if (krb5_get_init_creds_password(context, &creds, 
				   auth_user, passwd, NULL, NULL, 
				   0, NULL, &opts)) {
    krb5_cc_destroy(context, ccache);
    krb5_free_principal(context, auth_user);
    krb5_free_context(context);
    return SASL_FAIL;
  }

  /* at this point we should have a TGT. Let's make sure it is valid */
  if (krb5_cc_store_cred(context, ccache, &creds)) {
    krb5_free_principal(context, auth_user);
    krb5_cc_destroy(context, ccache);
    krb5_free_context(context);
    return SASL_FAIL;
  }

  if (!k5support_verify_tgt(context, ccache)) {
    goto fini;
  }

  /* 
   * fall through -- user is valid beyond this point  
   */
  
  /* try to ensure that no one diddled with result */
  assert(result == SASL_FAIL); 
  result = SASL_OK;
  

 fini:
/* destroy any tickets we had */
  krb5_free_cred_contents(context, &creds);
  krb5_free_principal(context, auth_user);
  krb5_cc_destroy(context, ccache);
  krb5_free_context(context);
 return result;
}

#endif /* HAVE_GSSAPI_H */

#ifdef HAVE_GETSPNAM
static int shadow_verify_password(sasl_conn_t *conn __attribute__((unused)),
				  const char *userid,
				  const char *password,
				  const char *service __attribute__((unused)),
				  const char *user_realm 
				               __attribute__((unused)), 
				  const char **reply)
{
  char *salt;
  char *crypted;
  struct spwd *spwd;

  if (!userid || !password) {
      return SASL_BADPARAM;
  }
  if (reply) { *reply = NULL; }

  spwd = getspnam(userid);
  if (! spwd)
    return SASL_BADAUTH; /* can't use it */
  salt = spwd->sp_pwdp;
  crypted = crypt(password, salt);
  if (strcmp(crypted, spwd->sp_pwdp))
    return SASL_BADAUTH;	/* we lose. */

  return SASL_OK;
}
#endif /* HAVE_GETSPNAM */

#ifdef HAVE_SIA
#include <sia.h>
#include <siad.h>
#include <arpa/inet.h>

static int sia_verify_password(sasl_conn_t *conn,
				  const char *userid,
				  const char *password,
				  const char *service __attribute__((unused)),
				  const char *user_realm 
				               __attribute__((unused)), 
				  const char **reply)
{
  char *host;
  struct sockaddr_in *addr;
  char *argv[2], *argv0 = "SASL";
  SIAENTITY *ent;
  int ret;
  argv[0] = argv0;
  argv[1] = NULL;

  if (!userid || !password) {
      return SASL_BADPARAM;
  }
  if (reply) { *reply = NULL; }

  /* Get the remote host */
  if (sasl_getprop(conn, SASL_IP_REMOTE, (void **) &addr) != SASL_OK)
      host = NULL;
  else
      host = inet_ntoa(*addr);

  /* Try to validate */
  if (sia_ses_init (&ent, 1, argv, host, userid, NULL, 0, NULL) != SIASUCCESS)
      return SASL_BADAUTH;
  if ((ret = sia_ses_authent (NULL, password, ent)) != SIASUCCESS) {
      if (ret & SIASTOP)
          sia_ses_release (&ent);
      return SASL_BADAUTH;
  }
  if (sia_ses_estab (NULL, ent) == SIASUCCESS) {
      sia_ses_release (&ent);
      return SASL_OK;
  }
  return SASL_BADAUTH;
}
#endif /* HAVE_SIA */

#ifdef HAVE_GETPWNAM
static int passwd_verify_password(sasl_conn_t *conn __attribute__((unused)),
				  const char *userid,
				  const char *password,
				  const char *service __attribute__((unused)),
				  const char *user_realm 
				               __attribute__((unused)), 
				  const char **reply)
{
  struct passwd *pwd;
  char *salt;
  char *crypted;

  if (!userid || !password) {
      return SASL_BADPARAM;
  }
  if (reply) { *reply = NULL; }

  pwd=getpwnam(userid);
  if (pwd==NULL) return SASL_NOUSER;

  salt = pwd->pw_passwd;

  crypted= crypt(password, salt);

  if (strcmp(crypted, pwd->pw_passwd)==0)
    return SASL_OK;

  return SASL_BADAUTH;
}
#endif /* HAVE_GETPWNAM */

#ifdef HAVE_PAM
struct sasl_pam_data {
    const char *userid;
    const char *password;
    int pam_error;
};

static int sasl_pam_conv(int num_msg, const struct pam_message **msg,
			 struct pam_response **resp, void *appdata_ptr)
{
    struct pam_response *reply = NULL;
    struct sasl_pam_data *pd = (struct sasl_pam_data *) appdata_ptr;
    int i;
    int ret;

    if (pd == NULL) {
	/* solaris bug? */
	return PAM_CONV_ERR;
    }

    reply = (struct pam_response *) sasl_ALLOC(sizeof(struct pam_response) * 
					       num_msg);
    if (reply == NULL)
	return PAM_CONV_ERR;

    for (i = 0; i < num_msg; i++) {
	switch (msg[i]->msg_style) {
	    /* making the blatant assumption that echo on means user,
	       echo off means password */
	case PAM_PROMPT_ECHO_ON:
	    reply[i].resp_retcode = PAM_SUCCESS;
	    ret = _sasl_strdup(pd->userid, &reply[i].resp, NULL);
	    if (ret != SASL_OK)
		return PAM_CONV_ERR;
	    break;
	case PAM_PROMPT_ECHO_OFF:
	    reply[i].resp_retcode = PAM_SUCCESS;
	    ret = _sasl_strdup(pd->password, &reply[i].resp, NULL);
	    if (ret != SASL_OK)
		return PAM_CONV_ERR;
	    break;
	case PAM_TEXT_INFO:
	case PAM_ERROR_MSG:
	    /* ignore it, but pam still wants a NULL response... */
	    reply[i].resp_retcode = PAM_SUCCESS;
	    reply[i].resp = NULL;
	    break;
	default:		/* error! */
	    sasl_FREE(reply);
	    pd->pam_error = 1;
	    return PAM_CONV_ERR;
	}
    }
    *resp = reply;
    return PAM_SUCCESS;
}

static struct pam_conv my_conv = {
    &sasl_pam_conv,		/* int (*conv) */
    NULL			/* appdata_ptr */
};

static int pam_verify_password(sasl_conn_t *conn __attribute__((unused)),
			       const char *userid, 
			       const char *password,
			       const char *service,
			       const char *user_realm __attribute__((unused)), 
			       const char **reply)
{
    pam_handle_t *pamh;
    struct sasl_pam_data pd;
    int pam_error;

    if (!userid || !password) {
	return SASL_BADPARAM;
    }
    /* protect against a bogus PAM plugin */
    if (!*userid || !*password) {
	return SASL_FAIL;
    }

    if (reply) { *reply = NULL; }

    my_conv.appdata_ptr = &pd;

    pd.userid = userid;
    pd.password = password;
    pd.pam_error = 0;

    pam_error = pam_start(service, userid, &my_conv, &pamh);
    if (pam_error != PAM_SUCCESS) {
	goto pam_err;
    }
    pam_error = pam_authenticate(pamh, PAM_SILENT);
    if (pam_error != PAM_SUCCESS) {
	goto pam_err;
    }
    pam_end(pamh, PAM_SUCCESS);

    return SASL_OK;    

pam_err:
    return SASL_BADAUTH;
}

#endif /* HAVE_PAM */

/* we store the following secret to check plaintext passwords:
 *
 * <salt> \0 <secret>
 *
 * where <secret> = MD5(<salt>, "sasldb", <pass>)
 */
static int _sasl_make_plain_secret(const char *salt, 
				   const char *passwd, int passlen,
				   sasl_secret_t **secret)
{
    MD5_CTX ctx;
    unsigned sec_len = 16 + 1 + 16; /* salt + "\0" + hash */

    *secret = (sasl_secret_t *) sasl_ALLOC(sizeof(sasl_secret_t) +
					   sec_len * sizeof(char));
    if (*secret == NULL) {
	return SASL_NOMEM;
    }

    MD5Init(&ctx);
    MD5Update(&ctx, salt, 16);
    MD5Update(&ctx, "sasldb", 6);
    MD5Update(&ctx, passwd, passlen);
    memcpy((*secret)->data, salt, 16);
    memcpy((*secret)->data + 16, "\0", 1);
    MD5Final((*secret)->data + 17, &ctx);
    (*secret)->len = sec_len;
    
    return SASL_OK;
}

/* returns the realm we should pretend to be in */
static int parseuser(char **user, char **realm, const char *user_realm, 
		     const char *serverFQDN, const char *input)
{
    int ret;
    char *r;

    assert(user && serverFQDN);

    if (!user_realm) {
	ret = _sasl_strdup(serverFQDN, realm, NULL);
	if (ret == SASL_OK) {
	    ret = _sasl_strdup(input, user, NULL);
	}
    } else if (user_realm[0]) {
	ret = _sasl_strdup(user_realm, realm, NULL);
	if (ret == SASL_OK) {
	    ret = _sasl_strdup(input, user, NULL);
	}
    } else {
	/* otherwise, we gotta get it from the user */
	r = strchr(input, '@');
	if (!r) {
	    /* hmmm, the user didn't specify a realm */
	    /* we'll default to the serverFQDN */
	    ret = _sasl_strdup(serverFQDN, realm, NULL);
	    if (ret == SASL_OK) {
		ret = _sasl_strdup(input, user, NULL);
	    }
	} else {
	    r++;
	    ret = _sasl_strdup(r, realm, NULL);
	    *--r = '\0';
	    *user = sasl_ALLOC(r - input + 1);
	    if (*user) {
		strncpy(*user, input, r - input +1);
	    } else {
		ret = SASL_NOMEM;
	    }
	    *r = '@';
	}
    }

    return ret;
}

static int sasldb_verify_password(sasl_conn_t *conn,
				  const char *userstr,
				  const char *passwd,
				  const char *service __attribute__((unused)),
				  const char *user_realm, 
				  const char **reply)
{
    sasl_server_getsecret_t *getsec;
    void *context = NULL;
    int ret = SASL_FAIL;
    sasl_secret_t *secret = NULL;
    sasl_secret_t *construct = NULL;
    char *userid = NULL;
    char *realm = NULL;

    if (reply) { *reply = NULL; }
    if (!userstr || !passwd) {
	return SASL_BADPARAM;
    }
    ret = parseuser(&userid, &realm, user_realm, conn->serverFQDN, userstr);
    if (ret != SASL_OK) {
	/* error parsing user */
	goto done;
    }
    ret = _sasl_getcallback(conn, SASL_CB_SERVER_GETSECRET, &getsec, &context);
    if (ret != SASL_OK) {
	/* error getting getsecret callback */
	goto done;
    }

    ret = getsec(context, "PLAIN", userid, realm, &secret);
    if (ret != SASL_OK) {
	/* error getting PLAIN secret */
	goto done;
    }

    ret = _sasl_make_plain_secret(secret->data, passwd, strlen(passwd),
				  &construct);
    if (ret != SASL_OK) {
	goto done;
    }

    if (!memcmp(secret->data, construct->data, secret->len)) {
	/* password verified! */
	ret = SASL_OK;
    } else {
	/* passwords do not match */
	ret = SASL_BADAUTH;
    }

 done:
    if (userid) sasl_FREE(userid);
    if (realm)  sasl_FREE(realm);

    if (secret) sasl_free_secret(&secret);
    if (construct) sasl_free_secret(&construct);
    return ret;
}

/* this routine sets the sasldb password given a user/pass */
int _sasl_sasldb_set_pass(sasl_conn_t *conn,
			  const char *userstr, 
			  const char *pass,
			  unsigned passlen,
			  const char *user_realm,
			  int flags,
			  const char **errstr)
{
    char *userid = NULL;
    char *realm = NULL;
    int ret = SASL_OK;

    if (errstr) {
	*errstr = NULL;
    }

    ret = parseuser(&userid, &realm, user_realm, conn->serverFQDN, userstr);
    if (ret != SASL_OK) {
	return ret;
    }

    if (pass != NULL && !(flags & SASL_SET_DISABLE)) {
	/* set the password */
	sasl_secret_t *sec = NULL;
	char salt[16];
	sasl_rand_t *rpool = NULL;
	sasl_server_getsecret_t *getsec;
	sasl_server_putsecret_t *putsec;
	void *context;

	/* if SASL_SET_CREATE is set, we don't want to overwrite an
	   existing account */
	if (flags & SASL_SET_CREATE) {
	    ret = _sasl_getcallback(conn, SASL_CB_SERVER_GETSECRET,
				    &getsec, &context);
	    if (ret == SASL_OK) {
		ret = getsec(context, "PLAIN", userid, realm, &sec);
		if (ret == SASL_OK) {
		    sasl_free_secret(&sec);
		    ret = SASL_NOCHANGE;
		}
		if (ret == SASL_NOUSER) {
		    /* hmmm, don't want to change this */
		    ret = SASL_OK;
		}
	    }
	}
	
	/* ret == SASL_OK iff we still want to set this password */
	if (ret == SASL_OK) {
	    ret = sasl_randcreate(&rpool);
	}
	if (ret == SASL_OK) {
	    sasl_rand(rpool, salt, 16);
	    ret = _sasl_make_plain_secret(salt, pass, passlen, &sec);
	}
	if (ret == SASL_OK) {
	    ret = _sasl_getcallback(conn, SASL_CB_SERVER_PUTSECRET, 
				    &putsec, &context);
	}
	if (ret == SASL_OK) {
	    ret = putsec(context, "PLAIN", userid, realm, sec);
	}
	if (ret != SASL_OK) {
	    _sasl_log(conn, SASL_LOG_ERR, NULL, ret, 0,
		      "failed to set plaintext secret for %s: %z", userid);
	}
	if (rpool) {
	    sasl_randfree(&rpool);
	}
	if (sec) {
	    sasl_free_secret(&sec);
	}
    } else { 
	/* SASL_SET_DISABLE specified */
	sasl_server_putsecret_t *putsec;
	void *context;

	ret = _sasl_getcallback(conn, SASL_CB_SERVER_PUTSECRET, 
				&putsec, &context);
	if (ret == SASL_OK) {
	    ret = putsec(context, "PLAIN", userid, realm, NULL);
	}
	if (ret != SASL_OK) {
	    _sasl_log(conn, SASL_LOG_ERR, NULL, ret, 0,
		      "failed to disable account for %s: %z", userid);
	}
    }

    if (userid)   sasl_FREE(userid);
    if (realm)    sasl_FREE(realm);
    return ret;
}


#ifdef HAVE_PWCHECK
/*
 * Keep calling the writev() system call with 'fd', 'iov', and 'iovcnt'
 * until all the data is written out or an error occurs.
 */
static int retry_writev(int fd, struct iovec *iov, int iovcnt)
{
    int n;
    int i;
    int written = 0;
    static int iov_max =
#ifdef MAXIOV
	MAXIOV
#else
#ifdef IOV_MAX
	IOV_MAX
#else
	8192
#endif
#endif
	;
    
    for (;;) {
	while (iovcnt && iov[0].iov_len == 0) {
	    iov++;
	    iovcnt--;
	}

	if (!iovcnt) return written;

	n = writev(fd, iov, iovcnt > iov_max ? iov_max : iovcnt);
	if (n == -1) {
	    if (errno == EINVAL && iov_max > 10) {
		iov_max /= 2;
		continue;
	    }
	    if (errno == EINTR) continue;
	    return -1;
	}

	written += n;

	for (i = 0; i < iovcnt; i++) {
	    if ((int)iov[i].iov_len > n) {
		iov[i].iov_base = (char *)iov[i].iov_base + n;
		iov[i].iov_len -= n;
		break;
	    }
	    n -= iov[i].iov_len;
	    iov[i].iov_len = 0;
	}

	if (i == iovcnt) return written;
    }
}



/* pwcheck daemon-authenticated login */
static int pwcheck_verify_password(sasl_conn_t *conn  __attribute__((unused)),
				   const char *userid, 
				   const char *passwd,
				   const char *service __attribute__((unused)),
				   const char *user_realm 
				               __attribute__((unused)), 
				   const char **reply)
{
    int s;
    struct sockaddr_un srvaddr;
    int r;
    struct iovec iov[10];
    static char response[1024];
    unsigned int start, n;
    char pwpath[1024];

    if (reply) { *reply = NULL; }

    if (strlen(PWCHECKDIR)+8+1 > sizeof(pwpath)) return SASL_FAIL;

    strcpy(pwpath, PWCHECKDIR);
    strcat(pwpath, "/pwcheck");

    s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1) return errno;

    memset((char *)&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sun_family = AF_UNIX;
    strncpy(srvaddr.sun_path, pwpath, sizeof(srvaddr.sun_path));
    r = connect(s, (struct sockaddr *)&srvaddr, sizeof(srvaddr));
    if (r == -1) {
	if (reply) { *reply = "cannot connect to pwcheck server"; }
	return SASL_FAIL;
    }

    iov[0].iov_base = (char *)userid;
    iov[0].iov_len = strlen(userid)+1;
    iov[1].iov_base = (char *)passwd;
    iov[1].iov_len = strlen(passwd)+1;

    retry_writev(s, iov, 2);

    start = 0;
    while (start < sizeof(response) - 1) {
	n = read(s, response+start, sizeof(response) - 1 - start);
	if (n < 1) break;
	start += n;
    }

    close(s);

    if (start > 1 && !strncmp(response, "OK", 2)) {
	return SASL_OK;
    }

    response[start] = '\0';
    if (reply) { *reply = response; }
    return SASL_BADAUTH;
}

#endif

#ifdef HAVE_SASLAUTHD
/* saslauthd-authenticated login */
static int saslauthd_verify_password(sasl_conn_t *conn,
				   const char *userid, 
				   const char *passwd,
				   const char *service __attribute__((unused)),
				   const char *user_realm 
				               __attribute__((unused)), 
				   const char **reply)
{
    static char response[1024];
    int s;
    struct sockaddr_un srvaddr;
    int r, n;
    unsigned int start;
    sasl_getopt_t *getopt;
    void *context;
    char pwpath[sizeof(srvaddr.sun_path)];
    const char *p = NULL;

    if (reply)
	*reply = NULL;

    /* check to see if the user configured a rundir */
    if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) == SASL_OK) {
	getopt(context, NULL, "saslauthd_path", &p, NULL);
    }
    if (p) {
	strncpy(pwpath, p, sizeof(pwpath));
    } else {
	if (strlen(PATH_SASLAUTHD_RUNDIR) + 4 + 1 > sizeof(pwpath))
	    return SASL_FAIL;

	strcpy(pwpath, PATH_SASLAUTHD_RUNDIR);
	strcat(pwpath, "/mux");
    }

    s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
	return errno;

    memset((char *)&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sun_family = AF_UNIX;
    strncpy(srvaddr.sun_path, pwpath, sizeof(srvaddr.sun_path));

    r = connect(s, (struct sockaddr *) &srvaddr, sizeof(srvaddr));
    if (r == -1) {
	if (reply)
	    *reply = "cannot connect to pwcheck server";
	return SASL_FAIL;
    }

    {
	int u_len;
	int p_len;
	char *msg;

	u_len = strlen(userid) + 1;
	p_len = strlen(passwd) + 1;
	msg = sasl_ALLOC(u_len + p_len);
	if (msg == NULL) {
	    close(s);
	    if (reply)
		*reply = "not enough memory";
	    return SASL_FAIL;
	}
	strcpy(msg, userid);
	strcpy(msg + u_len, passwd);

	while (write(s, msg, u_len + p_len) == -1)
	    switch (errno) {
	    case EINTR:
		continue;
	    default:
		sasl_FREE(msg);
		if (reply)
		    *reply = "write failed";
		return SASL_FAIL;
	    }

	sasl_FREE(msg);
    }

    start = 0;
    while (start < sizeof(response) - 1) {
	n = read(s, response + start, sizeof(response) - 1 - start);
	if (n < 1)
	    break;
	start += n;
    }

    close(s);

    if (start > 1 && !strncmp(response, "OK", 2))
	return SASL_OK;

    response[start] = '\0';
    if (reply)
	*reply = response;
    return SASL_BADAUTH;
}

#endif

struct sasl_verify_password_s _sasl_verify_password[] = {
    { "sasldb", &sasldb_verify_password },
#ifdef HAVE_KRB
    { "kerberos_v4", &kerberos_verify_password },
#endif
#ifdef HAVE_GSSAPI_H
    { "kerberos_v5", &kerberos5_verify_password },
#endif
#ifdef HAVE_GETSPNAM
    { "shadow", &shadow_verify_password },
#endif
#ifdef HAVE_SIA
    { "sia", &sia_verify_password },
#endif
#ifdef HAVE_GETPWNAM
    { "passwd", &passwd_verify_password },
#endif
#ifdef HAVE_PAM
    { "pam", &pam_verify_password },
#endif
#ifdef HAVE_PWCHECK
    { "pwcheck", &pwcheck_verify_password },
#endif
#ifdef HAVE_SASLAUTHD
    { "saslauthd", &saslauthd_verify_password },
#endif
    { NULL, NULL }
};
