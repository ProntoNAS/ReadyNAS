efix86_nap_DEPS = arch/x86/interface/efi/efix86_nap.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/gpxe/nap.h include/gpxe/api.h \
 config/nap.h config/defaults.h config/defaults/pcbios.h \
 include/gpxe/null_nap.h arch/i386/include/bits/nap.h \
 arch/i386/include/gpxe/bios_nap.h arch/x86/include/gpxe/efi/efix86_nap.h \
 include/gpxe/efi/efi.h include/gpxe/efi/Uefi.h \
 include/gpxe/efi/Uefi/UefiBaseType.h include/gpxe/efi/Base.h \
 include/gpxe/efi/ProcessorBind.h include/gpxe/efi/Ia32/ProcessorBind.h \
 include/stdint.h arch/i386/include/bits/stdint.h \
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

include/gpxe/nap.h:

include/gpxe/api.h:

config/nap.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/null_nap.h:

arch/i386/include/bits/nap.h:

arch/i386/include/gpxe/bios_nap.h:

arch/x86/include/gpxe/efi/efix86_nap.h:

include/gpxe/efi/efi.h:

include/gpxe/efi/Uefi.h:

include/gpxe/efi/Uefi/UefiBaseType.h:

include/gpxe/efi/Base.h:

include/gpxe/efi/ProcessorBind.h:

include/gpxe/efi/Ia32/ProcessorBind.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

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

$(BIN)/efix86_nap.o : arch/x86/interface/efi/efix86_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(efix86_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efix86_nap.o
 
$(BIN)/efix86_nap.dbg%.o : arch/x86/interface/efi/efix86_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(efix86_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efix86_nap.dbg%.o
 
$(BIN)/efix86_nap.c : arch/x86/interface/efi/efix86_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(efix86_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efix86_nap.c
 
$(BIN)/efix86_nap.s : arch/x86/interface/efi/efix86_nap.c $(MAKEDEPS) $(POST_O_DEPS) $(efix86_nap_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efix86_nap.s
 
bin/deps/arch/x86/interface/efi/efix86_nap.c.d : $(efix86_nap_DEPS)
 
TAGS : $(efix86_nap_DEPS)

