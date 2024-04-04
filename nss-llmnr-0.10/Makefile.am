# $Id: Makefile.am 109 2007-01-01 17:22:10Z lennart $
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

EXTRA_DIST=bootstrap.sh README LICENSE
SUBDIRS=src doc

MAINTAINERCLEANFILES=README
noinst_DATA = README

README:
	rm -f README
	$(MAKE) -C doc README
	cd $(srcdir) && ln -s doc/README README

homepage: all dist
	test -d $$HOME/homepage/private
	mkdir -p $$HOME/homepage/private/projects/nss-llmnr
	cp nss-llmnr-@PACKAGE_VERSION@.tar.gz doc/README.html doc/style.css $$HOME/homepage/private/projects/nss-llmnr
	ln -sf README.html $$HOME/homepage/private/projects/nss-llmnr/index.html

.PHONY: homepage
