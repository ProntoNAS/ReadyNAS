#!/bin/bash
#Running bash because /bin/sh is a complete piece of shit on solaris
#I wonder how long it will take the fuckwits at Sun to wake up

# Run this to generate all the initial makefiles, etc.

die() {
        echo >&2
        echo "$@" >&2
        exit 1
}

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR=`pwd` || die "Error running pwd"
cd $srcdir || die "Error cding into \"$srcdir\""


DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo >&2
	echo "You must have autoconf installed to compile iproxy." >&2
	echo "Download the appropriate package for your distribution," >&2
	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/" >&2
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo >&2
	echo "You must have automake installed to compile iproxy." >&2
	echo "Get ftp://ftp.cygnus.com/pub/home/tromey/automake-1.2d.tar.gz" >&2
	echo "(or a newer version if it is available)" >&2
	DIE=1
}

if test "$DIE" -eq 1; then
	exit 1
fi

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

case $CC in
xlc )
    am_opt=--include-deps;;
esac

aclocal $ACLOCAL_FLAGS || die "Error running aclocal"

(autoheader --version)  < /dev/null > /dev/null 2>&1 && autoheader || \
        die "Error running autoheader"
automake --add-missing $am_opt || die "Error running automake"
autoconf || die "Error running autoconf"

cd $THEDIR || die "Error cding into \"$THEDIR\""

$srcdir/configure "$@" || die "Error runnig configure"

echo 
echo "Now type 'make' to compile iproxy."
