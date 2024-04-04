mbr_DEPS = arch/i386/prefix/mbr.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/bootpart.S

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/bootpart.S:

$(BIN)/mbr.o : arch/i386/prefix/mbr.S $(MAKEDEPS) $(POST_O_DEPS) $(mbr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/mbr.o
 
$(BIN)/mbr.s : arch/i386/prefix/mbr.S $(MAKEDEPS) $(POST_O_DEPS) $(mbr_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/mbr.s
 
bin/deps/arch/i386/prefix/mbr.S.d : $(mbr_DEPS)
 
TAGS : $(mbr_DEPS)

