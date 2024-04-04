#!/bin/sh

rm -f config.cache 
aclocal 
autoheader
autoconf
automake --copy --add-missing
