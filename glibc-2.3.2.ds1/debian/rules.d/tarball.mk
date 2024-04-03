unpack: $(stamp)unpack
$(stamp)unpack: $(DEB_TARBALL) $(patsubst %,$(stamp)%,$(GLIBC_OVERLAYS))
	touch $(stamp)unpack

# FIXME: Support gzip as well!
$(DEB_TARBALL): $(stamp)$(DEB_TARBALL)
$(stamp)$(DEB_TARBALL):
	mkdir -p $(build-tree)
	cd $(build-tree) && tar -xjvf $(CURDIR)/$(DEB_TARBALL)
	touch $@

$(patsubst %,$(stamp)%,$(GLIBC_OVERLAYS)):
	cd $(DEB_SRCDIR) && tar -xjvf $(CURDIR)/$(notdir $@)
	touch $@


