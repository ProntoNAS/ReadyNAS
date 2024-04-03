GLIBC_PASSES += nptl i686
DEB_ARCH_REGULAR_PACKAGES += libc6-i686

# NPTL requires at least i486 assembly.  We don't need to take
# special measures for i386 systems, since Debian kernel images now
# emulate the missing instructions on the i386.
nptl_configure_target=i486-linux
nptl_configure_build=i486-linux
nptl_extra_cflags = -march=i486 -mcpu=i686 -g1 -O3

# We use -march=i686 and glibc's i686 routines use cmov, so require it.
# A Debian-local glibc patch adds cmov to the search path.
# The optimized libraries also use NPTL!
i686_add-ons = nptl $(add-ons)
i686_configure_target=i686-linux
i686_configure_build=i686-linux
i686_extra_cflags = -march=i686 -mcpu=i686 -g1 -O3
i686_LIBDIR = /tls/i686/cmov
i686_MIN_KERNEL_SUPPORTED = 2.6.0
i686_extra_config_options = $(extra_config_options) --disable-profile --with-tls --with-__thread

libc_extra_config_options = $(extra_config_options) --with-tls --without-__thread
