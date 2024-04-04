/*
 * <spindown.c>
 *
 * Copyright (C) 2000-2010 Daniel Kobras
 *
 * portions of spindown code adapted from hdparm, which is
 * Copyright (C) 2007 Mark Lord -- freely distributable
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
 * $Id: spindown.c,v 1.14 2010/07/25 00:44:52 nold Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "disk_info.h"
#include "spindown.h"
#include "diskhelper.h"
#include "noflushd.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <scsi/sg.h>

/* Indicate whether SCSI support was enabled. */
int nfd_do_scsi = 0;

/* IDE knows two versions of several commands. Some versions work on
 * old drives, some on new ones. We keep trying until we run out of
 * options.
 */
static int spindown_ide_check(char *name, int major, int minor)
{
	if (IDE_DISK_MAJOR(major))
		return 1;

	return 0;
}

static int spindown_ide(int fd)
{
	unsigned char pwrcheck[4]={WIN_CHECKPOWERMODE1, 0, 0, 0};
	unsigned char spindown[4]={WIN_STANDBYNOW1, 0, 0, 0};

	/* Make sure all prior (sync) commands have completed. */
	(void) ioctl(fd, HDIO_DRIVE_CMD, NULL);
	
	/* Has the user manually spun down the drive already? */
	if (ioctl(fd, HDIO_DRIVE_CMD, &pwrcheck) &&
	    (pwrcheck[0] = WIN_CHECKPOWERMODE2) &&
	    ioctl(fd, HDIO_DRIVE_CMD, &pwrcheck)) {
		if (errno == EIO && !pwrcheck[0] && !pwrcheck[1]) {
			DEBUG("Disk already sleeping");
			return 0;
		}
		DEBUG("Current disk state unknown");
	} else {
		if (pwrcheck[2] != 255) {
			DEBUG("Disk already standing by");
			return 0;
		}
		DEBUG("Disk was up and running");
	}
	
	if (ioctl(fd, HDIO_DRIVE_CMD, &spindown) &&
	    (spindown[0] = WIN_STANDBYNOW2) &&
	    ioctl(fd, HDIO_DRIVE_CMD, &spindown))
		return -1;

	return 0;
}

#ifdef SG_IO
/* ATA and SCSI disks are handled via a common interface these days and
 * need to be spun down using SG_IO functions.
 */
enum {
	ATA_OP_STANDBYNOW1 = 0xe0,
	ATA_OP_CHECKPOWERMODE1 = 0xe5,
	ATA_OP_STANDBYNOW2 = 0x94,
	ATA_OP_CHECKPOWERMODE2 = 0x98,
	ATA_USING_LBA = (1 << 6),
	ATA_STAT_DRQ = (1 << 3),
	ATA_STAT_ERR = (1 << 0),
};

enum {
	SG_ATA_16 = 0x85,
	SG_ATA_16_LEN = 16,
	SG_ATA_PROTO_NON_DATA = (3 << 1)
};

enum {
	SG_CDB2_CHECK_COND      = (1 << 5)
};

static int sgio_send(int fd, unsigned char cmd, unsigned char *rv)
{
	unsigned char cdb[SG_ATA_16_LEN];
	unsigned char sb[32];
	unsigned char *desc;
	unsigned char status, error;
	sg_io_hdr_t io_hdr;

	memset(&cdb, 0, sizeof(cdb));
	memset(&sb,     0, sizeof(sb));
	memset(&io_hdr, 0, sizeof(io_hdr));

	cdb[ 0] = SG_ATA_16;
	cdb[ 1] = SG_ATA_PROTO_NON_DATA;
	cdb[ 2] = SG_CDB2_CHECK_COND;
	cdb[13] = ATA_USING_LBA;
	cdb[14] = cmd;

	io_hdr.cmd_len = SG_ATA_16_LEN;
	io_hdr.interface_id     = 'S';
	io_hdr.mx_sb_len        = sizeof(sb);
	io_hdr.dxfer_direction  = SG_DXFER_NONE;
	io_hdr.dxfer_len        = 0;
	io_hdr.dxferp           = NULL;
	io_hdr.cmdp             = cdb;
	io_hdr.sbp              = sb;
	io_hdr.pack_id          = 0;
	io_hdr.timeout          = 5000; /* msecs */

	if (ioctl(fd, SG_IO, &io_hdr) == -1) {
		DEBUG("SG_IO ioctl() failed for cmd %u, %s",
		      cmd, strerror(errno));
		return -1;
	}

	desc = sb + 8;
	status = desc[13];
	error = desc[ 3];
	if (rv)
		*rv = desc[ 5];

	if (status & (ATA_STAT_ERR | ATA_STAT_DRQ)) {
		DEBUG("SG_IO cmd %u failed, status %u, error %u",
		      cmd, status, error);
		errno = EIO;
		return -1;
	}

	return 0;
}

