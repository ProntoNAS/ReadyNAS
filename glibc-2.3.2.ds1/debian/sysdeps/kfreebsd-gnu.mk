# This is for a Glibc-using FreeBSD system.

# FIXME: Have to do both nptl and linuxthreads for now, because of the
# patch system.  Should be only linuxthreads.
GLIBC_OVERLAYS ?= $(shell ls nptl* glibc-linuxthreads*)

libc = libc1

# Linuxthreads Config
threads = yes
libc_add-ons = linuxthreads $(add-ons)

extra_config_options = $(extra_config_options) --disable-compatible-utmp --enable-kernel-include=4.6
