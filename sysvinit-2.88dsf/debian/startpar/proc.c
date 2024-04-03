/*
 * Copyright (c) 2004 SuSE Linux AG
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
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ****************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "proc.h"

static unsigned long int scan_one(const char* buff, const char *key)
{
    const char *b = strstr(buff, key);
    unsigned long val = 0;

    if (!b) {
	fprintf(stderr, "ERROR: no hit for %s\n", key);
	return ~0UL;
    }
    if (sscanf(b + strlen(key), " %lu", &val) != 1)
	return 0;
    return val;
}

int read_proc(unsigned long int * const prcs_run, unsigned long int * const prcs_blked)
{
    char StatBuf[2048], *ptr = &StatBuf[0];
    unsigned long int running, blocked;
    ssize_t len;
    size_t skip;
    FILE *stat;

    *prcs_run = 0;
    *prcs_blked = 0;

    if ((stat = fopen("/proc/stat", "r")) == (FILE*)0) {
	fprintf(stderr, "ERROR: could not open /proc/stat: %s\n", strerror(errno));
	return 1;
    }

    len = sizeof(StatBuf);
    while ((len > 0) && (fgets(ptr, len, stat))) {
	if (ptr[0] != 'p')
	    continue;
	skip = strlen(ptr);
	len -= skip;
	ptr += skip;
    }
    fclose(stat);
    
    /* These fields are not present in /proc/stat for 2.4 kernels */
    running = scan_one(StatBuf, "procs_running");
    blocked = scan_one(StatBuf, "procs_blocked");

    if (running == ~0UL || blocked == ~0UL)
	return 1;

    *prcs_run   = running;
    *prcs_blked = blocked;

    return 0;
}
