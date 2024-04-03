ifeq ($(with_libgcc),yes)
  $(lib_binaries)	+= libgcc

  ifeq ($(with_lib64gcc),yes)
    $(lib_binaries)	+= lib64gcc
  endif
  ifeq ($(biarch32),yes)
    $(lib_binaries)	+= lib32gcc
  endif
  ifeq ($(biarchn32),yes)
    $(lib_binaries)	+= libn32gcc
  endif
endif

p_lgcc		= libgcc$(GCC_SONAME)$(cross_lib_arch)
p_lgccdbg	= libgcc$(GCC_SONAME)-dbg$(cross_lib_arch)
d_lgcc		= debian/$(p_lgcc)
d_lgccdbg	= debian/$(p_lgccdbg)

p_l32gcc	= lib32gcc$(GCC_SONAME)$(cross_lib_arch)
p_l32gccdbg	= lib32gcc$(GCC_SONAME)-dbg$(cross_lib_arch)
d_l32gcc	= debian/$(p_l32gcc)
d_l32gccdbg	= debian/$(p_l32gccdbg)

p_l64gcc	= lib64gcc$(GCC_SONAME)$(cross_lib_arch)
p_l64gccdbg	= lib64gcc$(GCC_SONAME)-dbg$(cross_lib_arch)
d_l64gcc	= debian/$(p_l64gcc)
d_l64gccdbg	= debian/$(p_l64gccdbg)

p_ln32gcc	= libn32gcc$(GCC_SONAME)$(cross_lib_arch)
p_ln32gccdbg	= libn32gcc$(GCC_SONAME)-dbg$(cross_lib_arch)
d_ln32gcc	= debian/$(p_ln32gcc)
d_ln32gccdbg	= debian/$(p_ln32gccdbg)

define __do_libgcc
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l) $(d_d)

	dh_installdirs -p$(p_l) \
		$(docdir)/$(p_l) \
		$(libgcc_dir$(2))
	$(if $(filter yes,$(with_shared_libgcc)),
		mv $(d)/$(usr_lib$(2))/libgcc_s.so.$(GCC_SONAME) \
			$(d_l)/$(libgcc_dir$(2))/.
	)

	debian/dh_doclink -p$(p_l) $(if $(3),$(3),$(p_base))
	debian/dh_doclink -p$(p_d) $(if $(3),$(3),$(p_base))
	debian/dh_rmemptydirs -p$(p_l)
	debian/dh_rmemptydirs -p$(p_d)
	dh_strip -p$(p_l) --dbg-package=$(p_d)

	$(if $(filter yes,$(with_shared_libgcc)),
		dh_makeshlibs -p$(p_l) -p$(p_d) \
			-- -v$(DEB_LIBGCC_VERSION)
		$(call cross_mangle_shlibs,$(p_l))
		$(if $(filter $(DEB_TARGET_ARCH),armel),
			grep -q '^ __aeabi' $(d_l)/DEBIAN/symbols \
			  || cat debian/libgcc1.symbols.aeabi \
				>> $(d_l)/DEBIAN/symbols
		)
	)

	$(cross_shlibdeps) dh_shlibdeps -p$(p_l)
	$(call cross_mangle_substvars,$(p_l))

	dh_compress -p$(p_l) -p$(p_d)
	dh_fixperms -p$(p_l) -p$(p_d)
	dh_gencontrol -p$(p_l) -p$(p_d) \
		-- -v$(DEB_LIBGCC_VERSION) $(common_substvars)
	$(call cross_mangle_control,$(p_l))

	$(if $(2),,	# only for native
		mkdir -p $(d_l)/usr/share/lintian/overrides
		echo '$(p_l): package-name-doesnt-match-sonames' \
			> $(d_l)/usr/share/lintian/overrides/$(p_l)
	)

	dh_installdeb -p$(p_l) -p$(p_d)
	dh_md5sums -p$(p_l) -p$(p_d)
	dh_builddeb -p$(p_l) -p$(p_d)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
endef

do_libgcc = $(call __do_libgcc,lib$(1)gcc$(GCC_SONAME),$(1),$(2))
# ----------------------------------------------------------------------

$(binary_stamp)-libgcc: $(install_dependencies)
ifeq ($(with_standalone_gcj),yes)
	$(call do_libgcc,,$(p_jbase))
else
	$(call do_libgcc,,)
endif

$(binary_stamp)-lib64gcc: $(install_dependencies)
	$(call do_libgcc,64,)

$(binary_stamp)-lib32gcc: $(install_dependencies)
	$(call do_libgcc,32,)

$(binary_stamp)-libn32gcc: $(install_dependencies)
	$(call do_libgcc,n32,)
