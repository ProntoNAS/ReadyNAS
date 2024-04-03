/*
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 *
 * GPT (GUID Partition Table) signature detection. Based on libparted and
 * util-linux/partx.
 *
 * Warning: this code doesn't do all GPT checks (CRC32, Protective MBR, ..).
 *          It's really GPT signature detection only.
 *
 * Copyright (C) 2007 Karel Zak <kzak@redhat.com>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "gpt.h"

#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define SECTOR_SIZE	512	/* default */

#define _GET_BYTE(x, n)		( ((x) >> (8 * (n))) & 0xff )

#define _PED_SWAP64(x)		( (_GET_BYTE(x, 0) << 56)	\
				+ (_GET_BYTE(x, 1) << 48)	\
				+ (_GET_BYTE(x, 2) << 40)	\
				+ (_GET_BYTE(x, 3) << 32)	\
				+ (_GET_BYTE(x, 4) << 24)	\
				+ (_GET_BYTE(x, 5) << 16)	\
				+ (_GET_BYTE(x, 6) << 8)	\
				+ (_GET_BYTE(x, 7) << 0) )

#define PED_SWAP64(x)           ((uint64_t) _PED_SWAP64( (uint64_t) (x) ))

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define CPU_TO_LE64(x)	(x)
#else
# define CPU_TO_LE64(x)	PED_SWAP64(x)
#endif

#define BLKSSZGET  _IO(0x12,104) /* get block device sector size */
#define BLKGETLASTSECT  _IO(0x12,108)   /* get last sector of block device */
#define BLKGETSIZE _IO(0x12,96)	/* return device size */
#define BLKGETSIZE64 _IOR(0x12,114,size_t)	/* return device size in bytes (u64 *arg) */

#define GPT_HEADER_SIGNATURE 0x5452415020494645LL
#define GPT_PRIMARY_PARTITION_TABLE_LBA 1

typedef struct {
        uint32_t time_low;
        uint16_t time_mid;
        uint16_t time_hi_and_version;
        uint8_t  clock_seq_hi_and_reserved;
        uint8_t  clock_seq_low;
        uint8_t  node[6];
} /* __attribute__ ((packed)) */ efi_guid_t;
/* commented out "__attribute__ ((packed))" to work around gcc bug (fixed
 * in gcc3.1): __attribute__ ((packed)) breaks addressing on initialized
 * data.  It turns out we don't need it in this case, so it doesn't break
 * anything :)
 */

typedef struct _GuidPartitionTableHeader_t {
	uint64_t Signature;
	uint32_t Revision;
	uint32_t HeaderSize;
	uint32_t HeaderCRC32;
	uint32_t Reserved1;
	uint64_t MyLBA;
	uint64_t AlternateLBA;
	uint64_t FirstUsableLBA;
	uint64_t LastUsableLBA;
	efi_guid_t DiskGUID;
	uint64_t PartitionEntryLBA;
	uint32_t NumberOfPartitionEntries;
	uint32_t SizeOfPartitionEntry;
	uint32_t PartitionEntryArrayCRC32;
	uint8_t Reserved2[512 - 92];
} __attribute__ ((packed)) GuidPartitionTableHeader_t;

struct blkdev_ioctl_param {
        unsigned int block;
        size_t content_length;
        char * block_contents;
};

static int
_get_linux_version (void)
{
	static int kver = -1;
	struct utsname uts;
	int major;
	int minor;
	int teeny;

	if (kver != -1)
		return kver;
	if (uname (&uts))
		return kver = 0;
	if (sscanf (uts.release, "%u.%u.%u", &major, &minor, &teeny) != 3)
		return kver = 0;
	return kver = KERNEL_VERSION (major, minor, teeny);
}

static unsigned int
_get_sector_size (int fd)
{
	unsigned int sector_size;

	if (_get_linux_version() < KERNEL_VERSION (2,3,0))
		return SECTOR_SIZE;
	if (ioctl (fd, BLKSSZGET, &sector_size))
		return SECTOR_SIZE;
	return sector_size;
}

static uint64_t
_get_num_sectors(int fd)
{
	int version = _get_linux_version();
	unsigned long	size;
	uint64_t bytes=0;

	if (version >= KERNEL_VERSION(2,5,4) ||
		(version <  KERNEL_VERSION(2,5,0) &&
		 version >= KERNEL_VERSION (2,4,18)))
	{
                if (ioctl(fd, BLKGETSIZE64, &bytes) == 0)
                        return bytes / _get_sector_size(fd);
	}
	if (ioctl (fd, BLKGETSIZE, &size))
		return 0;
	return size;
}

