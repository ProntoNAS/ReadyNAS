ifeq ($(with_libgcc),yes)
  arch_binaries	:= $(arch_binaries) libgcc
  ifeq ($(with_lib64gcc),yes)
    arch_binaries	:= $(arch_binaries) lib64gcc
  endif
  ifeq ($(biarch_ia32),yes)
    arch_binaries	:= $(arch_binaries) lib32gcc
  endif
endif

p_lgcc		= libgcc$(GCC_SONAME)
d_lgcc		= debian/$(p_lgcc)

p_l32gcc	= lib32gcc$(GCC_SONAME)
d_l32gcc	= debian/$(p_l32gcc)

p_l64gcc	= lib64gcc$(GCC_SONAME)
d_l64gcc	= debian/$(p_l64gcc)

ifeq ($(with_shared_libgcc),yes)
files_lgcc = \
	$(libdir)/libgcc_s.so.$(GCC_SONAME)
files_l64gcc = \
	lib64/libgcc_s.so.$(GCC_SONAME)
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
		$(libdir)

ifeq ($(with_shared_libgcc),yes)
	mv $(d)/$(PF)/lib/libgcc_s.so.$(GCC_SONAME) $(d)/$(libdir)/.
	DH_COMPAT=2 dh_movefiles -p$(p_lgcc) $(files_lgcc)
endif

	dh_installdocs -p$(p_lgcc)
	dh_installchangelogs -p$(p_lgcc)

	debian/dh_rmemptydirs -p$(p_lgcc)
ifeq ($(with_shared_libgcc),yes)
  ifeq ($(DEB_TARGET_GNU_CPU),ia64)
	cp -a $(PWD)/$(d)-unwind/usr/lib/libunwind.so.* $(d_lgcc)/$(libdir)/
	#mv $(d)/$(PF)/lib/libunwind.so.7 $(d_lgcc)/$(libdir)/.
	dh_makeshlibs -p$(p_lgcc) -V '$(p_lgcc) (>= 1:3.4.3-6)'
	dh_shlibdeps -p$(p_lgcc) -Xlibgcc_s
  else
	dh_makeshlibs -p$(p_lgcc) -V '$(p_lgcc) (>= $(DEB_LIBGCC_SOVERSION))'
	dh_shlibdeps -p$(p_lgcc)
  endif
	cat debian/$(p_lgcc)/DEBIAN/shlibs >> debian/shlibs.local
endif
	dh_strip -p$(p_lgcc)
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
		lib64

ifeq ($(with_shared_libgcc),yes)
	install -d $(d)/lib64
	mv $(d)/$(PF)/lib64/libgcc_s.so.$(GCC_SONAME) $(d)/lib64/.
endif
	DH_COMPAT=2 dh_movefiles -p$(p_l64gcc) $(files_l64gcc)

	dh_installdocs -p$(p_l64gcc)
	dh_installchangelogs -p$(p_l64gcc)

	debian/dh_rmemptydirs -p$(p_l64gcc)
	dh_strip -p$(p_l64gcc)
	dh_compress -p$(p_l64gcc)
	dh_fixperms -p$(p_l64gcc)
ifeq ($(with_shared_libgcc),yes)
	dh_makeshlibs -p$(p_l64gcc) \
		-V '$(p_l64gcc) (>= $(DEB_LIBGCC_SOVERSION))'
# this does not work ... shlibs.local doesn't distinguish 32/64 bit libs
#	cat debian/$(p_l64gcc)/DEBIAN/shlibs >> debian/shlibs.local
endif
ifeq ($(DEB_TARGET_GNU_CPU),s390)
#	dh_shlibdeps -p$(p_l64gcc)
#/usr/bin/ldd: line 1: /lib/ld64.so.1: cannot execute binary file
#dpkg-shlibdeps: failure: ldd on `debian/lib64gcc1/lib64/libgcc_s.so.1' gave error exit status 1
	echo 'shlibs:Depends=libc6-s390x (>= 2.3.1-1)' \
		> debian/$(p_l64gcc).substvars
else
  ifeq ($(DEB_TARGET_GNU_CPU),i386)
	echo 'shlibs:Depends=amd64-libs (>= 0.1)' \
		> debian/$(p_l64gcc).substvars
  else
    ifeq ($(DEB_TARGET_GNU_CPU),powerpc)
        # XXX change dependency when lib64c6 exists
	echo 'shlibs:Depends=libc6 (>= 0.1)' \
		> debian/$(p_l64gcc).substvars
    else
      ifeq ($(DEB_TARGET_GNU_CPU),sparc)
	echo 'shlibs:Depends=libc6-sparc64' \
		> debian/$(p_l64gcc).substvars
      else
	dh_shlibdeps -p$(p_l64gcc)
      endif
    endif
  endif
endif
	dh_gencontrol -p$(p_l64gcc) -u-v$(DEB_LIBGCC_VERSION)
	b=lib64gcc; v=$(GCC_SONAME); \
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
		emul/ia32-linux/usr/lib

	mv $(d)/$(PF)/lib32/libgcc_s.so.$(GCC_SONAME) \
		$(d_l32gcc)/emul/ia32-linux/usr/lib/.

	dh_installdocs -p$(p_l32gcc)
	dh_installchangelogs -p$(p_l32gcc)

	debian/dh_rmemptydirs -p$(p_l32gcc)
	dh_strip -p$(p_l32gcc)
	dh_compress -p$(p_l32gcc)
	dh_fixperms -p$(p_l32gcc)
	dh_gencontrol -p$(p_l32gcc) -u-v$(DEB_VERSION)
	b=lib32gcc; v=$(GCC_SONAME); \
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

unwind_tgz = libunwind_0.98.5.orig.tar.gz
unwind_dir = libunwind-0.98.5
$(binary_stamp)-libunwind:
	rm -rf $(unwind_tgz) $(unwind_dir) $(d)-unwind
	uudecode libunwind.uue
	tar xfz $(unwind_tgz)
	cd $(unwind_dir) && CC="$(PWD)/build/gcc/xgcc -B$(PWD)/build/gcc/" ./configure \
		--build=$(DEB_BUILD_GNU_TYPE) \
		--host=$(DEB_HOST_GNU_TYPE) \
		--prefix=/usr
	$(MAKE) -C $(unwind_dir)
	$(MAKE) -C $(unwind_dir) install DESTDIR=$(PWD)/$(d)-unwind
	touch $(binary_stamp)-libunwind
