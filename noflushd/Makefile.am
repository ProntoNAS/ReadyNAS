SUBDIRS=src man skripts
DIST_SUBDIRS=src man skripts
EXTRA_DIST=autogen.sh BUGS
doc_DATA=BUGS README THANKS TODO ChangeLog 

dist-hook:
	mkdir $(distdir)/contrib
	cp -p `find $(srcdir)/contrib/ -type f -maxdepth 1` $(distdir)/contrib/
	mkdir $(distdir)/debian
	cp -p `find $(srcdir)/debian/ -type f -maxdepth 1` $(distdir)/debian/
	mkdir $(distdir)/debian/po
	cp -p `find $(srcdir)/debian/po -type f -maxdepth 1` $(distdir)/debian/po
	mkdir $(distdir)/redhat
	cp -p `find $(srcdir)/redhat/ -type f -maxdepth 1` $(distdir)/redhat/
