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

SRCS=platform/unix/gettickcount.c platform/unix/unix_dll_common.cpp platform/unix/pthreadthreads.cpp platform/unix/UnixThreads.cpp platform/unix/linux_dll.cpp hxthread.cpp dllpath.cpp dllacces.cpp hxdate.cpp hxrquest.cpp genthrd.cpp pq.cpp pxtimer.cpp recognizer.cpp dllaccesserver.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/platform/unix/gettickcount.o dbg/obj/platform/unix/unix_dll_common.o dbg/obj/platform/unix/pthreadthreads.o dbg/obj/platform/unix/UnixThreads.o dbg/obj/platform/unix/linux_dll.o dbg/obj/hxthread.o dbg/obj/dllpath.o dbg/obj/dllacces.o dbg/obj/hxdate.o dbg/obj/hxrquest.o dbg/obj/genthrd.o dbg/obj/pq.o dbg/obj/pxtimer.o dbg/obj/recognizer.o dbg/obj/dllaccesserver.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -Ipub/platform/unix -I../include -I../container/pub -I../dbgtool/pub -I../util/pub -I../fileio/pub -I../runtime/pub -I./pub

DEFINES=-I. -include dbg/common_system_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/syslib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/syslib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/syslib.a $(OBJS)
	$(RANLIB)   dbg/syslib.a

dbg/obj/platform/unix/gettickcount.o: platform/unix/gettickcount.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/gettickcount.o -c platform/unix/gettickcount.c

dbg/obj/platform/unix/unix_dll_common.o: platform/unix/unix_dll_common.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_dll_common.o -c platform/unix/unix_dll_common.cpp

dbg/obj/platform/unix/pthreadthreads.o: platform/unix/pthreadthreads.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/pthreadthreads.o -c platform/unix/pthreadthreads.cpp

dbg/obj/platform/unix/UnixThreads.o: platform/unix/UnixThreads.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/UnixThreads.o -c platform/unix/UnixThreads.cpp

dbg/obj/platform/unix/linux_dll.o: platform/unix/linux_dll.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/linux_dll.o -c platform/unix/linux_dll.cpp

dbg/obj/hxthread.o: hxthread.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxthread.o -c hxthread.cpp

dbg/obj/dllpath.o: dllpath.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/dllpath.o -c dllpath.cpp

dbg/obj/dllacces.o: dllacces.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/dllacces.o -c dllacces.cpp

dbg/obj/hxdate.o: hxdate.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxdate.o -c hxdate.cpp

dbg/obj/hxrquest.o: hxrquest.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxrquest.o -c hxrquest.cpp

dbg/obj/genthrd.o: genthrd.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/genthrd.o -c genthrd.cpp

dbg/obj/pq.o: pq.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pq.o -c pq.cpp

dbg/obj/pxtimer.o: pxtimer.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pxtimer.o -c pxtimer.cpp

dbg/obj/recognizer.o: recognizer.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/recognizer.o -c recognizer.cpp

dbg/obj/dllaccesserver.o: dllaccesserver.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/dllaccesserver.o -c dllaccesserver.cpp

clean: 
	$(RM) dbg/syslib.a dbg/obj/platform/unix/gettickcount.o dbg/obj/platform/unix/unix_dll_common.o dbg/obj/platform/unix/pthreadthreads.o dbg/obj/platform/unix/UnixThreads.o dbg/obj/platform/unix/linux_dll.o dbg/obj/hxthread.o dbg/obj/dllpath.o dbg/obj/dllacces.o dbg/obj/hxdate.o dbg/obj/hxrquest.o dbg/obj/genthrd.o dbg/obj/pq.o dbg/obj/pxtimer.o dbg/obj/recognizer.o dbg/obj/dllaccesserver.o

../../debug/syslib.a: dbg/syslib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/syslib.a"
	cp "dbg/syslib.a" "../../debug/syslib.a"

copy: ../../debug/syslib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:35 hagi Exp $

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

