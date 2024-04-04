$(lib_binaries)  += libvtv
ifeq ($(with_lib64vtv),yes)
  $(lib_binaries)  += lib64vtv
endif
ifeq ($(with_lib32vtv),yes)
  $(lib_binaries)	+= lib32vtv
endif
ifeq ($(with_libn32vtv),yes)
  $(lib_binaries)	+= libn32vtv
endif
ifeq ($(with_libx32vtv),yes)
  $(lib_binaries)	+= libx32vtv
endif
ifeq ($(with_libhfvtv),yes)
  $(lib_binaries)	+= libhfvtv
endif
ifeq ($(with_libsfvtv),yes)
  $(lib_binaries)	+= libsfvtv
endif

define __do_vtv
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l) $(d_d)
	dh_installdirs -p$(p_l) $(usr_lib$(2))
	DH_COMPAT=2 dh_movefiles -p$(p_l) $(usr_lib$(2))/libvtv.so.*

	debian/dh_doclink -p$(p_l) $(p_base)
	debian/dh_doclink -p$(p_d) $(p_base)

	if [ -f debian/$(p_l).overrides ]; then \
		mkdir -p debian/$(p_l)/usr/share/lintian/overrides; \
		cp debian/$(p_l).overrides debian/$(p_l)/usr/share/lintian/overrides/$(p_l); \
	fi

	dh_strip -p$(p_l) --dbg-package=$(p_d)
	dh_compress -p$(p_l) -p$(p_d)
	dh_fixperms -p$(p_l) -p$(p_d)
	$(cross_makeshlibs) dh_makeshlibs -p$(p_l)
	$(call cross_mangle_shlibs,$(p_l))
	$(ignshld)DIRNAME=$(subst n,,$(2)) $(cross_shlibdeps) dh_shlibdeps -p$(p_l) \
		$(call shlibdirs_to_search, \
			$(subst vtv$(VTV_SONAME),gcc$(GCC_SONAME),$(p_l)) \
			$(subst vtv$(VTV_SONAME),stdc++$(CXX_SONAME),$(p_l)) \
		,$(2))
	$(call cross_mangle_substvars,$(p_l))
	$(cross_gencontrol) dh_gencontrol -p$(p_l) -p$(p_d)	\
		-- -v$(DEB_VERSION) $(common_substvars)
	$(call cross_mangle_control,$(p_l))
	dh_installdeb -p$(p_l) -p$(p_d)
	dh_md5sums -p$(p_l) -p$(p_d)
	dh_builddeb -p$(p_l) -p$(p_d)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
endef

# ----------------------------------------------------------------------

do_vtv = $(call __do_vtv,lib$(1)vtv$(VTV_SONAME),$(1))

$(binary_stamp)-libvtv: $(install_stamp)
	$(call do_vtv,)

$(binary_stamp)-lib64vtv: $(install_stamp)
	$(call do_vtv,64)

$(binary_stamp)-lib32vtv: $(install_stamp)
	$(call do_vtv,32)

$(binary_stamp)-libn32vtv: $(install_stamp)
	$(call do_vtv,n32)

$(binary_stamp)-libx32vtv: $(install_stamp)
	$(call do_vtv,x32)

$(binary_stamp)-libhfvtv: $(install_dependencies)
	$(call do_vtv,hf)

$(binary_stamp)-libsfvtv: $(install_dependencies)
	$(call do_vtv,sf)
