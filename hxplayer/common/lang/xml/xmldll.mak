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

SRCS=xmlccf.cpp hxexpat/expatprs.cpp hxexpat/expatapi.cpp ../../import/expat/xmlparse/xmlparse.c ../../import/expat/xmltok/xmltok.c ../../import/expat/xmltok/xmlrole.c

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/xmldll/xmlccf.o dbg/obj/xmldll/hxexpat/expatprs.o dbg/obj/xmldll/hxexpat/expatapi.o dbg/obj/xmldll/par2/import/expat/xmlparse/xmlparse.o dbg/obj/xmldll/par2/import/expat/xmltok/xmltok.o dbg/obj/xmldll/par2/import/expat/xmltok/xmlrole.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -Ihxexpat -I../../../common/include -I../../../common/lang/xml -I../../../common/dbgtool/pub -I../../../common/system/pub -I../../../common/import/expat/xmlparse -I../../../common/import/expat/xmltok -I./pub -I../../../common/util/pub -I../../../common/container/pub

DEFINES=-I. -include dbg/common_lang_xml_xmldll_ribodefs.h

STATIC_LIBS=../../../common/util/dbg/utillib.a ../../../common/container/dbg/contlib.a ../../../common/system/dbg/syslib.a ../../../common/runtime/dbg/runtlib.a ../../../common/dbgtool/dbg/debuglib.a

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/xmldll dbg/hxxml.so

dbg/obj/xmldll: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll

all_objects: $(OBJS)

dbg/hxxml.so: $(OBJS) $(STATIC_LIBS)
	-@test -d dbg || mkdir dbg
	$(LD) $(LDFLAGS) -o dbg/hxxml.so $(OBJS) -L/usr/X11R6/lib $(DYNAMIC_LIBS) $(STATIC_LIBS) -u RMACreateInstance -u CanUnload -u CanUnload2 -lgcc

dbg/obj/xmldll/xmlccf.o: xmlccf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmldll/xmlccf.o -c xmlccf.cpp

dbg/obj/xmldll/hxexpat/expatprs.o: hxexpat/expatprs.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll
	-@test -d dbg/obj/xmldll/hxexpat || mkdir dbg/obj/xmldll/hxexpat
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmldll/hxexpat/expatprs.o -c hxexpat/expatprs.cpp

dbg/obj/xmldll/hxexpat/expatapi.o: hxexpat/expatapi.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll
	-@test -d dbg/obj/xmldll/hxexpat || mkdir dbg/obj/xmldll/hxexpat
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmldll/hxexpat/expatapi.o -c hxexpat/expatapi.cpp

dbg/obj/xmldll/par2/import/expat/xmlparse/xmlparse.o: ../../import/expat/xmlparse/xmlparse.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll
	-@test -d dbg/obj/xmldll/par2 || mkdir dbg/obj/xmldll/par2
	-@test -d dbg/obj/xmldll/par2/import || mkdir dbg/obj/xmldll/par2/import
	-@test -d dbg/obj/xmldll/par2/import/expat || mkdir dbg/obj/xmldll/par2/import/expat
	-@test -d dbg/obj/xmldll/par2/import/expat/xmlparse || mkdir dbg/obj/xmldll/par2/import/expat/xmlparse
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/xmldll/par2/import/expat/xmlparse/xmlparse.o -c ../../import/expat/xmlparse/xmlparse.c

dbg/obj/xmldll/par2/import/expat/xmltok/xmltok.o: ../../import/expat/xmltok/xmltok.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll
	-@test -d dbg/obj/xmldll/par2 || mkdir dbg/obj/xmldll/par2
	-@test -d dbg/obj/xmldll/par2/import || mkdir dbg/obj/xmldll/par2/import
	-@test -d dbg/obj/xmldll/par2/import/expat || mkdir dbg/obj/xmldll/par2/import/expat
	-@test -d dbg/obj/xmldll/par2/import/expat/xmltok || mkdir dbg/obj/xmldll/par2/import/expat/xmltok
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/xmldll/par2/import/expat/xmltok/xmltok.o -c ../../import/expat/xmltok/xmltok.c

dbg/obj/xmldll/par2/import/expat/xmltok/xmlrole.o: ../../import/expat/xmltok/xmlrole.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll
	-@test -d dbg/obj/xmldll/par2 || mkdir dbg/obj/xmldll/par2
	-@test -d dbg/obj/xmldll/par2/import || mkdir dbg/obj/xmldll/par2/import
	-@test -d dbg/obj/xmldll/par2/import/expat || mkdir dbg/obj/xmldll/par2/import/expat
	-@test -d dbg/obj/xmldll/par2/import/expat/xmltok || mkdir dbg/obj/xmldll/par2/import/expat/xmltok
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/xmldll/par2/import/expat/xmltok/xmlrole.o -c ../../import/expat/xmltok/xmlrole.c

clean: 
	$(RM) dbg/hxxml.so dbg/obj/xmldll/xmlccf.o dbg/obj/xmldll/hxexpat/expatprs.o dbg/obj/xmldll/hxexpat/expatapi.o dbg/obj/xmldll/par2/import/expat/xmlparse/xmlparse.o dbg/obj/xmldll/par2/import/expat/xmltok/xmltok.o dbg/obj/xmldll/par2/import/expat/xmltok/xmlrole.o

../../../debug/hxxml.so: dbg/hxxml.so
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/hxxml.so"
	cp "dbg/hxxml.so" "../../../debug/hxxml.so"

copy: ../../../debug/hxxml.so

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/xmldll -mxmldll.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: xmldll.mak,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/xmldll/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll

dbg/obj/xmldll/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll

dbg/obj/xmldll/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll

dbg/obj/xmldll/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmldll || mkdir dbg/obj/xmldll

