pnic_DEPS = drivers/net/pnic.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/stdint.h \
 arch/i386/include/bits/stdint.h include/stdio.h include/stdarg.h \
 include/gpxe/io.h include/gpxe/api.h config/ioapi.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/uaccess.h include/string.h \
 include/stddef.h arch/x86/include/bits/string.h \
 include/gpxe/efi/efi_uaccess.h arch/i386/include/bits/uaccess.h \
 arch/i386/include/librm.h include/gpxe/efi/efi_io.h \
 arch/i386/include/bits/io.h arch/i386/include/gpxe/x86_io.h \
 include/errno.h include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/assert.h include/gpxe/tables.h include/gpxe/pci_io.h \
 include/gpxe/efi/efi_pci.h arch/x86/include/bits/pci_io.h \
 arch/x86/include/gpxe/pcibios.h arch/x86/include/gpxe/pcidirect.h \
 include/gpxe/pci_ids.h include/gpxe/if_ether.h include/gpxe/ethernet.h \
 include/gpxe/iobuf.h include/gpxe/netdevice.h include/gpxe/refcnt.h \
 include/gpxe/settings.h drivers/net/pnic_api.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdio.h:

include/stdarg.h:

include/gpxe/io.h:

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

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/assert.h:

include/gpxe/tables.h:

include/gpxe/pci_io.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/pci_ids.h:

include/gpxe/if_ether.h:

include/gpxe/ethernet.h:

include/gpxe/iobuf.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

drivers/net/pnic_api.h:

$(BIN)/pnic.o : drivers/net/pnic.c $(MAKEDEPS) $(POST_O_DEPS) $(pnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/pnic.o
 
$(BIN)/pnic.dbg%.o : drivers/net/pnic.c $(MAKEDEPS) $(POST_O_DEPS) $(pnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/pnic.dbg%.o
 
$(BIN)/pnic.c : drivers/net/pnic.c $(MAKEDEPS) $(POST_O_DEPS) $(pnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/pnic.c
 
$(BIN)/pnic.s : drivers/net/pnic.c $(MAKEDEPS) $(POST_O_DEPS) $(pnic_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/pnic.s
 
bin/deps/drivers/net/pnic.c.d : $(pnic_DEPS)
 
TAGS : $(pnic_DEPS)


# NIC	
# NIC	family	drivers/net/pnic
DRIVERS += pnic

# NIC	pnic	fefe,efef	Bochs Pseudo NIC Adaptor
DRIVER_pnic = pnic
ROM_TYPE_pnic = pci
ROM_DESCRIPTION_pnic = "Bochs Pseudo NIC Adaptor"
PCI_VENDOR_pnic = 0xfefe
PCI_DEVICE_pnic = 0xefef
ROMS += pnic
ROMS_pnic += pnic

# NIC	fefeefef	fefe,efef	Bochs Pseudo NIC Adaptor
DRIVER_fefeefef = pnic
ROM_TYPE_fefeefef = pci
ROM_DESCRIPTION_fefeefef = "Bochs Pseudo NIC Adaptor"
PCI_VENDOR_fefeefef = 0xfefe
PCI_DEVICE_fefeefef = 0xefef
ROMS += fefeefef
ROMS_pnic += fefeefef
