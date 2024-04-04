/* Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/types.h>    
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <dirent.h>
#include <security/pam_appl.h>

#include "apr.h"
#include "apr_md5.h"
#include "apr_portable.h"
#include "apr_random.h"
#include "apr_file_io.h"

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"

#include "mod_privsep.h"


module AP_MODULE_DECLARE_DATA privsep_module;

/* global */
int privsep_enabled = 0;
int privsep_inited = 0;
int privsep_client_fd;
privsep_token_t *privsep_root_token;
int privsep_worker_sp[2];

#if APR_HAS_THREADS
/* If we have a threaded worker, we use a per worker child process lock
   to be used around sending and recieving to the privsep process
   as there is only one socketpair per child process */
apr_thread_mutex_t *privsep_worker_mutex;
#endif

/* server */
static int privsep_debug = 0;
static pid_t privsep_pid = 0;
static int privsep_server_fd;
static char privsep_msgbuf[PRIVSEP_BUF_SIZE];


/* remove /./'s, /../'s and trailing .. */
static int privsep_secure_path(char *s)
{
    char *r, *w;
    int last_was_slash = 0;

    r = w = s;
    while(*r != 0)
	{
	    /* Ignore duplicate /'s */
	    if (*r == '/' && last_was_slash)
		{
		    r++;
		    continue;
		}
	    /* Calculate /../ in a secure way and avoid */
	    if (last_was_slash && *r == '.')
		{
		    if (*(r+1) == '.') {
			/* skip past .. or ../ with read pointer */
			if (*(r+2) == '/') r += 3;
			else if (*(r+2) == 0) r += 2;
			/* skip back to last / with write pointer */
			if (w > s+1)
			    {
				w--;
				while(*(w-1) != '/') { w--; }
				continue;
			    }
			else
			    {
				return -1; /* Bad Request */
			    }
		    } else if (*(r+1) == '/') {
			r += 2;
			continue;
		    }
		}
	    *w = *r;
	    last_was_slash = (*r == '/');
	    r++;
	    w++;
	}
    *w = 0;
    return 0;
}

/*
 * auth_pam_talker: supply authentication information to PAM when asked
 *
 * (code from mod_auth_pam)
 *
 * Assumptions:
 *   A password is asked for by requesting input without echoing
 *   A username is asked for by requesting input _with_ echoing
 *
 */
static int auth_pam_talker(int num_msg,
			   const struct pam_message **msg,
			   struct pam_response **resp,
			   void *appdata_ptr)
{
    unsigned short i = 0;
    auth_pam_userinfo *userinfo = (auth_pam_userinfo*)appdata_ptr;
    struct pam_response *response = 0;

    /* parameter sanity checking */
    if (!resp || !msg || !userinfo)
	return PAM_CONV_ERR;

    /* allocate memory to store response */
    response = malloc(num_msg * sizeof(struct pam_response));
    if (!response)
	return PAM_CONV_ERR;

    /* copy values */
    for(i = 0; i < num_msg; i++) {
	/* initialize to safe values */
	response[i].resp_retcode = 0;
	response[i].resp = 0;

	/* select response based on requested output style */
	switch(msg[i]->msg_style) {
	case PAM_PROMPT_ECHO_ON:
	    /* on memory allocation failure, auth fails */
	    response[i].resp = strdup(userinfo->user);
	    break;
	case PAM_PROMPT_ECHO_OFF:
	    response[i].resp = strdup(userinfo->pass);
	    break;
	default:
	    if (response) free(response);
	    return PAM_CONV_ERR;
	}
    }
    /* everything okay, set PAM response values */
    *resp = response;
    return PAM_SUCCESS;
}

