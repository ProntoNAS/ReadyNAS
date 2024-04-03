
LVMIOPSOURCES = lvmiopversion.c lvm_check_lvm2.c lvm_check_lvm1.c

all: lvmiopversion

lvmiopversion: $(LVMIOPSOURCES)
	gcc -g -O2 -I. -o $@ $(LVMIOPSOURCES)

clean:
	rm -f lvmiopversion

install:
	install -d -o root -g root -m 0755 $(DESTDIR)/sbin
	install -o root -g root -m 0755 lvmiopversion \
		$(DESTDIR)/sbin/lvmiopversion

	install -d -o root -g root -m 0755 $(DESTDIR)/usr/sbin
	install -o root -g root -m 0755 \
		lvm-bin-scan $(DESTDIR)/usr/sbin/lvm-bin-scan

	install -d -o root -g root -m 0755 $(DESTDIR)/usr/share/man/man1
	install -p -o root -g root -m 0644 lvmiopversion.1 \
		$(DESTDIR)/usr/share/man/man1/
	install -p -o root -g root -m 0644 lvm-bin-scan.1 \
		$(DESTDIR)/usr/share/man/man1/

.PHONY: all clean install
