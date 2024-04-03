ifeq ($(with_objcdev),yes)
  arch_binaries := $(arch_binaries) libobjc
endif

p_lobjc	= libobjc$(OBJC_SONAME)
d_lobjc	= debian/$(p_lobjc)

dirs_lobjc = \
	$(docdir)/objc \
	$(PF)/$(libdir)
files_lobjc = \
	$(PF)/$(libdir)/libobjc.so.*
ifeq ($(with_objc_gc),yes)
  files_lobjc += \
	$(PF)/$(libdir)/libobjc_gc.so.*
endif

ifeq ($(with_lib64objc),yes)
	dirs_lobjc   +=	$(PF)/$(lib64)
	files_lobjc  +=	$(PF)/$(lib64)/libobjc.so.*
  ifeq ($(with_objc_gc),yes)
	files_lobjc  +=	$(PF)/$(lib64)/libobjc_gc.so.*
  endif
endif


$(binary_stamp)-libobjc: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_lobjc)
	dh_installdirs -p$(p_lobjc) $(dirs_lobjc)
#	mv $(d)/$(gcc_lib_dir)/libobjc.so.* $(d)/$(PF)/$(libdir)/.
#ifeq ($(with_objc_gc),yes)
#	mv $(d)/$(gcc_lib_dir)/libobjc_gc.so.* $(d)/$(PF)/$(libdir)/.
#endif
	dh_movefiles -p$(p_lobjc) $(files_lobjc)

	dh_installdocs -p$(p_lobjc)
	echo "See /$(docdir)/$(p_base) for more information" \
		> $(d_lobjc)/$(docdir)/$(p_lobjc)/README.Debian
	dh_installchangelogs -p$(p_lobjc) $(srcdir)/libobjc/ChangeLog

	debian/dh_rmemptydirs -p$(p_lobjc)

	dh_strip -p$(p_lobjc)
	dh_compress -p$(p_lobjc)

	dh_fixperms -p$(p_lobjc)
	b=libobjc; \
	for ext in preinst postinst prerm postrm; do \
	  for t in '' -dev -dbg; do \
	    v=$(OBJC_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done
	DH_COMPAT=3 dh_makeshlibs -p$(p_lobjc) \
		-V '$(p_lobjc) (>= $(DEB_OBJC_SOVERSION))'
	dh_shlibdeps -p$(p_lobjc)
	dh_gencontrol -p$(p_lobjc) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_lobjc)
	dh_md5sums -p$(p_lobjc)
	dh_builddeb -p$(p_lobjc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
