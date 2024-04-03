arch_binaries  := $(arch_binaries) nof

p_nof  = gcc$(pkg_arch)-nof
d_nof  = debian/$(p_nof)

dirs_nof = \
	$(docdir) \
	$(PF)/$(DEB_TARGET_GNU_TYPE)/lib/nof
ifeq ($(with_cdev),yes)
  dirs_nof += \
	$(gcc_lib_dir)/nof
endif

ifeq ($(with_cdev),yes)
  files_nof = \
	$(PF)/$(DEB_TARGET_GNU_TYPE)/lib/libgcc_s_nof.so.$(GCC_SONAME) \
	$(gcc_lib_dir)/libgcc_s_nof.so \
	$(PF)/$(DEB_TARGET_GNU_TYPE)/lib/nof \
	$(gcc_lib_dir)/nof
else
  files_nof = \
	lib/libgcc_s_nof.so.$(GCC_SONAME)
endif

# ----------------------------------------------------------------------
$(binary_stamp)-nof: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	ln -sf /$(PF)/$(DEB_TARGET_GNU_TYPE)/lib/libgcc_s_nof.so.$(GCC_SONAME) \
		$(d)/$(gcc_lib_dir)/libgcc_s_nof.so

	rm -rf $(d_nof)
	dh_installdirs -p$(p_nof) $(dirs_nof)
	dh_movefiles -p$(p_nof) $(files_nof)
	debian/dh_doclink -p$(p_nof) $(p_base)
	dh_strip -p$(p_nof)
	dh_compress -p$(p_nof)
	dh_fixperms -p$(p_nof)
	dh_gencontrol -p$(p_nof) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_nof)
	dh_md5sums -p$(p_nof)
	dh_builddeb -p$(p_nof)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
