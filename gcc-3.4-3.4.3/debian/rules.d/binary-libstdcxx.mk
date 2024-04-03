ifeq ($(with_libcxx),yes)
  arch_binaries  := $(arch_binaries) libstdcxx
endif
ifeq ($(with_lib64cxx),yes)
  arch_binaries  := $(arch_binaries) lib64stdcxx
endif
ifeq ($(biarch_ia32),yes)
  arch_binaries	:= $(arch_binaries) lib32stdcxx
endif

ifeq ($(with_cxxdev),yes)
  arch_binaries  := $(arch_binaries) libstdcxx-dev
  indep_binaries := $(indep_binaries) libstdcxx-doc
endif

p_lib	= libstdc++$(CXX_SONAME)
ifeq ($(with_oldcxxabi),yes)
  p_lib	= libstdc++6-0
endif
ifeq ($(with_oldarmabi),yes)
  p_lib	= libstdc++6-0
endif
p_dev	= $(p_lib)-dev
p_pic	= $(p_lib)-pic
p_dbg	= $(p_lib)-dbg
p_lib64	= lib64stdc++$(CXX_SONAME)
p_lib32	= lib32stdc++$(CXX_SONAME)
p_libd	= libstdc++6-doc
ifeq ($(with_oldcxxabi),yes)
  p_lib64	= lib64stdc++6-0
  p_lib32	= lib32stdc++6-0
endif

d_lib	= debian/$(p_lib)
d_lib64	= debian/$(p_lib64)
d_lib32	= debian/$(p_lib32)
d_dev	= debian/$(p_dev)
d_pic	= debian/$(p_pic)
d_dbg	= debian/$(p_dbg)
d_libd	= debian/$(p_libd)

dirs_lib = \
	$(docdir) \
	$(PF)/$(libdir)

dirs_lib64 = \
	$(docdir) \
	$(PF)/lib64

files_lib = \
	$(PF)/$(libdir)/libstdc++.so.*

files_lib64 = \
	$(PF)/lib64/libstdc++.so.*

dirs_dev = \
	$(docdir)/$(p_base)/C++ \
	$(PF)/$(libdir) \
	$(gcc_lib_dir)/include \
	$(cxx_inc_dir)

files_dev = \
	$(cxx_inc_dir)/ \
	$(gcc_lib_dir)/libstdc++.{a,so} \
	$(gcc_lib_dir)/libsupc++.a
# Not yet...
#	$(PF)/$(libdir)/lib{supc,stdc}++.la

dirs_dbg = \
	$(docdir) \
	$(PF)/$(libdir)/debug \
	$(gcc_lib_dir)
files_dbg = \
	$(PF)/$(libdir)/debug/libstdc++.*

dirs_pic = \
	$(docdir) \
	$(gcc_lib_dir)
files_pic = \
	$(gcc_lib_dir)/libstdc++_pic.a

ifeq ($(with_lib64cxx),yes)
    dirs_dev += $(gcc_lib_dir)/64/
    files_dev += $(gcc_lib_dir)/64/libstdc++.{a,so} \
        $(gcc_lib_dir)/64/libsupc++.a
    dirs_dbg += $(PF)/lib64/debug
    files_dbg += $(PF)/lib64/debug/libstdc++.*
    dirs_pic += $(gcc_lib_dir)
    files_pic += $(gcc_lib_dir)/64/libstdc++_pic.a
endif
ifeq ($(biarch_ia32),yes)
    dirs_dev += $(gcc_lib_dir)/32/
    files_dev += $(gcc_lib_dir)/32/libstdc++.{a,so} \
        $(gcc_lib_dir)/32/libsupc++.a
    dirs_dbg += $(PF)/lib32/debug
    files_dbg += $(PF)/lib32/debug/libstdc++.*
    dirs_pic += $(gcc_lib_dir)
    files_pic += $(gcc_lib_dir)/32/libstdc++_pic.a
endif

# ----------------------------------------------------------------------

gxx_baseline_dir = $(shell \
			sed -n '/^baseline_dir *=/s,.*= *\(.*\)\$$.*$$,\1,p' \
			    $(buildlibdir)/libstdc++-v3/testsuite/Makefile)
gxx_baseline_file = $(gxx_baseline_dir)/baseline_symbols.txt