static int privsep_authenticate(const char **errormsg,
				const char *user, const char *pass,
				server_rec *server_conf,
				const char *rhost)
{
    int rv;
    auth_pam_userinfo userinfo = { user, pass };
    struct pam_conv conv_info = { &auth_pam_talker, (void*)&userinfo};
    pam_handle_t *pamh  = NULL;

    /* initialize pam */
    rv = pam_start((strcmp(user, "admin") ?
                    PRIVSEP_PAM_SERVICE_NAME : "frontview"),
                   user, &conv_info, &pamh);
    if (rv != PAM_SUCCESS) {
	*errormsg = pam_strerror(pamh, rv);
	return PRIVSEP_AUTH_GENERAL_ERROR;
    }

    /* set remote hostname */
    if (rhost && rhost[0] != '\0') {
        rv = pam_set_item(pamh, PAM_RHOST, rhost);
        if (rv != PAM_SUCCESS) {
	    *errormsg = pam_strerror(pamh, rv);
	    pam_end(pamh, rv);
	    return PRIVSEP_AUTH_GENERAL_ERROR;
        }
    }

#ifdef HAVE_PAM_FAIL_DELAY
    /* We need to delay in client so we don't block other requests.
       You also need to set "nodelay" flag in the apache auth configuration
       so the auth module doesn't delay (pam_fail_delay only sets the
       minimum delay which modules can override). */
    pam_fail_delay(pamh, 0 /* usec */);
#endif

    /* try to authenticate user, log error on failure */
    rv = pam_authenticate(pamh, PAM_DISALLOW_NULL_AUTHTOK);
    if (rv != PAM_SUCCESS) {
	*errormsg = pam_strerror(pamh, rv);
	pam_end(pamh, PAM_SUCCESS);
	switch(rv) {
	case PAM_AUTH_ERR:
	    return PRIVSEP_AUTH_DENIED;
	case PAM_USER_UNKNOWN:
	    return PRIVSEP_AUTH_USER_NOT_FOUND;
	default:
	    return PRIVSEP_AUTH_GENERAL_ERROR;
	}
    }

    /* check that the account is healthy */
    rv = pam_acct_mgmt(pamh, PAM_DISALLOW_NULL_AUTHTOK);
    if (rv != PAM_SUCCESS) {
	*errormsg = pam_strerror(pamh, rv);
	pam_end(pamh, PAM_SUCCESS);
	return PRIVSEP_AUTH_GENERAL_ERROR;
    }

    pam_end(pamh, PAM_SUCCESS);
    return PRIVSEP_AUTH_GRANTED;
}

static int privsep_salt_token(privsep_token_t *token)
{
    return apr_generate_random_bytes(token->salt, sizeof(token->salt));
}

static int privsep_check_token(privsep_token_t *token, unsigned char* auth_key)
{
    char digest[APR_MD5_DIGESTSIZE];
    apr_md5_ctx_t md5_ctx;

    /* save digest for comparison */
    memcpy(digest, token->digest, APR_MD5_DIGESTSIZE);

    /* hash auth_key, uid, gid and salt */
    apr_md5_init(&md5_ctx);
    apr_md5_update(&md5_ctx, auth_key, PRIVSEP_AUTH_KEY_LEN);
    apr_md5_update(&md5_ctx, (unsigned char*)&token->uid, sizeof(token->uid));
    apr_md5_update(&md5_ctx, (unsigned char*)&token->gid, sizeof(token->gid));
    apr_md5_update(&md5_ctx, token->salt, sizeof(token->salt));
    apr_md5_final(token->digest, &md5_ctx);

    return memcmp(digest, token->digest, APR_MD5_DIGESTSIZE);
}

static char* privsep_print_token(privsep_token_t *token)
{
    static char buf[sizeof(privsep_token_t) * 2 + 32];
    sprintf(buf, "uid=%d gid=%d salt=%04x%04x hash=%04x%04x%04x%04x",
	    token->uid, token->gid,
	    *((unsigned*)token->salt),
	    *(((unsigned*)token->salt)+1),
	    *((unsigned*)token->digest),
	    *(((unsigned*)token->digest)+1),
	    *(((unsigned*)token->digest)+2),
	    *(((unsigned*)token->digest)+3));
    return buf;
}

/* setup message header and control message to send fd */

void privsep_send_fd(struct msghdr *mh, char *buf, int fd)
{
    struct cmsghdr *cmsg;

    mh->msg_control = buf;
    mh->msg_controllen = CMSG_SPACE(sizeof (fd));
    cmsg = CMSG_FIRSTHDR(mh);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof (fd));
    *(int *) CMSG_DATA (cmsg) = fd;
    mh->msg_controllen = cmsg->cmsg_len;
}

/* main privileged request handler */

