ifneq (,$(filter yes, $(biarch64) $(biarch32) $(biarchn32)))
  arch_binaries  := $(arch_binaries) objc-multi
endif
arch_binaries := $(arch_binaries) objc

p_objc	= gobjc$(pkg_ver)$(cross_bin_arch)
d_objc	= debian/$(p_objc)

p_objc_m= gobjc$(pkg_ver)-multilib$(cross_bin_arch)
d_objc_m= debian/$(p_objc_m)

dirs_objc = \
	$(docdir)/$(p_base)/ObjC \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir)/include

files_objc = \
	$(gcc_lexec_dir)/cc1obj \
	$(gcc_lib_dir)/include/objc

define do_objc
	dh_installdirs -p$(2) $(gcc_lib_dir$(1))
	$(call install_gcc_lib,libobjc,$(OBJC_SONAME),$(1),$(2))
	$(if $(filter yes,$(with_objc_gc)),
		dh_link -p$(2) \
		  /$(usr_lib$(1))/libobjc_gc.so.$(OBJC_SONAME) \
		  /$(gcc_lib_dir$(1))/libobjc_gc.so
	)

endef

$(binary_stamp)-objc: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_objc)
	dh_installdirs -p$(p_objc) $(dirs_objc)
	DH_COMPAT=2 dh_movefiles -p$(p_objc) $(files_objc)

	$(call do_objc,,$(p_objc))

	cp -p $(srcdir)/libobjc/{README*,THREADS*} \
		$(d_objc)/$(docdir)/$(p_base)/ObjC/.

	cp -p $(srcdir)/libobjc/ChangeLog \
		$(d_objc)/$(docdir)/$(p_base)/ObjC/changelog.libobjc

	debian/dh_doclink -p$(p_objc) $(p_base)

	debian/dh_rmemptydirs -p$(p_objc)

	dh_strip -p$(p_objc)
	dh_compress -p$(p_objc)

	dh_fixperms -p$(p_objc)
	dh_shlibdeps -p$(p_objc)
	dh_gencontrol -p$(p_objc) -- -v$(DEB_VERSION) $(common_substvars)
	dh_installdeb -p$(p_objc)
	dh_md5sums -p$(p_objc)
	dh_builddeb -p$(p_objc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

$(binary_stamp)-objc-multi: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_objc_m)
	dh_installdirs -p$(p_objc_m) $(docdir)

	$(foreach flavour,$(flavours), \
		$(call do_objc,$(flavour),$(p_objc_m)))

	debian/dh_doclink -p$(p_objc_m) $(p_base)

	dh_strip -p$(p_objc_m)
	dh_compress -p$(p_objc_m)

	dh_fixperms -p$(p_objc_m)
	dh_shlibdeps -p$(p_objc_m)
	dh_gencontrol -p$(p_objc_m) -- -v$(DEB_VERSION) $(common_substvars)
	dh_installdeb -p$(p_objc_m)
	dh_md5sums -p$(p_objc_m)
	dh_builddeb -p$(p_objc_m)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
