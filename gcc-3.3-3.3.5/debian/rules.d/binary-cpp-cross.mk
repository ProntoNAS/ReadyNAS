arch_binaries  := $(arch_binaries) cpp

dirs_cpp = \
	$(docdir) \
	$(PF)/share/man/man1 \
	$(PF)/bin \
	$(gcc_lexec_dir)

files_cpp = \
	$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-cpp$(pkg_ver) \
	$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-cpp$(pkg_ver).1 \
	$(gcc_lexec_dir)/cc1

# ----------------------------------------------------------------------
$(binary_stamp)-cpp: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_cpp)
	dh_installdirs -p$(p_cpp) $(dirs_cpp)
	dh_movefiles -p$(p_cpp) $(files_cpp)

	debian/dh_doclink -p$(p_cpp) $(p_base)
	debian/dh_rmemptydirs -p$(p_cpp)

	dh_strip -p$(p_cpp)
	dh_compress -p$(p_cpp)
	dh_fixperms -p$(p_cpp)
	dh_shlibdeps -p$(p_cpp)
	dh_gencontrol -p$(p_cpp) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_cpp)
	sed 's/cross-/$(TP)/g;s/-ver/$(pkg_ver)/g;s/gcc/cpp/g' < debian/gcc-cross.postinst > debian/$(p_cpp)/DEBIAN/postinst
	sed 's/cross-/$(TP)/g;s/-ver/$(pkg_ver)/g;s/gcc/cpp/g' < debian/gcc-cross.prerm > debian/$(p_cpp)/DEBIAN/prerm
	chmod 755 debian/$(p_cpp)/DEBIAN/{postinst,prerm}
	dh_md5sums -p$(p_cpp)
	dh_builddeb -p$(p_cpp)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

