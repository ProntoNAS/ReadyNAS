GLIBC_PASSES += nptl s390x
DEB_ARCH_REGULAR_PACKAGES += libc6-s390x libc6-dev-s390x

s390x_MIN_KERNEL_SUPPORTED = 2.4.1
s390x_configure_target = s390x-linux
s390x_CC = $(CC) -m64
s390x_add-ons = linuxthreads $(add-ons)
libc6-s390x_shlib_dep = libc6-s390x (>= $(shlib_dep_ver))
s390x_extra_cflags = -g1 -O3
s390x_LIBDIR = 64
s390x_extra_config_options := $(extra_config_options) --disable-profile

define libc6-s390x_extra_pkg_install
mkdir -p debian/$(curpass)/lib
ln -s /lib64/ld64.so.1 debian/$(curpass)/lib
endef
