#!/bin/sh

# Copyright (C) 2012 Red Hat, Inc. All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

. lib/inittest

prepare_lvs() {
	lvremove -f $vg
	lvcreate -L10M -n $lv1 $vg
	lvcreate -L8M -n $lv2 $vg
}

#
# Main
#
aux have_thin 1 0 0 || skip

aux prepare_pvs 4 64

# build one large PV
vgcreate $vg1 $(head -n 3 DEVICES)
# 32bit linux kernels are fragille with device size >= 16T
# maybe  uname -m    [ x86_64 | i686 ]
TSIZE=64T
aux can_use_16T || TSIZE=15T
lvcreate -s -l 100%FREE -n $lv $vg1 --virtualsize $TSIZE
aux extend_filter_LVMTEST

pvcreate "$DM_DEV_DIR/$vg1/$lv"
vgcreate $vg -s 64K $(tail -n+4 DEVICES) "$DM_DEV_DIR/$vg1/$lv"

lvcreate -L1T -n $lv1 $vg
invalid lvconvert --yes -c 8M --type thin --poolmetadatasize 1G $vg/$lv1

# needs some --cachepool or --thinpool
invalid lvconvert --yes --poolmetadatasize 1G $vg/$lv1
lvremove -f $vg

# create mirrored LVs for data and metadata volumes
lvcreate -aey -L10M --type mirror -m1 --mirrorlog core -n $lv1 $vg
lvcreate -aey -L10M -n $lv2 $vg
lvchange -an $vg/$lv1

# conversion fails for mirror segment type
fail lvconvert --thinpool $vg/$lv1
# cannot use same LV
fail lvconvert --yes --thinpool $vg/$lv2 --poolmetadata $vg/$lv2

prepare_lvs

# conversion fails for internal volumes
# can't use --readahead with --poolmetadata
invalid lvconvert --thinpool $vg/$lv1 --poolmetadata $vg/$lv2 --readahead 512
lvconvert --yes --thinpool $vg/$lv1 --poolmetadata $vg/$lv2

prepare_lvs
lvconvert --yes -c 64 --stripes 2 --thinpool $vg/$lv1 --readahead 48

lvremove -f $vg
lvcreate -L1T -n $lv1 $vg
lvconvert --yes -c 8M --type thin-pool $vg/$lv1

lvremove -f $vg
# test with bigger sizes
lvcreate -L1T -n $lv1 $vg
lvcreate -L8M -n $lv2 $vg
lvcreate -L1M -n $lv3 $vg

# chunk size is bigger then size of thin pool data
fail lvconvert --yes -c 1G --thinpool $vg/$lv3
# stripes can't be used with poolmetadata
invalid lvconvert --stripes 2 --thinpool $vg/$lv1 --poolmetadata $vg/$lv2
# too small metadata (<2M)
fail lvconvert --yes -c 64 --thinpool $vg/$lv1 --poolmetadata $vg/$lv3
# too small chunk size fails
# 'fail' because profiles need to read VG
fail lvconvert -c 4 --thinpool $vg/$lv1 --poolmetadata $vg/$lv2
# too big chunk size fails
fail lvconvert -c 2G --thinpool $vg/$lv1 --poolmetadata $vg/$lv2
# negative chunk size fails
invalid lvconvert -c -256 --thinpool $vg/$lv1 --poolmetadata $vg/$lv2
# non power of 2 fails
fail lvconvert -c 88 --thinpool $vg/$lv1 --poolmetadata $vg/$lv2

# Warning about smaller then suggested
lvconvert --yes -c 256 --thinpool $vg/$lv1 --poolmetadata $vg/$lv2 |& tee err
grep "WARNING: Chunk size is smaller" err

lvremove -f $vg
lvcreate -L1T -n $lv1 $vg
lvcreate -L32G -n $lv2 $vg
# Warning about bigger then needed
lvconvert --yes --thinpool $vg/$lv1 --poolmetadata $vg/$lv2 |& tee err
grep "WARNING: Maximum" err

lvremove -f $vg

if test "$TSIZE" = 64T; then
lvcreate -L24T -n $lv1 $vg
# Warning about bigger then needed (24T data and 16G -> 128K chunk)
lvconvert --yes -c 64 --thinpool $vg/$lv1 |& tee err
grep "WARNING: Chunk size is too small" err
fi

#lvs -a -o+chunk_size,stripe_size,seg_pe_ranges

# Convertions of pool to mirror or RAID is unsupported
fail lvconvert --type mirror -m1 $vg/$lv1
fail lvconvert --type raid1 -m1 $vg/$lv1

vgremove -ff $vg
