/*
 * lvm_check_lvm2.c
 *
 * Copyright (C) 2002  Patrick Caulfield <patrick@debian.org>
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

/* This checks for the existence of the LVM2 device-mapper
   in the kernel and returns 200 if so, (used as a version number)
                               0 if not,
*/

#include <stdio.h>
#include <string.h>

int check_lvm2(void)
{
    FILE *misc;
    char linebuf[1024];

    misc = fopen("/proc/misc", "r");
    if (misc) {
        while (fgets(linebuf, sizeof(linebuf), misc)) {
            if (strstr(linebuf, "device-mapper\n") != NULL) {
                fclose(misc);
                return 200;
            }
        }

        fclose(misc);
    }

    return 0;
}
