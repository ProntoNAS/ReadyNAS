basename_DEPS = core/basename.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/libgen.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/libgen.h:

$(BIN)/basename.o : core/basename.c $(MAKEDEPS) $(POST_O_DEPS) $(basename_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/basename.o
 
$(BIN)/basename.dbg%.o : core/basename.c $(MAKEDEPS) $(POST_O_DEPS) $(basename_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/basename.dbg%.o
 
$(BIN)/basename.c : core/basename.c $(MAKEDEPS) $(POST_O_DEPS) $(basename_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/basename.c
 
$(BIN)/basename.s : core/basename.c $(MAKEDEPS) $(POST_O_DEPS) $(basename_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/basename.s
 
bin/deps/core/basename.c.d : $(basename_DEPS)
 
TAGS : $(basename_DEPS)

