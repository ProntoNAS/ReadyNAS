#!/bin/sh
#
# trash-mdcs.sh
#
#	Trash RAID file data blocks 
#	For demo to repair corrupted RAID component
#	Copyright (c) 2014 NETGEAR
#	Copyright (c) 2014,2015 Hiro Sugawara
#
#	Usage: [SBIN=/usr/sbin] trash-mdcs.sh <file> [<len>@<pos> ...]'
#

MYNAME=`basename $0`

[ "$SBIN" ] 	|| SBIN=/usr/sbin
PROG=$SBIN/mdcsrepair
DAEMON=$SBIN/mdcsrepaird
MNT=/mnt

ZERO=yes
COMPONENT=yes
DRY=no

help() {
	cat <<_END >&2
Demonstrates MDCSREPIR feature.
Usage: $0 [-n][-r][-R] <file> [<size>@<pos> ...]
       -n   Dry run - do not actually trash file
       -r   Random data instead of zeroing
       -R   Trash RAID instead of component(s)
_END
	exit 1
}

warn() {
	echo "$MYNAME: $*" >&2
}

while true
do
	case "$1" in
		-n)	DRY=yes; shift;;
		-r)	RANDOM=no; shift;;
		-R)	COMPONENT=no; shift;;
		-*)	help;;
		*)	break;;
	esac
done

if [ -z "$*" ]
then
	help
fi

if ! [ -x $PROG ]
then
	warn "Cannot find '$PROG'..."
	exit 1
fi

if ! [ -e $1 ]
then
	warn "Cannot find '$1'..."
	exit 1
fi

flush_caches() {
	sync
	sleep 1
	echo 3 >/proc/sys/vm/drop_caches
}

# Return component disk's position
# comppos <md_dev> <comp_dev> <pos> <len> [P|Q]
comp_pos() {
	cd=`echo $2 | sed 's|/|\\\\/|g'`
	case "$4" in
	P|Q)
		$PROG -n -v -d -w $1 $3 $4 00000000 11111111 2>/dev/null | \
			awk "/\[$4\] $cd GOOD \@ .+ Expected: / { print \$6 }"
	;;
	*)
		$PROG -n -v -d -w $1 $3 $4 00000000 11111111 2>/dev/null | \
			awk "/^$cd BAD  \@ .+ Expected: / { print \$4 }"
	;;
	esac
}

# List data/P/Q component device
# comp_dev <md_dev> <pos> <len> [P|Q|E]
comp_dev() {
	local comp
	local dev
	local array="`$PROG -n -v -d -w $1 $2 $3 00000000 11111111 2>/dev/null | \
		grep '^\[0'`"
	for comp in $array X
	do
		case "$4" in
		P|Q)
			echo "$comp" | grep -q "\[$4\]" && break
			;;
		E)
			echo "$comp" | grep -q "\[[[:digit:]]\]" && break
			;;
		*)
			echo "$comp" | grep -q "\[[[:digit:]]\*\]" && break
			;;
		esac
	done

	[ -z "$comp" -o "$comp" = X ] && return
	local name=`echo $comp | sed 's|^.*(||;s|).*||'`
	if [ "$name" = 'F' ]
	then
		warn "$1 has a failed component."
		return
	fi
	echo "/dev/$name"
}

# Trash component device
# zero_dd_lite <comp_dev> <pos> <len>
zero_dd_lite() {
	local source=/dev/zero
	[ "ZERO" != yes ]	&& source=/dev/urandom
	echo "Trashing $1 @ $2..."
	if [ "$DRY" != yes ]
	then
		dd conv=fsync if=$source of=$1 seek=$2 bs=1 count=$3 2>/dev/null
	fi
}

debug_on() {
	local colon=':'
	[ "$1" ] && colon=$1
	
	echo $colon >/proc/fs/btrfs/mdcsrepair
}

debug_off() {
	echo >/proc/fs/btrfs/mdcsrepair
}

fsof() {
	if ! which realpath >/dev/null 2>&1
	then
		apt-get -y install realpath >/dev/null 2>&1
	fi

	local f=`realpath $1`	|| exit

	while [ "$f" != / ]
	do
		local d=`df $f | awk '! /^Filesystem/ { print $1 }'`
		case "$d" in
			/dev/*)
				echo $d
				return
				;;
			-)
				f=`dirname $f`
				;;
			*)
				warn "cannot examine filesystem for $f" >&2
				exit 1
			;;
		esac
	done
}

trash() {
	local file=`realpath $1`
	local ino=`stat --printf="%i\\n" $file`
	local len=`echo $2 | sed 's|@.*$||'`
	local pos=`echo $2 | sed 's|^.*@||'`

	if echo "$len$pos" | grep -qvE '[[:digit:]]'
	then
		warn "Bad <len>@<pos> '$2'" >&2
		exit 1
	fi

	local debug=`cat /proc/fs/btrfs/mdcsrepair`
	#echo fs/btrfs/inode.c: >/proc/fs/btrfs/mdcsrepair
	echo -n :$ino >/proc/fs/btrfs/mdcsrepair
	local blklen=4096
	local tmpfile="/tmp/trash-mdcs.$$.tmp"

	while [ $len -gt 0 ]
	do
		local fbx=$((($pos/$blklen)*$blklen))
		local thislen=$(($len%$blklen))
		[ $thislen = 0 ] && thislen=$blklen
		rm -f $tmpfile
		$DAEMON -v -d -w >$tmpfile &
		local dpid=$!
		flush_caches
		dd if=$file of=/dev/null bs=1 skip=$pos count=1 2>/dev/null
		sleep 3
		kill $dpid 2>/dev/null
		wait 2>/dev/null
		local fb_start=`grep -m 1 "\] $ino:$file @ $fbx\$" $tmpfile | \
				awk '{ print $NF }'`
		local blk_off=$(($pos-$fb_start))

		local md=`grep -m 1 -A 1 "\] $ino:$file @ $fbx\$" $tmpfile | \
				tail -1 | \
				awk '{ print $1 }'`
		local dpos=`grep -m 1 -A 1 "\] $ino:$file @ $fbx\$" $tmpfile | \
				tail -1 | \
				awk '{ print $4 }'`
		dpos=$(($dpos+$blk_off))

		if [ "$COMPONENT" = yes ]
		then
			local cdev=`comp_dev $md $dpos $thislen x`
			[ "$cdev" ] || exit 1
			local cpos=`comp_pos $md $cdev $dpos $thislen`
			[ "$cpos" ] || exit 1

			zero_dd_lite $cdev $cpos $thislen
		else
			zero_dd_lite $md $dpos $thislen
		fi

		len=$(($len-$thislen))
		pos=$(($pos+$thislen))
	done
	echo $debug >/proc/fs/btrfs/mdcsrepair

	rm -f $tmpfile
	sleep 3
	flush_caches
}

file2trash=$1
shift

if [ $# = 0 ]
then
	trash $file2trash "2@0"
else
	while [ $# -ge 1 ]
	do
		trash $file2trash $1
		shift
	done

	##repair $file2trash
fi
