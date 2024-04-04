/*
 * <diskhelper.h>
 *
 * Copyright (C) 2000, 2001 Daniel Kobras
 *
 * Definitions taken from linux/major.h from the Linux kernel sources,
 * copyright Linus Torvalds and others.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * $Id: diskhelper.h,v 1.7 2004/03/16 21:20:21 nold Exp $
 *
 */

#ifndef _NFD_DISKHELPER_H
#define _NFD_DISKHELPER_H

#define IDE0_MAJOR      3
#define IDE1_MAJOR      22
#define IDE2_MAJOR	33
#define IDE3_MAJOR      34
#define IDE4_MAJOR      56
#define IDE5_MAJOR      57
#define IDE6_MAJOR      88
#define IDE7_MAJOR      89
#define IDE8_MAJOR      90
#define IDE9_MAJOR      91


#define IDE_DISK_MAJOR(M) ((M) == IDE0_MAJOR || (M) == IDE1_MAJOR || \
			   (M) == IDE2_MAJOR || (M) == IDE3_MAJOR || \
			   (M) == IDE4_MAJOR || (M) == IDE5_MAJOR || \
			   (M) == IDE6_MAJOR || (M) == IDE7_MAJOR || \
			   (M) == IDE8_MAJOR || (M) == IDE9_MAJOR)
	
/* IDE spindown and powercheck ioctl()s - stolen from hdparm source */
#define WIN_STANDBYNOW1		0xE0
#define WIN_STANDBYNOW2		0x94
#define WIN_CHECKPOWERMODE1	0xE5
#define WIN_CHECKPOWERMODE2	0x98

#define HDIO_DRIVE_CMD          0x031f

/* glibc 2.0 had the ioctls defined in include/scsi/scsi_ioctl.h. Looks like 
 * this file got removed in glibc 2.1. */
#define SCSI_IOCTL_START_UNIT	5
#define SCSI_IOCTL_STOP_UNIT	6

#define SCSI_DISK0_MAJOR	8
#define SCSI_DISK1_MAJOR        65
#define SCSI_DISK2_MAJOR        66
#define SCSI_DISK3_MAJOR        67
#define SCSI_DISK4_MAJOR        68
#define SCSI_DISK5_MAJOR        69
#define SCSI_DISK6_MAJOR        70
#define SCSI_DISK7_MAJOR        71

#define SCSI_DISK_MAJOR(M) ((M) == SCSI_DISK0_MAJOR || \
	((M) >= SCSI_DISK1_MAJOR && (M) <= SCSI_DISK7_MAJOR))

#define MD_MAJOR	9
#define LVM_BLK_MAJOR   58
#define AMIRAID_MAJOR	101
#define ATARAID_MAJOR	114
#define IS_DAC960(M)    ((M) >= 48 && (M) <= 55)
#define DM_MAJOR        253  //dm-* device
	
/* True if we deal with a meta device that is composed of other
 * low level block devices.
 */
#define IS_META(major,minor)	\
		(((major) == MD_MAJOR || (major) == LVM_BLK_MAJOR || \
		  (major) == AMIRAID_MAJOR || (major) == ATARAID_MAJOR || \
		  (major) == DM_MAJOR || IS_DAC960((major))))

#endif

