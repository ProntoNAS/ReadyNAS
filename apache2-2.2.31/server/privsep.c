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

#include "apr.h"
#include "apr_file_io.h"

#include "httpd.h"

#include "privsep.h"


static ap_privsep_hooks_t *hook = NULL;


/* set hooks for a privsep module to intercept token requests and apr file io */

APR_DECLARE(void) ap_privsep_set_hooks(ap_privsep_hooks_t *hooks)
{
    hook = hooks;
}


/* hookable privsep token functions */

/* returns special preauth stat token for use in ap_directory_walk before
   any authentication has been performed */
AP_DECLARE(privsep_token_t*) ap_privsep_preauth_stat_token(const request_rec *r)
{
    if(hook && hook->get_preauth_token)
	return hook->get_preauth_token(r);
    else
	return NULL;
}

/* returns the privileged token associated with the request by using
   authentication context information from the request */
AP_DECLARE(privsep_token_t*) ap_privsep_token(const request_rec *r)
{
    if(hook && hook->get_token)
	return hook->get_token(r);
    else
	return NULL;
}


/* apr hookable privileged wrapper functions */

APR_DECLARE(apr_status_t) ap_privsep_stat(privsep_token_t *token,
					  apr_finfo_t *finfo, 
					  const char *fname, 
					  apr_int32_t wanted,
					  apr_pool_t *pool)
{
    if(hook && hook->stat)
	return hook->stat(token, finfo, fname, wanted, pool);
    else
	return apr_stat(finfo, fname, wanted, pool);
}

APR_DECLARE(apr_status_t) ap_privsep_file_perms_set(privsep_token_t *token,
						    const char *fname, 
						    apr_fileperms_t perms)
{
    if(hook && hook->file_perms_set)
	return hook->file_perms_set(token, fname, perms);
    else
	return apr_file_perms_set(fname, perms);
}

APR_DECLARE(apr_status_t) ap_privsep_dir_make(privsep_token_t *token,
					      const char *path,
					      apr_fileperms_t perm, 
					      apr_pool_t *pool)
{
    if(hook && hook->dir_make)
	return hook->dir_make(token, path, perm, pool);
    else
	return apr_dir_make(path, perm, pool);
}

APR_DECLARE(apr_status_t) ap_privsep_dir_remove(privsep_token_t *token,
						const char *path,
						apr_pool_t *pool)
{
    if(hook && hook->dir_remove)
	return hook->dir_remove(token, path, pool);
    else
	return apr_dir_remove(path, pool);
}

APR_DECLARE(apr_status_t) ap_privsep_file_remove(privsep_token_t *token,
						 const char *path,
						 apr_pool_t *pool)
{
    if(hook && hook->file_remove)
	return hook->file_remove(token, path, pool);
    else
	return apr_file_remove(path, pool);
}

APR_DECLARE(apr_status_t) ap_privsep_file_rename(privsep_token_t *token,
						 const char *from_path, 
						 const char *to_path,
						 apr_pool_t *p)
{
    if(hook && hook->file_rename)
	return hook->file_rename(token, from_path, to_path, p);
    else
	return apr_file_rename(from_path, to_path, p);  
}

APR_DECLARE(apr_status_t) ap_privsep_file_open(privsep_token_t *token,
					       apr_file_t **new, 
					       const char *fname, 
					       apr_int32_t flag, 
					       apr_fileperms_t perm, 
					       apr_pool_t *pool)
{
    if(hook && hook->file_open)
	return hook->file_open(token, new, fname, flag, perm, pool);
    else
	return apr_file_open(new, fname, flag, perm, pool);
}

APR_DECLARE(apr_status_t) ap_privsep_dir_open(privsep_token_t *token,
					      apr_dir_t **new,
					      const char *dirname, 
					      apr_pool_t *pool)
{
    if(hook && hook->dir_open)
	return hook->dir_open(token, new, dirname, pool);
    else
	return apr_dir_open(new, dirname, pool);
}

APR_DECLARE(apr_status_t) ap_privsep_dir_read(privsep_token_t *token,
					      apr_finfo_t *finfo,
					      apr_int32_t wanted,
					      apr_dir_t *thedir)
{
    if(hook && hook->dir_read)
	return hook->dir_read(token, finfo, wanted, thedir);
    else
	return apr_dir_read(finfo, wanted, thedir);
}
