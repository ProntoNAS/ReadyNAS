/*
 * raidlib.h : Utility for the Linux Multiple Devices driver
 *            Copyright (C) 1998 Erik Troan, Ingo Molnar
 *
 * Common includes and declarations.
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 */

#ifndef H_RAIDLIB
#define H_RAIDLIB

extern int do_quiet_flag;

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define OUT(x...) do { if (!do_quiet_flag) fprintf(stderr,##x); } while (0)
#define ERR(x...) fprintf(stderr,##x)

extern struct md_version md_ver;
enum mkraidFunc {mkraid, raid0run };
enum raidFunc {raidstart, raidrun, raidstop, raidsetfaulty,
			raidstop_ro, raidhotremove, raidhotadd, raidhotgenerateerror};
extern md_cfg_entry_t *cfg_head, *cfg;
extern int do_quiet_flag;

extern md_u32 get_random(void);
extern int handleOneConfig(enum raidFunc func, md_cfg_entry_t * cfg);
extern int prepare_raidlib (void);
int open_or_die (char *file);
int do_raidstop (int fd, char *dev, int ro);
int do_raidstart_rw (int fd, char *dev);
int do_raidhotremove (int md_fd, char * disk_name, char *md_name);
int do_raidhotadd (int md_fd, char * disk_name, char *md_name);
int do_raidhotgenerateerror (int md_fd, char * disk_name, char *md_name);
int do_raidsetfaulty (int md_fd, char * disk_name, char * md_name);

#endif
