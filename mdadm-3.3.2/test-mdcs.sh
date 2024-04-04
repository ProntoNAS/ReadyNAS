#!/bin/bash
#
# test-mdcs.sh
#
#	Test mdcsrepair suite (kernel code, mdcsrepaird, and mdcsrepair)
#	Copyright (c) 2014 NETGERA
#	Copyright (c) 2014 Hiro Sugawara
#
#	Usage: [SBIN=/usr/sbin] test-mdcs.sh <level> [<level>...'
#		level: RAID levels
#

[ "$SBIN" ] 	|| SBIN=/usr/sbin

MD=/dev/md90
MD2=/dev/md91

########################################################################
# If you want to use an array of actual HDD partitions /dev/sdb[1-8]
# instead of loop devices, uncomment the following.
# Partitions must exist with >=100MB in size.
########################################################################
##SDB=sdb
########################################################################

PROG=$SBIN/mdcsrepair
DAEMON=$SBIN/mdcsrepaird
FILTER="-D 9:90"
LOGFILE=/tmp/test-mdcs.log
MNT=/mnt
SUBVOL=$MNT/subvol
IMGSIZE=100M

if [ -z "$*" ]
then
	cat <<_END >&2
Tests MDCSREPIR feature.
Creates loop devices /data/img/loop-img.[0-7] to simulate MD components.

Usage: $0 <level> [<level> ...]
_END
	exit 1
fi

if ! [ -x $PROG ]
then
	echo "Cannot find '$PROG'..." >&2
	exit 1
fi

if ! [ -x $DAEMON ]
then
	echo "Cannot find '$DAEMON'..." >&2
	exit 1
fi

for level in $@
do
	eval RAID$level=yes
done

drop_caches() {
	echo 3 >/proc/sys/vm/drop_caches
}

flush_caches() {
	sync
	sleep 1
	echo 3 >/proc/sys/vm/drop_caches
}

create_img() {
	local s=$1
	echo "Creating ${s}B disk images /data/img/loop-img.*..."
	mkdir -p /data/img
	local i
	for i in 0 1 2 3 4 5 6 7
	do
		truncate -s $s /data/img/loop-img.$i
	done
}

delete_img() {
	rm -f /data/img/loop-img.[0-7]
}

create_loop() {
	echo "Creating loop devices /dev/loop*..."
	local i
	for i in 0 1 2 3 4 5 6 7
	do
		losetup /dev/loop$i /data/img/loop-img.$i
	done
}

delete_loop() {
	losetup -d /dev/loop[0-7] 2>/dev/null
}

randomize() {
	echo "Filling random data to loop devices..."
	local i
	for i in 0 1 2 3 4 5 6 7
	do
		[ $i -ge $1 ]	&& break
		dd conv=fsync if=/dev/urandom of=/dev/loop$i bs=1M 2>/dev/null &
	done
	wait 2>/dev/null
	drop_caches
}

# create_raid <md_dev> <level> <ndisks> [<start> <base> <chunksize> <layout>]
create_raid() {
	local md=$1
	local lvl=$2
	local n=$3
	local n_1=$(($n - 1))
	local start=$4
	local base=$5
	local chunk=$6
	local layout=$7
	local sysf="/sys/block/`basename $md`/md/sync_action"
	local xopt=

	[ "$start" ]	|| start=0
	[ "$base" ]	|| base=loop

	[ "$chunk" ] && xopt="--chunk=$chunk"
	[ "$layout" ] && xopt="$xopt --layout=$layout"

	local range="$(($start+1))-$(($start+${n_1}))"

	echo "Creating RAID$lvl$layout deivce $md with /dev/${base}[0-${n_1}]..."
	##randomize $n
	echo mdadm -C $md -e 1 --level=$lvl --raid-devices=$n	\
		$xopt /dev/${base}[${range}] /dev/${base}${start}
	yes | mdadm -C $md -e 0.90 --level=$lvl --raid-devices=$n	\
		$xopt /dev/${base}[${range}] /dev/${base}${start}	|| exit
	while [ "`cat $sysf`" != idle ]
	do
		sleep 1
	done
	echo "$md created."
}

