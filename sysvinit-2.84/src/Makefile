#
# Makefile	Makefile for the systemV init suite.
#		Targets:   all      compiles everything
#		           install  installs the binaries (not the scripts)
#                          clean    cleans up object files
#			   clobber  really cleans up
#
# Version:	@(#)Makefile  2.83-3  06-Nov-2001  miquels@cistron.nl
#

CC	= cc
CFLAGS	= -Wall -O2 -D_GNU_SOURCE
LDFLAGS	= -s
STATIC	=

# For Debian we do not build all programs, otherwise we do.
ifeq ($(DEBIAN),)
PROGS	= init halt shutdown killall5 runlevel sulogin utmpdump \
		last mesg wall
else
PROGS	= init halt shutdown killall5 runlevel sulogin last mesg
endif

BIN_OWNER	= root
BIN_GROUP	= root
BIN_COMBO	= $(BIN_OWNER).$(BIN_GROUP)
INSTALL		= install -o $(BIN_OWNER) -g $(BIN_GROUP)
MANDIR		= /usr/share/man

# Additional libs for Gnu Libc
ifneq ($(wildcard /usr/lib/libcrypt.a),)
LCRYPT		= -lcrypt
endif

all:		$(PROGS)

init:		init.o init_utmp.o
		$(CC) $(LDFLAGS) $(STATIC) -o $@ init.o init_utmp.o

halt:		halt.o ifdown.o hddown.o utmp.o reboot.h
		$(CC) $(LDFLAGS) -o $@ halt.o ifdown.o hddown.o utmp.o

last:		last.o oldutmp.h
		$(CC) $(LDFLAGS) -o $@ last.o

mesg:		mesg.o
		$(CC) $(LDFLAGS) -o $@ mesg.o

utmpdump:	utmpdump.o
		$(CC) $(LDFLAGS) -o $@ utmpdump.o

runlevel:	runlevel.o
		$(CC) $(LDFLAGS) -o $@ runlevel.o

sulogin:	sulogin.o
		$(CC) $(LDFLAGS) $(STATIC) -o $@ sulogin.o $(LCRYPT)

wall:		dowall.o wall.o
		$(CC) $(LDFLAGS) -o $@ dowall.o wall.o

shutdown:	dowall.o shutdown.o utmp.o reboot.h
		$(CC) $(LDFLAGS) -o $@ dowall.o shutdown.o utmp.o

bootlogd:	bootlogd.o
		$(CC) $(LDFLAGS) -o $@ bootlogd.o

init.o:		init.c init.h set.h reboot.h
		$(CC) -c $(CFLAGS) init.c

utmp.o:		utmp.c init.h
		$(CC) -c $(CFLAGS) utmp.c

init_utmp.o:	utmp.c init.h
		$(CC) -c $(CFLAGS) -DINIT_MAIN utmp.c -o init_utmp.o

cleanobjs:
		rm -f *.o *.bak

clean:		cleanobjs
		@echo Type \"make clobber\" to really clean up.

clobber:	cleanobjs
		rm -f $(PROGS)

distclean:	clobber

install:
		$(INSTALL) -m 755 halt init killall5 sulogin \
			runlevel shutdown $(ROOT)/sbin
		# These are not installed by default
ifeq ($(DEBIAN),)
		$(INSTALL) -m 555 utmpdump wall $(ROOT)/usr/bin
endif
		# $(INSTALL) -m 755 etc/initscript.sample $(ROOT)/etc
		$(INSTALL) -m 755 mesg last $(ROOT)/usr/bin
		cd $(ROOT)/sbin; ln -sf halt reboot; chown $(BIN_COMBO) reboot
		cd $(ROOT)/sbin; ln -sf halt poweroff; chown $(BIN_COMBO) poweroff
		cd $(ROOT)/sbin; ln -sf init telinit; chown $(BIN_COMBO) telinit
		cd $(ROOT)/bin; ln -sf ../sbin/killall5 pidof; chown $(BIN_COMBO) pidof
		cd $(ROOT)/usr/bin; ln -sf last lastb; chown $(BIN_COMBO) lastb
		$(INSTALL) -m 644 initreq.h $(ROOT)/usr/include
		$(INSTALL) -m 644 ../man/*.8 $(ROOT)$(MANDIR)/man8
		$(INSTALL) -m 644 ../man/*.5 $(ROOT)$(MANDIR)/man5
ifeq ($(DEBIAN),)
		$(INSTALL) -m 644 ../man/wall.1 $(ROOT)$(MANDIR)/man1
endif
		$(INSTALL) -m 644 ../man/last.1 ../man/lastb.1 ../man/mesg.1 \
			$(ROOT)$(MANDIR)/man1
		#
		# This part is skipped on debian systems, the
		# debian.preinst script takes care of it.
		@if [ ! -p /dev/initctl ]; then \
		 echo "Creating /dev/initctl"; \
		 rm -f /dev/initctl; \
		 mknod -m 600 /dev/initctl p; fi
