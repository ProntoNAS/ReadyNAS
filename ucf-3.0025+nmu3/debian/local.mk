############################ -*- Mode: Makefile -*- ###########################
## local.mk --- 
## Author           : Manoj Srivastava ( srivasta@glaurung.green-gryphon.com ) 
## Created On       : Sat Nov 15 10:42:10 2003
## Created On Node  : glaurung.green-gryphon.com
## Last Modified By : Manoj Srivastava
## Last Modified On : Wed Sep  6 13:28:52 2006
## Last Machine Used: glaurung.internal.golden-gryphon.com
## Update Count     : 20
## Status           : Unknown, Use with caution!
## HISTORY          : 
## Description      : 
## 
## arch-tag: b07b1015-30ba-4b46-915f-78c776a808f4
## 
###############################################################################

testdir:
	$(checkdir)

debian/stamp/BUILD/ucf: debian/stamp/build/ucf
debian/stamp/INST/ucf:  debian/stamp/install/ucf
debian/stamp/BIN/ucf:   debian/stamp/binary/ucf

CLEAN/ucf::
	-rm -rf $(TMPTOP)


debian/stamp/build/ucf:  
	$(checkdir)
	$(REASON)
	@test -d debian/stamp/build || mkdir -p debian/stamp/build
	$(CHECKPO)
	$(MAKE) build
	@echo done > $@

debian/stamp/install/ucf:
	$(REASON)
	$(checkdir)
	$(TESTROOT)
	rm -rf              $(TMPTOP)
	$(make_directory)   $(TMPTOP)
	chmod g-s           $(TMPTOP)
	$(make_directory)   $(LINTIANDIR)
	echo "$(package): description-synopsis-might-not-be-phrased-properly" \
                                >> $(LINTIANDIR)/$(package)
	echo "$(package): no-debconf-config" \
                                >> $(LINTIANDIR)/$(package)
	echo "$(package): debconf-is-not-a-registry usr/bin/ucf" \
                                >> $(LINTIANDIR)/$(package)
	chmod -R u+w        $(TMPTOP)/usr/
	$(MAKE)             prefix=$(TMPTOP)   install
	@test -d debian/stamp/install || mkdir -p debian/stamp/install
	@echo done > $@

debian/stamp/binary/ucf:
	$(REASON)
	$(checkdir)
	$(TESTROOT)
	$(make_directory)   $(TMPTOP)/DEBIAN
	$(install_program)  debian/preinst    $(TMPTOP)/DEBIAN/preinst
	$(install_program)  debian/postinst   $(TMPTOP)/DEBIAN/postinst
	$(install_program)  debian/postrm     $(TMPTOP)/DEBIAN/postrm
	$(install_file)     debian/conffiles  $(TMPTOP)/DEBIAN/conffiles
	po2debconf debian/templates > $(TMPTOP)/DEBIAN/templates
	dpkg-gencontrol     -V'debconf-depends=debconf (>= $(MINDEBCONFVER))' \
                             -p$(package) -isp      -P$(TMPTOP)
	$(create_md5sum)    $(TMPTOP)
	chown -R root:root  $(TMPTOP)
	chmod -R u+w,go=rX  $(TMPTOP)
	dpkg --build        $(TMPTOP) ..
	@test -d debian/stamp/binary || mkdir -p debian/stamp/binary
	@echo done > $@