delete_raid() {
	while [ "$1" ]
	do
		mdadm --stop $1 2>/dev/null
		shift
	done
}

mkfs_raid() {
	mkfs.btrfs -f $1
}

mount_raid() {
	mount -t btrfs $1 $MNT && mount -o remount,rw $MNT
}

unmount_raid() {
	umount $1 2>/dev/null
}

create_subvol() {
	btrfs subvol create $SUBVOL
	btrfs subvol list $MNT
}

populate_raid() {
	local size
	echo "Creating random content files"

	while [ "$1" ]
	do
		for size in 452 4520 45200
		do
			echo "Size: $size bytes in $1"
			dd if=/dev/urandom of=$1/$size bs=$size count=1	\
				2>/dev/null
		done
		shift
	done
}

delete_all() {
	echo "Releasing all resources..."
	killall `basename $DAEMON` 2>/dev/null
	unmount_raid $MNT
	delete_raid $MD $MD2 2>/dev/null
	sleep 10
	delete_loop
	delete_img
}

# Return computed BAD CRC
# md_crc <md_dev> <pos> <len>
md_crc() {
	md=`echo $1 | sed 's|/|\\\\/|g'`
	$PROG -n -v -d $1 $2 $3 00000000 11111111 2>/dev/null |	\
		awk "/^$md BAD  \@ $2 Expected: / { print \$NF }"
}

# Return component disk's position
# comp_pos <md_dev> <comp_dev> <pos> <len> [P|Q]
comp_pos() {
	cd=`echo $2 | sed 's|/|\\\\/|g'`
	case "$5" in
	P|Q)
		$PROG -n -v -d $1 $3 $4 00000000 11111111 2>/dev/null | \
			awk "/\[$5\] $cd GOOD \@ .+ Expected: / { print \$6 }"
	;;
	*)
		$PROG -n -v -d $1 $3 $4 00000000 11111111 2>/dev/null | \
			awk "/^$cd BAD  \@ .+ Expected: / { print \$4 }"
	;;
	esac
}

# List data/P/Q component device
# comp_dev <md_dev> <pos> <len> [P|Q|E]
comp_dev() {
	local comp
	local dev
	local array="`$PROG -n -v -d $1 $2 $3 00000000 11111111 2>/dev/null | \
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
	echo "/dev/$name"
}

# List all component devices
# comp_devs <md_dev> <pos> <len>
comp_devs() {
	$PROG -n -v -d $1 $2 $3 00000000 11111111 2>/dev/null | \
		grep '^\[[[:digit:]]\]' |	\
		sed 's|\[.\](|/dev/|g;s|)||g'
}

# Trash component device
# zero_dd <comp_dev> <pos> <len>
zero_dd() {
	echo "Trashing $1 @ $2..."
	dd conv=fsync if=/dev/zero of=$1 seek=$2 bs=1 count=$3 2>/dev/null
	sleep 3
	flush_caches
}

# test_header <level> "<title>"
test_header() {
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	echo "RAID$1: $2"
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
}

debug_on() {
	local colon=':'
	[ "$1" ] && colon=$1
	
	echo $colon >/proc/fs/btrfs/mdcsrepair
}

debug_off() {
	echo >/proc/fs/btrfs/mdcsrepair
}

