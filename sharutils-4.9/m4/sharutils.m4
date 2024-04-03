# sharutils.m4 serial 2 (sharutils-4.3.75)
dnl Copyright (C) 2002 Free Software Foundation, Inc.
dnl This file is free software, distributed under the terms of the GNU
dnl General Public License.

dnl Authors:
dnl   Karl Eichwalder <ke@suse.de>, 2002.

AC_DEFUN([CHECK_COMPRESS],
[
  AC_PATH_PROGS(COMPRESS, [compress], [no])
  if test $COMPRESS != no; then
    cp $srcdir/COPYING tCOPYING
    AC_MSG_CHECKING([whether compress works])
    if compress tCOPYING >/dev/null 2>&1; then
      AC_DEFINE([HAVE_COMPRESS], [1],
                [Define if compress is available.])
      AC_MSG_RESULT([yes])
    else
      COMPRESS=no
      AC_MSG_RESULT([no, installing compress-dummy])
    fi
    rm -f tCOPYING tCOPYING.Z
  fi
  if test $COMPRESS = no; then
    ADD_SCRIPT="$ADD_SCRIPT compress-dummy"
    AC_CONFIG_FILES([src/compress-dummy])
  fi
])

AC_DEFUN([CHECK_COMPRESS_AND_LINK],
[
  CHECK_COMPRESS
  if test $COMPRESS = no; then
    AC_ARG_ENABLE([compress-link],
      [AS_HELP_STRING([--enable-compress-link],
	[install compress link if the program is missing])])
    if test "x$enable_compress_link" = xyes; then
      INSTALL_COMPRESS_LINK="compress-link"
    else
      INSTALL_COMPRESS_LINK=
    fi
  fi
  AC_SUBST(INSTALL_COMPRESS_LINK)
])
