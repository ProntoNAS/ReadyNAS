crc32_DEPS = crypto/crc32.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/crc32.h include/stdint.h \
 arch/i386/include/bits/stdint.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/crc32.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

$(BIN)/crc32.o : crypto/crc32.c $(MAKEDEPS) $(POST_O_DEPS) $(crc32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/crc32.o
 
$(BIN)/crc32.dbg%.o : crypto/crc32.c $(MAKEDEPS) $(POST_O_DEPS) $(crc32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/crc32.dbg%.o
 
$(BIN)/crc32.c : crypto/crc32.c $(MAKEDEPS) $(POST_O_DEPS) $(crc32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/crc32.c
 
$(BIN)/crc32.s : crypto/crc32.c $(MAKEDEPS) $(POST_O_DEPS) $(crc32_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/crc32.s
 
bin/deps/crypto/crc32.c.d : $(crc32_DEPS)
 
TAGS : $(crc32_DEPS)

