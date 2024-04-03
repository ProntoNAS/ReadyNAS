ifeq ($(with_java),yes)
  arch_binaries  := $(arch_binaries) java
  indep_binaries := $(indep_binaries) gcjjar
endif

ifeq ($(with_common_libs),yes)
  indep_binaries  := $(indep_binaries) libgcj-common
endif

ifeq ($(with_javadev),yes)
  arch_binaries  := $(arch_binaries) javadev
endif

p_gcj	= gcj$(pkg_ver)
p_gij	= gij$(pkg_ver)
p_jcom	= libgcj-common
p_jlib	= libgcj$(GCJ_SONAME)
p_jlibc	= libgcj$(GCJ_SONAME)-common
p_jlibx	= libgcj$(GCJ_SONAME)-awt
p_jdev	= libgcj$(GCJ_SONAME)-dev

d_gcj	= debian/$(p_gcj)
d_gij	= debian/$(p_gij)
d_jcom	= debian/$(p_jcom)
d_jlib	= debian/$(p_jlib)
d_jlibc	= debian/$(p_jlibc)
d_jlibx	= debian/$(p_jlibx)
d_jdev	= debian/$(p_jdev)

dirs_gcj = \
	$(docdir)/$(p_base)/java \
	$(PF)/bin \
	$(PF)/share/man/man1 \
	$(PF)/share/info \
	$(gcc_lexec_dir)
files_gcj = \
	$(PF)/bin/{gcj,gcjh,jv-convert,jv-scan,jcf-dump,rmic}$(pkg_ver) \
	$(PF)/share/man/man1/{gcj,gcjh,jv-convert,jv-scan,jcf-dump,rmic}$(pkg_ver).1 \
	$(gcc_lexec_dir)/{jc1,jvgenmain}

ifneq ($(GFDL_INVARIANT_FREE),yes)
  files_gcj += \
	$(PF)/share/info/gcj*
endif

dirs_gij = \
	$(docdir)/$(p_base)/java \
	$(PF)/bin \
	$(PF)/share/man/man1

files_gij = \
	$(PF)/bin/{gij,rmiregistry}$(pkg_ver) \
	$(PF)/share/man/man1/{gij,rmiregistry}$(pkg_ver).1

dirs_jcom = \
	$(PF)/$(libdir)

files_jcom = \
	$(PF)/$(libdir)/security

dirs_jlib = \
	$(docdir)/$(p_jlib) \
	$(PF)/$(libdir)

files_jlib = \
	$(PF)/$(libdir)/libgcj*.so.* \
	$(PF)/$(libdir)/lib-org-*.so.*

dirs_jlibc = \
	$(docdir)/$(p_jlib) \
	$(PF)/share/java

files_jlibc = \
	$(PF)/share/java/libgcj-$(VER).jar

dirs_jlibx = \
	$(docdir)/$(p_jlib) \
	$(PF)/$(libdir) \
	$(PF)/share/java

files_jlibx = \
	$(PF)/$(libdir)/lib-gnu-awt*.so.* \

dirs_jdev = \
	$(docdir)/$(p_jlib) \
	$(PF)/include \
	$(PF)/$(libdir) \
	$(gcc_lib_dir)/include/gcj

files_jdev = \
	$(PF)/include/{gcj,java,javax,jni.h,jvmpi.h} \
	$(PF)/include/gnu/{awt,classpath,gcj,java} \
	$(PF)/$(libdir)/libgcj*.{a,la} \
	$(PF)/$(libdir)/{libgcj*.so,libgcj.spec} \
	$(gcc_lib_dir)/include/gcj/libgcj-config.h \
	$(PF)/$(libdir)/lib-gnu-*.{a,la} \
	$(PF)/$(libdir)/lib-gnu-*.so \
	$(PF)/$(libdir)/lib-org-*.{a,la} \
	$(PF)/$(libdir)/lib-org-*.so

ifeq ($(with_lib64gcj),yes)
	dirs_jlib    +=	$(PF)/$(lib64)
	files_jlib   +=	$(PF)/$(lib64)/libgcj*.so.* \
			$(PF)/$(lib64)/lib-gnu-*.so.* \
			$(PF)/$(lib64)/lib-org-*.so.*

	dirs_jdev    += $(PF)/$(lib64)
	files_jdev   += $(PF)/$(lib64)/libgcj*.{a,so,la} \
			$(PF)/$(lib64)/lib-gnu-*.{a,so,la} \
			$(PF)/$(lib64)/lib-org-*.{a,so,la}
endif

