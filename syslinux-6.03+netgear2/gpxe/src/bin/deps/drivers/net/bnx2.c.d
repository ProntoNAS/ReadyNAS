bnx2_DEPS = drivers/net/bnx2.c include/compiler.h \
 arch/i386/include/bits/compiler.h include/etherboot.h include/stddef.h \
 include/stdint.h arch/i386/include/bits/stdint.h include/stdlib.h \
 include/assert.h include/stdio.h include/stdarg.h include/unistd.h \
 include/gpxe/timer.h include/gpxe/api.h config/timer.h config/defaults.h \
 config/defaults/pcbios.h include/gpxe/efi/efi_timer.h \
 arch/i386/include/bits/timer.h arch/i386/include/gpxe/bios_timer.h \
 arch/i386/include/gpxe/timer2.h arch/i386/include/gpxe/rdtsc_timer.h \
 include/strings.h arch/i386/include/limits.h include/string.h \
 arch/x86/include/bits/string.h include/console.h include/gpxe/tables.h \
 include/gpxe/if_arp.h include/gpxe/if_ether.h include/nic.h \
 include/byteswap.h include/endian.h arch/i386/include/bits/endian.h \
 arch/i386/include/bits/byteswap.h include/little_bswap.h \
 include/gpxe/pci.h include/gpxe/device.h include/gpxe/list.h \
 include/gpxe/pci_io.h config/ioapi.h include/gpxe/efi/efi_pci.h \
 arch/x86/include/bits/pci_io.h arch/x86/include/gpxe/pcibios.h \
 arch/x86/include/gpxe/pcidirect.h include/gpxe/io.h \
 include/gpxe/uaccess.h include/gpxe/efi/efi_uaccess.h \
 arch/i386/include/bits/uaccess.h arch/i386/include/librm.h \
 include/gpxe/efi/efi_io.h arch/i386/include/bits/io.h \
 arch/i386/include/gpxe/x86_io.h include/gpxe/pci_ids.h \
 include/gpxe/isapnp.h include/gpxe/isa_ids.h include/gpxe/isa.h \
 include/gpxe/eisa.h include/gpxe/mca.h include/errno.h \
 include/gpxe/errfile.h arch/i386/include/bits/errfile.h \
 include/gpxe/ethernet.h include/mii.h include/gpxe/netdevice.h \
 include/gpxe/refcnt.h include/gpxe/settings.h drivers/net/bnx2.h \
 drivers/net/bnx2_fw.h

include/compiler.h:

arch/i386/include/bits/compiler.h:

include/etherboot.h:

include/stddef.h:

include/stdint.h:

arch/i386/include/bits/stdint.h:

include/stdlib.h:

include/assert.h:

include/stdio.h:

include/stdarg.h:

include/unistd.h:

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

include/strings.h:

arch/i386/include/limits.h:

include/string.h:

arch/x86/include/bits/string.h:

include/console.h:

include/gpxe/tables.h:

include/gpxe/if_arp.h:

include/gpxe/if_ether.h:

include/nic.h:

include/byteswap.h:

include/endian.h:

arch/i386/include/bits/endian.h:

arch/i386/include/bits/byteswap.h:

include/little_bswap.h:

include/gpxe/pci.h:

include/gpxe/device.h:

include/gpxe/list.h:

include/gpxe/pci_io.h:

config/ioapi.h:

include/gpxe/efi/efi_pci.h:

arch/x86/include/bits/pci_io.h:

arch/x86/include/gpxe/pcibios.h:

arch/x86/include/gpxe/pcidirect.h:

include/gpxe/io.h:

include/gpxe/uaccess.h:

include/gpxe/efi/efi_uaccess.h:

arch/i386/include/bits/uaccess.h:

arch/i386/include/librm.h:

include/gpxe/efi/efi_io.h:

arch/i386/include/bits/io.h:

arch/i386/include/gpxe/x86_io.h:

include/gpxe/pci_ids.h:

include/gpxe/isapnp.h:

include/gpxe/isa_ids.h:

include/gpxe/isa.h:

include/gpxe/eisa.h:

include/gpxe/mca.h:

include/errno.h:

include/gpxe/errfile.h:

arch/i386/include/bits/errfile.h:

include/gpxe/ethernet.h:

include/mii.h:

include/gpxe/netdevice.h:

include/gpxe/refcnt.h:

include/gpxe/settings.h:

drivers/net/bnx2.h:

drivers/net/bnx2_fw.h:

$(BIN)/bnx2.o : drivers/net/bnx2.c $(MAKEDEPS) $(POST_O_DEPS) $(bnx2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c)
 
BOBJS += $(BIN)/bnx2.o
 
$(BIN)/bnx2.dbg%.o : drivers/net/bnx2.c $(MAKEDEPS) $(POST_O_DEPS) $(bnx2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_dbg%.o)
 
