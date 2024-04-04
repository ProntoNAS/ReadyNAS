# $Id: Makefile.am 114 2007-02-08 16:42:22Z lennart $
#
# This file is part of nss-llmnr.
#
# nss-llmnr is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# nss-llmnr is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with nss-llmnr; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA.

EXTRA_DIST = map-file

AM_CFLAGS = \
	-DLLMNR_ALLOW_FILE=\"$(sysconfdir)/llmnr.allow\" \
	-DRESOLV_CONF_FILE=\"$(sysconfdir)/resolv.conf\"

#AM_CFLAGS += -DNDEBUG=1 -Os

# This cool debug trap works on i386/gcc only
AM_CFLAGS+='-DDEBUG_TRAP=__asm__("int $$3")'

AM_LDFLAGS=-avoid-version -module -export-dynamic

lib_LTLIBRARIES = 

if FREEBSD_NSS
lib_LTLIBRARIES += \
	nss_llmnr.la \
	nss_llmnr4.la \
	nss_llmnr6.la
else
lib_LTLIBRARIES += \
	libnss_llmnr.la \
	libnss_llmnr4.la \
	libnss_llmnr6.la
endif

noinst_PROGRAMS = nss-test
sources = util.c util.h

sources += query.c query.h dns.c dns.h
noinst_PROGRAMS += llmnr-test

# GNU Libc
libnss_llmnr_la_SOURCES= $(sources) nss.c
libnss_llmnr_la_CFLAGS=$(AM_CFLAGS) 
libnss_llmnr_la_LDFLAGS=$(AM_LDFLAGS) -shrext .so.2 -Wl,-version-script=$(srcdir)/map-file

libnss_llmnr4_la_SOURCES=$(libnss_llmnr_la_SOURCES)
libnss_llmnr4_la_CFLAGS=$(libnss_llmnr_la_CFLAGS) -DNSS_IPV4_ONLY=1
libnss_llmnr4_la_LDFLAGS=$(libnss_llmnr_la_LDFLAGS)

libnss_llmnr6_la_SOURCES=$(libnss_llmnr_la_SOURCES)
libnss_llmnr6_la_CFLAGS=$(libnss_llmnr_la_CFLAGS) -DNSS_IPV6_ONLY=1
libnss_llmnr6_la_LDFLAGS=$(libnss_llmnr_la_LDFLAGS)

# FreeBSD
nss_llmnr_la_SOURCES=$(sources) bsdnss.c
nss_llmnr_la_CFLAGS=$(AM_CFLAGS)
nss_llmnr_la_LDFLAGS=$(AM_LDFLAGS) -shrext .so.1

nss_llmnr4_la_SOURCES=$(nss_llmnr_la_SOURCES)
nss_llmnr4_la_CFLAGS=$(nss_llmnr_la_CFLAGS) -DNSS_IPV4_ONLY=1
nss_llmnr4_la_LDFLAGS=$(nss_llmnr_la_LDFLAGS)

nss_llmnr6_la_SOURCES=$(nss_llmnr_la_SOURCES)
nss_llmnr6_la_CFLAGS=$(nss_llmnr_la_CFLAGS) -DNSS_IPV6_ONLY=1
nss_llmnr6_la_LDFLAGS=$(nss_llmnr_la_LDFLAGS)

llmnr_test_SOURCES = \
	query.c query.h \
	dns.c dns.h \
	util.c util.h \
	llmnr-test.c

nss_test_SOURCES = \
	nss-test.c

install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/libnss_llmnr.la 
	rm -f $(DESTDIR)$(libdir)/libnss_llmnr4.la 
	rm -f $(DESTDIR)$(libdir)/libnss_llmnr6.la 

uninstall-hook:
	rm -f $(DESTDIR)$(libdir)/libnss_llmnr.so.2 
	rm -f $(DESTDIR)$(libdir)/libnss_llmnr4.so.2 
	rm -f $(DESTDIR)$(libdir)/libnss_llmnr6.so.2 
