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

SRCS=looseprs.cpp hxxmlprs.cpp tagparse.cpp xmlencod.cpp xmlvalid.cpp xmlesc.cpp rpsmplxml.cpp xmlwrite.cpp xmlconfig.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/xmllib/looseprs.o dbg/obj/xmllib/hxxmlprs.o dbg/obj/xmllib/tagparse.o dbg/obj/xmllib/xmlencod.o dbg/obj/xmllib/xmlvalid.o dbg/obj/xmllib/xmlesc.o dbg/obj/xmllib/rpsmplxml.o dbg/obj/xmllib/xmlwrite.o dbg/obj/xmllib/xmlconfig.o

SOURCE_OBJS=

INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I../../../common/include -I../../../common/util/pub -I../../../common/container/pub -I../../../common/dbgtool/pub -I../../../common/fileio/pub -I../../../common/system/pub -I./pub

DEFINES=-I. -include dbg/common_lang_xml_xmllib_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/xmllib dbg/xmllib.a

dbg/obj/xmllib: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib

all_objects: $(OBJS)

dbg/xmllib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/xmllib.a $(OBJS)
	$(RANLIB)   dbg/xmllib.a

dbg/obj/xmllib/looseprs.o: looseprs.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/looseprs.o -c looseprs.cpp

dbg/obj/xmllib/hxxmlprs.o: hxxmlprs.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/hxxmlprs.o -c hxxmlprs.cpp

dbg/obj/xmllib/tagparse.o: tagparse.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/tagparse.o -c tagparse.cpp

dbg/obj/xmllib/xmlencod.o: xmlencod.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/xmlencod.o -c xmlencod.cpp

dbg/obj/xmllib/xmlvalid.o: xmlvalid.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/xmlvalid.o -c xmlvalid.cpp

dbg/obj/xmllib/xmlesc.o: xmlesc.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/xmlesc.o -c xmlesc.cpp

dbg/obj/xmllib/rpsmplxml.o: rpsmplxml.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/rpsmplxml.o -c rpsmplxml.cpp

dbg/obj/xmllib/xmlwrite.o: xmlwrite.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/xmlwrite.o -c xmlwrite.cpp

dbg/obj/xmllib/xmlconfig.o: xmlconfig.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/xmllib/xmlconfig.o -c xmlconfig.cpp

clean: 
	$(RM) dbg/xmllib.a dbg/obj/xmllib/looseprs.o dbg/obj/xmllib/hxxmlprs.o dbg/obj/xmllib/tagparse.o dbg/obj/xmllib/xmlencod.o dbg/obj/xmllib/xmlvalid.o dbg/obj/xmllib/xmlesc.o dbg/obj/xmllib/rpsmplxml.o dbg/obj/xmllib/xmlwrite.o dbg/obj/xmllib/xmlconfig.o

../../../debug/xmllib.a: dbg/xmllib.a
	-@test -d ../../../debug || mkdir ../../../debug
	rm -f "../../../debug/xmllib.a"
	cp "dbg/xmllib.a" "../../../debug/xmllib.a"

copy: ../../../debug/xmllib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/xmllib -mxmllib.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: xmllib.mak,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/xmllib/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib

dbg/obj/xmllib/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib

dbg/obj/xmllib/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib

dbg/obj/xmllib/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/xmllib || mkdir dbg/obj/xmllib

