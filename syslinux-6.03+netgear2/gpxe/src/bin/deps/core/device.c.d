device_DEPS = core/device.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/string.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
 arch/x86/include/bits/string.h include/gpxe/list.h include/assert.h \
 include/gpxe/tables.h include/gpxe/device.h include/gpxe/init.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/string.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

arch/x86/include/bits/string.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/device.h:

include/gpxe/init.h:

$(BIN)/device.o : core/device.c $(MAKEDEPS) $(POST_O_DEPS) $(device_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/device.o
 
$(BIN)/device.dbg%.o : core/device.c $(MAKEDEPS) $(POST_O_DEPS) $(device_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/device.dbg%.o
 
$(BIN)/device.c : core/device.c $(MAKEDEPS) $(POST_O_DEPS) $(device_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/device.c
 
$(BIN)/device.s : core/device.c $(MAKEDEPS) $(POST_O_DEPS) $(device_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/device.s
 
bin/deps/core/device.c.d : $(device_DEPS)
 
TAGS : $(device_DEPS)

