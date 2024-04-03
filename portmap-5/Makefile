# @(#) Makefile 1.6 96/07/06 23:06:17

####################################
### Beginning of configurable stuff.

# By default, logfile entries are written to the same file as used for
# sendmail transaction logs. Change the definition of the following macro
# if you disagree. See `man 3 syslog' for examples. Some syslog versions
# do not provide this flexibility.
#
FACILITY=LOG_DAEMON

# To disable tcp-wrapper style access control, comment out the following
# macro definitions.  Access control can also be turned off by providing
# no access control tables. The local system, since it runs the portmap
# daemon, is always treated as an authorized host.

HOSTS_ACCESS= -DHOSTS_ACCESS
#WRAP_LIB = $(WRAP_DIR)/libwrap.a
WRAP_LIB = -lwrap

# Comment out if your RPC library does not allocate privileged ports for
# requests from processes with root privilege, or the new portmap will
# always reject requests to register/unregister services on privileged
# ports. You can find out by running "rpcinfo -p"; if all mountd and NIS
# daemons use a port >= 1024 you should probably disable the next line.

CHECK_PORT = -DCHECK_PORT

# Warning: troublesome feature ahead!! Enable only when you are really
# desperate!!
#
# It is possible to prevent an attacker from manipulating your portmapper
# tables from outside with requests that contain spoofed source addresses.
# The countermeasure is to force all rpc servers to register and
# unregister with the portmapper via the loopback network interface,
# instead of via the primary network interface that every host can talk
# to. For this countermeasure to work it is necessary to uncomment the
# LOOPBACK definition below, and to take the following additional steps:
# 
# (1) Modify the libc library (or librpc if you have one) and replace
# get_myaddress() by a version that selects the loopback address instead
# of the primary network interface address. A suitable version is
# provided in the file get_myaddress.c. This forces rpc servers to send
# all set/unset requests to the loopback address.
# 
# (2) Rebuild all statically-linked rpc servers with the modified
# library.
# 
# (3) Disable IP source routing in the kernel (otherwise an outside
# attacker can still send requests that appear to come from the local
# machine).
# 
# Instead of (1) it may be sufficient to run the rpc servers with a
# preload shared object that implements the alternate get_myaddress()
# behavior (see Makefile.shlib). You still need to disable IP source
# routing, though.
#
# I warned you, you need to be really desperate to do this. It is
# probably much easier to just block port UDP and TCP ports 111 on
# your routers.
#
# LOOPBACK = -DLOOPBACK_SETUNSET

# When the portmapper cannot find any local interfaces (it will complain
# to the syslog daemon) your system probably has variable-length socket
# address structures (struct sockaddr has a sa_len component; examples:
# AIX 4.1 and 4.4BSD). Uncomment next macro definition in that case.
#
# SA_LEN = -DHAS_SA_LEN		# AIX 4.x, BSD 4.4, FreeBSD, NetBSD

# With verbose logging on, HP-UX 9.x and AIX 4.1 leave zombies behind when
# SIGCHLD is not ignored. Enable next macro for a fix.
#
ZOMBIES = -DIGNORE_SIGCHLD	# AIX 4.x, HP-UX 9.x

# Uncomment the following macro if your system does not have u_long.
#
# ULONG	=-Du_long="unsigned long"

# Later versions of the tcp wrapper (log_tcp package) come with a
# libwrap.a object library. WRAP_DIR should specify the directory with
# that library.

WRAP_DIR= $(TCPD_DIR)

# Auxiliary object files that may be missing from your C library.
#
AUX	= daemon.o strerror.o

# NEXTSTEP is a little different. The following seems to work with NS 3.2
#
# SETPGRP	=-DUSE_SETPGRP00
# LIBS	= -m
# NSARCHS	= -arch m68k -arch i386 -arch hppa

# Auxiliary libraries that you may have to specify
#
# LIBS	= -lrpc

# Comment out if your compiler talks ANSI and understands const
#
#CONST   = -Dconst=

### End of configurable stuff.
##############################

GLIBC=$(shell grep -s -c __GLIBC__ /usr/include/features.h)

ifeq ($(GLIBC),0)
LIBS   += # -lbsd
else
LIBS   += -lnsl
endif


SHELL	= /bin/sh

COPT	= $(CONST) $(HOSTS_ACCESS) $(CHECK_PORT) \
	$(SYS) -DFACILITY=$(FACILITY) $(ULONG) $(ZOMBIES) $(SA_LEN) \
	$(LOOPBACK) $(SETPGRP)
CFLAGS	= -Wall $(COPT) -O2 $(NSARCHS)
OBJECTS	= portmap.o pmap_check.o from_local.o $(AUX)

all:	portmap pmap_dump pmap_set

portmap: $(OBJECTS) # $(WRAP_DIR)/libwrap.a
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(WRAP_LIB) $(LIBS)

pmap_dump: pmap_dump.c
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

pmap_set: pmap_set.c
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

from_local: from_local.c
	cc $(CFLAGS) -DTEST -o $@ from_local.c

get_myaddress: get_myaddress.c
	cc $(CFLAGS) -DTEST -o $@ get_myaddress.c $(LIBS)

install: all
	install -o root -g root -m 0755 -s portmap ${BASEDIR}/sbin
	install -o root -g root -m 0755 -s pmap_dump ${BASEDIR}/sbin
	install -o root -g root -m 0755 -s pmap_set ${BASEDIR}/sbin
	install -o root -g root -m 0644 portmap.8 ${BASEDIR}/usr/share/man/man8
	install -o root -g root -m 0644 pmap_dump.8 ${BASEDIR}/usr/share/man/man8
	install -o root -g root -m 0644 pmap_set.8 ${BASEDIR}/usr/share/man/man8
	cat README BLURB >${BASEDIR}/usr/share/doc/portmap/portmapper.txt
	gzip -9f ${BASEDIR}/usr/share/doc/portmap/portmapper.txt


lint:	
	lint $(COPT) $(OBJECTS:%.o=%.c)

clean:
	rm -f *.o portmap pmap_dump pmap_set from_local get_myaddress \
	    get_myaddress.so core

tidy:	clean
	chmod 755 . ; chmod -R a+r .

deps:
	@$(CC) -M $(CFLAGS) *.c | grep -v /usr/include |sed 's/\.\///'

daemon.o: daemon.c
from_local.o: from_local.c
get_myaddress.o: get_myaddress.c
pmap_check.o: pmap_check.c
pmap_check.o: pmap_check.h Makefile
pmap_dump.o: pmap_dump.c
pmap_set.o: pmap_set.c
portmap.o: portmap.c
portmap.o: pmap_check.h Makefile
strerror.o: strerror.c
