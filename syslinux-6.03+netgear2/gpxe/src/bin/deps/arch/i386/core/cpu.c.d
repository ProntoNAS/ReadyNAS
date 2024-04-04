cpu_DEPS = arch/i386/core/cpu.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/string.h include/stddef.h \
 arch/x86/include/bits/string.h include/cpu.h \
 arch/i386/include/bits/cpu.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/cpu.h:

arch/i386/include/bits/cpu.h:

$(BIN)/cpu.o : arch/i386/core/cpu.c $(MAKEDEPS) $(POST_O_DEPS) $(cpu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/cpu.o
 
$(BIN)/cpu.dbg%.o : arch/i386/core/cpu.c $(MAKEDEPS) $(POST_O_DEPS) $(cpu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/cpu.dbg%.o
 
$(BIN)/cpu.c : arch/i386/core/cpu.c $(MAKEDEPS) $(POST_O_DEPS) $(cpu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/cpu.c
 
$(BIN)/cpu.s : arch/i386/core/cpu.c $(MAKEDEPS) $(POST_O_DEPS) $(cpu_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/cpu.s
 
bin/deps/arch/i386/core/cpu.c.d : $(cpu_DEPS)
 
TAGS : $(cpu_DEPS)

