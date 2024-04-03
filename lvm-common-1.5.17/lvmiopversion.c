/*
 * tools/lvmiopversion.c
 *
 * Copyright (C) 2000  Andreas Dilger <adilger@turbolinux.com>
 * Modifications to act as wrapper (C) 2000 Tom Lees <tom@debian.org>
 * Modifications (almost a rewrite) for LVM2 (C) 2003 Patrick Caulfield <patrick@debian.org>
 * Copyright (C) 2003  Andres Salomon <dilinger@voxel.net>
 *
 * LVM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * LVM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LVM; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

#define LVM_EDRIVER             95
#define LVM_EINVALID_IOP            96
#define LVM_EINVALID_CMD_LINE           99

const char *cmd = "lvmiopversion";

int main(int argc, char *argv[])
{
    struct stat st;
    char prog[256];
    char *name;
    int version;

    name = basename(argv[0]);
    version = check_lvm2();
    /* revert to lvm1 if a) kernel driver isn't loaded, or
     * b) there's an active VG that's using the lvm1 kernel driver. */
    if (!version || count_lvm1_VGs())
        version = check_lvm1();

    /* If we were called as lvmiopversion, display the IOP and exit */
    if (strcmp(name, cmd) == 0) {
        printf("%d\n", version);
        return 0;
    }

    snprintf(prog, sizeof(prog), "/lib/lvm-%d/%s", version, name);
    if (stat(prog, &st) != 0 && version >= 200) {
        /* LVM2 kernel is loaded, but userspace is unavailable; try LVM1 */
        version = check_lvm1();
        if (version)
            snprintf(prog, sizeof(prog), "/lib/lvm-%d/%s", version, name);
    }
    
    /* Let missing driver error messages come from the tools themselves */
    if (!version || stat(prog, &st) != 0)
        snprintf(prog, sizeof(prog), "/lib/lvm-default/%s", name);

    /* Don't even try to deal w/ ancient LVM1 drivers */
    if (version && version < 10) {
        fprintf(stderr, "%s -- Unsupported LVM driver/module in kernel, please upgrade!\n\n", cmd);
        return LVM_EINVALID_IOP;
    }
 
    /* and attempt to execute program.. */
    execv(prog, argv);
    if (errno == ENOENT) {
	fprintf(stderr, "No program \"%s\" found for your current version of LVM\n", name);
    }
    else {
        fprintf(stderr, "%s -- Exec of %s failed\n\n", cmd, name);
        perror(cmd);
    }
    return LVM_EINVALID_CMD_LINE;
}
