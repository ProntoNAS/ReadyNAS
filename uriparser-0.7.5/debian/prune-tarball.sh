#! /bin/bash

set -e

if [ ! -r "$1" ]; then
    echo >&2 "E: need an existing tarball as the first argument."
    exit 1
fi

TMP_DIR=`mktemp -d`
TARBALL_NAME="`basename $1`"
trap "rm -rf $TMP_DIR" 0

tar xCf "$TMP_DIR" "$1"
rm "$TMP_DIR"/*/doc/rfc{3986,3513,1866}.htm
rm "$TMP_DIR"/*/doc/rfc3986_grammar_only.txt

(
cd "$TMP_DIR"
tar czf "$TARBALL_NAME" *
)

mv "$1" "$1.bak"
mv "$TMP_DIR/$TARBALL_NAME" `dirname "$1"`
