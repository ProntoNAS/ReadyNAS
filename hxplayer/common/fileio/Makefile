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

SRCS=bufdataf.cpp mmapmgr.cpp platform/unix/cunixf.cpp platform/unix/mmapdatf.cpp platform/unix/hxbufdataf.cpp platform/unix/unbufdataf.cpp platform/unix/unixdir.cpp platform/unix/hxdir.cpp platform/unix/unixff.cpp platform/unix/filespec.cpp platform/unix/filespecutils.cpp platform/unix/filespecutils_unix.cpp chunkres.cpp chxdfmem.cpp datffact.cpp hxdirlist.cpp xhxdir.cpp findfile.cpp hxxfile.cpp cihx2chxdataf.cpp cchx2ihxdataf.cpp fmemio.cpp sio.cpp fsio.cpp chxdataf.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/bufdataf.o dbg/obj/mmapmgr.o dbg/obj/platform/unix/cunixf.o dbg/obj/platform/unix/mmapdatf.o dbg/obj/platform/unix/hxbufdataf.o dbg/obj/platform/unix/unbufdataf.o dbg/obj/platform/unix/unixdir.o dbg/obj/platform/unix/hxdir.o dbg/obj/platform/unix/unixff.o dbg/obj/platform/unix/filespec.o dbg/obj/platform/unix/filespecutils.o dbg/obj/platform/unix/filespecutils_unix.o dbg/obj/chunkres.o dbg/obj/chxdfmem.o dbg/obj/datffact.o dbg/obj/hxdirlist.o dbg/obj/xhxdir.o dbg/obj/findfile.o dbg/obj/hxxfile.o dbg/obj/cihx2chxdataf.o dbg/obj/cchx2ihxdataf.o dbg/obj/fmemio.o dbg/obj/sio.o dbg/obj/fsio.o dbg/obj/chxdataf.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -Ipub/platform/unix -I. -I../../common/include -I../../common/container/pub -I../../common/util/pub -I../../common/dbgtool/pub -I../../common/netio/pub -I../../common/system/pub -I./pub

DEFINES=-I. -include dbg/common_fileio_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/fileiolib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/fileiolib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/fileiolib.a $(OBJS)
	$(RANLIB)   dbg/fileiolib.a

dbg/obj/bufdataf.o: bufdataf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/bufdataf.o -c bufdataf.cpp

dbg/obj/mmapmgr.o: mmapmgr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/mmapmgr.o -c mmapmgr.cpp

dbg/obj/platform/unix/cunixf.o: platform/unix/cunixf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/cunixf.o -c platform/unix/cunixf.cpp

dbg/obj/platform/unix/mmapdatf.o: platform/unix/mmapdatf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/mmapdatf.o -c platform/unix/mmapdatf.cpp

dbg/obj/platform/unix/hxbufdataf.o: platform/unix/hxbufdataf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/hxbufdataf.o -c platform/unix/hxbufdataf.cpp

dbg/obj/platform/unix/unbufdataf.o: platform/unix/unbufdataf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unbufdataf.o -c platform/unix/unbufdataf.cpp

dbg/obj/platform/unix/unixdir.o: platform/unix/unixdir.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unixdir.o -c platform/unix/unixdir.cpp

dbg/obj/platform/unix/hxdir.o: platform/unix/hxdir.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/hxdir.o -c platform/unix/hxdir.cpp

dbg/obj/platform/unix/unixff.o: platform/unix/unixff.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unixff.o -c platform/unix/unixff.cpp

dbg/obj/platform/unix/filespec.o: platform/unix/filespec.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/filespec.o -c platform/unix/filespec.cpp

dbg/obj/platform/unix/filespecutils.o: platform/unix/filespecutils.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/filespecutils.o -c platform/unix/filespecutils.cpp

dbg/obj/platform/unix/filespecutils_unix.o: platform/unix/filespecutils_unix.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/filespecutils_unix.o -c platform/unix/filespecutils_unix.cpp

dbg/obj/chunkres.o: chunkres.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chunkres.o -c chunkres.cpp

dbg/obj/chxdfmem.o: chxdfmem.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chxdfmem.o -c chxdfmem.cpp

dbg/obj/datffact.o: datffact.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/datffact.o -c datffact.cpp

dbg/obj/hxdirlist.o: hxdirlist.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxdirlist.o -c hxdirlist.cpp

dbg/obj/xhxdir.o: xhxdir.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xhxdir.o -c xhxdir.cpp

dbg/obj/findfile.o: findfile.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/findfile.o -c findfile.cpp

dbg/obj/hxxfile.o: hxxfile.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxxfile.o -c hxxfile.cpp

dbg/obj/cihx2chxdataf.o: cihx2chxdataf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cihx2chxdataf.o -c cihx2chxdataf.cpp

dbg/obj/cchx2ihxdataf.o: cchx2ihxdataf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cchx2ihxdataf.o -c cchx2ihxdataf.cpp

dbg/obj/fmemio.o: fmemio.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/fmemio.o -c fmemio.cpp

dbg/obj/sio.o: sio.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/sio.o -c sio.cpp

dbg/obj/fsio.o: fsio.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/fsio.o -c fsio.cpp

dbg/obj/chxdataf.o: chxdataf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chxdataf.o -c chxdataf.cpp

clean: 
	$(RM) dbg/fileiolib.a dbg/obj/bufdataf.o dbg/obj/mmapmgr.o dbg/obj/platform/unix/cunixf.o dbg/obj/platform/unix/mmapdatf.o dbg/obj/platform/unix/hxbufdataf.o dbg/obj/platform/unix/unbufdataf.o dbg/obj/platform/unix/unixdir.o dbg/obj/platform/unix/hxdir.o dbg/obj/platform/unix/unixff.o dbg/obj/platform/unix/filespec.o dbg/obj/platform/unix/filespecutils.o dbg/obj/platform/unix/filespecutils_unix.o dbg/obj/chunkres.o dbg/obj/chxdfmem.o dbg/obj/datffact.o dbg/obj/hxdirlist.o dbg/obj/xhxdir.o dbg/obj/findfile.o dbg/obj/hxxfile.o dbg/obj/cihx2chxdataf.o dbg/obj/cchx2ihxdataf.o dbg/obj/fmemio.o dbg/obj/sio.o dbg/obj/fsio.o dbg/obj/chxdataf.o

../../debug/fileiolib.a: dbg/fileiolib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/fileiolib.a"
	cp "dbg/fileiolib.a" "../../debug/fileiolib.a"

copy: ../../debug/fileiolib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:37 hagi Exp $

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

