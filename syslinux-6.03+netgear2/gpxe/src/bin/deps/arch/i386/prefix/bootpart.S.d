bootpart_DEPS = arch/i386/prefix/bootpart.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/bootpart.o : arch/i386/prefix/bootpart.S $(MAKEDEPS) $(POST_O_DEPS) $(bootpart_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/bootpart.o
 
$(BIN)/bootpart.s : arch/i386/prefix/bootpart.S $(MAKEDEPS) $(POST_O_DEPS) $(bootpart_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/bootpart.s
 
bin/deps/arch/i386/prefix/bootpart.S.d : $(bootpart_DEPS)
 
TAGS : $(bootpart_DEPS)

