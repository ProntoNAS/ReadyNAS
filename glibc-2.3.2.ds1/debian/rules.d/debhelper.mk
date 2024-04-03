# This is so horribly wrong.  libc-pic does a whole pile of gratuitous
# renames.  There's very little we can do for now.  Maybe after
# Sarge releases we can consider breaking packages, but certainly not now.

$(stamp)binaryinst_$(libc)-pic:: $(stamp)debhelper
	@echo Running special kludge for $(libc)-pic
	dh_testroot
	dh_installdirs -p$(curpass)
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/libc_pic.a debian/$(libc)-pic/usr/lib/.
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/libc.map debian/$(libc)-pic/usr/lib/libc_pic.map
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/elf/soinit.os debian/$(libc)-pic/usr/lib/libc_pic/soinit.o
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/elf/sofini.os debian/$(libc)-pic/usr/lib/libc_pic/sofini.o

	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/math/libm_pic.a debian/$(libc)-pic/usr/lib/.
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/libm.map debian/$(libc)-pic/usr/lib/libm_pic.map
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/resolv/libresolv_pic.a debian/$(libc)-pic/usr/lib/.
	install --mode=0644 build-tree/$(DEB_HOST_ARCH)-libc/libresolv.map debian/$(libc)-pic/usr/lib/libresolv_pic.map

# Some per-package extra files to install.
define $(libc)_extra_debhelper_pkg_install
	install --mode=0644 $(DEB_SRCDIR)/ChangeLog debian/$(curpass)/usr/share/doc/$(curpass)/changelog
	install --mode=0644 $(DEB_SRCDIR)/linuxthreads/README debian/$(curpass)/usr/share/doc/$(curpass)/README.linuxthreads
	install --mode=0644 $(DEB_SRCDIR)/linuxthreads/ChangeLog debian/$(curpass)/usr/share/doc/$(curpass)/ChangeLog.linuxthreads
	case " $(GLIBC_PASSES) " in \
	*" nptl "*) \
	  install --mode=0644 $(DEB_SRCDIR)/nptl/ChangeLog debian/$(curpass)/usr/share/doc/$(curpass)/ChangeLog.nptl; \
	  ;; \
	esac
	# dh_installmanpages thinks that .so is a language.
	install --mode=0644 debian/local/manpages/ld.so.8 debian/$(curpass)/usr/share/man/man8/ld.so.8

	install --mode=0644 debian/FAQ debian/$(curpass)/usr/share/doc/$(curpass)/README.Debian
endef

define locales_extra_debhelper_pkg_install
	install --mode=0644 $(DEB_SRCDIR)/localedata/ChangeLog debian/$(curpass)/usr/share/doc/$(curpass)/changelog
endef

define glibc-doc_extra_debhelper_pkg_install
	install --mode=0644 $(DEB_SRCDIR)/ChangeLog debian/$(curpass)/usr/share/doc/$(curpass)/changelog
	install --mode=0644 $(DEB_SRCDIR)/linuxthreads/FAQ.html debian/$(curpass)/usr/share/doc/$(curpass)/FAQ.linuxthreads.html
endef

# Should each of these have per-package options?

$(patsubst %,binaryinst_%,$(DEB_ARCH_REGULAR_PACKAGES) $(DEB_INDEP_REGULAR_PACKAGES)) :: binaryinst_% : $(stamp)binaryinst_%

# Make sure the debug packages are built last, since other packages may add
# files to them.
debug-packages = $(filter %-dbg,$(DEB_ARCH_REGULAR_PACKAGES) $(DEB_INDEP_REGULAR_PACKAGES))
non-debug-packages = $(filter-out %-dbg,$(DEB_ARCH_REGULAR_PACKAGES) $(DEB_INDEP_REGULAR_PACKAGES))
$(patsubst %,$(stamp)binaryinst_%,$(debug-packages)):: $(patsubst %,$(stamp)binaryinst_%,$(non-debug-packages))

$(patsubst %,$(stamp)binaryinst_%,$(DEB_ARCH_REGULAR_PACKAGES) $(DEB_INDEP_REGULAR_PACKAGES)):: $(stamp)debhelper
	@echo Running debhelper for $(curpass)
	dh_testroot
	dh_installdirs -p$(curpass)
	dh_install -p$(curpass)
	dh_installman -p$(curpass)
	dh_installinfo -p$(curpass)
	dh_installdebconf -p$(curpass)
	dh_installchangelogs -p$(curpass)
	dh_installinit -p$(curpass)
	dh_installdocs -p$(curpass) 
	dh_link -p$(curpass)

	# extra_debhelper_pkg_install is used for debhelper.mk only.
	# when you want to install extra packages, use extra_pkg_install.
	$(call xx,extra_debhelper_pkg_install)
	$(call xx,extra_pkg_install)

