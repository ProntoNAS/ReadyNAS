arch_binaries  := $(arch_binaries) proto

p_proto	= protoize
d_proto	= debian/$(p_proto)

dirs_proto = \
	$(docdir) \
	$(PF)/share/man/man1 \
	$(PF)/bin
files_proto = \
	$(PF)/bin/{protoize,unprotoize} \
	$(PF)/share/man/man1/{protoize,unprotoize}.1

# ----------------------------------------------------------------------
$(binary_stamp)-proto: $(install_stamp)
	dh_testdir
	dh_testroot
	mv $(install_stamp) $(install_stamp)-tmp

	rm -rf $(d_proto)
	dh_installdirs -p$(p_proto) $(dirs_proto)
	$(IR) debian/protoize.1 $(d)/$(PF)/share/man/man1/
	ln -sf protoize.1 $(d)/$(PF)/share/man/man1/unprotoize.1
	dh_movefiles -p$(p_proto) $(files_proto)

	debian/dh_doclink -p$(p_proto) $(p_base)
	dh_strip -p$(p_proto)
	dh_compress -p$(p_proto)
	dh_fixperms -p$(p_proto)
	dh_shlibdeps -p$(p_proto)
	dh_gencontrol -p$(p_proto) -u-v$(DEB_VERSION)
	dh_installdeb -p$(p_proto)
	dh_md5sums -p$(p_proto)
	dh_builddeb -p$(p_proto)

	trap '' 1 2 3 15; touch $@; mv $(install_stamp)-tmp $(install_stamp)
