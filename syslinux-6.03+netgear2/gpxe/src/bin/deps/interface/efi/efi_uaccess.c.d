efi_uaccess_DEPS = interface/efi/efi_uaccess.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/uaccess.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h include/gpxe/api.h \
 config/ioapi.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi.h include/gpxe/efi/Uefi.h \
 include/gpxe/efi/Uefi/UefiBaseType.h include/gpxe/efi/Base.h \
 include/gpxe/efi/ProcessorBind.h include/gpxe/efi/Ia32/ProcessorBind.h \
 include/gpxe/efi/Uefi/UefiSpec.h include/gpxe/efi/Uefi/UefiMultiPhase.h \
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
 include/gpxe/tables.h include/gpxe/uuid.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/gpxe/uaccess.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

include/gpxe/api.h:

config/ioapi.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

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

$(BIN)/efi_uaccess.o : interface/efi/efi_uaccess.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_uaccess_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efi_uaccess.o
 
$(BIN)/efi_uaccess.dbg%.o : interface/efi/efi_uaccess.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_uaccess_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efi_uaccess.dbg%.o
 
$(BIN)/efi_uaccess.c : interface/efi/efi_uaccess.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_uaccess_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efi_uaccess.c
 
$(BIN)/efi_uaccess.s : interface/efi/efi_uaccess.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_uaccess_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efi_uaccess.s
 
bin/deps/interface/efi/efi_uaccess.c.d : $(efi_uaccess_DEPS)
 
TAGS : $(efi_uaccess_DEPS)