static int spindown_sgio_check(char *name, int major, int minor)
{
	int fd, ret;
	unsigned char state;

	fd = open(name, O_RDONLY);
	if (fd == -1)
		return -1;

	ret = 1;
	if (sgio_send(fd, ATA_OP_CHECKPOWERMODE1, &state) &&
            sgio_send(fd, ATA_OP_CHECKPOWERMODE2, &state))
		ret = 0;

	(void) close(fd);

	return ret;
}

static int spindown_sgio(int fd)
{
	unsigned char state;

	if (sgio_send(fd, ATA_OP_CHECKPOWERMODE1, &state) &&
            sgio_send(fd, ATA_OP_CHECKPOWERMODE2, &state)) {
		DEBUG("Current disk state unknown");
	} else if (!state) {
		DEBUG("Disk already sleeping");
		return 0;
	} else {
		DEBUG("Disk was up and running - spinning down");
	}

	if (sgio_send(fd, ATA_OP_STANDBYNOW1, NULL) &&
	    sgio_send(fd, ATA_OP_STANDBYNOW2, NULL))
		return -1;

	return 0;
	

}

#else
/* Stubs if SG_IO is not supported by build environment */
static int spindown_sgio_check(char *name, int major, int minor)
{
	return 0;
}

static int spindown_sgio(int fd)
{
	return -1;
}
#endif /* SG_IO */

static int spindown_scsi_check(char *name, int major, int minor)
{
	if (SCSI_DISK_MAJOR(major)) {
		if (nfd_do_scsi)
			return 1;
		INFO("SCSI spindown is dangerous and disabled by default");
	}

	return 0;
}

static int spindown_scsi(int fd)
{
	/* FIXME Need a way to specify a barrier ensuring completion of
	 * previous syncs like in the IDE case. No idea whether this is
	 * possible with SCSI.
	 */
	return ioctl(fd, SCSI_IOCTL_STOP_UNIT, NULL);
}

/*
 * Generic spindown dispatcher. Calls the spindown method in the
 * disk_info structure. Returns 0 on failure, 1 on success.
 */
int spindown(disk_info_t di)
{
	int fd, err=0;

	DEBUG("Sending spindown command to %s.", di->name);

	fd = open(di->name, O_RDONLY);
	if (fd == -1) {
		ERR("Could not open %s for spindown", di->name);
		goto _err;
	}
	if (di->spindown(fd)) {
		ERR("Spindown for %s failed", di->name);
		goto _out;
	}
	di->spundown_at = time(NULL);
	err=1;
_out:
	close(fd);
_err:
	return err;
	
}

typedef struct spindown_handler {
	char *name;
	int (*check)(char *, int, int);
	spindown_t f;
} spindown_handler_t;

spindown_handler_t spindown_handlers[] = {
	{ "SGIO", spindown_sgio_check, spindown_sgio },
	{ "IDE", spindown_ide_check, spindown_ide },
	{ "SCSI", spindown_scsi_check, spindown_scsi },
	{ NULL, NULL, NULL }
};

spindown_t spindown_setup(disk_info_t di)
{
	spindown_handler_t *handler;

	for (handler=spindown_handlers; handler->name; handler++) {
		if (handler->check(di->name, di->major, di->minor)) {
			DEBUG("%s: using spindown handler %s",
			     di->name, handler->name);
			return handler->f;
		}
	}

	return NULL;

}
