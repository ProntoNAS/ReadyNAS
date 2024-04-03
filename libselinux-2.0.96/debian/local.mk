############################ -*- Mode: Makefile -*- ###########################
## local.mk --- 
## Author	    : Manoj Srivastava ( srivasta@glaurung.green-gryphon.com ) 
## Created On	    : Sat Nov 15 10:42:10 2003
## Created On Node  : glaurung.green-gryphon.com
## Last Modified By : Manoj Srivastava
## Last Modified On : Tue Sep  1 22:59:42 2009
## Last Machine Used: anzu.internal.golden-gryphon.com
## Update Count	    : 76
## Status	    : Unknown, Use with caution!
## HISTORY	    : 
## Description	    : 
## 
## arch-tag: b07b1015-30ba-4b46-915f-78c776a808f4
## 
###############################################################################

testdir:
	$(testdir)

debian/stamp/BUILD/libselinux1: debian/stamp/build/libselinux1
debian/stamp/INST/libselinux1:  debian/stamp/install/libselinux1
debian/stamp/BIN/libselinux1:   debian/stamp/binary/libselinux1


debian/stamp/INST/libselinux1-dev: debian/stamp/install/libselinux1-dev
debian/stamp/BIN/libselinux1-dev: debian/stamp/binary/libselinux1-dev

debian/stamp/INST/python-selinux: debian/stamp/install/python-selinux
debian/stamp/BIN/python-selinux: debian/stamp/binary/python-selinux

debian/stamp/BUILD/libselinux-ruby1.8: debian/stamp/build/libselinux-ruby1.8
debian/stamp/INST/libselinux-ruby1.8: debian/stamp/install/libselinux-ruby1.8
debian/stamp/BIN/libselinux-ruby1.8: debian/stamp/binary/libselinux-ruby1.8

debian/stamp/INST/selinux-utils: debian/stamp/install/selinux-utils
debian/stamp/BIN/selinux-utils: debian/stamp/binary/selinux-utils


CLN-common::
	$(REASON)
	-test ! -f Makefile || $(MAKE) clean

CLEAN/libselinux1::
	test ! -d $(TMPTOP) || rm -rf $(TMPTOP)

CLEAN/libselinux1-dev::
	test ! -d $(TMPTOP) || rm -rf $(TMPTOP)


CLEAN/python-selinux::
	test ! -d $(TMPTOP) || rm -rf $(TMPTOP)

CLEAN/libselinux-ruby1.8::
	test ! -d $(TMPTOP) || rm -rf $(TMPTOP)

CLEAN/selinux-utils::
	test ! -d $(TMPTOP) || rm -rf $(TMPTOP)


