cpio_DEPS = core/cpio.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/stdio.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/stdarg.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/gpxe/cpio.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdio.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/cpio.h:

$(BIN)/cpio.o : core/cpio.c $(MAKEDEPS) $(POST_O_DEPS) $(cpio_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/cpio.o
 
$(BIN)/cpio.dbg%.o : core/cpio.c $(MAKEDEPS) $(POST_O_DEPS) $(cpio_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/cpio.dbg%.o
 
$(BIN)/cpio.c : core/cpio.c $(MAKEDEPS) $(POST_O_DEPS) $(cpio_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/cpio.c
 
$(BIN)/cpio.s : core/cpio.c $(MAKEDEPS) $(POST_O_DEPS) $(cpio_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/cpio.s
 
bin/deps/core/cpio.c.d : $(cpio_DEPS)
 
TAGS : $(cpio_DEPS)

