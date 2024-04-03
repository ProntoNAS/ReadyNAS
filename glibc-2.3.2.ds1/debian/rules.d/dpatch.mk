# Stolen from the 'dpatch' package, and modified to support srcdir != . builds
# Also supports stampdir
# Tell patch target to depend on unpack
# Use DEB_SOURCE_PACKAGE instead of PACKAGE

# Also now respects $(stamp-dir)
# -*- Makefile -*-, you silly Emacs!
# vim: set ft=make:

DEB_BUILD_ARCH	?= $(shell dpkg-architecture -qDEB_BUILD_ARCH)
DPATCH_ARCH	:= ${DEB_BUILD_ARCH}
PATCHLIST_ALL	 = $(shell test -f debian/patches/00list && grep -v ^\# debian/patches/00list)
PATCHLIST_ARCH	 = $(shell test -f debian/patches/00list.${DPATCH_ARCH} && grep -v ^\# debian/patches/00list.${DPATCH_ARCH})
PATCHLIST	?= $(shell echo ${PATCHLIST_ALL} ${PATCHLIST_ARCH})

UNPATCHLIST	 = $(shell echo ${PATCHLIST} | tr ' ' '\n' | tac)

# Expand them right now
PATCHLIST	:= ${PATCHLIST}
UNPATCHLIST	:= ${UNPATCHLIST}

patched-dir := $(stamp)patched

patch: $(stamp)patch-stamp
$(stamp)patch-stamp: $(stamp)unpack
	test -d $(patched-dir) || install -d $(patched-dir)
	@echo "Patches applied in the Debian version of ${DEB_SOURCE_PACKAGE}:" > $@T
	@for patch in ${PATCHLIST}; do \
		patchdir=$${patch%/*}; \
		test "x$$patchdir" = "x$$patch" || \
			install -d $(patched-dir)/$$patchdir; \
                stamp=$(patched-dir)/$${patch%%.dpatch}.dpatch; \
		test -e debian/patches/$$patch || ext=.dpatch; \
                patch=debian/patches/$$patch$$ext; \
                author=`sed -n "s,^#* *.*dpatch by *,,p" $$patch`; \
                test -x $$patch || chmod +x $$patch; \
		if test -f $$stamp; then \
                        echo "$$patch already applied."; \
                        echo -e "\n$$patch ($$author):" >> $@T; \
                        sed -n 's/^#* *DP: */  /p' $$patch >> $@T; \
                else \
                        echo -n "applying patch $$patch..."; \
                        if $$patch -patch $(DEB_SRCDIR) >$$stamp.new 2>&1; then \
                                mv $$stamp.new $$stamp; \
                                touch $$stamp; \
                                echo -e "\n$$patch ($$author):" >> $@T; \
                                sed -n 's/^#* *DP: */  /p' $$patch >> $@T; \
                                echo " ok."; \
                        else \
                                echo " failed."; \
                                exit 1; \
                        fi; \
                fi; \
	done
	mv -f $@T $@

unpatch:
	@for patch in ${UNPATCHLIST}; do \
                stamp=$(patched-dir)/$${patch%%.dpatch}.dpatch; \
		test -e debian/patches/$$patch || ext=.dpatch; \
                patch=debian/patches/$$patch$$ext; \
                test -x $$patch || chmod +x $$patch; \
                if test -f $$stamp; then \
                        echo -n "reverting patch $$patch..."; \
                        if $$patch -unpatch $(DEB_SRCDIR) 2>&1 >/dev/null; then \
                                rm -f $$stamp; \
                                echo " ok."; \
                        else \
                                echo " failed."; \
                                exit 1; \
                        fi; \
                fi; \
	done
	rm -rf $(stamp)patch-stamp $(stamp)patch-stampT $(debian-patched)
