/* $Id: pcic.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 * pcic.c: Sparc/PCI controller support
 *
 * Copyright (C) 1998 V. Roganov and G. Raiko
 *
 * Code is derived from Ultra/PCI PSYCHO controller support, see that
 * for author info.
 *
 * Support for diverse IIep based platforms by Pete Zaitcev.
 * CP-1200 by Eric Brower.
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/slab.h>

#include <asm/io.h>

#include <linux/ctype.h>
#include <linux/pci.h>
#include <linux/timex.h>
#include <linux/interrupt.h>

#include <asm/irq.h>
#include <asm/uaccess.h>

#define pci_for_each_dev(dev) \
      for(dev = pci_dev_g(pci_devices.next); dev != pci_dev_g(&pci_devices); dev = pci_dev_g(dev->global_list.next))

#ifdef CONFIG_PCI_DEBUG
#define DBG		printk
#else
#define DBG(...)	
#endif

extern unsigned int g_sys_type;

struct pci_fixup pcibios_fixups[] = {
	{ 0 }
};

unsigned int pcic_pin_to_irq(unsigned int pin, char *name);

#define CONFIG_CMD(bus, device_fn, where) (PADRE_PCI_CONFIG_ADDRESS_EN | (((unsigned int)bus) << 16) | (((unsigned int)device_fn) << 8) | (where))

static int pcic_read_config_dword(unsigned int busno, unsigned int devfn,
		    int where, u32 *value)
{
	unsigned long flags;
	   
	local_irq_save(flags);	
	writel(CONFIG_CMD(busno,devfn,where), PADRE_PCI_CONFIG_ADDRESS);
	*value = readl(PADRE_PCI_CONFIG_DATA);
	writel(0,PADRE_PCI_CONFIG_ADDRESS);
	local_irq_restore(flags);

	return 0;
}

static int pcic_write_config_dword(unsigned int busno, unsigned int devfn,
		    int where, u32 value)
{
	unsigned long flags;

	local_irq_save(flags);	
	writel(CONFIG_CMD(busno,devfn,where), PADRE_PCI_CONFIG_ADDRESS);
	writel(value, PADRE_PCI_CONFIG_DATA);
	writel(0,PADRE_PCI_CONFIG_ADDRESS);

	local_irq_restore(flags);
	
	return 0;
}

static int pcic_read_config(struct pci_bus *bus, unsigned int devfn,
		   int where, int size, u32 *val)
{
    unsigned int v;

    if (bus->number != 0) return -EINVAL;
    switch (size) {
    case 1:
        pcic_read_config_dword(bus->number, devfn, where&~3, &v);
        *val = 0xff & (v >> (8*(where & 3)));
        return 0;
    case 2:
        if (where&1) return -EINVAL;
        pcic_read_config_dword(bus->number, devfn, where&~3, &v);
        *val = 0xffff & (v >> (8*(where & 3)));
        return 0;
    case 4:
        if (where&3) return -EINVAL;
        pcic_read_config_dword(bus->number, devfn, where&~3, val);
        return 0;
    }
    return -EINVAL;
}
	
static int pcic_write_config(struct pci_bus *bus, unsigned int devfn,
		   int where, int size, u32 val)
{
    unsigned int v;

    if (bus->number != 0) return -EINVAL;
    switch (size) {
    case 1:
        pcic_read_config_dword(bus->number, devfn, where&~3, &v);
        v = (v & ~(0xff << (8*(where&3)))) |
            ((0xff&val) << (8*(where&3)));
        return pcic_write_config_dword(bus->number, devfn, where&~3, v);
    case 2:
        if (where&1) return -EINVAL;
        pcic_read_config_dword(bus->number, devfn, where&~3, &v);
        v = (v & ~(0xffff << (8*(where&3)))) |
            ((0xffff&val) << (8*(where&3)));
        return pcic_write_config_dword(bus->number, devfn, where&~3, v);
    case 4:
	    if (where&3) return -EINVAL;
	    return pcic_write_config_dword(bus->number, devfn, where, val);
	}
	return -EINVAL;
}

static struct pci_ops pcic_ops = {
    .read =     pcic_read_config,
    .write =    pcic_write_config,
};


#define PCI_INTERRUPT_PINTA		27
#define PCI_INTERRUPT_PINTB		26
#define PCI_SLOT0				21
#define PCI_SLOT1				22
#define PCI_SLOT2				23

void pcibios_set_master(struct pci_dev *dev)
{
	u32 lat;
	pcic_read_config(dev->bus, dev->devfn, PCI_LATENCY_TIMER, 1, &lat);
	pcic_write_config(dev->bus, dev->devfn, PCI_LATENCY_TIMER, 1, 128);
}

void __init pcibios_fixup_irqs(void)
{
	struct pci_dev *dev;
	int slot;
	

	DBG("PCI: IRQ fixup\n");
	pci_for_each_dev(dev) {
		slot = PCI_SLOT(dev->devfn);
		if(slot == PCI_SLOT0)
			dev->irq = PCI_INTERRUPT_PINTA;
		else if(slot == PCI_SLOT1)
			dev->irq = PCI_INTERRUPT_PINTB;
		else {
			u8 pin;
			pci_read_config_byte(dev,PCI_INTERRUPT_PIN, &pin);
			if(pin == 2) {
                                // INTB pin of VIA USB chip connect to different sig.
                                switch (padre_boot_param.vpd_extend[0]) {
                                case 1:
                                        // FireDance
                                        dev->irq = PCI_INTERRUPT_PINTA;
                                        break;
                                default:
                                        // All other platforms
                                        dev->irq = PCI_INTERRUPT_PINTB;
                                        break;
                                }
                        }
			else
				dev->irq = PCI_INTERRUPT_PINTA;
		}
		DBG("slot=%d,irq=%d\n",slot,dev->irq);
	}
}


void __init pcibios_fixup_resources(void)
{
	struct pci_dev *dev;
	int i;
	unsigned long offset;

	DBG("PCI: address fixup\n");
	writel(0x4444, PADRE_PCI_SWAP);
	pci_for_each_dev(dev) {
		for( i = 0 ; i < 6; i++) {
			 if (dev->resource[i].flags & PCI_BASE_ADDRESS_SPACE_IO)
				 offset = 0x5c000000UL;
			 else 
				 offset = 0x4c000000UL;
                        // add offset for padre only when resource is assigned.
                        // so taht proc interface will not show those resources.
                        if (dev->resource[i].end) {
				dev->resource[i].start += offset;
				dev->resource[i].end +=   offset;
			}
			DBG("slot[%d]=%d,start=%lux, end=%lux\n",i,PCI_SLOT(dev->devfn),dev->resource[i].start,
					dev->resource[i].end);
		}
	}	
}

#define PCISERR_IRQ	11
#define PCIPERR_IRQ 	10
static irqreturn_t pcierr_interrupt(int irq, void *date, struct pt_regs *regs)
{
	if( irq == PCISERR_IRQ)
		printk("PCI system error\n");
	else if(irq == PCIPERR_IRQ)
		printk("PCI parity error\n");
	else
		printk("PCI unknow error\n");
	return IRQ_HANDLED;
}

/*
 * Main entry point from the PCI subsystem.
 */

