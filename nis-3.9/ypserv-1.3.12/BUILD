#!/bin/sh
#
# Copyright (c) 1997, 1998, 1999 Thorsten Kukuk, Germany
#
# Author: Thorsten Kukuk <kukuk@suse.de>
#

if [ ! "$arch" -a ! "$os" -a ! "$rel" ]
then
        arch=`uname -m | sed -e 's/ //g'`
        os=`uname -s | sed -e 's/\//-/g' -e 's/ //g'`
        rel=`uname -r | sed -e 's/(/-/g' -e 's/)//g'`
else
	echo "ERROR: Could not determine system !"
fi

echo "Configuration: arch=$arch, os=$os, rel=$rel"

if [ ! -d obj.$arch.$os-$rel ]
then
   mkdir obj.$arch.$os-$rel
fi

cd obj.$arch.$os-$rel

../configure --enable-fqdn --enable-checkroot --enable-ypmake

if make
then
	echo ""
	echo ""
	echo "ypserv is compiled successfully."
	echo ""
	echo "change in the directory obj.$arch.$os-$rel, type make install for"
	echo "installing and edit /var/yp/Makefile, if this is a NIS master"
	echo "host"
else
	echo "Error when compiling ypserv ..."
fi
