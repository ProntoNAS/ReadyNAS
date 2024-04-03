# Because variables can be masked at anypoint by declaring
# PASS_VAR, we need to call all variables as $(call xx,VAR)
# This little bit of magic makes it possible:
xx=$(if $($(curpass)_$(1)),$($(curpass)_$(1)),$($(1)))

$(patsubst %,mkbuilddir_%,$(GLIBC_PASSES)) :: mkbuilddir_% : $(stamp)mkbuilddir_%
$(stamp)mkbuilddir_%: $(stamp)patch-stamp $(LINUX_HEADER_DIR)
	@echo Making builddir for $(curpass)
	test -d $(DEB_BUILDDIR) || mkdir $(DEB_BUILDDIR)
	touch $@

$(patsubst %,configure_%,$(GLIBC_PASSES)) :: configure_% : $(stamp)configure_%
$(stamp)configure_%: $(stamp)mkbuilddir_%

	@echo Configuring $(curpass)
	rm -f $(DEB_BUILDDIR)/configparms
	echo "CC = $(call xx,CC)"	>> $(DEB_BUILDDIR)/configparms
	echo "BUILD_CC = $(BUILD_CC)"	>> $(DEB_BUILDDIR)/configparms
	echo "CFLAGS = $(HOST_CFLAGS)"	>> $(DEB_BUILDDIR)/configparms
	echo "BUILD_CFLAGS = $(BUILD_CFLAGS)" >> $(DEB_BUILDDIR)/configparms
	echo "BASH := /bin/bash"	>> $(DEB_BUILDDIR)/configparms
	echo "KSH := /bin/bash"		>> $(DEB_BUILDDIR)/configparms
	echo "mandir = $(mandir)"	>> $(DEB_BUILDDIR)/configparms
	echo "infodir = $(infodir)"	>> $(DEB_BUILDDIR)/configparms
	echo "libexecdir = $(libexecdir)" >> $(DEB_BUILDDIR)/configparms
	echo "LIBGD = no"		>> $(DEB_BUILDDIR)/configparms
	echo "sysconfdir = /etc"	>> $(DEB_BUILDDIR)/configparms
	echo "rootsbindir = /sbin"	>> $(DEB_BUILDDIR)/configparms
ifneq ($(call xx,slibdir),)
	echo "slibdir = $(call xx,slibdir)" >> $(DEB_BUILDDIR)/configparms
endif

	# Prevent autoconf from running unexpectedly by setting it to false.
	# Also explicitly pass CC down - this is needed to get -m64 on
	# Sparc, et cetera.

	configure_build=$(call xx,configure_build); \
	if [ $(call xx,configure_target) = $$configure_build ]; then \
	  echo "Checking that we're running at least kernel version: $(call xx,MIN_KERNEL_SUPPORTED)"; \
	  if ! $(call kernel_check,$(call xx,MIN_KERNEL_SUPPORTED)); then \
	    configure_build=`echo $$configure_build | sed 's/^\([^-]*\)-\([^-]*\)$$/\1-dummy-\2/'`; \
	    echo "No.  Forcing cross-compile by setting build to $$configure_build."; \
	  fi; \
	fi; \
	cd $(DEB_BUILDDIR) && \
		CC="$(call xx,CC)" \
		AUTOCONF=false \
		$(CURDIR)/$(DEB_SRCDIR)/configure \
		--host=$(call xx,configure_target) \
		--build=$$configure_build --prefix=/usr --without-cvs \
		--enable-add-ons="$(call xx,add-ons)" \
		$(call xx,with_headers) $(call xx,extra_config_options) 2>&1 | tee -a $(log_build)

	touch $@

$(patsubst %,build_%,$(GLIBC_PASSES)) :: build_% : $(stamp)build_%
$(stamp)build_%: $(stamp)configure_%
	@echo Building $(curpass)
	$(MAKE) -C $(DEB_BUILDDIR) -j $(NJOBS) 2>&1 | tee -a $(log_build)
	touch $@

$(patsubst %,check_%,$(GLIBC_PASSES)) :: check_% : $(stamp)check_%
$(stamp)check_%: $(stamp)build_%
	if [ -n "$(findstring nocheck,$(DEB_BUILD_OPTIONS))" ]; then \
	  echo "DEB_BUILD_OPTIONS contains nocheck, skipping tests."; \
	elif [ $(call xx,configure_build) != $(call xx,configure_target) ]; then \
	  echo "Cross compiling, skipping tests."; \
	elif ! $(call kernel_check,$(call xx,MIN_KERNEL_SUPPORTED)); then \
	  echo "Kernel too old, skipping tests."; \
	else \
	  echo Testing $(curpass); \
	  $(MAKE) -C $(DEB_BUILDDIR) -j $(NJOBS) -k check 2>&1 | tee -a $(log_test); \
	fi
	touch $@

$(patsubst %,install_%,$(GLIBC_PASSES)) :: install_% : $(stamp)install_%
$(stamp)install_%: $(stamp)check_%
	@echo Installing $(curpass)
	rm -rf $(CURDIR)/debian/tmp-$(curpass)
	$(MAKE) -C $(DEB_BUILDDIR) -j $(NJOBS) \
	  install_root=$(CURDIR)/debian/tmp-$(curpass) install

	if [ $(curpass) = libc ]; then \
	  $(MAKE) -f debian/generate-supported.mk IN=$(DEB_SRCDIR)/localedata/SUPPORTED \
	    OUT=debian/tmp-$(curpass)/usr/share/i18n/SUPPORTED; \
	  (cd $(DEB_SRCDIR)/manual && texi2html -split_chapter libc.texinfo); \
	  install --mode=0644 $(DEB_SRCDIR)/manual/dir-add.info \
	    debian/tmp-$(curpass)/usr/share/info/libc-dir-add.info; \
	fi

	$(call xx,extra_install)
	touch $@
