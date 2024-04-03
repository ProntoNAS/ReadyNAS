#
# This Makefile controls a build process of $(TITLE) module for
# Linux-PAM. You should not modify this Makefile (unless you know
# what you are doing!).

# $Id: Makefile,v 1.7 2002/09/21 18:25:46 hartmans Exp $
# Created by Cristian Gafton <gafton@redhat.com>

include ../../Make.Rules

TITLE=pam_userdb

ifeq ($(HAVE_NDBM_H),yes)
 WHICH_DB=ndbm
 ifeq ($(HAVE_LIBNDBM),yes) 
  MODULE_SIMPLE_EXTRALIBS = -lndbm 
 endif
else
ifeq ($(HAVE_LIBDB),yes)
 WHICH_DB=db
 MODULE_SIMPLE_EXTRALIBS = -ldb3
else
 WHICH_DB=none
endif
endif

ifeq ($(WHICH_DB),none)

include ../dont_makefile

else

MODULE_SIMPLE_EXTRAFILES = conv

include ../Simple.Rules

endif
