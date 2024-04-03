#! /bin/sh

### BEGIN INIT INFO
# Provides:		makedev
# Required-Start:	$local_fs
# Required-Stop:	$local_fs
# Default-Start:	2 3 4 5
# Default-Stop:		0 1 6
# Short-Description:	Creates device files in /dev
### END INIT INFO

N=/etc/init.d/makedev
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

set -e

case "$1" in
  start)
	test -L /dev/MAKEDEV || ln -fs /sbin/MAKEDEV /dev/MAKEDEV

	# create dvb-devices, if the dvb-directory allready exists, and 
	# no devfs or udev is in use (workaround for the changed major-number
	# of the dvb-devices, introduced in kernel 2.6.8, to be able to 
	# switch between kernels <= 2.6.7 and >= 2.6.8, without manual 
	# recreation of the devices (MAKEDEV dvb checks the kernel-version 
	# and uses the correct major-number))
	
	if [ ! -e /dev/.devfsd -a ! -e /dev/.udevdb -a ! -e /dev/.udev ]; then
	  	if [ -d /dev/dvb ]; then
			cd /dev && ./MAKEDEV dvb
	  	fi
	fi
	
	;;
  stop|reload|restart|force-reload)
	;;
  *)
	echo "Usage: $N {start|stop|restart|force-reload}" >&2
	exit 1
	;;
esac

exit 0
