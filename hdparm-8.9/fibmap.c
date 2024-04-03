/*
 * Based upon original code by Ric Wheeler and Mark Lord.
 *
 * Copyright (c) EMC Corporation 2008
 * Copyright (c) Mark Lord 2008
 *
 * You may use/distribute this freely, under the terms of either
 * (your choice) the GNU General Public License version 2,
 * or a BSD style license.
 */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/fs.h>

#include "hdparm.h"

struct file_extent {
	__u64 byte_offset;
	__u64 first_block;
	__u64 last_block;
	__u64 block_count;
};

static int
handle_extent(struct file_extent ext, unsigned int sectors_per_block, __u64 start_lba, __u64 target_sect, __u64 *target_lba)
{
	char lba_info[64];
	__u64 begin_lba, end_lba;
	__u64 nsectors = ext.block_count * sectors_per_block;

	if (ext.first_block) {
		begin_lba = start_lba + (ext.first_block * sectors_per_block);
		end_lba   = start_lba + (ext.last_block * sectors_per_block) + sectors_per_block - 1;
	} else {
		begin_lba = end_lba = 0;
	}

	if (target_lba) {
		if (ext.first_block) {
			__u64 begin_sect = ext.byte_offset / 512;
			__u64 end_sect   = begin_sect + nsectors;
			if (target_sect >= begin_sect && target_sect < end_sect) {
				*target_lba = begin_lba + (target_sect - begin_sect);
				return 1;
			}
		}
		return 0;
	}

	if (ext.first_block)
		sprintf(lba_info, "%10llu %10llu", begin_lba, end_lba);
	else
		strcpy(lba_info, "      -          -   ");
	printf("%12llu %s %10llu\n", ext.byte_offset, lba_info, nsectors);
	return 0;
}

int do_fibmap(const char *file_name, __u64 target_sect, __u64 *target_lba)
{
	int fd, err;
	struct stat st;
	__u64 start_lba = 0, blk_idx, hole = ~0ULL;
	unsigned int sectors_per_block;
	unsigned long num_blocks;
	struct file_extent ext;

	if ((fd = open(file_name, O_RDONLY)) == -1 || fstat(fd, &st) == -1) {
		err = errno;
		perror(file_name);
		return err;
	}

	if (!S_ISREG(st.st_mode)) {
		fprintf(stderr, "%s: not a regular file\n", file_name);
		close(fd);
		return EINVAL;
	}
#if 0
	if (st.st_size == 0) {
	    	fprintf(stderr, "%s: empty file\n", file_name);
		close(fd);
		return EINVAL;
	}
#endif
	/*
	 * Get the filesystem starting LBA:
	 */
	err = get_dev_t_geometry(st.st_dev, NULL, NULL, NULL, &start_lba, NULL);
	if (err)
		return err;

	sectors_per_block = st.st_blksize / 512;
	if (!target_lba) {
		printf("\n%s: underlying filesystem begins at LBA %llu; assuming 512 byte sectors.\n", file_name, start_lba);
		printf("%12s %10s %10s %10s\n", "byte_offset", "begin_LBA", "end_LBA", "sectors");
	}

	/*
	 * How many calls to FIBMAP do we need?
	 * FIBMAP returns a filesystem block number (counted from the start of the device)
	 * for each file block.  This can be converted to a disk LBA using the filesystem
	 * blocksize and LBA offset obtained earlier.
	 */
	num_blocks = (st.st_size + st.st_blksize - 1) / st.st_blksize;
	memset(&ext, 0, sizeof(ext));

	/*
	 * Loop through the file, building a map of the extents.
	 * All of this is done in filesystem blocks size (fs_blksize) units.
	 *
	 * Assumptions:
	 * Throughout the file, there can be any number of blocks backed by holes
	 * or by allocated blocks.  Tail-packed files are special - if we find a file
	 * that has a size and has no allocated blocks, we can flag it as a "tail-packed"
	 * file if we care: data is packed into the tail space of the inode block.
	 */
	for (blk_idx = 0; blk_idx < num_blocks; blk_idx++) {
		unsigned long blknum = blk_idx;
		__u64 fs_blknum;
		/*
		 * FIBMAP takes a block index as input and on return replaces it with a
		 * block number relative to the beginning of the filesystem/partition.
		 * An output value of zero means "unallocated", or a "hole" in a sparse file.
		 * Note that this is a 32-bit value on 32-bit systems, so it will not work
		 * properly on files/filesystems with more than 4 billion blocks (~16TB),
		 */
		if (ioctl(fd, FIBMAP, &blknum) == -1) {
			err = errno;
			perror("ioctl(FIBMAP)");
			close(fd);
			return err;
		}
		fs_blknum = blknum;	/* work in 64-bits as much as possible */

		if (blk_idx && fs_blknum == (ext.last_block + 1)) {
			/*
			 * Continuation of extent: Bump last_block and block_count.
			 */
			ext.last_block = fs_blknum ? fs_blknum : hole;
			ext.block_count++;
		} else {
			/*
			 * New extent: print previous extent (if any), and re-init the extent record.
			 */
			if (blk_idx && handle_extent(ext, sectors_per_block, start_lba, target_sect, target_lba))
				goto done;
			ext.first_block = fs_blknum;
			ext.last_block  = fs_blknum ? fs_blknum : hole;
			ext.block_count = 1;
			ext.byte_offset = blk_idx * st.st_blksize;
		}
	}
	if (!handle_extent(ext, sectors_per_block, start_lba, target_sect, target_lba)) {
		close(fd);
		return EBADSLT;
	}
#if 0
	if (no_non_hole_extents_found) {
		/*
		 * If we don't count any allocated blocks in a non zero-length file,
		 * then assume that it is tail-packed.
		 */
		if (!target_lba)
	    		printf("%s: TAIL PACKED (no allocated disk blocks)\n", file_name);
	}
#endif
done:
	close (fd);
	return 0;
}
