ifeq ($(with_libgfortran),yes)
  $(lib_binaries) += libgfortran
endif
ifeq ($(with_lib64gfortran),yes)
  $(lib_binaries) += lib64fortran
endif
ifeq ($(with_lib32gfortran),yes)
  $(lib_binaries) += lib32fortran
endif
ifeq ($(with_libn32gfortran),yes)
  $(lib_binaries) += libn32fortran
endif

ifeq ($(with_fdev),yes)
  ifneq (,$(filter yes, $(biarch64) $(biarch32) $(biarchn32)))
    arch_binaries  := $(arch_binaries) fdev-multi
  endif
  arch_binaries  := $(arch_binaries) fdev
  ifneq ($(DEB_CROSS),yes)
    ifneq ($(GFDL_INVARIANT_FREE),yes)
      indep_binaries := $(indep_binaries) fortran-doc
    endif
  endif
endif

p_g95	= gfortran$(pkg_ver)$(cross_bin_arch)
p_g95_m	= gfortran$(pkg_ver)-multilib$(cross_bin_arch)
p_g95d	= gfortran$(pkg_ver)-doc
p_flib	= libgfortran$(FORTRAN_SONAME)$(cross_lib_arch)

d_g95	= debian/$(p_g95)
d_g95_m	= debian/$(p_g95_m)
d_g95d	= debian/$(p_g95d)

dirs_g95 = \
	$(docdir)/$(p_base)/fortran \
	$(PF)/bin \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir) \
	$(PF)/include \
	$(PF)/share/man/man1
files_g95 = \
	$(PF)/bin/$(cmd_prefix)gfortran$(pkg_ver) \
	$(gcc_lib_dir)/finclude \
	$(gcc_lexec_dir)/f951 

ifneq ($(GFDL_INVARIANT_FREE),yes)
  files_g95 += \
	$(PF)/share/man/man1/$(cmd_prefix)gfortran$(pkg_ver).1
endif

# ----------------------------------------------------------------------
define __do_fortran
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l) $(d_d)
	dh_installdirs -p$(p_l) $(2)
	DH_COMPAT=2 dh_movefiles -p$(p_l) $(2)/libgfortran.so.*

	debian/dh_doclink -p$(p_l) $(p_base)
	debian/dh_doclink -p$(p_d) $(p_base)

	dh_strip -p$(p_l) --dbg-package=$(p_d)
	dh_compress -p$(p_l) -p$(p_d)
	dh_fixperms -p$(p_l) -p$(p_d)
	dh_makeshlibs -p$(p_l)
	$(call cross_mangle_shlibs,$(p_l))
	$(cross_shlibdeps) dh_shlibdeps -p$(p_l)
	$(call cross_mangle_substvars,$(p_l))
	dh_gencontrol -p$(p_l) -p$(p_d) \
		-- -v$(DEB_VERSION) $(common_substvars)
	$(call cross_mangle_control,$(p_l))
	dh_installdeb -p$(p_l) -p$(p_d)
	dh_md5sums -p$(p_l) -p$(p_d)
	dh_builddeb -p$(p_l) -p$(p_d)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
endef

do_fortran = $(call __do_fortran,lib$(1)gfortran$(FORTRAN_SONAME),$(usr_lib$(1)))

define do_fortran_dev
	dh_installdirs -p$(2) $(gcc_lib_dir$(1))
	DH_COMPAT=2 dh_movefiles -p$(2) \
		$(gcc_lib_dir$(1))/libgfortranbegin.a
	$(call install_gcc_lib,libgfortran,$(FORTRAN_SONAME),$(1),$(2))
endef
# ----------------------------------------------------------------------
$(binary_stamp)-libgfortran: $(install_stamp)
	$(call do_fortran,)

$(binary_stamp)-lib64fortran: $(install_stamp)
	$(call do_fortran,64)

$(binary_stamp)-lib32fortran: $(install_stamp)
	$(call do_fortran,32)

$(binary_stamp)-libn32fortran: $(install_stamp)
	$(call do_fortran,n32)

# ----------------------------------------------------------------------
$(binary_stamp)-fdev: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_g95)
	dh_installdirs -p$(p_g95) $(dirs_g95)

	DH_COMPAT=2 dh_movefiles -p$(p_g95) $(files_g95)

	$(call do_fortran_dev,,$(p_g95))

ifneq ($(DEB_CROSS),yes)
	ln -sf gfortran$(pkg_ver) \
	    $(d_g95)/$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-gfortran$(pkg_ver)
	ln -sf gfortran$(pkg_ver) \
	    $(d_g95)/$(PF)/bin/$(TARGET_ALIAS)-gfortran$(pkg_ver)
ifneq ($(GFDL_INVARIANT_FREE),yes)
	ln -sf gfortran$(pkg_ver).1 \
	    $(d_g95)/$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-gfortran$(pkg_ver).1
	ln -sf gfortran$(pkg_ver).1 \
	    $(d_g95)/$(PF)/share/man/man1/$(TARGET_ALIAS)-gfortran$(pkg_ver).1
endif
endif

	debian/dh_doclink -p$(p_g95) $(p_base)

	cp -p $(srcdir)/gcc/fortran/ChangeLog \
		$(d_g95)/$(docdir)/$(p_base)/fortran/changelog
	debian/dh_rmemptydirs -p$(p_g95)

	dh_strip -p$(p_g95)
	dh_compress -p$(p_g95)
	dh_fixperms -p$(p_g95)
	dh_shlibdeps -p$(p_g95)
	dh_gencontrol -p$(p_g95) -- -v$(DEB_VERSION) $(common_substvars)
	dh_installdeb -p$(p_g95)
	dh_md5sums -p$(p_g95)
	dh_builddeb -p$(p_g95)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-fdev-multi: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_g95_m)
	dh_installdirs -p$(p_g95_m) $(docdir)

	$(foreach flavour,$(flavours), \
		$(call do_fortran_dev,$(flavour),$(p_g95_m)))

	debian/dh_doclink -p$(p_g95_m) $(p_base)
	debian/dh_rmemptydirs -p$(p_g95_m)
	dh_strip -p$(p_g95_m)
	dh_compress -p$(p_g95_m)
	dh_fixperms -p$(p_g95_m)
	dh_shlibdeps -p$(p_g95_m)
	dh_gencontrol -p$(p_g95_m) -- -v$(DEB_VERSION) $(common_substvars)
	dh_installdeb -p$(p_g95_m)
	dh_md5sums -p$(p_g95_m)
	dh_builddeb -p$(p_g95_m)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-fortran-doc: $(build_html_stamp) $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_g95d)
	dh_installdirs -p$(p_g95d) \
		$(docdir)/$(p_base)/fortran \
		$(PF)/share/info
	DH_COMPAT=2 dh_movefiles -p$(p_g95d) \
		$(PF)/share/info/gfortran*

	debian/dh_doclink -p$(p_g95d) $(p_base)
ifneq ($(GFDL_INVARIANT_FREE),yes)
	dh_installdocs -p$(p_g95d)
	rm -f $(d_g95d)/$(docdir)/$(p_base)/copyright
	cp -p html/gfortran.html $(d_g95d)/$(docdir)/$(p_base)/fortran/
endif

	dh_compress -p$(p_g95d)
	dh_fixperms -p$(p_g95d)
	dh_installdeb -p$(p_g95d)
	dh_gencontrol -p$(p_g95d) -- -v$(DEB_VERSION) $(common_substvars)
	dh_md5sums -p$(p_g95d)
	dh_builddeb -p$(p_g95d)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
