acpi_DEPS = core/acpi.c include/compiler.h arch/i386/include/bits/compiler.h \
 include/gpxe/acpi.h include/stdint.h arch/i386/include/bits/stdint.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/acpi.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

$(BIN)/acpi.o : core/acpi.c $(MAKEDEPS) $(POST_O_DEPS) $(acpi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/acpi.o
 
$(BIN)/acpi.dbg%.o : core/acpi.c $(MAKEDEPS) $(POST_O_DEPS) $(acpi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/acpi.dbg%.o
 
$(BIN)/acpi.c : core/acpi.c $(MAKEDEPS) $(POST_O_DEPS) $(acpi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/acpi.c
 
$(BIN)/acpi.s : core/acpi.c $(MAKEDEPS) $(POST_O_DEPS) $(acpi_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/acpi.s
 
bin/deps/core/acpi.c.d : $(acpi_DEPS)
 
TAGS : $(acpi_DEPS)

