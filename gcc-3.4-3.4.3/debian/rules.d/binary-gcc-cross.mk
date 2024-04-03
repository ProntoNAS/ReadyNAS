arch_binaries  := $(arch_binaries) gcc

# gcc must be moved after g77 and g++
# not all files $(PF)/include/*.h are part of gcc,
# but it becomes difficult to name all these files ...

dirs_gcc = \
	$(PF)/bin \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir)/include \
	$(PF)/share/man/man1 $(libdir)

files_gcc = \
	$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-gcc$(pkg_ver) \
	$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-gcc$(pkg_ver).1 \
	$(gcc_lexec_dir)/collect2 \
	$(gcc_lib_dir)/{specs,libgcc*,*.o} \
	$(gcc_lib_dir)/include/README \
	$(gcc_lib_dir)/include/{float,iso646,limits,std*,syslimits,unwind,varargs}.h \
	$(shell for d in asm bits gnu linux; do \
		  test -e $(d)/$(gcc_lib_dir)/include/$$d \
		    && echo $(gcc_lib_dir)/include/$$d; \
		done)

ifeq ($(biarch),yes)
    files_gcc += $(gcc_lib_dir)/64/{libgcc*,*.o}
endif

files_gcc += \
	$(shell test -e $(d)/$(gcc_lib_dir)/SYSCALLS.c.X \
		&& echo $(gcc_lib_dir)/SYSCALLS.c.X)

ifeq ($(DEB_TARGET_ARCH),ia64)
    files_gcc += $(gcc_lib_dir)/include/ia64intrin.h
endif

ifeq ($(DEB_TARGET_ARCH),i386)
    files_gcc += $(gcc_lib_dir)/include/{,e,p,x}mmintrin.h
endif

ifeq ($(DEB_TARGET_ARCH),hurd-i386)
    files_gcc += $(gcc_lib_dir)/include/{,e,p,x}mmintrin.h
endif

ifeq ($(DEB_TARGET_ARCH),m68k)
    files_gcc += $(gcc_lib_dir)/include/math-68881.h
endif

ifeq ($(DEB_TARGET_ARCH),powerpc)
    files_gcc += $(gcc_lib_dir)/include/{altivec.h,ppc-asm.h}
endif

usr_doc_files = debian/README.Bugs \
	$(shell test -f $(srcdir)/FAQ && echo $(srcdir)/FAQ)
ifeq ($(with_check),yes)
  usr_doc_files += test-summary
endif

# ----------------------------------------------------------------------
$(binary_stamp)-gcc: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gcc)
	dh_installdirs -p$(p_gcc) $(dirs_gcc)

	rm -f $(d)/$(PF)/$(libdir)/libgcc_s.so
	ln -sf /$(PF)/$(DEB_TARGET_GNU_TYPE)/$(libdir)/libgcc_s.so.$(GCC_SONAME) $(d)/$(gcc_lib_dir)/libgcc_s.so
ifeq ($(biarch),yes)
	rm -f $(d)/$(PF)/$(lib64)/libgcc_s.so
	ln -sf /$(PF)/$(DEB_TARGET_GNU_TYPE)/$(lib64)/libgcc_s.so.$(GCC_SONAME) $(d)/$(gcc_lib_dir)/libgcc_s_64.so
	ln -sf /$(PF)/$(DEB_TARGET_GNU_TYPE)/$(lib64)/libgcc_s.so.$(GCC_SONAME) $(d)/$(gcc_lib_dir)/64/libgcc_s.so
endif

	DH_COMPAT=2 dh_movefiles -p$(p_gcc) $(files_gcc)

#	dh_installdebconf
	debian/dh_doclink -p$(p_gcc) $(p_base)
	debian/dh_rmemptydirs -p$(p_gcc)
	PATH=/usr/share/dpkg-cross:$$PATH dh_strip -p$(p_gcc)
	dh_compress -p$(p_gcc)
	dh_fixperms -p$(p_gcc)
	dh_shlibdeps -p$(p_gcc)
	dh_gencontrol -p$(p_gcc) -u-v$(DEB_VERSION)
	sed 's/cross-/$(TP)/g;s/-ver/$(pkg_ver)/g' < debian/gcc-cross.postinst > debian/$(p_gcc)/DEBIAN/postinst
	sed 's/cross-/$(TP)/g;s/-ver/$(pkg_ver)/g' < debian/gcc-cross.prerm > debian/$(p_gcc)/DEBIAN/prerm
	chmod 755 debian/$(p_gcc)/DEBIAN/{postinst,prerm}
	dh_installdeb -p$(p_gcc)
	dh_md5sums -p$(p_gcc)
	dh_builddeb -p$(p_gcc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

	: # remove empty directories, when all components are in place
	for d in `find $(d) -depth -type d -empty 2> /dev/null`; do \
	    while rmdir $$d 2> /dev/null; do d=`dirname $$d`; done; \
	done

	@echo "Listing installed files not included in any package:"
	-find $(d) ! -type d

