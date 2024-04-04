undiloader_DEPS = arch/i386/prefix/undiloader.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/undiloader.o : arch/i386/prefix/undiloader.S $(MAKEDEPS) $(POST_O_DEPS) $(undiloader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/undiloader.o
 
$(BIN)/undiloader.s : arch/i386/prefix/undiloader.S $(MAKEDEPS) $(POST_O_DEPS) $(undiloader_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/undiloader.s
 
bin/deps/arch/i386/prefix/undiloader.S.d : $(undiloader_DEPS)
 
TAGS : $(undiloader_DEPS)

