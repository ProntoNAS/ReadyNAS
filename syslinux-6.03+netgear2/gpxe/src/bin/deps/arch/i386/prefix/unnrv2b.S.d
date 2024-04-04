unnrv2b_DEPS = arch/i386/prefix/unnrv2b.S include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/unnrv2b.o : arch/i386/prefix/unnrv2b.S $(MAKEDEPS) $(POST_O_DEPS) $(unnrv2b_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/unnrv2b.o
 
$(BIN)/unnrv2b.s : arch/i386/prefix/unnrv2b.S $(MAKEDEPS) $(POST_O_DEPS) $(unnrv2b_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/unnrv2b.s
 
bin/deps/arch/i386/prefix/unnrv2b.S.d : $(unnrv2b_DEPS)
 
TAGS : $(unnrv2b_DEPS)

