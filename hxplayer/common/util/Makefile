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

SRCS=platform/unix/unix_hurl.cpp platform/unix/unix_library.cpp platform/unix/unix_misc.cpp platform/unix/unix_pref.cpp platform/unix/cunixprefutils.cpp platform/unix/uproc.cpp hxstrutl.cpp hxdefpackethookhlp.cpp dbcs.cpp hxrand.cpp netbyte.cpp pacutil.cpp portaddr.cpp activewrap.cpp activopt.cpp asmrulep.cpp baseobj.cpp cachobj.cpp commreg.cpp cookhlpr.cpp cookies.cpp cpacemkr.cpp chxuuid.cpp db_dict.cpp key.cpp md5.c md5hl.c mimechk.cpp miscsp.cpp mmx_util.c cpuident.c nptime.cpp ntptime.cpp pathutil.cpp perplex.cpp pckunpck.cpp hxmangle.cpp hxurl.cpp hxwinver.cpp pref.cpp property.cpp random32.c readpath.cpp rlstate.cpp hxathsp.cpp hxcomsp.cpp hxcorsp.cpp hxdbsp.cpp hxengsp.cpp hxfilsp.cpp hxmonsp.cpp hxpktsp.cpp hxplgsp.cpp hxplnsp.cpp hxinfcod.cpp smpte.cpp timerep.cpp tparse.c verutil.cpp vsrcinfo.cpp watchlst.cpp localrep.c rtsputil.cpp crdcache.cpp char_stack.cpp hxparse.cpp buffutil.cpp addupcol.cpp chxkeepalive.cpp chxminiccf.cpp chxcharstack.cpp packedvalues.cpp hxprefutil.cpp stream_desc_hlpr.cpp hxcbobj.cpp clone_values.cpp HXErrorCodeStrings.c tconverter/fxpoint/tconverter_fxp.cpp

