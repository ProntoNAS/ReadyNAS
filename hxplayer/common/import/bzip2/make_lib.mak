__STANDARD__=YES

SRCROOT=../../..

RM=rm -f

RM_DIR=rm -rf

MAKE_DEP=

MAKE_DEP_FLAGS=$(INCLUDES) $(DEFINES)

MAKE_LIB=/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr

MAKE_LIB_FLAGS=

RANLIB=ranlib

CP=cp

MAKE=make

CC=gcc

CCFLAGS=--permissive -pipe -fsigned-char -O0 -g -DDEBUG -D_DEBUG  $(INCLUDES) $(DEFINES)

CXX=g++

CXXFLAGS=--permissive -pipe -fsigned-char -O0 -g -DDEBUG -D_DEBUG  $(INCLUDES) $(DEFINES)

LD=g++

LDFLAGS=

SRCS=blocksort.c huffman.c crctable.c randtable.c compress.c decompress.c bzlib.c

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/make_lib/blocksort.o dbg/obj/make_lib/huffman.o dbg/obj/make_lib/crctable.o dbg/obj/make_lib/randtable.o dbg/obj/make_lib/compress.o dbg/obj/make_lib/decompress.o dbg/obj/make_lib/bzlib.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I./pub

DEFINES=-I. -include dbg/common_import_bzip2_make_lib_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/make_lib dbg/libbz2.a

dbg/obj/make_lib: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib

all_objects: $(OBJS)

dbg/libbz2.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/libbz2.a $(OBJS)
	$(RANLIB)   dbg/libbz2.a

dbg/obj/make_lib/blocksort.o: blocksort.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/blocksort.o -c blocksort.c

dbg/obj/make_lib/huffman.o: huffman.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/huffman.o -c huffman.c

dbg/obj/make_lib/crctable.o: crctable.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/crctable.o -c crctable.c

dbg/obj/make_lib/randtable.o: randtable.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/randtable.o -c randtable.c

dbg/obj/make_lib/compress.o: compress.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/compress.o -c compress.c

dbg/obj/make_lib/decompress.o: decompress.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/decompress.o -c decompress.c

dbg/obj/make_lib/bzlib.o: bzlib.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/make_lib/bzlib.o -c bzlib.c

clean: 
	$(RM) dbg/libbz2.a dbg/obj/make_lib/blocksort.o dbg/obj/make_lib/huffman.o dbg/obj/make_lib/crctable.o dbg/obj/make_lib/randtable.o dbg/obj/make_lib/compress.o dbg/obj/make_lib/decompress.o dbg/obj/make_lib/bzlib.o

../debug/libbz2.a: dbg/libbz2.a
	-@test -d ../debug || mkdir ../debug
	rm -f "../debug/libbz2.a"
	cp "dbg/libbz2.a" "../debug/libbz2.a"

copy: ../debug/libbz2.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/make_lib -mmake_lib.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: make_lib.mak,v 1.1.1.1 2006/03/29 16:45:29 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/make_lib/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib

dbg/obj/make_lib/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib

dbg/obj/make_lib/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/make_lib || mkdir dbg/obj/make_lib

