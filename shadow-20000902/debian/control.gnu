Source: shadow
Section: base
Priority: required
Maintainer: Karl Ramm <kcr@debian.org>
Standards-Version: 3.5.0.0
Build-Depends: autoconf, automake, bzip2, gettext, texinfo, file

Package: passwd
Architecture: any
Depends: ${shlibs:Depends}
Section: base
Priority: required
Conflicts: suidregister (<< 0.50)
Description: Change and administer password and group data.
 This package includes passwd, chsh, chfn, and many other programs to
 maintain password and group data.
 .
 Shadow passwords are supported.  See /usr/share/doc/passwd/README.Debian.gz