debian/README.libstdc++-baseline:
	cat debian/README.libstdc++-baseline.in \
		> debian/README.libstdc++-baseline

	baseline_name=`basename $(gxx_baseline_dir)`; \
	baseline_parentdir=`dirname $(gxx_baseline_dir)`; \
	compat_baseline_name=""; \
	if [ -f "$(gxx_baseline_file)" ]; then \
	  ( \
	    echo "A baseline file for $$baseline_name was found."; \
	    echo "Running the check-abi script ..."; \
	    echo ""; \
	    $(MAKE) -C $(buildlibdir)/libstdc++-v3/testsuite \
		check-abi; \
	  ) >> debian/README.libstdc++-baseline; \
	else \
	  ( \
	    echo "No baseline file found for $$baseline_name."; \
	    echo "Generating a new baseline file ..."; \
	    echo ""; \
	  ) >> debian/README.libstdc++-baseline; \
	  mkdir $(gxx_baseline_dir); \
	  $(MAKE) -C $(buildlibdir)/libstdc++-v3/testsuite new-abi-baseline; \
	  cat $(gxx_baseline_file) >> debian/README.libstdc++-baseline; \
	fi

# ----------------------------------------------------------------------
$(binary_stamp)-libstdcxx: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_lib)
	dh_installdirs -p$(p_lib) $(dirs_lib)
	DH_COMPAT=2 dh_movefiles -p$(p_lib) $(files_lib)

	dh_installdocs -p$(p_lib)
	echo "See /$(docdir)/$(p_base) for more information" \
		> $(d_lib)/$(docdir)/$(p_lib)/README.Debian

	dh_installchangelogs -p$(p_lib)
	debian/dh_rmemptydirs -p$(p_lib)

	dh_strip -p$(p_lib)
	dh_compress -p$(p_lib)
	dh_fixperms -p$(p_lib)
	dh_makeshlibs -p$(p_lib) -V '$(p_lib) (>= $(DEB_STDCXX_SOVERSION))'
	cat debian/$(p_lib)/DEBIAN/shlibs >> debian/shlibs.local
	dh_shlibdeps \
		-L$(p_lgcc) -l:$(d)/$(PF)/lib:$(d_lgcc)/lib:\
		-p$(p_lib)
	dh_gencontrol -p$(p_lib) -u-v$(DEB_VERSION)

	b=libstdc++; \
	for ext in preinst postinst prerm postrm; do \
	  for t in ''; do \
	    v=$(CXX_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done

	dh_installdeb -p$(p_lib)
	dh_md5sums -p$(p_lib)
	dh_builddeb -p$(p_lib)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-lib64stdcxx: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_lib64)
	dh_installdirs -p$(p_lib64) $(dirs_lib64)
	install -d $(d)/lib64
	DH_COMPAT=2 dh_movefiles -p$(p_lib64) $(files_lib64)
	dh_installdocs -p$(p_lib64)
	echo "See /$(docdir)/$(p_base) for more information" \
		> $(d_lib64)/$(docdir)/$(p_lib64)/README.Debian
	dh_installchangelogs -p$(p_lib64)
	debian/dh_rmemptydirs -p$(p_lib64)
	dh_strip -p$(p_lib64)
	dh_compress -p$(p_lib64)
	dh_fixperms -p$(p_lib64)
	dh_makeshlibs -p$(p_lib64) -V '$(p_lib64) (>= $(DEB_STDCXX_SOVERSION))'
# pass explicit dependencies to dh_shlibdeps
ifeq ($(DEB_TARGET_GNU_CPU),s390)
#	dh_shlibdeps -p$(p_lib64) -L $(p_l64gcc) -l $(d_l64gcc)/lib
#/usr/bin/ldd: line 1: /lib/ld64.so.1: cannot execute binary file
#dpkg-shlibdeps: failure: ldd on `debian/lib64gcc1/lib64/libgcc_s.so.1' gave error exit status 1
	echo 'shlibs:Depends=libc6-s390x (>= 2.3.1-1), $(p_l64gcc) (>= $(DEB_LIBGCC_SOVERSION))' \
		> debian/$(p_lib64).substvars
else
  ifeq ($(DEB_TARGET_GNU_CPU),i386)
	echo 'shlibs:Depends=amd64-libs (>= 0.1), $(p_l64gcc) (>= $(DEB_LIBGCC_SOVERSION))' \
		> debian/$(p_lib64).substvars
  else
    ifeq ($(DEB_TARGET_GNU_CPU),powerpc)
	# XXX change dependency when lib64c6 exists
	echo 'shlibs:Depends=libc6 (>= 0.1), $(p_l64gcc) (>= $(DEB_LIBGCC_SOVERSION))' \
		> debian/$(p_lib64).substvars
    else
      ifeq ($(DEB_TARGET_GNU_CPU),sparc)
	echo 'shlibs:Depends=libc6-sparc64 (>= 2.3.2.ds1-21), $(p_l64gcc) (>= $(DEB_LIBGCC_SOVERSION))' \
		> debian/$(p_lib64).substvars
      else
	dh_shlibdeps -p$(p_lib64) -L $(p_l64gcc) -l $(d_l64gcc)/lib
      endif
    endif
  endif
