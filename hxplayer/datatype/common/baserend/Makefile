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

SRCS=baserend.cpp vbasernd.cpp vdclback.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/baserend.o dbg/obj/vbasernd.o dbg/obj/vdclback.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I../../../common/include -I../../../common/container/pub -I../../../common/dbgtool/pub -I../../../common/util/pub -I../../../common/system/pub -I../../../client/include -I./pub

DEFINES=-I. -include dbg/datatype_common_baserend_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=-lX11

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/baserend.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/baserend.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/baserend.a $(OBJS)
	$(RANLIB)   dbg/baserend.a

dbg/obj/baserend.o: baserend.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/baserend.o -c baserend.cpp

dbg/obj/vbasernd.o: vbasernd.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/vbasernd.o -c vbasernd.cpp

dbg/obj/vdclback.o: vdclback.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/vdclback.o -c vdclback.cpp

clean: 
	$(RM) dbg/baserend.a dbg/obj/baserend.o dbg/obj/vbasernd.o dbg/obj/vdclback.o

../../../debug/baserend.a: dbg/baserend.a
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/baserend.a"
	cp "dbg/baserend.a" "../../../debug/baserend.a"

copy: ../../../debug/baserend.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $

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

dbg/obj/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj

