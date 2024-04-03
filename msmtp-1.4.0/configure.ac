dnl configure.ac
dnl Process this file with autoconf to produce a configure script.
#
# This file is part of msmtp, an SMTP client.
#
# Copyright (C) 2003, 2004, 2005
# Christophe Nowicki
# Martin Lambers <marlam@marlam.de>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#   msmtp is released under the GPL with the additional exemption that
#   compiling, linking, and/or using OpenSSL is allowed.
#

AC_INIT(msmtp, 1.4.0, marlam@marlam.de)
AC_CONFIG_SRCDIR(src/msmtp.c)

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
gl_EARLY
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_CANONICAL_SYSTEM

AC_C_CONST

AC_HEADER_STDC

dnl System
case "${target}" in *-*-mingw32*) windows=yes ;; *) windows=no ;; esac

dnl System services
AC_CHECK_FUNCS(syslog,,)

dnl Types
AC_CHECK_HEADERS(sys/types.h sys/socket.h)
AC_CHECK_TYPE([socklen_t],, [AC_DEFINE([socklen_t], [int],
    [Define to `int' if `socklen_t' is missing.])], [
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
])

dnl gnulib 
gl_SOURCE_BASE(gnulib)
gl_M4_BASE(gnulib/m4)
gl_LIB(libgnu)
gl_MODULES(base64 getopt md5 sysexits xalloc)
gl_INIT

dnl Networking libraries, getaddrinfo()
if test "$windows" = "yes"; then
    LIBS="$LIBS -lws2_32"
    AC_ARG_ENABLE(win32-ipv6, 
        AC_HELP_STRING([--disable-win32-ipv6], 
            [disable IPv6 support on Windows systems]),
        want_win32_ipv6=$enableval)
    if test "$want_win32_ipv6" != "no"; then
        AC_DEFINE(HAVE_GETADDRINFO, [], [Have getaddrinfo() function])
    fi
else
    AC_CHECK_FUNC(socket, [], 
        [AC_CHECK_LIB(socket, socket, [LIBS="$LIBS -lsocket"])])
    AC_CHECK_FUNC(getaddrinfo, 
        [AC_DEFINE(HAVE_GETADDRINFO, [], [Have getaddrinfo() function])
            have_getaddrinfo=yes],
        [AC_CHECK_FUNC(gethostbyname, [], 
            [AC_CHECK_LIB(nsl, gethostbyname, [LIBS="$LIBS -lnsl"])])
         AC_CHECK_FUNC(hstrerror, [], 
            [AC_CHECK_LIB(resolv, hstrerror, [LIBS="$LIBS -lresolv"])])])
fi

dnl pkg-config
AC_CHECK_PROG(have_pkgconfig, pkg-config, yes, no) 

dnl TLS/SSL
want_tls=yes
have_tls=no
want_gnutls=yes
want_openssl=yes
tls_lib=none
AC_ARG_ENABLE(ssl, AC_HELP_STRING([--disable-ssl], 
    [disable TLS/SSL support]), want_tls=$enableval)
if test "$want_tls" = "yes"; then
    AC_ARG_WITH(ssl, [--with-ssl=[gnutls|openssl] 
        TLS/SSL support via GnuTLS (default) or OpenSSL],
        if test "$withval" = "gnutls"; then
	    want_gnutls=yes
            want_openssl=no
	elif test "$withval" = "openssl"; then
            want_gnutls=no
            want_openssl=yes
	else
	    AC_MSG_ERROR(Use --with-ssl=gnutls or --with-ssl=openssl)
	fi)
else
    want_gnutls=no
    want_openssl=no
fi
if test "$want_gnutls" = "yes"; then
    AM_PATH_LIBGNUTLS(1.2.0, [have_gnutls=yes],)
    if test "$have_gnutls" = "yes"; then
        AC_DEFINE(HAVE_GNUTLS,, Build with GnuTLS support)
        have_tls=yes 
        tls_lib="GnuTLS"
        SSL_CFLAGS="$LIBGNUTLS_CFLAGS"
        SSL_LIBS="$LIBGNUTLS_LIBS"
    fi
fi
if test "$want_openssl" = "yes" -a "$have_tls" = "no"; then
    if test "$have_pkgconfig" = "yes"; then
        if pkg-config --exists openssl; then
            PKG_CHECK_MODULES(SSL, openssl, 
	        [have_openssl=yes], [have_openssl=no])
        fi
    fi
    if test "$have_openssl" != "yes"; then
        if test "$windows" = "yes"; then
            AC_CHECK_HEADER(openssl/ssl.h, AC_CHECK_LIB(ssl32, SSL_read,
                [have_openssl=yes SSL_LIBS="-leay32 -lssl32" ],, -leay32))
        else
            AC_CHECK_HEADER(openssl/ssl.h, AC_CHECK_LIB(ssl, SSL_read, 
                [have_openssl=yes SSL_LIBS="-lssl -lcrypto" ],, -lcrypto))
        fi
    fi
    if test "$have_openssl" = "yes"; then
        AC_DEFINE(HAVE_OPENSSL,, Build with OpenSSL support)
        have_tls=yes
        tls_lib="OpenSSL"
    fi
fi
if test "$have_tls" = "yes"; then
    CFLAGS="$CFLAGS $SSL_CFLAGS"
    AC_DEFINE(HAVE_SSL,, Build with TLS/SSL support)
    tls_obj="tls.o"
elif test "$want_tls" = "yes"; then
    AC_MSG_WARN([Neither GnuTLS nor OpenSSL found, disabling TLS/SSL support])
fi
AC_SUBST(tls_obj)

dnl GNU SASL
want_gsasl=yes
have_gsasl=no
AC_ARG_ENABLE(gsasl, AC_HELP_STRING([--disable-gsasl], [disable GNU SASL]), 
    want_gsasl=$enableval)
if test "$want_gsasl" != "no" ; then
    if test "$have_pkgconfig" = "yes"; then
        if pkg-config --exists libgsasl; then
            PKG_CHECK_MODULES(GSASL, libgsasl >= 0.2.4, 
	        [have_gsasl=yes], [have_gsasl=no])
        fi
    else
        AC_CHECK_HEADER(gsasl.h, 
	    AC_CHECK_LIB(gsasl, gsasl_check_version,
                [have_gsasl=yes AC_SUBST(GSASL_LIBS, -lgsasl)], 
		have_gsasl=no), have_gsasl=no)
    fi
fi
if test "$have_gsasl" = "yes"; then
    CFLAGS="$CFLAGS $GSASL_CFLAGS"
    AC_DEFINE(USE_GSASL,, Build with GNU SASL support)
else
    if test "$want_gsasl" = "yes"; then
        AC_MSG_WARN([Cannot find GNU SASL, disabling])
    fi
    crypto_obj="crypto.o"
fi
AC_SUBST(crypto_obj)


AC_CONFIG_FILES(Makefile doc/Makefile gnulib/Makefile src/Makefile)
AC_OUTPUT

echo
echo "Install prefix ..... : $prefix"
echo "TLS/SSL support .... : $have_tls (Library: $tls_lib)"
echo "GNU SASL support ... : $have_gsasl"
