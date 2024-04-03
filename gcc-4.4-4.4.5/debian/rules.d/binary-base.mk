$(lib_binaries) += base

# ---------------------------------------------------------------------------
# gcc-base

$(binary_stamp)-base: $(install_dependencies)
	dh_testdir
	dh_testroot
	rm -rf $(d_base)
	dh_installdirs -p$(p_base) \
		$(gcc_lexec_dir)

ifeq ($(with_common_gcclibdir),yes)
	ln -sf $(BASE_VERSION) \
	    $(d_base)/$(subst /$(BASE_VERSION),/$(GCC_VERSION),$(gcc_lib_dir))
	ln -sf $(BASE_VERSION) \
	    $(d_base)/$(subst /$(BASE_VERSION),/4.4.4,$(gcc_lib_dir))
  ifneq ($(gcc_lib_dir),$(gcc_lexec_dir))
	ln -sf $(BASE_VERSION) \
	    $(d_base)/$(subst /$(BASE_VERSION),/$(GCC_VERSION),$(gcc_lexec_dir))
	ln -sf $(BASE_VERSION) \
	    $(d_base)/$(subst /$(BASE_VERSION),/4.4.4,$(gcc_lexec_dir))
  endif
endif

ifeq ($(with_spu),yes)
	mkdir -p $(d_base)/$(gcc_spu_lexec_dir)
	mkdir -p $(d_base)/$(gcc_spu_lib_dir)
	ln -sf $(BASE_VERSION) $(d_base)/$(spulibexecdir)/gcc/spu/$(GCC_VERSION)
	ln -sf $(BASE_VERSION) $(d_base)/usr/spu/lib/gcc/spu/$(GCC_VERSION)
endif

	dh_installdocs -p$(p_base)
	dh_installchangelogs -p$(p_base)
	dh_compress -p$(p_base)
	dh_fixperms -p$(p_base)
	dh_gencontrol -p$(p_base) -- -v$(DEB_VERSION) $(common_substvars)
	dh_installdeb -p$(p_base)
	dh_md5sums -p$(p_base)
	dh_builddeb -p$(p_base)
	touch $@