ifeq ($(filter nostrip,$(DEB_BUILD_OPTIONS)),)
	# libpthread must be stripped specially; GDB needs the
	# non-dynamic symbol table in order to load the thread
	# debugging library.  We keep a full copy of the symbol
	# table in libc6-dbg but basic thread debugging should
	# work even without that package installed.

	# We use a wrapper script so that we only include the bare
	# minimum in /usr/lib/debug/lib for backtraces; anything
	# else takes too long to load in GDB.

	if test "$(NOSTRIP_$(curpass))" != 1; then			\
	  chmod a+x debian/wrapper/objcopy;				\
	  export PATH=$(shell pwd)/debian/wrapper:$$PATH;		\
	  dh_strip -p$(curpass) -Xlibpthread --keep-debug;		\
	  mkdir -p debian/$(libc)-dbg/usr/lib/debug;			\
	  if test -d debian/$(curpass)/usr/lib/debug; then		\
	    cd debian/$(curpass)/usr/lib/debug;				\
	    find . -type f -name \*.so\*				\
	      | cpio -pd $(shell pwd)/debian/$(libc)-dbg/usr/lib/debug;	\
	    cd ../../../../..;						\
	    rm -rf debian/$(curpass)/usr/lib/debug;			\
	  fi;								\
	  (cd debian/$(curpass);					\
	   find . -name libpthread-\*.so -exec				\
	     ../../debian/wrapper/objcopy --only-keep-debug '{}'	\
	     ../$(libc)-dbg/usr/lib/debug/'{}' ';' || true;		\
	   find . -name libpthread-\*.so -exec objcopy			\
	     --add-gnu-debuglink=../$(libc)-dbg/usr/lib/debug/'{}'	\
	     '{}' ';' || true);						\
	  find debian/$(curpass) -name libpthread-\*.so -exec		\
	    strip --strip-debug --remove-section=.comment		\
	    --remove-section=.note '{}' ';' || true;			\
	fi
endif

	dh_compress -p$(curpass)
	dh_fixperms -p$(curpass) -Xpt_chown
	# Use this instead of -X to dh_fixperms so that we can use
	# an unescaped regular expression.  ld.so must be executable;
	# libc.so and NPTL's libpthread.so print useful version
	# information when executed.
	# FIXME: LinuxThread's libpthread.so doesn't.  It would be good
	# to either fix that, or use a more robust method than searching
	# for /tls/ in the path to identify NPTL.
	find debian/$(curpass) -type f \( -regex '.*lib.*/ld.*so.*' \
		-o -regex '.*lib.*/tls/.*libpthread.*so.*' \
		-o -regex '.*lib.*/libc[.-].*so.*' \) \
		-exec chmod a+x '{}' ';'
	dh_makeshlibs -p$(curpass) -V "$(call xx,shlib_dep)"

	dh_installdeb -p$(curpass)
	# dh_shlibdeps -p$(curpass)
	dh_gencontrol -p$(curpass) -- $($(curpass)_control_flags)
	dh_md5sums -p$(curpass)
	dh_builddeb -p$(curpass)

	touch $@

$(patsubst %,binaryinst_%,$(DEB_UDEB_PACKAGES)) :: binaryinst_% : $(stamp)binaryinst_%
$(patsubst %,$(stamp)binaryinst_%,$(DEB_UDEB_PACKAGES)): $(stamp)debhelper
	@echo Running debhelper for $(curpass)
	dh_testroot
	dh_installdirs -p$(curpass)
	dh_install -p$(curpass)
	dh_strip -p$(curpass)
	dh_compress -p$(curpass)
	dh_fixperms -p$(curpass)
	find debian/$(curpass) -type f \( -regex '.*lib.*/ld.*so.*' \
		-o -regex '.*lib.*/tls/.*libpthread.*so.*' \
		-o -regex '.*lib.*/libc[.-].*so.*' \) \
		-exec chmod a+x '{}' ';'
	# dh_makeshlibs -p$(curpass) -V "$(call xx,shlib_dep)"
	dh_installdeb -p$(curpass)
	# dh_shlibdeps -p$(curpass)
	dh_gencontrol -p$(curpass)
	dh_builddeb -p$(curpass)

	touch $@

#Ugly kludge:
# I'm running out of time to get this sorted out properly.  Basically
# the problem is that nptl is like an optimised library, but not quite.
# So we'll filter it out of the passes list and deal with it specifically.
#
# Ideally, there should be some way of having an optimisation pass and
# say "include this in the main library" by setting a variable.
# But after 10 hours of staring at this thing, I can't figure it out.

OPT_PASSES = $(filter-out libc nptl,$(GLIBC_PASSES))
OPT_DESTDIRS = $(foreach pass,$(OPT_PASSES),$($(pass)_LIBDIR))
NPTL = $(filter nptl,$(GLIBC_PASSES))