test_mount() {
	local level=$1
	local file="fs/btrfs/disk-io.c"

	test_header $level "Testing mount/super block [$file]"

	rm -fr $LOGFILE
	$DAEMON -d -n 2>/dev/null >$LOGFILE &
	local dpid=$!
	sleep 1

	debug_on "$file:"
	mount_raid $MD
	unmount_raid $MNT
	kill $dpid
	wait 2>/dev/null
	debug_off

	local pos="`awk '{ print $4}' $LOGFILE`"
	local len="`awk '{ print $2}' $LOGFILE`"

	local cdev=`comp_dev $MD $pos $len x`
	local cpos=`comp_pos $MD $cdev $pos $len`

	echo "[$file] accesses $len bytes @$cpos on $cdev ($pos on $MD)."

	zero_dd $cdev $cpos 2
	$DAEMON -d &
	dpid=$!
	sleep 3

	echo "Trying to mount trashed array..."
	if mount_raid $MD
	then
		echo ">>>>>>>>> FAIL <<<<<<<<<<<"
		echo "Trashing component disk $cdev did not work out."
		exit 1
	fi
	echo "Filesystem mount failure as expected."
	sleep 5
	kill $dpid
	wait 2>/dev/null

	echo "Retrying to mount repaired array..."
	if mount_raid $MD
	then
		echo "Filesystem mount succeeded."
		echo ">>>>>>>>> SUCCESS <<<<<<<<<<<"
		echo
	else
		echo ">>>>>>>>> FAIL <<<<<<<<<<<"
		echo "Trashed component disk $cdev still affects MD."
		exit 1
	fi
	unmount_raid $MNT
}

test_inode() {
	local level=$1
	local lister="$2"
	local vfy1="$3"
	local vfy2="$4"
	local file=$5

	mount_raid $MD
	debug_on "$file:"

	rm -f $LOGFILE
	$DAEMON $FILTER -d -n 2>/dev/null >$LOGFILE &
	local dpid=$!
	flush_caches

	$lister 2>/dev/null
	sleep 5
	kill $dpid
	wait 2>/dev/null
	debug_off

	rm -f $LOGFILE.2
	head -q -n 20 $LOGFILE >$LOGFILE.2
	tail -q -n 20 $LOGFILE >>$LOGFILE.2
	mv -f $LOGFILE.2 $LOGFILE

	local line
	local fut
	local fpos

	$DAEMON -d &
	dpid=$!
	sleep 3

	local dovfy1
	local dovfy2
	local line1
	while read line
	do
		if echo $line | grep -q '^\['
		then
			echo $line
			fut=`echo $line | sed 's|^.*:||;s| .*$||'`
			fpos=`echo $line | awk '{ print $4 }'`
			dovfy1=`printf "$vfy1" $fut`
			dovfy2=`printf "$vfy2" $fut`
			continue
		elif ! echo $line | grep -q '^/dev/'
		then
			continue
		fi

		local len=`echo $line | awk '{ print $2 }'`
		local pos=`echo $line | awk '{ print $4 }'`
		local bcrc=`echo $line| awk '{ print $7 }' |	\
			awk -F : '{ print $2 }'`
		local gcrc=`echo $line| awk '{ print $7 }' |	\
			awk -F : '{ print $2 }'`

		tgt=`echo $line | awk '{ print $1 }'`

		if [ "$tgt" != $MD -a "$tgt" != $MD2 ]
		then
			echo $line
			echo ">>>>>>>>> Untargeted device <<<<<<<<<<<"
			echo
			continue
		fi

		if [ "$bcrc" != "$gcrc" ]
		then
			echo $line
			echo ">>>>>>>>> CRC error <<<<<<<<<<<"
			echo
			continue
		fi

		if [ "$fpos" ]
		then
			local clen=$((`stat -c %s $fut`-$fpos))
			[ $len -lt $clen ] && clen=$len
			if ! cmp -s -i $fpos:$pos -n $clen $fut $tgt
			then
				echo $line
				echo ">>>>>>>>> Untraceable instance <<<<<<<<<<"
				echo
				continue
			fi
		fi

		local cdev=`comp_dev $tgt $pos $len x`
		local cpos=`comp_pos $tgt $cdev $pos $len`

		zero_dd $cdev $cpos 2
		echo "Accessing trashed file $fut..."

		if eval "$dovfy1" 2>/dev/null
		then
			echo ">>>>>>>>> FAIL <<<<<<<<<<<"
			echo "Trashing component disk $cdev did not work out."
			exit 1
		fi
		echo "File access failure as expected."

		sleep 2
		echo "Re-accessing repaired file $fut..."
		if eval "$dovfy2"
		then
			echo "File access succeeded."
			echo ">>>>>>>>> SUCCESS <<<<<<<<<<<"
			echo
		else
			echo "File access failed."
			echo ">>>>>>>>> FAIL <<<<<<<<<<<"
			echo
			exit 1
		fi
	done <$LOGFILE

	sleep 5
	kill $dpid
	wait 2>/dev/null
	unmount_raid $MNT
}

