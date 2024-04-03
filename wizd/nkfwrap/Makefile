.SUFFIXES: .c .o .obj

CVS = cvs
#CFLAGS += -fPIC

all: nkfwrap.o nkfwrap_test

libnkf2.a: nkfwrap.o
	$(AR) rcs libnkf2.a nkfwrap.o

libnkf2.so: nkfwrap.o
	$(CC) -shared -o libnkf2.so nkfwrap.o

nkfwrap.o: nkf nkf/nkf.c nkf/utf8tbl.c nkf/config.h nkfwrap.c
	$(CC) $(CFLAGS) -c nkfwrap.c

nkfwrap_test: nkfwrap_test.o nkfwrap.o
	$(CC) $(CFLAGS) -o nkfwrap_test nkfwrap_test.o nkfwrap.o $(LDFLAGS)

update:
	if [ -d nkf/CVS ]; then \
		cd nkf; $(CVS) up; \
	else \
		[ -d nkf ] && rm -rf nkf/; $(MAKE) nkf; \
	fi

nkf:
	-rm -rf nkf/
	$(CVS) -z3 -d:pserver:anonymous@cvs.sourceforge.jp:/cvsroot/nkf co -d nkf/ nkf-2/nkf-utf8

nkf/utf8tbl.c:
	touch nkf/utf8tbl.c	# nkf <= 1.92 ?

nkf/config.h:
	touch nkf/config.h	# nkf <= 1.92 ?

clean:
	-rm -f nkfwrap_test *.o

moreclean:
	-rm -rf nkfwrap_test *.o nkf/
