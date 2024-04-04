pxe_entry_DEPS = arch/i386/interface/pxe/pxe_entry.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/pxe_entry.o : arch/i386/interface/pxe/pxe_entry.S $(MAKEDEPS) $(POST_O_DEPS) $(pxe_entry_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/pxe_entry.o
 
$(BIN)/pxe_entry.s : arch/i386/interface/pxe/pxe_entry.S $(MAKEDEPS) $(POST_O_DEPS) $(pxe_entry_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/pxe_entry.s
 
bin/deps/arch/i386/interface/pxe/pxe_entry.S.d : $(pxe_entry_DEPS)
 
TAGS : $(pxe_entry_DEPS)

