LANGS = de es fr ja ru
RAW_LANGS = pl

CLEANFILES = po4a-stamp

all-local: po4a-stamp

install-data-local: po4a/po/base-passwd.pot
	$(MKDIR_P) $(DESTDIR)$(mandir)/man8
	$(INSTALL_DATA) update-passwd.8 $(DESTDIR)$(mandir)/man8/
	set -e; for lang in $(LANGS) $(RAW_LANGS); do \
		mkdir -p $(DESTDIR)$(mandir)/$$lang/man8; \
		$(INSTALL_DATA) update-passwd.$$lang.8 \
			$(DESTDIR)$(mandir)/$$lang/man8/update-passwd.8; \
	done

clean-local:
	set -e; for lang in $(LANGS); do \
		rm -f update-passwd.$$lang.8; \
	done

po4a-stamp: update-passwd.8
	cd po4a && po4a po4a.cfg
	touch $@
