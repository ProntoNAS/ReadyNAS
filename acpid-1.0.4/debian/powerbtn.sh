#!/bin/sh
# /etc/acpi/powerbtn.sh
# Initiates a shutdown when the power putton has been
# pressed.

# If powersaved is running, let it process the acpi event
if pidof powersaved; then
    exit 0
fi

if ps -Af | grep -q '[k]desktop' && test -f /usr/bin/dcop
then
    dcop --all-sessions --all-users ksmserver ksmserver logout 0 2 0 && exit 0
else
    /sbin/shutdown -h now "Power button pressed"
fi
