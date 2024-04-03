dnl Check for LIBNETSNMP compiler flags. On success, set
dnl nut_have_libnetsnmp="yes" and set LIBNETSNMP_CFLAGS and
dnl LIBNETSNMP_LDFLAGS. On failure, set nut_have_libnetsnmp="no".
dnl This macro can be run multiple times, but will do the checking only
dnl once.

AC_DEFUN([NUT_CHECK_LIBNETSNMP],
[
if test -z "${nut_have_libnetsnmp_seen}"; then
	nut_have_libnetsnmp_seen=yes

	dnl save CFLAGS and LDFLAGS
	CFLAGS_ORIG="${CFLAGS}"
	LDFLAGS_ORIG="${LDFLAGS}"

	dnl See which version of the Net-SNMP library (if any) is installed
	AC_MSG_CHECKING(for Net-SNMP version via net-snmp-config)
	SNMP_VERSION=`net-snmp-config --version 2>/dev/null`
	if test "$?" = "0"; then
		AC_MSG_RESULT(${SNMP_VERSION} found)
	else
		AC_MSG_RESULT(not found)
	fi

	AC_MSG_CHECKING(for Net-SNMP cflags)
	AC_ARG_WITH(snmp-includes, [
		AC_HELP_STRING([--with-snmp-includes=CFLAGS], [include flags for the Net-SNMP library])
	], [CFLAGS="${withval}"], [CFLAGS="`net-snmp-config --cflags 2>/dev/null`"])
	AC_MSG_RESULT([${CFLAGS}])

	AC_MSG_CHECKING(for Net-SNMP libs)
	AC_ARG_WITH(snmp-libs, [
		AC_HELP_STRING([--with-snmp-libs=LDFLAGS], [linker flags for the Net-SNMP library])
	], [LDFLAGS="${withval}"], [LDFLAGS="`net-snmp-config --libs 2>/dev/null`"])
	AC_MSG_RESULT([${LDFLAGS}])

	dnl Check if the Net-SNMP library is usable
	AC_CHECK_HEADERS(net-snmp/net-snmp-config.h, [nut_have_libnetsnmp=yes], [nut_have_libnetsnmp=no], [AC_INCLUDES_DEFAULT])
	AC_CHECK_FUNCS(init_snmp, [], [nut_have_libnetsnmp=no])

	if test "${nut_have_libnetsnmp}" = "yes"; then
		LIBNETSNMP_CFLAGS="${CFLAGS}"
		LIBNETSNMP_LDFLAGS="${LDFLAGS}"
	fi

	dnl restore original CFLAGS and LDFLAGS
	CFLAGS="${CFLAGS_ORIG}"
	LDFLAGS="${LDFLAGS_ORIG}"
fi
])