dbg%.o_OBJS += $(BIN)/bnx2.dbg%.o
 
$(BIN)/bnx2.c : drivers/net/bnx2.c $(MAKEDEPS) $(POST_O_DEPS) $(bnx2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_c)
 
c_OBJS += $(BIN)/bnx2.c
 
$(BIN)/bnx2.s : drivers/net/bnx2.c $(MAKEDEPS) $(POST_O_DEPS) $(bnx2_DEPS) 
	$(QM)echo "  [BUILD] $@" 
	$(RULE_c_to_s)
 
s_OBJS += $(BIN)/bnx2.s
 
bin/deps/drivers/net/bnx2.c.d : $(bnx2_DEPS)
 
TAGS : $(bnx2_DEPS)


# NIC	
# NIC	family	drivers/net/bnx2
DRIVERS += bnx2

# NIC	bnx2-5706	14e4,164a	Broadcom NetXtreme II BCM5706
DRIVER_bnx2-5706 = bnx2
ROM_TYPE_bnx2-5706 = pci
ROM_DESCRIPTION_bnx2-5706 = "Broadcom NetXtreme II BCM5706"
PCI_VENDOR_bnx2-5706 = 0x14e4
PCI_DEVICE_bnx2-5706 = 0x164a
ROMS += bnx2-5706
ROMS_bnx2 += bnx2-5706

# NIC	14e4164a	14e4,164a	Broadcom NetXtreme II BCM5706
DRIVER_14e4164a = bnx2
ROM_TYPE_14e4164a = pci
ROM_DESCRIPTION_14e4164a = "Broadcom NetXtreme II BCM5706"
PCI_VENDOR_14e4164a = 0x14e4
PCI_DEVICE_14e4164a = 0x164a
ROMS += 14e4164a
ROMS_bnx2 += 14e4164a

# NIC	bnx2-5708	14e4,164c	Broadcom NetXtreme II BCM5708
DRIVER_bnx2-5708 = bnx2
ROM_TYPE_bnx2-5708 = pci
ROM_DESCRIPTION_bnx2-5708 = "Broadcom NetXtreme II BCM5708"
PCI_VENDOR_bnx2-5708 = 0x14e4
PCI_DEVICE_bnx2-5708 = 0x164c
ROMS += bnx2-5708
ROMS_bnx2 += bnx2-5708

# NIC	14e4164c	14e4,164c	Broadcom NetXtreme II BCM5708
DRIVER_14e4164c = bnx2
ROM_TYPE_14e4164c = pci
ROM_DESCRIPTION_14e4164c = "Broadcom NetXtreme II BCM5708"
PCI_VENDOR_14e4164c = 0x14e4
PCI_DEVICE_14e4164c = 0x164c
ROMS += 14e4164c
ROMS_bnx2 += 14e4164c

# NIC	bnx2-5706S	14e4,16aa	Broadcom NetXtreme II BCM5706S
DRIVER_bnx2-5706S = bnx2
ROM_TYPE_bnx2-5706S = pci
ROM_DESCRIPTION_bnx2-5706S = "Broadcom NetXtreme II BCM5706S"
PCI_VENDOR_bnx2-5706S = 0x14e4
PCI_DEVICE_bnx2-5706S = 0x16aa
ROMS += bnx2-5706S
ROMS_bnx2 += bnx2-5706S

# NIC	14e416aa	14e4,16aa	Broadcom NetXtreme II BCM5706S
DRIVER_14e416aa = bnx2
ROM_TYPE_14e416aa = pci
ROM_DESCRIPTION_14e416aa = "Broadcom NetXtreme II BCM5706S"
PCI_VENDOR_14e416aa = 0x14e4
PCI_DEVICE_14e416aa = 0x16aa
ROMS += 14e416aa
ROMS_bnx2 += 14e416aa

# NIC	bnx2-5708S	14e4,16ac	Broadcom NetXtreme II BCM5708S
DRIVER_bnx2-5708S = bnx2
ROM_TYPE_bnx2-5708S = pci
ROM_DESCRIPTION_bnx2-5708S = "Broadcom NetXtreme II BCM5708S"
PCI_VENDOR_bnx2-5708S = 0x14e4
PCI_DEVICE_bnx2-5708S = 0x16ac
ROMS += bnx2-5708S
ROMS_bnx2 += bnx2-5708S

# NIC	14e416ac	14e4,16ac	Broadcom NetXtreme II BCM5708S
DRIVER_14e416ac = bnx2
ROM_TYPE_14e416ac = pci
ROM_DESCRIPTION_14e416ac = "Broadcom NetXtreme II BCM5708S"
PCI_VENDOR_14e416ac = 0x14e4
PCI_DEVICE_14e416ac = 0x16ac
ROMS += 14e416ac
ROMS_bnx2 += 14e416ac
