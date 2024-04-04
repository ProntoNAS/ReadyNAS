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

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "mod_auth.h"

#include "mod_privsep.h"
#include "privsep.h"


static authn_status authn_privsep_check_password(request_rec *r,
						 const char *user, const char *password)
{
  privsep_token_t *token = ap_privsep_token(r);

  /* subrequest have the token copied from their parent request -
     if one exists we ask the privsep process to check to make sure
     nothing suspicious is going on */
  if(token)
    return ap_privsep_verify_token(token, user);
  else
    return ap_privsep_authenticate(r, user, password);
}

static const authn_provider authn_privsep_provider =
{
    &authn_privsep_check_password,
};

static void register_hooks(apr_pool_t *p)
{
     /* Register authn provider */
    ap_register_provider(p, AUTHN_PROVIDER_GROUP, "privsep", "0",
                         &authn_privsep_provider);
}

module AP_MODULE_DECLARE_DATA authn_privsep_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,                          /* dir config creater */
    NULL,                          /* dir merger --- default is to override */
    NULL,                          /* server config */
    NULL,                          /* merge server config */
    NULL,                          /* command apr_table_t */
    register_hooks                 /* register hooks */
};
