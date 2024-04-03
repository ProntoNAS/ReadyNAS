# Makefile for 4.4BSD db code in GNU C library.
# This code is taken verbatim from the BSD db 1.85 package.  Only this
# Makefile and compat.h were written for GNU libc, and the header files
# moved up to this directory.

subdir = db

subdir-dirs = btree db hash mpool recno
vpath %.c $(subdir-dirs)

extra-libs := libdb1
extra-libs-others := $(extra-libs)
libdb1-routines := bt_close bt_conv bt_debug bt_delete bt_get \
		   bt_open bt_overflow bt_page bt_put bt_search \
		   bt_seq bt_split bt_utils \
		   db \
		   hash hash_bigkey hash_buf hash_func hash_log2 hash_page \
		   ndbm \
		   mpool \
		   rec_close rec_delete rec_get rec_open rec_put rec_search \
		   rec_seq rec_utils

CFLAGS += -O2
CPPFLAGS += -I. -D_REENTRANT -D__DBINTERFACE_PRIVATE -DUSE_LIBDB1
CPPFLAGS += -D_GNU_SOURCE
libdb.so-version=.2

all: libdb1.so$(libdb.so-version)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -g -o $@ $<
%.os: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -fPIC -o $@ $<

libdb1.so$(libdb.so-version): $(patsubst %,%.os,$(libdb1-routines))
	$(CC) -Wl,-O1 -Wl,--version-script=Versions -Wl,-z,-combreloc \
	      -Wl,-soname=libdb.so$(libdb.so-version) \
	      -shared -o $@ $^ /usr/lib/libc.so

clean:
	rm -f $(patsubst %,%.o,$(libdb1-routines))
	rm -f $(patsubst %,%.os,$(libdb1-routines))
	rm -f libdb1.so$(libdb.so-version) *~
