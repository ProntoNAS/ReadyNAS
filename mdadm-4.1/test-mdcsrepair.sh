#!/bin/bash
#
# test-mdcsrepair.sh
#
#	Test mdcsrepair in a variety of modes
#	Copyright (c) 2014 NETGERA
#	Copyright (c) 2014 Hiro Sugawara
#
#	Usage: [SBIN=/usr/sbin] test-mdcsrepair.sh <level> [<level>...'
#		level: RAID levels
#

[ "$SBIN" ] 	|| SBIN=/usr/sbin
PROG=$SBIN/`basename $0 .sh | sed 's|^test-||'`
if [ -z "$*" ]
then
	cat <<_END >&2
Tests $PROG.
Creates loop devices /data/img/loop-img.[0-5] to simulate MD components.

Usage: $0 <level> [<level> ...]
_END
	exit 1
fi

if ! [ -x $PROG ]
then
	echo "Cannot find '$PROG'..." >&2
	exit 1
fi

MD=/dev/md90

for level in $@
do
	eval RAID$level=yes
done

POS="41000 820000 5230000"
LEN="100 210"

drop_caches() {
	echo 3 >/proc/sys/vm/drop_caches
}

create_img() {
	local s=$1
	echo "Creating ${s}B disk images /data/img/loop-img.*..."
	mkdir -p /data/img
	local i
	for i in 0 1 2 3 4 5
	do
		truncate -s $s /data/img/loop-img.$i
	done
}

delete_img() {
	rm -f /data/img/loop-img.[0-5]
}

create_loop() {
	echo "Creating loop devices /dev/loop*..."
	local i
	for i in 0 1 2 3 4 5
	do
		losetup /dev/loop$i /data/img/loop-img.$i
	done
}

delete_loop() {
	losetup -d /dev/loop[0-5]
}

randomize() {
	echo "Filling random data to loop devices..."
	local i
	for i in 0 1 2 3 4 5
	do
		[ $i -ge $1 ]	&& break
		dd conv=fsync if=/dev/urandom of=/dev/loop$i bs=1M 2>/dev/null &
	done
	wait
	drop_caches
}

# create_raid <md_dev> <level> <ndisks> <chunksize> <layout>
create_raid() {
	local md=$1
	local lvl=$2
	local n=$3
	local n_1=$(($n - 1))
	local chunk=$4
	local layout=$5
	local sysf="/sys/block/`basename $md`/md/sync_action"
	local xopt=
	[ "$chunk" ] && xopt="--chunk=$chunk"
	[ "$layout" ] && xopt="$xopt --layout=$layout"

	echo "Creating RAID$lvl$layout deivce $md with /dev/loop[0-${n_1}]..."
	randomize $n
	echo mdadm -C $md -e 0.90 --level=$lvl --raid-devices=$n	\
		$xopt /dev/loop[1-${n_1}] /dev/loop0
	yes | mdadm -C $md -e 0.90 --level=$lvl --raid-devices=$n	\
		$xopt /dev/loop[1-${n_1}] /dev/loop0	|| exit
	while [ "`cat $sysf`" != idle ]
	do
		sleep 1
	done
	echo "$md created."
}

unmount_raid() {
	local entry=`grep -q "^$1 " /proc/mounts`
	if [ "$entry" ]
	then
		umount `echo $entry | awk '{ print $2 }'`
	fi
}

delete_raid() {
	mdadm --stop $1 2>/dev/null
}

delete_all() {
	echo "Releasing all resources..."
	unmount_raid $MD
	delete_raid $MD
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
# comppos <md_dev> <comp_dev> <pos> <len> [P|Q]
comp_pos() {
	cd=`echo $2 | sed 's|/|\\\\/|g'`
	case "$5" in
	P|Q)
		$PROG -n -v -d $1 $3 $4 00000000 11111111 2>/dev/null | \
			awk "/\[$4\] $cd GOOD \@ .+ Expected: / { print \$6 }"
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
	drop_caches
}

# Test program driver
# success_fail <pos> <len> <good_crc> program...
success_fail() {
	local pos=$1
	shift
	local len=$1
	shift
	local good=$1
	shift

	# Run test program
	$@
	local result=$?

	local after=`md_crc $MD $pos $len`

	if [ $result = 0 -a $after = $good ]
	then
		echo ">>>>>>>> SUCCESS <<<<<<<<<<<"
	else
		echo ">>>>>>>>> FAIL <<<<<<<<<<<"
		return 1
	fi
}

# test_header <level> <ndisks> <dsize> <pos> <len> [<txt>]
test_header() {
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	echo "RAID$1: $6 Corrupt @$4 $5 bytes on $2 $3B disks"
	echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
}

# test_raid1 <pos> <size> <raid10txt>
test_raid1() {
	test_header 1$3 $ndisks $size $1 $2
	local p=$1
	local s=$2
	local good=`md_crc $MD $p $s`


	# It is unknown which component disk is used for this position.
	# Try until trashing is found effective.
	for comp in `comp_devs $MD $p $s`
	do
		local cpos=`comp_pos $MD $comp $p $s`
		zero_dd $comp $cpos 10
		local bad=`md_crc $MD $p $s`
		if [ $bad != $good ]
		then
			success_fail $p $s $good $PROG -v -d $MD $p $s $bad $good
			local res=$?
			echo
			return $res
		fi

		# Restore component disk from MD.
		dd conv=fsync if=$MD skip=$p of=$comp seek=$cpos	\
			bs=1 count=10 2>/dev/null
	done
	echo "#### Could not simulate data corruption."
	return 1
}

