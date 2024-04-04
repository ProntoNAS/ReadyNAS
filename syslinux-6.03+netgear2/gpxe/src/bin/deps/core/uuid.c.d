uuid_DEPS = core/uuid.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdio.h \
 include/stdarg.h include/byteswap.h include/endian.h \
 arch/i386/include/bits/endian.h arch/i386/include/bits/byteswap.h \
 include/little_bswap.h include/gpxe/uuid.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/uuid.h:

$(BIN)/uuid.o : core/uuid.c $(MAKEDEPS) $(POST_O_DEPS) $(uuid_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/uuid.o
 
$(BIN)/uuid.dbg%.o : core/uuid.c $(MAKEDEPS) $(POST_O_DEPS) $(uuid_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/uuid.dbg%.o
 
$(BIN)/uuid.c : core/uuid.c $(MAKEDEPS) $(POST_O_DEPS) $(uuid_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/uuid.c
 
$(BIN)/uuid.s : core/uuid.c $(MAKEDEPS) $(POST_O_DEPS) $(uuid_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/uuid.s
 
bin/deps/core/uuid.c.d : $(uuid_DEPS)
 
TAGS : $(uuid_DEPS)

