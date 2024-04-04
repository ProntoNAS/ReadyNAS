############################ -*- Mode: Makefile -*- ###########################
## Makefile<ucf> --- 
## Author           : Manoj Srivastava ( srivasta@glaurung.green-gryphon.com ) 
## Created On       : Tue Nov 18 22:00:27 2003
## Created On Node  : glaurung.green-gryphon.com
## Last Modified By : Manoj Srivastava
## Last Modified On : Sun Apr 16 16:32:23 2006
## Last Machine Used: glaurung.internal.golden-gryphon.com
## Update Count     : 6
## Status           : Unknown, Use with caution!
## HISTORY          : 
## Description      : 
## 
###############################################################################
prefix    = $(DESTDIR)
package   = ucf

ETCDIR    = $(prefix)/etc
BINDIR    = $(prefix)/usr/bin
DEBLIBDIR = $(prefix)/var/lib/$(package)
DEBDOCDIR = $(prefix)/usr/share/doc/$(package)
MANDIR    = $(prefix)/usr/share/man/
MAN1DIR   = $(MANDIR)/man1
MAN5DIR   = $(MANDIR)/man5

# install commands
install_file    := /usr/bin/install -p    -o root -g root -m 644
install_program := /usr/bin/install -p    -o root -g root -m 755
make_directory  := /usr/bin/install -p -d -o root -g root -m 755

all build: check

check:
	bash -n  ucf
	bash -n  ucfr
	perl -wc ucfq
	bash -n  debian/ucf.preinst
	bash -n  debian/ucf.postinst
	bash -n  debian/ucf.postrm

install:
	$(make_directory)   $(BINDIR)
	$(make_directory)   $(ETCDIR)
	$(make_directory)   $(MAN1DIR)
	$(make_directory)   $(MAN5DIR)
	$(make_directory)   $(DEBLIBDIR)
	$(make_directory)   $(DEBDOCDIR)/examples
	$(install_program)  ucf               $(BINDIR)
	$(install_file)     ucf.1             $(MAN1DIR)
	gzip -9fq           $(MAN1DIR)/ucf.1
	$(install_program)  ucfr              $(BINDIR)
	$(install_file)     ucfr.1            $(MAN1DIR)
	gzip -9fq           $(MAN1DIR)/ucfr.1
	$(install_program)  ucfq              $(BINDIR)
	$(install_file)     ucfq.1            $(MAN1DIR)
	gzip -9fq           $(MAN1DIR)/ucfq.1
	$(install_program)  lcf               $(BINDIR)
	$(install_file)     lcf.1             $(MAN1DIR)
	gzip -9fq           $(MAN1DIR)/lcf.1
	$(install_file)     ucf.conf.5        $(MAN5DIR)
	gzip -9fq           $(MAN5DIR)/ucf.conf.5 
	$(install_file)     ucf.conf          $(ETCDIR)
	$(install_file)     debian/changelog  $(DEBDOCDIR)/changelog
	gzip -9frq          $(DEBDOCDIR)
# make sure the copyright file is not compressed
	$(install_file)     debian/copyright  $(DEBDOCDIR)/copyright
	$(install_file)     examples/postinst $(DEBDOCDIR)/examples/
	$(install_file)     examples/postrm   $(DEBDOCDIR)/examples/

clean distclean:
	@echo nothing to do for clean

