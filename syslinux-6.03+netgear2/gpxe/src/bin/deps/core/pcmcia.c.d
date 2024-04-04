pcmcia_DEPS = core/pcmcia.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/pcmcia.o : core/pcmcia.c $(MAKEDEPS) $(POST_O_DEPS) $(pcmcia_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pcmcia.o
 
$(BIN)/pcmcia.dbg%.o : core/pcmcia.c $(MAKEDEPS) $(POST_O_DEPS) $(pcmcia_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pcmcia.dbg%.o
 
$(BIN)/pcmcia.c : core/pcmcia.c $(MAKEDEPS) $(POST_O_DEPS) $(pcmcia_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pcmcia.c
 
$(BIN)/pcmcia.s : core/pcmcia.c $(MAKEDEPS) $(POST_O_DEPS) $(pcmcia_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pcmcia.s
 
bin/deps/core/pcmcia.c.d : $(pcmcia_DEPS)
 
TAGS : $(pcmcia_DEPS)

