image_cmd_DEPS = hci/commands/image_cmd.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdlib.h include/assert.h \
 include/stdio.h include/stdarg.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/libgen.h include/getopt.h \
 include/stddef.h include/gpxe/image.h include/gpxe/tables.h \
 include/gpxe/list.h include/gpxe/uaccess.h include/string.h \
 arch/x86/include/bits/string.h include/gpxe/api.h config/ioapi.h \
 config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/refcnt.h include/gpxe/command.h \
 include/usr/imgmgmt.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/libgen.h:

include/getopt.h:

include/stddef.h:

include/gpxe/image.h:

include/gpxe/tables.h:

include/gpxe/list.h:

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

include/gpxe/refcnt.h:

include/gpxe/command.h:

include/usr/imgmgmt.h:

$(BIN)/image_cmd.o : hci/commands/image_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(image_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/image_cmd.o
 
$(BIN)/image_cmd.dbg%.o : hci/commands/image_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(image_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/image_cmd.dbg%.o
 
$(BIN)/image_cmd.c : hci/commands/image_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(image_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/image_cmd.c
 
$(BIN)/image_cmd.s : hci/commands/image_cmd.c $(MAKEDEPS) $(POST_O_DEPS) $(image_cmd_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/image_cmd.s
 
bin/deps/hci/commands/image_cmd.c.d : $(image_cmd_DEPS)
 
TAGS : $(image_cmd_DEPS)

