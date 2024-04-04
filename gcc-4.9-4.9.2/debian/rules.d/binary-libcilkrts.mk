$(lib_binaries)  += libcilkrts
ifeq ($(with_lib64cilkrts),yes)
  $(lib_binaries)  += lib64cilkrts
endif
ifeq ($(with_lib32cilkrts),yes)
  $(lib_binaries)	+= lib32cilkrts
endif
ifeq ($(with_libn32cilkrts),yes)
  $(lib_binaries)	+= libn32cilkrts
endif
ifeq ($(with_libx32cilkrts),yes)
  $(lib_binaries)	+= libx32cilkrts
endif
ifeq ($(with_libhfcilkrts),yes)
  $(lib_binaries)	+= libhfcilkrts
endif
ifeq ($(with_libsfcilkrts),yes)
  $(lib_binaries)	+= libsfcilkrts
endif

define __do_cilkrts
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l) $(d_d)
	dh_installdirs -p$(p_l) $(usr_lib$(2))
	DH_COMPAT=2 dh_movefiles -p$(p_l) $(usr_lib$(2))/libcilkrts.so.*

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
			$(subst cilkrts$(CILKRTS_SONAME),gcc$(GCC_SONAME),$(p_l)) \
			$(subst cilkrts$(CILKRTS_SONAME),stdc++$(CXX_SONAME),$(p_l)) \
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

do_cilkrts = $(call __do_cilkrts,lib$(1)cilkrts$(CILKRTS_SONAME),$(1))

$(binary_stamp)-libcilkrts: $(install_stamp)
	$(call do_cilkrts,)

$(binary_stamp)-lib64cilkrts: $(install_stamp)
	$(call do_cilkrts,64)

$(binary_stamp)-lib32cilkrts: $(install_stamp)
	$(call do_cilkrts,32)

$(binary_stamp)-libn32cilkrts: $(install_stamp)
	$(call do_cilkrts,n32)

$(binary_stamp)-libx32cilkrts: $(install_stamp)
	$(call do_cilkrts,x32)

$(binary_stamp)-libhfcilkrts: $(install_dependencies)
	$(call do_cilkrts,hf)

$(binary_stamp)-libsfcilkrts: $(install_dependencies)
	$(call do_cilkrts,sf)
