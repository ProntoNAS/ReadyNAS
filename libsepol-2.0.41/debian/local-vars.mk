############################ -*- Mode: Makefile -*- ###########################
## local-vars.mk --- 
## Author           : Manoj Srivastava ( srivasta@glaurung.green-gryphon.com ) 
## Created On       : Sat Nov 15 10:43:00 2003
## Created On Node  : glaurung.green-gryphon.com
## Last Modified By : Manoj Srivastava
## Last Modified On : Wed Oct 27 12:05:18 2004
## Last Machine Used: glaurung.internal.golden-gryphon.com
## Update Count     : 15
## Status           : Unknown, Use with caution!
## HISTORY          : 
## Description      : 
## 
## arch-tag: 1a76a87e-7af5-424a-a30d-61660c8f243e
## 
###############################################################################

FILES_TO_CLEAN  = debian/files debian/substvars substvars.utils
STAMPS_TO_CLEAN = 
DIRS_TO_CLEAN   =  debian/stamp

# Location of the source dir
SRCTOP    := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
TMPTOP     = $(SRCTOP)/debian/$(package)
LINTIANDIR = $(TMPTOP)/usr/share/lintian/overrides

PREFIX  = /usr
BINDIR  = $(TMPTOP)$(PREFIX)/bin
LIBDIR  = $(TMPTOP)/lib
INCLUDE = $(TMPTOP)$(PREFIX)/include
INCDIR  = $(INCLUDE)/sepol

MANDIR  = $(TMPTOP)/usr/share/man/
MAN1DIR = $(TMPTOP)/usr/share/man/man1
MAN3DIR = $(TMPTOP)/usr/share/man/man3
MAN5DIR = $(TMPTOP)/usr/share/man/man5
MAN7DIR = $(TMPTOP)/usr/share/man/man7
MAN8DIR = $(TMPTOP)/usr/share/man/man8
INFODIR = $(TMPTOP)/usr/share/info
DOCTOP  = $(TMPTOP)/usr/share/doc
DOCDIR  = $(DOCTOP)/$(package)

define checkdir
	@test -f debian/rules -a -f src/policydb.c || \
          (echo Not in correct source directory; exit 1)
endef

define checkroot
	@test $$(id -u) = 0 || (echo need root priviledges; exit 1)
endef
