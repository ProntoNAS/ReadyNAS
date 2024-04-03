__STANDARD__=YES

SRCROOT=../../../../../..

RM=rm -f

RM_DIR=rm -rf

MAKE_DEP=

MAKE_DEP_FLAGS=$(INCLUDES) $(DEFINES)

MAKE_LIB=$(SRCROOT)/build/bin/pyar cr

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

SRCS=initguid.cpp aacdecdll.cpp aacdecdrm.cpp raacentrydrm.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/umake_dll/initguid.o dbg/obj/umake_dll/aacdecdll.o dbg/obj/umake_dll/aacdecdrm.o dbg/obj/umake_dll/raacentrydrm.o

SOURCE_OBJS=

INCLUDES=-I../../../../../../common/runtime/pub -I/usr/X11R6/include -I../pub -I../../../../../../common/include -I../../../../../../datatype/include -I../../../../../../datatype/rm/include -I../../../../../../common/dbgtool/pub -I../../../../../../datatype/mp4/common/pub -I../../../../../../common/util/pub -I./pub

DEFINES=-I. -include dbg/datatype_aac_codec_fixpt_decoder_hxwrap_umake_dll_ribodefs.h

STATIC_LIBS=../../../../../../common/runtime/dbg/runtlib.a ../../../../../../common/dbgtool/dbg/debuglib.a ../../../../../../common/util/dbg/utillib.a ../../../../../../datatype/mp4/common/dbg/mp4comlib.a ../../../../../../datatype/aac/codec/fixpt/decoder/hxwrap/dbg/aacdeclib.a

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/umake_dll dbg/raac.so

dbg/obj/umake_dll: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_dll || mkdir dbg/obj/umake_dll

all_objects: $(OBJS)

dbg/raac.so: $(OBJS) $(STATIC_LIBS)
	-@test -d dbg || mkdir dbg
	$(LD) $(LDFLAGS) -o dbg/raac.so $(OBJS) -L/usr/X11R6/lib $(DYNAMIC_LIBS) $(STATIC_LIBS) -u RACreateDecoderInstance -u RAOpenCodec2 -u RACloseCodec -u RAGetFlavorProperty -u RAInitDecoder -u RADecode -u RAFlush -u RAFreeDecoder -u RASetComMode -u RAGetGUID -u RAGetBackend -lgcc

dbg/obj/umake_dll/initguid.o: initguid.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_dll || mkdir dbg/obj/umake_dll
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/umake_dll/initguid.o -c initguid.cpp

dbg/obj/umake_dll/aacdecdll.o: aacdecdll.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_dll || mkdir dbg/obj/umake_dll
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/umake_dll/aacdecdll.o -c aacdecdll.cpp

dbg/obj/umake_dll/aacdecdrm.o: aacdecdrm.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_dll || mkdir dbg/obj/umake_dll
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/umake_dll/aacdecdrm.o -c aacdecdrm.cpp

dbg/obj/umake_dll/raacentrydrm.o: raacentrydrm.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/umake_dll || mkdir dbg/obj/umake_dll
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/umake_dll/raacentrydrm.o -c raacentrydrm.cpp

clean: 
	$(RM) dbg/raac.so dbg/obj/umake_dll/initguid.o dbg/obj/umake_dll/aacdecdll.o dbg/obj/umake_dll/aacdecdrm.o dbg/obj/umake_dll/raacentrydrm.o

../../../../../../debug/raac.so: dbg/raac.so
	-@test -d ../../../../../../debug || mkdir ../../../../../../debug
	rm -f "../../../../../../debug/raac.so"
	cp "dbg/raac.so" "../../../../../../debug/raac.so"

copy: ../../../../../../debug/raac.so

depend: 
	$(SRCROOT)/build/bin/mkdepend.bin --gnumake -tdbg/obj/umake_dll -mumake_dll.mak $(INCLUDES) $(SRCS)

