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

SRCS=string.cpp stdlib.cpp assert.cpp hlxosstr.cpp safestring.cpp windows.cpp time.cpp stat.cpp errno.cpp stdio.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/string.o dbg/obj/stdlib.o dbg/obj/assert.o dbg/obj/hlxosstr.o dbg/obj/safestring.o dbg/obj/windows.o dbg/obj/time.o dbg/obj/stat.o dbg/obj/errno.o dbg/obj/stdio.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I../../common/include -I./pub

DEFINES=-I. -include dbg/common_runtime_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/runtlib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/runtlib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/runtlib.a $(OBJS)
	$(RANLIB)   dbg/runtlib.a

dbg/obj/string.o: string.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/string.o -c string.cpp

dbg/obj/stdlib.o: stdlib.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/stdlib.o -c stdlib.cpp

dbg/obj/assert.o: assert.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/assert.o -c assert.cpp

dbg/obj/hlxosstr.o: hlxosstr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hlxosstr.o -c hlxosstr.cpp

dbg/obj/safestring.o: safestring.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/safestring.o -c safestring.cpp

dbg/obj/windows.o: windows.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/windows.o -c windows.cpp

dbg/obj/time.o: time.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/time.o -c time.cpp

dbg/obj/stat.o: stat.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/stat.o -c stat.cpp

dbg/obj/errno.o: errno.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/errno.o -c errno.cpp

dbg/obj/stdio.o: stdio.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/stdio.o -c stdio.cpp

clean: 
	$(RM) dbg/runtlib.a dbg/obj/string.o dbg/obj/stdlib.o dbg/obj/assert.o dbg/obj/hlxosstr.o dbg/obj/safestring.o dbg/obj/windows.o dbg/obj/time.o dbg/obj/stat.o dbg/obj/errno.o dbg/obj/stdio.o

../../debug/runtlib.a: dbg/runtlib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/runtlib.a"
	cp "dbg/runtlib.a" "../../debug/runtlib.a"

copy: ../../debug/runtlib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:33 hagi Exp $

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

