config_romprefix_DEPS = config/config_romprefix.c include/compiler.h \
 arch/i386/include/bits/compiler.h config/general.h config/defaults.h \
 config/defaults/pcbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

$(BIN)/config_romprefix.o : config/config_romprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(config_romprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/config_romprefix.o
 
$(BIN)/config_romprefix.dbg%.o : config/config_romprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(config_romprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/config_romprefix.dbg%.o
 
$(BIN)/config_romprefix.c : config/config_romprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(config_romprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/config_romprefix.c
 
$(BIN)/config_romprefix.s : config/config_romprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(config_romprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/config_romprefix.s
 
bin/deps/config/config_romprefix.c.d : $(config_romprefix_DEPS)
 
TAGS : $(config_romprefix_DEPS)

