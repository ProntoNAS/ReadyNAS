arch_binaries := $(arch_binaries) gdc

ifeq ($(with_libphobos),yes)
  arch_binaries += libphobos
endif

p_gdc		= gdc$(pkg_ver)$(cross_bin_arch)
p_libphobos	= libphobos$(libphobos_version)$(pkg_ver)$(cross_bin_arch)-dev

d_gdc		= debian/$(p_gdc)
d_libphobos	= debian/$(p_libphobos)