static uint64_t
last_lba(int fd)
{
	int rc;
	uint64_t sectors = 0;
	struct stat s;

	memset(&s, 0, sizeof (s));
	rc = fstat(fd, &s);
	if (rc == -1)
	{
		fprintf(stderr, "last_lba() could not stat: %s\n",
			strerror(errno));
		return 0;
	}
	if (S_ISBLK(s.st_mode))
		sectors = _get_num_sectors(fd);
	else if (S_ISREG(s.st_mode))
		sectors = s.st_size >> _get_sector_size(fd);
	else
	{
		fprintf(stderr,
			"last_lba(): I don't know how to handle files with mode %o\n",
			s.st_mode);
		sectors = 1;
	}
	return sectors - 1;
}

static ssize_t
read_lastoddsector(int fd, uint64_t lba, void *buffer, size_t count)
{
        int rc;
        struct blkdev_ioctl_param ioctl_param;

        if (!buffer) return 0;

        ioctl_param.block = 0; /* read the last sector */
        ioctl_param.content_length = count;
        ioctl_param.block_contents = buffer;

        rc = ioctl(fd, BLKGETLASTSECT, &ioctl_param);
        if (rc == -1) perror("read failed");

        return !rc;
}

static ssize_t
read_lba(int fd, uint64_t lba, void *buffer, size_t bytes)
{
	int sector_size = _get_sector_size(fd);
	off_t offset = lba * sector_size;
        ssize_t bytesread;

	lseek(fd, offset, SEEK_SET);
	bytesread = read(fd, buffer, bytes);

        /* Kludge.  This is necessary to read/write the last
           block of an odd-sized disk, until Linux 2.5.x kernel fixes.
           This is only used by gpt.c, and only to read
           one sector, so we don't have to be fancy.
        */
        if (!bytesread && !(last_lba(fd) & 1) && lba == last_lba(fd))
                bytesread = read_lastoddsector(fd, lba, buffer, bytes);
        return bytesread;
}

static GuidPartitionTableHeader_t *
alloc_read_gpt_header(int fd, uint64_t lba)
{
	GuidPartitionTableHeader_t *gpt =
		(GuidPartitionTableHeader_t *) malloc(sizeof (GuidPartitionTableHeader_t));
	if (!gpt)
		return NULL;
	memset(gpt, 0, sizeof (*gpt));
	if (!read_lba(fd, lba, gpt, sizeof (GuidPartitionTableHeader_t)))
	{
		free(gpt);
		return NULL;
	}
	return gpt;
}

static int
gpt_check_signature(int fd, uint64_t lba)
{
	GuidPartitionTableHeader_t *gpt;
	int res=0;

	if ((gpt = alloc_read_gpt_header(fd, lba)))
	{
		if (gpt->Signature == CPU_TO_LE64(GPT_HEADER_SIGNATURE))
			res = 1;
		free(gpt);
	}
	return res;
}

/* returns:
 *	0 not found GPT
 *	1 for valid primary GPT header
 *	2 for valid alternative GPT header
 */
int
gpt_probe_signature_fd(int fd)
{
	int res = 0;

	/* check primary GPT header */
	if (gpt_check_signature(fd, GPT_PRIMARY_PARTITION_TABLE_LBA))
		res = 1;
	else
	{
		/* check alternative GPT header */
		uint64_t lastlba = last_lba(fd);
		if (gpt_check_signature(fd, lastlba))
			res = 2;
	}
	return res;
}

int
gpt_probe_signature_devname(char *devname)
{
	int res, fd;
	if ((fd = open(devname, O_RDONLY)) < 0)
		return 0;
	res = gpt_probe_signature_fd(fd);
	close(fd);
	return res;
}

#ifdef GPT_TEST_MAIN
int
main(int argc, char **argv)
{
	if (argc!=2)
	{
		fprintf(stderr, "usage: %s <dev>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (gpt_probe_signature_devname(argv[1]))
		printf("GPT (GUID Partition Table) detected on %s\n", argv[1]);
	exit(EXIT_SUCCESS);
}
#endif
