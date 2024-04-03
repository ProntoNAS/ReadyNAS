#
# Makefile
#

all:

clean:
	rm -f cacert.org.crt

install:
	cat root.crt class3.crt > cacert.org.crt
	install -m 644 cacert.org.crt $(CERTSDIR)/cacert.org.crt

