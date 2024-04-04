dnl
dnl "$Id$"
dnl
dnl   systemd stuff for CUPS.

dnl Find whether systemd is available

SDLIBS=""
AC_ARG_WITH([systemdsystemunitdir],
        AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files]),
        [], [with_systemdsystemunitdir=$($PKGCONFIG --variable=systemdsystemunitdir systemd)])
if test "x$with_systemdsystemunitdir" != xno; then
        AC_MSG_CHECKING(for libsystemd-daemon)
        AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
        if $PKGCONFIG --exists libsystemd-daemon; then
		AC_MSG_RESULT(yes)
        	SDCFLAGS=`$PKGCONFIG --cflags libsystemd-daemon`
        	SDLIBS=`$PKGCONFIG --libs libsystemd-daemon`
		AC_DEFINE(HAVE_SYSTEMD)
	else
		AC_MSG_RESULT(no)
	fi
fi

if test -n "$with_systemdsystemunitdir" -a "x$with_systemdsystemunitdir" != xno ; then
        SYSTEMD_UNITS="cups.service cups.socket cups.path"
else
        SYSTEMD_UNITS=""
fi

AC_SUBST(SYSTEMD_UNITS)
AC_SUBST(SDLIBS)

dnl
dnl "$Id$"
dnl
