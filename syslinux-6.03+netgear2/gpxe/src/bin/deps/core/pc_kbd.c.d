pc_kbd_DEPS = core/pc_kbd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/io.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/console.h include/gpxe/tables.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/io.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/console.h:

include/gpxe/tables.h:

$(BIN)/pc_kbd.o : core/pc_kbd.c $(MAKEDEPS) $(POST_O_DEPS) $(pc_kbd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pc_kbd.o
 
$(BIN)/pc_kbd.dbg%.o : core/pc_kbd.c $(MAKEDEPS) $(POST_O_DEPS) $(pc_kbd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pc_kbd.dbg%.o
 
$(BIN)/pc_kbd.c : core/pc_kbd.c $(MAKEDEPS) $(POST_O_DEPS) $(pc_kbd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pc_kbd.c
 
$(BIN)/pc_kbd.s : core/pc_kbd.c $(MAKEDEPS) $(POST_O_DEPS) $(pc_kbd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pc_kbd.s
 
bin/deps/core/pc_kbd.c.d : $(pc_kbd_DEPS)
 
TAGS : $(pc_kbd_DEPS)

