arch_binaries := $(arch_binaries) objc

p_objc	= gobjc$(pkg_ver)
d_objc	= debian/$(p_objc)

dirs_objc = \
	$(docdir)/$(p_base)/ObjC \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir)/include

files_objc = \
	$(gcc_lexec_dir)/cc1obj \
	$(gcc_lib_dir)/include/objc \
	$(gcc_lib_dir)/{cc1obj,libobjc*.a,libobjc*.la}

$(binary_stamp)-objc: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	mv $(d)/$(PF)/$(libdir)/libobjc*.{a,la} $(d)/$(gcc_lib_dir)/

	rm -rf $(d_objc)
	dh_installdirs -p$(p_objc) $(dirs_objc)
	dh_movefiles -p$(p_objc) $(files_objc)

	dh_link -p$(p_objc) \
	  /$(PF)/$(libdir)/libobjc.so.$(OBJC_SONAME) /$(gcc_lib_dir)/libobjc.so
ifeq ($(with_objc_gc),yes)
	dh_link -p$(p_objc) \
	  /$(PF)/$(libdir)/libobjc_gc.so.$(OBJC_SONAME) \
	  /$(gcc_lib_dir)/libobjc_gc.so
endif

	debian/dh_doclink -p$(p_objc) $(p_base)
	cp -p $(srcdir)/libobjc/{README*,THREADS*} \
		$(d_objc)/$(docdir)/$(p_base)/ObjC/.

	debian/dh_rmemptydirs -p$(p_objc)

	dh_strip -p$(p_objc)
	dh_compress -p$(p_objc)

	dh_fixperms -p$(p_objc)
	dh_shlibdeps -p$(p_objc)
	dh_gencontrol -p$(p_objc) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_objc)
	dh_md5sums -p$(p_objc)
	dh_builddeb -p$(p_objc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
