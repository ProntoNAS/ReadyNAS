/*
 * libdpkg - Debian packaging suite library routines
 * macros.h - C language support macros
 *
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

#ifndef LIBDPKG_MACROS_H
#define LIBDPKG_MACROS_H

#ifndef LIBDPKG_VOLATILE_API
#error "The libdpkg API is to be considered volatile, please read 'README.api'."
#endif

/* Language definitions. */

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define DPKG_ATTR_UNUSED	__attribute__((unused))
#define DPKG_ATTR_CONST		__attribute__((const))
#define DPKG_ATTR_NORET		__attribute__((noreturn))
#define DPKG_ATTR_PRINTF(n)	__attribute__((format(printf, n, n + 1)))
#define DPKG_ATTR_VPRINTF(n)	__attribute__((format(printf, n, 0)))
#else
#define DPKG_ATTR_UNUSED
#define DPKG_ATTR_CONST
#define DPKG_ATTR_NORET
#define DPKG_ATTR_PRINTF(n)
#define DPKG_ATTR_VPRINTF(n)
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define DPKG_ATTR_SENTINEL	__attribute__((sentinel))
#else
#define DPKG_ATTR_SENTINEL
#endif

#ifdef __cplusplus
#define DPKG_BEGIN_DECLS	extern "C" {
#define DPKG_END_DECLS		}
#else
#define DPKG_BEGIN_DECLS
#define DPKG_END_DECLS
#endif

#ifndef array_count
#define array_count(a) (sizeof(a) / sizeof((a)[0]))
#endif

/* For C++ use native implementations from STL or similar. */
#ifndef __cplusplus
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#endif

#endif /* LIBDPKG_MACROS_H */

