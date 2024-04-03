ifeq ($(with_objcdev),yes)
  $(lib_binaries) += libobjc
endif
ifeq ($(with_lib64objc),yes)
  $(lib_binaries)  += lib64objc
endif
ifeq ($(with_lib32objc),yes)
  $(lib_binaries)	+= lib32objc
endif
ifeq ($(with_libn32objc),yes)
  $(lib_binaries)	+= libn32objc
endif

files_lobjc = \
	$(2)/libobjc.so.*
ifeq ($(with_objc_gc),yes)
  files_lobjc += \
	$(2)/libobjc_gc.so.*
endif

define __do_libobjc
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l) $(d_d)
	dh_installdirs -p$(p_l) \
		$(2)
	DH_COMPAT=2 dh_movefiles -p$(p_l) \
		$(files_lobjc)

	debian/dh_doclink -p$(p_l) $(p_base)
	debian/dh_doclink -p$(p_d) $(p_base)

	dh_strip -p$(p_l) --dbg-package=$(p_d)
	dh_compress -p$(p_l) -p$(p_d)
	dh_fixperms -p$(p_l) -p$(p_d)
	dh_makeshlibs -p$(p_l) -Xlibobjc_gc.so
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

# ----------------------------------------------------------------------

do_libobjc = $(call __do_libobjc,lib$(1)objc$(OBJC_SONAME),$(usr_lib$(1)))

$(binary_stamp)-libobjc: $(install_stamp)
	$(call do_libobjc,)

$(binary_stamp)-lib64objc: $(install_stamp)
	$(call do_libobjc,64)

$(binary_stamp)-lib32objc: $(install_stamp)
	$(call do_libobjc,32)

$(binary_stamp)-libn32objc: $(install_stamp)
	$(call do_libobjc,n32)
