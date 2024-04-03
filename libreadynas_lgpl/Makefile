
all: libreadynas_lgpl

ifeq (,$(findstring arm,${shell uname -m}))
        CROSS=arm-linux-gnueabi-
        CROSS_D=arm-linux-gnueabi
endif

libreadynas_lgpl: util.lo lcd.lo psu.lo sgio.lo
	@echo Linking $@.so
	@${CROSS}libtool --tag=CC --quiet --mode=link ${CROSS}gcc -o libreadynas_lgpl.la util.lo lcd.lo psu.lo sgio.lo -rpath /usr/${CROSS_D}/lib -version-info '1:1:1'

install: all
	${CROSS}libtool --tag=CC --mode=install install -s libreadynas_lgpl.la /usr/${CROSS_D}/lib
	install -D readynas.h /usr/${CROSS_D}/include/libreadynas_lgpl/readynas.h

clean:
	rm -rf *.o *.lo *.la *.so .libs _libs *~

.SUFFIXES: .c .o .lo

.c.lo:
	@echo Compiling $*.c
	@${CROSS}libtool --tag=CC --quiet --mode=compile ${CROSS}gcc -Wall -o $@ -c $< && exit 0;\
		echo "The following command failed:" 1>&2;\
		echo "${CROSS}libtool --tag=CC --quiet --mode=compile ${CROSS}gcc -Wall -o $@ -c $<";\
		${CROSS}libtool --tag=CC --quiet --mode=compile ${CROSS}gcc -Wall -o $@ -c $< &>/dev/null
