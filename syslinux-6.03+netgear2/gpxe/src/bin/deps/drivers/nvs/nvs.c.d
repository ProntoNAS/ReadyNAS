nvs_DEPS = drivers/nvs/nvs.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/assert.h include/gpxe/nvs.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/assert.h:

include/gpxe/nvs.h:

$(BIN)/nvs.o : drivers/nvs/nvs.c $(MAKEDEPS) $(POST_O_DEPS) $(nvs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/nvs.o
 
$(BIN)/nvs.dbg%.o : drivers/nvs/nvs.c $(MAKEDEPS) $(POST_O_DEPS) $(nvs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/nvs.dbg%.o
 
$(BIN)/nvs.c : drivers/nvs/nvs.c $(MAKEDEPS) $(POST_O_DEPS) $(nvs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/nvs.c
 
$(BIN)/nvs.s : drivers/nvs/nvs.c $(MAKEDEPS) $(POST_O_DEPS) $(nvs_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/nvs.s
 
bin/deps/drivers/nvs/nvs.c.d : $(nvs_DEPS)
 
TAGS : $(nvs_DEPS)

