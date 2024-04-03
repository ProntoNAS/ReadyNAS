#!/bin/sh
## ----------------------------------------------------------------------
## autogen.sh : refresh GNU autotools toolchain for libxml2, and
## refreshes doc/examples/index.html
## For use in root directory of the build tree ONLY.
## ----------------------------------------------------------------------
## Requires: autoconf (2.5x), automake1.8, libtool (1.5.x), xsltproc,
## libxml2-utils
## ----------------------------------------------------------------------

## ----------------------------------------------------------------------
set -e

## ----------------------------------------------------------------------
libtoolize --force --copy

## ----------------------------------------------------------------------
aclocal-1.8

## ----------------------------------------------------------------------
autoheader

## ----------------------------------------------------------------------
automake-1.8 --foreign --add-missing --force-missing --copy

## ----------------------------------------------------------------------
autoconf

# clean up the junk that was created
rm -rf autom4te.cache

# rebuild doc/examples/index.html
cd doc/examples
xsltproc examples.xsl examples.xml
xmllint --valid --noout index.html
cd ../..

## ----------------------------------------------------------------------
exit 0

## ----------------------------------------------------------------------
