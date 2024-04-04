# Unix makefile for the JBIG-KIT library

# Select an ANSI/ISO C compiler here, GNU gcc is recommended
CC = gcc

# Options for the compiler: A high optimization level is suggested
CFLAGS = -g -O -W -Wall -ansi -pedantic # --coverage

%.o: %.c
	$(CC) $(CFLAGS) -c $<
%.pic_o: %.c
	$(CC) -fPIC $(CFLAGS) -c -o $@ $<

all: libjbig.a libjbig.so libjbig85.a tstcodec tstcodec85

tstcodec: tstcodec.o jbig.o jbig_ar.o
	$(CC) $(CFLAGS) -o tstcodec tstcodec.o jbig.o jbig_ar.o

tstcodec85: tstcodec85.o jbig85.o jbig_ar.o
	$(CC) $(CFLAGS) -o tstcodec85 tstcodec85.o jbig85.o jbig_ar.o

libjbig.a: jbig.o jbig_ar.o jbig85.o
	rm -f libjbig.a
	$(if $(DEB_HOST_GNU_TYPE),$(DEB_HOST_GNU_TYPE)-ar,ar) cru \
		libjbig.a jbig.o jbig_ar.o
	$(if $(DEB_HOST_GNU_TYPE),$(DEB_HOST_GNU_TYPE)-ranlib,ranlib) \
		libjbig.a

libjbig.so: jbig.pic_o jbig_ar.pic_o jbig85.pic_o
	$(CC) -shared $(LDFLAGS) -Wl,-soname,libjbig.so.0 -o libjbig.so.0 $+
	ln -sf libjbig.so.0 libjbig.so

libjbig85.a: jbig85.o jbig_ar.o
	rm -f libjbig85.a
	$(if $(DEB_HOST_GNU_TYPE),$(DEB_HOST_GNU_TYPE)-ar,ar) cru \
		libjbig85.a jbig85.o jbig_ar.o
	$(if $(DEB_HOST_GNU_TYPE),$(DEB_HOST_GNU_TYPE)-ranlib,ranlib) \
		libjbig85.a

jbig.o: jbig.c jbig.h jbig_ar.h
jbig85.o: jbig85.c jbig85.h jbig_ar.h
jbig_ar.o: jbig_ar.c jbig_ar.h
tstcodec.o: tstcodec.c jbig.h
tstcodec85.o: tstcodec85.c jbig85.h

update-po: jbig.c jbig85.c Makefile
	xgettext -ojbig.pot -k_ \
	  --copyright-holder='Markus Kuhn' \
	  --msgid-bugs-address='http://www.cl.cam.ac.uk/~mgk25/jbigkit/' \
	  --package-name jbigkit \
	jbig.c jbig85.c
	cd po && for po in *.po ; do \
	  msgmerge --update $$po ../jbig.pot ; done

analyze:
	clang --analyze *.c

test: tstcodec tstcodec85
	./tstcodec
	./tstcodec85

t82test.pbm: tstcodec
	./tstcodec $@

clean:
	rm -f *.o *.gcda *.gcno *.gcov *.plist *~ core gmon.out dbg_d\=??.pbm
	rm -f *.so* *.a *.la *.pic_o
	rm -f t82test.pbm
	rm -f tstcodec tstcodec85
