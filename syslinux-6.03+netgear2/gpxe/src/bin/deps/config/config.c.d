config_DEPS = config/config.c include/compiler.h \
 arch/i386/include/bits/compiler.h config/general.h config/defaults.h \
 config/defaults/pcbios.h config/console.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

config/console.h:

$(BIN)/config.o : config/config.c $(MAKEDEPS) $(POST_O_DEPS) $(config_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/config.o
 
$(BIN)/config.dbg%.o : config/config.c $(MAKEDEPS) $(POST_O_DEPS) $(config_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/config.dbg%.o
 
$(BIN)/config.c : config/config.c $(MAKEDEPS) $(POST_O_DEPS) $(config_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/config.c
 
$(BIN)/config.s : config/config.c $(MAKEDEPS) $(POST_O_DEPS) $(config_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/config.s
 
bin/deps/config/config.c.d : $(config_DEPS)
 
TAGS : $(config_DEPS)

