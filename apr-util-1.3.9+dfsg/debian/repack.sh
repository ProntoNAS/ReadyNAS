#!/bin/bash

# "borrowed" from pkg-perl :)

# Repackage upstream source to exclude non-distributable files
# should be called as "repack.sh --upstream-source <ver> <downloaded file>
# (for example, via uscan)

set -e
set -u

VER="$2"
FILE="$3"
PKG=$(dpkg-parsechangelog | awk -F": " '/^Source: /{print $2}')

REPACK_DIR="$PKG-$VER.orig" # DevRef § 6.7.8.2

echo -e "\nRepackaging $FILE\n"

DIR=`mktemp -d ./tmpRepackXXXXXX`
trap "rm -rf \"$DIR\"" QUIT INT EXIT

# Create an extra directory to cope with rootless tarballs
UP_BASE="$DIR/unpack"
mkdir "$UP_BASE"
tar xzf "$FILE" -C "$UP_BASE"

if [ `ls -1 "$UP_BASE" | wc -l` -eq 1 ]; then
	# Tarball does contain a root directory
	UP_BASE="$UP_BASE/`ls -1 "$UP_BASE"`"
fi

## Remove stuff
rm -fvr $UP_BASE/{crypto,include}/{apr_md4.,apr_md5.}{c,h}
## End

mv "$UP_BASE" "$DIR/$REPACK_DIR"

# Using a pipe hides tar errors!
tar cfC "$DIR/repacked.tar" "$DIR" "$REPACK_DIR"
gzip -9 < "$DIR/repacked.tar" > "$DIR/repacked.tar.gz"

mv "$DIR/repacked.tar.gz" "$FILE"

echo "*** $FILE repackaged"
NEWFILE="$(echo "$FILE" | sed "s/$VER/&+dfsg/")"
mv "$FILE" "$NEWFILE"
echo
echo "Renamed $FILE to $NEWFILE"
