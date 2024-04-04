isa_ids_DEPS = drivers/bus/isa_ids.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/isa_ids.h

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

include/gpxe/isa_ids.h:

$(BIN)/isa_ids.o : drivers/bus/isa_ids.c $(MAKEDEPS) $(POST_O_DEPS) $(isa_ids_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/isa_ids.o
 
$(BIN)/isa_ids.dbg%.o : drivers/bus/isa_ids.c $(MAKEDEPS) $(POST_O_DEPS) $(isa_ids_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/isa_ids.dbg%.o
 
$(BIN)/isa_ids.c : drivers/bus/isa_ids.c $(MAKEDEPS) $(POST_O_DEPS) $(isa_ids_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/isa_ids.c
 
$(BIN)/isa_ids.s : drivers/bus/isa_ids.c $(MAKEDEPS) $(POST_O_DEPS) $(isa_ids_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/isa_ids.s
 
bin/deps/drivers/bus/isa_ids.c.d : $(isa_ids_DEPS)
 
TAGS : $(isa_ids_DEPS)