void __init pcibios_init(void)
{
	int retval;

	DBG("PCI: Probing PCI hardware\n");

        if(g_sys_type != SYSTEM_TYPE_PZERO)
        {
		retval = request_irq(PCISERR_IRQ,&pcierr_interrupt,0,"pciserr",NULL);
		if( retval )
			printk("Cannot register irq for pci system error\n");
	}

	retval = request_irq(PCIPERR_IRQ,&pcierr_interrupt,0,"pciperr",NULL);
	if( retval )
		printk("Cannot register irq for pci parity error\n");
	
	pci_scan_bus(0, &pcic_ops, NULL);

	pcibios_fixup_irqs();

	pcibios_fixup_resources();

}

subsys_initcall(pcibios_init);

/*
 * Normally called from {do_}pci_scan_bus...
 */

void __init pcibios_fixup_bus(struct pci_bus *bus)
{
	struct pci_dev *dev;
	int i, has_io, has_mem;
	unsigned short cmd;

	/*
	 * Next crud is an equivalent of pbm = pcic_bus_to_pbm(bus);
	 */
	if (bus->number != 0) {
		printk("pcibios_fixup_bus: nonzero bus 0x%x\n", bus->number);
		return;
	}
	

	list_for_each_entry(dev, &bus->devices, bus_list) {
		u8 irq;
		
		u32 val0,val1,val2;

		/*
		 * Comment from i386 branch:
		 *     There are buggy BIOSes that forget to enable I/O and memory
		 *     access to PCI devices. We try to fix this, but we need to
		 *     be sure that the BIOS didn't forget to assign an address
		 *     to the device. [mj]
		 * OBP is a case of such BIOS :-)
		 */

		pci_read_config_byte(dev,PCI_INTERRUPT_PIN,&irq);
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
		printk("irq in config space is %d\n",irq);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
		
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_0,&val0);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_1,&val1);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_2,&val2);
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
		printk("before assign dev=%p val0=%x, val1=%x,val2=%x\n",dev,val0,val1,val2);
		printk("rs0=%x,rs1=%x,rs2=%x\n",dev->resource[0].start,
									    dev->resource[1].start,
										dev->resource[2].start);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
		
		has_io = has_mem = 0;
		for(i=0; i<6; i++) {
			unsigned long f = dev->resource[i].flags;
			
			pci_assign_resource(dev, i);

			if (f & IORESOURCE_IO) {
				has_io = 1;
			} else if (f & IORESOURCE_MEM)
				has_mem = 1;
		}
		
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_0,&val0);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_1,&val1);
		pci_read_config_dword(dev,PCI_BASE_ADDRESS_2,&val2);
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
		printk("after assign dev=%p val0=%x, val1=%x,val2=%x\n",dev,val0,val1,val2);
		printk("rs0=%x,rs1=%x,rs2=%x\n",dev->resource[0].start,
									    dev->resource[1].start,
										dev->resource[2].start);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
		
		pci_read_config_word(dev, PCI_COMMAND, &cmd);
		
		if (has_io && !(cmd & PCI_COMMAND_IO)) {
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
			printk("PCIC: Enabling I/O for device %02x:%02x\n",
				dev->bus->number, dev->devfn);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
			cmd |= PCI_COMMAND_IO;
			pci_write_config_word(dev, PCI_COMMAND, cmd);
		}
		if (has_mem && !(cmd & PCI_COMMAND_MEMORY)) {
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
			printk("PCIC: Enabling memory for device %02x:%02x\n",
				dev->bus->number, dev->devfn);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
			cmd |= PCI_COMMAND_MEMORY;
			pci_write_config_word(dev, PCI_COMMAND, cmd);
		}    
	}
}


/*
 * Other archs parse arguments here.
 */
char * __init pcibios_setup(char *str)
{
	return str;
}

/*
 */
void pcibios_update_resource(struct pci_dev *dev, struct resource *root,
			     struct resource *res, int resource)
{
	u32 new, check;
	int reg;

	new = res->start | (res->flags & PCI_REGION_FLAG_MASK);
	if (resource < 6) {
		reg = PCI_BASE_ADDRESS_0 + 4*resource;
	} else if (resource == PCI_ROM_RESOURCE) {
		res->flags |= PCI_ROM_ADDRESS_ENABLE;
		new |= PCI_ROM_ADDRESS_ENABLE;
		reg = dev->rom_base_reg;
	} else {
		/* Somebody might have asked allocation of a non-standard resource */
		return;
	}
    pci_write_config_dword(dev, reg, new);
    pci_read_config_dword(dev, reg, &check);
    if ((new ^ check) & ((new & PCI_BASE_ADDRESS_SPACE_IO) ? PCI_BASE_ADDRESS_IO_MASK : PCI_BASE_ADDRESS_MEM_MASK)) {
#ifdef BINZHANPORTING
		printk(KERN_ERR "PCI: Error while updating region "
						"%s/%d (%08x != %08x)\n", dev->pretty_name, resource,
						new, check);
#endif
	}
}

void pcibios_align_resource(void *data, struct resource *res,
		                unsigned long size, unsigned long align)
{
}

int pcibios_enable_device(struct pci_dev *pdev, int mask)
{
	    return 0;
}

/*
 * This probably belongs here rather than ioport.c because
 * we do not want this crud linked into SBus kernels.
 * Also, think for a moment about likes of floppy.c that
 * include architecture specific parts. They may want to redefine ins/outs.
 *
 * We do not use horroble macroses here because we want to
 * advance pointer by sizeof(size).
 */
void outsb(unsigned long addr, const void *src, unsigned long count) {
	while (count) {
		count -= 1;
		writeb(*(const char *)src, addr);
		src += 1;
		addr += 1;
	}
}

void outsw(unsigned long addr, const void *src, unsigned long count) {
	while (count) {
		count -= 2;
		writew(*(const short *)src, addr);
		src += 2;
		addr += 2;
	}
}

void outsl(unsigned long addr, const void *src, unsigned long count) {
	while (count) {
		count -= 4;
		writel(*(const long *)src, addr);
		src += 4;
		addr += 4;
	}
}

void insb(unsigned long addr, void *dst, unsigned long count) {
	while (count) {
		count -= 1;
		*(unsigned char *)dst = readb(addr);
		dst += 1;
		addr += 1;
	}
}

void insw(unsigned long addr, void *dst, unsigned long count) {
	while (count) {
		count -= 2;
		*(unsigned short *)dst = readw(addr);
		dst += 2;
		addr += 2;
	}
}

void insl(unsigned long addr, void *dst, unsigned long count) {
	while (count) {
		count -= 4;
		/*
		 * XXX I am sure we are in for an unaligned trap here.
		 */
		*(unsigned long *)dst = readl(addr);
		dst += 4;
		addr += 4;
	}
}

