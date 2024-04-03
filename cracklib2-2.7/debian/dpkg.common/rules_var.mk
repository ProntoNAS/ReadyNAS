#!/usr/bin/make -f
#
# source:
#   /var/cvs/projects/debian/dpkg.common/rules_var.mk,v
#
# revision:
#   @(#) rules_var.mk,v 1.11 1999/03/08 04:34:57 jplejacq Exp
#
# copyright:
#   Copyright (C) 1999 Jean Pierre LeJacq <jplejacq@quoininc.com>
#
#   Distributed under the GNU GENERAL PUBLIC LICENSE.
#
# description:
#   Standard variables for package building script for package.
#
#   This program is package independent and should work unmodified
#   with any package.



#
# Generate list of all binary packages for this source package.
#
# bugs:
#   rules_pkgs creates the link debian/control since this is required
#   by the other dynamic variables.  Unfortunately, the link isn't
#   available after checkout from CVS which causes an error message to 
#   be printed.  This solves this problem but not in an elegant way.
#
#   I also make the build programs executable in debian/dpkg.common/
#   since the diff process when building form upstream original souce
#   turns the execute bit off.  This solves this problem but not in an
#   elegant way.
#
rules_pkgs=\
  $(shell\
    test -f debian/control || cp debian/dpkg.src/control debian;\
    find debian/dpkg.common/\
      -type f\
      -maxdepth 1\
      -not \( -name substfiles -or -name rules_var.mk \)\
      -exec chmod a+x {} \; ;\
    DH_DOPACKAGES=$$(./debian/dpkg.common/getpackages);\
    echo $${DH_DOPACKAGES}\
  )


#
# Generate list of architecture indep binary packages for this source
# package.
#
rules_pkgs_indep=\
  $(shell\
    DH_DOINDEP=$$(./debian/dpkg.common/getpackages indep);\
    echo $${DH_DOINDEP}\
  )


#
# Generate list of architecture dependent binary packages for this
# source package.
#
rules_pkgs_arch=\
  $(shell\
    DH_DOARCH=$$(./debian/dpkg.common/getpackages arch);\
    echo $${DH_DOARCH}\
  )


#
# Generate temporary directory to build debian binary package.
#
define rules_tmpdir
$(patsubst %.stamp-check,%.tmp,$(patsubst %.stamp-modify,%.tmp,$(patsubst %.stamp-binary,%.tmp,$(patsubst %.stamp-build,%.tmp,$(@)))))
endef


#
# Generate name of debian binary package.
#
define rules_pkg
$(patsubst %.stamp-check,%,$(patsubst %.stamp-modify,%,$(patsubst %.stamp-binary,%,$(patsubst %.stamp-build,%,$(@)))))
endef
