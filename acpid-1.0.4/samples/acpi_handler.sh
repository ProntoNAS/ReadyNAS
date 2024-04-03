#!/bin/sh
# a sample skeleton for handling ACPI events

if [ $# != 1 ]; then
	exit 1
fi
set $*

case "$1" in
	button)
		case "$2" in
			power)	/sbin/init 0
				;;
			*)	logger "ACPI action $2 is not defined"
				;;
		esac
		;;

	*)
		logger "ACPI group $1 is not defined"
		;;
esac
