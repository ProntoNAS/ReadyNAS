
/*
 * detect_multipath.c
 *
 * Copyright (C) 2000, Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * 8/14/01 - Initial version snagged from the scsi_reserve package as
 * 	     the scan_drives.c file and then modified to only do
 * 	     multipath detection.
 */

/*
 * This is a small and simple program that uses the scsi_reserve library
 * and the md library code to detect and create raidtab entries for any
 * multipath devices found on a system.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#if defined(__arm__)
#define BLKGETSIZE _IO(0x12, 96)
#define BLOCK_SIZE      1024
#else
#include <linux/fs.h>
#endif

#include <errno.h>
#include <unistd.h>

#include <popt.h>

#include <sys/types.h>
#include <sys/ioctl.h>

#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>

#include "common.h"
#include "parser.h"
#include "raidlib.h"
#include "raid_io.h"
#include "version.h"
#include "scsi.h"

#define NUM_SCSI_DEVICES	128
#define MAX_PARTITIONS		16

int global_verbose;
int global_superblock;
int global_new;

struct poptOption popt_options[] = {
    {"verbose", 'v', POPT_ARG_NONE, &global_verbose, 0,
     "Print out extra information", NULL},
    {"superblock", 's', POPT_ARG_NONE, &global_superblock, 0,
     "Print out only those devices that already have a valid multipath "
     "raid superblock", NULL},
    {"new", 'n', POPT_ARG_NONE, &global_new, 0,
     "Print out only those devices that don't have a valid multipath "
     "superblock", NULL},
    POPT_AUTOHELP
    {NULL, 0, 0, NULL, 0, NULL, NULL}
};

int
main(int argc, char *argv[])
{
    int fd, i, j, num_devs;
    scsi_sg_dev_t *sg_dev[NUM_SCSI_DEVICES];
    char *buffer[NUM_SCSI_DEVICES];
    poptContext context;

    context = poptGetContext(argv[0], argc, (const char **)argv,
			     (const struct poptOption *)&popt_options, 0);

    i = poptGetNextOpt(context);
    if (i < -1) {
	poptPrintHelp(context, stderr, 0);
	exit(1);
    }

    if (global_superblock && global_new) {
	fprintf(stderr, "Only one of the -n or -s options are allowed\n");
	exit(1);
    }

    if(prepare_raidlib())
	exit(1);

    for (i = 0; i < NUM_SCSI_DEVICES; i++) {
	sg_dev[i] = NULL;
	buffer[i] = NULL;
    }

    i = 0;
    j = 0;
    do {
	if ((buffer[j] == NULL) && ((buffer[j] = malloc(128)) == NULL)) {
	    fprintf(stderr, "Unable to allocate memory, exiting.\n");
	    exit(1);
	}

	if (i < 26)
	    sprintf(buffer[j], "/dev/sd%c", i + 'a');
	else
	    sprintf(buffer[j], "/dev/sd%c%c", i / 26 + 'a' - 1,
		    i % 26 + 'a');

	fd = open(buffer[j], O_RDONLY | O_NDELAY);
	if (fd != -1) {
	    sg_dev[j] = scsi_init_sg_device(fd, buffer[j]);
	    if (sg_dev[j] && sg_dev[j]->initialized)
		j++;
	}
    } while (++i < NUM_SCSI_DEVICES);

    num_devs = j;

    if (global_verbose)
	printf("\nChecking for multipath drives...(%d devices)\n",
	       num_devs);

    for (i = 0; i < num_devs - 1; i++) {
	int multipath_array[NUM_SCSI_DEVICES], k, l;

	k = 1;
	multipath_array[0] = i;
	multipath_array[k] = -1;
	for (j = i + 1; j < num_devs; j++) {
	    int a0, a1, b0, b1;

	    if (!sg_dev[i]->initialized || !sg_dev[j]->initialized)
		continue;

	    a0 = sg_dev[i]->id_eui64[0];
	    a1 = sg_dev[i]->id_eui64[1];
	    b0 = sg_dev[j]->id_eui64[0];
	    b1 = sg_dev[j]->id_eui64[1];
	    if ((a0 || a1) && (b0 || b1) && ((a0 == b0) && (a1 == b1))) {
		if (global_verbose)
		    printf("%s and %s are multiple paths to the same "
			   "device (id_eui64)\n", sg_dev[i]->name,
			   sg_dev[j]->name);
		multipath_array[k++] = j;
		continue;
	    } else if ((a0 || a1 || b0 || b1) &&
		       ((a0 != b0) || (a1 != b1))) {
		continue;
	    }

	    a0 = sg_dev[i]->id_fcph[0];
	    a1 = sg_dev[i]->id_fcph[1];
	    b0 = sg_dev[j]->id_fcph[0];
	    b1 = sg_dev[j]->id_fcph[1];
	    if ((a0 || a1) && (b0 || b1) && ((a0 == b0) && (a1 == b1))) {
		if (global_verbose)
		    printf("%s and %s are multiple paths to the same "
			   "device (id_fcph)\n", sg_dev[i]->name,
			   sg_dev[j]->name);
		multipath_array[k++] = j;
		continue;
	    } else if ((a0 || a1 || b0 || b1) &&
		       ((a0 != b0) || (a1 != b1))) {
		continue;
	    }
	    if ((sg_dev[i]->scsi_dev_id == sg_dev[j]->scsi_dev_id) &&
		(sg_dev[i]->scsi_dev_lun == sg_dev[j]->scsi_dev_lun) &&
		!strcmp(sg_dev[i]->vendor, sg_dev[j]->vendor) &&
		!strcmp(sg_dev[i]->product, sg_dev[j]->product) &&
		!strcmp(sg_dev[i]->revision, sg_dev[j]->revision)) {
		if (sg_dev[i]->serial_number && sg_dev[j]->serial_number &&
		    !strcmp(sg_dev[i]->serial_number,
			    sg_dev[j]->serial_number)) {
		    if (global_verbose)
			printf("%s and %s are multiple paths to the same "
			       "device (serial_number)\n", sg_dev[i]->name,
			       sg_dev[j]->name);
		    multipath_array[k++] = j;
		    continue;
		} else if (sg_dev[i]->serial_number != NULL ||
			   sg_dev[j]->serial_number != NULL) {
		    continue;
		}
		if (sg_dev[i]->id_vendor && sg_dev[j]->id_vendor &&
		    !strcmp(sg_dev[i]->id_vendor, sg_dev[j]->id_vendor)) {
		    if (global_verbose)
			printf("%s and %s are multiple paths to the same "
			       "device (id_vendor)\n", sg_dev[i]->name,
			       sg_dev[j]->name);
		    multipath_array[k++] = j;
		    continue;
		} else if (sg_dev[i]->id_vendor != NULL ||
			   sg_dev[j]->id_vendor != NULL) {
		    continue;
		}
		if (sg_dev[i]->id_nonunique && sg_dev[j]->id_nonunique &&
		    !strcmp(sg_dev[i]->id_nonunique,
			    sg_dev[j]->id_nonunique)) {
		    if (global_verbose)
			printf("%s and %s are multiple paths to the same "
			       "device (id_nonunique)\n", sg_dev[i]->name,
			       sg_dev[j]->name);
		    multipath_array[k++] = j;
		    continue;
		} else if (sg_dev[i]->id_vendor != NULL ||
			   sg_dev[j]->id_vendor != NULL) {
		    continue;
		}
	    }
	}

	if (k > 1) {
	    int has_superblock;
	    unsigned long nr_blocks;
	    unsigned long sb_offset;
	    unsigned char sb_buf[MD_SB_BYTES];
	    md_superblock_t *sb;

	    sb = (md_superblock_t *)&sb_buf[0];

	    if (ioctl(sg_dev[i]->disk_fd, BLKGETSIZE, &nr_blocks) == -1) {
		perror("ioctl BLKGETSIZE");
		fprintf(stderr, "Error getting size for %s\n",
			sg_dev[i]->name);
		continue;
	    }

	    if (nr_blocks < MD_RESERVED_BLOCKS) {
		fprintf(stderr, "Device %s is too small to be a raid device.\n",
			sg_dev[i]->name);
		continue;
	    }

	    nr_blocks >>= 1; /* convert to 1K blocks */

	    sb_offset = MD_NEW_SIZE_BLOCKS(nr_blocks);
	    if (raidseek(sg_dev[i]->disk_fd, sb_offset) == -1) {
		perror("raidseek");
		fprintf(stderr, "Unable to seek to superblock offset of %s\n",
			sg_dev[i]->name);
		continue;
	    }

	    if (read(sg_dev[i]->disk_fd, sb_buf, MD_SB_BYTES) != MD_SB_BYTES) {
		perror("read");
		fprintf(stderr, "Short read for superblock from %s\n",
			sg_dev[i]->name);
		continue;
	    }

	    if (sb->md_magic == MD_SB_MAGIC && sb->level == MULTIPATH)
		has_superblock = 1;
	    else
		has_superblock = 0;

	    if (!(global_superblock && !has_superblock) &&
		!(global_new && has_superblock)) {
	    	if (k == 2) {
		    printf ("%s and %s are multiple paths to ", sg_dev[i]->name,
			    sg_dev[multipath_array[1]]->name);
		    if(has_superblock)
			printf("/dev/md%d\n", sb->md_minor);
		    else
			printf("the same device\n");
		    /* mark it not-initialized, then we won't scan it again */
		    sg_dev[multipath_array[1]]->initialized = 0;
		} else {
		    printf("%s, ", sg_dev[i]->name);
		    for (j = 1; j < (k - 1); j++) {
			printf("%s, ", sg_dev[multipath_array[j]]->name);
			sg_dev[multipath_array[j]]->initialized = 0;
		    }
		    printf("and %s are multiple paths to ",
			   sg_dev[multipath_array[j]]->name);
		    if(has_superblock)
			printf("/dev/md%d\n", sb->md_minor);
		    else
			printf("the same device\n");
		    sg_dev[multipath_array[j]]->initialized = 0;
		}
	    }
	    /*
	     * Now process partitions on the main array device to see if
	     * any of them have superblocks.
	     */
	    for(l=1; l<MAX_PARTITIONS; l++) {
		int fd;
		char name[256];

		sprintf(name,"%s%d",sg_dev[i]->name,l);
		if((fd = open(name,O_RDONLY | O_NDELAY)) == -1)
		    continue;

		if (global_verbose)
		    printf("Checking for a superblock on %s\n", name);

		if (ioctl(fd, BLKGETSIZE, &nr_blocks) == -1) {
		    perror("ioctl BLKGETSIZE");
		    fprintf(stderr, "Error getting size for %s\n", name);
		    continue;
		}

		if (nr_blocks < MD_RESERVED_BLOCKS) {
		    fprintf(stderr, "Device %s is too small to be a raid "
				    "device.\n", name);
		    continue;
		}

		nr_blocks >>= 1; /* convert to 1K blocks */

		sb_offset = MD_NEW_SIZE_BLOCKS(nr_blocks);
		if (raidseek(fd, sb_offset) == -1) {
		    perror("raidseek");
		    fprintf(stderr, "Unable to seek to superblock offset of "
				    "%s\n", name);
		    continue;
		}

		if (read(fd, sb_buf, MD_SB_BYTES) != MD_SB_BYTES) {
		    perror("read");
		    fprintf(stderr, "Short read for superblock from %s\n",
			    name);
		    continue;
		}

		/* When creating raid arrays, the MULTIPATH personality is
		 * enumerated as 7.  However, when parser.c actually puts
		 * the level into the param block for being written out to
		 * the real superblock, it uses -4 for MULTIPATH.  So,
		 * check that the sb->level == -4 here
		 */
		if (sb->md_magic == MD_SB_MAGIC && sb->level == -4) {
		    has_superblock = 1;
		} else {
		    has_superblock = 0;
		}

	        if (has_superblock && !global_new) {
		    printf("/dev/md%d = ", sb->md_minor);
		    printf("%s, ", name);
		    for (j = 1; j < (k - 1); j++) {
			printf("%s%d, ", sg_dev[multipath_array[j]]->name, l);
		    }
		    printf("%s%d\n", sg_dev[multipath_array[j]]->name, l);
		}
	    }
	}
    }
    if (global_verbose)
	printf("\n");

    exit(0);
}
