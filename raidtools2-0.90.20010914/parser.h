/*
 * parser.h : Utility for the Linux Multiple Devices driver
 *            Copyright (C) 1997 Ingo Molnar, Miguel de Icaza, Gadi Oxman
 *	      Copyright (C) 1998 Erik Troan
 *
 * Parser externally visible interface.
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 */

extern int parse_config (FILE *fp);

extern char *parity_algorithm_table[];

typedef struct md_raid_info {
	md_array_info_t param;
	md_disk_info_t disks[MD_SB_DISKS];
} md_raid_info_t;

typedef struct md_cfg_entry {
	md_raid_info_t array; 
	char *device_name[MD_SB_DISKS];
	md_u32 sb_block_offset[MD_SB_DISKS];
	char *md_name;
	struct md_cfg_entry *next;
} md_cfg_entry_t;

extern md_cfg_entry_t *cfg_head;
extern md_cfg_entry_t *cfg;
extern int force_flag;
