com32_wrapper_DEPS = arch/i386/interface/syslinux/com32_wrapper.S \
 include/compiler.h arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/com32_wrapper.o : arch/i386/interface/syslinux/com32_wrapper.S $(MAKEDEPS) $(POST_O_DEPS) $(com32_wrapper_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/com32_wrapper.o
 
$(BIN)/com32_wrapper.s : arch/i386/interface/syslinux/com32_wrapper.S $(MAKEDEPS) $(POST_O_DEPS) $(com32_wrapper_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/com32_wrapper.s
 
bin/deps/arch/i386/interface/syslinux/com32_wrapper.S.d : $(com32_wrapper_DEPS)
 
TAGS : $(com32_wrapper_DEPS)

