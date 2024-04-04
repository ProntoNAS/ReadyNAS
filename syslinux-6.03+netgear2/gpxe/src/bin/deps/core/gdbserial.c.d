gdbserial_DEPS = core/gdbserial.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/assert.h include/gpxe/serial.h \
 include/gpxe/gdbstub.h include/stdint.h arch/i386/include/bits/stdint.h \
 include/gpxe/tables.h arch/i386/include/gdbmach.h \
 include/gpxe/gdbserial.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/assert.h:

include/gpxe/serial.h:

include/gpxe/gdbstub.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/tables.h:

arch/i386/include/gdbmach.h:

include/gpxe/gdbserial.h:

$(BIN)/gdbserial.o : core/gdbserial.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbserial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/gdbserial.o
 
$(BIN)/gdbserial.dbg%.o : core/gdbserial.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbserial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/gdbserial.dbg%.o
 
$(BIN)/gdbserial.c : core/gdbserial.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbserial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/gdbserial.c
 
$(BIN)/gdbserial.s : core/gdbserial.c $(MAKEDEPS) $(POST_O_DEPS) $(gdbserial_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/gdbserial.s
 
bin/deps/core/gdbserial.c.d : $(gdbserial_DEPS)
 
TAGS : $(gdbserial_DEPS)

