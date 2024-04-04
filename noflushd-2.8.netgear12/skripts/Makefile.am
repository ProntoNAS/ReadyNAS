init_SCRIPTS=noflushd
EXTRA_DIST=noflushd.templ

noflushd:
	sed -e "s,+sbindir+,$(sbindir)," < $(srcdir)/noflushd.templ > noflushd
	chmod +x noflushd

clean-local:
	test -z "noflushd" || rm -f "noflushd"
