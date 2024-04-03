ifeq ($(with_libg2c),yes)
  arch_binaries  := $(arch_binaries) libg2c
endif

ifeq ($(with_fdev),yes)
  arch_binaries  := $(arch_binaries) fdev
  ifneq ($(GFDL_INVARIANT_FREE),yes)
    indep_binaries := $(indep_binaries) fortran-doc
  endif
  ifeq ($(with_common_libs),yes)
    arch_binaries  := $(arch_binaries) libg2c-dev
  endif
endif

p_g77	= g77$(pkg_ver)
p_g77d	= g77$(pkg_ver)-doc
p_g2c	= libg2c$(F77_SONAME)
p_g2cd	= libg2c$(F77_SONAME)-dev

d_g77	= debian/$(p_g77)
d_g77d	= debian/$(p_g77d)
d_g2c	= debian/$(p_g2c)
d_g2cd	= debian/$(p_g2cd)

dirs_g77 = \
	$(docdir)/$(p_base)/fortran \
	$(PF)/bin \
	$(gcc_lexec_dir) \
	$(gcc_lib_dir)/include \
	$(PF)/include \
	$(PF)/share/man/man1
files_g77 = \
	$(PF)/bin/g77$(pkg_ver) \
	$(gcc_lexec_dir)/f771 \
	$(PF)/share/man/man1/g77$(pkg_ver).1

dirs_g2c = \
	$(docdir)/$(p_base)/fortran \
	$(PF)/$(libdir) \

files_g2c = \
	$(PF)/$(libdir)/libg2c.so.*

dirs_g2cd = \
	$(docdir)/$(p_base)/fortran \
	$(PF)/$(libdir) \

files_g2cd = \
	$(PF)/$(libdir)/libg2c.{a,la,so} \
	$(PF)/$(libdir)/libfrtbegin.a \
	$(gcc_lib_dir)/include/g2c.h

ifeq ($(with_lib64g2c),yes)
	dirs_g2c   += $(PF)/$(lib64)
	files_g2c  += $(PF)/$(lib64)/libg2c.so.*

	dirs_g2cd  += $(PF)/$(lib64)
	files_g2cd += $(PF)/$(lib64)/{libg2c.{a,la,so},libfrtbegin.a}
endif

# ----------------------------------------------------------------------
$(binary_stamp)-libg2c: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_g2c)
	dh_installdirs -p$(p_g2c) $(dirs_g2c)
	dh_movefiles -p$(p_g2c) $(files_g2c)
	debian/dh_doclink -p$(p_g2c) $(p_base)
	cp -p debian/README.libf2c \
		$(d_g2c)/$(docdir)/$(p_base)/fortran/README.Debian

	dh_strip -p$(p_g2c)
	dh_compress -p$(p_g2c)
	dh_fixperms -p$(p_g2c)
	DH_COMPAT=3 dh_makeshlibs -p$(p_g2c) \
		 -V '$(p_g2c) (>= $(DEB_SOVERSION))'
	dh_shlibdeps -p$(p_g2c)
	dh_gencontrol -p$(p_g2c) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_g2c)
	dh_md5sums -p$(p_g2c)
	dh_builddeb -p$(p_g2c)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-libg2c-dev: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_g2cd)
	dh_installdirs -p$(p_g2cd) $(dirs_g2cd)
	dh_movefiles -p$(p_g2cd) $(files_g2cd)
	debian/dh_doclink -p$(p_g2cd) $(p_base)

	dh_strip -p$(p_g2cd)
	dh_compress -p$(p_g2cd)
	dh_fixperms -p$(p_g2cd)
	dh_shlibdeps -p$(p_g2cd)
	dh_gencontrol -p$(p_g2cd) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_g2cd)
	dh_md5sums -p$(p_g2cd)
	dh_builddeb -p$(p_g2cd)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-fdev: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

