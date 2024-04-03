#! /bin/sh
# $Id: iptables_flush.sh,v 1.1.1.1 2007-01-12 02:38:46 jmaggard Exp $
IPTABLES=iptables

#flush all rules owned by miniupnpd
$IPTABLES -t nat -F MINIUPNPD
$IPTABLES -t filter -F MINIUPNPD

