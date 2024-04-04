efi_image_DEPS = image/efi_image.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/errno.h include/gpxe/errfile.h \
 arch/i386/include/bits/errfile.h include/gpxe/efi/efi.h \
 include/gpxe/efi/Uefi.h include/gpxe/efi/Uefi/UefiBaseType.h \
 include/gpxe/efi/Base.h include/gpxe/efi/ProcessorBind.h \
 include/gpxe/efi/Ia32/ProcessorBind.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/gpxe/efi/Uefi/UefiSpec.h \
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
 include/gpxe/tables.h include/gpxe/uuid.h include/gpxe/image.h \
 include/gpxe/list.h include/stddef.h include/assert.h \
 include/gpxe/uaccess.h include/string.h arch/x86/include/bits/string.h \
 include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/refcnt.h include/gpxe/features.h include/gpxe/dhcp.h \
 include/gpxe/in.h include/gpxe/socket.h include/gpxe/netdevice.h \
 include/gpxe/settings.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

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

include/gpxe/image.h:

include/gpxe/list.h:

include/stddef.h:

include/assert.h:

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

include/gpxe/features.h:

include/gpxe/dhcp.h:

include/gpxe/in.h:

include/gpxe/socket.h:

include/gpxe/netdevice.h:

include/gpxe/settings.h:

$(BIN)/efi_image.o : image/efi_image.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/efi_image.o
 
$(BIN)/efi_image.dbg%.o : image/efi_image.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/efi_image.dbg%.o
 
$(BIN)/efi_image.c : image/efi_image.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/efi_image.c
 
$(BIN)/efi_image.s : image/efi_image.c $(MAKEDEPS) $(POST_O_DEPS) $(efi_image_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/efi_image.s
 
bin/deps/image/efi_image.c.d : $(efi_image_DEPS)
 
TAGS : $(efi_image_DEPS)

