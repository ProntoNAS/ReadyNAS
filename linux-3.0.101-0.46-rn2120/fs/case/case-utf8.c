/*
 * UTF-8 to Unicode codepoint translation
 *
 * Copyright (c) 2013 NETGEAR, Inc.
 * Copyright (c) 2013 Hiro Sugawara
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
 */

#include <linux/kernel.h>
#include <linux/uio.h>
#include "case.h"

/*
 * Converts one character from top of a UTF8 string to its unicode codepoint.
 * String may be split into discontiguous parts.
 *	vec	UTF8 string vector. Vector contents will be updated.
 *	veclen	Number of vector elemensts
 * Rerturns unicode codepoint or ~0 if string is too short
 *
 * This code is very signedness-sensitive.
 */
u32 utf8_to_cp(struct iovec *vec, int veclen)
{
	static const s8 mask[] = {0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
	const s8 *mp = &mask[0];

	int shift = 0;
	u32 cx = 0;
	s8 c;
	s8 *p = vec->iov_base;
	size_t len = vec->iov_len;

	BUG_ON(!veclen || !len || !p);

	c = *p++;
	len--;
	if (!(c & 0x80)) {
		cx = (u32)c;
		goto done;
	}

	do {
		s8 mask = *mp++;

		if (unlikely(!len)) {
			if (unlikely(!--veclen))
				return (u32)~0;
			(vec++)->iov_len = 0;
			p = vec->iov_base;
			len = vec->iov_len;
			BUG_ON(!len || !p);
		}
		shift += 6;
		cx |= (*p++ & 0x3f);
		len--;
		if ((c & mask) == (mask << 1)) {
			cx |= ((c & ~mask) << shift);
			break;
		}
		cx <<= 6;
	} while (~*mp);

done:
	vec->iov_base = p;
	vec->iov_len = len;
	return cx;
}

static int inline utf8_strcasecmp_vec(struct iovec *vec1, int len1,
				struct iovec *vec2, int len2)
{
	return ucp_strcasecmp_vec(vec1, len1, utf8_to_cp,
				  vec2, len2, utf8_to_cp);
}

static u32 utf8_toupper_vec(struct iovec *vec, int veclen)
{
	return ucp_toupper_vec(vec, veclen, utf8_to_cp);
}

const struct strcase_operations utf8_strcase_ops = {
	.strcasecmp_vec	= utf8_strcasecmp_vec,
	.toupper_vec = utf8_toupper_vec,
};
