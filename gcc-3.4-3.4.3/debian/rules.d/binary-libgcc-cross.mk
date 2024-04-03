arch_binaries  := $(arch_binaries) libgcc
ifeq ($(with_lib64gcc),yes)
  arch_binaries	:= $(arch_binaries) lib64gcc
endif
ifeq ($(biarch_ia32),yes)
  arch_binaries	:= $(arch_binaries) lib32gcc
endif

p_lgcc		= libgcc$(GCC_SONAME)$(cross_lib_arch)
d_lgcc		= debian/$(p_lgcc)

p_l64gcc	= lib32gcc$(GCC_SONAME)$(cross_lib_arch)
d_l64gcc	= debian/$(p_l32gcc)

p_l64gcc	= lib64gcc$(GCC_SONAME)$(cross_lib_arch)
d_l64gcc	= debian/$(p_l64gcc)

ifeq ($(with_shared_libgcc),yes)
files_lgcc = \
	$(PF)/$(DEB_TARGET_GNU_TYPE)/lib/libgcc_s.so.$(GCC_SONAME)
files_l64gcc = \
	$(PF)/$(DEB_TARGET_GNU_TYPE)/lib64/libgcc_s.so.$(GCC_SONAME)
endif

# ----------------------------------------------------------------------
ifeq ($(DEB_TARGET_GNU_CPU),ia64)
$(binary_stamp)-libgcc: $(install_dependencies) $(binary_stamp)-libunwind
else
$(binary_stamp)-libgcc: $(install_dependencies)
endif
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_lgcc)
	dh_installdirs -p$(p_lgcc) \
		$(docdir)/$(p_lgcc) \
		$(PF)/$(DEB_TARGET_GNU_TYPE)/lib

ifeq ($(with_shared_libgcc),yes)
	DH_COMPAT=2 dh_movefiles -p$(p_lgcc) $(files_lgcc)
endif

	dh_installdocs -p$(p_lgcc)
	dh_installchangelogs -p$(p_lgcc)

	debian/dh_rmemptydirs -p$(p_lgcc)
ifeq ($(with_shared_libgcc),yes)
  ifeq ($(DEB_TARGET_GNU_CPU),ia64)
	cp -a $(PWD)/$(d)-unwind/usr/lib/libunwind.so.* $(d_lgcc)/usr/$(DEB_TARGET_GNU_TYPE)/lib/
	dh_makeshlibs -p$(p_lgcc) -V '$(p_lgcc) (>= 1:3.4.3-6)' -n
	sed s/$(cross_lib_arch)//g < debian/$(p_lgcc)/DEBIAN/shlibs > debian/$(p_lgcc)/DEBIAN/shlibs.fixed
	mv debian/$(p_lgcc)/DEBIAN/shlibs.fixed debian/$(p_lgcc)/DEBIAN/shlibs
	touch debian/$(p_lgcc).substvars
	ARCH=$(DEB_TARGET_ARCH) MAKEFLAGS="CC=something" dh_shlibdeps -p$(p_lgcc) -Xlibgcc_s
	sed 's/\(lib[^ ]*\) /\1$(cross_lib_arch) /g' < debian/$(p_lgcc).substvars > debian/$(p_lgcc).substvars.new
	mv debian/$(p_lgcc).substvars.new debian/$(p_lgcc).substvars
  else
	dh_makeshlibs -p$(p_lgcc) -V '$(p_lgcc) (>= $(DEB_LIBGCC_SOVERSION))' -n
	sed s/$(cross_lib_arch)//g < debian/$(p_lgcc)/DEBIAN/shlibs > debian/$(p_lgcc)/DEBIAN/shlibs.fixed
	mv debian/$(p_lgcc)/DEBIAN/shlibs.fixed debian/$(p_lgcc)/DEBIAN/shlibs
	touch debian/$(p_lgcc).substvars
	ARCH=$(DEB_TARGET_ARCH) MAKEFLAGS="CC=something" dh_shlibdeps -p$(p_lgcc)
	sed 's/\(lib[^ ]*\) /\1$(cross_lib_arch) /g' < debian/$(p_lgcc).substvars > debian/$(p_lgcc).substvars.new
	mv debian/$(p_lgcc).substvars.new debian/$(p_lgcc).substvars
  endif
	cat debian/$(p_lgcc)/DEBIAN/shlibs >> debian/shlibs.local
