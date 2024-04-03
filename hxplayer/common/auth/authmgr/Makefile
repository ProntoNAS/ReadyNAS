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

LDFLAGS=-shared

SRCS=authmgr.cpp cliauth.cpp svrauth.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/authmgr.o dbg/obj/cliauth.o dbg/obj/svrauth.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I../../../common/include -I./pub -I../../../common/system/pub -I../../../common/dbgtool/pub -I../../../common/util/pub -I../../../common/container/pub

DEFINES=-I. -include dbg/common_auth_authmgr_ribodefs.h

STATIC_LIBS=../../../common/system/dbg/syslib.a ../../../common/dbgtool/dbg/debuglib.a ../../../common/util/dbg/utillib.a ../../../common/container/dbg/contlib.a ../../../common/runtime/dbg/runtlib.a

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/authmgr.so

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/authmgr.so: $(OBJS) $(STATIC_LIBS)
	-@test -d dbg || mkdir dbg
	$(LD) $(LDFLAGS) -o dbg/authmgr.so $(OBJS) -L/usr/X11R6/lib $(DYNAMIC_LIBS) $(STATIC_LIBS) -u RMACreateInstance -lgcc

dbg/obj/authmgr.o: authmgr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/authmgr.o -c authmgr.cpp

dbg/obj/cliauth.o: cliauth.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cliauth.o -c cliauth.cpp

dbg/obj/svrauth.o: svrauth.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/svrauth.o -c svrauth.cpp

clean: 
	$(RM) dbg/authmgr.so dbg/obj/authmgr.o dbg/obj/cliauth.o dbg/obj/svrauth.o

../../../debug/authmgr.so: dbg/authmgr.so
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/authmgr.so"
	cp "dbg/authmgr.so" "../../../debug/authmgr.so"

copy: ../../../debug/authmgr.so

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

