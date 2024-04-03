#!/bin/sh
#
# Repackage upstream source to exclude non-distributable files.
# Should be called as "repack sh --upstream-source <version> <file>
# (for example, via uscan).

set -e
set -u

if [ $# -ne 3 ]; then
        echo "Usage: $0 --upstream-source <version> <file>"
        exit 1
fi

OPT_VERSION=$2
OPT_FILE=$3
TMPDIR=`mktemp -d`
trap "rm -rf $TMPDIR" QUIT INT EXIT

echo "Repackaging $OPT_FILE"

tar xzf $OPT_FILE -C $TMPDIR

orig_file_path=`readlink --canonicalize $OPT_FILE`
upstream_directory=`ls -1 $TMPDIR | head -1`
package_name=`dpkg-parsechangelog | sed -n 's/^Source: //p'`
dfsg_directory=${package_name}_${OPT_VERSION}.dfsg1
dfsg_file_path=`dirname $orig_file_path`/$dfsg_directory.orig.tar.gz

# Use a subshell to remove the non-dfsg-free files
(
        set -e
        set -u

        cd $TMPDIR/$upstream_directory

        # Individual files to remove
        for file in \
                doc/draft* \
                doc/rfc* \
                java/doc/draft* ; do
                rm -v $file
        done

        # Whole directories to remove
        for directory in \
                dlcompat-20010505 ; do
                rm -rfv $directory
        done
)

# Rename upstream tarball root directory and repackage the file
(
        cd $TMPDIR
        mv $upstream_directory $dfsg_directory
        tar czf $orig_file_path *
        mv $orig_file_path $dfsg_file_path
)

echo "File $OPT_FILE repackaged successfully to $dfsg_file_path"

