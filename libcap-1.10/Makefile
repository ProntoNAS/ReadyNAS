#
# $Id: Makefile,v 1.1.1.1 1999/04/17 22:16:31 morgan Exp $
#
# Makefile for libcap

ifndef topdir
topdir=$(shell pwd)
endif
include $(topdir)/Make.Rules
DESTDIR=

#
# flags
#

all install clean: %: %-here
	make -C $(topdir)/libcap $(MAKE_DEFS) $@
	make -C $(topdir)/progs $(MAKE_DEFS) $@
	make -C $(topdir)/doc $(MAKE_DEFS) $@

all-here:

install-here:

clean-here:
	$(LOCALCLEAN)

distclean: clean
	$(DISTCLEAN)

release: distclean
	cd .. ; tar cvfz libcap-$(VERSION).$(MINOR).tar.gz libcap-$(VERSION).$(MINOR)