OBJS=$(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS=dbg/obj/platform/unix/unix_hurl.o dbg/obj/platform/unix/unix_library.o dbg/obj/platform/unix/unix_misc.o dbg/obj/platform/unix/unix_pref.o dbg/obj/platform/unix/cunixprefutils.o dbg/obj/platform/unix/uproc.o dbg/obj/hxstrutl.o dbg/obj/hxdefpackethookhlp.o dbg/obj/dbcs.o dbg/obj/hxrand.o dbg/obj/netbyte.o dbg/obj/pacutil.o dbg/obj/portaddr.o dbg/obj/activewrap.o dbg/obj/activopt.o dbg/obj/asmrulep.o dbg/obj/baseobj.o dbg/obj/cachobj.o dbg/obj/commreg.o dbg/obj/cookhlpr.o dbg/obj/cookies.o dbg/obj/cpacemkr.o dbg/obj/chxuuid.o dbg/obj/db_dict.o dbg/obj/key.o dbg/obj/md5.o dbg/obj/md5hl.o dbg/obj/mimechk.o dbg/obj/miscsp.o dbg/obj/mmx_util.o dbg/obj/cpuident.o dbg/obj/nptime.o dbg/obj/ntptime.o dbg/obj/pathutil.o dbg/obj/perplex.o dbg/obj/pckunpck.o dbg/obj/hxmangle.o dbg/obj/hxurl.o dbg/obj/hxwinver.o dbg/obj/pref.o dbg/obj/property.o dbg/obj/random32.o dbg/obj/readpath.o dbg/obj/rlstate.o dbg/obj/hxathsp.o dbg/obj/hxcomsp.o dbg/obj/hxcorsp.o dbg/obj/hxdbsp.o dbg/obj/hxengsp.o dbg/obj/hxfilsp.o dbg/obj/hxmonsp.o dbg/obj/hxpktsp.o dbg/obj/hxplgsp.o dbg/obj/hxplnsp.o dbg/obj/hxinfcod.o dbg/obj/smpte.o dbg/obj/timerep.o dbg/obj/tparse.o dbg/obj/verutil.o dbg/obj/vsrcinfo.o dbg/obj/watchlst.o dbg/obj/localrep.o dbg/obj/rtsputil.o dbg/obj/crdcache.o dbg/obj/char_stack.o dbg/obj/hxparse.o dbg/obj/buffutil.o dbg/obj/addupcol.o dbg/obj/chxkeepalive.o dbg/obj/chxminiccf.o dbg/obj/chxcharstack.o dbg/obj/packedvalues.o dbg/obj/hxprefutil.o dbg/obj/stream_desc_hlpr.o dbg/obj/hxcbobj.o dbg/obj/clone_values.o dbg/obj/HXErrorCodeStrings.o dbg/obj/tconverter/fxpoint/tconverter_fxp.o

SOURCE_OBJS=

INCLUDES=-I../../common/runtime/pub -I/usr/X11R6/include -Ipub/platform/unix -I../include -I../container/pub -I../dbgtool/pub -I../system/pub -I../fileio/pub -I../runtime/pub -I./pub

DEFINES=-I. -include dbg/common_util_ribodefs.h

STATIC_LIBS=

DYNAMIC_LIBS=

.SUFFIXES: .cpp .so

.c.o: 
	$(CC) $(CCFLAGS) -fPIC -DPIC -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

all: dbg/obj dbg/utillib.a

dbg/obj: 
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj

all_objects: $(OBJS)

dbg/utillib.a: $(OBJS)
	-@test -d dbg || mkdir dbg
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/pyar cr   dbg/utillib.a $(OBJS)
	$(RANLIB)   dbg/utillib.a

dbg/obj/platform/unix/unix_hurl.o: platform/unix/unix_hurl.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_hurl.o -c platform/unix/unix_hurl.cpp

dbg/obj/platform/unix/unix_library.o: platform/unix/unix_library.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_library.o -c platform/unix/unix_library.cpp

dbg/obj/platform/unix/unix_misc.o: platform/unix/unix_misc.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_misc.o -c platform/unix/unix_misc.cpp

dbg/obj/platform/unix/unix_pref.o: platform/unix/unix_pref.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/unix_pref.o -c platform/unix/unix_pref.cpp

dbg/obj/platform/unix/cunixprefutils.o: platform/unix/cunixprefutils.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/cunixprefutils.o -c platform/unix/cunixprefutils.cpp

dbg/obj/platform/unix/uproc.o: platform/unix/uproc.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/platform || mkdir dbg/obj/platform
	-@test -d dbg/obj/platform/unix || mkdir dbg/obj/platform/unix
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/platform/unix/uproc.o -c platform/unix/uproc.cpp

dbg/obj/hxstrutl.o: hxstrutl.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxstrutl.o -c hxstrutl.cpp

dbg/obj/hxdefpackethookhlp.o: hxdefpackethookhlp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxdefpackethookhlp.o -c hxdefpackethookhlp.cpp

dbg/obj/dbcs.o: dbcs.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/dbcs.o -c dbcs.cpp

dbg/obj/hxrand.o: hxrand.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxrand.o -c hxrand.cpp

dbg/obj/netbyte.o: netbyte.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/netbyte.o -c netbyte.cpp

dbg/obj/pacutil.o: pacutil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pacutil.o -c pacutil.cpp

dbg/obj/portaddr.o: portaddr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/portaddr.o -c portaddr.cpp

dbg/obj/activewrap.o: activewrap.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/activewrap.o -c activewrap.cpp

dbg/obj/activopt.o: activopt.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/activopt.o -c activopt.cpp

dbg/obj/asmrulep.o: asmrulep.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/asmrulep.o -c asmrulep.cpp

dbg/obj/baseobj.o: baseobj.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/baseobj.o -c baseobj.cpp

dbg/obj/cachobj.o: cachobj.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cachobj.o -c cachobj.cpp

dbg/obj/commreg.o: commreg.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/commreg.o -c commreg.cpp

dbg/obj/cookhlpr.o: cookhlpr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cookhlpr.o -c cookhlpr.cpp

dbg/obj/cookies.o: cookies.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cookies.o -c cookies.cpp

dbg/obj/cpacemkr.o: cpacemkr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/cpacemkr.o -c cpacemkr.cpp

dbg/obj/chxuuid.o: chxuuid.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chxuuid.o -c chxuuid.cpp

dbg/obj/db_dict.o: db_dict.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/db_dict.o -c db_dict.cpp

dbg/obj/key.o: key.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/key.o -c key.cpp

dbg/obj/md5.o: md5.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/md5.o -c md5.c

dbg/obj/md5hl.o: md5hl.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/md5hl.o -c md5hl.c

dbg/obj/mimechk.o: mimechk.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/mimechk.o -c mimechk.cpp

dbg/obj/miscsp.o: miscsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/miscsp.o -c miscsp.cpp

dbg/obj/mmx_util.o: mmx_util.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/mmx_util.o -c mmx_util.c

dbg/obj/cpuident.o: cpuident.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/cpuident.o -c cpuident.c

dbg/obj/nptime.o: nptime.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/nptime.o -c nptime.cpp

dbg/obj/ntptime.o: ntptime.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/ntptime.o -c ntptime.cpp

dbg/obj/pathutil.o: pathutil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pathutil.o -c pathutil.cpp

dbg/obj/perplex.o: perplex.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/perplex.o -c perplex.cpp

dbg/obj/pckunpck.o: pckunpck.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pckunpck.o -c pckunpck.cpp

dbg/obj/hxmangle.o: hxmangle.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxmangle.o -c hxmangle.cpp

dbg/obj/hxurl.o: hxurl.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxurl.o -c hxurl.cpp

dbg/obj/hxwinver.o: hxwinver.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxwinver.o -c hxwinver.cpp

dbg/obj/pref.o: pref.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/pref.o -c pref.cpp

dbg/obj/property.o: property.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/property.o -c property.cpp

dbg/obj/random32.o: random32.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/random32.o -c random32.c

dbg/obj/readpath.o: readpath.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/readpath.o -c readpath.cpp

dbg/obj/rlstate.o: rlstate.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/rlstate.o -c rlstate.cpp

dbg/obj/hxathsp.o: hxathsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxathsp.o -c hxathsp.cpp

dbg/obj/hxcomsp.o: hxcomsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxcomsp.o -c hxcomsp.cpp

dbg/obj/hxcorsp.o: hxcorsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxcorsp.o -c hxcorsp.cpp

dbg/obj/hxdbsp.o: hxdbsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxdbsp.o -c hxdbsp.cpp

dbg/obj/hxengsp.o: hxengsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxengsp.o -c hxengsp.cpp

dbg/obj/hxfilsp.o: hxfilsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxfilsp.o -c hxfilsp.cpp

dbg/obj/hxmonsp.o: hxmonsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxmonsp.o -c hxmonsp.cpp

dbg/obj/hxpktsp.o: hxpktsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxpktsp.o -c hxpktsp.cpp

dbg/obj/hxplgsp.o: hxplgsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxplgsp.o -c hxplgsp.cpp

dbg/obj/hxplnsp.o: hxplnsp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxplnsp.o -c hxplnsp.cpp

dbg/obj/hxinfcod.o: hxinfcod.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxinfcod.o -c hxinfcod.cpp

dbg/obj/smpte.o: smpte.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/smpte.o -c smpte.cpp

dbg/obj/timerep.o: timerep.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/timerep.o -c timerep.cpp

dbg/obj/tparse.o: tparse.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/tparse.o -c tparse.c

dbg/obj/verutil.o: verutil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/verutil.o -c verutil.cpp

dbg/obj/vsrcinfo.o: vsrcinfo.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/vsrcinfo.o -c vsrcinfo.cpp

dbg/obj/watchlst.o: watchlst.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/watchlst.o -c watchlst.cpp

dbg/obj/localrep.o: localrep.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/localrep.o -c localrep.c

dbg/obj/rtsputil.o: rtsputil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/rtsputil.o -c rtsputil.cpp

dbg/obj/crdcache.o: crdcache.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/crdcache.o -c crdcache.cpp

dbg/obj/char_stack.o: char_stack.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/char_stack.o -c char_stack.cpp

dbg/obj/hxparse.o: hxparse.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxparse.o -c hxparse.cpp

dbg/obj/buffutil.o: buffutil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/buffutil.o -c buffutil.cpp

dbg/obj/addupcol.o: addupcol.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/addupcol.o -c addupcol.cpp

dbg/obj/chxkeepalive.o: chxkeepalive.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chxkeepalive.o -c chxkeepalive.cpp

dbg/obj/chxminiccf.o: chxminiccf.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chxminiccf.o -c chxminiccf.cpp

dbg/obj/chxcharstack.o: chxcharstack.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/chxcharstack.o -c chxcharstack.cpp

dbg/obj/packedvalues.o: packedvalues.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/packedvalues.o -c packedvalues.cpp

dbg/obj/hxprefutil.o: hxprefutil.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxprefutil.o -c hxprefutil.cpp

dbg/obj/stream_desc_hlpr.o: stream_desc_hlpr.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/stream_desc_hlpr.o -c stream_desc_hlpr.cpp

dbg/obj/hxcbobj.o: hxcbobj.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/hxcbobj.o -c hxcbobj.cpp

dbg/obj/clone_values.o: clone_values.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/clone_values.o -c clone_values.cpp

dbg/obj/HXErrorCodeStrings.o: HXErrorCodeStrings.c
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	$(CC) $(CCFLAGS) -fPIC -DPIC -o dbg/obj/HXErrorCodeStrings.o -c HXErrorCodeStrings.c

dbg/obj/tconverter/fxpoint/tconverter_fxp.o: tconverter/fxpoint/tconverter_fxp.cpp
	-@test -d dbg || mkdir dbg
	-@test -d dbg/obj || mkdir dbg/obj
	-@test -d dbg/obj/tconverter || mkdir dbg/obj/tconverter
	-@test -d dbg/obj/tconverter/fxpoint || mkdir dbg/obj/tconverter/fxpoint
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o dbg/obj/tconverter/fxpoint/tconverter_fxp.o -c tconverter/fxpoint/tconverter_fxp.cpp

clean: 
	$(RM) dbg/utillib.a dbg/obj/platform/unix/unix_hurl.o dbg/obj/platform/unix/unix_library.o dbg/obj/platform/unix/unix_misc.o dbg/obj/platform/unix/unix_pref.o dbg/obj/platform/unix/cunixprefutils.o dbg/obj/platform/unix/uproc.o dbg/obj/hxstrutl.o dbg/obj/hxdefpackethookhlp.o dbg/obj/dbcs.o dbg/obj/hxrand.o dbg/obj/netbyte.o dbg/obj/pacutil.o dbg/obj/portaddr.o dbg/obj/activewrap.o dbg/obj/activopt.o dbg/obj/asmrulep.o dbg/obj/baseobj.o dbg/obj/cachobj.o dbg/obj/commreg.o dbg/obj/cookhlpr.o dbg/obj/cookies.o dbg/obj/cpacemkr.o dbg/obj/chxuuid.o dbg/obj/db_dict.o dbg/obj/key.o dbg/obj/md5.o dbg/obj/md5hl.o dbg/obj/mimechk.o dbg/obj/miscsp.o dbg/obj/mmx_util.o dbg/obj/cpuident.o dbg/obj/nptime.o dbg/obj/ntptime.o dbg/obj/pathutil.o dbg/obj/perplex.o dbg/obj/pckunpck.o dbg/obj/hxmangle.o dbg/obj/hxurl.o dbg/obj/hxwinver.o dbg/obj/pref.o dbg/obj/property.o dbg/obj/random32.o dbg/obj/readpath.o dbg/obj/rlstate.o dbg/obj/hxathsp.o dbg/obj/hxcomsp.o dbg/obj/hxcorsp.o dbg/obj/hxdbsp.o dbg/obj/hxengsp.o dbg/obj/hxfilsp.o dbg/obj/hxmonsp.o dbg/obj/hxpktsp.o dbg/obj/hxplgsp.o dbg/obj/hxplnsp.o dbg/obj/hxinfcod.o dbg/obj/smpte.o dbg/obj/timerep.o dbg/obj/tparse.o dbg/obj/verutil.o dbg/obj/vsrcinfo.o dbg/obj/watchlst.o dbg/obj/localrep.o dbg/obj/rtsputil.o dbg/obj/crdcache.o dbg/obj/char_stack.o dbg/obj/hxparse.o dbg/obj/buffutil.o dbg/obj/addupcol.o dbg/obj/chxkeepalive.o dbg/obj/chxminiccf.o dbg/obj/chxcharstack.o dbg/obj/packedvalues.o dbg/obj/hxprefutil.o dbg/obj/stream_desc_hlpr.o dbg/obj/hxcbobj.o dbg/obj/clone_values.o dbg/obj/HXErrorCodeStrings.o dbg/obj/tconverter/fxpoint/tconverter_fxp.o

../../debug/utillib.a: dbg/utillib.a
	-@test -d ../../debug || mkdir ../../debug
	rm -f "../../debug/utillib.a"
	cp "dbg/utillib.a" "../../debug/utillib.a"

copy: ../../debug/utillib.a

depend: 
	/usr/src/TEST/jason/iTunes/hxplayer/build/bin/mkdepend.bin --gnumake -tdbg/obj -mMakefile $(INCLUDES) $(SRCS)


# DO NOT DELETE -- mkdepend depends on this line
# Dependency magic by $Id: Makefile,v 1.1.1.1 2006/03/29 16:45:31 hagi Exp $

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