static void privsep_service_request (unsigned char *auth_key,
				     server_rec *server_conf,
				     apr_pool_t *ptemp)
{
    int rv;
    struct msghdr mh;
    struct iovec iv;
    privsep_message_t *msg = (privsep_message_t*)privsep_msgbuf;
    struct cmsghdr *cmsg;
    char *path1, *path2;
    int replyfd = -1;
    char ccmsg[CMSG_SPACE(sizeof (int))];
    apr_file_t *tmpfile = NULL;
    apr_dir_t *tmpdir = NULL;

    /* construct an iovec large enough to receive the input parameters */
    iv.iov_base = privsep_msgbuf;
    iv.iov_len = PRIVSEP_BUF_SIZE;

    /* link the iovec into the message header */
    memset (&mh, 0, sizeof (mh));
    mh.msg_iov = &iv;
    mh.msg_iovlen = 1;

    /* prepare the header to receive a control message with the replyfd */
    mh.msg_control = ccmsg;
    mh.msg_controllen = sizeof (ccmsg);

    /* wait for a message */
    if ((rv = recvmsg (privsep_server_fd, &mh, 0)) < 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: recvmsg");
	return;
    }

    /* pull out the replyfd that was sent in the control message */
    cmsg = CMSG_FIRSTHDR (&mh);
    if (cmsg == NULL) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: missing control header");
	goto fail_perm;
    } else if (!cmsg->cmsg_type == SCM_RIGHTS) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: control message not fd");
	goto fail_perm;
    } else {
	replyfd = (*(int *) CMSG_DATA (cmsg));
    }

    /* pull out the path arguments */
    path1 = privsep_msgbuf + sizeof(privsep_message_t);
    path2 = path1 + strlen(path1) + 1;

    /* check the token */
    if (privsep_check_token(&msg->token, auth_key) != 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: invalid token");
	goto fail_perm;
    }

    /* set iovec len for reply */
    iv.iov_len = sizeof (*msg);

    mh.msg_flags = 0;
    mh.msg_control = NULL;
    mh.msg_controllen = 0;

    /* full authentication */
    if (msg->command == privsep_command_authenticate) {
	struct passwd *pw = NULL;
	const char *errormsg = "success";

	msg->retval = privsep_authenticate(&errormsg, path1, path2,
					       server_conf, msg->rhost);
	if (msg->retval != PRIVSEP_AUTH_GRANTED) {

	} else if ((pw = getpwnam (path1)) < 0) {
	    errormsg = "no such user";
	    msg->retval = PRIVSEP_AUTH_USER_NOT_FOUND;
	} else if (pw->pw_uid == 0) {
	    errormsg = "disallowed uid 0";
	    msg->retval = PRIVSEP_AUTH_DENIED;
	} else if (pw->pw_gid == 0) {
	    errormsg = "disallowed gid 0";
	    msg->retval = PRIVSEP_AUTH_DENIED;
	} else if (privsep_salt_token(&msg->token) != APR_SUCCESS) {
	    errormsg = "failed to create salt";
	    msg->retval = PRIVSEP_AUTH_GENERAL_ERROR;
	} else {
	    msg->token.uid = pw->pw_uid;
	    msg->token.gid = pw->pw_gid;
	    privsep_check_token(&msg->token, auth_key);
	    errormsg = privsep_print_token(&msg->token);
	    msg->retval = PRIVSEP_AUTH_GRANTED;
	}
	if (privsep_debug || msg->retval != PRIVSEP_AUTH_GRANTED)
	    ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			 "privsep_%-15s %-5d \"%s\" %s (rc=%d)",
			 "authenticate", pw ? pw->pw_uid : -1, path1,
			 errormsg, msg->retval);
	goto send_reply;
    }

    /* uid and gid for username must match hashed token
       (this method is used by subrequests to short circuit full
        authentication when the parent request is already authenticated) */
    if (msg->command == privsep_command_verify_token) {
	struct passwd *pw = NULL;
	const char *errormsg = "success";

	if ((pw = getpwnam (path1)) < 0) {
	    errormsg = "no such user";
	    msg->retval = PRIVSEP_AUTH_USER_NOT_FOUND;
	} else if (msg->token.uid != pw->pw_uid) {
	    errormsg = "user uid does not match token uid";
	    msg->retval = PRIVSEP_AUTH_GENERAL_ERROR;
	} else if (msg->token.gid != pw->pw_gid) {
	    errormsg = "user gid does not match token gid";
	    msg->retval = PRIVSEP_AUTH_GENERAL_ERROR;
	} else {
	    msg->retval = PRIVSEP_AUTH_GRANTED;
	}
	if (privsep_debug || msg->retval != PRIVSEP_AUTH_GRANTED)
	    ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			 "privsep_%-15s %-5d \"%s\" %s (rc=%d)",
			 "verify_token", pw ? pw->pw_uid : -1, path1,
			 errormsg, msg->retval);
	goto send_reply;
    }

    if ( privsep_secure_path (path1) < 0 ||
	 (strlen(path2) && privsep_secure_path (path2) < 0) ) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: invalid path ignored.");
	goto fail_perm;
    }

    /* only allow stat for root uid or gid */
    if ((msg->token.uid == 0 || msg->token.gid == 0) &&
	!(msg->command == privsep_command_stat)) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: uid 0 disallowed");
	goto fail_perm;
    }

    /* initialise groups and set effective user and group */
    /* TODO - use NGROUPS so we can have supplementary groups, this will
       need cached setuid processes for performance reasons */
    if (setgroups (1 /* NGROUPS */, &msg->token.gid) < 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: setgroups(1, [%d])",
		     msg->token.gid);
	goto fail_perm;
    }
    if (setegid (msg->token.gid) < 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: setegid(%d)\n",
		     msg->token.gid);
	goto fail_perm;
    }
    if (seteuid (msg->token.uid) < 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_conf,
		     "privsep_service_request: seteuid(%d)",
		     msg->token.uid);
	goto fail_perm;
    }

    /* dispatch privileged command */
    switch (msg->command)
	{
	case privsep_command_file_open:
	    msg->retval = apr_file_open(&tmpfile, path1,
					    msg->flags, msg->perms, ptemp);
	    if(msg->retval == APR_SUCCESS) {
		apr_os_file_t fd;
		apr_os_file_get(&fd, tmpfile);
		privsep_send_fd(&mh, ccmsg, fd);
	    }
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\", %d, %04o (rc=%d)",
			     "file_open", msg->token.uid, path1,
			     msg->flags, msg->perms, msg->retval);
	    break;

	case privsep_command_dir_open:
	    msg->retval = apr_dir_open(&tmpdir, path1, ptemp);
	    if(msg->retval == APR_SUCCESS) {
		apr_os_dir_t *dir;
		apr_os_dir_get(&dir, tmpdir);
		privsep_send_fd(&mh, ccmsg, dirfd(dir));
	    }
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\" (rc=%d)",
			     "dir_open", msg->token.uid, path1,
			     msg->retval);
	    break;

	case privsep_command_file_perms_set:
	    msg->retval = apr_file_perms_set(path1, msg->perms);
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s  %-5d \"%s\", %04o (rc=%d)",
			     "file_perms_set", msg->token.uid, path1,
			     msg->perms, msg->retval);
	    break;

	case privsep_command_stat:
	    msg->retval = apr_stat(&msg->finfo, path1, msg->flags, NULL);
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\" (rc=%d)",
			     (msg->flags & APR_FINFO_LINK) ? "lstat" : "stat",
			     msg->token.uid, path1, msg->retval);
	    break;

	case privsep_command_dir_make:
	    msg->retval = apr_dir_make(path1, msg->perms, NULL);
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\", %04o (rc=%d)",
			     "dir_make", msg->token.uid, path1,
			     msg->perms, msg->retval);
	    break;

	case privsep_command_file_rename:
	    msg->retval = apr_file_rename(path1, path2, NULL);
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\", \"%s\" (rc=%d)",
			     "file_rename", msg->token.uid, path1, path2,
			     msg->retval);
	    break;

	case privsep_command_dir_remove:
	    msg->retval = apr_dir_remove(path1, NULL);
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\" (rc=%d)",
			     "dir_remove", msg->token.uid, path1,
			     msg->retval);
	    break;

	case privsep_command_file_remove:
	    msg->retval = apr_file_remove(path1, NULL);
	    if (privsep_debug)
		ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_conf,
			     "privsep_%-15s %-5d \"%s\" (rc=%d)",
			     "file_remove", msg->token.uid, path1,
			     msg->retval);
	    break;

	}
