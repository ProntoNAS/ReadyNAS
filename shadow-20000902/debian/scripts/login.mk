binary-login: build debian/control $(dh_mak_deps)
	$(checkdir)
	$(checkroot)
	-rm -rf debian/tmp-l
#	install -d debian/tmp-l/{DEBIAN,bin,etc/{init.d,pam.d},usr/{bin,share/man/man{1,5,8},share/doc/login,sbin}}
	install -d debian/tmp-l/{DEBIAN,bin,etc/{init.d,pam.d},usr/{bin,share/man/man{1,5,8},share/doc/login}}
	$(IE) $(BUILD_TREE)/src/{login,su} debian/tmp-l/bin/
	# We make su suid by default, so there is no chance of
	# it being installed without suid root perms in the
	# event that dpkg dies. I'de rather have it with default
	# perms for a little bit than without suid root perms
	chmod u+s debian/tmp-l/bin/su
	$(IE) $(BUILD_TREE)/src/newgrp debian/tmp-l/usr/bin/
	$(IE) $(BUILD_TREE)/src/{faillog,lastlog} debian/tmp-l/usr/bin/
	$(ID) $(BUILD_TREE)/man/{login.1,newgrp.1,su.1} debian/tmp-l/usr/share/man/man1/
	$(ID) $(BUILD_TREE)/man/{login.defs.5,faillog.5,dialups.5} debian/tmp-l/usr/share/man/man5/
#	$(ID) $(BUILD_TREE)/man/{faillog.8,lastlog.8,logoutd.8} debian/tmp-l/usr/share/man/man8/
	$(ID) $(BUILD_TREE)/man/{faillog.8,lastlog.8} debian/tmp-l/usr/share/man/man8/
	ln -s newgrp debian/tmp-l/usr/bin/sg
	ln -s dialups.5.gz debian/tmp-l/usr/share/man/man5/d_passwd.5.gz
	ln -s newgrp.1.gz debian/tmp-l/usr/share/man/man1/sg.1.gz
	$(ID) debian/local/login.defs.pam.linux debian/tmp-l/etc/login.defs
	$(ID) debian/local/securetty debian/tmp-l/etc/
#	install debian/local/logoutd debian/tmp-l/etc/init.d/
#	$(IE) $(BUILD_TREE)/src/logoutd debian/tmp-l/usr/sbin/
	$(ID) $(BUILD_TREE)/doc/CHANGES debian/tmp-l/usr/share/doc/login/changelog
	$(ID) $(BUILD_TREE)/doc/{README,README.debian,README.pam} \
		debian/tmp-l/usr/share/doc/login/
	$(ID) debian/changelog debian/tmp-l/usr/share/doc/login/changelog.Debian
	$(ID) debian/local/login.pam.d debian/tmp-l/etc/pam.d/login
	$(ID) debian/local/su.pam.d debian/tmp-l/etc/pam.d/su
	find debian/tmp-l/usr/share/{doc,man} -type f | xargs gzip -9
	$(ID) debian/copyright debian/tmp-l/usr/share/doc/login/copyright
	install debian/login.preinst debian/tmp-l/DEBIAN/preinst
	install debian/login.postinst debian/tmp-l/DEBIAN/postinst
	install debian/login.prerm    debian/tmp-l/DEBIAN/prerm
	install debian/login.postrm   debian/tmp-l/DEBIAN/postrm
	install -m644 debian/login.conffiles debian/tmp-l/DEBIAN/conffiles
	# Make some executables suid
	chmod 4755 debian/tmp-l/bin/su debian/tmp-l/usr/bin/newgrp \
		debian/tmp-l/bin/login
#	dpkg-shlibdeps debian/tmp-l/{bin/*,usr/bin/*,usr/sbin/*}
	dpkg-shlibdeps debian/tmp-l/{bin/*,usr/bin/*}
	dpkg-gencontrol -isp -plogin -Pdebian/tmp-l
	$(SHELL) debian/scripts/checksums debian/tmp-l
	dpkg-deb --build debian/tmp-l ..
