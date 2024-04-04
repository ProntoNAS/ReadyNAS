/* scsi-startstop.c
 * $Id: scsi-startstop.c,v 1.3 2000/01/15 18:37:55 belbo Exp $
 *
 * tool to spin down SCSI disks
 * ('nuff said)
 *
 * 19990826 Daniel Kobras <kobras@linux.de>
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/major.h>

#define SCSI_IOCTL_START_UNIT	5
#define SCSI_IOCTL_STOP_UNIT	6

void usage(char *name)
{
    fprintf(stderr, "Usage: %s <device> {start|stop}\n"
		    "       spin up/down SCSI disk on <device>.\n",
		    name);
}

int main(int argc, char *argv[])
{
    int res = 1, fd, ioc;
    struct stat statbuf;

    if (argc != 3) {
	usage(argv[0]);
	exit(1);
    }
    if (!strcmp("start", argv[2]))
	ioc = SCSI_IOCTL_START_UNIT;
    else if (!strcmp("stop", argv[2]))
	ioc = SCSI_IOCTL_STOP_UNIT;
    else {
	usage(argv[0]);
	exit(1);
    }
    if ((fd = open(argv[1], O_RDWR)) < 0)
	perror(argv[0]), exit(1);
    if (fstat(fd, &statbuf) < 0)
	goto _err_out;
    if (!S_ISBLK(statbuf.st_mode) ||
	(statbuf.st_rdev >> 8) & 255 != SCSI_DISK_MAJOR) {
		fprintf(stderr, "%s is not a SCSI block device\n", argv[1]);
		goto _out;
    }
    if (ioctl(fd, SCSI_IOCTL_STOP_UNIT, NULL) < 0)
	goto _err_out;
    res = 0;
  _out:
    close(fd);
    exit(res);
  _err_out:
    perror(argv[1]);
    goto _out;
}