send_reply:
    /* send success reply */
    rv = sendmsg (replyfd, &mh, 0);
    if(tmpdir != NULL)
	apr_dir_close(tmpdir);
    if(tmpfile != NULL)
	apr_file_close(tmpfile);
    close(replyfd);
    goto out;

fail_perm:
    /* send error reply */
    memset (msg, 0, sizeof(*msg));
    msg->retval = APR_EACCES;
    if (replyfd >= 0) {
        rv = sendmsg (replyfd, &mh, 0);
        close(replyfd);
    }
out:
    seteuid (0);
    setegid (0);
}


/* module clean up - server */

static void privsep_server_signal(int sig)
{
    close (privsep_server_fd);
    exit(0);
}

static apr_status_t privsep_server_cleanup(void *theworker)
{
    privsep_inited = 0;
    close (privsep_client_fd);
    if(privsep_pid)
	kill(privsep_pid, SIGTERM);
    return APR_SUCCESS;
}


/* module initialisation - server */

static apr_status_t privsep_server_init(apr_pool_t *pconf, apr_pool_t *plog,
					apr_pool_t *ptemp, server_rec *server_conf)
{
    int sv[2];
    int arg, bytes;
    volatile unsigned char auth_key[PRIVSEP_AUTH_KEY_LEN];

    void *data = NULL;
    const char *userdata_key = "privsep_post_config";

    /* Apache loads DSO modules twice. We want to wait until the second
     * load before setting up our global mutex and shared memory segment.
     * To avoid the first call to the post_config hook, we set some
     * dummy userdata in a pool that lives longer than the first DSO
     * load, and only run if that data is set on subsequent calls to
     * this hook. */
    apr_pool_userdata_get(&data, userdata_key, server_conf->process->pool);
    if (data == NULL) {
	/* WARNING: This must *not* be apr_pool_userdata_setn(). The
	 * reason for this is because the static symbol section of the
	 * DSO may not be at the same address offset when it is reloaded.
	 * Since setn() does not make a copy and only compares addresses,
	 * the get() will be unable to find the original userdata. */
	apr_pool_userdata_set((const void *)1, userdata_key,
			      apr_pool_cleanup_null, server_conf->process->pool);
	return APR_SUCCESS; /* This would be the first time through */
    }

    if(!privsep_enabled || privsep_inited) return 0;

    /* create auth key on stack and clear in client when we fork */
    if(apr_generate_random_bytes(auth_key, PRIVSEP_AUTH_KEY_LEN) != APR_SUCCESS) {
	ap_log_error(APLOG_MARK, APLOG_CRIT, 0, server_conf,
		     "privsep_server_init: couldn't create random key");
	return APR_EGENERAL;
    }

    /* create special limited root token (stat only) */
    privsep_root_token = apr_pcalloc(pconf, sizeof(privsep_token_t));
    if (privsep_salt_token(privsep_root_token) < 0) {
	ap_log_error(APLOG_MARK, APLOG_CRIT, 0, server_conf,
		     "privsep_server_init: failed to initialize stat token\n");
	return APR_EGENERAL;
    }
    privsep_check_token(privsep_root_token, auth_key);

    /* create socket pair for communication with worker processes */
    if (socketpair(PF_UNIX, SOCK_DGRAM, 0, sv) < 0) {
	ap_log_error(APLOG_MARK, APLOG_CRIT, 0, server_conf,
		     "privsep_server_init: error creating socketpair");
	return APR_EGENERAL;
    }
    privsep_client_fd = sv[0];
    privsep_server_fd = sv[1];
    apr_pool_cleanup_register(pconf, NULL, privsep_server_cleanup,
			      apr_pool_cleanup_null);

    /* create privsep child process */
    if ((privsep_pid = fork ()) < 0) {
	ap_log_error(APLOG_MARK, APLOG_CRIT, 0, server_conf,
		     "privsep_server_init: fork");
	return APR_EGENERAL;
    }
    else if (privsep_pid > 0) {
	/* in apache parent process */

	/* clear auth key off stack */
	memset(auth_key, 0, PRIVSEP_AUTH_KEY_LEN);
	/* close server side of the privsep socketpair */
	close(privsep_server_fd);
	privsep_inited = 1;
	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, server_conf,
		     "Privilege separation started (pid %d)", privsep_pid);
	return APR_SUCCESS;
    }

    /* in privsep child */

    /* close client side of the privsep socketpair */
    close(privsep_client_fd);
    signal(SIGHUP, privsep_server_signal);
    signal(SIGTERM, privsep_server_signal);
    signal(SIGUSR1, privsep_server_signal);
    umask(002);

    /* okay, here's the main loop */
    while (1) privsep_service_request(auth_key, server_conf, ptemp);
}


