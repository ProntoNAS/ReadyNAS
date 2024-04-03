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

SRCS=bitstream.cpp bitpack.cpp riff.cpp deintl.cpp invtelec.cpp resize.cpp pyldparse.cpp pxtransp.cpp metautil.cpp sspayld.cpp adjtime.cpp rendstats.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/bitstream.o dbg/obj/bitpack.o dbg/obj/riff.o dbg/obj/deintl.o dbg/obj/invtelec.o dbg/obj/resize.o dbg/obj/pyldparse.o dbg/obj/pxtransp.o dbg/obj/metautil.o dbg/obj/sspayld.o dbg/obj/adjtime.o dbg/obj/rendstats.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I../../../common/include -I../../../common/system/pub -I../../../common/dbgtool/pub -I../../../common/util/pub -I../../../common/container/pub -I./pub

DEFINES=-I. -include dbg/datatype_common_util_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/dtutillib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/dtutillib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/dtutillib.a $(OBJS)
	$(RANLIB)   dbg/dtutillib.a

dbg/obj/bitstream.o: bitstream.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/bitstream.o -c bitstream.cpp

dbg/obj/bitpack.o: bitpack.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/bitpack.o -c bitpack.cpp

dbg/obj/riff.o: riff.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/riff.o -c riff.cpp

dbg/obj/deintl.o: deintl.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/deintl.o -c deintl.cpp

dbg/obj/invtelec.o: invtelec.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/invtelec.o -c invtelec.cpp

dbg/obj/resize.o: resize.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/resize.o -c resize.cpp

dbg/obj/pyldparse.o: pyldparse.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pyldparse.o -c pyldparse.cpp

dbg/obj/pxtransp.o: pxtransp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pxtransp.o -c pxtransp.cpp

dbg/obj/metautil.o: metautil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/metautil.o -c metautil.cpp

dbg/obj/sspayld.o: sspayld.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/sspayld.o -c sspayld.cpp

dbg/obj/adjtime.o: adjtime.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/adjtime.o -c adjtime.cpp

dbg/obj/rendstats.o: rendstats.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/rendstats.o -c rendstats.cpp

clean: 
	$(RM) dbg/dtutillib.a dbg/obj/bitstream.o dbg/obj/bitpack.o dbg/obj/riff.o dbg/obj/deintl.o dbg/obj/invtelec.o dbg/obj/resize.o dbg/obj/pyldparse.o dbg/obj/pxtransp.o dbg/obj/metautil.o dbg/obj/sspayld.o dbg/obj/adjtime.o dbg/obj/rendstats.o

../../../debug/dtutillib.a: dbg/dtutillib.a
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/dtutillib.a"
	cp "dbg/dtutillib.a" "../../../debug/dtutillib.a"

copy: ../../../debug/dtutillib.a

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

