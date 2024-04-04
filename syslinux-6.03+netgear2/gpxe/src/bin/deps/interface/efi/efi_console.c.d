efi_console_DEPS = interface/efi/efi_console.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/gpxe/efi/efi.h \
 include/gpxe/efi/Uefi.h include/gpxe/efi/Uefi/UefiBaseType.h \
 include/gpxe/efi/Base.h include/gpxe/efi/ProcessorBind.h \
 include/gpxe/efi/Ia32/ProcessorBind.h include/gpxe/efi/Uefi/UefiSpec.h \
 include/gpxe/efi/Uefi/UefiMultiPhase.h \
 include/gpxe/efi/Protocol/DevicePath.h include/gpxe/efi/Guid/PcAnsi.h \
 include/gpxe/efi/Protocol/SimpleTextIn.h \
 include/gpxe/efi/Protocol/SimpleTextOut.h \
 include/gpxe/efi/Uefi/UefiPxe.h include/gpxe/efi/Uefi/UefiGpt.h \
 include/gpxe/efi/Uefi/UefiInternalFormRepresentation.h \
 include/gpxe/efi/PiDxe.h include/gpxe/efi/Pi/PiDxeCis.h \
 include/gpxe/efi/Pi/PiMultiPhase.h \
 include/gpxe/efi/Pi/PiFirmwareVolume.h \
 include/gpxe/efi/Pi/PiFirmwareFile.h include/gpxe/efi/Pi/PiBootMode.h \
 include/gpxe/efi/Pi/PiHob.h include/gpxe/efi/Pi/PiDependency.h \
 include/gpxe/tables.h include/gpxe/uuid.h include/gpxe/ansiesc.h \
 include/console.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/efi/efi.h:

include/gpxe/efi/Uefi.h:

include/gpxe/efi/Uefi/UefiBaseType.h:

include/gpxe/efi/Base.h:

include/gpxe/efi/ProcessorBind.h:

include/gpxe/efi/Ia32/ProcessorBind.h:

include/gpxe/efi/Uefi/UefiSpec.h:

include/gpxe/efi/Uefi/UefiMultiPhase.h:

include/gpxe/efi/Protocol/DevicePath.h:

include/gpxe/efi/Guid/PcAnsi.h:

include/gpxe/efi/Protocol/SimpleTextIn.h:

include/gpxe/efi/Protocol/SimpleTextOut.h:

include/gpxe/efi/Uefi/UefiPxe.h:

include/gpxe/efi/Uefi/UefiGpt.h:

include/gpxe/efi/Uefi/UefiInternalFormRepresentation.h:

include/gpxe/efi/PiDxe.h:

include/gpxe/efi/Pi/PiDxeCis.h:

include/gpxe/efi/Pi/PiMultiPhase.h:

include/gpxe/efi/Pi/PiFirmwareVolume.h:

include/gpxe/efi/Pi/PiFirmwareFile.h:

include/gpxe/efi/Pi/PiBootMode.h:

include/gpxe/efi/Pi/PiHob.h:

include/gpxe/efi/Pi/PiDependency.h:

include/gpxe/tables.h:

include/gpxe/uuid.h:

include/gpxe/ansiesc.h:

include/console.h:

$(BIN)/efi_console.o : interface/efi/efi_console.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efi_console.o
 
$(BIN)/efi_console.dbg%.o : interface/efi/efi_console.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efi_console.dbg%.o
 
$(BIN)/efi_console.c : interface/efi/efi_console.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efi_console.c
 
$(BIN)/efi_console.s : interface/efi/efi_console.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_console_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efi_console.s
 
bin/deps/interface/efi/efi_console.c.d : $(efi_console_DEPS)
 
TAGS : $(efi_console_DEPS)

