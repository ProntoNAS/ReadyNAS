efi_timer_DEPS = interface/efi/efi_timer.c include/compiler.h \
 arch/i386/include/bits/compiler.h arch/i386/include/limits.h \
 include/assert.h include/unistd.h include/stddef.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdarg.h include/gpxe/timer.h \
 include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/gpxe/efi/efi.h include/gpxe/efi/Uefi.h \
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
 include/gpxe/tables.h include/gpxe/uuid.h \
 include/gpxe/efi/Protocol/Cpu.h include/gpxe/efi/Protocol/DebugSupport.h \
 include/gpxe/efi/IndustryStandard/PeImage.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

arch/i386/include/limits.h:

include/assert.h:

include/unistd.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdarg.h:

include/gpxe/timer.h:

include/gpxe/api.h:

config/timer.h:

config/defaults.h:

config/defaults/pcbios.h:

include/gpxe/efi/efi_timer.h:

arch/i386/include/bits/timer.h:

arch/i386/include/gpxe/bios_timer.h:

arch/i386/include/gpxe/timer2.h:

arch/i386/include/gpxe/rdtsc_timer.h:

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

include/gpxe/efi/Protocol/Cpu.h:

include/gpxe/efi/Protocol/DebugSupport.h:

include/gpxe/efi/IndustryStandard/PeImage.h:

$(BIN)/efi_timer.o : interface/efi/efi_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efi_timer.o
 
$(BIN)/efi_timer.dbg%.o : interface/efi/efi_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efi_timer.dbg%.o
 
$(BIN)/efi_timer.c : interface/efi/efi_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efi_timer.c
 
$(BIN)/efi_timer.s : interface/efi/efi_timer.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_timer_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efi_timer.s
 
bin/deps/interface/efi/efi_timer.c.d : $(efi_timer_DEPS)
 
TAGS : $(efi_timer_DEPS)

