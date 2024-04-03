/* unbind.c - handle an ldap unbind operation */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldbm/unbind.c,v 1.3.8.4 2002/01/04 20:38:35 kurt Exp $ */
/*
 * Copyright 1998-2002 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */

#include "portable.h"

#include <stdio.h>
#include <ac/socket.h>

#include "slap.h"

void
ldbm_back_unbind(
	Backend     *be,
	Connection  *conn,
	Operation   *op
)
{
}
