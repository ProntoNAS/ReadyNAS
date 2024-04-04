pxe_cmd_DEPS = arch/i386/hci/commands/pxe_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/netdevice.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/gpxe/list.h \
 include/stddef.h include/assert.h include/gpxe/tables.h \
 include/gpxe/refcnt.h include/gpxe/settings.h include/gpxe/command.h \
 include/hci/ifmgmt_cmd.h arch/i386/include/pxe_call.h \
 arch/i386/include/pxe_api.h arch/i386/include/pxe_types.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 arch/i386/include/realmode.h arch/i386/include/registers.h \
 include/gpxe/uaccess.h include/string.h arch/x86/include/bits/string.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/netdevice.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

include/gpxe/command.h:

include/hci/ifmgmt_cmd.h:

arch/i386/include/pxe_call.h:

arch/i386/include/pxe_api.h:

arch/i386/include/pxe_types.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

arch/i386/include/realmode.h:

arch/i386/include/registers.h:

include/gpxe/uaccess.h:

include/string.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

$(BIN)/pxe_cmd.o : arch/i386/hci/commands/pxe_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pxe_cmd.o
 
$(BIN)/pxe_cmd.dbg%.o : arch/i386/hci/commands/pxe_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pxe_cmd.dbg%.o
 
$(BIN)/pxe_cmd.c : arch/i386/hci/commands/pxe_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pxe_cmd.c
 
$(BIN)/pxe_cmd.s : arch/i386/hci/commands/pxe_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(pxe_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pxe_cmd.s
 
bin/deps/arch/i386/hci/commands/pxe_cmd.c.d : $(pxe_cmd_DEPS)
 
TAGS : $(pxe_cmd_DEPS)

