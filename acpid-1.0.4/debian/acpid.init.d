#! /bin/sh -e

# Check for daemon presence
test -x /usr/sbin/acpid || exit 0

# Check for ACPI support on kernel side
[ -d /proc/acpi ] || exit 0

# Include acpid defaults if available
OPTIONS=""
if [ -f /etc/default/acpid ] ; then
	. /etc/default/acpid
fi

# As the name says. If the kernel supports modules, it'll try to load
# the ones listed in "MODULES".
load_modules() {
        LIST=`/sbin/lsmod|awk '!/Module/ {print $1}'`

	# Get list of available modules
        LOC="/lib/modules/`uname -r`/kernel/drivers/acpi"
        if [ -d $LOC ]; then
	  MODAVAIL=`( find $LOC -type f -name "*.o" -printf "basename %f .o\n"; \
		find $LOC -type f -name "*.ko" -printf "basename %f .ko\n" ) | /bin/sh`
	else
	  MODAVAIL=""
	fi

        if [ "$MODULES" = "all" ]; then
		MODULES="$MODAVAIL"
        fi

	if [ -n "$MODULES" ]; then
		echo "Loading ACPI modules:"
	        for mod in $MODULES; do
			echo $MODAVAIL | grep -q -w "$mod" || continue
			echo "    $mod"
	                echo $LIST | grep -q -w "$mod" || modprobe $mod || /bin/true
	        done
	fi
}

case "$1" in
  start)
    [ -f /proc/modules ] && load_modules
    echo -n "Starting Advanced Configuration and Power Interface daemon: "
    start-stop-daemon --start --quiet --exec /usr/sbin/acpid -- -c /etc/acpi/events $OPTIONS
    echo "acpid."
    ;;
  stop)
    echo -n "Stopping Advanced Configuration and Power Interface daemon: "
    start-stop-daemon --stop --quiet --oknodo --retry 2 --exec /usr/sbin/acpid
    echo "acpid."
    ;;
  restart)
    $0 stop
    sleep 1
    $0 start
    ;;
  reload|force-reload) 
    echo "Reloading Advanced Configuration and Power Interface daemon configuration files"
    start-stop-daemon --stop --signal 1 --exec /usr/sbin/acpid
    ;;
  *)
    echo "Usage: /etc/init.d/acpid {start|stop|restart|reload|force-reload}"
    exit 1
esac

exit 0
