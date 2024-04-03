#!/bin/sh
#
#  Script to create a 'pristine' tarball for the debian ffmpeg source package
#  Copyright (C) 2008, Reinhard Tartler
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

set -eu

usage() {
	cat >&2 <<EOF
usage: $0 [-dh]
  -h : display help
  -r : svn revision
  -o : output tarball name
  -c : path to cleanup script
EOF
}

debug () {
	$DEBUG && echo "DEBUG: $*" >&2
}

error () {
	echo "$1" >&2
	exit 1;
}

set +e
PARAMS=`getopt hr:c:o: "$@"`
if test $? -ne 0; then usage; exit 1; fi;
set -e

eval set -- "$PARAMS"

DEBUG=false
REVISION=
CLEANUPSCRIPT=
TARBALL=

while test $# -gt 0
do
	case $1 in
		-h) usage; exit 1 ;;
		-r) REVISION=$2; shift ;;
		-c) CLEANUPSCRIPT=$2; shift ;;
		-o) TARBALL=$2; shift ;;
		--) shift ; break ;;
		*)  echo "Internal error!" ; exit 1 ;;
	esac
	shift
done

if [ -z $REVISION ]; then
	error "you need to specify an svn revision"
fi

if [ -z $TARBALL ]; then
	error "you need to specify a tarballname"
fi

if [ -n $CLEANUPSCRIPT ] && [ -f $CLEANUPSCRIPT ]; then
	if [ ! -x $CLEANUPSCRIPT ]; then
		error "$CLEANUPSCRIPT must be executable"
	fi
fi

TMPDIR=`mktemp -d`
trap 'rm -rf ${TMPDIR}'  EXIT

svn export -r${REVISION} \
	svn://svn.mplayerhq.hu/ffmpeg/trunk \
	${TMPDIR}/ffmpeg

# libswscale is just an unversioned redirect, so we have to export it properly by hand
rm -rf ${TMPDIR}/ffmpeg/libswscale

svn export -r${REVISION} \
	svn://svn.mplayerhq.hu/mplayer/trunk/libswscale \
	${TMPDIR}/ffmpeg/libswscale
	
if [ -n ${CLEANUPSCRIPT} ]; then
	( cd ${TMPDIR}/ffmpeg && ${CLEANUPSCRIPT} )
fi

tar czf ${TARBALL} -C ${TMPDIR} ffmpeg

