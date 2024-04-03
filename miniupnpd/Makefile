# $Id: Makefile,v 1.3 2009-04-14 22:19:22 jmaggard Exp $
# http://miniupnp.free.fr/
# Author : Thomas Bernard
# for use with GNU Make
CFLAGS = -Wall -g -O2 -D_GNU_SOURCE
#CFLAGS = -Wall -Os -D_GNU_SOURCE
CC = gcc
RM = rm -f
INSTALL = install

BASEOBJS = miniupnpd.o upnphttp.o upnpdescgen.o upnpsoap.o \
           upnpreplyparse.o minixml.o \
	   getifaddr.o upnpglobalvars.o

LNXOBJS = linux/getifstats.o

TESTUPNPDESCGENOBJS = testupnpdescgen.o upnpdescgen.o

#EXECUTABLES = miniupnpd testupnpdescgen testgetifstats
EXECUTABLES = miniupnpd

.PHONY:	all clean install depend

all:	$(EXECUTABLES)

clean:
	$(RM) $(BASEOBJS) $(LNXOBJS) $(EXECUTABLES) upnpd
	$(RM) testupnpdescgen.o testgetifstats.o

install:	miniupnpd
	@echo no install at the moment...

miniupnpd:	$(BASEOBJS) $(LNXOBJS) $(LIBS)
	gcc   miniupnpd.o upnphttp.o upnpdescgen.o upnpsoap.o upnpreplyparse.o minixml.o getifaddr.o upnpglobalvars.o linux/getifstats.o   -o miniupnpd
	ln -f miniupnpd upnpd
	strip --remove-section=.note --remove-section=.comment miniupnpd

testupnpdescgen:	$(TESTUPNPDESCGENOBJS)

testgetifstats:	testgetifstats.o linux/getifstats.o

config.h:	genconfig.sh
	./genconfig.sh

depend:	config.h
	makedepend -f$(MAKEFILE_LIST) -Y \
	$(BASEOBJS:.o=.c) $(LNXOBJS:.o=.c) $(TESTUPNPDESCGENOBJS:.o=.c) \
	testgetifstats.c 2>/dev/null

# DO NOT DELETE

miniupnpd.o: upnpglobalvars.h upnphttp.h config.h upnpdescgen.h
miniupnpd.o: miniupnpdpath.h getifaddr.h upnpsoap.h
upnphttp.o: upnphttp.h config.h upnpdescgen.h miniupnpdpath.h upnpsoap.h
upnpdescgen.o: upnpdescgen.h miniupnpdpath.h upnpglobalvars.h
upnpdescgen.o: upnpdescstrings.h config.h
upnpsoap.o: upnpglobalvars.h upnphttp.h config.h upnpsoap.h upnpreplyparse.h
upnpsoap.o: openbsd/getifstats.h
upnpreplyparse.o: upnpreplyparse.h minixml.h
upnpglobalvars.o: upnpglobalvars.h
linux/getifstats.o: openbsd/getifstats.h
testupnpdescgen.o: upnpdescgen.h
upnpdescgen.o: upnpdescgen.h miniupnpdpath.h upnpglobalvars.h
upnpdescgen.o: upnpdescstrings.h config.h
testgetifstats.o: openbsd/getifstats.h
