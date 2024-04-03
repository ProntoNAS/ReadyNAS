#!/bin/sh

# Flush exim queue
if [ -x /usr/sbin/exim ]; then
	/usr/sbin/exim -qf
fi
