/*
 * libdpkg - Debian packaging suite library routines
 * varbuf.h - variable length expandable buffer handling
 *
 * Copyright © 1994, 1995 Ian Jackson <ian@chiark.greenend.org.uk>
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

#ifndef LIBDPKG_VARBUF_H
#define LIBDPKG_VARBUF_H

#include <stddef.h>
#include <stdarg.h>

#include <dpkg/macros.h>

DPKG_BEGIN_DECLS

/*
 * varbufinit must be called exactly once before the use of each varbuf
 * (including before any call to varbuf_destroy), or the variable must be
 * initialized with VARBUF_INIT.
 *
 * However, varbufs allocated ‘static’ are properly initialised anyway and
 * do not need varbufinit; multiple consecutive calls to varbufinit before
 * any use are allowed.
 *
 * varbuf_destroy must be called after a varbuf is finished with, if anything
 * other than varbufinit has been done. After this you are allowed but
 * not required to call varbufinit again if you want to start using the
 * varbuf again.
 *
 * Callers using C++ need not worry about any of this.
 */
struct varbuf {
	size_t used, size;
	char *buf;

#ifdef __cplusplus
	varbuf(size_t _size = 0);
	~varbuf();
	void init(size_t _size = 0);
	void reset();
	void destroy();
	void operator()(int c);
	void operator()(const char *s);
	void terminate(void/*to shut 2.6.3 up*/);
	const char *string();
#endif
};

#define VARBUF_INIT { 0, 0, NULL }

void varbufinit(struct varbuf *v, size_t size);
void varbuf_grow(struct varbuf *v, size_t need_size);
void varbuf_trunc(struct varbuf *v, size_t used_size);
char *varbuf_detach(struct varbuf *v);
void varbufreset(struct varbuf *v);
void varbuf_destroy(struct varbuf *v);

void varbufaddc(struct varbuf *v, int c);
void varbufdupc(struct varbuf *v, int c, size_t n);
void varbufsubstc(struct varbuf *v, int c_src, int c_dst);
#define varbufaddstr(v, s) varbufaddbuf(v, s, strlen(s))
void varbufaddbuf(struct varbuf *v, const void *s, size_t size);

int varbufprintf(struct varbuf *v, const char *fmt, ...) DPKG_ATTR_PRINTF(2);
int varbufvprintf(struct varbuf *v, const char *fmt, va_list va)
	DPKG_ATTR_VPRINTF(2);

DPKG_END_DECLS

#ifdef __cplusplus
inline
varbuf::varbuf(size_t _size)
{
	varbufinit(this, _size);
}

inline
varbuf::~varbuf()
{
	varbuf_destroy(this);
}

inline void
varbuf::init(size_t _size)
{
	varbufinit(this, _size);
}

inline void
varbuf::reset()
{
	used = 0;
}

inline void
varbuf::destroy()
{
	varbuf_destroy(this);
}

inline void
varbuf::operator()(int c)
{
	varbufaddc(this, c);
}

inline void
varbuf::operator()(const char *s)
{
	varbufaddstr(this, s);
}

inline void
varbuf::terminate(void/*to shut 2.6.3 up*/)
{
	varbufaddc(this, 0);
	used--;
}

inline const char *
varbuf::string()
{
	terminate();
	return buf;
}
#endif

#endif /* LIBDPKG_VARBUF_H */
