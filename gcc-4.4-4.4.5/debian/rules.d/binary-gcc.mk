ifneq (,$(filter yes, $(biarch64) $(biarch32) $(biarchn32)))
  arch_binaries  := $(arch_binaries) gcc-multi
endif
ifeq ($(with_plugins),yes)
  arch_binaries  += gcc-plugindev
endif
arch_binaries  := $(arch_binaries) gcc

ifneq ($(DEB_CROSS),yes)
  ifneq ($(GFDL_INVARIANT_FREE),yes)
    indep_binaries := $(indep_binaries) gcc-doc
  endif
  ifeq ($(with_nls),yes)
    indep_binaries := $(indep_binaries) gcc-locales
  endif
endif

# gcc must be moved after g77 and g++
# not all files $(PF)/include/*.h are part of gcc,
# but it becomes difficult to name all these files ...

dirs_gcc = \
	$(docdir)/$(p_base)/{gcc,libssp,gomp} \
	$(PF)/bin \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir)/{include,include-fixed} \
	$(PF)/share/man/man1 $(libgcc_dir)

# XXX: what about triarch mapping?
files_gcc = \
	$(PF)/bin/$(cmd_prefix){gcc,gcov}$(pkg_ver) \
	$(gcc_lexec_dir)/collect2 \
	$(gcc_lib_dir)/include/std*.h \
	$(shell for h in \
		    README features.h arm_neon.h \
		    {cpuid,decfloat,float,iso646,limits,mm3dnow,mm_malloc}.h \
		    {ppu_intrinsics,paired,spu2vmx,vec_types,si2vmx}.h \
		    {,a,b,e,i,n,p,s,t,w,x}mmintrin.h mmintrin-common.h \
		    {x86,avx}intrin.h \
		    {cross-stdarg,syslimits,unwind,varargs}.h; \
		do \
		  test -e $(d)/$(gcc_lib_dir)/include/$$h \
		    && echo $(gcc_lib_dir)/include/$$h; \
		  test -e $(d)/$(gcc_lib_dir)/include-fixed/$$h \
		    && echo $(gcc_lib_dir)/include-fixed/$$h; \
		done) \
	$(shell for d in \
		  asm bits gnu linux $(TARGET_ALIAS) \
		  $(subst $(DEB_TARGET_GNU_CPU),$(biarch_cpu),$(TARGET_ALIAS)); \
		do \
		  test -e $(d)/$(gcc_lib_dir)/include/$$d \
		    && echo $(gcc_lib_dir)/include/$$d; \
		  test -e $(d)/$(gcc_lib_dir)/include-fixed/$$d \
		    && echo $(gcc_lib_dir)/include-fixed/$$d; \
		done) \
	$(shell test -e $(d)/$(gcc_lib_dir)/SYSCALLS.c.X \
		&& echo $(gcc_lib_dir)/SYSCALLS.c.X)

ifneq ($(GFDL_INVARIANT_FREE),yes)
    files_gcc += \
	$(PF)/share/man/man1/$(cmd_prefix){gcc,gcov}$(pkg_ver).1
endif

ifeq ($(with_libssp),yes)
    files_gcc += $(gcc_lib_dir)/include/ssp
endif
ifeq ($(with_gomp),yes)
    files_gcc += $(gcc_lib_dir)/include/omp.h
endif

ifeq ($(DEB_TARGET_ARCH),ia64)
    files_gcc += $(gcc_lib_dir)/include/ia64intrin.h
endif

ifeq ($(DEB_TARGET_ARCH),m68k)
    files_gcc += $(gcc_lib_dir)/include/math-68881.h
endif

ifeq ($(DEB_TARGET_ARCH),$(findstring $(DEB_TARGET_ARCH),powerpc ppc64 powerpcspe))
    files_gcc += $(gcc_lib_dir)/include/{altivec.h,ppc-asm.h,spe.h}
endif

usr_doc_files = debian/README.Bugs \
	$(shell test -f $(srcdir)/FAQ && echo $(srcdir)/FAQ) \
	$(shell test -f test-summary && echo test-summary)

p_loc	= gcc$(pkg_ver)-locales
d_loc	= debian/$(p_loc)

p_gcc_m	= gcc$(pkg_ver)-multilib$(cross_bin_arch)
d_gcc_m	= debian/$(p_gcc_m)

p_pld	= gcc$(pkg_ver)-plugin-dev
d_pld	= debian/$(p_pld)

# __do_gcc_libs(flavour,package,todir,fromdir)
define __do_gcc_libs
	dh_installdirs -p$(2) $(3)
