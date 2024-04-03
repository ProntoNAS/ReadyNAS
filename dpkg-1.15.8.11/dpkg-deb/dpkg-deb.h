/*
 * dpkg-deb - construction and deconstruction of *.deb archives
 * dpkg-deb.h - external definitions for this program
 *
 * Copyright © 1994,1995 Ian Jackson <ian@chiark.greenend.org.uk>
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

#ifndef DPKG_DEB_H
#define DPKG_DEB_H

typedef void dofunction(const char *const *argv);
dofunction do_build DPKG_ATTR_NORET;
dofunction do_contents, do_control, do_showinfo;
dofunction do_info, do_field, do_extract, do_vextract, do_fsystarfile;

extern int debugflag, nocheckflag, oldformatflag;
extern const struct cmdinfo *cipaction;
extern dofunction *action;

void extracthalf(const char *debar, const char *directory,
                 const char *taroption, int admininfo);

extern const char* showformat;
extern struct compressor *compressor;
extern int compress_level;

#define ARCHIVEVERSION		"2.0"

#define BUILDCONTROLDIR		"DEBIAN"
#define EXTRACTCONTROLDIR	BUILDCONTROLDIR

/* Set BUILDOLDPKGFORMAT to 1 to build old-format archives by default. */
#ifndef BUILDOLDPKGFORMAT
#define BUILDOLDPKGFORMAT 0
#endif

#define OLDARCHIVEVERSION	"0.939000"

#define OLDDEBDIR		"DEBIAN"
#define OLDOLDDEBDIR		".DEBIAN"

#define DEBMAGIC		"debian-binary"
#define ADMINMEMBER		"control.tar.gz"
#define DATAMEMBER		"data.tar"

#define MAXFILENAME 2048
#define MAXFIELDNAME 200

#ifdef PATH_MAX
# define INTERPRETER_MAX PATH_MAX
#else
# define INTERPRETER_MAX 1024
#endif

#endif /* DPKG_DEB_H */
