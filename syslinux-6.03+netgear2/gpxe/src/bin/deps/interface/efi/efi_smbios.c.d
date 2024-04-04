efi_smbios_DEPS = interface/efi/efi_smbios.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/smbios.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/api.h config/general.h \
 config/defaults.h config/defaults/pcbios.h include/gpxe/uaccess.h \
 include/string.h include/stddef.h arch/x86/include/bits/string.h \
 config/ioapi.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_smbios.h arch/i386/include/bits/smbios.h \
 arch/i386/include/gpxe/bios_smbios.h include/gpxe/efi/efi.h \
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
 include/gpxe/tables.h include/gpxe/uuid.h include/gpxe/efi/Guid/SmBios.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/smbios.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/gpxe/api.h:

config/general.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/uaccess.h:

include/string.h:

include/stddef.h:

arch/x86/include/bits/string.h:

config/ioapi.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_smbios.h:

arch/i386/include/bits/smbios.h:

arch/i386/include/gpxe/bios_smbios.h:

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

include/gpxe/efi/Guid/SmBios.h:

$(BIN)/efi_smbios.o : interface/efi/efi_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efi_smbios.o
 
$(BIN)/efi_smbios.dbg%.o : interface/efi/efi_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efi_smbios.dbg%.o
 
$(BIN)/efi_smbios.c : interface/efi/efi_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efi_smbios.c
 
$(BIN)/efi_smbios.s : interface/efi/efi_smbios.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_smbios_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efi_smbios.s
 
bin/deps/interface/efi/efi_smbios.c.d : $(efi_smbios_DEPS)
 
TAGS : $(efi_smbios_DEPS)