test_find() {
	local level=$1
	local root=$2

	file=fs/btrfs/inode.c
	test_header $level "Testing file data blocks [$file] @$root"
	test_inode $level	\
		"find $root -type f -exec dd if={} of=/dev/null iflag=direct ;"\
		"dd if=%s of=/dev/null iflag=direct"	\
		"dd if=%s of=/dev/null iflag=direct"	\
		$file
}

test_find_jbod() {
	local level=$1
	local root=$2

	file=fs/btrfs/inode.c
	test_header $level "Testing data blocks on added disk [$file] @$root"
	echo "Adding more md device to btrfs..."
	mount_raid $MD
	btrfs device add $MD2 $MNT
	echo "Creating test file spanning 2 disks..."
        dd if=/dev/urandom of=$root/jbod bs=$IMGSIZE count=4 2>/dev/null
	unmount_raid $MD

	local filter="$FILTER"
	FILTER="-D 9:91"
	test_inode $level       \
		"dd if=$MNT/jbod of=/dev/null iflag=direct"\
		"dd if=%s of=/dev/null iflag=direct"    \
		"dd if=%s of=/dev/null iflag=direct"    \
		$file
	FILTER="$filter"
}

test_scrub() {
	local level=$1
	local root=$2

	file=fs/btrfs/scrub.c
	test_header $level "Testing scrubbed data blocks [$file] @$root"
	test_inode $level	\
		"btrfs scrub start -B $root"	\
		"! btrfs scrub start -B $root |	\
			grep -q -E 'error details: csum|super=1'"\
		"btrfs scrub start -B $root | grep -q 'with 0 errors'"\
		$file
}

test_compress() {
	local level=$1
	local root=$2

	file=fs/btrfs/compression.c
	test_header $level "Testing compressed data blocks [$file] @$root"
	echo "Creating a compressed file..."

	local src=/etc/mime.types
	mount_raid $MD
	rm -f $root/compressed
	touch $root/compressed
	chattr +c $root/compressed
	cat $src $src >>$root/compressed
	unmount_raid $MNT

	test_inode $level	\
		"dd if=$root/compressed of=/dev/null iflag=direct"	\
		"dd if=%s of=/dev/null iflag=direct"	\
		"dd if=%s of=/dev/null iflag=direct"	\
		$file
}

runtest() {
	level=$1
	delete_raid $MD $MD2
	local ndisks=2

	case $level in
		5)	ndisks=4
			;;
		6)	ndisks=4
			;;
		*)
			echo "Unsupported RAID level $level." >&2
			exit 1
			;;
	esac

	if [ "$SDB" ]
	then
		create_raid $MD $level $ndisks 1 $SDB
	else
		create_raid $MD $level $ndisks 0
	fi
	mkfs_raid $MD

	# test_mount must run for all cases
	test_mount $level

	mount_raid $MD
	create_subvol $SUBVOL
	populate_raid $MNT $SUBVOL
	unmount_raid $MNT

	# Each individual test may be commented out.
	for root in $MNT $SUBVOL
	do
		test_find $level $root
		test_scrub $level $root
		test_compress $level $root
	done

	if [ "$SDB" ]
	then
		create_raid $MD2 $level $ndisks 5 $SDB
	else
		create_raid $MD2 $level $ndisks 4
	fi
	test_find_jbod $level $MNT
	delete_raid $MD $MD2
}

delete_all

if [ -z "$SDB" ]
then
	create_img $IMGSIZE
	create_loop
fi

[ "$RAID1" ] && runtest 1
[ "$RAID5" ] && runtest 5
[ "$RAID6" ] && runtest 6
[ "$RAID10" ] && runtest 10
