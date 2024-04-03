#! /bin/sh

set -e

. /etc/default/devpts

[ "`uname -s`" = "Linux" ] || exit 0

#
#	First find out if devpts is available.
#
#	As of 2.5.68, devpts is not automounted when using devfs. So we
#	mount devpts if it is compiled in (older devfs didn't require it
#	to be compiled in at all).
#
devpts_avail=`grep -ci '[<[:space:]]devpts' /proc/filesystems || true`
devpts_mounted=`grep -ci '/dev/pts' /proc/mounts || true`

if [ "$devpts_avail" != 0 ]
then
	#
	#	Create mountpoint and multiplexor device.
	#
	[ -d /dev/pts ] || mkdir --mode=755 /dev/pts
	[ -c /dev/ptmx ] || mknod --mode=666 /dev/ptmx c 5 2

	#
	#	Mount /dev/pts if needed.
	#
	if [ "$devpts_mounted" = 0 ]
	then
		mount -t devpts devpts /dev/pts -ogid=${TTYGRP},mode=${TTYMODE}
	fi
fi
