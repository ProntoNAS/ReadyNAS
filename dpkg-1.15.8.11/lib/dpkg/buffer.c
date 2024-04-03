/*
 * libdpkg - Debian packaging suite library routines
 * buffer.c - buffer I/O handling routines
 *
 * Copyright © 1999, 2000 Wichert Akkerman <wakkerma@debian.org>
 * Copyright © 2000-2003 Adam Heath <doogie@debian.org>
 * Copyright © 2008, 2009 Guillem Jover <guillem@debian.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <compat.h>

#include <sys/types.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <dpkg/i18n.h>
#include <dpkg/dpkg.h>
#include <dpkg/varbuf.h>
#include <dpkg/md5.h>
#include <dpkg/buffer.h>

struct buffer_write_md5ctx {
	struct MD5Context ctx;
	char *hash;
};

static void
buffer_md5_init(struct buffer_data *data)
{
	struct buffer_write_md5ctx *ctx;

	ctx = m_malloc(sizeof(struct buffer_write_md5ctx));
	ctx->hash = data->arg.ptr;
	data->arg.ptr = ctx;
	MD5Init(&ctx->ctx);
}

off_t
buffer_init(struct buffer_data *read_data, struct buffer_data *write_data)
{
	switch (write_data->type) {
	case BUFFER_WRITE_MD5:
		buffer_md5_init(write_data);
		break;
	}
	return 0;
}

static void
buffer_md5_done(struct buffer_data *data)
{
	struct buffer_write_md5ctx *ctx;
	unsigned char digest[16], *p = digest;
	char *hash;
	int i;

	ctx = (struct buffer_write_md5ctx *)data->arg.ptr;
	hash = ctx->hash;
	MD5Final(digest, &ctx->ctx);
	for (i = 0; i < 16; ++i) {
		sprintf(hash, "%02x", *p++);
		hash += 2;
	}
	*hash = '\0';
	free(ctx);
}

off_t
buffer_done(struct buffer_data *read_data, struct buffer_data *write_data)
{
	switch (write_data->type) {
	case BUFFER_WRITE_MD5:
		buffer_md5_done(write_data);
		break;
	}
	return 0;
}

off_t
buffer_write(struct buffer_data *data, const void *buf, off_t length)
{
	off_t ret = length;

	switch (data->type) {
	case BUFFER_WRITE_BUF:
		memcpy(data->arg.ptr, buf, length);
		data->arg.ptr += length;
		break;
	case BUFFER_WRITE_VBUF:
		varbufaddbuf((struct varbuf *)data->arg.ptr, buf, length);
		break;
	case BUFFER_WRITE_FD:
		ret = write(data->arg.i, buf, length);
		break;
	case BUFFER_WRITE_NULL:
		break;
	case BUFFER_WRITE_STREAM:
		ret = fwrite(buf, 1, length, (FILE *)data->arg.ptr);
		if (feof((FILE *)data->arg.ptr))
			return -1;
		if (ferror((FILE *)data->arg.ptr))
			return -1;
		break;
	case BUFFER_WRITE_MD5:
		MD5Update(&(((struct buffer_write_md5ctx *)data->arg.ptr)->ctx), buf, length);
		break;
	default:
		internerr("unknown data type '%i' in buffer_write",
		          data->type);
	}

	return ret;
}

off_t
buffer_read(struct buffer_data *data, void *buf, off_t length)
{
	off_t ret;

	switch (data->type) {
	case BUFFER_READ_FD:
		ret = read(data->arg.i, buf, length);
		break;
	case BUFFER_READ_STREAM:
		ret = fread(buf, 1, length, (FILE *)data->arg.ptr);
		if (feof((FILE *)data->arg.ptr))
			return ret;
		if (ferror((FILE *)data->arg.ptr))
			return -1;
		break;
	default:
		internerr("unknown data type '%i' in buffer_read\n",
		          data->type);
	}

	return ret;
}

#define buffer_copy_TYPE(name, type1, name1, type2, name2) \
off_t \
buffer_copy_##name(type1 n1, int typeIn, \
                   type2 n2, int typeOut, \
                   off_t limit, const char *desc, ...) \
{ \
	va_list args; \
	struct buffer_data read_data, write_data; \
	struct varbuf v = VARBUF_INIT; \
	off_t ret; \
\
	read_data.arg.name1 = n1; \
	read_data.type = typeIn; \
	write_data.arg.name2 = n2; \
	write_data.type = typeOut; \
\
	va_start(args, desc); \
	varbufvprintf(&v, desc, args); \
	va_end(args); \
\
	buffer_init(&read_data, &write_data); \
	ret = buffer_copy(&read_data, &write_data, limit, v.buf); \
	buffer_done(&read_data, &write_data); \
\
	varbuf_destroy(&v); \
\
	return ret; \
}

buffer_copy_TYPE(IntInt, int, i, int, i);
buffer_copy_TYPE(IntPtr, int, i, void *, ptr);
buffer_copy_TYPE(PtrInt, void *, ptr, int, i);
buffer_copy_TYPE(PtrPtr, void *, ptr, void *, ptr);

off_t
buffer_hash(const void *input, void *output, int type, off_t limit)
{
	struct buffer_data data = { .arg.ptr = output, .type = type };
	off_t ret;

	buffer_init(NULL, &data);
	ret = buffer_write(&data, input, limit);
	buffer_done(NULL, &data);

	return ret;
}

off_t
buffer_copy(struct buffer_data *read_data, struct buffer_data *write_data,
            off_t limit, const char *desc)
{
	char *buf, *writebuf;
	int bufsize = 32768;
	long bytesread = 0, byteswritten = 0;
	off_t totalread = 0, totalwritten = 0;

	if ((limit != -1) && (limit < bufsize))
		bufsize = limit;
	if (bufsize == 0)
		return 0;

	buf = m_malloc(bufsize);

	while (bytesread >= 0 && byteswritten >= 0 && bufsize > 0) {
		bytesread = buffer_read(read_data, buf, bufsize);
		if (bytesread < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			break;
		}
		if (bytesread == 0)
			break;

		totalread += bytesread;
		if (limit != -1) {
			limit -= bytesread;
			if (limit < bufsize)
				bufsize = limit;
		}
		writebuf = buf;
		while (bytesread) {
			byteswritten = buffer_write(write_data, writebuf, bytesread);
			if (byteswritten == -1) {
				if (errno == EINTR || errno == EAGAIN)
					continue;
				break;
			}
			if (byteswritten == 0)
				break;

			bytesread -= byteswritten;
			totalwritten += byteswritten;
			writebuf += byteswritten;
		}
	}

	if (bytesread < 0)
		ohshite(_("failed to read on buffer copy for %s"), desc);
	if (byteswritten < 0)
		ohshite(_("failed in write on buffer copy for %s"), desc);
	if (limit > 0)
		ohshit(_("short read on buffer copy for %s"), desc);

	free(buf);

	return totalread;
}

