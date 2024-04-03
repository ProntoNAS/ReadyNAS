#include "common.h"

int getMdVersion(struct md_version * ver) {
    int file;
    int ret;

    file = open("/dev/md0",O_RDONLY);
    if (file < 0) {
	file = open("/dev/md/0",O_RDONLY);
	if (file < 0)
		return -1;
    }

    ret = ioctl(file, RAID_VERSION, (unsigned long)ver);
    if (ret == -1) {
	if (errno == EINVAL) {
	    /* just fake it */
	    ver->major = 0;
	    ver->minor = 36;
	    ver->patchlevel = 0;
	} else {
	    close(file);
	    return -1;
	}
    }

    close(file);

    return 0;
}
