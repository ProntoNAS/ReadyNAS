# Makefile for ACPI daemon

# update these numbers for new releases
VERSION = 1.0.4

INSTPREFIX =
BINDIR = $(INSTPREFIX)/usr/bin
SBINDIR = $(INSTPREFIX)/usr/sbin
MAN8DIR = $(INSTPREFIX)/usr/share/man/man8

SBIN_PROGS = acpid
BIN_PROGS = acpi_listen
PROGS = $(SBIN_PROGS) $(BIN_PROGS)

acpid_SRCS = acpid.c event.c ud_socket.c
acpid_OBJS = $(acpid_SRCS:.c=.o)

acpi_listen_SRCS = acpi_listen.c ud_socket.c
acpi_listen_OBJS = $(acpi_listen_SRCS:.c=.o)

MAN8 = acpid.8 acpi_listen.8
MAN8GZ = $(MAN8:.8=.8.gz)

CFLAGS = -Wall -Werror -g $(DEFS)
DEFS = -DVERSION="\"$(VERSION)\""

all: $(PROGS)

acpid: $(acpid_OBJS)

acpi_listen: $(acpi_listen_OBJS)

man: $(MAN8)
	for a in $^; do gzip -f -9 -c $$a > $$a.gz; done

install: $(PROGS) man
	mkdir -p $(SBINDIR)
	install -m 750 acpid $(SBINDIR)
	install -m 755 acpi_listen $(BINDIR)
	mkdir -p $(MAN8DIR)
	install -m 644 $(MAN8GZ) $(MAN8DIR)

DISTTMP=/tmp
dist:
	rm -rf $(DISTTMP)/acpid-$(VERSION)
	mkdir -p $(DISTTMP)/acpid-$(VERSION)
	cp -a * $(DISTTMP)/acpid-$(VERSION)
	find $(DISTTMP)/acpid-$(VERSION) -type d -name CVS | xargs rm -rf
	make -C $(DISTTMP)/acpid-$(VERSION) clean
	tar -C $(DISTTMP) -zcvf acpid-$(VERSION).tar.gz acpid-$(VERSION)
	rm -rf $(DISTTMP)/acpid-$(VERSION)

clean:
	$(RM) $(PROGS) $(MAN8GZ) *.o

