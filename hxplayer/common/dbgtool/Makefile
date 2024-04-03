__STANDARD__=YES

SRCROOT=../..

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

SRCS=hxassert.cpp hxstat.cpp multilog.cpp print.cpp platform/default/debug.cpp memprb.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/hxassert.o dbg/obj/hxstat.o dbg/obj/multilog.o dbg/obj/print.o dbg/obj/platform/default/debug.o dbg/obj/memprb.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I../include -I../runtime/pub -I../system/pub -I../fileio/pub -I../util/pub -Ipub/platform/default -I./pub

DEFINES=-I. -include dbg/common_dbgtool_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/debuglib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/debuglib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/debuglib.a $(OBJS)
	$(RANLIB)   dbg/debuglib.a

dbg/obj/hxassert.o: hxassert.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxassert.o -c hxassert.cpp

dbg/obj/hxstat.o: hxstat.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxstat.o -c hxstat.cpp

dbg/obj/multilog.o: multilog.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/multilog.o -c multilog.cpp

dbg/obj/print.o: print.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/print.o -c print.cpp

dbg/obj/platform/default/debug.o: platform/default/debug.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/default || mkdir dbg/obj/platform/default
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/default/debug.o -c platform/default/debug.cpp

dbg/obj/memprb.o: memprb.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/memprb.o -c memprb.cpp

clean: 
	$(RM) dbg/debuglib.a dbg/obj/hxassert.o dbg/obj/hxstat.o dbg/obj/multilog.o dbg/obj/print.o dbg/obj/platform/default/debug.o dbg/obj/memprb.o

../../debug/debuglib.a: dbg/debuglib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/debuglib.a"
	cp "dbg/debuglib.a" "../../debug/debuglib.a"

copy: ../../debug/debuglib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:36 hagi Exp $

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

