arch_binaries  := $(arch_binaries) fastjar

p_jar	= fastjar
d_jar	= debian/$(p_jar)

dirs_jar = \
	$(docdir)/$(p_jar) \
	$(PF)/bin \
	$(PF)/share/man/man1 \
	$(PF)/share/info

files_jar = \
	$(PF)/bin/{fastjar,grepjar} \
	$(PF)/share/man/man1/{fastjar,grepjar}.1 \
	$(PF)/share/info/fastjar.info

# ----------------------------------------------------------------------
$(binary_stamp)-fastjar: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_jar)
	dh_installdirs -p$(p_jar)  $(dirs_jar)

	mv $(d)/$(PF)/bin/jar $(d)/$(PF)/bin/fastjar
	mv $(d)/$(PF)/share/man/man1/jar.1 $(d)/$(PF)/share/man/man1/fastjar.1

	dh_movefiles -p$(p_jar)  $(files_jar)

	dh_installdocs -p$(p_jar) $(srcdir)/fastjar/{CHANGES,NEWS,README}
	dh_installchangelogs -p$(p_jar) $(srcdir)/fastjar/ChangeLog

	debian/dh_rmemptydirs -p$(p_jar)

	dh_strip -p$(p_jar)
	dh_compress -p$(p_jar)
	dh_fixperms -p$(p_jar)
	dh_shlibdeps -p$(p_jar)
	dh_gencontrol -p$(p_jar) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_jar)
	dh_md5sums -p$(p_jar)
	dh_builddeb -p$(p_jar)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
