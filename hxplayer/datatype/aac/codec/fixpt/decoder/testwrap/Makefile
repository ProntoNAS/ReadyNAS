# Written by Infrant
#
CC=gcc -g -O2

aac2wav: main.c debug.c ../hxwrap/dbg/aacdeclib.a aacdec.h wrapper.h
	$(CC) -o aac2wav main.c debug.c ../hxwrap/dbg/aacdeclib.a

clean:
	rm -f *~ *.o aac2wav

