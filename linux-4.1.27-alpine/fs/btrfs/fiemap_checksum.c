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

#define BTRFS_CSUM_LEN	4096

int extent_fiemap_ex2csum(struct inode *inode,
				struct fiemap_extent_info *fieinfo,
				u64 foffset, u64 logical, u64 length, u32 flags)
{
	struct btrfs_fs_info *fs_info = BTRFS_I(inode)->root->fs_info;
	struct btrfs_root *root = fs_info->csum_root;
	int ret;
	LIST_HEAD(csum_list);

	if ((ret = btrfs_lookup_csums_range(root, logical, logical + length - 1,
						&csum_list, 0)))
		return ret;

	while (!list_empty(&csum_list)) {
		struct btrfs_ordered_sum *sum = list_first_entry(&csum_list,
						struct btrfs_ordered_sum, list);

		if (!ret) {
			int len = sum->len;
			u32 csum;

			while (!ret && len > 0) {
				struct fiemap_extent __user *extent =
					fieinfo->fi_extents_start +
						fieinfo->fi_extents_mapped;
				int index = ((u32)(logical - sum->bytenr))/
						root->sectorsize;
				memcpy(&csum, sum->sums + index, sizeof csum);
				ret = fiemap_fill_next_extent(fieinfo, foffset,
					logical,
					min_t(size_t, BTRFS_CSUM_LEN, len),
					FIEMAP_EXTENT_CHECKSUM |
						FIEMAP_EXTENT_LOGLENGTH);

				if (ret >= 0 &&
					copy_to_user(&extent->fe_checksum,
							&csum, sizeof(csum)))
					ret = -EFAULT;

				len -= BTRFS_CSUM_LEN;
				logical += BTRFS_CSUM_LEN;
				foffset += BTRFS_CSUM_LEN;
			}
		}
		list_del(&sum->list);
		kfree(sum);
	}

	return ret;
}
