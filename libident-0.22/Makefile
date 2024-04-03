#
# Makefile for the libident library
#

MAKE=make
SHELL=/bin/sh
RM=rm -f
RANLIB=ranlib
AR=ar cq

INSTROOT=/usr/local
LIBDIR=$(INSTROOT)/lib
INCDIR=$(INSTROOT)/include
MANDIR=$(INSTROOT)/share/man/man3

#
# Add -g for debugging information
#
# Add -DHAVE_ANSIHEADERS if your system have the ANSI C header files
# needed for our purposes (stdlib, string, unistd).
#
CFLAGS = -O

%.sh.o: %.c
	$(CC) $(CFLAGS) -D_REENTRANT -fPIC -c $*.c -o $@

LDLIBS =
LIBIDENT = libident.a
SHLIBIDENT = libident.so
SHLIB0 = libident.so.0
SHLIB = libident.so.0.22

OBJS = ident.o id_open.o id_close.o id_query.o id_parse.o support.o version.o
SHOBJS=$(OBJS:.o=.sh.o)

#
fail:
	@echo "Select your platform and type 'make <platform>'"
	@echo ""
	@echo aix
	@echo bsd
	@echo bsd2.11
	@echo dynix
	@echo hpux
	@echo irix
	@echo linux
	@echo netstep3.0
	@echo netstep3.1
	@echo sunos4
	@echo sunos5
	@echo svr4
	@echo ultrix
	@echo openvms
	@echo ""
	@echo other


all: 	$(LIBIDENT) $(SHLIBIDENT)

$(LIBIDENT):	$(OBJS)
	-$(RM) $(LIBIDENT)
	$(AR) $(LIBIDENT) $(OBJS)
	-$(RANLIB) $(LIBIDENT)

$(SHLIBIDENT):  $(SHOBJS)
	$(CC) -shared -D_REENTRANT -lc -Wl,-soname,$(SHLIB0) -o $@ $(SHOBJS)

testers: lookup-tester ident-tester

ident-tester: $(LIBIDENT) ident-tester.o
	$(CC) $(CFLAGS) -o ident-tester ident-tester.o $(LIBIDENT) $(LDLIBS)

lookup-tester: $(LIBIDENT) lookup-tester.o
	$(CC) $(CFLAGS) -o lookup-tester lookup-tester.o $(LIBIDENT) $(LDLIBS)


ident.o:	 ident.c         ident.h
id_open.o:	 id_open.c       ident.h
id_close.o:	 id_close.c      ident.h
id_query.o:	 id_query.c      ident.h
id_parse.o:	 id_parse.c      ident.h
support.o:	 support.c	 ident.h
ident-tester.o:  ident-tester.c  ident.h
lookup-tester.o: lookup-tester.c ident.h
version.o:	 version.c

#

version:
	(PACKNAME=`basename \`pwd\`` ; echo 'char id_version[] = "'`echo $$PACKNAME | cut -d- -f2`'";' >version.c)

#

install:
	-$(RM) $(LIBDIR)/$(SHLIB) $(LIBDIR)/$(SHLIB0)
	install -o root -g root -m 644 $(SHLIBIDENT) $(LIBDIR)/$(SHLIB)
	cd $(LIBDIR) && ln -s $(SHLIB) $(SHLIB0)

install-dev:
	-$(RM) $(LIBDIR)/$(LIBIDENT)
	install -o root -g root -m 644 $(LIBIDENT) $(LIBDIR)/$(LIBIDENT)
	-$(RANLIB) $(LIBDIR)/$(LIBIDENT)
	cd $(LIBDIR) && ln -s $(SHLIB0) $(SHLIBIDENT)
	-$(RM) $(INCDIR)/ident.h
	install -o root -g root -m 644 ident.h $(INCDIR)/ident.h
	-$(RM) $(MANDIR)/ident.3
	install -o root -g root -m 644 ident.3 $(MANDIR)/ident.3

clean:
	-$(RM) $(LIBIDENT) ident-tester lookup-tester *~ core *.o \#*
	-$(RM) $(SHLIBIDENT)

#

distZ:	clean version
	(PACKNAME=`basename \`pwd\`` ; cd .. ; /usr/gnu/bin/tar Zcf $$PACKNAME.tar.Z $$PACKNAME)

dist distz:	clean version
	(PACKNAME=`basename \`pwd\`` ; cd .. ; /usr/gnu/bin/tar cf - $$PACKNAME | gzip -9 >$$PACKNAME.tar.gz)



#
# System specific targets ---------------------------------------------
#
sunos5 solaris2 svr4:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -DHAVE_ANSIHEADERS" LDLIBS="-lsocket -lnsl" RANLIB=/bin/true

#
#
sunos sunos4 solaris1 ultrix:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -DHAVE_ANSIHEADERS"


irix:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -ansi -DHAVE_ANSIHEADERS" LDLIBS="-lsocket -lnsl" RANLIB=/bin/true

#
#
bsd:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -DSIGRETURNTYPE=int"

bsd2.11:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -DSIGRETURNTYPE=int -DHAVE_ANSIHEADERS"


#
#
dynix dynix3:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -DSIGRETURNTYPE=int"

#
#
aix hpux hpux8 hpux9:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -DHAVE_ANSIHEADERS"

#
#
linux:
	@$(MAKE) all CC="gcc" CFLAGS="-O -DHAVE_ANSIHEADERS"

#
#
nextstep3.0:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -Wall -s -object -DHAVE_ANSIHEADERS -DNeXT3"

#
#
NEXTARCHS=-arch m68k -arch i386

nextstep3.1:
	@$(MAKE) all CC="$(CC)" CFLAGS="-O -Wall -s -object -DHAVE_ANSIHEADERS -DNeXT3 $(NEXTARCHS)"


openvms:	all

other:		all
