#!/bin/sh
#
#  Script to create a 'pristine' tarball for the debian ffmpeg source package
#  Copyright (C) 2008, 2009, 2010 Reinhard Tartler
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
  -d : date of svn snapshot
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
PARAMS=`getopt hd: "$@"`
if test $? -ne 0; then usage; exit 1; fi;
set -e

eval set -- "$PARAMS"

DEBUG=false
SVNDATE=

while test $# -gt 0
do
	case $1 in
		-h) usage; exit 1 ;;
		-d) SVNDATE=$2; shift ;;
		--) shift ; break ;;
		*)  echo "Internal error!" ; exit 1 ;;
	esac
	shift
done

# sanity checks now
dh_testdir

if [ -z $SVNDATE ]; then
	error "you need to specify an svn date. e.g. 20081230 for Dec 29. 2008"
fi

TARBALL=../ffmpeg_0.6~svn${SVNDATE}.orig.tar.gz
PACKAGENAME=ffmpeg

TMPDIR=`mktemp -d`
trap 'rm -rf ${TMPDIR}'  EXIT

baseurl="svn://svn.ffmpeg.org/ffmpeg/branches/0.6"

echo "fetching source from ${baseurl}"

svn export -r{${SVNDATE}} \
	--ignore-externals \
	${baseurl}  \
	${TMPDIR}/${PACKAGENAME}

svn info -r{${SVNDATE}} \
	${baseurl} \
	| awk '/^Revision/ {print $2}' \
	> ${TMPDIR}/${PACKAGENAME}/.svnrevision

tar czf ${TARBALL} -C ${TMPDIR} ${PACKAGENAME}

echo "Created tarball for version ${SVNDATE} in ${TARBALL}"
