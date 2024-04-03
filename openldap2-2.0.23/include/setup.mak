# $OpenLDAP: pkg/ldap/include/setup.mak,v 1.2.6.4 2002/01/04 20:38:16 kurt Exp $
#
# Copyright 1998-2002 The OpenLDAP Foundation, All Rights Reserved.
# COPYING RESTRICTIONS APPLY, see COPYRIGHT file
#

.SUFFIXES: .nt

.nt.h:
	copy $*.nt $*.h

all: setup.txt

setup.txt: lber_types.h ldap_config.h ldap_features.h portable.h setup.mak
	copy setup.mak setup.txt

lber_types.h: lber_types.nt
ldap_config.h: ldap_config.nt
ldap_features.h: ldap_features.nt
portable.h: portable.nt
