arch_binaries  := $(arch_binaries) cxx

dirs_cxx = \
	$(docdir) \
	$(PF)/bin \
	$(gcc_lib_dir) \
	$(PF)/share/man/man1
files_cxx = \
	$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-g++$(pkg_ver) \
	$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-g++$(pkg_ver).1 \
	$(gcc_lib_dir)/cc1plus

# ----------------------------------------------------------------------
$(binary_stamp)-cxx: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_cxx)
	dh_installdirs -p$(p_cxx) $(dirs_cxx)
	dh_movefiles -p$(p_cxx) $(files_cxx)
# g++ man page is a .so link
	rm -f $(d_cxx)/$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-g++$(pkg_ver).1
	ln -sf $(DEB_TARGET_GNU_TYPE)-gcc$(pkg_ver).1.gz \
		$(d_cxx)/$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-g++$(pkg_ver).1.gz

	debian/dh_doclink -p$(p_cxx) $(p_base)
	debian/dh_rmemptydirs -p$(p_cxx)

	dh_strip -p$(p_cxx)
	dh_compress -p$(p_cxx)
	dh_fixperms -p$(p_cxx)
	dh_shlibdeps -p$(p_cxx)
	dh_gencontrol -p$(p_cxx) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_cxx)
	sed 's/cross-/$(TP)/g;s/-ver/$(pkg_ver)/g;s/gcc/g++/g' < debian/gcc-cross.postinst > debian/$(p_cxx)/DEBIAN/postinst
	sed 's/cross-/$(TP)/g;s/-ver/$(pkg_ver)/g;s/gcc/g++/g' < debian/gcc-cross.prerm > debian/$(p_cxx)/DEBIAN/prerm
	chmod 755 debian/$(p_cxx)/DEBIAN/{postinst,prerm}
	dh_md5sums -p$(p_cxx)
	dh_builddeb -p$(p_cxx)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
