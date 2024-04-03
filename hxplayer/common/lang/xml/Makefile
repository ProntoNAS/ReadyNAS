__STANDARD__=YES
SRCROOT=../../..
## Generated from Umakefil, do not edit, do not commit to cvs!

RM=rm -f
RM_DIR=rm -rf
MAKE_DEP=
MAKE_DEP_FLAGS =   $(INCLUDES) $(DEFINES)
MAKE_LIB=/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr 
MAKE_LIB_FLAGS= 
RANLIB=ranlib
CP=cp
MAKE=make
CC=gcc
CCFLAGS = --permissive -pipe -fsigned-char -O0 -g -DDEBUG -D_DEBUG  $(INCLUDES) $(DEFINES)
CXX=g++
CXXFLAGS = --permissive -pipe -fsigned-char -O0 -g -DDEBUG -D_DEBUG  $(INCLUDES) $(DEFINES)
LD=g++
LDFLAGS= 
SRCS=
OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)
COMPILED_OBJS=
SOURCE_OBJS=
INCLUDES=-I../../../common/runtime/pub -I/usr/X11R6/include -I./pub
DEFINES=-I. -include dbg/common_lang_xml_ribodefs.h
STATIC_LIBS=
DYNAMIC_LIBS= 

.SUFFIXES: .cpp .so

.c.o:
	$(CC) $(CCFLAGS) -o  $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -o  $@ -c $<

all: dbg/obj 
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmllib.mak
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmldll.mak

dbg/obj:
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmllib.mak all_objects
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmldll.mak all_objects

depend:
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmllib.mak depend
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmldll.mak depend

copy: 
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmllib.mak copy
	$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmldll.mak copy

clean:
	-$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmllib.mak clean
	-$(MAKE) $(SUBMAKEFLAGS) SUBMAKEFLAGS="$(SUBMAKEFLAGS)" -f xmldll.mak clean