# stage1 builds static libgcc only
	$(if $(filter $(DEB_STAGE),stage1),,
		: # libgcc_s.so may be a linker script on some architectures
		set -e; \
		if [ -h $(4)/libgcc_s.so ]; then \
		  rm -f $(4)/libgcc_s.so; \
		  dh_link -p$(2) /$(libgcc_dir$(1))/libgcc_s.so.$(GCC_SONAME) \
		    $(3)/libgcc_s.so; \
		else \
		  mv $(4)/libgcc_s.so $(d)/$(3)/libgcc_s.so; \
		  dh_link -p$(2) /$(libgcc_dir$(1))/libgcc_s.so.$(GCC_SONAME) \
		    $(3)/libgcc_s.so.$(GCC_SONAME); \
		fi; \
		$(if $(1), dh_link -p$(2) /$(3)/libgcc_s.so \
		    $(gcc_lib_dir)/libgcc_s_$(1).so;)
	)
	DH_COMPAT=2 dh_movefiles -p$(2) \
		$(3)/{libgcc*,libgcov.a,*.o}
	$(if $(filter yes, $(with_lib$(1)gmath)),
		$(call install_gcc_lib,libgcc-math,$(GCC_SONAME),$(1),$(2))
	)
	$(if $(filter yes, $(with_libssp)),
		$(call install_gcc_lib,libssp,$(SSP_SONAME),$(1),$(2))
	)
	$(if $(filter yes, $(with_ssp)),
		mv $(4)/libssp_nonshared.a debian/$(2)/$(3)/;
	)
	$(if $(filter yes, $(with_gomp)),
		$(call install_gcc_lib,libgomp,$(GOMP_SONAME),$(1),$(2))
	)

endef

# do_gcc_libs(flavour,pkg)
define do_gcc_libs
	$(call __do_gcc_libs,$(1),$(2),$(gcc_lib_dir$(1)),$(d)/$(usr_lib$(1)))
endef

# ----------------------------------------------------------------------
$(binary_stamp)-gcc: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gcc)
	dh_installdirs -p$(p_gcc) $(dirs_gcc)

	$(call do_gcc_libs,,$(p_gcc))

ifeq ($(with_linaro_branch),yes)
	cp -p $(srcdir)/ChangeLog.linaro \
		$(d_gcc)/$(docdir)/$(p_base)/changelog.linaro
endif
ifeq ($(with_libssp),yes)
	cp -p $(srcdir)/libssp/ChangeLog \
		$(d_gcc)/$(docdir)/$(p_base)/libssp/changelog
endif
ifeq ($(with_gomp),yes)
	mv $(d)/$(usr_lib)/libgomp*.spec $(d_gcc)/$(gcc_lib_dir)/
	cp -p $(srcdir)/libgomp/ChangeLog \
		$(d_gcc)/$(docdir)/$(p_base)/gomp/changelog
endif

	DH_COMPAT=2 dh_movefiles -p$(p_gcc) $(files_gcc)

ifneq ($(DEB_CROSS),yes)
	ln -sf gcc$(pkg_ver) \
	    $(d_gcc)/$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-gcc$(pkg_ver)
	ln -sf gcc$(pkg_ver) \
	    $(d_gcc)/$(PF)/bin/$(TARGET_ALIAS)-gcc$(pkg_ver)
ifneq ($(GFDL_INVARIANT_FREE),yes)
	ln -sf gcc$(pkg_ver).1 \
	    $(d_gcc)/$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-gcc$(pkg_ver).1
	ln -sf gcc$(pkg_ver).1 \
	    $(d_gcc)/$(PF)/share/man/man1/$(TARGET_ALIAS)-gcc$(pkg_ver).1
endif
endif

#	dh_installdebconf
	debian/dh_doclink -p$(p_gcc) $(p_base)
	cp -p $(usr_doc_files) $(d_gcc)/$(docdir)/$(p_base)/.
	if [ -f testsuite-comparision ]; then \
	  cp -p testsuite-comparision $(d_gcc)/$(docdir)/$(p_base)/. ; \
	fi
	cp -p debian/README.ssp $(d_gcc)/$(docdir)/$(p_base)/
	cp -p debian/NEWS.gcc $(d_gcc)/$(docdir)/$(p_base)/NEWS
	cp -p debian/NEWS.html $(d_gcc)/$(docdir)/$(p_base)/NEWS.html
	cp -p $(srcdir)/ChangeLog $(d_gcc)/$(docdir)/$(p_base)/changelog
	cp -p $(srcdir)/gcc/ChangeLog \
		$(d_gcc)/$(docdir)/$(p_base)/gcc/changelog
	if [ -f $(builddir)/gcc/.bad_compare ]; then \
	  ( \
	    echo "The comparision of the stage2 and stage3 object files shows differences."; \
	    echo "The Debian package was modified to ignore these differences."; \
	    echo ""; \
	    echo "The following files differ:"; \
	    echo ""; \
	    cat $(builddir)/gcc/.bad_compare; \
	  ) > $(d_gcc)/$(docdir)/$(p_base)/BOOTSTRAP_COMPARISION_FAILURE; \
	else \
	  true; \
	fi
	debian/dh_rmemptydirs -p$(p_gcc)
	dh_strip -p$(p_gcc)
	dh_compress -p$(p_gcc) -X README.Bugs
	dh_fixperms -p$(p_gcc)
	dh_shlibdeps -p$(p_gcc)
	dh_gencontrol -p$(p_gcc) -- -v$(DEB_VERSION) $(common_substvars)
	dh_installdeb -p$(p_gcc)
	dh_md5sums -p$(p_gcc)
	dh_builddeb -p$(p_gcc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

	: # remove empty directories, when all components are in place
	-find $(d) -type d -empty -delete

	@echo "Listing installed files not included in any package:"
	-find $(d) ! -type d

# ----------------------------------------------------------------------

$(binary_stamp)-gcc-multi: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gcc_m)
	dh_installdirs -p$(p_gcc_m) $(docdir)

	$(foreach flavour,$(flavours), \
		$(call do_gcc_libs,$(flavour),$(p_gcc_m)))

	debian/dh_doclink -p$(p_gcc_m) $(p_base)
	debian/dh_rmemptydirs -p$(p_gcc_m)

	dh_strip -p$(p_gcc_m)
	dh_compress -p$(p_gcc_m)
	dh_shlibdeps -p$(p_gcc_m)
	dh_fixperms -p$(p_gcc_m)
	dh_installdeb -p$(p_gcc_m)
	dh_gencontrol -p$(p_gcc_m) -- -v$(DEB_VERSION) $(common_substvars)
	dh_md5sums -p$(p_gcc_m)
	dh_builddeb -p$(p_gcc_m)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-gcc-plugindev: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_pld)
	dh_installdirs -p$(p_pld) \
		$(docdir) \
		$(gcc_lib_dir)
	DH_COMPAT=2 dh_movefiles -p$(p_pld) \
		$(gcc_lib_dir)/plugin
	cp $(srcdir)/gcc/config/vxworks-dummy.h \
		$(d_pld)/$(gcc_lib_dir)/plugin/include/config/

	debian/dh_doclink -p$(p_pld) $(p_base)
	debian/dh_rmemptydirs -p$(p_pld)

	dh_compress -p$(p_pld)
	dh_fixperms -p$(p_pld)
	dh_installdeb -p$(p_pld)
	dh_gencontrol -p$(p_pld) -- -v$(DEB_VERSION) $(common_substvars)
	dh_md5sums -p$(p_pld)
	dh_builddeb -p$(p_pld)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-gcc-locales: $(install_dependencies)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_loc)
	dh_installdirs -p$(p_loc) \
		$(docdir)
	DH_COMPAT=2 dh_movefiles -p$(p_loc) \
		$(PF)/share/locale/*/*/cpplib*.* \
		$(PF)/share/locale/*/*/gcc*.*

	debian/dh_doclink -p$(p_loc) $(p_base)
	debian/dh_rmemptydirs -p$(p_loc)

	dh_compress -p$(p_loc)
	dh_fixperms -p$(p_loc)
	dh_installdeb -p$(p_loc)
	dh_gencontrol -p$(p_loc) -- -v$(DEB_VERSION) $(common_substvars)
	dh_md5sums -p$(p_loc)
	dh_builddeb -p$(p_loc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-gcc-doc: $(build_html_stamp) $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_doc)
	dh_installdirs -p$(p_doc) \
		$(docdir)/$(p_base) \
		$(PF)/share/info
	DH_COMPAT=2 dh_movefiles -p$(p_doc) \
		$(PF)/share/info/gcc*
	rm -f $(d_doc)/$(PF)/share/info/gccinst*

ifeq ($(with_gomp),yes)
	$(MAKE) -C $(buildlibdir)/libgomp stamp-build-info
	cp -p $(buildlibdir)/libgomp/libgomp$(pkg_ver).info $(d_doc)/$(PF)/share/info/
endif

	debian/dh_doclink -p$(p_doc) $(p_base)
	dh_installdocs -p$(p_doc) html/gcc.html html/gccint.html
ifeq ($(with_gomp),yes)
	cp -p html/libgomp.html $(d_doc)/usr/share/doc/$(p_doc)/
endif
	rm -f $(d_doc)/$(docdir)/$(p_base)/copyright
	debian/dh_rmemptydirs -p$(p_doc)

	dh_compress -p$(p_doc)
	dh_fixperms -p$(p_doc)
	dh_installdeb -p$(p_doc)
	dh_gencontrol -p$(p_doc) -- -v$(DEB_VERSION) $(common_substvars)
	dh_md5sums -p$(p_doc)
	dh_builddeb -p$(p_doc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
