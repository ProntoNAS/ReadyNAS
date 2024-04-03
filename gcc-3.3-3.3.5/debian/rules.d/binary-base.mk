arch_binaries := base $(arch_binaries)

# ---------------------------------------------------------------------------
# gcc-base

$(binary_stamp)-base: $(install_dependencies)
	dh_testdir
	dh_testroot
	rm -rf $(d_base)
	dh_installdirs -p$(p_base)
	dh_installdocs -p$(p_base)
ifeq ($(with_base_only),yes)
	dh_installchangelogs -p$(p_base)
else
	dh_installchangelogs -p$(p_base) $(srcdir)/ChangeLog
endif
	dh_compress -p$(p_base)
	dh_fixperms -p$(p_base)
	dh_gencontrol -p$(p_base) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_base)
	dh_md5sums -p$(p_base)
	dh_builddeb -p$(p_base)
	touch $@