# ----------------------------------------------------------------------
$(binary_stamp)-libgcj-common: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	dh_installdirs -p$(p_jcom) $(dirs_jcom)
	[ -d $(d)/$(PF)/$(libdir)/security ] || mkdir -p $(d)/$(PF)/$(libdir)/security
	[ -f $(d)/$(PF)/$(libdir)/security/classpath.security ] || \
	  cp $(srcdir)/libjava/java/security/*.security \
	    $(d)/$(PF)/$(libdir)/security/.
	dh_movefiles -p$(p_jcom) $(files_jcom)
	debian/dh_doclink -p$(p_jcom) $(p_base)
	debian/dh_rmemptydirs -p$(p_jcom)
	dh_compress -p$(p_jcom)
	dh_fixperms -p$(p_jcom)
	dh_gencontrol -p$(p_jcom) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_jcom)
	dh_md5sums -p$(p_jcom)
	dh_builddeb -p$(p_jcom)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-gcjjar: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	dh_installdirs -p$(p_jlibc) $(dirs_jlibc)
	DH_COMPAT=2 dh_movefiles -p$(p_jlibc) $(files_jlibc)
	debian/dh_doclink -p$(p_jlibc) $(p_base)
	debian/dh_rmemptydirs -p$(p_jlibc)
	dh_compress -p$(p_jlibc)
	dh_fixperms -p$(p_jlibc)
	dh_gencontrol -p$(p_jlibc) -u-v$(DEB_VERSION)
	cp -p debian/libgcj4-common.preinst.in debian/libgcj4-common.preinst
	dh_installdeb -p$(p_jlibc)
	dh_md5sums -p$(p_jlibc)
	dh_builddeb -p$(p_jlibc)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)


# ----------------------------------------------------------------------
$(binary_stamp)-java: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	dh_installdirs -p$(p_gij)  $(dirs_gij)
	dh_installdirs -p$(p_jlib) $(dirs_jlib)
	dh_installdirs -p$(p_jlibx) $(dirs_jlibx)

	dh_movefiles -p$(p_gij)  $(files_gij)
	dh_movefiles -p$(p_jlib) $(files_jlib)
	dh_movefiles -p$(p_jlibx) $(files_jlibx)

	debian/dh_doclink -p$(p_gij) $(p_base)
	dh_installdocs -p$(p_jlib) $(srcdir)/libjava/{NEWS,README,THANKS}
	dh_installchangelogs -p$(p_jlib)
	debian/dh_doclink -p$(p_jlibx) $(p_jlib)

	cp -p debian/gij-wrapper $(d_gij)/$(PF)/bin/gij-wrapper$(pkg_ver)
	chmod 755 $(d_gij)/$(PF)/bin/gij-wrapper$(pkg_ver)
	cp -p debian/gij-wrapper.1 \
		$(d_gij)/$(PF)/share/man/man1/gij-wrapper$(pkg_ver).1

	debian/dh_rmemptydirs -p$(p_gij)
	debian/dh_rmemptydirs -p$(p_jlib)
	debian/dh_rmemptydirs -p$(p_jlibx)

	DH_COMPAT=3 dh_makeshlibs -p$(p_jlib) \
		-V '$(p_jlib) (>= $(DEB_SOVERSION))'
	cat debian/$(p_jlib)/DEBIAN/shlibs >> debian/shlibs.local

	dh_makeshlibs -p$(p_jlibx) -V '$(p_jlibx) (>= $(DEB_SOVERSION))'

	dh_strip -p$(p_gij) -p$(p_jlib) -p$(p_jlibx)
	dh_compress -p$(p_gij) -p$(p_jlib) -p$(p_jlibx)
	dh_fixperms -p$(p_gij) -p$(p_jlib) -p$(p_jlibx)
# the libstdc++ binary packages aren't built yet ...
	echo 'libstdc++ $(CXX_SONAME) $(p_lib) (>= $(DEB_STDCXX_SOVERSION))' \
	    >> debian/shlibs.local
	dh_shlibdeps \
		-l:$(d)/$(PF)/$(libdir):$(d_lib)/$(PF)/$(libdir):$(d_jlib)/$(PF)/$(libdir): \
		-p$(p_gij) -p$(p_jlib) -p$(p_jlibx)
	sed -e 's/$(p_jlib)[^,]*//' -e 's/, *,/,/' debian/$(p_jlib).substvars \
		>> debian/$(p_jlib).substvars.tmp \
	    && mv -f debian/$(p_jlib).substvars.tmp debian/$(p_jlib).substvars
	dh_gencontrol \
		-p$(p_gij) -p$(p_jlib) -p$(p_jlibx) \
		-u-v$(DEB_VERSION)
	b=libgcj; \
	for ext in preinst postinst prerm postrm; do \
	  for t in '' -dev; do \
	    v=$(GCJ_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done
	dh_installdeb -p$(p_gij) -p$(p_jlib) -p$(p_jlibx)
	dh_md5sums -p$(p_gij) -p$(p_jlib) -p$(p_jlibx)
	dh_builddeb -p$(p_gij) -p$(p_jlib) -p$(p_jlibx)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

# ----------------------------------------------------------------------
$(binary_stamp)-javadev: $(build_html_stamp) $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gcj)
	dh_installdirs -p$(p_gcj)  $(dirs_gcj)
	dh_installdirs -p$(p_jdev) $(dirs_jdev)

