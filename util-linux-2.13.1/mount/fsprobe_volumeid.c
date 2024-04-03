#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libvolume_id.h>

#include "fsprobe.h"
#include "realpath.h"
#include "mount_paths.h"
#include "sundries.h"

enum probe_type {
	VOLUME_ID_NONE,
	VOLUME_ID_LABEL,
	VOLUME_ID_UUID,
	VOLUME_ID_TYPE,
};

static char
*probe(const char *device, enum probe_type type)
{
	int fd;
	uint64_t size;
	struct volume_id *id;
	const char *val;
	char *value = NULL;

	fd = open(device, O_RDONLY);
	if (fd < 0)
		return NULL;

	id = volume_id_open_fd(fd);
	if (!id) {
		close(fd);
		return NULL;
	}

	/* TODO: use blkdev_get_size() */
	if (ioctl(fd, BLKGETSIZE64, &size) != 0)
		size = 0;

	if (volume_id_probe_all(id, 0, size) == 0) {
		switch(type) {
		case VOLUME_ID_LABEL:
			if (volume_id_get_label(id, &val))
				value  = xstrdup(val);
			break;
		case VOLUME_ID_UUID:
			if (volume_id_get_uuid(id, &val))
				value  = xstrdup(val);
			break;
		case VOLUME_ID_TYPE:
			if (volume_id_get_type(id, &val))
				value  = xstrdup(val);
			break;
		default:
			break;
		}
	}

	volume_id_close(id);
	close(fd);
	return value;
}

void
fsprobe_init(void)
{
}

void
fsprobe_exit(void)
{
}

int
fsprobe_known_fstype(const char *fstype)
{
	if (volume_id_get_prober_by_type(fstype) != NULL)
		return 1;
	return 0;
}

const char *
fsprobe_get_uuid_by_devname(const char *devname)
{
	return probe(devname, VOLUME_ID_UUID);
}

const char *
fsprobe_get_label_by_devname(const char *devname)
{
	return probe(devname, VOLUME_ID_LABEL);
}

const char *
fsprobe_get_fstype_by_devname(const char *devname)
{
	return probe(devname, VOLUME_ID_TYPE);
}

const char *
fsprobe_get_devname_by_uuid(const char *uuid)
{
	char dev[PATH_MAX];
	size_t len;

	if (!uuid)
		return NULL;

	strcpy(dev, PATH_DEV_BYUUID "/");
	len = strlen(PATH_DEV_BYUUID "/");
	if (!volume_id_encode_string(uuid, &dev[len], sizeof(dev) - len) != 0)
		return NULL;
	return canonicalize(dev);
}

const char *
fsprobe_get_devname_by_label(const char *label)
{
	char dev[PATH_MAX];
	size_t len;

	if (!label)
		return NULL;
	strcpy(dev, PATH_DEV_BYLABEL "/");
	len = strlen(PATH_DEV_BYLABEL "/");
	if (!volume_id_encode_string(label, &dev[len], sizeof(dev) - len) != 0)
		return NULL;
	return canonicalize(dev);
}
