SUBDIRS=	en_US.88591 fr_FR.88591 de_DE.88591
GENCATNEW=	gencat --new
GENCAT=		gencat


all:	catalogs ../hostname-nls.h


../hostname-nls.h:	hostname.m
	${GENCATNEW} /dev/null hostname.m -H ../hostname-nls.h

hostname.m:	en_US.88591/hostname.cat

catalogs:	de_DE.88591/hostname.cat fr_FR.88591/hostname.cat \
		en_US.88591/hostname.cat

en_US.88591/hostname.cat:	en_US.88591/*.m
	@echo en_US.88591
	@cp en_US.88591/hostname.m hostname.m
	@if [ -f en_US.88591/hostname.cat ]; then \
		${GENCAT} en_US.88591/hostname.cat hostname.m; \
	else \
		${GENCATNEW} en_US.88591/hostname.cat en_US.88591/hostname.m; \
	fi

fr_FR.88591/hostname.cat:	fr_FR.88591/*.m
	@echo fr_FR.88591
	@cp fr_FR.88591/hostname.m hostname.m
	@if [ -f fr_FR.88591/hostname.cat ]; then \
		${GENCAT} fr_FR.88591/hostname.cat fr_FR.88591/hostname.m; \
	else \
		${GENCATNEW} fr_FR.88591/hostname.cat fr_FR.88591/hostname.m; \
	fi

de_DE.88591/hostname.cat:	de_DE.88591/*.m
	@echo de_DE.88591
	@cp de_DE.88591/hostname.m hostname.m
	@if [ -f de_DE.88591/hostname.cat ]; then \
		${GENCAT} de_DE.88591/hostname.cat de_DE.88591/hostname.m; \
	else \
		${GENCATNEW} de_DE.88591/hostname.cat de_DE.88591/hostname.m; \
	fi

clean:
	for i in ${SUBDIRS} ; \
	do \
	    rm -f $$i/*~ $$i/*.orig $$i/DEADJOE ; \
	    rm -f $$i/hostname.cat ; \
	done; \
	rm -f ../hostname-nls.h *~ *.orig

clobber: clean