#	sed "s,^libdir=.*,libdir='/$(gcc_lib_dir)'," \
#		$(d)/$(PF)/lib/libg2c.la > $(d)/$(gcc_lib_dir)/libg2c.la
#	rm -f $(d)/$(PF)/lib/libg2c.la
#	mv $(d)/$(PF)/lib/libg2c.{a,so} $(d)/$(gcc_lib_dir)/
#	ln -sf ../../../libg2c.so.$(F77_SONAME) \
#		$(d)/$(gcc_lib_dir)/libg2c.so
#	mv $(d)/$(PF)/lib/libfrtbegin.a $(d)/$(gcc_lib_dir)/

#ifeq ($(biarch),yes)
#  ifeq ($(DEB_TARGET_ARCH),i386)
#	mv $(d)/$(PF)/$(lib64)/libg2c.{a,la,so} $(d)/$(gcc_lib_dir)/64
#	ln -sf ../../../../lib64/libg2c.so.$(F77_SONAME) \
#		$(d)/$(gcc_lib_dir)/64/libg2c.so
#	mv $(d)/$(PF)/$(lib64)/libfrtbegin.a $(d)/$(gcc_lib_dir)/64
#  endif
#endif
	rm -rf $(d_g77)
	dh_installdirs -p$(p_g77) $(dirs_g77)
	dh_movefiles -p$(p_g77) $(files_g77)

#	dh_installdirs -p$(p_g2cd) $(dirs_g2cd)
#	dh_movefiles -p$(p_g2cd) $(files_g2cd)

	ln -sf g77$(pkg_ver) \
	    $(d_g77)/$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-g77$(pkg_ver)
	ln -sf g77$(pkg_ver).1 \
	    $(d_g77)/$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-g77$(pkg_ver).1
	ln -sf g77$(pkg_ver) \
	    $(d_g77)/$(PF)/bin/$(TARGET_ALIAS)-g77$(pkg_ver)
	ln -sf g77$(pkg_ver).1 \
	    $(d_g77)/$(PF)/share/man/man1/$(TARGET_ALIAS)-g77$(pkg_ver).1

	: # compatibility links (Debian g77-2.95 and g77-3.0 had these ...)
	dh_link -p$(p_g77) \
	  /$(PF)/$(libdir)/libg2c.so /$(gcc_lib_dir)/libg2c-pic.so \
	  /$(PF)/$(libdir)/libg2c.a  /$(gcc_lib_dir)/libg2c-pic.a

	debian/dh_doclink -p$(p_g77) $(p_base)
#	debian/dh_doclink -p$(p_g2cd) $(p_base)

#	#cp -p $(srcdir)/gcc/f/{NEWS,BUGS} \
#	#	$(d_g77)/$(docdir)/$(p_base)/fortran/.
	cp -p $(srcdir)/libf2c/README \
		$(d_g77)/$(docdir)/$(p_base)/fortran/README.libf2c
	cp -p $(srcdir)/gcc/f/ChangeLog \
		$(d_g77)/$(docdir)/$(p_base)/fortran/changelog
	debian/dh_rmemptydirs -p$(p_g77)
#	debian/dh_rmemptydirs -p$(p_g2cd)

	dh_strip -p$(p_g77)
	dh_compress -p$(p_g77)
	dh_fixperms -p$(p_g77)
	dh_shlibdeps -p$(p_g77)
	dh_gencontrol -p$(p_g77) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_g77)
	dh_md5sums -p$(p_g77)
	dh_builddeb -p$(p_g77)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-fortran-doc: $(build_html_stamp) $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_g77d)
	dh_installdirs -p$(p_g77d) \
		$(docdir)/$(p_base)/fortran \
		$(PF)/share/info
	dh_movefiles -p$(p_g77d) \
		$(PF)/share/info/g77*

	debian/dh_doclink -p$(p_g77d) $(p_base)
ifneq ($(GFDL_INVARIANT_FREE),yes)
	dh_installdocs -p$(p_g77d)
	rm -f $(d_g77d)/$(docdir)/$(p_base)/copyright
	cp -p html/g77.html $(d_g77d)/$(docdir)/$(p_base)/fortran/
endif

	dh_compress -p$(p_g77d)
	dh_fixperms -p$(p_g77d)
	dh_installdeb -p$(p_g77d)
	dh_gencontrol -p$(p_g77d) -u-v$(DEB_VERSION)
	dh_md5sums -p$(p_g77d)
	dh_builddeb -p$(p_g77d)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
