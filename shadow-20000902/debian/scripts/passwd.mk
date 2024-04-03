binary-passwd: build debian/control $(dh_mak_deps)
	$(checkdir)
	$(checkroot)
	-rm -rf debian/tmp-p
	install -d debian/tmp-p/{DEBIAN,etc,usr/{sbin,bin,share/man/{man1,man5,man8},share/doc/passwd}}
	$(ID) debian/local/shells debian/tmp-p/etc/
	$(IE) $(BUILD_TREE)/src/{chage,chfn,chsh,expiry,gpasswd,passwd} debian/tmp-p/usr/bin/
	$(IE) $(BUILD_TREE)/src/{chpasswd,groupadd,groupdel,groupmod,grpck,grpconv,grpunconv} \
	  $(BUILD_TREE)/src/{newusers,pwck,pwconv,pwunconv,useradd,userdel,cppw} \
	  $(BUILD_TREE)/src/{dpasswd,usermod,vipw} debian/tmp-p/usr/sbin/
	$(ID) $(BUILD_TREE)/man/{expiry.1,chage.1,chfn.1,chsh.1,gpasswd.1,passwd.1} \
	  debian/tmp-p/usr/share/man/man1/
	$(ID) $(BUILD_TREE)/man/{chpasswd.8,groupadd.8,groupdel.8,groupmod.8,grpck.8} \
	  $(BUILD_TREE)/man/{newusers.8,pwck.8,dpasswd.8} \
	$(BUILD_TREE)/man/{useradd.8,userdel.8,usermod.8,vipw.8,shadowconfig.8,pwconv.8} \
	  debian/tmp-p/usr/share/man/man8/
	$(ID) $(BUILD_TREE)/man/{passwd.5,shadow.5} debian/tmp-p/usr/share/man/man5/
	ln -s vipw debian/tmp-p/usr/sbin/vigr
	ln -s vipw.8.gz debian/tmp-p/usr/share/man/man8/vigr.8.gz
	ln -s cppw debian/tmp-p/usr/sbin/cpgr
	for i in pwunconv.8.gz grpconv.8.gz grpunconv.8.gz ; do \
	  ln -s pwconv.8.gz debian/tmp-p/usr/share/man/man8/$$i ; done
	$(ID) debian/changelog debian/tmp-p/usr/share/doc/passwd/changelog.Debian
	$(ID) $(BUILD_TREE)/doc/{README,README.linux,README.debian} \
	  debian/tmp-p/usr/share/doc/passwd/
	$(ID) $(BUILD_TREE)/doc/CHANGES debian/tmp-p/usr/share/doc/passwd/changelog
	$(ID) debian/local/passwd.expire.cron debian/tmp-p/usr/share/doc/passwd/
	install debian/passwd.preinst debian/tmp-p/DEBIAN/preinst
	install debian/passwd.prerm debian/tmp-p/DEBIAN/prerm
	install debian/passwd.postinst debian/tmp-p/DEBIAN/postinst
	dpkg-shlibdeps debian/tmp-p/usr/{bin/*,sbin/*}
	# dpkg-shlibdeps fails on scripts, so install them now...
	install $(BUILD_TREE)/src/shadowconfig.sh debian/tmp-p/usr/sbin/shadowconfig
ifeq ($(DEB_HOST_GNU_SYSTEM),gnu)
# Hurd does not use PAM yet
	$(ID) $(BUILD_TREE)/doc/README.limits debian/tmp-p/usr/share/doc/passwd/
	$(ID) $(BUILD_TREE)/etc/login.defs.hurd debian/tmp-p/etc/login.defs
	echo "/etc/login.defs" >> debian/tmp-p/DEBIAN/conffiles
	$(ID) $(BUILD_TREE)/man/login.defs.5 debian/tmp-p/usr/share/man/man5/
	grep -v /etc/pam.d debian/passwd.conffiles > debian/tmp-p/DEBIAN/conffiles
else
	install -d debian/tmp-p/etc/pam.d
	$(ID) $(BUILD_TREE)/doc/README.pam debian/tmp-p/usr/share/doc/passwd/
	$(ID) debian/local/chfn.pam.d debian/tmp-p/etc/pam.d/chfn
	$(ID) debian/local/chsh.pam.d debian/tmp-p/etc/pam.d/chsh
	$(ID) debian/local/passwd.pam.d debian/tmp-p/etc/pam.d/passwd
	$(ID) debian/passwd.conffiles debian/tmp-p/DEBIAN/conffiles
	install -m755 debian/passwd.config debian/tmp-p/DEBIAN/config
	for template in debian/passwd.templates*; do \
		name=`echo $$template | sed 's,debian/passwd.,,'`; \
		$(ID) $$template debian/tmp-p/DEBIAN/$$name; \
	done
endif
	# Some suid/sgid programs...
	chmod 2755 debian/tmp-p/usr/bin/expiry \
		debian/tmp-p/usr/bin/chage
	chgrp shadow debian/tmp-p/usr/bin/expiry \
		debian/tmp-p/usr/bin/chage
	chmod 4755 debian/tmp-p/usr/bin/chfn \
		debian/tmp-p/usr/bin/chsh \
		debian/tmp-p/usr/bin/gpasswd \
		debian/tmp-p/usr/bin/passwd
	find debian/tmp-p/usr/share/{doc,man} -type f | xargs gzip -9f
	$(ID) debian/copyright debian/tmp-p/usr/share/doc/passwd/copyright
	dpkg-gencontrol -isp -ppasswd -Pdebian/tmp-p
	$(SHELL) debian/scripts/checksums debian/tmp-p
	dpkg-deb --build debian/tmp-p ..
