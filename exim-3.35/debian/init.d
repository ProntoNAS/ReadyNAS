#! /bin/sh
# /etc/init.d/exim
#
# Written by Miquel van Smoorenburg <miquels@drinkel.ow.org>.
# Modified for Debian GNU/Linux by Ian Murdock <imurdock@gnu.ai.mit.edu>.
# Modified for exim by Tim Cutts <timc@chiark.greenend.org.uk>

set -e

# Exit if exim runs from /etc/inetd.conf
if [ -f /etc/inetd.conf ] && grep -q "^ *smtp" /etc/inetd.conf; then
    exit 0
fi

DAEMON=/usr/sbin/exim
NAME=exim

test -x $DAEMON || exit 0

case "$1" in
  start)
    echo -n "Starting MTA: "
    start-stop-daemon --start --pidfile /var/run/exim/exim.pid \
			    --exec $DAEMON -- -bd -q30m
    echo "exim."
    ;;
  stop)
    echo -n "Stopping MTA: "
    start-stop-daemon --stop --pidfile /var/run/exim/exim.pid \
			    --oknodo --retry 30 --exec $DAEMON
    echo "exim."
      ;;
  restart)
    echo -n "Restarting MTA: "
    start-stop-daemon --stop --pidfile /var/run/exim/exim.pid \
			    --oknodo --retry 30 --exec $DAEMON
    start-stop-daemon --start --pidfile /var/run/exim/exim.pid \
			    --exec $DAEMON -- -bd -q30m
    echo "exim."
    ;;
  reload|force-reload)
    echo "Reloading $NAME configuration files"
    start-stop-daemon --stop --pidfile /var/run/exim/exim.pid \
			    --signal 1 --exec $DAEMON
    ;;
  *)
    echo "Usage: /etc/init.d/$NAME {start|stop|restart|reload}"
    exit 1
    ;;
esac

exit 0
