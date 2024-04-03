# $OpenLDAP: pkg/ldap/build/lib-static.mk,v 1.2.8.3 2002/01/04 20:38:06 kurt Exp $
## Copyright 1998-2002 The OpenLDAP Foundation
## COPYING RESTRICTIONS APPLY.  See COPYRIGHT File in top level directory
## of this package for details.
##---------------------------------------------------------------------------
##
## Makefile Template for Static Libraries
##

$(LIBRARY): version.o
	$(AR) ru $@ $(OBJS) version.o
	@$(RANLIB) $@;	\
	$(RM) ../$@;	\
	(d=`$(PWD)` ; cd .. ; $(LN_S) `$(BASENAME) $$d`/$@ $@)

Makefile: $(top_srcdir)/build/lib-static.mk
