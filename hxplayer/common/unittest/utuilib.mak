SRCROOT=../..
## Generated from utuilib, do not edit, do not commit to cvs!

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
INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I./pub
DEFINES=-I. -include dbg/common_unittest_utuilib_ribodefs.h
STATIC_LIBS=
DYNAMIC_LIBS= 

.SUFFIXES: .cpp .so

.c.o:
	$(CC) $(CCFLAGS) -o  $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -o  $@ -c $<

all:
depend:
clean:
dist:
copy:
