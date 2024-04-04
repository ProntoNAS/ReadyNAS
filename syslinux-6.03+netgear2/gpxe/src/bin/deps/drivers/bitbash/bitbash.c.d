bitbash_DEPS = drivers/bitbash/bitbash.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/bitbash.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/bitbash.h:

$(BIN)/bitbash.o : drivers/bitbash/bitbash.c $(MAKEDEPS) $(POST_O_DEPS) $(bitbash_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bitbash.o
 
$(BIN)/bitbash.dbg%.o : drivers/bitbash/bitbash.c $(MAKEDEPS) $(POST_O_DEPS) $(bitbash_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bitbash.dbg%.o
 
$(BIN)/bitbash.c : drivers/bitbash/bitbash.c $(MAKEDEPS) $(POST_O_DEPS) $(bitbash_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bitbash.c
 
$(BIN)/bitbash.s : drivers/bitbash/bitbash.c $(MAKEDEPS) $(POST_O_DEPS) $(bitbash_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bitbash.s
 
bin/deps/drivers/bitbash/bitbash.c.d : $(bitbash_DEPS)
 
TAGS : $(bitbash_DEPS)

