LIBS=-lpam
APXS=apxs
# try this, if you're not root and apxs is in the standard place
#APXS=/usr/sbin/apxs

SOURCES=mod_auth_pam.c mod_auth_sys_group.c
# Apache 2.0 uses GNU libtool, hence the libtool suffix
TARGETS=$(SOURCES:.c=.la)

all: $(TARGETS)

# general rule to build
%.la: %.c
	$(APXS) -c $< $(LIBS)

install: $(TARGETS)
	$(APXS) -i $(TARGETS)
	[ -d /etc/pam.d/ -a ! -f /etc/pam.d/httpd ] && install -m 644 -o root -g root samples/httpd /etc/pam.d/

clean:
	-rm -f $(TARGETS) *~ $(SOURCES:.c=.slo) $(SOURCES:.c=.lo) $(SOURCES:.c=.so) $(SOURCES:.c=.o) 
