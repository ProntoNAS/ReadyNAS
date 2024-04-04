/*
 * Copyright (C) 2009 Red Hat, Inc.
 *
 * This file may be redistributed under the terms of the
 * GNU Lesser General Public License.
 */

#include "superblocks.h"

const struct blkid_idinfo netgear_ghs_idinfo =
{
	.name		= "netgear_global_hotspare",
	.usage		= BLKID_USAGE_RAID,
	.magics		= {
		{ .magic = "\x4e\x45\x54\x47\x45\x41\x52\x47\x48\x53",
		  .len = 9,
		  .sboff = 448
		},
		{ NULL }
	}
};
