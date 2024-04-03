#!/bin/sh

if [ -e Makefile ]; then
	make realclean
fi
autoheader
autoconf
./configure $*
