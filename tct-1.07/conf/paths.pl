#
#  If you add anything to this file, add it to reconfig!
#
#  No format required, try to keep things alphabetical at top, platform
# specific next, then our internal TCT commands last for sheer readability.
#

$ACCTCOM="/bin/acctcom";
$ARP="/usr/sbin/arp";
$AT="/usr/bin/at";
$CAT="/bin/cat";
$CP="/bin/cp";

$CRONTAB="/usr/bin/crontab";
$DATE="/bin/date";
$DMESG="/bin/dmesg";
$DOMAINNAME="/bin/domainname";
$ECHO="/bin/echo";

$FIND="/usr/bin/find";
$FINGER="/usr/bin/finger";
$IFCONFIG="/sbin/ifconfig";
$IPCS="/usr/bin/ipcs";
$LAST="/usr/bin/last";

$LSOF="/usr/sbin/lsof";
$MKDIR="/bin/mkdir";
$NETSTAT="/bin/netstat";
$PS="/bin/ps";
$PWD="/bin/pwd";

$RPCINFO="/usr/bin/rpcinfo";
$SHOWMOUNT="/sbin/showmount";
$STRINGS="/usr/bin/strings";
$SU="/bin/su";
$SYNC="/bin/sync";
$TEE="/usr/bin/tee";

$TOP="/usr/bin/top";
$UNAME="/bin/uname";
$UPTIME="/usr/bin/uptime";
$W="/usr/bin/w";
$WHO="/usr/bin/who";

$XAUTH="/usr/X11R6/bin/xauth";
$XHOST="/usr/X11R6/bin/xhost";
$YPCAT="/usr/bin/ypcat";

# suns...
$EEPROM="/usr/sbin/eeprom";
$FORMAT="/etc/format";
$SHOWREV="/bin/showrev";
$NFSSTAT="/usr/sbin/nfsstat";

# solaris
$SWAP="/etc/swap";
$PKGINFO="/bin/pkginfo";

# sunos
$DEVINFO="/usr/sbin/devinfo";
$PSTAT="/usr/sbin/pstat";

# linux
$RPM="/etc/rpm";
$KSYMS="/sbin/ksyms";
$LSMOD="/sbin/lsmod";

# kernel modules
$MODINFO="/sbin/modinfo";
$MODSTAT="/usr/bin/modstat";

#
# figure something out to figure out df... bsd vs. sysV
#
$DF="/bin/df";

#
# our stuff
#
$FILE     = "/usr/bin/file";
$MD5      = "/usr/bin/md5sum";
$PCAT     = "/usr/bin/pcat";
$ICAT     = "/usr/bin/inode-cat";
$ILS      = "/usr/bin/ils";
$LASTCOMM = "/usr/bin/lastcomm";
$MAJ_MIN  = "/usr/bin/major_minor";
$TIMEOUT  = "/usr/bin/timeout";

