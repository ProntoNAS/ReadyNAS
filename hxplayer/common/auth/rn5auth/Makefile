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

SRCS=authfacplin.cpp hashauthbase.cpp rn5auth.cpp digestauth.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/authfacplin.o dbg/obj/hashauthbase.o dbg/obj/rn5auth.o dbg/obj/digestauth.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I../../../common/include -I./pub -I../../../common/util/pub -I../../../common/system/pub -I../../../common/container/pub -I../../../common/dbgtool/pub

DEFINES=-I. -include dbg/common_auth_rn5auth_ribodefs.h

STATIC_LIBS=../../../common/util/dbg/utillib.a ../../../common/system/dbg/syslib.a ../../../common/container/dbg/contlib.a ../../../common/dbgtool/dbg/debuglib.a ../../../common/runtime/dbg/runtlib.a

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/rn5auth.so

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/rn5auth.so: $(OBJS) $(STATIC_LIBS)
	-@test -d dbg || mkdir dbg
	$(LD) $(LDFLAGS) -o dbg/rn5auth.so $(OBJS) -L/usr/X11R6/lib $(DYNAMIC_LIBS) $(STATIC_LIBS) -u RMACreateInstance -u RMAShutdown -lgcc

dbg/obj/authfacplin.o: authfacplin.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/authfacplin.o -c authfacplin.cpp

dbg/obj/hashauthbase.o: hashauthbase.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hashauthbase.o -c hashauthbase.cpp

dbg/obj/rn5auth.o: rn5auth.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/rn5auth.o -c rn5auth.cpp

dbg/obj/digestauth.o: digestauth.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/digestauth.o -c digestauth.cpp

clean: 
	$(RM) dbg/rn5auth.so dbg/obj/authfacplin.o dbg/obj/hashauthbase.o dbg/obj/rn5auth.o dbg/obj/digestauth.o

../../../debug/rn5auth.so: dbg/rn5auth.so
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/rn5auth.so"
	cp "dbg/rn5auth.so" "../../../debug/rn5auth.so"

copy: ../../../debug/rn5auth.so

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

