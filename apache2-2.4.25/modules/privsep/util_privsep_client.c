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
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <dirent.h>

#include "apr.h"
#include "apr_md5.h"
#include "apr_portable.h"
#include "apr_strings.h"
#include "apr_file_io.h"

#include "httpd.h"
#include "http_request.h"
#include "http_log.h"
#include "http_core.h"

#include "mod_privsep.h"
#include "privsep.h"

struct apr_dir_t {
    apr_pool_t *pool;
    char *dirname;
    DIR *dirstruct;
#ifdef APR_USE_READDIR64_R
    struct dirent64 *entry;
#else
    struct dirent *entry;
#endif
};
extern apr_status_t apr_unix_file_cleanup(void *thefile);
extern apr_status_t apr_unix_child_file_cleanup(void *thefile);

/* function to send privileged command requests used by wrappers */

static apr_status_t privsep_send_request(privsep_message_t *msg,
					 privsep_token_t *token,
					 const char *path1, ...)
{
    va_list ap;
    struct msghdr mh;
    struct iovec iv[3];
    int rv;
    struct cmsghdr *cmsg;
    char ccmsg[CMSG_SPACE (sizeof (int))];
    const char *path2 = "\0";

    va_start (ap, path1);
    if (msg->command == privsep_command_file_rename ||
	msg->command == privsep_command_authenticate)
	path2 = va_arg (ap, const char*);
    va_end (ap);

    /* Make sure we don't send too much */
    if (sizeof(privsep_message_t) + strlen(path1) + strlen(path2)
	> PRIVSEP_BUF_SIZE)
	{
	    ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
			 "privsep_send_request: args to large");
	    return APR_EGENERAL;
	}

    /* copy the token and request addr into the command structure */
    memcpy(&msg->token, token, sizeof(msg->token));

    /* construct an iovec with the input parameters */
    iv[0].iov_base = msg;
    iv[0].iov_len = sizeof (*msg);
    iv[1].iov_base = (void*)path1;
    iv[1].iov_len = strlen(path1)+1;
    iv[2].iov_base = (void*)path2;
    iv[2].iov_len = strlen(path2)+1;

    /* link the iovec into the message header */
    memset(&mh, 0, sizeof(mh));
    mh.msg_iov = iv;
    mh.msg_iovlen = 3;

    /* send child socketpair side that we listen for the reply on */
    mh.msg_flags = 0;
    privsep_send_fd(&mh, ccmsg, privsep_worker_sp[0]);

    /* lock per child message serialization mutex */
    if(privsep_worker_mutex_lock() != APR_SUCCESS) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
		     "privsep_send_request: privsep_worker_mutex_lock failed");
	return APR_EGENERAL;
    }

    /* fire away */
    if ((rv = sendmsg (privsep_client_fd, &mh, 0)) < 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
		     "privsep_send_request: sendmsg (rc=%d)", errno);

	/* unlock message serialization mutex */
	privsep_worker_mutex_unlock();

	return APR_EGENERAL;
    }

    /* prepare the header to receive a control message */
    /* with the return payload */
    mh.msg_control = ccmsg;
    mh.msg_controllen = sizeof (ccmsg);

    /* let the reply fall into the same struct */
    mh.msg_iovlen = 1;
    rv = recvmsg (privsep_worker_sp[1], &mh, 0);

    if (rv < 0) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
		     "privsep_send_request: recvmsg (rc=%d)", errno);

	/* unlock message serialization mutex */
	privsep_worker_mutex_unlock();

	return APR_EGENERAL;
    }

    /* unlock message serialization mutex */
    privsep_worker_mutex_unlock();

    if (rv != sizeof(privsep_message_t)) {
	ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
		     "privsep_send_request: invalid reply length");
	return APR_EGENERAL;
    }

    if (msg->command == privsep_command_file_open ||
	msg->command == privsep_command_dir_open) {
	/* attempt to extract control message containing fd */
	cmsg = CMSG_FIRSTHDR (&mh);
	if (cmsg == NULL) {
	    /* might be an error return */
	    return APR_EGENERAL;
	}
	if (!cmsg->cmsg_type == SCM_RIGHTS) {
	    ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
			 "privsep_send_request: control message unknown type %d",
			 cmsg->cmsg_type);
	    return APR_EGENERAL;
	}
	/* otherwise return with the fd in the message */
	return (*(int *) CMSG_DATA (cmsg));
    }

    return msg->retval;
}


/* privsep token fetching functions */

static privsep_token_t* privsep_get_token(const request_rec *r)
{
    return (privsep_token_t*)apr_table_get(r->notes, PRIVSEP_TOKEN_NOTE);
}

static privsep_token_t* privsep_get_preauth_stat_token(const request_rec *r)
{
    privsep_token_t *token = privsep_get_token(r);

    if(token)
	return token;
    else if(privsep_inited)
	return privsep_root_token;
    else
	return NULL;
}


/* privsep auth client wrapper functions */

AP_DECLARE(privsep_authn_status) ap_privsep_authenticate(const request_rec *r,
							 const char *user,
							 const char *pass)
{
    privsep_message_t msg;
    int result;
    const char *rhost = NULL;

    if(!privsep_inited)
	return PRIVSEP_AUTH_GENERAL_ERROR;

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_authenticate;

    rhost = ap_get_remote_host(r->connection, NULL, REMOTE_NOLOOKUP, NULL);
    if (rhost)
	strncpy(msg.rhost, rhost, sizeof(msg.rhost) - 1);

    result = privsep_send_request(&msg, privsep_root_token, user, pass);

    if(result == PRIVSEP_AUTH_GRANTED) {
	char *privsep_token =
	    (char *)apr_palloc(r->pool, sizeof(msg.token));
	memcpy(privsep_token, &msg.token, sizeof(msg.token));
	apr_table_setn(r->notes, PRIVSEP_TOKEN_NOTE, privsep_token);
    } else {
	/* we've disabled delay in the privsep process so as to not
	   block it, instead we need to delay here */
	usleep(PRIVSEP_AUTH_FAIL_DELAY);
    }

    return result;
}

AP_DECLARE(privsep_authn_status) ap_privsep_verify_token(privsep_token_t *token,
							 const char *user)
{
    privsep_message_t msg;

    if(!privsep_inited)
	return PRIVSEP_AUTH_GENERAL_ERROR;

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_verify_token;
    return privsep_send_request(&msg, token, user);
}


/* privsep portable apr wrappers */

static apr_status_t privsep_stat(privsep_token_t *token,
				 apr_finfo_t *finfo, 
				 const char *fname, 
				 apr_int32_t wanted,
				 apr_pool_t *pool)
{
    privsep_message_t msg;
    int result;

    if (!privsep_inited || !token)
	return apr_stat(finfo, fname, wanted, pool);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_stat;
    msg.flags = wanted;
    result = privsep_send_request(&msg, token, fname);
    if ((result == APR_SUCCESS) || (result == APR_INCOMPLETE)) 
    	memcpy(finfo, &msg.finfo, sizeof(*finfo));
    return result;
}

static apr_status_t privsep_file_perms_set(privsep_token_t *token,
					   const char *fname, 
					   apr_fileperms_t perms)
{
    privsep_message_t msg;

    if(!privsep_inited || !token)
	return apr_file_perms_set(fname, perms);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_file_perms_set;
    msg.perms = perms;
    return privsep_send_request(&msg, token, fname);
}


static apr_status_t privsep_dir_make(privsep_token_t *token,
				     const char *path,
				     apr_fileperms_t perm, 
				     apr_pool_t *pool)
{
    privsep_message_t msg;

    if(!privsep_inited || !token)
      return apr_dir_make(path, perm, pool);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_dir_make;
    msg.perms = perm;
    return privsep_send_request(&msg, token, path);
}


static apr_status_t privsep_dir_remove(privsep_token_t *token,
				       const char *path,
				       apr_pool_t *pool)
{
    privsep_message_t msg;

    if(!privsep_inited || !token)
      return apr_dir_remove(path, pool);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_dir_remove;
    return privsep_send_request(&msg, token, path);
}


static apr_status_t privsep_file_remove(privsep_token_t *token,
					const char *path,
					apr_pool_t *pool)
{
    privsep_message_t msg;

    if(!privsep_inited || !token)
      return apr_file_remove(path, pool);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_file_remove;
    return privsep_send_request(&msg, token, path);
}


static apr_status_t privsep_file_rename(privsep_token_t *token,
					const char *from_path, 
					const char *to_path,
					apr_pool_t *p)
{
    privsep_message_t msg;

    if(!privsep_inited || !token)
      return apr_file_rename(from_path, to_path, p);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_file_rename;
    return privsep_send_request(&msg, token, from_path, to_path);
}


