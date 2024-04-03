/*
 * mkpv.c : LVM Utility for the Linux Logical Volume Manger driver
 *          Copyright (C) 1998 Ingo Molnar
 *
 * This utility creates a Physical Volume
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 *
 * 2001-12-21: /proc/mount checking, Darik Horn <dajhorn at uwaterloo dot ca>
 *
 */

#include "common.h"
#include "parser.h"
#include "raid_io.h"
#include "raidlib.h"
#include "version.h"
#include <popt.h>
#include <sys/mount.h>		/* for BLKGETSIZE */
#ifndef BLKGETSIZE
#include <linux/fs.h>          /* for BLKGETSIZE */
#endif
#include <sys/sysmacros.h>

#include "lvm-int.h"

#define MKPV_MAJOR_VERSION            (0)
#define MKPV_MINOR_VERSION            (90)
#define MKPV_PATCHLEVEL_VERSION       (0)

void usage (void) {
    printf("usage: mkpv [--configfile] [--version] [--force]\n");
    printf("       [-acfhuv] </dev/md?>*\n");
}

static long long lvmseek (int fd, unsigned long block)
{
	return (raidseek(fd, block*(LVM_BLOCKSIZE/MD_BLK_SIZ)));
}

static int mkvg (int fd, int forceSanity)
{
	unsigned char buffer[LVM_BLOCKSIZE];
	vg_sb_t *vg_sb;
	lv_descriptor_t *lv;

	if (lvmseek(fd, 1) == -1)
		return 1;
	if ((read(fd, buffer, LVM_BLOCKSIZE)) != LVM_BLOCKSIZE)
		return 1;
	vg_sb = (vg_sb_t *) buffer;

	if (!forceSanity) {
		/*
		 * take a look at the superblock ...
		 */
		if (vg_sb->vg_magic == LVM_VG_SB_MAGIC) {
			fprintf(stderr, "VG appears to be already created -- use -f to\nforce the destruction of old contents\n");
			return 1;
		}
	}

	memset(buffer,0,LVM_BLOCKSIZE);

	printf("creating VG ...\n");
	vg_sb->vg_magic = LVM_VG_SB_MAGIC;

	vg_sb->nr_lvs = 1;

	printf("creating LV 1 ... \n");

	lv = vg_sb->lv_array + 1;
	lv->lv_id = 1;
	lv->lv_max_indices = 20000;
	lv->lv_free_indices = lv->lv_max_indices;
	lv->md_id = 9;
	
	if (lvmseek(fd, 1) == -1)
		return 1;
	if ((write(fd, buffer, LVM_BLOCKSIZE)) != LVM_BLOCKSIZE)
		return 1;

	return 0;
}

