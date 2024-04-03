dnl Process this file with autoconf to produce a configure script.

dnl Copyright (C) 2002, 2004, 2005, 2006, 2007, 2008, 2009 Simon Josefsson.
dnl
dnl This file is part of Libntlm.
dnl
dnl Libntlm is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU Lesser General Public License as
dnl published by the Free Software Foundation; either version 2.1 of
dnl the License, or (at your option) any later version.
dnl
dnl Libntlm is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with Libntlm; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
dnl 02110-1301, USA

AC_PREREQ(2.61)
AC_INIT([libntlm], [1.2], [libntlm@josefsson.org])
AC_COPYRIGHT([Copyright (c) 2002-2008 Simon Josefsson <simon@josefsson.org>.])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# Interfaces removed:    CURRENT++, AGE=0, REVISION=0
# Interfaces added:      CURRENT++, AGE++, REVISION=0
# No interfaces changed:                   REVISION++
AC_SUBST(LT_CURRENT, 0)
AC_SUBST(LT_AGE, 0)
AC_SUBST(LT_REVISION, 17)

AM_INIT_AUTOMAKE([1.10 gnits -Wall -Werror])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR(ntlm.h.in)

# Checks for programs.
AC_PROG_CC
gl_EARLY
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_C_BIGENDIAN
sj_VALGRIND

# For gnulib.
gl_INIT
AC_DEFINE([check_version], ntlm_check_version, [Rename to public API name.])

# Add many warnings, except some...
if test "$GCC" = "yes"; then
  nw="$nw -Wsystem-headers"         # Don't let system headers trigger warnings
  nw="$nw -Wconversion"             # Too many warnings for now
  nw="$nw -Wsign-conversion"        # Too many warnings for now
  nw="$nw -Wtraditional"            # Warns on #elif which we use often
  nw="$nw -Wtraditional-conversion" # Too many warnings for now
  nw="$nw -Wunreachable-code"       # Too many false positives
  gl_MANYWARN_ALL_GCC([ws])
  gl_MANYWARN_COMPLEMENT(ws, [$ws], [$nw])
  for w in $ws; do
    gl_WARN_ADD([$w])
  done
  gl_WARN_ADD([-Wno-pointer-sign])     # Too many warnings for now
  gl_WARN_ADD([-fdiagnostics-show-option])
  export WARN_CFLAGS
fi

AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  tests/Makefile
  libntlm.pc
  libntlm.spec
  ntlm.h
])
AC_OUTPUT
