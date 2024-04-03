#
# checkfs.sh	Check all filesystems.
#
# Version:	@(#)checkfs  2.83  05-Oct-2001  miquels@cistron.nl
#

. /etc/default/rcS

#
# Check the rest of the file systems.
#
if [ ! -f /fastboot ]
then
    if [ -f /forcefsck ]
    then
        force="-f"
    else
        force=""
    fi
    if [ "$FSCKFIX"  = yes ]
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
    echo "Checking all file systems..."
    fsck $spinner -R -A $fix $force
    if [ $? -gt 1 ]
    then
      echo
      echo "fsck failed.  Please repair manually."
      echo
      echo "CONTROL-D will exit from this shell and continue system startup."
      echo
      # Start a single user shell on the console
      /sbin/sulogin $CONSOLE
    fi
fi
rm -f /fastboot /forcefsck

: exit 0
