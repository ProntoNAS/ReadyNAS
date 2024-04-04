/*
 * case.h - Case-insensitive file name comparators
 * Copyright (c) 2013 Hiro Sugawara <hiro.sugawara@netgear.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS source
 * in the file COPYING); if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __FS_CASE_CASE_H
#define __FS_CASE_CASE_H

#include <uapi/linux/uio.h>


struct strcase_operations {
	int (*strcasecmp_vec)(struct iovec *iov1, int vlen1,
				struct iovec *iov2, int vlen2);
	u32 (*toupper_vec)(struct iovec *iov, int vlen);
};

#endif
