/* $OpenLDAP: pkg/ldap/libraries/liblutil/memcmp.c,v 1.2.8.2 2000/06/13 17:57:25 kurt Exp $ */
#include "portable.h"

#include <ac/string.h>

/* 
 * Memory Compare
 */
int
(memcmp)(const void *v1, const void *v2, int n) 
{
    if (n != 0) {
		const unsigned char *s1=v1, *s2=v2;
        do {
            if (*s1++ != *s2++)
                return (*--s1 - *--s2);
        } while (--n != 0);
    }
    return (0);
} 
