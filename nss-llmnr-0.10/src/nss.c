/* $Id: nss.c 115 2007-05-12 14:43:48Z lennart $ */

/***
    This file is part of nss-llmnr.
 
    nss-llmnr is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2 of the License,
    or (at your option) any later version.
 
    nss-llmnr is distributed in the hope that it will be useful, but1
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public License
    along with nss-llmnr; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
    USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <netdb.h>
#include <sys/socket.h>
#include <nss.h>
#include <stdio.h>
#include <stdlib.h>

#include "query.h"


#if defined(NSS_IPV4_ONLY)
#define _nss_llmnr_gethostbyname2_r _nss_llmnr4_gethostbyname2_r
#define _nss_llmnr_gethostbyname_r  _nss_llmnr4_gethostbyname_r
#define _nss_llmnr_gethostbyaddr_r  _nss_llmnr4_gethostbyaddr_r
#elif defined(NSS_IPV6_ONLY)
#define _nss_llmnr_gethostbyname2_r _nss_llmnr6_gethostbyname2_r
#define _nss_llmnr_gethostbyname_r  _nss_llmnr6_gethostbyname_r
#define _nss_llmnr_gethostbyaddr_r  _nss_llmnr6_gethostbyaddr_r
#endif

/* Maximum number of entries to return */
#define MAX_ENTRIES 16

/* The resolv.conf page states that they only support 6 domains */
#define MAX_SEARCH_DOMAINS 6

#define ALIGN(idx) do { \
  if (idx % sizeof(void*)) \
    idx += (sizeof(void*) - idx % sizeof(void*)); /* Align on word boundary */ \
} while(0)

struct userdata {
    int count;
    int data_len; /* only valid when doing reverse lookup */
    union  {
        ipv4_address_t ipv4[MAX_ENTRIES];
        ipv6_address_t ipv6[MAX_ENTRIES];
        char *name[MAX_ENTRIES];
    } data;
};

#ifndef NSS_IPV6_ONLY
static void ipv4_callback(const ipv4_address_t *ipv4, void *userdata) {
    struct userdata *u = userdata;
    assert(ipv4 && userdata);

    if (u->count >= MAX_ENTRIES)
        return;

    u->data.ipv4[u->count++] = *ipv4;
    u->data_len += sizeof(ipv4_address_t);
}
#endif

#ifndef NSS_IPV4_ONLY
static void ipv6_callback(const ipv6_address_t *ipv6, void *userdata) {
    struct userdata *u = userdata;
    assert(ipv6 && userdata);

    if (u->count >= MAX_ENTRIES)
        return;

    u->data.ipv6[u->count++] = *ipv6;
    u->data_len += sizeof(ipv6_address_t);
}
#endif

static void name_callback(const char*name, void *userdata) {
    struct userdata *u = userdata;
    assert(name && userdata);

    if (u->count >= MAX_ENTRIES)
        return;

    u->data.name[u->count++] = strdup(name);
    u->data_len += strlen(name)+1;
}

#ifdef HONOUR_SEARCH_DOMAINS

static char **alloc_domains(unsigned ndomains) {
    char **domains;

    if (!(domains = malloc(sizeof(char*) * ndomains)))
        return NULL;

    /* initialize them all to 0 */
    memset(domains, 0, sizeof(char*) * ndomains);
    return domains;
}

static void free_domains(char **domains) {
    char **p;

    if (!domains)
        return;

    for(p = domains; *p; p++) 
        free(*p);

    free(domains);
}

static char** parse_domains(const char *domains_in) {
    /* leave room for the NULL terminator */
    char **domains_out;
    const char *start = domains_in;
    unsigned domain = 0;

    if (!(domains_out = alloc_domains(MAX_SEARCH_DOMAINS+1)))
        return NULL;

    while (domain < MAX_SEARCH_DOMAINS) {
        const char *end;
        char *tmp;
        size_t domain_len;
        
        end = start + strcspn(start, " \t\r\n");
        domain_len = (end - start);

        if (!(tmp = malloc(domain_len + 1)))
            break;
        
        memcpy(tmp, start, domain_len);
        tmp[domain_len] = '\0';

        domains_out[domain++] = tmp;

        end += strspn(end," \t\r\n");

        if (!*end)
            break;
        
        start = end;
    }

    return domains_out;
}

static char** get_search_domains(void) {
    FILE *f = 0;
    char **domains = NULL;

    /* according to the resolv.conf man page (in Linux) the LOCALDOMAIN
       environment variable should override the settings in the resolv.conf file */
    char *line = getenv("LOCALDOMAIN");
    if (line && *line != 0)
        return parse_domains(line);
    
    if (!(f = fopen(RESOLV_CONF_FILE, "r")))
        return NULL;

    while (!feof(f)) {
        char *start = NULL;
        char ln[512];
	  
        if (!fgets(ln, sizeof(ln), f))
            break;

        start = ln + strspn(ln, " \t\r\n");
    
        if (strncmp(start, "search", 6) && strncmp(start, "domain", 6))
            continue;
        
        if (start[6] != ' ' && start[6] != '\t')
            continue;

        /* scan to the end of the keyword ('search' or 'domain' currently) */
        start += strcspn(start, " \t\r\n");

        /* find the begining of the first domain in the list */
        start += strspn(start, " \t\r\n");

        /* the resolv.conf manpage also states that 'search' and 'domain' are mutually exclusive
           and that the last one wins. */
        free_domains(domains);
        domains = parse_domains(start);
    }

    fclose(f);

    return domains;
}

