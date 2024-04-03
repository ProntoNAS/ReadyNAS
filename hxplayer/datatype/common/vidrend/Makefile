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

SRCS=vidrend.cpp vidrendf.cpp vidstats.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/vidrend.o dbg/obj/vidrendf.o dbg/obj/vidstats.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I../../../common/include -I../../../common/container/pub -I../../../common/dbgtool/pub -I../../../common/util/pub -I../../../common/system/pub -I../../../client/common/container/pub -I../../../client/include -I../../../protocol/sdp/pub -I../../../video/include -I../container/pub -I../util/pub -I./pub

DEFINES=-I. -include dbg/datatype_common_vidrend_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/vidrend.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/vidrend.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/vidrend.a $(OBJS)
	$(RANLIB)   dbg/vidrend.a

dbg/obj/vidrend.o: vidrend.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/vidrend.o -c vidrend.cpp

dbg/obj/vidrendf.o: vidrendf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/vidrendf.o -c vidrendf.cpp

dbg/obj/vidstats.o: vidstats.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/vidstats.o -c vidstats.cpp

clean: 
	$(RM) dbg/vidrend.a dbg/obj/vidrend.o dbg/obj/vidrendf.o dbg/obj/vidstats.o

../../../debug/vidrend.a: dbg/vidrend.a
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/vidrend.a"
	cp "dbg/vidrend.a" "../../../debug/vidrend.a"

copy: ../../../debug/vidrend.a

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

