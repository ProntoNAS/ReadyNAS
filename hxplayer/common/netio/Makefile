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

SRCS=platform/unix/unix_net.cpp platform/unix/unix_conn.cpp conn.cpp sockio.cpp thrdconn.cpp threngin.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/platform/unix/unix_net.o dbg/obj/platform/unix/unix_conn.o dbg/obj/conn.o dbg/obj/sockio.o dbg/obj/thrdconn.o dbg/obj/threngin.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I../../common/netio/pub/platform/unix -I../../common/include -I../../common/container/pub -I../../common/util/pub -I../../common/dbgtool/pub -I../../common/system/pub -I../../common/runtime/hlxclib/sys -I./pub

DEFINES=-I. -include dbg/common_netio_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/netiolib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/netiolib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/netiolib.a $(OBJS)
	$(RANLIB)   dbg/netiolib.a

dbg/obj/platform/unix/unix_net.o: platform/unix/unix_net.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_net.o -c platform/unix/unix_net.cpp

dbg/obj/platform/unix/unix_conn.o: platform/unix/unix_conn.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_conn.o -c platform/unix/unix_conn.cpp

dbg/obj/conn.o: conn.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/conn.o -c conn.cpp

dbg/obj/sockio.o: sockio.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/sockio.o -c sockio.cpp

dbg/obj/thrdconn.o: thrdconn.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/thrdconn.o -c thrdconn.cpp

dbg/obj/threngin.o: threngin.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/threngin.o -c threngin.cpp

clean: 
	$(RM) dbg/netiolib.a dbg/obj/platform/unix/unix_net.o dbg/obj/platform/unix/unix_conn.o dbg/obj/conn.o dbg/obj/sockio.o dbg/obj/thrdconn.o dbg/obj/threngin.o

../../debug/netiolib.a: dbg/netiolib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/netiolib.a"
	cp "dbg/netiolib.a" "../../debug/netiolib.a"

copy: ../../debug/netiolib.a

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

dbg/obj/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj

