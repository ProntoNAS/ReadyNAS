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

SRCS=adler32.c compress.c crc32.c gzio.c uncompr.c deflate.c trees.c zutil.c inflate.c infblock.c inftrees.c infcodes.c infutil.c inffast.c

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/adler32.o dbg/obj/compress.o dbg/obj/crc32.o dbg/obj/gzio.o dbg/obj/uncompr.o dbg/obj/deflate.o dbg/obj/trees.o dbg/obj/zutil.o dbg/obj/inflate.o dbg/obj/infblock.o dbg/obj/inftrees.o dbg/obj/infcodes.o dbg/obj/infutil.o dbg/obj/inffast.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I../../../common/include -I./pub

DEFINES=-I. -include dbg/common_import_zlib_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/zlib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/zlib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/zlib.a $(OBJS)
	$(RANLIB)   dbg/zlib.a

dbg/obj/adler32.o: adler32.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/adler32.o -c adler32.c

dbg/obj/compress.o: compress.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/compress.o -c compress.c

dbg/obj/crc32.o: crc32.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/crc32.o -c crc32.c

dbg/obj/gzio.o: gzio.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/gzio.o -c gzio.c

dbg/obj/uncompr.o: uncompr.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/uncompr.o -c uncompr.c

dbg/obj/deflate.o: deflate.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/deflate.o -c deflate.c

dbg/obj/trees.o: trees.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/trees.o -c trees.c

dbg/obj/zutil.o: zutil.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/zutil.o -c zutil.c

dbg/obj/inflate.o: inflate.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/inflate.o -c inflate.c

dbg/obj/infblock.o: infblock.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/infblock.o -c infblock.c

dbg/obj/inftrees.o: inftrees.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/inftrees.o -c inftrees.c

dbg/obj/infcodes.o: infcodes.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/infcodes.o -c infcodes.c

dbg/obj/infutil.o: infutil.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/infutil.o -c infutil.c

dbg/obj/inffast.o: inffast.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/inffast.o -c inffast.c

clean: 
	$(RM) dbg/zlib.a dbg/obj/adler32.o dbg/obj/compress.o dbg/obj/crc32.o dbg/obj/gzio.o dbg/obj/uncompr.o dbg/obj/deflate.o dbg/obj/trees.o dbg/obj/zutil.o dbg/obj/inflate.o dbg/obj/infblock.o dbg/obj/inftrees.o dbg/obj/infcodes.o dbg/obj/infutil.o dbg/obj/inffast.o

../../../debug/zlib.a: dbg/zlib.a
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/zlib.a"
	cp "dbg/zlib.a" "../../../debug/zlib.a"

copy: ../../../debug/zlib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj

dbg/obj/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj

dbg/obj/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj

