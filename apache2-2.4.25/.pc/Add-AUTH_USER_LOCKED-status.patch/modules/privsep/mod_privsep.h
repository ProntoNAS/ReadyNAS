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

#ifndef MOD_PRIVSEP_H
#define MOD_PRIVSEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "apr.h"
#include "apr_file_io.h"
#include "apr_md5.h"
#include "apr_portable.h"


#define PRIVSEP_AUTH_KEY_LEN            16
#define PRIVSEP_TOKEN_SALT_LEN          8
#define PRIVSEP_USERNAME_LEN            32
#define PRIVSEP_AUTH_FAIL_DELAY         2000000 /* usecs */
#define PRIVSEP_BUF_SIZE                (sizeof(privsep_message_t) + PATH_MAX * 2)
#define PRIVSEP_PAM_SERVICE_NAME        "apache"
#define PRIVSEP_TOKEN_NOTE              "privsep-token"


#if APR_HAS_THREADS
extern apr_thread_mutex_t *privsep_worker_mutex;
#define privsep_worker_mutex_lock() \
  apr_thread_mutex_lock(privsep_worker_mutex)
#define privsep_worker_mutex_unlock() \
  apr_thread_mutex_unlock(privsep_worker_mutex)
#else
#define privsep_worker_mutex_lock() (APR_SUCCESS)
#define privsep_worker_mutex_unlock() (APR_SUCCESS)
#endif

/* definitions */

/* privileged command type */
typedef enum {
  privsep_command_authenticate,
  privsep_command_verify_token,
  privsep_command_file_open,
  privsep_command_dir_open,
  privsep_command_file_perms_set,
  privsep_command_stat,
  privsep_command_dir_make,
  privsep_command_file_rename,
  privsep_command_dir_remove,
  privsep_command_file_remove
} privsep_command_t;

typedef enum {
    PRIVSEP_AUTH_DENIED,
    PRIVSEP_AUTH_GRANTED,
    PRIVSEP_AUTH_USER_FOUND,
    PRIVSEP_AUTH_USER_NOT_FOUND,
    PRIVSEP_AUTH_GENERAL_ERROR
} privsep_authn_status;

/* authinfo for PAM */
typedef struct {
  const char *user;
  const char *pass;
} auth_pam_userinfo;

/* privileged token with cyrpto hash */
typedef struct {
  uid_t uid;
  gid_t gid;
  unsigned char salt[PRIVSEP_TOKEN_SALT_LEN];
  unsigned char digest[APR_MD5_DIGESTSIZE];
} privsep_token_t;
#define HAVE_PRIVSEP_TOKEN_T

/* privileged command message */
typedef struct {
  privsep_command_t             command;
  int                           retval;
  int                           flags;
  apr_fileperms_t               perms;
  apr_finfo_t                   finfo;
  privsep_token_t               token;
  char                          rhost[64];  /* remote hostname */
  /*
    TODO - add remote address into request so that we can make
    token hash include address so it can not be used for a
    request being made from another address

    struct in_addr             remote;
  */
} privsep_message_t;

/* module directory config */
typedef struct {
  int enabled;
} privsep_dir_conf;


/* globals */
extern int privsep_enabled;
extern int privsep_inited;
extern int privsep_client_fd;
extern privsep_token_t *privsep_root_token;
extern int privsep_worker_sp[2];


/* module directory configuration interface used by wrappers */

int privsep_is_enabled(const request_rec *r);

/* setup message header and control message to send fd */

void privsep_send_fd(struct msghdr *mh, char *buf, int fd);


/* privileged wrapper client functions used by authn-privsep module */

AP_DECLARE(privsep_authn_status) ap_privsep_authenticate(const request_rec *r,
							 const char *user,
							 const char *pass);

AP_DECLARE(privsep_authn_status) ap_privsep_verify_token(privsep_token_t *token,
							 const char *user);

/* install file io privsep hooks */

void ap_privsep_register_file_io_hooks();


#ifdef __cplusplus
}
#endif

#endif	/* !MOD_PRIVSEP_H */
