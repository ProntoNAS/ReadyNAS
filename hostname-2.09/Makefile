CFLAGS=-O2 -Wall -Wstrict-prototypes -Wmissing-prototypes -DSTANDALONE
LDFLAGS=-s -lresolv

# uncomment the following line if you want NLS
# NLS = 1

# uncomment the following line if you want to install to a different base dir.
# BASEDIR=/mnt/test

###############################################################################

ifneq (${NLS},)
CFLAGS += -DNLS=1
endif

OBJS=hostname.o localdomain.o localhost.o

hostname: nlscat $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS)
	strip hostname
	ln -f hostname dnsdomainname

nlscat:
ifneq (${NLS},)
	(cd nls && make)
endif

pkgs=dnsdomainname
nlspkgs=en fr de
install: hostname
ifneq (${NLS},)
	for a in $(nlspkgs);do\
		install -o root -g root -m 755 -d ${BASEDIR}/usr/lib/locale/$${a}_US.88591;\
		install -o root -g root -m 755 nls/$${a}_US.88591/hostname.cat ${BASEDIR}/usr/lib/locale/$${a}_US.88591;\
		if [ -d man/$${a}_US.88591/ ];then \
			install -o root -g root -m 755 -d ${BASEDIR}/usr/share/man/$${a}_US.88591/man1;\
			install -o root -g root -m 755 man/$${a}_US.88591/hostname.1 ${BASEDIR}/usr/share/man/$${a}_US.88591/man1;\
			for b in $(pkgs);do \
				ln -fs hostname.1 ${BASEDIR}/usr/share/man/$${a}_US.88591/man1/$$b.1; \
			done \
		fi \
	done
else
	install -o root -g root -m 0755 -d ${BASEDIR}/usr/share/man/man1
	install -o root -g root -m 0644 man/en_US.88591/hostname.1 ${BASEDIR}/usr/share/man/man1
	for a in $(pkgs);do \
		ln -fs hostname.1 ${BASEDIR}/usr/share/man/man1/$$a.1; \
	done
endif
	install -o root -g root -m 0755 hostname ${BASEDIR}/bin
	for a in $(pkgs);do \
		ln -f hostname ${BASEDIR}/bin/$$a; \
	done \

clean:
ifneq (${NLS},)
	(cd nls && make clean)
endif
	rm -f $(OBJS) hostname dnsdomainname core