endif
	dh_gencontrol -p$(p_lib64) -u-v$(DEB_VERSION)

	b=lib64stdc++; \
	for ext in preinst postinst prerm postrm; do \
	  for t in ''; do \
	    v=$(CXX_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done

	dh_installdeb -p$(p_lib64)
	dh_md5sums -p$(p_lib64)
	dh_builddeb -p$(p_lib64)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-lib32stdcxx: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_lib32)
	dh_installdirs -p$(p_lib32) \
		$(docdir)/$(p_lib32) \
		emul/ia32-linux/usr/lib

	mv $(d)/$(PF)/lib32/libstdc++.so.* \
		$(d_lib32)/emul/ia32-linux/usr/lib/.

	dh_installdocs -p$(p_lib32)
	echo "See /$(docdir)/$(p_base) for more information" \
		> $(d_lib32)/$(docdir)/$(p_lib32)/README.Debian
	dh_installchangelogs -p$(p_lib32)
	debian/dh_rmemptydirs -p$(p_lib32)
	dh_strip -p$(p_lib32)
	dh_compress -p$(p_lib32)
	dh_fixperms -p$(p_lib32)
	dh_gencontrol -p$(p_lib32) -u-v$(DEB_VERSION)

	b=lib32stdc++; \
	for ext in preinst postinst prerm postrm; do \
	  for t in ''; do \
	    v=$(CXX_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done

	dh_installdeb -p$(p_lib32)
	dh_md5sums -p$(p_lib32)
	dh_builddeb -p$(p_lib32)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
ifeq ($(with_libcxx),yes)
$(binary_stamp)-libstdcxx-dev: $(install_stamp) \
    $(binary_stamp)-libstdcxx debian/README.libstdc++-baseline
else
$(binary_stamp)-libstdcxx-dev: $(install_stamp) debian/README.libstdc++-baseline
endif
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_dev) $(d_pic)
	dh_installdirs -p$(p_dev) $(dirs_dev)
	dh_installdirs -p$(p_pic) $(dirs_pic)
	dh_installdirs -p$(p_dbg) $(dirs_dbg)

	: # - correct libstdc++-v3 file locations
	mv $(d)/$(PF)/$(libdir)/libsupc++.a $(d)/$(gcc_lib_dir)/
	mv $(d)/$(PF)/$(libdir)/libstdc++.{a,so} $(d)/$(gcc_lib_dir)/
	mv $(d)/$(PF)/$(libdir)/libstdc++_pic.a $(d)/$(gcc_lib_dir)/

	rm -f $(d)/$(PF)/$(libdir)/debug/libstdc++_pic.a
	rm -f $(d)/$(PF)/lib64/debug/libstdc++_pic.a

	: # remove precompiled headers
	-find $(d) -type d -name '*.gch' | xargs rm -rf

ifeq ($(with_lib64cxx),yes)
	mv $(d)/$(PF)/lib64/lib*c++*.{a,so} $(d)/$(gcc_lib_dir)/64/.
endif
ifeq ($(biarch_ia32),yes)
	mv $(d)/$(PF)/lib32/lib*c++*.{a,so} $(d)/$(gcc_lib_dir)/32/.
endif

	DH_COMPAT=2 dh_movefiles -p$(p_dev) $(files_dev)
	DH_COMPAT=2 dh_movefiles -p$(p_pic) $(files_pic)
	DH_COMPAT=2 dh_movefiles -p$(p_dbg) $(files_dbg)

	dh_link -p$(p_dev) \
		/$(PF)/$(libdir)/libstdc++.so.$(CXX_SONAME) \
		/$(gcc_lib_dir)/libstdc++.so
ifeq ($(with_lib64cxx),yes)
	dh_link -p$(p_dev) \
		/$(PF)/lib64/libstdc++.so.$(CXX_SONAME) \
		/$(gcc_lib_dir)/64/libstdc++.so
endif
ifeq ($(biarch_ia32),yes)
	dh_link -p$(p_dev) \
		/$(PF)/lib32/libstdc++.so.$(CXX_SONAME) \
		/$(gcc_lib_dir)/32/libstdc++.so
endif

	debian/dh_doclink -p$(p_dev) $(p_base)
	debian/dh_doclink -p$(p_pic) $(p_base)
	debian/dh_doclink -p$(p_dbg) $(p_base)
	cp -p $(srcdir)/libstdc++-v3/ChangeLog \
		$(d_dev)/$(docdir)/$(p_base)/C++/changelog.libstdc++
	cp -p debian/README.libstdc++-baseline \
		$(d_dev)/$(docdir)/$(p_base)/C++/README.libstdc++-baseline
	if [ -f $(buildlibdir)/libstdc++-v3/testsuite/current_symbols.txt ]; \
	then \
	  cp -p $(buildlibdir)/libstdc++-v3/testsuite/current_symbols.txt \
	    $(d_dev)/$(docdir)/$(p_base)/C++/libstdc++_symbols.txt; \
	fi
	cp -p $(srcdir)/libstdc++-v3/config/linker-map.gnu \
		$(d_pic)/$(gcc_lib_dir)/libstdc++_pic.map

