arch_binaries  := $(arch_binaries) gcc-cross

dirs_gcc = \
	$(docdir)/$(p_base)/gcc \
	$(PF)/bin \
	$(gcc_lib_dir)/include \
	$(PF)/share/man/man1

files_gcc = \
	$(PF)/bin/{$(GCC_TARGET)-linux-gcc,$(GCC_TARGET)-linux-cpp}$(pkg_ver) \
	$(PF)/share/man/man1/{$(GCC_TARGET)-linux-gcc}$(pkg_ver).1 \
	$(gcc_lib_dir) \


files_gcc += \
	$(shell test -e $(d)/$(gcc_lib_dir)/SYSCALLS.c.X \
		&& echo $(gcc_lib_dir)/SYSCALLS.c.X)

usr_doc_files = debian/README.Bugs \
	$(shell test -f $(srcdir)/FAQ && echo $(srcdir)/FAQ)
ifeq ($(with_check),yes)
  usr_doc_files += test-summary
endif
ifeq ($(DEB_HOST_ARCH),sparc)
  usr_doc_files += debian/README.sparc
endif

# ----------------------------------------------------------------------
$(binary_stamp)-gcc-cross: $(install_stamp)
	dh_testdir
	dh_testroot
	rm -rf $(d_gcc)
	dh_installdirs -p$(p_gcc) $(dirs_gcc)
	$(IS) debian/c89 $(d)/$(PF)/bin/
	$(IR) debian/c89.1 $(d)/$(PF)/share/man/man1/

	rm -f $(d)/$(PF)/$(libdir)/libgcc_s.so
	ln -sf /$(libdir)/libgcc_s.so.$(GCC_SONAME) $(d)/$(gcc_lib_dir)/libgcc_s.so

	dh_movefiles -p$(p_gcc) $(files_gcc)

#	dh_installdebconf
	debian/dh_doclink -p$(p_gcc) $(p_base)
	cp -p $(usr_doc_files) $(d_gcc)/$(docdir)/$(p_base)/.
	cp -p debian/NEWS.gcc $(d_gcc)/$(docdir)/$(p_base)/NEWS
	dh_undocumented -p$(p_gcc) gccbug$(pkg_ver).1
	debian/dh_rmemptydirs -p$(p_gcc)
	dh_strip -p$(p_gcc)
	dh_compress -p$(p_gcc)
	dh_fixperms -p$(p_gcc)
	dh_shlibdeps -p$(p_gcc)
	dh_gencontrol -p$(p_gcc) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_gcc)
	dh_md5sums -p$(p_gcc)
	dh_builddeb -p$(p_gcc)
	touch $@

