#!/bin/sh

exec 2> /dev/null

DATADIR="/usr/share/GeoIP"
GUNZIP="/bin/gunzip"
MAXMINDURL="http://geolite.maxmind.com/download/geoip/database/asnum"
WGET="/usr/bin/wget -q -O -"
TMPDIR=$(mktemp -d)

if [ ! -d "$DATADIR" ] ; then
	echo "Data directory $DATADIR/ doesn't exist!"
	exit 1
fi

if [ ! -w "$DATADIR" ] ; then
	echo "Can't write to $DATADIR directory!"
	exit 1
fi

cd "${TMPDIR}"

${WGET} "${MAXMINDURL}/GeoIPASNum.dat.gz" | ${GUNZIP} > GeoIPASNum.dat

if [ $? != 0 ] ; then
	echo "Can't download a free GeoLite ASNum database!"
	exit 1
fi

mv -f "GeoIPASNum.dat" "${DATADIR}/"

if [ $? != 0 ] ; then
	echo "Can't move databases file to ${DATADIR}/"
	exit 1
fi

exit 0
