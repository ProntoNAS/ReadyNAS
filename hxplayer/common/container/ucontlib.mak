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

SRCS=carray.cpp hxslist.cpp chxpckts.cpp hxbuffer.cpp hxsbuffer.cpp buffer.cpp nestbuff.cpp chxfgbuf.cpp hxstring.cpp hxstradv.cpp hxstrfmt.cpp cbqueue.cpp cbbqueue.cpp timebuff.cpp hxguid.cpp statinfo.cpp hxstack.cpp hxvalues.cpp hxordval.cpp chxmapptrtoptr.cpp chxmapstringtoob.cpp chxmaplongtoobj.cpp chxmapguidtoobj.cpp chxmapstringtostring.cpp hxmaputils.cpp dict.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/ucontlib/carray.o dbg/obj/ucontlib/hxslist.o dbg/obj/ucontlib/chxpckts.o dbg/obj/ucontlib/hxbuffer.o dbg/obj/ucontlib/hxsbuffer.o dbg/obj/ucontlib/buffer.o dbg/obj/ucontlib/nestbuff.o dbg/obj/ucontlib/chxfgbuf.o dbg/obj/ucontlib/hxstring.o dbg/obj/ucontlib/hxstradv.o dbg/obj/ucontlib/hxstrfmt.o dbg/obj/ucontlib/cbqueue.o dbg/obj/ucontlib/cbbqueue.o dbg/obj/ucontlib/timebuff.o dbg/obj/ucontlib/hxguid.o dbg/obj/ucontlib/statinfo.o dbg/obj/ucontlib/hxstack.o dbg/obj/ucontlib/hxvalues.o dbg/obj/ucontlib/hxordval.o dbg/obj/ucontlib/chxmapptrtoptr.o dbg/obj/ucontlib/chxmapstringtoob.o dbg/obj/ucontlib/chxmaplongtoobj.o dbg/obj/ucontlib/chxmapguidtoobj.o dbg/obj/ucontlib/chxmapstringtostring.o dbg/obj/ucontlib/hxmaputils.o dbg/obj/ucontlib/dict.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I../include -I../dbgtool/pub -I../runtime/pub -I../util/pub -I../system/pub -I./pub

DEFINES=-I. -include dbg/common_container_ucontlib_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/ucontlib dbg/contlib.a

dbg/obj/ucontlib: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib

all_objects: $(OBJS)

dbg/contlib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/contlib.a $(OBJS)
	$(RANLIB)   dbg/contlib.a

dbg/obj/ucontlib/carray.o: carray.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/carray.o -c carray.cpp

dbg/obj/ucontlib/hxslist.o: hxslist.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxslist.o -c hxslist.cpp

dbg/obj/ucontlib/chxpckts.o: chxpckts.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxpckts.o -c chxpckts.cpp

dbg/obj/ucontlib/hxbuffer.o: hxbuffer.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxbuffer.o -c hxbuffer.cpp

dbg/obj/ucontlib/hxsbuffer.o: hxsbuffer.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxsbuffer.o -c hxsbuffer.cpp

dbg/obj/ucontlib/buffer.o: buffer.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/buffer.o -c buffer.cpp

dbg/obj/ucontlib/nestbuff.o: nestbuff.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/nestbuff.o -c nestbuff.cpp

dbg/obj/ucontlib/chxfgbuf.o: chxfgbuf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxfgbuf.o -c chxfgbuf.cpp

dbg/obj/ucontlib/hxstring.o: hxstring.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxstring.o -c hxstring.cpp

dbg/obj/ucontlib/hxstradv.o: hxstradv.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxstradv.o -c hxstradv.cpp

dbg/obj/ucontlib/hxstrfmt.o: hxstrfmt.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxstrfmt.o -c hxstrfmt.cpp

dbg/obj/ucontlib/cbqueue.o: cbqueue.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/cbqueue.o -c cbqueue.cpp

dbg/obj/ucontlib/cbbqueue.o: cbbqueue.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/cbbqueue.o -c cbbqueue.cpp

dbg/obj/ucontlib/timebuff.o: timebuff.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/timebuff.o -c timebuff.cpp

dbg/obj/ucontlib/hxguid.o: hxguid.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxguid.o -c hxguid.cpp

dbg/obj/ucontlib/statinfo.o: statinfo.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/statinfo.o -c statinfo.cpp

dbg/obj/ucontlib/hxstack.o: hxstack.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxstack.o -c hxstack.cpp

dbg/obj/ucontlib/hxvalues.o: hxvalues.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxvalues.o -c hxvalues.cpp

dbg/obj/ucontlib/hxordval.o: hxordval.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxordval.o -c hxordval.cpp

dbg/obj/ucontlib/chxmapptrtoptr.o: chxmapptrtoptr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxmapptrtoptr.o -c chxmapptrtoptr.cpp

dbg/obj/ucontlib/chxmapstringtoob.o: chxmapstringtoob.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxmapstringtoob.o -c chxmapstringtoob.cpp

dbg/obj/ucontlib/chxmaplongtoobj.o: chxmaplongtoobj.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxmaplongtoobj.o -c chxmaplongtoobj.cpp

dbg/obj/ucontlib/chxmapguidtoobj.o: chxmapguidtoobj.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxmapguidtoobj.o -c chxmapguidtoobj.cpp

dbg/obj/ucontlib/chxmapstringtostring.o: chxmapstringtostring.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/chxmapstringtostring.o -c chxmapstringtostring.cpp

dbg/obj/ucontlib/hxmaputils.o: hxmaputils.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/hxmaputils.o -c hxmaputils.cpp

dbg/obj/ucontlib/dict.o: dict.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ucontlib/dict.o -c dict.cpp

clean: 
	$(RM) dbg/contlib.a dbg/obj/ucontlib/carray.o dbg/obj/ucontlib/hxslist.o dbg/obj/ucontlib/chxpckts.o dbg/obj/ucontlib/hxbuffer.o dbg/obj/ucontlib/hxsbuffer.o dbg/obj/ucontlib/buffer.o dbg/obj/ucontlib/nestbuff.o dbg/obj/ucontlib/chxfgbuf.o dbg/obj/ucontlib/hxstring.o dbg/obj/ucontlib/hxstradv.o dbg/obj/ucontlib/hxstrfmt.o dbg/obj/ucontlib/cbqueue.o dbg/obj/ucontlib/cbbqueue.o dbg/obj/ucontlib/timebuff.o dbg/obj/ucontlib/hxguid.o dbg/obj/ucontlib/statinfo.o dbg/obj/ucontlib/hxstack.o dbg/obj/ucontlib/hxvalues.o dbg/obj/ucontlib/hxordval.o dbg/obj/ucontlib/chxmapptrtoptr.o dbg/obj/ucontlib/chxmapstringtoob.o dbg/obj/ucontlib/chxmaplongtoobj.o dbg/obj/ucontlib/chxmapguidtoobj.o dbg/obj/ucontlib/chxmapstringtostring.o dbg/obj/ucontlib/hxmaputils.o dbg/obj/ucontlib/dict.o

../../debug/contlib.a: dbg/contlib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/contlib.a"
	cp "dbg/contlib.a" "../../debug/contlib.a"

copy: ../../debug/contlib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/ucontlib -mucontlib.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: ucontlib.mak,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/ucontlib/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib

dbg/obj/ucontlib/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib

dbg/obj/ucontlib/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib

dbg/obj/ucontlib/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/ucontlib || mkdir dbg/obj/ucontlib

