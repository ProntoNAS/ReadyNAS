#
# checkroot.sh	Check to root file system.
#
# Version:	@(#)checkroot.sh  2.84-3  25-Jan-2002  miquels@cistron.nl
#

. /etc/default/rcS

#
# Set SULOGIN in /etc/default/rcS to yes if you want a sulogin to be spawned
# from this script *before anything else* with a timeout, like SCO does.
#
[ "$SULOGIN" = yes ] && sulogin -t 30 $CONSOLE

#
# Ensure that bdflush (update) is running before any major I/O is
# performed (the following fsck is a good example of such activity :).
#
[ -x /sbin/update ] && update

#
# Read /etc/fstab.
#
exec 9>&0 </etc/fstab
rootmode=rw
rootopts=rw
rootcheck=yes
swap_on_md=no
devfs=
while read fs mnt type opts dump pass junk
do
	case "$fs" in
		""|\#*)
			continue;
			;;
		/dev/md*)
			# Swap on md device.
			[ "$type" = swap ] && swap_on_md=yes
			;;
		/dev/*)
			;;
		*)
			# Might be a swapfile.
			[ "$type" = swap ] && swap_on_md=yes
			;;
	esac
	[ "$type" = devfs ] && devfs="$fs"
	[ "$mnt" != / ] && continue
	rootopts="$opts"
	[ "$pass" = 0 -o "$pass" = "" ] && rootcheck=no
	case "$opts" in
		ro|ro,*|*,ro|*,ro,*)
			rootmode=ro
			;;
	esac
done
exec 0>&9 9>&-

#
# Mount /proc. If /proc/1 exists, but /proc is not mounted,
# issue a warning so that the user knows something is wrong.
#
doproc=yes
if [ -d /proc/1 ]
then
	rootino=`ls -lid /proc | sed -ne 's/^ *\([0-9]\+\).*$/\1/p'`
	if [ "$rootino" -gt 2 ]
	then
		echo "WARNING: found junk under the /proc mountpoint"
	else
		doproc=no
	fi
fi
[ "$doproc" = yes ] && mount -n /proc

#
# Activate the swap device(s) in /etc/fstab. This needs to be done
# before fsck, since fsck can be quite memory-hungry.
#
doswap=no
case "`uname -r`" in
	2.[0123].*)
		if [ $swap_on_md = yes ] && grep -qs resync /proc/mdstat
		then
			[ "$VERBOSE" != no ] &&
			  echo "Not activating swap - RAID array resyncing"
		else
			doswap=yes
		fi
		;;
	*)
		doswap=yes
		;;
esac
if [ $doswap = yes ]
then
	[ "$VERBOSE" != no ] && echo "Activating swap."
	swapon -a 2> /dev/null
fi

#
# Check the root file system.
#
if [ -f /fastboot ] || [ $rootcheck = no ]
then
  [ $rootcheck = yes ] && echo "Fast boot, no file system check"
else
  #
  # Ensure that root is quiescent and read-only before fsck'ing.
  #
  mount -n -o remount,ro /
  if [ $? = 0 ]
  then
    if [ -f /forcefsck ]
    then
	force="-f"
    else
	force=""
    fi
    if [ "$FSCKFIX" = yes ]
    then
	fix="-y"
    else
	fix="-a"
    fi
    spinner="-C"
    case "$TERM" in
        dumb|network|unknown|"") spinner="" ;;
    esac
    [ `uname -m` = s390 ] && spinner="" # This should go away
    echo "Checking root file system..."
    fsck $spinner $force $fix /
    #
    # If there was a failure, drop into single-user mode.
    #
    # NOTE: "failure" is defined as exiting with a return code of
    # 2 or larger.  A return code of 1 indicates that file system
    # errors were corrected but that the boot may proceed.
    #
    if [ $? -gt 1 ]
    then
      # Surprise! Re-directing from a HERE document (as in
      # "cat << EOF") won't work, because the root is read-only.
      echo
      echo "fsck failed.  Please repair manually and reboot.  Please note"
      echo "that the root file system is currently mounted read-only.  To"
      echo "remount it read-write:"
      echo
      echo "   # mount -n -o remount,rw /"
      echo
      echo "CONTROL-D will exit from this shell and REBOOT the system."
      echo
      # Start a single user shell on the console
      /sbin/sulogin $CONSOLE
      reboot -f
    fi
  else
    echo "*** ERROR!  Cannot fsck root fs because it is not mounted read-only!"
    echo
  fi
fi

#
#	If the root filesystem was not marked as read-only in /etc/fstab,
#	remount the rootfs rw but do not try to change mtab because it
#	is on a ro fs until the remount succeeded. Then clean up old mtabs
#	and finally write the new mtab.
#
mount -n -o remount,$rootmode /
if [ "$rootmode" = rw ]
then
	rm -f /etc/mtab~ /etc/nologin
	: > /etc/mtab
	mount -f -o remount /
	mount -f /proc
	[ "$devfs" ] && grep -q '^devfs /dev' /proc/mounts && mount -f "$devfs"
fi

: exit 0

