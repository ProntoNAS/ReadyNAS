/* compare.c - DNS SRV backend compare function */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-dnssrv/compare.c,v 1.8.2.3 2002/01/04 20:38:32 kurt Exp $ */
/*
 * Copyright 2000-2002 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */

#include "portable.h"

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"
#include "back-dnssrv.h"

int
dnssrv_back_compare(
    Backend	*be,
    Connection	*conn,
    Operation	*op,
    const char	*dn,
    const char	*ndn,
	AttributeAssertion *ava
)
{
	assert( get_manageDSAit( op ) );

	/* not implemented */

	return LDAP_OTHER;
}
