efidrvprefix_DEPS = arch/x86/prefix/efidrvprefix.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdlib.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/assert.h include/gpxe/init.h \
 include/gpxe/tables.h include/gpxe/efi/efi.h include/gpxe/efi/Uefi.h \
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
 include/gpxe/uuid.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdlib.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/assert.h:

include/gpxe/init.h:

include/gpxe/tables.h:

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

include/gpxe/uuid.h:

$(BIN)/efidrvprefix.o : arch/x86/prefix/efidrvprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(efidrvprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efidrvprefix.o
 
$(BIN)/efidrvprefix.dbg%.o : arch/x86/prefix/efidrvprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(efidrvprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efidrvprefix.dbg%.o
 
$(BIN)/efidrvprefix.c : arch/x86/prefix/efidrvprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(efidrvprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efidrvprefix.c
 
$(BIN)/efidrvprefix.s : arch/x86/prefix/efidrvprefix.c $(MAKEDEPS) $(POST_O_DEPS) $(efidrvprefix_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efidrvprefix.s
 
bin/deps/arch/x86/prefix/efidrvprefix.c.d : $(efidrvprefix_DEPS)
 
TAGS : $(efidrvprefix_DEPS)

