serial_console_DEPS = core/serial_console.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/init.h \
 include/gpxe/tables.h include/gpxe/serial.h include/console.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/init.h:

include/gpxe/tables.h:

include/gpxe/serial.h:

include/console.h:

$(BIN)/serial_console.o : core/serial_console.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/serial_console.o
 
$(BIN)/serial_console.dbg%.o : core/serial_console.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/serial_console.dbg%.o
 
$(BIN)/serial_console.c : core/serial_console.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/serial_console.c
 
$(BIN)/serial_console.s : core/serial_console.c $(MAKEDEPS) $(POST_O_DEPS) $(serial_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/serial_console.s
 
bin/deps/core/serial_console.c.d : $(serial_console_DEPS)
 
TAGS : $(serial_console_DEPS)

