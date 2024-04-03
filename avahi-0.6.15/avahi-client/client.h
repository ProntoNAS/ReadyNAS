#ifndef fooclienthfoo
#define fooclienthfoo

/* $Id: client.h 1261 2006-08-22 02:15:27Z lennart $ */

/***
  This file is part of avahi.
 
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
 
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#include <inttypes.h>

#include <avahi-common/cdecl.h>
#include <avahi-common/address.h>
#include <avahi-common/strlst.h>
#include <avahi-common/defs.h>
#include <avahi-common/watch.h>
#include <avahi-common/gccmacro.h>

/** \file client.h Definitions and functions for the client API over D-Bus */

AVAHI_C_DECL_BEGIN

/** A connection context */
typedef struct AvahiClient AvahiClient;

/** States of a client object, a superset of AvahiServerState */
typedef enum {
    AVAHI_CLIENT_S_REGISTERING = AVAHI_SERVER_REGISTERING,  /**< Server state: REGISTERING */
    AVAHI_CLIENT_S_RUNNING = AVAHI_SERVER_RUNNING,          /**< Server state: RUNNING */
    AVAHI_CLIENT_S_COLLISION = AVAHI_SERVER_COLLISION,      /**< Server state: COLLISION */
    AVAHI_CLIENT_FAILURE = 100,                             /**< Some kind of error happened on the client side */
    AVAHI_CLIENT_CONNECTING = 101                           /**< We're still connecting. This state is only entered when AVAHI_CLIENT_NO_FAIL has been passed to avahi_client_new() and the daemon is not yet available. */  
} AvahiClientState;

typedef enum {
    AVAHI_CLIENT_IGNORE_USER_CONFIG = 1, /**< Don't read user configuration */
    AVAHI_CLIENT_NO_FAIL = 2        /**< Don't fail if the daemon is not available when avahi_client_new() is called, instead enter AVAHI_CLIENT_CONNECTING state and wait for the daemon to appear */
} AvahiClientFlags;

/** The function prototype for the callback of an AvahiClient */
typedef void (*AvahiClientCallback) (
    AvahiClient *s,
    AvahiClientState state /**< The new state of the client */,
    void* userdata /**< The user data that was passed to avahi_client_new() */);

/** Creates a new client instance */
AvahiClient* avahi_client_new (
    const AvahiPoll *poll_api /**< The abstract event loop API to use */,
    AvahiClientFlags flags /**< Some flags to modify the behaviour of  the client library */,
    AvahiClientCallback callback /**< A callback that is called whenever the state of the client changes. This may be NULL */,
    void *userdata /**< Some arbitrary user data pointer that will be passed to the callback function */,
    int *error /**< If creation of the client fails, this integer will contain the error cause. May be NULL if you aren't interested in the reason why avahi_client_new() failed. */);

/** Free a client instance. This will automatically free all
 * associated browser, resolve and entry group objects. All pointers
 * to such objects become invalid! */
void avahi_client_free(AvahiClient *client);

/** Get the version of the server */
const char* avahi_client_get_version_string (AvahiClient*);

/** Get host name */
const char* avahi_client_get_host_name (AvahiClient*);

/** Set host name. \since 0.6.13 */
int avahi_client_set_host_name(AvahiClient*, const char *name);

/** Get domain name */
const char* avahi_client_get_domain_name (AvahiClient*);

/** Get FQDN domain name */
const char* avahi_client_get_host_name_fqdn (AvahiClient*);

/** Get state */
AvahiClientState avahi_client_get_state(AvahiClient *client); 

/** Get the last error number */
int avahi_client_errno (AvahiClient*);

/** Return the local service cookie. returns AVAHI_SERVICE_COOKIE_INVALID on failure. */
uint32_t avahi_client_get_local_service_cookie(AvahiClient *client);

/** Return 1 if gethostbyname() supports mDNS lookups, 0 otherwise. \since 0.6.5 */
int avahi_nss_support(void);

AVAHI_C_DECL_END

#endif
