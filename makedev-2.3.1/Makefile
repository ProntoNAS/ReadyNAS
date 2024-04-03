# $Id: Makefile,v 2.2 1995/05/21 17:45:18 alfie Exp $

BINDIR  = $(ROOT)/sbin
DEVDIR  = $(ROOT)/dev
MANDIR  = $(ROOT)/usr/share/man
MANEXT  = 8

default:
	@echo Nothing to make.
	
install:
	install -m 755 MAKEDEV $(BINDIR)
	install -m 644 MAKEDEV.man $(MANDIR)/man$(MANEXT)/MAKEDEV.$(MANEXT)

clean:
