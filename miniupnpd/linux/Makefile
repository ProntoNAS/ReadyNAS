# $Id: Makefile,v 1.1.1.1 2007-01-12 02:38:46 jmaggard Exp $
CFLAGS=-Wall -g

all:	iptcrdr.o testiptcrdr

clean:
	$(RM) *.o

testiptcrdr:	testiptcrdr.o iptcrdr.o -liptc

iptcrdr.o:	iptcrdr.c iptcrdr.h