static apr_status_t privsep_file_open(privsep_token_t *token,
				      apr_file_t **new, 
				      const char *fname, 
				      apr_int32_t flag, 
				      apr_fileperms_t perm, 
				      apr_pool_t *pool)
{
    privsep_message_t msg;
    apr_os_file_t fd;
    apr_status_t rv;

    if(!privsep_inited || !token)
      return apr_file_open(new, fname, flag, perm, pool);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_file_open;
    msg.flags = flag;
    msg.perms = perm;
    fd = privsep_send_request(&msg, token, fname);

    if (msg.retval != APR_SUCCESS)
        return msg.retval;

    rv = apr_os_file_put(new, &fd, flag, pool);

    if (!(flag & APR_FILE_NOCLEANUP)) {
	apr_pool_cleanup_register(pool, (void *)(*new), 
				  apr_unix_file_cleanup, 
				  apr_unix_child_file_cleanup);
    }
    return rv;
}


static apr_status_t privsep_dir_open(privsep_token_t *token,
				     apr_dir_t **new, const char *dirname, 
				     apr_pool_t *pool)
{
    privsep_message_t msg;
    apr_os_file_t fd;
    DIR *dir;

    if(!privsep_inited || !token)
      return apr_dir_open(new, dirname, pool);

    memset(&msg, 0, sizeof(msg));
    msg.command = privsep_command_dir_open;
    fd = privsep_send_request(&msg, token, dirname);

    if (msg.retval != APR_SUCCESS)
        return msg.retval;

    /* apr_os_dir_put does not create a fullly allocated apr_dir_t
       so we need to open a directory to create one then replace
       the fd in the associated apr_os_dir */
    if(apr_dir_open(new, "/", pool) != APR_SUCCESS) {
	close(fd);
	return APR_EGENERAL;
    }

    /* Ok, now the nasty bit:
       Here we assume that an integer file descriptor is the first 
       element of the opaque DIR structure pointed at by dir
       This is the case on Linux, FreeBSD, Mac OS X and Solaris */
    apr_os_dir_get(&dir, *new);
    close(*(int *)dir);  
    *(int *)dir = fd;
    (*new)->dirname = apr_pstrdup(pool, dirname);
    return apr_os_dir_put(new, dir, pool);
}


static apr_status_t privsep_dir_read(privsep_token_t *token,
                                     apr_finfo_t *finfo,
                                     apr_int32_t wanted,
                                     apr_dir_t *thedir)
{
    apr_status_t ret = 0;
    apr_int32_t stat_wanted = wanted & APR_FINFO_NORM;
    wanted &= ~APR_FINFO_NORM;
    const char *savename;

    /* mask out any flags that would cause a call to apr_stat */
    if((ret = apr_dir_read(finfo, wanted, thedir)) != APR_SUCCESS)
	return ret;
    stat_wanted &= ~finfo->valid;

    /* now do the ap_privsep_stat */
    savename = finfo->name;
    stat_wanted &= ~APR_FINFO_NAME;
    if (stat_wanted)
    {
        char fspec[APR_PATH_MAX];
        char *end;

        end = apr_cpystrn(fspec, thedir->dirname, sizeof fspec);

        if (end > fspec && end[-1] != '/' && (end < fspec + APR_PATH_MAX))
            *end++ = '/';

        apr_cpystrn(end, finfo->name, sizeof fspec - (end - fspec));

        ret = ap_privsep_stat(token, finfo, fspec,
			      APR_FINFO_LINK | stat_wanted, thedir->pool);
	stat_wanted &= ~finfo->valid;
    }
    finfo->name = savename;
    finfo->valid |= APR_FINFO_NAME;

    if (stat_wanted)
        return APR_INCOMPLETE;

    return ret;
}


/* install file io hooks */

static ap_privsep_hooks_t hooks = {
    privsep_get_preauth_stat_token,
    privsep_get_token,
    privsep_stat,
    privsep_file_perms_set,
    privsep_dir_make,
    privsep_dir_remove,
    privsep_file_remove,
    privsep_file_rename,
    privsep_file_open,
    privsep_dir_open,
    privsep_dir_read
};

void ap_privsep_register_file_io_hooks()
{
    ap_privsep_set_hooks(&hooks);
}