# test_raid5 <pos> <size>
test_raid5() {
	test_header 5 $ndisks $size $1 $2
	local good=`md_crc $MD $1 $2`
	local bdisk=`comp_dev $MD $1 $2`
	local cpos=`comp_pos $MD $bdisk $1 $2`
	zero_dd $bdisk $cpos 10
	local bad=`md_crc $MD $1 $2`

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	echo
	return $res
}

# Restore failed disk
restore_disk() {
	local md=$1
	local fdisk=$2
	local sysf="/sys/block/`basename $md`/md/sync_action"
	mdadm $md -a $fdisk
	while [ `cat $sysf` != idle ]
	do
		sleep 1
	done
	echo
}

# Dx and Q have bad CRC.
test_raid6_dq() {
	test_header 6 $ndisks $size $1 $2 "D and Q"
	zero_dd $ddisk $dpos 10
	local bad=`md_crc $MD $1 $2`
	zero_dd $qdisk $qpos 10

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	echo
	return $res
}

# Dx and P have bad CRC.
test_raid6_dp() {
	test_header 6 $ndisks $size $1 $2 "D and P"
	zero_dd $ddisk $dpos 10
	local bad=`md_crc $MD $1 $2`
	zero_dd $pdisk $ppos 10

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	echo
	return $res
}

# Dx has bad CRC. Dy is failed.
test_raid6_d_e() {
	test_header 6 $ndisks $size $1 $2 "E failed and D"
	zero_dd $ddisk $dpos 10
	mdadm $MD -f $edisk
	mdadm $MD -r $edisk
	local bad=`md_crc $MD $1 $2`

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	restore_disk $MD $edisk
	return $res
}

# P has bad CRC. Dx is failed.
test_raid6_p_d() {
	test_header 6 $ndisks $size $1 $2 "D failed and P"
	zero_dd $ddisk $dpos 10
	mdadm $MD -f $edisk
	mdadm $MD -r $edisk
	local bad=`md_crc $MD $1 $2`

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	restore_disk $MD $edisk
	return $res
}

# Dx has bad CRC. P is failed.
test_raid6_d_p() {
	test_header 6 $ndisks $size $1 $2 "P failed and D"
	zero_dd $ddisk $dpos 10
	mdadm $MD -f $pdisk
	mdadm $MD -r $pdisk
	local bad=`md_crc $MD $1 $2`

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	restore_disk $MD $pdisk
	return $res
}

# Dx has bad CRC. Q is failed.
test_raid6_d_q() {
	test_header 6 $ndisks $size $1 $2 "Q failed and D"
	zero_dd $ddisk $dpos 10
	mdadm $MD -f $qdisk
	mdadm $MD -r $qdisk
	local bad=`md_crc $MD $1 $2`

	success_fail $1 $2 $good $PROG -v -d $MD $1 $2 $bad $good
	local res=$?
	restore_disk $MD $qdisk
	return $res
}

#
# RAID6 tests various bad/fail patterns
# test_raid6 <pos> <size>
#
test_raid6() {
	local good=`md_crc $MD $1 $2`
	local ddisk=`comp_dev $MD $1 $2`
	local edisk=`comp_dev $MD $1 $2 E`
	local pdisk=`comp_dev $MD $1 $2 P`
	local qdisk=`comp_dev $MD $1 $2 Q`
	local dpos=`comp_pos $MD $ddisk $1 $2`
	local ppos=`comp_pos $MD $pdisk $1 $2 P`
	local qpos=`comp_pos $MD $qdisk $1 $2 Q`
	test_raid6_dq $@	|| return 1
	test_raid6_dp $@	|| return 1
	test_raid6_d_e $@	|| return 1
	test_raid6_p_d $@	|| return 1
	test_raid6_d_p $@	|| return 1
	test_raid6_d_q $@	|| return 1
}


raid1() {
	for ndisks in 2 3
	do
		delete_raid $MD
		create_raid $MD 1 $ndisks
		for pos in $POS
		do
			for len in $LEN
			do
				test_raid1 $pos $len	|| exit 1
			done
		done
	done
}

raid5() {
	for ndisks in 4 6
	do
		for chunk in 64 512
		do
			delete_raid $MD
			create_raid $MD 5 $ndisks $chunk
			for pos in $POS
			do
				for len in $LEN
				do
					test_raid5 $pos $len	|| exit 1
				done
			done
		done
	done
}

raid6() {
	for ndisks in 5 6
	do
		for chunk in 64 512
		do
			delete_raid $MD
			create_raid $MD 6 $ndisks $chunk
			for pos in $POS
			do
				for len in $LEN
				do
					test_raid6 $pos $len	|| exit 1
				done
			done
		done
	done
}

# RAID10 tests various layouts
raid10() {
	for ndisks in 5 6
	do
	    for layout in n2 n3 f2 f3 o2 o3
	    do
		for chunk in 64 512
		do
			delete_raid $MD
			create_raid $MD 10 $ndisks $chunk $layout
			for pos in $POS
			do
				for len in $LEN
				do
				    test_raid1 $pos $len "0$layout" || exit 1
				done
			done
		done
	    done
	done
}

for size in 10M 23M 49M
do
	delete_all

	create_img $size
	create_loop

	[ "$RAID1" ] && raid1
	[ "$RAID5" ] && raid5
	[ "$RAID6" ] && raid6
	[ "$RAID10" ] && raid10
done
