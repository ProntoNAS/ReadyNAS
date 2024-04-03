#! /bin/sh
# $Id: iptables_display.sh,v 1.1.1.1 2007-01-12 02:38:46 jmaggard Exp $
IPTABLES=iptables

#display all chains relative to miniupnpd
$IPTABLES -v -n -t nat -L PREROUTING
$IPTABLES -v -n -t nat -L MINIUPNPD
$IPTABLES -v -n -t filter -L FORWARD
$IPTABLES -v -n -t filter -L MINIUPNPD

