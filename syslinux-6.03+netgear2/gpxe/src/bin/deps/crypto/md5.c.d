md5_DEPS = crypto/md5.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/crypto.h include/gpxe/md5.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/crypto.h:

include/gpxe/md5.h:

$(BIN)/md5.o : crypto/md5.c $(MAKEDEPS) $(POST_O_DEPS) $(md5_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/md5.o
 
$(BIN)/md5.dbg%.o : crypto/md5.c $(MAKEDEPS) $(POST_O_DEPS) $(md5_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/md5.dbg%.o
 
$(BIN)/md5.c : crypto/md5.c $(MAKEDEPS) $(POST_O_DEPS) $(md5_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/md5.c
 
$(BIN)/md5.s : crypto/md5.c $(MAKEDEPS) $(POST_O_DEPS) $(md5_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/md5.s
 
bin/deps/crypto/md5.c.d : $(md5_DEPS)
 
TAGS : $(md5_DEPS)

