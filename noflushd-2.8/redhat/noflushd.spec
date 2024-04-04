%define	version	2.7.5
%define prefix /usr

Summary: Daemon that sends idle disks to sleep
Name: noflushd
Version: %{version}
Release: 1
Copyright: GPL
Group: System Environment/Daemons
Source0: noflushd_%{version}-1.tar.gz
Prefix: %{prefix}
BuildRoot: /tmp/%{name}-%{version}-tmp
Vendor: Daniel Kobras <kobras@linux.de>

%changelog
* Mon May 09 2005 Daniel Kobras <kobras@linux.de>

- New upstream version 2.7.5.

* Thu Apr 09 2004 Daniel Kobras <kobras@linux.de>

- New upstream version 2.7.3.

* Tue Mar 16 2004 Daniel Kobras <kobras@linux.de>

- New upstream version 2.7.2.

* Sun Mar 07 2004 Daniel Kobras <kobras@linux.de>

- New upstream version 2.7.1.

* Tue Jan 20 2004 Daniel Kobras <kobras@linux.de>

- New upstream version 2.7.

* Mon May 13 2002 Daniel Kobras <kobras@linux.de>

- New upstream version 2.6.3.

* Sat Apr 20 2002 Daniel Kobras <kobras@linux.de>

- New upstream version 2.6.2.

* Tue Jan 22 2002 Daniel Kobras <kobras@linux.de>

- New upstream version.

* Mon Aug 06 2001 Daniel Kobras <kobras@linux.de>

- New upstream version.

* Tue Mar 13 2001 Daniel Kobras <kobras@linux.de>

- New upstream version.

* Fri Jan 20 2001 Daniel Kobras <kobras@linux.de>

- New upstream version.

* Fri Dec 22 2000 Daniel Kobras <kobras@linux.de>

- New upstream version.

* Wed Dec 20 2000 Daniel Kobras <kobras@linux.de>

- New upstream version.

* Tue Nov 14 2000 Klaus Knopper <knopper@knopper.net>

- spec file fixes

* Tue Nov 14 2000 Daniel Kobras <kobras@linux.de>

  Release 2.0-1:

- New upstream version.

* Sat Aug 07 2000 Daniel Kobras <kobras@linux.de>

  Release 1.8.5-1:

- New upstream version.

* Sat Aug 03 2000 Daniel Kobras <kobras@linux.de>

  Release 1.8.4-1:

- New upstream version
- Fixed permission handling on RPM build. (rpm --rebuild should 
  work now also as non-root user.)

* Mon Mar 13 2000 Daniel Kobras <kobras@linux.de>

  Release 1.8.3-1:

- New upstream version

* Mon Feb  7 2000 Daniel Kobras <kobras@linux.de>

  Release 1.8.2-1:

- New upstream version

* Thu Jan 27 2000 Daniel Kobras <kobras@linux.de>

  Release 1.8.1-1:

- New upstream version

* Tue Jan 25 2000 Daniel Kobras <kobras@linux.de>

  Release 1.8-1:

- New upstream version

* Sat Sep 04 1999 Daniel Kobras <kobras@linux.de>

  Release 1.7.4-1:

- Merged RedHat and SuSE specs

* Fri Aug 27 1999 Daniel Kobras <kobras@linux.de>

  Release 1.7.3-1:

- Fixed several minor bugs in spec file

* Wed Aug 25 1999 Daniel Kobras <kobras@linux.de>

  Release 1.7.2-1:

- Added RPM spec file stuff

%description
noflushd is a simple daemon that monitors disk activity and spins down
disks whose idle time exceeds a certain timeout. It requires a kernel thread
named kupdate which is present in Linux kernel version 2.2.11 and later. For
earlier kernels, bdflush version 1.6 provides equal functionality.

%prep
%setup -n noflushd-%{version}

%build
LDFLAGS="-s" CFLAGS="$RPM_OPT_FLAGS" ac_cv_prog_cc_g=no \
./configure --prefix=%{prefix} --sysconfdir=/etc --mandir=%{prefix}/share/man \
--with-scheme=redhat && make

%install
make prefix=%{prefix} DESTDIR=$RPM_BUILD_ROOT initdir=/etc/rc.d/init.d install

%post
echo "Updating startup files..."
# SuSE init sucks. They have this rctab helper, but it's only for
# interactive use and error recovery.
if [ -d sbin/init.d ]; then 
	[ -d var/adm/fillup-templates ] && 
	cat > var/adm/fillup-templates/rc.config.noflushd << EOF
#
# Start disk idle daemon noflushd?
#
START_NOFLUSHD="yes"
NOFLUSHD_TIMEOUT=60     # timeout in minutes before disk is spun down
NOFLUSHD_DISKS="/dev/hda"       # disks to be monitorred
EOF
	for i in 1 2 3; do 
		ln -sf ../noflushd sbin/init.d/rc$i.d/S80noflushd
		ln -sf ../noflushd sbin/init.d/rc$i.d/K10noflushd
	done
	if [ -x bin/fillup ]; then 
		bin/fillup -q -d = etc/rc.config var/adm/fillup-templates/rc.config.noflushd	
		echo "NOTE: Default timeout is 1 hour." 
		echo "      Edit NOFLUSHD_TIMEOUT in /etc/rc.config to change."
	else 
		echo "NOTE: fillup not found. rc.config unchanged." 
		echo "      Edit /sbin/init.d/noflushd to set the default timeout (1h)."
	fi 
else 
if [ -x /sbin/chkconfig ]; then 
	/sbin/chkconfig --add noflushd 
else 
	echo "ERROR: Could not include noflushd in system startup. Please update"
	echo "       your system by hand."
fi 
echo "NOTE: Edit /etc/rc.d/init.d/noflushd to set the default timeout (1h)." 
fi
%preun
if [ -d /sbin/init.d ]; then
	rm -f /var/adm/fillup-templates/rc.config.noflushd || /bin/true
	for i in /sbin/init.d/rc?.d; do
		rm -f $i/*noflushd
	done
else 
if [ -x /sbin/chkconfig ]; then
	/sbin/chkconfig --del noflushd
fi
fi

%files
%defattr(-, root, root, 0755)
%doc README AUTHORS NEWS COPYING ChangeLog THANKS TODO BUGS
%{prefix}/sbin/noflushd
%config /etc/rc.d/init.d/noflushd
%{prefix}/share/man/man8/noflushd.8*
