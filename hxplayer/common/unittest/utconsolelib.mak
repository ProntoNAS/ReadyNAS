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

SRCS=ut_console_stub.cpp hx_ut_debug.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/utconsolelib/ut_console_stub.o dbg/obj/utconsolelib/hx_ut_debug.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I./pub

DEFINES=-I. -include dbg/common_unittest_utconsolelib_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/utconsolelib dbg/utconsolelib.a

dbg/obj/utconsolelib: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib

all_objects: $(OBJS)

dbg/utconsolelib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/utconsolelib.a $(OBJS)
	$(RANLIB)   dbg/utconsolelib.a

dbg/obj/utconsolelib/ut_console_stub.o: ut_console_stub.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/utconsolelib/ut_console_stub.o -c ut_console_stub.cpp

dbg/obj/utconsolelib/hx_ut_debug.o: hx_ut_debug.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/utconsolelib/hx_ut_debug.o -c hx_ut_debug.cpp

clean: 
	$(RM) dbg/utconsolelib.a dbg/obj/utconsolelib/ut_console_stub.o dbg/obj/utconsolelib/hx_ut_debug.o

../../debug/utconsolelib.a: dbg/utconsolelib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/utconsolelib.a"
	cp "dbg/utconsolelib.a" "../../debug/utconsolelib.a"

copy: ../../debug/utconsolelib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/utconsolelib -mutconsolelib.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: utconsolelib.mak,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/utconsolelib/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib

dbg/obj/utconsolelib/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib

dbg/obj/utconsolelib/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib

dbg/obj/utconsolelib/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/utconsolelib || mkdir dbg/obj/utconsolelib

