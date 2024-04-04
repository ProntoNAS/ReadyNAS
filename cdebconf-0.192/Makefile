# This is just the bootstrapping makefile

all: cdebconf man

globalmakeflags:
	./configure

cdebconf: globalmakeflags
	$(MAKE) -C src

man:
	$(MAKE) -C man

configure: configure.ac
	autoconf

install:
	$(MAKE) -C src $@
	$(MAKE) -C man $@

clean: 
	rm -f *-stamp
	[ ! -f globalmakeflags ] || $(MAKE) -C src $@

distclean: clean
	rm -f config.status config.log config.cache globalmakeflags
	rm -rf autom4te.cache
	rm -f src/cdebconf.conf-dist src/Makefile src/config.h man/Makefile
	rm -f src/modules/db/rfc822db/Makefile
	rm -f src/modules/frontend/gtk/Makefile
	rm -f src/modules/frontend/ncurses/Makefile
	rm -f src/modules/frontend/newt/Makefile
	rm -f src/modules/frontend/passthrough/Makefile
	rm -f src/modules/frontend/slang/Makefile
	rm -f src/modules/frontend/text/Makefile

.PHONY: cdebconf clean distclean
