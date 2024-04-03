__STANDARD__=YES

SRCROOT=../../../../../..

RM=rm -f

RM_DIR=rm -rf

MAKE_DEP=

MAKE_DEP_FLAGS=$(INCLUDES) $(DEFINES)

MAKE_LIB=$(SRCROOT)/build/bin/pyar cr

MAKE_LIB_FLAGS=

RANLIB=ranlib

CP=cp

MAKE=make

CC=gcc

CCFLAGS=-pipe -fsigned-char -O2 -D_PADRE -DDEBUG -D_DEBUG  $(INCLUDES) $(DEFINES)

CXX=g++

CXXFLAGS=--permissive -pipe -fsigned-char -O2 -DDEBUG -D_DEBUG  $(INCLUDES) $(DEFINES)

LD=g++

LDFLAGS=

SRCS=../aacdec.c ../aactabs.c ../real/bitstream.c ../real/buffers.c ../real/dct4.c ../real/decelmnt.c ../real/dequant.c ../real/fft.c ../real/filefmt.c ../real/huffman.c ../real/hufftabs.c ../real/imdct.c ../real/noiseless.c ../real/pns.c ../real/stproc.c ../real/tns.c ../real/trigtabs.c

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/umake_lib/par1/aacdec.o dbg/obj/umake_lib/par1/aactabs.o dbg/obj/umake_lib/par1/real/bitstream.o dbg/obj/umake_lib/par1/real/buffers.o dbg/obj/umake_lib/par1/real/dct4.o dbg/obj/umake_lib/par1/real/decelmnt.o dbg/obj/umake_lib/par1/real/dequant.o dbg/obj/umake_lib/par1/real/fft.o dbg/obj/umake_lib/par1/real/filefmt.o dbg/obj/umake_lib/par1/real/huffman.o dbg/obj/umake_lib/par1/real/hufftabs.o dbg/obj/umake_lib/par1/real/imdct.o dbg/obj/umake_lib/par1/real/noiseless.o dbg/obj/umake_lib/par1/real/pns.o dbg/obj/umake_lib/par1/real/stproc.o dbg/obj/umake_lib/par1/real/tns.o dbg/obj/umake_lib/par1/real/trigtabs.o

SOURCE_OBJS=

INCLUDES=-I../../../../../../common/runtime/pub -I/usr/X11R6/include -I../pub -I../../../../../../common/include -I./pub

DEFINES=-I. -include dbg/datatype_aac_codec_fixpt_decoder_hxwrap_umake_lib_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/umake_lib dbg/aacdeclib.a

dbg/obj/umake_lib: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib

all_objects: $(OBJS)

dbg/aacdeclib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	$(SRCROOT)/build/bin/pyar cr   dbg/aacdeclib.a $(OBJS)
	$(RANLIB)   dbg/aacdeclib.a

dbg/obj/umake_lib/par1/aacdec.o: ../aacdec.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/aacdec.o -c ../aacdec.c

dbg/obj/umake_lib/par1/aactabs.o: ../aactabs.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/aactabs.o -c ../aactabs.c

dbg/obj/umake_lib/par1/real/bitstream.o: ../real/bitstream.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/bitstream.o -c ../real/bitstream.c

dbg/obj/umake_lib/par1/real/buffers.o: ../real/buffers.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/buffers.o -c ../real/buffers.c

dbg/obj/umake_lib/par1/real/dct4.o: ../real/dct4.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/dct4.o -c ../real/dct4.c

dbg/obj/umake_lib/par1/real/decelmnt.o: ../real/decelmnt.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/decelmnt.o -c ../real/decelmnt.c

dbg/obj/umake_lib/par1/real/dequant.o: ../real/dequant.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/dequant.o -c ../real/dequant.c

dbg/obj/umake_lib/par1/real/fft.o: ../real/fft.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/fft.o -c ../real/fft.c

dbg/obj/umake_lib/par1/real/filefmt.o: ../real/filefmt.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/filefmt.o -c ../real/filefmt.c

dbg/obj/umake_lib/par1/real/huffman.o: ../real/huffman.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/huffman.o -c ../real/huffman.c

dbg/obj/umake_lib/par1/real/hufftabs.o: ../real/hufftabs.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/hufftabs.o -c ../real/hufftabs.c

dbg/obj/umake_lib/par1/real/imdct.o: ../real/imdct.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/imdct.o -c ../real/imdct.c

dbg/obj/umake_lib/par1/real/noiseless.o: ../real/noiseless.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/noiseless.o -c ../real/noiseless.c

dbg/obj/umake_lib/par1/real/pns.o: ../real/pns.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/pns.o -c ../real/pns.c

dbg/obj/umake_lib/par1/real/stproc.o: ../real/stproc.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/stproc.o -c ../real/stproc.c

dbg/obj/umake_lib/par1/real/tns.o: ../real/tns.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/tns.o -c ../real/tns.c

dbg/obj/umake_lib/par1/real/trigtabs.o: ../real/trigtabs.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_lib || mkdir dbg/obj/umake_lib
	-@test -d dbg/obj/umake_lib/par1 || mkdir dbg/obj/umake_lib/par1
	-@test -d dbg/obj/umake_lib/par1/real || mkdir dbg/obj/umake_lib/par1/real
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/umake_lib/par1/real/trigtabs.o -c ../real/trigtabs.c

clean: 
	$(RM) dbg/aacdeclib.a dbg/obj/umake_lib/par1/aacdec.o dbg/obj/umake_lib/par1/aactabs.o dbg/obj/umake_lib/par1/real/bitstream.o dbg/obj/umake_lib/par1/real/buffers.o dbg/obj/umake_lib/par1/real/dct4.o dbg/obj/umake_lib/par1/real/decelmnt.o dbg/obj/umake_lib/par1/real/dequant.o dbg/obj/umake_lib/par1/real/fft.o dbg/obj/umake_lib/par1/real/filefmt.o dbg/obj/umake_lib/par1/real/huffman.o dbg/obj/umake_lib/par1/real/hufftabs.o dbg/obj/umake_lib/par1/real/imdct.o dbg/obj/umake_lib/par1/real/noiseless.o dbg/obj/umake_lib/par1/real/pns.o dbg/obj/umake_lib/par1/real/stproc.o dbg/obj/umake_lib/par1/real/tns.o dbg/obj/umake_lib/par1/real/trigtabs.o

../../../../../../debug/aacdeclib.a: dbg/aacdeclib.a
	-@test -d ../../../../../../debug || mkdir ../../../../../../debug
	rm -f "../../../../../../debug/aacdeclib.a"
	cp "dbg/aacdeclib.a" "../../../../../../debug/aacdeclib.a"

copy: ../../../../../../debug/aacdeclib.a

depend: 
	$(SRCROOT)/build/bin/mkdepend.bin --gnumake -tdbg/obj/umake_lib -mumake_lib.mak $(INCLUDES) $(SRCS)

