/*
 * raid_io.h : Utility for the Linux Multiple Devices driver
 *             Copyright (C) 1997 Ingo Molnar, Miguel de Icaza, Gadi Oxman
 *	       Copyright (C) 1998 Erik Troan
 *
 * Externaly visible RAID IO operations interface.
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 */

#include "common.h"
#include "raidlib.h"

void progress (unsigned long blocks, unsigned long current);

long long raidseek (unsigned int fd, unsigned long blk);
extern void print_sb (md_superblock_t *sb);
int analyze_sb (struct md_version * ver, enum mkraidFunc func, 
	md_cfg_entry_t * cfg, int forceSanity,
	int upgradeArray, int forceResync);
extern int init_set (md_cfg_entry_t * p);
extern int check_active (md_cfg_entry_t *p);
extern int read_sb (md_cfg_entry_t *p);
int write_sb (md_cfg_entry_t * p, int old);


