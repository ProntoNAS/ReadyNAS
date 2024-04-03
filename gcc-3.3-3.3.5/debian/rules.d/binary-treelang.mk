arch_binaries := $(arch_binaries) treelang

p_tree	= treelang$(pkg_ver)
d_tree	= debian/$(p_tree)

dirs_tree = \
	$(docdir)/$(p_base)/treelang \
	$(gcc_lexec_dir) \
	$(PF)/share/info

files_tree = \
	$(gcc_lexec_dir)/tree1

ifneq ($(GFDL_INVARIANT_FREE),yes)
  files_tree += \
	$(PF)/share/info/treelang*
endif

$(binary_stamp)-treelang: $(build_html_stamp) $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_tree)
	dh_installdirs -p$(p_tree) $(dirs_tree)
	dh_movefiles -p$(p_tree) $(files_tree)

	debian/dh_doclink -p$(p_tree) $(p_base)
ifneq ($(GFDL_INVARIANT_FREE),yes)
	dh_installdocs -p$(p_tree)
	rm -f $(d_tree)/$(docdir)/$(p_base)/copyright
	cp -p html/treelang.html $(d_tree)/$(docdir)/$(p_base)/treelang/
endif
	cp -p $(srcdir)/gcc/treelang/README \
		$(d_tree)/$(docdir)/$(p_base)/treelang/.
	cp -p $(srcdir)/gcc/treelang/ChangeLog \
		$(d_tree)/$(docdir)/$(p_base)/treelang/changelog
	cp -p debian/README.treelang \
		$(d_tree)/$(docdir)/$(p_base)/treelang/README.Debian

	debian/dh_rmemptydirs -p$(p_tree)

	dh_strip -p$(p_tree)
	dh_compress -p$(p_tree)

	dh_fixperms -p$(p_tree)
	dh_shlibdeps -p$(p_tree)
	dh_gencontrol -p$(p_tree) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_tree)
	dh_md5sums -p$(p_tree)
	dh_builddeb -p$(p_tree)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
