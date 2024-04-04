# -*- autoconf -*-
# Process this file with autoconf to produce a configure script.
# Written by Jens Peter Secher <jps@debian.org>.

AC_INIT([Crypto++], [5.6.1], [libcrypto++@bugs.debian.org])
AC_CONFIG_SRCDIR([cryptlib.h])
AC_CONFIG_HEADERS([cryptopp_config.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_CHECK_TYPES([ptrdiff_t])
AC_C_BIGENDIAN(
	AC_DEFINE(IS_BIG_ENDIAN, [], [Big-endian architecture]),
	AC_DEFINE(IS_LITTLE_ENDIAN, [], [Little-endian architecture]),
	AC_MSG_ERROR([Can't tell endianess of platform]))

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostbyname gettimeofday memmove memset pow select socket])

AC_CONFIG_FILES([Makefile libcrypto++.pc])
AC_CONFIG_MACRO_DIR([m4])
AC_OUTPUT
