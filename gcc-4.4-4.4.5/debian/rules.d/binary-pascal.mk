arch_binaries  := $(arch_binaries) pascal
indep_binaries := $(indep_binaries) pascal-doc

p_gpc	= gpc$(pkg_ver)
p_gpcd	= gpc$(pkg_ver)-doc

d_gpc	= debian/$(p_gpc)
d_gpcd	= debian/$(p_gpcd)

dirs_gpc = \
	$(docdir)/$(p_base)/pascal \
	$(PF)/bin \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir)/{include,units} \
	$(PF)/share/man/man1
ifeq ($(with_gpidump),yes)
  files_gpc = \
	$(PF)/bin/{binobj,gpc,gpc-run,gpidump}$(pkg_ver) \
	$(PF)/share/man/man1/{binobj,gpc,gpc-run,gpidump}$(pkg_ver).1 \
	$(gcc_lexec_dir)/gpc1 \
	$(gcc_lib_dir)/{libgpc.a,units} \
	$(gcc_lib_dir)/include/gpc-in-c.h
else
  files_gpc = \
	$(PF)/bin/{binobj,gpc,gpc-run}$(pkg_ver) \
	$(PF)/share/man/man1/{binobj,gpc,gpc-run}$(pkg_ver).1 \
	$(gcc_lexec_dir)/gpc1 \
	$(gcc_lib_dir)/{libgpc.a,units} \
	$(gcc_lib_dir)/include/gpc-in-c.h
endif

# ----------------------------------------------------------------------
$(binary_stamp)-pascal: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gpc)
	dh_installdirs -p$(p_gpc) $(dirs_gpc)

	rm -f $(d)/$(PF)/bin/pc $(d)/$(PF)/share/man/man1/pc.1
	DH_COMPAT=2 dh_movefiles -p$(p_gpc) $(files_gpc)

	dh_installdocs -p$(p_gpcd) 
	dh_installchangelogs -p$(p_gpcd)

	cp -p $(srcdir)/gcc/p/{AUTHORS,FAQ,NEWS,README} \
		$(d_gpc)/$(docdir)/$(p_base)/pascal/.
	cp -p $(srcdir)/gcc/p/test/README \
		$(d_gpc)/$(docdir)/$(p_base)/pascal/README.gpc-test
	cp -p $(srcdir)/gcc/p/ChangeLog \
		$(d_gpc)/$(docdir)/$(p_base)/pascal/changelog

#	ln -sf ../$(p_gpc)/examples $(d_gpcd)/$(docdir)/$(p_gpcd)/examples
#	ln -sf ../$(p_gpc)/docdemos $(d_gpcd)/$(docdir)/$(p_gpcd)/docdemos

	dh_strip -p$(p_gpc)
	dh_compress -p$(p_gpc)
	dh_fixperms -p$(p_gpc)
	dh_shlibdeps -p$(p_gpc)
	dh_gencontrol -p$(p_gpc) -- -v$(DEB_GPC_VERSION) $(common_substvars)
	dh_installdeb -p$(p_gpc)
	dh_md5sums -p$(p_gpc)
	dh_builddeb -p$(p_gpc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-pascal-doc: $(build_html_stamp) $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gpcd)
	dh_installdirs -p$(p_gpcd) \
		$(docdir)/$(p_base)/pascal \
		$(docdir)/$(p_gpc)/pascal \
		$(PF)/share/info
	DH_COMPAT=2 dh_movefiles -p$(p_gpcd) \
		$(PF)/share/info/gpc*$(pkg_ver)*info*

	dh_installdocs -p$(p_gpcd) 
	dh_installchangelogs -p$(p_gpcd)
	cp -p html/gpc.html html/gpcs.html \
		$(d_gpcd)/$(docdir)/$(p_base)/pascal/
	mv $(d)/$(PF)/doc/gpc/demos \
		$(d_gpcd)/$(docdir)/$(p_base)/pascal/examples
	mv $(d)/$(PF)/doc/gpc/docdemos \
		$(d_gpcd)/$(docdir)/$(p_base)/pascal/.

	for i in gpc.html gpc.html docdemos examples; do \
	  ln -sf ../$(p_base)/pascal/$$i \
	    $(d_gpcd)/usr/share/doc/$(p_gpcd)/$$i; \
	  ln -sf ../$(p_base)/pascal/$$i \
	    $(d_gpcd)/usr/share/doc/$(p_gpc)/$$i; \
	done

#	-$(MAKE) -C $(builddir)/gcc gpc.ps
#	cp -p $(builddir)/gcc/gpc.ps $(d_gpcd)/$(docdir)/$(p_base)/pascal/.

	debian/dh_rmemptydirs -p$(p_gpcd)

	dh_compress -p$(p_gpcd)
	dh_fixperms -p$(p_gpcd)
	dh_installdeb -p$(p_gpcd)
	dh_gencontrol -p$(p_gpcd) -- -v$(DEB_GPC_VERSION) $(common_substvars)
	dh_md5sums -p$(p_gpcd)
	dh_builddeb -p$(p_gpcd)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
