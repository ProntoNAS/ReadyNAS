## Makefile for the doc subdirectory of the GNU NLS Utilities
##
## Copyright (C) 1995, 1996, 2005, 2007 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

## Process this file with automake to produce Makefile.in.

info_TEXINFOS = sharutils.texi

dist_man1_MANS = shar.1 unshar.1
dist_man5_MANS =

if UUCODE
  dist_man1_MANS += uuencode.1
  dist_man5_MANS += uuencode.5
  man1_MANS = uudecode.1
endif

CLEANFILES = $(man1_MANS) $(man5_MANS)

EXTRA_DIST = doclicense.texi

uudecode.1: Makefile
	man1base=`echo $(man1dir) | sed 's@.*/\(.*\)@\1@'`; \
	echo ".so $$man1base/uuencode.1" > $@

sharutils.txt :
	makeinfo --no-split --fill-column=70 --paragraph-indent=0 --no-headers \
		--output=$@ sharutils.texi

gnudocs: sharutils.txt sharutils.info
	@chmod +x ${top_srcdir}/doc/mkgnudoc.sh ; \
	MAKE=$(MAKE) ; export MAKE ; \
	${top_srcdir}/doc/mkgnudoc.sh $(PACKAGE_STRING)
