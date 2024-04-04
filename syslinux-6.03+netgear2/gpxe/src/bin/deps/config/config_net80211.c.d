config_net80211_DEPS = config/config_net80211.c include/compiler.h \
 arch/i386/include/bits/compiler.h config/general.h config/defaults.h \
 config/defaults/pcbios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

$(BIN)/config_net80211.o : config/config_net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(config_net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/config_net80211.o
 
$(BIN)/config_net80211.dbg%.o : config/config_net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(config_net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/config_net80211.dbg%.o
 
$(BIN)/config_net80211.c : config/config_net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(config_net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/config_net80211.c
 
$(BIN)/config_net80211.s : config/config_net80211.c $(MAKEDEPS) $(POST_O_DEPS) $(config_net80211_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/config_net80211.s
 
bin/deps/config/config_net80211.c.d : $(config_net80211_DEPS)
 
TAGS : $(config_net80211_DEPS)

