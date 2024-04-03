# $Id: Makefile.am 75 2004-12-18 23:42:51Z lennart $
#
# This file is part of libdaemon.
#
# libdaemon is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# libdaemon is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libdaemon; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.

EXTRA_DIST=bootstrap.sh README LICENSE libdaemon.spec
SUBDIRS=libdaemon doc examples

MAINTAINERCLEANFILES = README libdaemon.spec
noinst_DATA = README libdaemon.spec

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdaemon.pc

README:
	rm -f README
	$(MAKE) -C doc README
	ln -s doc/README README

doxygen:
	$(MAKE) -C doc doxygen

homepage: dist doxygen
	test -d $$HOME/homepage/private
	mkdir -p $$HOME/homepage/private/projects/libdaemon
	cp libdaemon-@PACKAGE_VERSION@.tar.gz $$HOME/homepage/private/projects/libdaemon
	cp doc/README.html doc/style.css $$HOME/homepage/private/projects/libdaemon
	cp $$HOME/homepage/private/projects/libdaemon/README.html $$HOME/homepage/private/projects/libdaemon/index.html
	cp -av doc/reference/ $$HOME/homepage/private/projects/libdaemon/

distcleancheck:
	@:

.PHONY: homepage distcleancheck
