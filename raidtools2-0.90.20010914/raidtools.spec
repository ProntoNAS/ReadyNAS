Summary: Tools for creating and maintaining software RAID devices.
Name: raidtools
Version: 0.90
Release: 23
Copyright: GPL
Group: System Environment/Base
Source: raidtools-multipath-2.4.2-curr-CVS.tar.gz
BuildRoot: %{_tmppath}/%{name}-root
BuildPreReq: popt
Prereq: MAKEDEV >= 3.0
Obsoletes: md, md-tools
Conflicts: kernel < 2.2

%description
The raidtools package includes the tools you need to set up and
maintain a software RAID device (using two or more disk drives in
combination for fault tolerance and improved performance) on a Linux
system. It only works with Linux 2.2 kernels and later, or with a 2.0
kernel specifically patched with newer RAID support.

Install raidtools if you need to set up RAID on your system.

%prep
%setup -q -c -n raidtools

%build
./autogen.sh
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/dev
make ROOTDIR=$RPM_BUILD_ROOT mandir=%{_mandir} install_bin install_doc

#for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15; do
#  mknod -m 0600 $RPM_BUILD_ROOT/dev/md$i b 9 $i
#done

%post
cd /dev
./MAKEDEV -m 16 md

%files
%defattr(-,root,root)
/sbin/*
%{_mandir}/*/*
%doc COPYING README *.sample

%changelog
* Mon Aug 27 2001 Doug Ledford <dledford@redhat.com>
- Include a pared down version of the scsi_reserve library for use by
  the detect_multipath program.  This is to avoid a build requirement
  on the scsi_reserve-devel package.

* Tue Aug 14 2001 Doug Ledford <dledford@redhat.com>
- Add the detect_multipath code.
- Add a buildprereq for the scsi_reserve-devel package and popt package

* Sun Jun 24 2001 Elliot Lee <sopwith@redhat.com>
- Bump release + rebuild.

* Thu Mar 22 2001 Erik Troan <ewt@redhat.com>
- updated to support multipath in 2.4.2 kernel

* Tue Aug 08 2000 Erik Troan <ewt@redhat.com>
- removed old HOWTO from package

* Sat Aug 05 2000 Erik Troan <ewt@redhat.com>
- parse /etc/mtab correctly

* Wed Jul 12 2000 Prospector <bugzilla@redhat.com>
- automatic rebuild

* Mon Jun 19 2000 Nalin Dahyabhai <nalin@redhat.com>
- use MAKEDEV to create md devices

* Mon Jun 19 2000 Than Ngo <than@redhat.de>
- FHS fixes
- add post and preun to create and delte device

* Fri Mar 17 2000 Erik Troan <ewt@redhat.com>
- added patch for ia64

* Mon Feb  7 2000 Bill Nottingham <notting@redhat.com>
- handle compressed manpages

* Wed Aug 25 1999 Cristian Gafton <gafton@redhat.com>
- updated to 0.90 from 080399

* Tue Jul 27 1999 Cristian Gafton <gafton@redhat.com>
- added patch from Jakub for fixing __NR__llseek on sparcs

* Tue Apr 06 1999 Cristian Gafton <gafton@redhat.com>
- updated sources from mingo

* Sun Mar 21 1999 Cristian Gafton <gafton@redhat.com> 
- auto rebuild in the new build environment (release 2)

* Mon Feb 08 1999 Erik Troan <ewt@redhat.com>
- updated to 0.90 19980128

* Mon Oct 12 1998 Erik Troan <ewt@redhat.com>
- backrev'd to 0.50beta10 (which works with old md)
- patched to actually work with old level md support

* Fri Oct 09 1998 Cristian Gafton <gafton@redhat.com>
- put some real Summary and description fields
- obsoletes the md and md-tools package that are floating around on the net

* Sat Sep 19 1998 Jeff Johnson <jbj@redhat.com>
- tweak description/summary for uniqueness.

* Wed Sep 16 1998 Jeff Johnson <jbj@redhat.com>
- repackage for RH 5.2.
