gdbstub_DEPS = core/gdbstub.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/stdio.h \
 include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/ctype.h include/byteswap.h \
 include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/gdbstub.h include/gpxe/tables.h arch/i386/include/gdbmach.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/ctype.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/gdbstub.h:

include/gpxe/tables.h:

arch/i386/include/gdbmach.h:

$(BIN)/gdbstub.o : core/gdbstub.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/gdbstub.o
 
$(BIN)/gdbstub.dbg%.o : core/gdbstub.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/gdbstub.dbg%.o
 
$(BIN)/gdbstub.c : core/gdbstub.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/gdbstub.c
 
$(BIN)/gdbstub.s : core/gdbstub.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbstub_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/gdbstub.s
 
bin/deps/core/gdbstub.c.d : $(gdbstub_DEPS)
 
TAGS : $(gdbstub_DEPS)

