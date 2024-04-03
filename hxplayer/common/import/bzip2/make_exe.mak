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

SRCS=blocksort.c huffman.c crctable.c randtable.c compress.c decompress.c bzlib.c bzip2.c

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/make_exe/blocksort.o dbg/obj/make_exe/huffman.o dbg/obj/make_exe/crctable.o dbg/obj/make_exe/randtable.o dbg/obj/make_exe/compress.o dbg/obj/make_exe/decompress.o dbg/obj/make_exe/bzlib.o dbg/obj/make_exe/bzip2.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I./pub

DEFINES=-I. -include dbg/common_import_bzip2_make_exe_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -o  $@ -c $<

all: dbg/obj/make_exe dbg/bzip2

dbg/obj/make_exe: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe

all_objects: $(OBJS)

dbg/bzip2: $(OBJS) $(STATIC_LIBS)
	-@test -d dbg || mkdir dbg
	$(LD) $(LDFLAGS) -o dbg/bzip2 $(OBJS) -L/usr/X11R6/lib $(DYNAMIC_LIBS) $(STATIC_LIBS)

dbg/obj/make_exe/blocksort.o: blocksort.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/blocksort.o -c blocksort.c

dbg/obj/make_exe/huffman.o: huffman.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/huffman.o -c huffman.c

dbg/obj/make_exe/crctable.o: crctable.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/crctable.o -c crctable.c

dbg/obj/make_exe/randtable.o: randtable.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/randtable.o -c randtable.c

dbg/obj/make_exe/compress.o: compress.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/compress.o -c compress.c

dbg/obj/make_exe/decompress.o: decompress.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/decompress.o -c decompress.c

dbg/obj/make_exe/bzlib.o: bzlib.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/bzlib.o -c bzlib.c

dbg/obj/make_exe/bzip2.o: bzip2.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe
	$(CC) $(CCFLAGS) -o dbg/obj/make_exe/bzip2.o -c bzip2.c

clean: 
	$(RM) dbg/bzip2 dbg/obj/make_exe/blocksort.o dbg/obj/make_exe/huffman.o dbg/obj/make_exe/crctable.o dbg/obj/make_exe/randtable.o dbg/obj/make_exe/compress.o dbg/obj/make_exe/decompress.o dbg/obj/make_exe/bzlib.o dbg/obj/make_exe/bzip2.o

../debug/bzip2: dbg/bzip2
	-@test -d ../debug || mkdir ../debug
	rm -f "../debug/bzip2"
	cp "dbg/bzip2" "../debug/bzip2"

copy: ../debug/bzip2

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/make_exe -mmake_exe.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: make_exe.mak,v 1.1.1.1 2006/03/29 16:45:29 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/make_exe/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe

dbg/obj/make_exe/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe

dbg/obj/make_exe/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/make_exe || mkdir dbg/obj/make_exe

