## Process this file with automake to produce Makefile.in
# Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2013 Simon Josefsson.
#
# This file is part of Libntlm.
#
# Libntlm is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of
# the License, or (at your option) any later version.
#
# Libntlm is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Libntlm; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,
# USA

SUBDIRS = gl

ACLOCAL_AMFLAGS = -I m4 -I gl/m4

EXTRA_DIST = cfg.mk maint.mk
EXTRA_DIST += libntlm.pc.in test.txt libntlm.spec

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libntlm.pc

lib_LTLIBRARIES = libntlm.la

include_HEADERS = ntlm.h

AM_CFLAGS = $(WARN_CFLAGS)
AM_CPPFLAGS = -I$(srcdir)/gl -I$(builddir)/gl -DNTLM_SRCDIR=\"$(srcdir)\"

libntlm_la_SOURCES = smbutil.c smbencrypt.c
libntlm_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-symbols-regex '^(dumpSmb|buildSmb|ntlm_).*' -no-undefined

libntlm_la_LIBADD = gl/libgnu.la

# test

TESTS = test_ntlm
check_PROGRAMS = $(TESTS)
LDADD = libntlm.la gl/libgnu.la
CLEANFILES = test.out
test_ntlm_LDFLAGS = -no-install

TESTS_ENVIRONMENT = $(VALGRIND)
