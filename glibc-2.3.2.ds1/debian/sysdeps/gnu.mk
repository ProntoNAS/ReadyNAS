# This is for the GNU OS.  Commonly known as the Hurd.

# FIXME: Have to do both nptl and linuxthreads for now, because of the
# patch system.  Shouldn't need either of these.
GLIBC_OVERLAYS ?= $(shell ls nptl* glibc-linuxthreads*)

threads = no
libc = libc0.3
slibdir = /lib

# Glibc should really do this for us.
define libc_extra_install
mkdir -p debian/tmp-$(curpass)/lib
ln -s ld.so.1 debian/tmp-$(curpass)/lib/ld.so
endef

# Do not care about kernel versions for now.
define kernel_check
true
endef

