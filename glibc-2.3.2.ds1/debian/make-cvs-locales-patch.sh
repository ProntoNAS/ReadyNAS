#!/bin/bash

set -e

# This script is designed to help make patches to update from the last
# release to the latest CVS.  Hand it the argument of the directory from
# which to generate the diff.

# This script is not robust.  Feel free to improve it.  Specifically,
# run this from the root of the package.

# This file is in the PUBLIC DOMAIN
# written by Jeff Bailey jbailey@debian.org September 6th, 2002

if [ $# -ne 1 ]; then
  echo "`basename $0`: script expects a CVS tree to diff against"
  exit 1
fi

PATCHLIST=10_cvs debian/rules patch

SRCDIR=build-tree/glibc-*

# DESCRIPTION, DETAILS, DATE, PATCHLEVEL

cat debian/patches/template.dpatch | sed -e "s/@DESCRIPTION@/Locales CVS Update/" -e "s/@DATE@/`date`/" -e "s/@PATCHLEVEL@/2/" > debian/patches/11_cvs_locales.dpatch

diff -urN -x CVS $SRCDIR/localedata $1/localedata >>debian/patches/11_cvs_locales.dpatch

