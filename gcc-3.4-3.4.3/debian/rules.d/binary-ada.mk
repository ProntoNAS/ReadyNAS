arch_binaries := $(arch_binaries) ada
ifneq ($(GFDL_INVARIANT_FREE),yes)
  indep_binaries := $(indep_binaries) ada-doc
endif

ifeq ($(with_libgnat),yes)
  arch_binaries := $(arch_binaries) libgnat
endif

p_gnat	= gnat-$(GNAT_VERSION)
p_lgnat	= libgnat-$(GNAT_VERSION)
p_gnatd	= $(p_gnat)-doc

d_gnat	= debian/$(p_gnat)
d_lgnat	= debian/$(p_lgnat)
d_gnatd	= debian/$(p_gnatd)

GNAT_TOOLS = gnat gnatbind gnatbl gnatchop gnatclean gnatfind gnatkr gnatlink \
		gnatls gnatmake gnatname gnatprep gnatxref gpr2make gprcmd

dirs_gnat = \
	$(docdir)/$(p_base)/Ada \
	$(PF)/bin \
	$(PF)/share/man/man1 \
	$(gcc_lib_dir) \
	$(gcc_lexec_dir) \
	$(PF)/$(libdir)/gnat

files_gnat = \
	$(PF)/share/gnat \
	$(gcc_lexec_dir)/gnat1 \
	$(gcc_lib_dir)/{adalib,adainclude} \
	$(foreach i,$(GNAT_TOOLS),$(PF)/bin/$(i))

#	$(foreach i,$(GNAT_TOOLS),$(PF)/bin/$(i)-$(GNAT_VERSION))

dirs_lgnat = \
	$(docdir) \
	$(PF)/lib
files_lgnat = \
	$(PF)/$(libdir)/lib{gnat,gnarl}-$(GNAT_VERSION).so.1

update-ada-files:
	cd $(builddir) && tar cfj ada-generated.tar.bz2 \
		gcc/ada/{sinfo.h,einfo.h,nmake.ads,nmake.adb,treeprs.ads}
	uuencode $(builddir)/ada-generated.tar.bz2 ada-generated.tar.bz2 \
		> debian/patches/ada-generated.uue

$(binary_stamp)-libgnat: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_lgnat)
	dh_installdirs -p$(p_lgnat) $(dirs_lgnat)

	for lib in lib{gnat,gnarl}; do \
	  vlib=$$lib-$(GNAT_VERSION); \
	  mv $(d)/$(gcc_lib_dir)/adalib/$$vlib.so.1 $(d)/$(PF)/$(libdir)/. ; \
	  rm -f $(d)/$(gcc_lib_dir)/adalib/$$lib.so.1; \
	done

	DH_COMPAT=2 dh_movefiles -p$(p_lgnat) $(files_lgnat)

	debian/dh_doclink -p$(p_lgnat) $(p_base)

	debian/dh_rmemptydirs -p$(p_lgnat)

	dh_strip -p$(p_lgnat)
	dh_compress -p$(p_lgnat)
	dh_fixperms -p$(p_lgnat)
	b=libgnat; \
	for ext in preinst postinst prerm postrm; do \
	  for t in '' -dev -dbg; do \
	    v=$(GNAT_VERSION); \
	    if [ -f debian/$$b$$t.$$ext ]; then \
	      cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	    fi; \
	  done; \
	done

	dh_makeshlibs -p$(p_lgnat) -V '$(p_lgnat) (>= $(DEB_SOVERSION))'
	cat debian/$(p_lgnat)/DEBIAN/shlibs >> debian/shlibs.local

	dh_shlibdeps -p$(p_lgnat)
	dh_gencontrol -p$(p_lgnat) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_lgnat)
	dh_md5sums -p$(p_lgnat)
	dh_builddeb -p$(p_lgnat)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)

