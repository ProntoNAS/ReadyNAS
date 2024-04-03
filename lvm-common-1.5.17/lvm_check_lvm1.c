/*
 * lvm_check_lvm1.c
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

/* This checks for the existence of the LVM1 kernel driver
 * and returns the IOP number.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <asm/ioctl.h>
#include <sys/types.h>
#include <dirent.h>

#define LVM_GET_IOP_VERSION     _IOR ( 0xfe, 0x98, int)

int check_lvm1(void)
{
   int devfile = -1;
   int ret = 0;
   ushort lvm_iop_version = 0;

   if ( ( devfile = open ("/dev/lvm", O_RDONLY)) == -1)
       return 0;

   ret = ioctl ( devfile, LVM_GET_IOP_VERSION, &lvm_iop_version);
   close(devfile);

   if ( ret == 0)
       return lvm_iop_version;
   else
       return 0;
}

int count_lvm1_VGs(void)
{
    int count = 0;
    struct dirent *dentry;
    DIR *d;

    d = opendir("/proc/lvm/VGs");
    if (d != NULL) {
        while ((dentry = readdir(d)) != NULL) {
            if (strcmp(dentry->d_name, ".") && strcmp(dentry->d_name, ".."))
                count++;
        }

        closedir(d);  
    }

    return count;
}
