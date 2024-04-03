/*
 * common.h : Utility for the Linux Multiple Devices driver
 *            Copyright (C) 1997 Ingo Molnar, Miguel de Icaza, Gadi Oxman
 *            Copyright (C) 1998 Erik Troan
 *
 * Common includes and declarations.
 *
 * This source is covered by the GNU GPL, the same as all Linux kernel
 * sources.
 */

#ifndef H_COMMON
#define H_COMMON

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <mntent.h>

#include "md-int.h"

typedef int kdev_t;
#include <linux/unistd.h>

#define DEBUG				(0)
#define EXIT_USAGE			(EXIT_FAILURE)
#define EXIT_VERSION			(0)
#define MAX_LINE_LENGTH			(100)
#define ZERO_BUFFER_SIZE		(64)	/* 64kB */
#define MD_BLK_SIZ			(0x400)
#define RAID_CONFIG			"/etc/raidtab"

#define MKRAID_MAJOR_VERSION            (0)
#define MKRAID_MINOR_VERSION            (90)
#define MKRAID_PATCHLEVEL_VERSION       (0)

extern int do_quiet_flag;

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define OUT(x...) do { if (!do_quiet_flag) fprintf(stderr,##x); } while (0)
#define ERR(x...) fprintf(stderr,##x)

#endif