/* module clean up - worker */

static apr_status_t privsep_worker_cleanup(void *theworker)
{
    close(privsep_worker_sp[0]);
    close(privsep_worker_sp[1]);
    return APR_SUCCESS;
}


/* module initialisation - worker */

static void privsep_worker_init(apr_pool_t *pchild, server_rec *server_conf)
{
    if(!privsep_enabled) return;

    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, server_conf,
		 "Privilege separation child init (pid %d)", getpid());

    if (socketpair(PF_UNIX, SOCK_DGRAM, 0, privsep_worker_sp) < 0) {
	ap_log_error(APLOG_MARK, APLOG_CRIT, errno, server_conf,
		     "privsep_worker_init: error creating socketpair");
	return;
    }

#if APR_HAS_THREADS
    apr_thread_mutex_create(&privsep_worker_mutex,
			    APR_THREAD_MUTEX_DEFAULT, pchild);
#endif

    apr_pool_cleanup_register(pchild, NULL, privsep_worker_cleanup,
			      apr_pool_cleanup_null);
}


/* module configuration */

int privsep_is_enabled(const request_rec *r)
{
    privsep_dir_conf *conf = (privsep_dir_conf *)
        ap_get_module_config(r->per_dir_config, &privsep_module);
    return (conf ? conf->enabled : 0);
}

