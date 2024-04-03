/*
 * <spindown.c>
 *
 * Copyright (C) 2000 Daniel Kobras
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
 * $Id: spindown.c,v 1.1.1.1 2010-09-21 01:09:51 jmaggard Exp $
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

/* Indicate whether SCSI support was enabled. */
int nfd_do_scsi = 0;

/* IDE knows two versions of several commands. Some versions work on
 * old drives, some on new ones. We keep trying until we run out of
 * options.
 */
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
		di->time_left=timeout_get(di->timeouts);
		goto _out;
	}
	di->spundown_at = time(NULL);
	err=1;
_out:
	close(fd);
_err:
	return err;
	
}

spindown_t spindown_setup(int major, int minor)
{
#ifdef __arm__
	return spindown_ide;
#endif
	if (IDE_DISK_MAJOR(major))
		return spindown_ide;

	if (SCSI_DISK_MAJOR(major)) {
		if (nfd_do_scsi)
			return spindown_scsi;

		INFO("SCSI spindown is dangerous and disabled by default");
	}

	return NULL;

}