#endif

enum nss_status _nss_llmnr_gethostbyname2_r(
    const char *name,
    int af,
    struct hostent * result,
    char *buffer,
    size_t buflen,
    int *errnop,
    int *h_errnop) {

    struct userdata u;
    enum nss_status status = NSS_STATUS_UNAVAIL;
    int i;
    size_t address_length, l, idx, astart;
    void (*ipv4_func)(const ipv4_address_t *ipv4, void *userdata);
    void (*ipv6_func)(const ipv6_address_t *ipv6, void *userdata);
    int name_allowed;



    int fd = -1;

/*     DEBUG_TRAP; */

    if (af == AF_UNSPEC)
#ifdef NSS_IPV6_ONLY
        af = AF_INET6;
#else
        af = AF_INET;
#endif

#ifdef NSS_IPV4_ONLY
    if (af != AF_INET) 
#elif NSS_IPV6_ONLY
    if (af != AF_INET6)
#else        
    if (af != AF_INET && af != AF_INET6)
#endif        
    {    
        *errnop = EINVAL;
        *h_errnop = NO_RECOVERY;

        goto finish;
    }

    address_length = af == AF_INET ? sizeof(ipv4_address_t) : sizeof(ipv6_address_t);
    if (buflen <
        sizeof(char*)+    /* alias names */
        strlen(name)+1)  {   /* official name */
        
        *errnop = ERANGE;
        *h_errnop = NO_RECOVERY;
        status = NSS_STATUS_TRYAGAIN;
        
        goto finish;
    }
    
    u.count = 0;
    u.data_len = 0;

#ifdef NSS_IPV6_ONLY
    ipv4_func = NULL;
#else
    ipv4_func = af == AF_INET ? ipv4_callback : NULL;
#endif    

#ifdef NSS_IPV4_ONLY
    ipv6_func = NULL;
#else
    ipv6_func = af == AF_INET6 ? ipv6_callback : NULL;
#endif

    name_allowed = 1;

    {
        if ((fd = llmnr_open_socket()) < 0) {
            *errnop = errno;
            *h_errnop = NO_RECOVERY;
            goto finish;
        }

        if (name_allowed) {
            /* Ignore return value */
            llmnr_query_name(fd, name, ipv4_func, ipv6_func, &u);

            if (!u.count)
                status = NSS_STATUS_NOTFOUND;
        }

#ifdef HONOUR_SEARCH_DOMAINS
        if (u.count == 0 && !ends_with(name, ".")) {
            char **domains;
            
            /* Try the search domains if the user did not use a traling '.' */
            
            if ((domains = get_search_domains())) {
                char **p;
                
                for (p = domains; *p; p++) {
                    int fullnamesize = 0;
                    char *fullname = NULL;
                    
                    fullnamesize = strlen(name) + strlen(*p) + 2;
                    if (!(fullname = malloc(fullnamesize)))
                        break;
                    
                    snprintf(fullname, fullnamesize, "%s.%s", name, *p);
                    
                    if (verify_name_allowed(fullname)) {
                        
                        /* Ignore return value */
                        llmnr_query_name(fd, fullname, ipv4_func, ipv6_func, &u);
                        
                        if (u.count > 0) {
                            /* We found something, so let's quit */
                            free(fullname);
                            break;
                        } else
                            status = NSS_STATUS_NOTFOUND;

                    }
                    
                    free(fullname);
                }
                
                free_domains(domains);
	    }
        }
#endif /* HONOUR_SEARCH_DOMAINS */
    }

    if (u.count == 0) {
        *errnop = ETIMEDOUT;
        *h_errnop = HOST_NOT_FOUND;
        goto finish;
    }
    
    /* Alias names */
    *((char**) buffer) = NULL;
    result->h_aliases = (char**) buffer;
    idx = sizeof(char*);
    
    /* Official name */
    strcpy(buffer+idx, name); 
    result->h_name = buffer+idx;
    idx += strlen(name)+1;

    ALIGN(idx);
    
    result->h_addrtype = af;
    result->h_length = address_length;
    
    /* Check if there's enough space for the addresses */
    if (buflen < idx+u.data_len+sizeof(char*)*(u.count+1)+sizeof(void*)) {
        *errnop = ERANGE;
        *h_errnop = NO_RECOVERY;
        status = NSS_STATUS_TRYAGAIN;
        goto finish;
    }

    /* Addresses */
    astart = idx;
    l = u.count*address_length;
    memcpy(buffer+astart, &u.data, l);
    idx += l;
    /* realign, whilst the address is a multiple of 32bits, we
     * frequently lose alignment for 64bit systems */
    ALIGN(idx);

    /* Address array address_lenght is always a multiple of 32bits */
    for (i = 0; i < u.count; i++)
        ((char**) (buffer+idx))[i] = buffer+astart+address_length*i;
    ((char**) (buffer+idx))[i] = NULL;
    result->h_addr_list = (char**) (buffer+idx);

    status = NSS_STATUS_SUCCESS;
    
finish:
    if (fd >= 0)
        close(fd);

    return status;
}

