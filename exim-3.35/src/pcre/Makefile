# Makefile for PCRE (Perl-Compatible Regular Expression) library for use by
# Exim. This is a tailored Makefile, not the normal one that comes with the
# PCRE distribution.

AR = ar cq
CC = gcc -O2 -Wall
CFLAGS =
RANLIB = @true

##############################################################################

OBJ = maketables.o get.o pcre.o study.o

all:            libpcre.a ../../util/pcretest

../../util/pcretest: libpcre.a pcretest.o
		$(CC) $(CFLAGS) -o ../../util/pcretest pcretest.o libpcre.a

libpcre.a:      $(OBJ)
		-rm -f libpcre.a
		$(AR) libpcre.a $(OBJ)
		$(RANLIB) libpcre.a

pcre.o:         chartables.c pcre.c config.h pcre.h internal.h Makefile
		$(CC) -c $(CFLAGS) pcre.c

maketables.o:   maketables.c config.h pcre.h internal.h Makefile
		$(CC) -c $(CFLAGS) maketables.c

get.o:          get.c pcre.h config.h internal.h Makefile
		$(CC) -c $(CFLAGS) get.c

study.o:        study.c pcre.h config.h internal.h Makefile
		$(CC) -c $(CFLAGS) study.c

pcretest.o:     pcretest.c config.h pcre.h internal.h Makefile
		$(CC) -c -DNOPOSIX $(CFLAGS) pcretest.c

# An auxiliary program makes the default character table source

chartables.c:   dftables
		./dftables >chartables.c

dftables:       dftables.c maketables.c config.h pcre.h internal.h Makefile
		$(CC) -o dftables $(CFLAGS) dftables.c

# End