static int mkpv (char *name, int forceSanity)
{
	FILE *fp;
	unsigned char buffer[LVM_BLOCKSIZE];
	pv_sb_t *pv_sb;
	int fd, i, block_groups, bg_size;
	unsigned long nr_blocks;
	pv_block_group_t *pv_bg;
	struct mntent * mntinfo;
	int err;

	fd = open(name, O_RDWR);
	if (fd == -1) {
		perror("make PV: ");
		return 1;
	}

	/*
	 * Check if the device is mounted
	 */
	if (!(fp = setmntent("/etc/mtab", "r")) && !(fp = setmntent("/proc/mounts", "r"))) {
		fprintf(stderr, "mkpv: unable to open both /etc/mtab and /proc/mounts\n");
		return 1;
	}
	
	err = 0;
	while ((mntinfo = getmntent(fp))) {
	    if (!strcmp(mntinfo->mnt_fsname, name)) {
		    fprintf(stderr, "%s is mounted\n", name);
		    err = 1;
	    }
	}

	endmntent(fp);
	if (err) return 1;

/* FIXME: implement this ... */
#if 0
	if (!forceSanity) {
		/*
		 * Check if the device contains an ext2 filesystem
		 * ... tough with an LVM!
		 */
		if ((lvmseek(fd, 1)) == -1)
			return 1;
		if ((read(fd, buffer, LVM_BLOCKSIZE)) != LVM_BLOCKSIZE)
			return 1;
		if (buffer[56] == 0x53 && buffer[57] == 0xef && buffer[33] == 0x20 && buffer[37] == 0x20) {
			fprintf(stderr, "%s appears to contain an ext2 filesystem -- use -f to override\n", name);
			return 1;
		}
	}
#endif
	if (lvmseek(fd, 0) == -1)
		return 1;
	if ((read(fd, buffer, LVM_BLOCKSIZE)) != LVM_BLOCKSIZE)
		return 1;
	pv_sb = (pv_sb_t *) buffer;

	if (!forceSanity) {
		/*
		 * take a look at the superblock ...
		 */
		if (pv_sb->pv_magic == LVM_PV_SB_MAGIC) {
			fprintf(stderr, "PV %s appears to be already created -- use -f to\nforce the destruction of old contents\n", name);
			return 1;
		}
	}

	memset(buffer,0,LVM_BLOCKSIZE);
	memset(buffer,0x7f,(LVM_PV_SB_GENERIC_WORDS-17)*4);

	printf("creating PV ... not fully yet :)\n");
	pv_sb->pv_magic = LVM_PV_SB_MAGIC;

	if (ioctl(fd, BLKGETSIZE, (unsigned long)&nr_blocks) == -1) {
		fprintf(stderr, "couldn't get device size for %s -- %s\n", name, strerror(errno));
		close(fd);
		return 1;
	}
	nr_blocks /= LVM_BLOCKSIZE/512;

	printf("%s's size: %ld KB.\n", name, nr_blocks*LVM_BLOCKSIZE/1024);

	bg_size = TOTAL_BLOCKS_PER_BG;
	pv_sb->pv_bg_size = bg_size;

	 /* 1 left for the superblock */
	nr_blocks--;

	/*
	 * Rounding to block groups, no partial block groups allowed.
	 */
	nr_blocks = (nr_blocks / pv_sb->pv_bg_size) * pv_sb->pv_bg_size;
	printf("%s's rounded size: %ld KB.\n",
				name, nr_blocks*LVM_BLOCKSIZE/1024);

	pv_sb->pv_total_size = nr_blocks;
	block_groups = nr_blocks / pv_sb->pv_bg_size;
	pv_sb->pv_block_groups = block_groups;

	pv_sb->pv_first_free = 1;
	pv_sb->pv_first_used = 0;
	pv_sb->pv_blocks_left = nr_blocks;
	pv_sb->pv_block_size = LVM_BLOCKSIZE;
	pv_sb->pv_pptr_size = sizeof(pv_pptr_t);

	pv_sb->pv_uuid0 = get_random();
	pv_sb->pv_uuid1 = get_random();
	pv_sb->pv_uuid2 = get_random();
	pv_sb->pv_uuid3 = get_random();

	pv_sb->pv_ctime = (md_u32) time(NULL);

        pv_sb->pv_major = MKPV_MAJOR_VERSION;
        pv_sb->pv_minor = MKPV_MINOR_VERSION;
        pv_sb->pv_patch = MKPV_PATCHLEVEL_VERSION;

	/*
	 * Check wether any field was left uninitialized, accidentally:
	 */
	{
		unsigned int * buf = (unsigned int *) buffer;

		for (i = 0; i < (LVM_PV_SB_GENERIC_WORDS-17); i++) {
			if (buf[i] == 0x7f7f7f7f) {
				printf("PV sb word %d uninitialized!\n", i);
				return 1;
			}
		}
	}

	if (lvmseek(fd, 0) == -1)
		return 1;
	if ((write(fd, buffer, LVM_BLOCKSIZE)) != LVM_BLOCKSIZE)
		return 1;

	mkvg(fd, forceSanity);

	/*
	 * Initialize all the block groups:
	 */
	pv_bg = (pv_block_group_t *) buffer;

	memset(pv_bg, 0, LVM_BLOCKSIZE);

	printf("initializing block groups on %s:\n", name);
	for (i = 0; i < block_groups; i++) {
		if (lvmseek(fd, i*bg_size + 2) == -1)
			return 1;
		if ((write(fd, buffer, LVM_BLOCKSIZE)) != LVM_BLOCKSIZE)
			return 1;
		if (!(i&31))
			sync();
		printf("#%d\r", i); fflush(stdout);
	}

	return 0;
}

int main (int argc, char *argv[])
{
    FILE *fp = NULL;
    int exit_status=0;
    int version = 0, help = 0;
    char * configFile = RAID_CONFIG;
    int force_flag = 0;
    char ** args;
    struct md_version ver;
    poptContext optCon;
    int i;
    struct poptOption optionsTable[] = {
	{ "configfile", 'c', POPT_ARG_STRING, &configFile, 0 },
	{ "force", 'f', 0, &force_flag, 0 },
	{ "help", 'h', 0, &help, 0 },
	{ "version", 'V', 0, &version, 0 },
	{ NULL, 0, 0, NULL, 0 }
    } ;

    optCon = poptGetContext("mkpv", argc, (const char **)argv, optionsTable, 0);
    if ((i = poptGetNextOpt(optCon)) < -1) {
	fprintf(stderr, "%s: %s\n", 
		poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
		poptStrerror(i));
	usage();
	return EXIT_FAILURE;
    }

    if (help) {
	usage();
	return EXIT_FAILURE;
    } else if (version) {
	printf("mkpv version %d.%d.%d\n", MKPV_MAJOR_VERSION,
			MKPV_MINOR_VERSION, MKPV_PATCHLEVEL_VERSION);
	return EXIT_VERSION;
    }

    fp = fopen(configFile, "r");
    if (fp == NULL) {
	fprintf(stderr, "Couldn't open %s -- %s\n", configFile, 
	        strerror(errno));
	goto abort;
    }

    srand((unsigned int) time(NULL));
    if (parse_config(fp))
	goto abort;

    args = (char **)poptGetArgs(optCon);
    if (!args) {
	fprintf(stderr, "nothing to do!\n");
	usage();
	return EXIT_FAILURE;
    }

    if (getMdVersion(&ver)) {
	fprintf(stderr, "cannot determine md version: %s\n", strerror(errno));
	return EXIT_FAILURE;
    }

    while (*args) {
	if (force_flag) {
		fprintf(stderr, 
		"DESTROYING the contents of %s in 5 seconds, Ctrl-C if unsure!\n", *args);
		sleep(5);
	}
	if (mkpv(*args, force_flag))
		goto abort;
	args++;
    }

    fclose(fp);
    return 0;

abort:
    fprintf(stderr, "mkpv: aborted\n");
    exit_status = 1;
    if (fp)
	fclose(fp);
    return exit_status;
}


