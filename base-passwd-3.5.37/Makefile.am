SUBDIRS = doc man

sbin_PROGRAMS = update-passwd

update_passwd_SOURCES = update-passwd.c
update_passwd_LDADD = -ldebconfclient

pkgdata_DATA = passwd.master group.master

dist_doc_DATA = README