ifeq ($(with_cxxdev),yes)
	debian/dh_rmemptydirs -p$(p_dev)
	debian/dh_rmemptydirs -p$(p_pic)
	debian/dh_rmemptydirs -p$(p_dbg)
endif

	dh_strip -p$(p_dev) -p$(p_pic)
	dh_compress -p$(p_dev) -p$(p_pic) -p$(p_dbg) -X.txt
	dh_fixperms -p$(p_dev) -p$(p_pic) -p$(p_dbg)
ifeq ($(with_lib64cxx),yes)
	dh_shlibdeps -p$(p_dev) -p$(p_pic) -p$(p_dbg) -Xlib64
else
	dh_shlibdeps -p$(p_dev) -p$(p_pic) -p$(p_dbg) -Xlib32/debug
endif
	dh_gencontrol -p$(p_dev) -p$(p_pic) -p$(p_dbg) \
		-u-v$(DEB_VERSION)

	b=libstdc++; \
	for ext in preinst postinst prerm postrm; do \
	  for t in -dev -dbg -pic; do \
	    v=$(CXX_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done

	dh_installdeb -p$(p_dev) -p$(p_pic) -p$(p_dbg)
	dh_md5sums -p$(p_dev) -p$(p_pic) -p$(p_dbg)
	dh_builddeb -p$(p_dev) -p$(p_pic) -p$(p_dbg)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------

doxygen_doc_dir = $(buildlibdir)/libstdc++-v3/docs/doxygen

doxygen-docs: $(build_doxygen_stamp)
$(build_doxygen_stamp):
	rm -rf $(doxygen_doc_dir)/html*
	$(MAKE) -C $(buildlibdir)/libstdc++-v3 SHELL=/bin/bash doxygen; \
	$(MAKE) -C $(buildlibdir)/libstdc++-v3 SHELL=/bin/bash doxygen-man
	if [ -f $(doxygen_doc_dir)/html_user/index.html ]; then \
	  sed -e 's,http://gcc\.gnu\.org/onlinedocs/libstdc++,../html,g' \
	      -e 's,<title>Main Page</title>,<title>libstdc++-v3 Source: Main Index</title>,' \
	    $(doxygen_doc_dir)/html_user/index.html \
	      > $(doxygen_doc_dir)/html_user/index.html.new; \
	  mv -f $(doxygen_doc_dir)/html_user/index.html.new \
	      $(doxygen_doc_dir)/html_user/index.html; \
	else \
	  echo "unable to generate doxygen docs"; \
	  exit 2; \
	fi
	touch $@

$(binary_stamp)-libstdcxx-doc: $(install_stamp) doxygen-docs
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_libd)
	dh_installdirs -p$(p_libd) \
		$(docdir)/$(p_base)/libstdc++ \
		$(PF)/share/man

#	debian/dh_doclink -p$(p_libd) $(p_base)
	dh_link -p$(p_libd) /usr/share/doc/$(p_base) /usr/share/doc/$(p_libd)
	dh_installdocs -p$(p_libd)
	rm -f $(d_libd)/$(docdir)/$(p_base)/copyright

	cp -a $(srcdir)/libstdc++-v3/docs/html \
		$(d_libd)/$(docdir)/$(p_base)/libstdc++/.
	ln -sf documentation.html \
		$(d_libd)/$(docdir)/$(p_base)/libstdc++/html/index.html
	-find $(d_libd)/$(docdir)/$(p_base)/libstdc++/ -name CVS -type d \
		| xargs rm -rf

	cp -a $(doxygen_doc_dir)/html_user \
		$(d_libd)/$(docdir)/$(p_base)/libstdc++/.
	cp -a $(doxygen_doc_dir)/man/man3 \
		$(d_libd)/$(PF)/share/man/.
	cp -p $(srcdir)/libstdc++-v3/docs/doxygen/Intro.3 \
		$(d_libd)/$(PF)/share/man/man3/C++Intro.3

#	mkdir -p $(d_libd)/usr/share/lintian/overrides
#	cp -p debian/$(p_libd).overrides \
#		$(d_libd)/usr/share/lintian/overrides/$(p_libd)

	dh_compress -p$(p_libd) -Xhtml/17_intro -X.txt -X.tag
	dh_fixperms -p$(p_libd)
	dh_gencontrol -p$(p_libd) -u-v$(DEB_VERSION)

	dh_installdeb -p$(p_libd)
	dh_md5sums -p$(p_libd)
	dh_builddeb -p$(p_libd)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