enum nss_status _nss_llmnr_gethostbyname_r (
    const char *name,
    struct hostent *result,
    char *buffer,
    size_t buflen,
    int *errnop,
    int *h_errnop) {
    return _nss_llmnr_gethostbyname2_r(
        name,
        AF_UNSPEC,
        result,
        buffer,
        buflen,
        errnop,
        h_errnop);
}

enum nss_status _nss_llmnr_gethostbyaddr_r(
    const void* addr,
    int len,
    int af,
    struct hostent *result,
    char *buffer,
    size_t buflen,
    int *errnop,
    int *h_errnop) {
    
    struct userdata u;
    enum nss_status status = NSS_STATUS_UNAVAIL;
    int r;
    size_t address_length, idx, astart;
    
    int fd = -1;

    *errnop = EINVAL;
    *h_errnop = NO_RECOVERY;

    u.count = 0;
    u.data_len = 0;

    /* Check for address types */
    address_length = af == AF_INET ? sizeof(ipv4_address_t) : sizeof(ipv6_address_t);

    if (len < (int) address_length ||
#ifdef NSS_IPV4_ONLY
        af != AF_INET
#elif NSS_IPV6_ONLY
        af != AF_INET6
#else        
        (af != AF_INET && af != AF_INET6)
#endif
        ) {
        *errnop = EINVAL;
        *h_errnop = NO_RECOVERY;

        goto finish;
    }

    /* Check for buffer space */
    if (buflen <
        sizeof(char*)+      /* alias names */
        address_length) {   /* address */
        
        *errnop = ERANGE;
        *h_errnop = NO_RECOVERY;
        status = NSS_STATUS_TRYAGAIN;

        goto finish;
    }

     /* Lookup using legacy mDNS queries */   
     {
        if ((fd = llmnr_open_socket()) < 0) {
            *errnop = errno;
            *h_errnop = NO_RECOVERY;
            goto finish;
        }

	r = -1;

#if ! defined(NSS_IPV6_ONLY) && ! defined(NSS_IPV4_ONLY)
        if (af == AF_INET)
#endif
#ifndef NSS_IPV6_ONLY
            r = llmnr_query_ipv4(fd, (const ipv4_address_t*) addr, name_callback, &u);
#endif
#if ! defined(NSS_IPV6_ONLY) && ! defined(NSS_IPV4_ONLY)
        else
#endif
#ifndef NSS_IPV4_ONLY
            r = llmnr_query_ipv6(fd, (const ipv6_address_t*) addr, name_callback, &u);
#endif
        if (r < 0) {
            *errnop = ETIMEDOUT;
            *h_errnop = HOST_NOT_FOUND;
            status = NSS_STATUS_NOTFOUND;
            goto finish;
        }
    }

    if (u.count == 0) {
        *errnop = ETIMEDOUT;
        *h_errnop = NO_RECOVERY;
        goto finish;
    }

    /* Alias names, assuming buffer starts a nicely aligned offset */
    *((char**) buffer) = NULL;
    result->h_aliases = (char**) buffer;
    idx = sizeof(char*);

    assert(u.count > 0);
    assert(u.data.name[0]);
    
    if (buflen <
        strlen(u.data.name[0])+1+ /* official names */
        sizeof(char*)+ /* alias names */
        address_length+  /* address */
        sizeof(void*)*2 + /* address list */
        sizeof(void*)) {  /* padding to get the alignment right */

        *errnop = ERANGE;
        *h_errnop = NO_RECOVERY;
        status = NSS_STATUS_TRYAGAIN;
        goto finish;
    }
    
    /* Official name */
    strcpy(buffer+idx, u.data.name[0]); 
    result->h_name = buffer+idx;
    idx += strlen(u.data.name[0])+1;
    
    result->h_addrtype = af;
    result->h_length = address_length;

    /* Address */
    astart = idx;
    memcpy(buffer+astart, addr, address_length);
    idx += address_length;

    /* Address array, idx might not be at pointer alignment anymore, so we need
     * to ensure it is*/
    ALIGN(idx);

    ((char**) (buffer+idx))[0] = buffer+astart;
    ((char**) (buffer+idx))[1] = NULL;
    result->h_addr_list = (char**) (buffer+idx);

    status = NSS_STATUS_SUCCESS;
    
finish:
    if (fd >= 0)
        close(fd);

    return status;
}