endif
	PATH=/usr/share/dpkg-cross:$$PATH dh_strip -p$(p_lgcc)
	dh_compress -p$(p_lgcc)
	dh_fixperms -p$(p_lgcc)
	dh_gencontrol -p$(p_lgcc) -u-v$(DEB_LIBGCC_VERSION)
	b=libgcc; v=$(GCC_SONAME); \
	for ext in preinst postinst prerm postrm; do \
	  if [ -f debian/$$b$$t.$$ext ]; then \
	    cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	  fi; \
	done
	dh_installdeb -p$(p_lgcc)
	dh_md5sums -p$(p_lgcc)
	dh_builddeb -p$(p_lgcc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-lib64gcc: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l64gcc)
	dh_installdirs -p$(p_l64gcc) \
		$(docdir)/$(p_l64gcc) \
		$(PF)/$(DEB_TARGET_GNU_TYPE)/lib64

ifeq ($(with_shared_libgcc),yes)
	DH_COMPAT=2 dh_movefiles -p$(p_l64gcc) $(files_l64gcc)
endif

	dh_installdocs -p$(p_l64gcc)
	dh_installchangelogs -p$(p_l64gcc)

	debian/dh_rmemptydirs -p$(p_l64gcc)
	PATH=/usr/share/dpkg-cross:$$PATH dh_strip -p$(p_l64gcc)
	dh_compress -p$(p_l64gcc)
	dh_fixperms -p$(p_l64gcc)
ifeq ($(with_shared_libgcc),yes)
	dh_makeshlibs -p$(p_l64gcc) \
		-V '$(p_l64gcc) (>= $(DEB_LIBGCC_SOVERSION))' -n
	sed s/$(cross_lib_arch)//g < debian/$(p_l64gcc)/DEBIAN/shlibs > debian/$(p_l64gcc)/DEBIAN/shlibs.fixed
	mv debian/$(p_l64gcc)/DEBIAN/shlibs.fixed debian/$(p_l64gcc)/DEBIAN/shlibs
	cat debian/$(p_l64gcc)/DEBIAN/shlibs >> debian/shlibs.local
endif
	ARCH=$(DEB_TARGET_ARCH) MAKEFLAGS="CC=something" dh_shlibdeps -p$(p_l64gcc)
	sed 's/\(lib[^ ]*\) /\1$(cross_lib_arch) /g' < debian/$(p_l64gcc).substvars > debian/$(p_l64gcc).substvars.new
	mv debian/$(p_l64gcc).substvars.new debian/$(p_l64gcc).substvars
	dh_gencontrol -p$(p_l64gcc) -u-v$(DEB_VERSION)
	b=libgcc; v=$(GCC_SONAME); \
	for ext in preinst postinst prerm postrm; do \
	  if [ -f debian/$$b$$t.$$ext ]; then \
	    cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	  fi; \
	done
	dh_installdeb -p$(p_l64gcc)
	dh_md5sums -p$(p_l64gcc)
	dh_builddeb -p$(p_l64gcc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------

$(binary_stamp)-lib32gcc: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_l32gcc)
	dh_installdirs -p$(p_l32gcc) \
		$(docdir)/$(p_l32gcc) \
		$(PF)/$(DEB_TARGET_GNU_TYPE)/lib32

ifeq ($(with_shared_libgcc),yes)
	DH_COMPAT=2 dh_movefiles -p$(p_l32gcc) $(files_l32gcc)
