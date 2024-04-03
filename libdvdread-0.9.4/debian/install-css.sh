#!/bin/sh
set -e

site=http://www.dtek.chalmers.se/groups/dvd/deb/
arch=$(dpkg --print-installation-architecture)

soname=2
uversion=1.2.5
available="alpha hppa i386 ia64 powerpc s390 sparc"
version=${uversion}-1

if ! type wget > /dev/null ; then
    echo "Install wget and run this script again"
    exit 1
fi

for a in $available; do
    if [  "$a" = "$arch" ]; then
	wget ${site}libdvdcss${soname}_${version}_${arch}.deb -O /tmp/libdvdcss.deb
	dpkg -i /tmp/libdvdcss.deb
	exit $?
    fi
done

echo "No binary deb available.  Will try to build and install it."
echo "You need to have debhelper, dpkg-dev and fakeroot installed."
echo "If not, interrupt now, install them and rerun this script."
echo ""
echo "This is higly experimental, look out for what happens below."
echo "If you want to stop, interrupt now (control-c), else press"
echo "return to proceed"
read dum

mkdir -p /tmp/dvd
cd /tmp/dvd
wget ${site}libdvdcss_${uversion}.orig.tar.gz
wget ${site}libdvdcss_${version}.diff.gz
wget ${site}libdvdcss_${version}.dsc
dpkg-source -x libdvdcss_${version}.dsc
cd libdvdcss-${uversion}
fakeroot ./debian/rules binary
echo "Any problems?  Interrupt now (control-c) and try to fix"
echo "manually, else go on and install (return)."
dpkg -i ../libdvdcss${soname}_${version}_${arch}.deb