static const char *privsep_set_enabled(cmd_parms *cmd, void *dconf,
				       const char *arg)
{
    privsep_dir_conf *conf = dconf;

    if (!strcasecmp(arg, "off") || !strcmp(arg, "0")) {
        conf->enabled = 0;
    } else {
        conf->enabled = 1;
	privsep_enabled = 1; /* we set a global flag if we are enabled on
                             any directory to enable start up of the daemon */
    }
    return NULL;
}

static const char *privsep_set_debug(cmd_parms *cmd, void *dconf,
				     const char *arg)
{
    privsep_dir_conf *conf = dconf;

    if (!strcasecmp(arg, "off") || !strcmp(arg, "0")) {
        privsep_debug = 0;
    } else {
        privsep_debug = 1;
    }
    return NULL;
}

static void *create_privsep_dir_config(apr_pool_t *p, char *dir)
{
    privsep_dir_conf *new =
        (privsep_dir_conf *) apr_pcalloc(p, sizeof(privsep_dir_conf));

    new->enabled = 0;

    return (void *) new;
}

static void *merge_privsep_dir_config(apr_pool_t *p, void *basev, void *addv)
{
    privsep_dir_conf *new = (privsep_dir_conf *)
	apr_pcalloc(p, sizeof(privsep_dir_conf));
    privsep_dir_conf *add = (privsep_dir_conf *) addv;
    privsep_dir_conf *base = (privsep_dir_conf *) basev;

    new->enabled = add->enabled || base->enabled;

    return new;
}

static void privsep_create_subrequest(request_rec *rnew, request_rec *r)
{
    /* copy privsep token to the subrequest if it exists */
    char* priv_token = (char *)apr_table_get(r->notes, PRIVSEP_TOKEN_NOTE);
    if(priv_token) {
      char *new_priv_token =
	  (char *)apr_palloc(rnew->pool, sizeof(privsep_token_t));
      memcpy(new_priv_token, priv_token, sizeof(privsep_token_t));
      apr_table_setn(rnew->notes, PRIVSEP_TOKEN_NOTE, new_priv_token);
    }
}

static const command_rec privsep_cmds[] = {
    AP_INIT_TAKE1("PrivilegeSeparation", privsep_set_enabled, NULL, ACCESS_CONF,
		  "Set to 'On' to enabled privilege separation"),
    AP_INIT_TAKE1("PrivilegeSeparationDebug", privsep_set_debug, NULL, RSRC_CONF,
		  "Set to 'On' to enabled privilege separation debug messages"),
    {NULL}
};

static void register_hooks(apr_pool_t *p)
{
    ap_hook_post_config(privsep_server_init, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_child_init(privsep_worker_init, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_create_subrequest(privsep_create_subrequest, NULL, NULL, APR_HOOK_MIDDLE);
    ap_privsep_register_file_io_hooks();
}

module AP_MODULE_DECLARE_DATA privsep_module = {
    STANDARD20_MODULE_STUFF,
    create_privsep_dir_config, /* create per-directory config structure */
    merge_privsep_dir_config,  /* merge per-directory config structures */
    NULL,                   /* create per-server config structure */
    NULL,                   /* merge per-server config structures */
    privsep_cmds,           /* command apr_table_t */
    register_hooks          /* register hooks */
};
