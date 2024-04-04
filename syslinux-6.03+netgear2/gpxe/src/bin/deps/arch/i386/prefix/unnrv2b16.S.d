unnrv2b16_DEPS = arch/i386/prefix/unnrv2b16.S include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/prefix/unnrv2b.S

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/prefix/unnrv2b.S:

$(BIN)/unnrv2b16.o : arch/i386/prefix/unnrv2b16.S $(MAKEDEPS) $(POST_O_DEPS) $(unnrv2b16_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S)
 
BOBJS += $(BIN)/unnrv2b16.o
 
$(BIN)/unnrv2b16.s : arch/i386/prefix/unnrv2b16.S $(MAKEDEPS) $(POST_O_DEPS) $(unnrv2b16_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_S_to_s)
 
s_OBJS += $(BIN)/unnrv2b16.s
 
bin/deps/arch/i386/prefix/unnrv2b16.S.d : $(unnrv2b16_DEPS)
 
TAGS : $(unnrv2b16_DEPS)