endif

	dh_installdocs -p$(p_l32gcc)
	dh_installchangelogs -p$(p_l32gcc)

	debian/dh_rmemptydirs -p$(p_l32gcc)
	PATH=/usr/share/dpkg-cross:$$PATH dh_strip -p$(p_l32gcc)
	dh_compress -p$(p_l32gcc)
	dh_fixperms -p$(p_l32gcc)
ifeq ($(with_shared_libgcc),yes)
	dh_makeshlibs -p$(p_l32gcc) \
		-V '$(p_l32gcc) (>= $(DEB_LIBGCC_SOVERSION))' -n
	sed s/$(cross_lib_arch)//g < debian/$(p_l32gcc)/DEBIAN/shlibs > debian/$(p_l32gcc)/DEBIAN/shlibs.fixed
	mv debian/$(p_l32gcc)/DEBIAN/shlibs.fixed debian/$(p_l32gcc)/DEBIAN/shlibs
	cat debian/$(p_l32gcc)/DEBIAN/shlibs >> debian/shlibs.local
endif
	ARCH=$(DEB_TARGET_ARCH) MAKEFLAGS="CC=something" dh_shlibdeps -p$(p_l32gcc)
	sed 's/\(lib[^ ]*\) /\1$(cross_lib_arch) /g' < debian/$(p_l32gcc).substvars > debian/$(p_l32gcc).substvars.new
	mv debian/$(p_l32gcc).substvars.new debian/$(p_l32gcc).substvars
	dh_gencontrol -p$(p_l32gcc) -u-v$(DEB_VERSION)
	b=libgcc; v=$(GCC_SONAME); \
	for ext in preinst postinst prerm postrm; do \
	  if [ -f debian/$$b$$t.$$ext ]; then \
	    cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	  fi; \
	done
	dh_installdeb -p$(p_l32gcc)
	dh_md5sums -p$(p_l32gcc)
	dh_builddeb -p$(p_l32gcc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------

unwind_dir = libunwind-0.98.3
xxx: $(binary_stamp)-libunwind
$(binary_stamp)-libunwind:
	rm -rf $(unwind_dir).tar.gz $(unwind_dir) $(d)-unwind
	uudecode libunwind.uue
	tar xfz $(unwind_dir).tar.gz

	: Below are several hacks to allow cross-build of libunwind unless
	: related bugs both at unwind side and at gcc side are fixed

	: Put pre-built Gcursor_i.h and Lcursor_i.h into src/ ...
	cp debian/cross-unwind/[GL]cursor_i.h $(unwind_dir)/src/
	touch $(unwind_dir)/src/[GL]cursor_i.h

	: ... and avoid rebuilding of those files
	sed 's/\(@[GL]cursor_i.h\):/\1DISABLE:/g' $(unwind_dir)/src/Makefile.in > $(unwind_dir)/src/Makefile.in.tmp
	mv -f $(unwind_dir)/src/Makefile.in.tmp $(unwind_dir)/src/Makefile.in
	
	: also, alter configure to disable REMOTE_ONLY mode of libunwind
	: that is turned on with target!=build
	: /manually checked that the following regexp affects only this/
	sed 's/test x\$$target_arch != x\$$build_arch/false/g' $(unwind_dir)/configure > $(unwind_dir)/configure.tmp
	mv $(unwind_dir)/configure.tmp $(unwind_dir)/configure
	chmod 755 $(unwind_dir)/configure

	: also, patch iseveral files to ensure PATH_MAX is defined
	: /limits.h does not include it because of some problem/
	cd $(unwind_dir)/src && patch -p1 < ../../debian/cross-unwind/path_max.patch

	cd $(unwind_dir) && CC="$(PWD)/build/gcc/xgcc -B$(PWD)/build/gcc/" ./configure \
		--build=$(DEB_BUILD_GNU_TYPE) \
		--host=$(DEB_TARGET_GNU_TYPE) \
		--prefix=/usr
	$(MAKE) -C $(unwind_dir)
	$(MAKE) -C $(unwind_dir) install DESTDIR=$(PWD)/$(d)-unwind
	touch $(binary_stamp)-libunwind
