#! /bin/sh

# on ia64 systems, the acats hangs in unaligned memory accesses.
# kill these testcases.

logfile=$1
stopfile=$2
interval=30

while true; do
    if [ -f "$stopfile" ]; then
	echo "`basename $0`: finished."
	exit 0
    fi
    sleep $interval
    if [ ! -f "$logfile" ]; then
	continue
    fi
    pids=$(ps aux | awk '/testsuite\/ada\/acats\/tests/ { print $2 }')
    if [ -n "$pids" ]; then
	sleep 15
        pids2=$(ps aux | awk '/testsuite\/ada\/acats\/tests/ { print $2 }')
	if [ "$pids" = "$pids2" ]; then
	    #echo kill: $pids
	    kill $pids
	    sleep 1
            pids2=$(ps aux | awk '/testsuite\/ada\/acats\/tests/ { print $2 }')
	    if [ "$pids" = "$pids2" ]; then
	        #echo kill -9: $pids
	        kill -9 $pids
	    fi
	fi
    fi
done
