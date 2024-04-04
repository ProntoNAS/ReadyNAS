CFLAGS+=-O2 -Wall

# uncomment the following line if you want to install to a different base dir.
#BASEDIR=/mnt/test

BINDIR:=/bin
MANDIR:=/usr/share/man

OBJS=hostname.o

hostname: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) -lnsl
	ln -fs hostname dnsdomainname
	ln -fs hostname domainname
	ln -fs hostname ypdomainname
	ln -fs hostname nisdomainname

install: hostname
	install -d ${BASEDIR}$(MANDIR)/man1
	install -o root -g root -m 0644 hostname.1 ${BASEDIR}$(MANDIR)/man1
	ln -fs hostname.1 ${BASEDIR}$(MANDIR)/man1/dnsdomainname.1
	ln -fs hostname.1 ${BASEDIR}$(MANDIR)/man1/domainname.1
	ln -fs hostname.1 ${BASEDIR}$(MANDIR)/man1/ypdomainname.1
	ln -fs hostname.1 ${BASEDIR}$(MANDIR)/man1/nisdomainname.1

	install -d ${BASEDIR}$(BINDIR)
	install -o root -g root -m 0755 hostname ${BASEDIR}$(BINDIR)
	ln -fs hostname ${BASEDIR}$(BINDIR)/dnsdomainname
	ln -fs hostname ${BASEDIR}$(BINDIR)/domainname
	ln -fs hostname ${BASEDIR}$(BINDIR)/nisdomainname
	ln -fs hostname ${BASEDIR}$(BINDIR)/ypdomainname

clean:
	-rm -f $(OBJS) hostname dnsdomainname domainname nisdomainname ypdomainname

.PHONY: clean install
