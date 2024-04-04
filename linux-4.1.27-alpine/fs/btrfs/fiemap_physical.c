/*
 * Copyright (C) 2014 NETGEAR, Inc.  All rights reserved.
 * Copyright (C) 2014 Hiro Sugawara  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#include <linux/types.h>
#include <linux/blkdev.h>
#include <linux/kdev_t.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include "ctree.h"
#include "disk-io.h"
#include "btrfs_inode.h"
#include "volumes.h"

static int fiemap_fill_next_extent_physical(struct fiemap_extent_info *fieinfo,
			u64 fpos, u64 llength, u64 phys, u64 len, u32 flags,
			dev_t dev)
{
	struct fiemap_extent __user *extent =
			fieinfo->fi_extents_start + fieinfo->fi_extents_mapped;
	typeof(extent->fe_pdev) pdev = new_encode_dev(dev);
	typeof(extent->fe_loglength) llen = llength;

	int ret = fiemap_fill_next_extent(fieinfo, fpos, phys, len,
		flags | FIEMAP_EXTENT_PHYSICAL | FIEMAP_EXTENT_LOGLENGTH);

	if (ret < 0)
		return ret;
	if (copy_to_user(&extent->fe_pdev, &pdev, sizeof(pdev)))
		return -EFAULT;
	if (copy_to_user(&extent->fe_loglength, &llen, sizeof(llen)))
		return -EFAULT;
	return ret;
}

int extent_fiemap_log2phys(struct inode *inode,
				struct fiemap_extent_info *fieinfo,
				u64 foffset, u64 logical, u64 len, u32 flags)
{
	struct btrfs_fs_info *fs_info = BTRFS_I(inode)->root->fs_info;
	struct btrfs_bio *bbio;
	int ret, i;
	u64 length = len;

	if ((ret = btrfs_map_block(fs_info, REQ_DISCARD, logical, &length, &bbio, 0)))
		return ret;
	for (i = 0; i < bbio->num_stripes; i++) {
		if ((ret = fiemap_fill_next_extent_physical(fieinfo, foffset,
				length,
				bbio->stripes[i].physical,
				bbio->stripes[i].length,
				(bbio->num_stripes > 1) &&
					(i < bbio->num_stripes - 1)
						? FIEMAP_EXTENT_MORESTRIPE
						: 0,
				bbio->stripes[i].dev->bdev->bd_dev)))
			break;
	}
	kfree(bbio);
	return ret;
}
