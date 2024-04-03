man_MANS = nfs4_uid_to_name.3
lib_LTLIBRARIES = libnfsidmap.la
libnfsidmap_la_SOURCES = libnfsidmap.c nss.c umich_ldap.c cfg.c strlcpy.c cfg.h nfsidmap_internal.h queue.h
libnfsidmap_la_LDFLAGS = -version-info 2:0:2
include_HEADERS = nfsidmap.h
EXTRA_DIST = $(man_MANS) libtest.c idmapd.conf

#	XXX: also exclude debian/files and debian/files.new ? do a clean??
dist-hook:
	mkdir $(distdir)/debian/
	find $(srcdir)/debian -maxdepth 1 -not -type d |xargs -i cp {} $(distdir)/debian/

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libnfsidmap.pc

