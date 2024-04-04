#!/bin/bash
# Copyright (C) 2014 Red Hat, Inc. All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# test pool behaviour when volume_list masks activation

. lib/inittest

#
# Main
#
aux have_thin 1 0 0 || skip

aux prepare_vg 2

lvcreate -T -L8M $vg/pool -V10M -n $lv1

aux lvmconf "activation/volume_list = [ \"$vg1\" ]"

# We still could pass - since pool is still active
lvcreate -V10 -n $lv2 -T $vg/pool

# but $lv2 is not active
check inactive $vg $lv2

vgchange -an $vg

# skip $vg from activation
aux lvmconf "activation/volume_list = [ \"$vg1\" ]"

# Pool is not active - so it cannot create thin volume
not lvcreate -V10 -T $vg/pool

aux lvmconf "activation/volume_list = [ \"$vg\" ]"

lvcreate -V10 -T $vg/pool

lvs -o +transaction_id,thin_id $vg

lvremove -ff $vg

check vg_field $vg lv_count "0"

vgremove -ff $vg