ifeq ($(with_libgnat),yes)
$(binary_stamp)-ada: $(install_stamp) $(binary_stamp)-libgnat
else
$(binary_stamp)-ada: $(install_stamp)
endif
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gnat)
	dh_installdirs -p$(p_gnat) $(dirs_gnat)

	DH_COMPAT=2 dh_movefiles -p$(p_gnat) $(files_gnat)

ifeq ($(with_libgnat),yes)
	for lib in lib{gnat,gnarl}; do \
	  vlib=$$lib-$(GNAT_VERSION); \
	  dh_link -p$(p_gnat) \
	    /$(PF)/$(libdir)/$$vlib.so.1 /$(gcc_lib_dir)/adalib/$$vlib.so \
	    /$(PF)/$(libdir)/$$vlib.so.1 /$(gcc_lib_dir)/adalib/$$lib.so; \
	done
endif

	debian/dh_doclink -p$(p_gnat) $(p_base)

	cp -p $(srcdir)/gcc/ada/ChangeLog \
		$(d_gnat)/$(docdir)/$(p_base)/Ada/changelog

	for i in $(GNAT_TOOLS); do \
	  case "$$i" in \
	    gnat) cp -p debian/gnat.1 $(d_gnat)/$(PF)/share/man/man1/ ;; \
	    *) ln -sf gnat.1 $(d_gnat)/$(PF)/share/man/man1/$$i.1; \
	  esac; \
	done

	ln -sf gcc$(pkg_ver) $(d_gnat)/$(PF)/bin/gnatgcc
	ln -sf gcc$(pkg_ver).1.gz $(d_gnat)/$(PF)/share/man/man1/gnatgcc.1.gz

	debian/dh_rmemptydirs -p$(p_gnat)

	dh_strip -p$(p_gnat)
	dh_compress -p$(p_gnat)
	dh_fixperms -p$(p_gnat)
ifeq ($(with_libgnat),yes)
	dh_shlibdeps -p$(p_gnat) -L $(p_lgnat) -l $(d_lgnat)/$(PF)/$(libdir)
else
	dh_shlibdeps -p$(p_gnat)
endif
	dh_gencontrol -p$(p_gnat) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_gnat)
	dh_md5sums -p$(p_gnat)
	dh_builddeb -p$(p_gnat)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)


ada_info_dir = $(d_gnatd)/$(PF)/share/info

$(binary_stamp)-ada-doc: $(build_html_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_gnatd)
	dh_installdirs -p$(p_gnatd) \
		$(docdir)/$(p_base)/Ada \
		$(PF)/share/info

	cd $(ada_info_dir) && \
	    makeinfo -I $(srcdir)/gcc/doc/include -I $(srcdir)/gcc/ada \
		-o gnat_ugn_unw-$(GNAT_VERSION).info \
		$(builddir)/gcc/doc/gnat_ugn_unw.texi
	cd $(ada_info_dir) && \
	    makeinfo -I $(srcdir)/gcc/doc/include -I $(srcdir)/gcc/ada \
		-o gnat_rm-$(GNAT_VERSION).info \
		$(srcdir)/gcc/ada/gnat_rm.texi
	cd $(ada_info_dir) && \
	    makeinfo -I $(srcdir)/gcc/doc/include -I $(srcdir)/gcc/ada \
		-o gnat-style-$(GNAT_VERSION).info \
		$(srcdir)/gcc/ada/gnat-style.texi

	debian/dh_doclink -p$(p_gnatd) $(p_base)
	dh_installdocs -p$(p_gnatd)
	rm -f $(d_gnatd)/$(docdir)/$(p_base)/copyright
	cp -p html/gnat_ugn_unw.html html/gnat_rm.html html/gnat-style.html \
		$(d_gnatd)/$(docdir)/$(p_base)/Ada/

	dh_compress -p$(p_gnatd)
	dh_fixperms -p$(p_gnatd)
	dh_installdeb -p$(p_gnatd)
	dh_gencontrol -p$(p_gnatd) -u-v$(DEB_VERSION)
	dh_md5sums -p$(p_gnatd)
	dh_builddeb -p$(p_gnatd)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
