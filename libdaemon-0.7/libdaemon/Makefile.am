# $Id: Makefile.am 71 2004-12-18 23:29:50Z lennart $
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

AM_CFLAGS=-D_GNU_SOURCE

pkginclude_HEADERS = \
	dlog.h \
	dfork.h \
	dsignal.h \
	dnonblock.h \
	dpid.h \
	dexec.h \
	daemon.h

lib_LTLIBRARIES = libdaemon.la

libdaemon_la_SOURCES = \
	dlog.c \
	dfork.c \
	dsignal.c \
	dnonblock.c \
	dpid.c \
	dexec.c \
	daemon.h \
	$(pkg_include_HEADERS)

libdaemon_la_LDFLAGS = -version-info 2:1:2
