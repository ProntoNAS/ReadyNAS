arch_binaries  := $(arch_binaries) libffi

p_ffi	= libffi$(FFI_SONAME)
p_ffid	= libffi$(FFI_SONAME)-dev

d_ffi	= debian/$(p_ffi)
d_ffid	= debian/$(p_ffid)

dirs_ffi = \
	$(docdir)/$(p_ffi) \
	$(PF)/$(libdir)
files_ffi = \
	$(PF)/$(libdir)/libffi.so.*

dirs_ffid = \
	$(docdir) \
	$(PF)/include
files_ffid = \
	$(addprefix $(PF)/include/, ffi.h ffi_mips.h fficonfig.h) \
	$(PF)/$(libdir)/libffi.{a,so,la}

ifeq ($(with_lib64ffi),yes)
	dirs_ffi   += $(PF)/$(lib64)
	files_ffi  += $(PF)/$(lib64)/libffi.so.*
	dirs_ffid  += $(PF)/$(lib64)
	files_ffid += $(PF)/$(lib64)/libffi.{a,so,la}
endif

$(binary_stamp)-libffi: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_ffi) $(d_ffid)
	dh_installdirs -p$(p_ffi) $(dirs_ffi)
	dh_installdirs -p$(p_ffid) $(dirs_ffid)

	dh_movefiles -p$(p_ffi) $(files_ffi)
	dh_movefiles -p$(p_ffid) $(files_ffid)

	dh_installdocs -p$(p_ffi) $(srcdir)/libffi/README
	dh_installchangelogs -p$(p_ffi) $(srcdir)/libffi/ChangeLog
	cp -p $(srcdir)/libffi/LICENSE $(d_ffi)/$(docdir)/$(p_ffi)/copyright
	cp -p $(srcdir)/libffi/ChangeLog.libgcj \
		$(d_ffi)/$(docdir)/$(p_ffi)/changelog.libgcj
	debian/dh_doclink -p$(p_ffid) $(p_ffi)

	debian/dh_rmemptydirs -p$(p_ffi)
	debian/dh_rmemptydirs -p$(p_ffid)

	dh_strip -p$(p_ffi) -p$(p_ffid)
	dh_compress -p$(p_ffi) -p$(p_ffid)
	dh_fixperms -p$(p_ffi) -p$(p_ffid)
	DH_COMPAT=3 dh_makeshlibs -p$(p_ffi) \
		-V '$(p_ffi) (>= $(DEB_FFI_SOVERSION))'
	dh_shlibdeps -p$(p_ffi) -p$(p_ffid)
	dh_gencontrol -p$(p_ffi) -p$(p_ffid) -u-v$(DEB_VERSION)
	b=libffi; v=$(FFI_SONAME); \
	for ext in preinst postinst prerm postrm; do \
	  if [ -f debian/$$b$$t.$$ext ]; then \
	    cp -pf debian/$$b$$t.$$ext debian/$$b$$v$$t.$$ext; \
	  fi; \
	done
	dh_installdeb -p$(p_ffi) -p$(p_ffid)
	dh_md5sums -p$(p_ffi) -p$(p_ffid)
	dh_builddeb -p$(p_ffi) -p$(p_ffid)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
