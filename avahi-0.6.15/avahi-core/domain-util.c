/* $Id: domain-util.c 1194 2006-04-24 00:32:27Z lennart $ */

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <stdio.h>

#include <avahi-common/malloc.h>

#include "domain-util.h"
#include "util.h"

static void strip_bad_chars(char *s) {
    char *p, *d;

    s[strcspn(s, ".")] = 0;
    
    for (p = s, d = s; *p; p++) 
        if ((*p >= 'a' && *p <= 'z') ||
            (*p >= 'A' && *p <= 'Z') ||
            (*p >= '0' && *p <= '9') ||
            *p == '-')
            *(d++) = *p;

    *d = 0;
}

char *avahi_get_host_name(char *ret_s, size_t size) {
    assert(ret_s);
    assert(size > 0);

    if (gethostname(ret_s, size) >= 0) {
        ret_s[size-1] = 0;
        strip_bad_chars(ret_s);
    } else
        *ret_s = 0;

    if (*ret_s == 0) {
        struct utsname utsname;
        
        /* No hostname was set, so let's take the OS name */

        if (uname(&utsname) >= 0) {
            snprintf(ret_s, size, "%s", utsname.sysname);
            strip_bad_chars(ret_s);
            avahi_strdown(ret_s);
        }

        if (*ret_s == 0)
            snprintf(ret_s, size, "unnamed");
    }

    if (size >= AVAHI_LABEL_MAX)
	ret_s[AVAHI_LABEL_MAX-1] = 0;    
    
    return ret_s;
}

char *avahi_get_host_name_strdup(void) {
    char t[AVAHI_DOMAIN_NAME_MAX];

    if (!(avahi_get_host_name(t, sizeof(t))))
        return NULL;

    return avahi_strdup(t);
}

int avahi_binary_domain_cmp(const char *a, const char *b) {
    assert(a);
    assert(b);

    if (a == b)
        return 0;

    for (;;) {
        char ca[AVAHI_LABEL_MAX], cb[AVAHI_LABEL_MAX], *p;
        int r;

        p = avahi_unescape_label(&a, ca, sizeof(ca));
        assert(p);
        p = avahi_unescape_label(&b, cb, sizeof(cb));
        assert(p);

        if ((r = strcmp(ca, cb)))
            return r;
        
        if (!*a && !*b)
            return 0;
    }
}

int avahi_domain_ends_with(const char *domain, const char *suffix) {
    assert(domain);
    assert(suffix);

    for (;;) {
        char dummy[AVAHI_LABEL_MAX], *r;

        if (*domain == 0)
            return 0;
        
        if (avahi_domain_equal(domain, suffix))
            return 1;

        r = avahi_unescape_label(&domain, dummy, sizeof(dummy));
        assert(r);
    } 
}