debhelper: $(stamp)debhelper
$(stamp)debhelper:

	for x in `find debian/debhelper.in -type f -maxdepth 1`; do \
	  y=debian/`basename $$x`; \
	  z=`echo $$y | sed -e 's#/libc#/$(libc)#'`; \
	  cp $$x $$z; \
	  sed -e "s#TMPDIR#debian/tmp-libc#" -i $$z; \
	  sed -e "s#DEB_SRCDIR#$(DEB_SRCDIR)#" -i $$z; \
	  sed -e "s#LIBC#$(libc)#" -i $$z; \
	  case $$z in \
	    *.install) sed -e "s/^#.*//" -i $$z ;; \
	  esac; \
	done

	# Hack: special-case passes whose destdir is 64 (i.e. /lib64)
	# to use a different install template, which includes more
	# libraries.  Also generate a -dev.  Non-64 libraries get scripts
	# to temporarily disable hwcap.  This needs some cleaning up.
	set -- $(OPT_DESTDIRS); \
	for x in $(OPT_PASSES); do \
	  destdir=$$1; \
	  shift; \
	  z=debian/$(libc)-$$x.install; \
	  if test $$destdir = 64; then \
	    cp debian/debhelper.in/libc-alt.install $$z; \
	    zd=debian/$(libc)-dev-$$x.install; \
	    cp debian/debhelper.in/libc-alt-dev.install $$zd; \
	    sed -e "s#TMPDIR#debian/tmp-$$x#" -i $$zd; \
	    sed -e "s#DEB_SRCDIR#$(DEB_SRCDIR)#" -i $$zd; \
	    sed -e "s#LIBC#$(libc)#" -i $$z; \
	    sed -e "s#DESTLIBDIR#$$destdir#" -i $$zd; \
	    sed -e "s/^#.*//" -i $$zd; \
	  else \
	    cp debian/debhelper.in/libc-otherbuild.install $$z; \
	    cp debian/debhelper.in/libc-otherbuild.preinst debian/$(libc)-$$x.preinst ; \
	    cp debian/debhelper.in/libc-otherbuild.postinst debian/$(libc)-$$x.postinst ; \
	    sed -e "s#OPT#$$x#" -i debian/$(libc)-$$x.preinst; \
	    sed -e "s#OPT#$$x#" -i debian/$(libc)-$$x.postinst; \
	  fi; \
	  sed -e "s#TMPDIR#debian/tmp-$$x#" -i $$z; \
	  sed -e "s#DEB_SRCDIR#$(DEB_SRCDIR)#" -i $$z; \
	  sed -e "s#DESTLIBDIR#$$destdir#" -i $$z; \
	  sed -e "s/^#.*//" -i $$z; \
	done

	# We use libc-otherbuild for this, since it's just a special case of
	# an optimised library that needs to wind up in /lib/tls
	# FIXME: We do not cover the case of processor optimised 
	# nptl libraries, like /lib/i686/tls
	# We probably don't care for now.
	for x in $(NPTL); do \
	  z=debian/$(libc).install; \
	  cat debian/debhelper.in/libc-otherbuild.install >>$$z; \
	  sed -e "s#TMPDIR#debian/tmp-$$x#" -i $$z; \
	  sed -e "s#DEB_SRCDIR#$(DEB_SRCDIR)#" -i $$z; \
	  sed -e "s#DESTLIBDIR#/tls#" -i $$z; \
	  case $$z in \
	    *.install) sed -e "s/^#.*//" -i $$z ;; \
	  esac; \
	done

	# Substitute __SUPPORTED_LOCALES__.
	perl -i -pe 'BEGIN {undef $$/; open(IN, "debian/tmp-libc/usr/share/i18n/SUPPORTED"); $$j=<IN>;} s/__SUPPORTED_LOCALES__/$$j/g;' debian/locales.config

	# Generate common substvars files.
	echo "locale:Depends=$(shell perl debian/debver2localesdep.pl $(DEB_VERSION))" > tmp.substvars

	for pkg in $(DEB_ARCH_REGULAR_PACKAGES) $(DEB_INDEP_REGULAR_PACKAGES) $(DEB_UDEB_PACKAGES); do \
	  cp tmp.substvars debian/$$pkg.substvars; \
	done
	rm -f tmp.substvars

	touch $(stamp)debhelper

debhelper-clean:
	dh_clean 

	rm -f debian/*.install*
	rm -f debian/*.install.*
	rm -f debian/*.manpages
	rm -f debian/*.links
	rm -f debian/*.postinst
	rm -f debian/*.preinst
	rm -f debian/*.postinst
	rm -f debian/*.prerm
	rm -f debian/*.postrm
	rm -f debian/*.info
	rm -f debian/*.init
	rm -f debian/*.config
	rm -f debian/*.templates
	rm -f debian/*.dirs
	rm -f debian/*.docs
	rm -f debian/*.doc-base

	rm -f $(stamp)binaryinst*
