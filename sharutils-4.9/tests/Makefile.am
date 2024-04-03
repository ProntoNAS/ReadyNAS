## Makefile for the checks subdirectory of the GNU Shar utilities.
##
## Copyright (C) 1995, 1996, 2005, 2006, 2007 Free Software Foundation, Inc.
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

## Process this file with automake to produce Makefile.in.

TESTS = uutest-1 uude-1 shar-1 shar-2 shar-3
TESTS_ENVIRONMENT = \
  top_srcdir=$(top_srcdir) \
  PATH=`cd ../src && pwd`:"$$PATH" \
  PACKAGE_STRING='$(PACKAGE_STRING)' top_builddir=$(top_builddir)

EXTRA_DIST = $(TESTS) testdata shar-1.ok

clean : local-clean

local-clean :
	rm -rf shar-1-* shar-1.in shar-2*.* $${TMPDIR=/tmp}/shar-3*.d
