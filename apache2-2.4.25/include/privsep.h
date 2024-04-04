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

#ifndef PRIVSEP_H
#define PRIVSEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apr.h"
#include "apr_file_io.h"

#ifndef HAVE_PRIVSEP_TOKEN_T
#include "apr_md5.h"
#define PRIVSEP_TOKEN_SALT_LEN          8
typedef struct {
  uid_t uid;
  gid_t gid;
  unsigned char salt[PRIVSEP_TOKEN_SALT_LEN];
  unsigned char digest[APR_MD5_DIGESTSIZE];
} privsep_token_t;
#endif


/* privsep token functions */

AP_DECLARE(privsep_token_t*) ap_privsep_preauth_stat_token(const request_rec *r);

AP_DECLARE(privsep_token_t*) ap_privsep_token(const request_rec *r);


/* apr privileged wrapper functions */

AP_DECLARE(apr_status_t) ap_privsep_stat(privsep_token_t *token,
					 apr_finfo_t *finfo, 
					 const char *fname, 
					 apr_int32_t wanted,
					 apr_pool_t *pool);

AP_DECLARE(apr_status_t) ap_privsep_file_perms_set(privsep_token_t *token,
						    const char *fname, 
						    apr_fileperms_t perms);

AP_DECLARE(apr_status_t) ap_privsep_dir_make(privsep_token_t *token,
					      const char *path,
					      apr_fileperms_t perm, 
					      apr_pool_t *pool);

AP_DECLARE(apr_status_t) ap_privsep_dir_remove(privsep_token_t *token,
						const char *path,
						apr_pool_t *pool);

AP_DECLARE(apr_status_t) ap_privsep_file_remove(privsep_token_t *token,
						 const char *path,
						 apr_pool_t *pool);

AP_DECLARE(apr_status_t) ap_privsep_file_rename(privsep_token_t *token,
						 const char *from_path, 
						 const char *to_path,
						 apr_pool_t *p);

AP_DECLARE(apr_status_t) ap_privsep_file_open(privsep_token_t *token,
					       apr_file_t **new, 
					       const char *fname, 
					       apr_int32_t flag, 
					       apr_fileperms_t perm, 
					       apr_pool_t *pool);

AP_DECLARE(apr_status_t) ap_privsep_dir_open(privsep_token_t *token,
					      apr_dir_t **new,
					      const char *dirname, 
					      apr_pool_t *pool);

AP_DECLARE(apr_status_t) ap_privsep_dir_read(privsep_token_t *token,
					      apr_finfo_t *finfo,
					      apr_int32_t wanted,
					      apr_dir_t *thedir);

/* privileged wrapper typedefs for hook functions */

typedef privsep_token_t* (ap_privsep_preauth_token_fn)(const request_rec *r);

typedef privsep_token_t* (ap_privsep_token_fn)(const request_rec *r);

typedef apr_status_t (ap_privsep_stat_fn)(privsep_token_t *token,
					  apr_finfo_t *finfo, 
					  const char *fname, 
					  apr_int32_t wanted,
					  apr_pool_t *pool);

typedef apr_status_t (ap_privsep_file_perms_set_fn)(privsep_token_t *token,
						    const char *fname, 
						    apr_fileperms_t perms);

typedef apr_status_t (ap_privsep_dir_make_fn)(privsep_token_t *token,
					      const char *path,
					      apr_fileperms_t perm, 
					      apr_pool_t *pool);

typedef apr_status_t (ap_privsep_dir_remove_fn)(privsep_token_t *token,
						const char *path,
						apr_pool_t *pool);

typedef apr_status_t (ap_privsep_file_remove_fn)(privsep_token_t *token,
						 const char *path,
						 apr_pool_t *pool);

typedef apr_status_t (ap_privsep_file_rename_fn)(privsep_token_t *token,
						 const char *from_path, 
						 const char *to_path,
						 apr_pool_t *p);

typedef apr_status_t (ap_privsep_file_open_fn)(privsep_token_t *token,
					       apr_file_t **new, 
					       const char *fname, 
					       apr_int32_t flag, 
					       apr_fileperms_t perm, 
					       apr_pool_t *pool);

typedef apr_status_t (ap_privsep_dir_open_fn)(privsep_token_t *token,
					      apr_dir_t **new,
					      const char *dirname, 
					      apr_pool_t *pool);

typedef apr_status_t (ap_privsep_dir_read_fn)(privsep_token_t *token,
					      apr_finfo_t *finfo,
					      apr_int32_t wanted,
					      apr_dir_t *thedir);

/* privsep apr file io dispatch hook structure */

typedef struct {
    ap_privsep_preauth_token_fn   *get_preauth_token;
    ap_privsep_token_fn           *get_token;
    ap_privsep_stat_fn            *stat;
    ap_privsep_file_perms_set_fn  *file_perms_set;
    ap_privsep_dir_make_fn        *dir_make;
    ap_privsep_dir_remove_fn      *dir_remove;
    ap_privsep_file_remove_fn     *file_remove;
    ap_privsep_file_rename_fn     *file_rename;
    ap_privsep_file_open_fn       *file_open;
    ap_privsep_dir_open_fn        *dir_open;
    ap_privsep_dir_read_fn        *dir_read;
} ap_privsep_hooks_t;

#endif