debian/stamp/build/libselinux1:
	$(checkdir)
	$(REASON)
	@test -d debian/stamp/build || mkdir -p debian/stamp/build
	$(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
           LIBSEPOLDIR=$(LIBSEPOLDIR) ARCH=$(DEB_HOST_ARCH) -C src \
            swigify
	$(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
           LIBSEPOLDIR=$(LIBSEPOLDIR) ARCH=$(DEB_HOST_ARCH) all
	$(MAKE) -C src libselinux.pc
	$(check-libraries)
ifeq (,$(strip $(filter nocheck,$(DEB_BUILD_OPTIONS))))
  ifeq ($(DEB_BUILD_GNU_TYPE),$(DEB_HOST_GNU_TYPE))
	$(SHELL) debian/common/get_shlib_ver
  endif
endif
	@echo done > $@

debian/stamp/build/libselinux-ruby1.8:
	$(checkdir)
	$(REASON)
	@test -d debian/stamp/build || mkdir -p debian/stamp/build
	$(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" rubywrap
	$(check-libraries)
	@echo done > $@

debian/stamp/install/libselinux1:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/install || mkdir -p debian/stamp/install
	rm -rf		    $(TMPTOP)
	$(make_directory)   $(TMPTOP)/selinux
	$(make_directory)   $(DOCDIR)
	$(make_directory)   $(LINTIANDIR)
	$(make_directory)   $(LIBDIR)
	$(make_directory)   $(TMPTOP)/DEBIAN
	$(install_file)	    debian/shlibs	     $(TMPTOP)/DEBIAN
	$(MAKE)		    DESTDIR=$(TMPTOP) -C src install
	rm -f		    $(LIBDIR)/libselinux.a
	rm -f		    $(LIBDIR)/libselinux.so
	rm -rf		    $(TMPTOP)/usr/lib/
	chmod 0644          $(LIBDIR)/libselinux.so.1
	$(install_file)	    debian/changelog	     $(DOCDIR)/changelog.Debian
	$(install_file)	    ChangeLog		     $(DOCDIR)/changelog
	gzip -9fqr	    $(DOCDIR)/
# Make sure the copyright file is not compressed
	$(install_file)	     debian/copyright	     $(DOCDIR)/copyright
	$(install_file)	     debian/lintian.overrides $(LINTIANDIR)/$(package)
	$(strip-lib)
	@echo done > $@

debian/stamp/install/libselinux1-dev:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/install || mkdir -p debian/stamp/install
	rm -rf		    $(TMPTOP)
	$(make_directory)   $(TMPTOP)
	$(make_directory)   $(DOCDIR)
	$(make_directory)   $(LIBDIR)
	$(make_directory)   $(INCDIR)
	$(make_directory)   $(MAN3DIR)
	$(make_directory)   $(MAN5DIR)
	$(make_directory)   $(MAN8DIR)
	$(MAKE)		    DESTDIR=$(TMPTOP) -C man install
	rm -rf		    $(MAN5DIR)
	rm -rf		    $(MAN8DIR)
	$(MAKE)		    DESTDIR=$(TMPTOP) -C include install
	$(MAKE)		    DESTDIR=$(TMPTOP) -C src install
	rm -fr		    $(LIBDIR)
	rm -f		    $(TMPTOP)/usr/lib/libselinux.so
	ln -s               /lib/libselinux.so.1     $(TMPTOP)/usr/lib/libselinux.so
	rm -rf              $(TMPTOP)/usr/lib/python2.4/
	$(install_file)	    debian/changelog 	     $(DOCDIR)/changelog.Debian
	$(install_file)	    ChangeLog		     $(DOCDIR)/changelog
	gzip -9fqr	    $(DOCDIR)/
	gzip -9fqr	    $(MANDIR)/
# Make sure the copyright file is not compressed
	$(install_file)	     debian/copyright	     $(DOCDIR)/copyright
	$(strip-lib)
	@echo done > $@

debian/stamp/install/python-selinux:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/install || mkdir -p debian/stamp/install
	rm -rf		    $(TMPTOP)
	$(make_directory)   $(TMPTOP)
	$(make_directory)   $(DOCDIR)
	$(make_directory)   $(MODULES_DIR)
	$(make_directory)   $(EXTENSIONS_DIR)
	for version in $(ALL_PY_VERSIONS); do                               \
          rm -f             src/$(SWIGSO) src/$(SWIGLOBJ);                  \
	  rm -f             src/audit2why.lo src/audit2why.so;              \
          $(MAKE) -C src DESTDIR=$(TMPTOP) PYLIBVER=python$$version         \
                  PYTHONLIBDIR=$(TMPTOP)$(PYTHONLIBDIRTOP)/python$$version/ \
                  CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"        \
	          LIBSEPOLDIR=$(LIBSEPOLDIR) ARCH=$(DEB_HOST_ARCH)          \
                  pywrap install-pywrap;                                    \
          if [ -d $(EXTENSIONS_DIR)/python$$version/site-packages ]; then   \
            chmod -x $(EXTENSIONS_DIR)/python$$version/site-packages/*;     \
            mv -f $(EXTENSIONS_DIR)/python$$version/site-packages/*.py      \
                  $(MODULES_DIR)/;                                          \
            mv -f $(EXTENSIONS_DIR)/python$$version/site-packages/*         \
                  $(EXTENSIONS_DIR)/python$$version/;                       \
	    rmdir $(EXTENSIONS_DIR)/python$$version/site-packages;          \
          fi;                                                               \
          if [ -z "$$versions" ]; then versions="$$version";                \
          else             versions="$$versions,$$version";                 \
          fi;                                                               \
	  chmod 0755 $(EXTENSIONS_DIR)/python$$version/selinux;             \
        done; echo "$$versions" > $(MODULES_DIR)/.version
	$(install_file)	    debian/changelog 	     $(DOCDIR)/changelog.Debian
	$(install_file)	    ChangeLog		     $(DOCDIR)/changelog
	gzip -9fqr	    $(DOCDIR)/
# Make sure the copyright file is not compressed
	$(install_file)	     debian/copyright	     $(DOCDIR)/copyright
	$(strip-lib)
	@echo done > $@


debian/stamp/install/libselinux-ruby1.8:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/install || mkdir -p debian/stamp/install
	rm -rf		    $(TMPTOP)
	$(make_directory)   $(TMPTOP)
	$(make_directory)   $(DOCDIR)
	$(make_directory)   $(RUBYINSTALL)
	$(MAKE)             DESTDIR=$(TMPTOP) RUBYINSTALL=$(RUBYINSTALL) install-rubywrap;
	$(install_file)	    debian/changelog 	     $(DOCDIR)/changelog.Debian
	$(install_file)	    ChangeLog		     $(DOCDIR)/changelog
	gzip -9fqr	    $(DOCDIR)/
# Make sure the copyright file is not compressed
	$(install_file)	     debian/copyright	     $(DOCDIR)/copyright
	$(strip-lib)
	@echo done > $@



debian/stamp/install/selinux-utils:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/install || mkdir -p debian/stamp/install
	rm -rf		    $(TMPTOP)
	$(make_directory)   $(TMPTOP)
	$(make_directory)   $(DOCDIR)
	$(make_directory)   $(MAN1DIR)
	$(make_directory)   $(MAN3DIR)
	$(make_directory)   $(MAN8DIR)
	$(MAKE)             DESTDIR=$(TMPTOP) -C man install
	rm -rf              $(MAN3DIR)
	mv                  $(MAN8DIR)/setenforce.8     $(MAN1DIR)/setenforce.1
	mv                  $(MAN8DIR)/getenforce.8     $(MAN1DIR)/getenforce.1
	mv                  $(MAN8DIR)/selinuxenabled.8 $(MAN1DIR)/selinuxenabled.1
	mv                  $(MAN8DIR)/togglesebool.8   $(MAN1DIR)/togglesebool.1
	$(MAKE) DESTDIR=$(TMPTOP) -C utils install
	$(install_file)	     debian/changelog	     $(DOCDIR)/changelog.Debian
	$(install_file)	    ChangeLog		     $(DOCDIR)/changelog
	gzip -9fqr	    $(DOCDIR)/
	gzip -9fqr	    $(MANDIR)/
# Make sure the copyright file is not compressed
	$(install_file)	     debian/copyright	     $(DOCDIR)/copyright
	$(strip-exec)
	@echo done > $@


debian/stamp/binary/libselinux1:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/binary || mkdir -p debian/stamp/binary
	$(install_script)    debian/postrm	     $(TMPTOP)/DEBIAN/postrm
	$(install_script)    debian/postinst	     $(TMPTOP)/DEBIAN/postinst
	dpkg-gensymbols      -p$(package)            -P$(TMPTOP) -c4
	$(get-shlib-deps)
	dpkg-gencontrol	     -p$(package) -isp	     -P$(TMPTOP)
	$(create_md5sum)     $(TMPTOP)
	chown -R root:root   $(TMPTOP)
	chmod -R u+w,go=rX   $(TMPTOP)
	dpkg --build	     $(TMPTOP) ..
	@echo done > $@

debian/stamp/binary/libselinux1-dev:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/binary || mkdir -p debian/stamp/binary
	$(make_directory)    $(TMPTOP)/DEBIAN
	dpkg-gencontrol	     -p$(package) -isp	     -P$(TMPTOP)
	$(create_md5sum)     $(TMPTOP)
	chown -R root:root   $(TMPTOP)
	chmod -R u+w,go=rX   $(TMPTOP)
	dpkg --build	     $(TMPTOP) ..
	@echo done > $@

debian/stamp/binary/python-selinux:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/binary || mkdir -p debian/stamp/binary
	$(make_directory)    $(TMPTOP)/DEBIAN
	$(install_script)    debian/python_prerm	     $(TMPTOP)/DEBIAN/prerm
	$(install_script)    debian/python_postinst	     $(TMPTOP)/DEBIAN/postinst
	if dpkg --compare-versions $(MIN_PY_VERSIONS) le $(PYDEFAULT); then              \
	  echo 'python:Depends=python (>= $(MIN_PY_VERSIONS)), python (<< $(STOP_VERSION))'  >  debian/substvars;     \
        else                                                                             \
          echo 'python:Depends=python (>= $(MIN_PY_VERSIONS)) | python$(MIN_PY_VERSIONS), python (<< $(STOP_VERSION))'\
                                                                  >  debian/substvars;   \
        fi
	echo 'python:Provides=$(PY_PROVIDES)'                 >> debian/substvars
	$(get-shlib-deps)
	dpkg-gencontrol	     -p$(package) -isp	     -P$(TMPTOP)
	find $(TMPTOP) -type f -name \*.so -exec chmod 0644 {} \;
	$(create_md5sum)     $(TMPTOP)
	chown -R root:root   $(TMPTOP)
	chmod -R u+w,go=rX   $(TMPTOP)
	dpkg --build	     $(TMPTOP) ..
	@echo done > $@

debian/stamp/binary/libselinux-ruby1.8:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/binary || mkdir -p debian/stamp/binary
	$(make_directory)    $(TMPTOP)/DEBIAN
	$(get-shlib-deps)
	dpkg-gencontrol	     -p$(package) -isp	     -P$(TMPTOP)
	find $(TMPTOP) -type f -name \*.so -exec chmod 0644 {} \;
	$(create_md5sum)     $(TMPTOP)
	chown -R root:root   $(TMPTOP)
	chmod -R u+w,go=rX   $(TMPTOP)
	dpkg --build	     $(TMPTOP) ..
	@echo done > $@

debian/stamp/binary/selinux-utils:
	$(checkdir)
	$(REASON)
	$(TESTROOT)
	@test -d debian/stamp/binary || mkdir -p debian/stamp/binary
	$(make_directory)    $(TMPTOP)/DEBIAN
	k=`find $(TMPTOP) -type f | ( while read i; do		 \
	    if file -b $$i | egrep -q "^ELF.*executable"; then	 \
	      j="$$j $$i";					 \
	    fi;							 \
	   done; echo $$j; )`; dpkg-shlibdeps -Ldebian/shlibs -Tsubstvars.utils $$k
	dpkg-gencontrol	     -Tsubstvars.utils -p$(package) -isp    -P$(TMPTOP)
	$(create_md5sum)     $(TMPTOP)
	chown -R root:root   $(TMPTOP)
	chmod -R u+w,go=rX   $(TMPTOP)
	dpkg --build	     $(TMPTOP) ..
	@echo done > $@
