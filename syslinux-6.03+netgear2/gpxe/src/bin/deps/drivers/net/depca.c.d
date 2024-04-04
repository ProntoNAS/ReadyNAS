depca_DEPS = drivers/net/depca.c include/compiler.h \
 arch/i386/include/bits/compiler.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

$(BIN)/depca.o : drivers/net/depca.c $(MAKEDEPS) $(POST_O_DEPS) $(depca_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/depca.o
 
$(BIN)/depca.dbg%.o : drivers/net/depca.c $(MAKEDEPS) $(POST_O_DEPS) $(depca_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/depca.dbg%.o
 
$(BIN)/depca.c : drivers/net/depca.c $(MAKEDEPS) $(POST_O_DEPS) $(depca_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/depca.c
 
$(BIN)/depca.s : drivers/net/depca.c $(MAKEDEPS) $(POST_O_DEPS) $(depca_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/depca.s
 
bin/deps/drivers/net/depca.c.d : $(depca_DEPS)
 
TAGS : $(depca_DEPS)


# NIC	
# NIC	family	drivers/net/depca
DRIVERS += depca

# NIC	depca	-	Digital DE100 and DE200
DRIVER_depca = depca
ROM_TYPE_depca = isa
ROM_DESCRIPTION_depca = "Digital DE100 and DE200"
ROMS += depca
ROMS_depca += depca
