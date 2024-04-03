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

SRCS=hx_unit_test.cpp hx_cmd_parser.cpp hx_cmd_based_test.cpp hx_ut_cmd_info.cpp hx_next_line.cpp hx_char_stack.cpp ut_string.cpp ut_param_util.cpp ut_cmd_line.cpp hx_simple_test_driver.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/unittest/hx_unit_test.o dbg/obj/unittest/hx_cmd_parser.o dbg/obj/unittest/hx_cmd_based_test.o dbg/obj/unittest/hx_ut_cmd_info.o dbg/obj/unittest/hx_next_line.o dbg/obj/unittest/hx_char_stack.o dbg/obj/unittest/ut_string.o dbg/obj/unittest/ut_param_util.o dbg/obj/unittest/ut_cmd_line.o dbg/obj/unittest/hx_simple_test_driver.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -I./pub

DEFINES=-I. -include dbg/common_unittest_unittest_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj/unittest dbg/unittest.a

dbg/obj/unittest: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest

all_objects: $(OBJS)

dbg/unittest.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/unittest.a $(OBJS)
	$(RANLIB)   dbg/unittest.a

dbg/obj/unittest/hx_unit_test.o: hx_unit_test.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_unit_test.o -c hx_unit_test.cpp

dbg/obj/unittest/hx_cmd_parser.o: hx_cmd_parser.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_cmd_parser.o -c hx_cmd_parser.cpp

dbg/obj/unittest/hx_cmd_based_test.o: hx_cmd_based_test.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_cmd_based_test.o -c hx_cmd_based_test.cpp

dbg/obj/unittest/hx_ut_cmd_info.o: hx_ut_cmd_info.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_ut_cmd_info.o -c hx_ut_cmd_info.cpp

dbg/obj/unittest/hx_next_line.o: hx_next_line.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_next_line.o -c hx_next_line.cpp

dbg/obj/unittest/hx_char_stack.o: hx_char_stack.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_char_stack.o -c hx_char_stack.cpp

dbg/obj/unittest/ut_string.o: ut_string.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/ut_string.o -c ut_string.cpp

dbg/obj/unittest/ut_param_util.o: ut_param_util.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/ut_param_util.o -c ut_param_util.cpp

dbg/obj/unittest/ut_cmd_line.o: ut_cmd_line.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/ut_cmd_line.o -c ut_cmd_line.cpp

dbg/obj/unittest/hx_simple_test_driver.o: hx_simple_test_driver.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/unittest || mkdir dbg/obj/unittest
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/unittest/hx_simple_test_driver.o -c hx_simple_test_driver.cpp

clean: 
	$(RM) dbg/unittest.a dbg/obj/unittest/hx_unit_test.o dbg/obj/unittest/hx_cmd_parser.o dbg/obj/unittest/hx_cmd_based_test.o dbg/obj/unittest/hx_ut_cmd_info.o dbg/obj/unittest/hx_next_line.o dbg/obj/unittest/hx_char_stack.o dbg/obj/unittest/ut_string.o dbg/obj/unittest/ut_param_util.o dbg/obj/unittest/ut_cmd_line.o dbg/obj/unittest/hx_simple_test_driver.o

../../debug/unittest.a: dbg/unittest.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/unittest.a"
	cp "dbg/unittest.a" "../../debug/unittest.a"

copy: ../../debug/unittest.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj/unittest -munittest.mak $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: unittest.mak,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $

ifneq ($MAKECMDGOALS),clean)
-include $(COMPILED_OBJS:.o=.dep)
endif

dbg/obj/unittest/%.dep: %.c
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/unittest || mkdir dbg/obj/unittest

dbg/obj/unittest/%.dep: %.cc
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/unittest || mkdir dbg/obj/unittest

dbg/obj/unittest/%.dep: %.cxx
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/unittest || mkdir dbg/obj/unittest

dbg/obj/unittest/%.dep: %.cpp
	test -d dbg || mkdir dbg
	test -d dbg/obj || mkdir dbg/obj
	test -d dbg/obj/unittest || mkdir dbg/obj/unittest