ifeq ($(with_multiarch),yes)
	mv debian/tmp/$(PF)/lib/libgcj.spec debian/tmp/$(PF)/$(libdir)
endif
	dh_movefiles -p$(p_gcj)  $(files_gcj)
	dh_movefiles -p$(p_jdev) $(files_jdev)

	ln -sf gcj$(pkg_ver) \
	    $(d_gcj)/$(PF)/bin/$(DEB_TARGET_GNU_TYPE)-gcj$(pkg_ver)
	ln -sf gcj$(pkg_ver).1 \
	    $(d_gcj)/$(PF)/share/man/man1/$(DEB_TARGET_GNU_TYPE)-gcj$(pkg_ver).1
	ln -sf gcj$(pkg_ver) \
	    $(d_gcj)/$(PF)/bin/$(TARGET_ALIAS)-gcj$(pkg_ver)
	ln -sf gcj$(pkg_ver).1 \
	    $(d_gcj)/$(PF)/share/man/man1/$(TARGET_ALIAS)-gcj$(pkg_ver).1

	debian/dh_doclink -p$(p_gcj) $(p_base)
ifneq ($(GFDL_INVARIANT_FREE),yes)
	dh_installdocs -p$(p_gcj)
	rm -f $(d_gcj)/$(docdir)/$(p_base)/copyright
	cp -p html/gcj.html $(d_gcj)/$(docdir)/$(p_base)/java/
endif
	cp -p $(srcdir)/libjava/doc/cni.sgml $(d_jdev)/$(docdir)/$(p_jlib)/.
	debian/dh_doclink -p$(p_jdev) $(p_jlib)
	cp -p debian/FAQ.gcj $(d_gcj)/$(docdir)/$(p_base)/java/.
	cp -p $(srcdir)/gcc/java/ChangeLog \
	        $(d_gcj)/$(docdir)/$(p_base)/java/changelog
	cp -p $(srcdir)/libjava/ChangeLog \
	        $(d_jdev)/$(docdir)/$(p_jlib)/changelog

	cp -p debian/gcj-wrapper $(d_gcj)/$(PF)/bin/gcj-wrapper$(pkg_ver)
	chmod 755 $(d_gcj)/$(PF)/bin/gcj-wrapper$(pkg_ver)
	cp -p debian/gcj-wrapper.1 \
		$(d_gcj)/$(PF)/share/man/man1/gcj-wrapper$(pkg_ver).1

	cp -p debian/gcjh-wrapper $(d_gcj)/$(PF)/bin/gcjh-wrapper$(pkg_ver)
	chmod 755 $(d_gcj)/$(PF)/bin/gcjh-wrapper$(pkg_ver)
	cp -p debian/gcjh-wrapper.1 \
	       $(d_gcj)/$(PF)/share/man/man1/gcjh-wrapper$(pkg_ver).1

	debian/dh_rmemptydirs -p$(p_gcj)
	debian/dh_rmemptydirs -p$(p_jdev)

	dh_strip -p$(p_gcj) -p$(p_jdev)
	dh_compress -p$(p_gcj) -p$(p_jdev)
	dh_fixperms -p$(p_gcj) -p$(p_jdev)
	dh_shlibdeps -l:$(d)/$(PF)/$(libdir):$(d_jlib)/$(PF)/$(libdir): \
		-p$(p_gcj) -p$(p_jdev)
	dh_gencontrol \
		-p$(p_gcj) -p$(p_jdev) \
		-u-v$(DEB_VERSION)
	b=libgcj; \
	for ext in preinst postinst prerm postrm; do \
	  for t in '' -dev; do \
	    v=$(GCJ_SONAME); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done
	dh_installdeb -p$(p_gcj) -p$(p_jdev)
	dh_md5sums -p$(p_gcj) -p$(p_jdev)
	dh_builddeb -p$(p_gcj) -p$(p_jdev)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
