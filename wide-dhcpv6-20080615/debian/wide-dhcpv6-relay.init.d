#!/bin/sh
### BEGIN INIT INFO
# Provides:          wide-dhcpv6-relay
# Required-Start:    $syslog $network $remote_fs
# Required-Stop:     $syslog $remote_fs
# Should-Start:      $local_fs
# Should-Stop:       $local_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start/Stop WIDE DHCPv6 relay agent
# Description:       (empty)
### END INIT INFO

PATH=/sbin:/bin:/usr/sbin:/usr/bin
DHCP6RBIN=/usr/sbin/dhcp6relay
DHCP6RPID=/var/run/dhcp6relay.pid
NAME="dhcp6relay"
DESC="WIDE DHCPv6 relay"

. /lib/lsb/init-functions

test -x $DHCP6RBIN || exit 0

if [ ! -f /etc/default/wide-dhcpv6-relay ]; then
	log_failure_msg \
            "/etc/default/wide-dhcpv6-relay does not exist! - Aborting..."
	log_failure_msg \
            "Run 'dpkg-reconfigure wide-dhcpv6-relay' to solve the problem."
	exit 1
else
	. /etc/default/wide-dhcpv6-relay
fi


[ "X$INTERFACES" != "X" ] || exit 0

# single arg is -v for messages, -q for none
check_status()
{
    if [ ! -r "$DHCP6RPID" ]; then
        test "$1" != -v || echo "$NAME is not running."
        return 3
    fi
    if read pid < "$DHCP6RPID" && ps -p "$pid" > /dev/null 2>&1; then
        test "$1" != -v || echo "$NAME is running."
        return 0
    else
        test "$1" != -v || echo "$NAME is not running but $DHCP6RPID exists."
        return 1
    fi
}

case "$1" in
	start)
		log_daemon_msg "Starting $DESC" "$NAME"
		start-stop-daemon --start --quiet --pidfile $DHCP6RPID \
			--oknodo --exec $DHCP6RBIN -- $INTERFACES
		sleep 2
		if check_status -q; then
			log_end_msg 0
		else
			log_end_msg 1
			exit 1
		fi
		;;
	stop)
		log_daemon_msg "Stopping $DESC" "$NAME"
		start-stop-daemon --stop --quiet --pidfile $DHCP6RPID --oknodo
                log_end_msg $?
		rm -f $DHCP6RPID
		;;
	restart|force-reload)
		$0 stop
		sleep 2
		$0 start
		if [ "$?" != "0" ]; then
			exit 1
		fi
		;;
	status)
		echo "Status of $NAME: "
		check_status -v
		exit "$?"
		;;
	*)
		echo "Usage: $0 (start|stop|restart|force-reload|status)"
		exit 1
esac

exit 0
