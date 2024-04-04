#! /bin/bash
#	Copyright (C) 2012 Canonical, Ltd.
#
#	This program is free software; you can redistribute it and/or
#	modify it under the terms of the GNU General Public License as
#	published by the Free Software Foundation, version 2 of the
#	License.

#=NAME onexec
#=DESCRIPTION 
# Verifies basic file access permission checks for change_onexec
#=END

pwd=`dirname $0`
pwd=`cd $pwd ; /bin/pwd`

bin=$pwd

. $bin/prologue.inc

file=$tmpdir/file
subfile=$tmpdir/file2
okperm=rw

othertest="$pwd/rename"
subtest="sub"
fqsubbase="$pwd/onexec"
fqsubtest="$fqsubbase//$subtest"
subtest2="$pwd//sub2"
subtest3="$pwd//sub3"

onexec="/proc/*/attr/exec"

touch $file $subfile

check_exec()
{
    local rc
    local actual
    local desc="$1"
    local pid="$2"
    local expected="$3"
    actual=`cat /proc/${pid}/attr/exec 2>/dev/null`
    rc=$?

    # /proc/${pid}/attr/exec returns invalid argument if onexec has not been called
    if [ $rc -ne 0 ] ; then
	if [ "${expected}" == "nochange" ] ; then
	    return 0
	fi
	echo "ONEXEC (${desc}) - exec transition not set"
	return $rc
    fi
    if [ "${actual% (*)}" != "${expected}" ] ; then
	echo "ONEXEC (${desc}) - check exec '${actual% (*)}' != expected '${expected}'"
	return 1
    fi

    return 0
}

check_current()
{
    local rc
    local actual
    local desc="$1"
    local pid="$2"
    local expected="$3"
    actual=`cat /proc/${pid}/attr/current 2>/dev/null`
    rc=$?

    # /proc/${pid}/attr/current return enoent if the onexec process already exited due to error
    if [ $rc -ne 0 ] ; then
	# These assume a check has already been done to see if the
	# task is still around
	echo -n "ONEXEC - check current ($1): "
	cat /proc/${pid}/attr/current
	return $rc
    fi

    if [ "${actual% (*)}" != "${expected}" ] ; then
	echo "ONEXEC - check current (${desc}) '${actual% (*)}' != expected '${expected}'"
	return 1
    fi

    return 0
}

do_test()
{
    local desc="$1"
    local prof="$2"
    local target_prof="$3"
    local res="$4"
    shift 4

    #ignore prologue.inc error trapping that catches our subfn return values

    runtestbg "ONEXEC $desc ($prof -> $target_prof)" $res $target_prof "$@"
    # check that transition does not happen before exec, and that transition
    # is set

    # give the onexec process a chance to run
    sleep 0.05

    # check that task hasn't exited because change_onexec failed
    if ! [ -d "/proc/${_pid}" ] ; then
	checktestfg
	return
    fi

    if ! check_current "${desc}" $_pid $prof ; then
	checktestfg
	return
    fi

    if ! check_exec "${desc}" $_pid $target_prof ; then
	checktestfg
	return
    fi

    kill -CONT $_pid

    checktestbg
}


# ONEXEC from UNCONFINED - don't change profile
do_test "" unconfined nochange pass $bin/open $file

# ONEXEC from UNCONFINED - target does NOT exist
genprofile image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open
do_test "" unconfined noexist fail $bin/open $file

# ONEXEC from UNCONFINED - change to rw profile, no exec profile to override
genprofile image=$bin/rw $bin/open:rix $file:rw
do_test "no px profile" unconfined $bin/rw pass $bin/open $file

# ONEXEC from UNCONFINED - don't change profile, make sure exec profile is applied
genprofile image=$bin/rw $bin/open:px $file:rw  -- image=$bin/open $file:rw
do_test "nochange px" unconfined nochange pass $bin/open $file

# ONEXEC from UNCONFINED - change to rw profile, override regular exec profile, exec profile doesn't have perms
genprofile image=$bin/rw $bin/open:px $file:rw  -- image=$bin/open
do_test "override px" unconfined $bin/rw pass $bin/open $file

#------

# NOTE: test program pauses for the driver script to catch up by sending
# and recieving SIGSTOP/SIGCONT, so the onexec program needs access to
# signals (this is not a script to test signal mediation)

# ONEXEC from CONFINED - don't change profile, open can't exec
genprofile 'change_profile->':$bin/rw $onexec:w signal:ALL
do_test "no px perm" $bin/onexec nochange fail $bin/open $file

# ONEXEC from CONFINED - don't change profile, open is run unconfined
genprofile 'change_profile->':$bin/rw $bin/open:rux $onexec:w signal:ALL
do_test "nochange rux" $bin/onexec nochange pass $bin/open $file

# ONEXEC from CONFINED - don't change profile, open is run confined without necessary perms
genprofile 'change_profile->':$bin/rw $onexec:w signal:ALL -- image=$bin/open $file:rw
do_test "nochange px - no px perm" $bin/onexec nochange fail $bin/open $file

# ONEXEC from CONFINED - don't change profile, open is run confined without necessary perms
genprofile 'change_profile->':$bin/rw $bin/open:rpx $onexec:w signal:ALL -- image=$bin/open
do_test "nochange px - no file perm" $bin/onexec nochange fail $bin/open $file

# ONEXEC from CONFINED - target does NOT exist
genprofile 'change_profile->':$bin/open $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open
do_test "noexist px" $bin/onexec noexist fail $bin/open $file

# ONEXEC from CONFINED - change to rw profile, no exec profile to override
genprofile 'change_profile->':$bin/rw $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix $file:rw
do_test "change profile - override rix" $bin/onexec $bin/rw pass $bin/open $file

# ONEXEC from CONFINED - change to rw profile, no exec profile to override, no explicit access to /proc/*/attr/exec
genprofile 'change_profile->':$bin/rw signal:ALL -- image=$bin/rw $bin/open:rix $file:rw
do_test "change profile - no onexec:w" $bin/onexec $bin/rw pass $bin/open $file

# ONEXEC from CONFINED - don't change profile, make sure exec profile is applied
genprofile 'change_profile->':$bin/rw $onexec:w $bin/open:rpx signal:ALL -- image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open $file:rw
do_test "nochange px" $bin/onexec nochange pass $bin/open $file

# ONEXEC from CONFINED - change to rw profile, override regular exec profile, exec profile doesn't have perms
genprofile 'change_profile->':$bin/rw $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open
do_test "override px" $bin/onexec $bin/rw pass $bin/open $file

# ONEXEC from - change to rw profile, override regular exec profile, exec profile has perms, rw doesn't
genprofile 'change_profile->':$bin/rw $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix  -- image=$bin/open $file:rw
do_test "override px" $bin/onexec $bin/rw fail $bin/open $file

# ONEXEC from COFINED - change to rw profile via glob rule, override exec profile, exec profile doesn't have perms
genprofile 'change_profile->':/** $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open
do_test "glob override px" $bin/onexec $bin/rw pass $bin/open $file

# ONEXEC from COFINED - change to exec profile via glob rule, override exec profile, exec profile doesn't have perms
genprofile 'change_profile->':/** $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open
do_test "glob override px" $bin/onexec $bin/open fail $bin/open $file

# ONEXEC from COFINED - change to exec profile via glob rule, override exec profile, exec profile has perms
genprofile 'change_profile->':/** $onexec:w signal:ALL -- image=$bin/rw $bin/open:rix $file:rw  -- image=$bin/open $file:rw
do_test "glob override px" $bin/onexec $bin/rw pass $bin/open $file

