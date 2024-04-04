/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sysdev.h>
#include <linux/mbus.h>
#include <asm/mach/time.h>
#include <linux/clocksource.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <mach/system.h>

#include <linux/tty.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/ata_platform.h>
#include <asm/serial.h>
#include <plat/cache-aurora-l2.h>

#include <mach/serial.h>

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "mvDebug.h"
#include "mvSysHwConfig.h"
#include "pex/mvPexRegs.h"
#include "cntmr/mvCntmr.h"
#include "gpp/mvGpp.h"
#include "plat/gpio.h"
#include "cpu/mvCpu.h"

#if defined(CONFIG_MV_INCLUDE_SDIO)
#include "sdmmc/mvSdmmc.h"
#include <plat/mvsdio.h>
#endif
#if defined(CONFIG_MV_INCLUDE_CESA)
#include "cesa/mvCesa.h"
#endif

#include <plat/mv_xor.h>

/* I2C */
#include <linux/i2c.h>
#include <linux/mv643xx_i2c.h>
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"

/* SPI */
#include "mvSysSpiApi.h"

/* Eth Phy */
#include "mvSysEthPhyApi.h"

/* LCD */
#include <video/dovefb.h>
#include <video/dovefbreg.h>
#include <mach/dove_bl.h>

/* NAND */
#ifdef CONFIG_MTD_NAND_NFC
#include "mv_mtd/nand_nfc.h"
#endif

#define MV_COHERENCY_FABRIC_CTRL_REG		(MV_COHERENCY_FABRIC_OFFSET + 0x0)
#define MV_COHERENCY_FABRIC_CFG_REG		(MV_COHERENCY_FABRIC_OFFSET + 0x4)

#ifdef CONFIG_FB_DOVE
extern unsigned int lcd0_enable;
extern int lcd_panel;
#endif
extern unsigned int irq_int_type[];
extern void __init axp_map_io(void);
extern void __init mv_init_irq(void);
extern struct sys_timer axp_timer;
extern MV_CPU_DEC_WIN* mv_sys_map(void);
#if defined(CONFIG_MV_INCLUDE_CESA)
extern u32 mv_crypto_virt_base_get(u8 chan);
#endif
extern void axp_init_irq(void);

/* for debug putstr */
static char arr[256];
MV_U32 mvTclk = 166666667;
MV_U32 mvSysclk = 200000000;

#ifdef CONFIG_MV_INCLUDE_GIG_ETH
MV_U8 mvMacAddr[CONFIG_MV_ETH_PORTS_NUM][6];
MV_U16 mvMtu[CONFIG_MV_ETH_PORTS_NUM] = {0};
#endif

/*
 * Helpers to get DDR bank info
 */
#define DDR_BASE_CS_OFF(n)	(0x0000 + ((n) << 3))
#define DDR_SIZE_CS_OFF(n)	(0x0004 + ((n) << 3))
#define TARGET_DDR		0
#define COHERENCY_STATUS_SHARED_NO_L2_ALLOC	0x1

struct mbus_dram_target_info armadaxp_mbus_dram_info;

/* XOR0 is disabled in Z1 Silicone */
#undef XOR0_ENABLE


/*********************************************************************************/
/**************                 Early Printk Support                **************/
/*********************************************************************************/
#ifdef MV_INCLUDE_EARLY_PRINTK
#define MV_UART0_LSR 	(*(volatile unsigned char *)(INTER_REGS_BASE + 0x12000 + 0x14))
#define MV_UART0_THR	(*(volatile unsigned char *)(INTER_REGS_BASE + 0x12000 + 0x0 ))
#define MV_UART1_LSR    (*(volatile unsigned char *)(INTER_REGS_BASE + 0x12100 + 0x14))
#define MV_UART1_THR    (*(volatile unsigned char *)(INTER_REGS_BASE + 0x12100 + 0x0 ))
#define MV_SERIAL_BASE 	((unsigned char *)(INTER_REGS_BASE + 0x12000 + 0x0 ))
#define DEV_REG		(*(volatile unsigned int *)(INTER_REGS_BASE + 0x40000))
#define CLK_REG         (*(volatile unsigned int *)(INTER_REGS_BASE + 0x2011c))
/*
 * This does not append a newline
 */
static void putstr(const char *s)
{
	unsigned int model;

	/* Get dev ID, make sure pex clk is on */
	if((CLK_REG & 0x4) == 0)
	{
		CLK_REG = CLK_REG | 0x4;
		model = (DEV_REG >> 16) & 0xffff;
		CLK_REG = CLK_REG & ~0x4;
	}
	else
		model = (DEV_REG >> 16) & 0xffff;

        while (*s) {
		while ((MV_UART0_LSR & UART_LSR_THRE) == 0);
		MV_UART0_THR = *s;

                if (*s == '\n') {
                        while ((MV_UART0_LSR & UART_LSR_THRE) == 0);
                        MV_UART0_THR = '\r';
                }
                s++;
        }
}
extern void putstr(const char *ptr);
void mv_early_printk(char *fmt,...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(arr,fmt,args);
	va_end(args);
	putstr(arr);
}
#endif

/*********************************************************************************/
/**************               UBoot Tagging Parameters              **************/
/*********************************************************************************/
#ifdef CONFIG_BE8_ON_LE
#define read_tag(a)    le32_to_cpu(a)
#define read_mtu(a)    le16_to_cpu(a)
#else
#define read_tag(a)    a
#define read_mtu(a)    a
#endif

extern MV_U32 gBoardId;
extern unsigned int elf_hwcap;
extern u32 mvIsUsbHost;

static int __init parse_tag_mv_uboot(const struct tag *tag)
{
	unsigned int mvUbootVer = 0;
	int i = 0;

	printk("Using UBoot passing parameters structure\n");
	mvUbootVer = read_tag(tag->u.mv_uboot.uboot_version);
	mvIsUsbHost = read_tag(tag->u.mv_uboot.isUsbHost);
	gBoardId =  (mvUbootVer & 0xff);

#ifdef CONFIG_MV_INCLUDE_GIG_ETH
	for (i = 0; i < CONFIG_MV_ETH_PORTS_NUM; i++) {
#if defined (CONFIG_OVERRIDE_ETH_CMDLINE)
		memset(mvMacAddr[i], 0, 6);
		mvMtu[i] = 0;
#else
printk(">>>>>>>Tag MAC %02x:%02x:%02x:%02x:%02x:%02x\n", tag->u.mv_uboot.macAddr[i][5], tag->u.mv_uboot.macAddr[i][4],
	tag->u.mv_uboot.macAddr[i][3], tag->u.mv_uboot.macAddr[i][2], tag->u.mv_uboot.macAddr[i][1], tag->u.mv_uboot.macAddr[i][0]);
		memcpy(mvMacAddr[i], tag->u.mv_uboot.macAddr[i], 6);
		mvMtu[i] = read_mtu(tag->u.mv_uboot.mtu[i]);
#endif
	}
#endif

#ifdef CONFIG_MV_NAND
               /* get NAND ECC type(1-bit or 4-bit) */
	if ((mvUbootVer >> 8) >= 0x3040c)
		mv_nand_ecc = read_tag(tag->u.mv_uboot.nand_ecc);
	else
		mv_nand_ecc = 1; /* fallback to 1-bit ECC */
#endif
	return 0;
}

__tagtable(ATAG_MV_UBOOT, parse_tag_mv_uboot);

/*********************************************************************************/
/**************                Command Line Parameters              **************/
/*********************************************************************************/
#ifdef CONFIG_MV_INCLUDE_USB
#include "mvSysUsbApi.h"
/* Required to get the configuration string from the Kernel Command Line */
static char *usb0Mode = "host";
static char *usb1Mode = "host";
static char *usb2Mode = "host";
int mv_usb0_cmdline_config(char *s);
int mv_usb1_cmdline_config(char *s);
int mv_usb2_cmdline_config(char *s);
__setup("usb0Mode=", mv_usb0_cmdline_config);
__setup("usb1Mode=", mv_usb1_cmdline_config);
__setup("usb2Mode=", mv_usb2_cmdline_config);

int mv_usb0_cmdline_config(char *s)
{
    usb0Mode = s;
    return 1;
}

int mv_usb1_cmdline_config(char *s)
{
    usb1Mode = s;
    return 1;
}

int mv_usb2_cmdline_config(char *s)
{
    usb2Mode = s;
    return 1;
}
#endif

#ifdef CONFIG_CACHE_AURORA_L2
static int noL2 = 0;
static int __init noL2_setup(char *__unused)
{
     noL2 = 1;
     return 1;
}

__setup("noL2", noL2_setup);
#endif

#ifndef CONFIG_SHEEVA_ERRATA_ARM_CPU_4948
unsigned int l0_disable_flag = 0;		/* L0 Enabled by Default */
static int __init l0_disable_setup(char *__unused)
{
     l0_disable_flag = 1;
     return 1;
}

__setup("l0_disable", l0_disable_setup);
#endif

#ifndef CONFIG_SHEEVA_ERRATA_ARM_CPU_5315
unsigned int sp_enable_flag = 0;		/* SP Disabled by Default */
static int __init spec_prefesth_setup(char *__unused)
{
     sp_enable_flag = 1;
     return 1;
}

__setup("sp_enable", spec_prefesth_setup);
#endif

#ifdef CONFIG_JTAG_DEBUG
	MV_U32 support_wait_for_interrupt = 0x0;
#else
	MV_U32 support_wait_for_interrupt = 0x1;
#endif
static int __init noWFI_setup(char *__unused)
{
     support_wait_for_interrupt = 0;
     return 1;
}

__setup("noWFI", noWFI_setup);

MV_U32 support_Z1A_serdes_cfg = 0x0;
static int __init serdesZ1A_setup(char *__unused)
{
     printk("Supporting Z1A Serdes Configurations.\n");
     support_Z1A_serdes_cfg = 1;
     return 1;
}

__setup("Z1A", serdesZ1A_setup);

char *nfcConfig = NULL;
static int __init nfcConfig_setup(char *s)
{
	nfcConfig = s;
	return 1;
}
__setup("nfcConfig=", nfcConfig_setup);

void __init armadaxp_setup_cpu_mbus(void)
{
	void __iomem *addr;
	int i;
	int cs;
	u8	coherency_status = 0;
#if defined(CONFIG_AURORA_IO_CACHE_COHERENCY) && defined(CONFIG_CACHE_AURORA_L2)
	if (noL2 == 0)
		coherency_status = COHERENCY_STATUS_SHARED_NO_L2_ALLOC;
#endif

	/*
	 * Setup MBUS dram target info.
	 */
	armadaxp_mbus_dram_info.mbus_dram_target_id = TARGET_DDR;
	addr = (void __iomem *)DDR_WINDOW_CPU_BASE;

	for (i = 0, cs = 0; i < 4; i++) {
		u32 base = readl(addr + DDR_BASE_CS_OFF(i));
		u32 size = readl(addr + DDR_SIZE_CS_OFF(i));

		/*
		 * Chip select enabled?
		 */
		if (size & 1) {
			struct mbus_dram_window *w;

			w = &armadaxp_mbus_dram_info.cs[cs++];
			w->cs_index = i;
			w->mbus_attr = 0xf & ~(1 << i);
			w->mbus_attr |= coherency_status << 4;
			w->base = base & 0xff000000;
			w->size = (size | 0x00ffffff) + 1;
		}
	}
	armadaxp_mbus_dram_info.num_cs = cs;
}

/*********************************************************************************/
/**************               I/O Devices Platform Info             **************/
/*********************************************************************************/
/*************
 * I2C(TWSI) *
 *************/
static struct mv64xxx_i2c_pdata axp_i2c_pdata = {
       .freq_m         = 8, /* assumes 166 MHz TCLK */
       .freq_n         = 3,
       .timeout        = 1000, /* Default timeout of 1 second */
};

static struct resource axp_i2c_0_resources[] = {
	{
		.name   = "i2c base",
		.start  = INTER_REGS_PHYS_BASE + MV_TWSI_SLAVE_REGS_OFFSET(0),
		.end    = INTER_REGS_PHYS_BASE + MV_TWSI_SLAVE_REGS_OFFSET(0) + 0x20 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "i2c irq",
		.start  = IRQ_AURORA_I2C0,
		.end    = IRQ_AURORA_I2C0,
		.flags  = IORESOURCE_IRQ,
	},
};

#ifdef CONFIG_FB_DOVE
static struct resource axp_i2c_1_resources[] = {
	{
		.name   = "i2c base",
		.start  = INTER_REGS_PHYS_BASE + MV_TWSI_SLAVE_REGS_OFFSET(1),
		.end    = INTER_REGS_PHYS_BASE + MV_TWSI_SLAVE_REGS_OFFSET(1) + 0x20 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "i2c irq",
		.start  = IRQ_AURORA_I2C1,
		.end    = IRQ_AURORA_I2C1,
		.flags  = IORESOURCE_IRQ,
	},
};
#endif

static struct platform_device axp_i2c = {
	.name           = MV64XXX_I2C_CTLR_NAME,
	.id             = 0,
	.num_resources  = ARRAY_SIZE(axp_i2c_0_resources),
	.resource       = axp_i2c_0_resources,
	.dev            = {
		.platform_data = &axp_i2c_pdata,
	},
};

/**********
 * UART-0 *
 **********/
static struct plat_serial8250_port aurora_uart0_data[] = {
	{
		.mapbase	= (INTER_REGS_PHYS_BASE | MV_UART_REGS_OFFSET(0)),
		.membase	= (char *)(INTER_REGS_BASE | MV_UART_REGS_OFFSET(0)),
		.irq		= IRQ_AURORA_UART0,
#if 0
		.flags		= UPF_SKIP_TEST | UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_MEM,
#else
		.flags		= UPF_FIXED_TYPE | UPF_SKIP_TEST | UPF_BOOT_AUTOCONF,
		.iotype		= UPIO_DWAPB,
		.private_data	= (void *) (INTER_REGS_BASE | MV_UART_REGS_OFFSET(0) | 0x7C),
		.type		= PORT_16550A,
#endif
		.regshift	= 2,
		.uartclk	= 0,
	}, {
	},
};

static struct resource aurora_uart0_resources[] = {
	{
		.start		= (INTER_REGS_PHYS_BASE | MV_UART_REGS_OFFSET(0)),
		.end		= (INTER_REGS_PHYS_BASE | MV_UART_REGS_OFFSET(0)) + SZ_256 - 1,
		.flags		= IORESOURCE_MEM,
	}, {
		.start		= IRQ_AURORA_UART0,
		.end		= IRQ_AURORA_UART0,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device aurora_uart0 = {
	.name			= "serial8250",
	.id			= 0,
	.dev			= {
		.platform_data	= aurora_uart0_data,
	},
	.resource		= aurora_uart0_resources,
	.num_resources		= ARRAY_SIZE(aurora_uart0_resources),
};


void __init serial_initialize(void)
{
	aurora_uart0_data[0].uartclk = mvBoardTclkGet();
	platform_device_register(&aurora_uart0);
}

/********
 * SDIO *
 ********/
#if defined(CONFIG_MV_INCLUDE_SDIO)
static struct resource mvsdio_resources[] = {
	[0] = {
		.start	= INTER_REGS_PHYS_BASE + MV_SDMMC_REGS_OFFSET,
		.end	= INTER_REGS_PHYS_BASE + MV_SDMMC_REGS_OFFSET + SZ_1K -1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_AURORA_SDIO,
		.end	= IRQ_AURORA_SDIO,
		.flags	= IORESOURCE_IRQ,
	},

};

static u64 mvsdio_dmamask = 0xffffffffUL;

static struct mvsdio_platform_data mvsdio_data = {
	.gpio_write_protect	= 0,
	.gpio_card_detect	= 0,
	.dram			= NULL,
};

static struct platform_device mv_sdio_plat = {
	.name		= "mvsdio",
	.id		= -1,
	.dev		= {
		.dma_mask = &mvsdio_dmamask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data	= &mvsdio_data,
	},
	.num_resources	= ARRAY_SIZE(mvsdio_resources),
	.resource	= mvsdio_resources,
};
#endif /* #if defined(CONFIG_MV_INCLUDE_SDIO) */

/*******
 * GBE *
 *******/
#ifdef CONFIG_MV_ETHERNET
#if defined(CONFIG_MV_ETH_LEGACY)
static struct platform_device mv88fx_eth = {
	.name		= "mv88fx_eth",
	.id		= 0,
	.num_resources	= 0,
};
#elif defined(CONFIG_MV_ETH_NETA)
static struct platform_device mv88fx_neta = {
	.name		= "mv88fx_neta",
	.id		= 0,
	.num_resources	= 0,
};
#else
#error "Ethernet Mode is not defined (should be Legacy or NETA)"
#endif /* Ethernet mode: legacy or NETA */

static void __init eth_init(void)
{
#if defined(CONFIG_MV_ETH_LEGACY)
	platform_device_register(&mv88fx_eth);
#elif defined(CONFIG_MV_ETH_NETA)
	platform_device_register(&mv88fx_neta);
#endif /* Ethernet mode: legacy or NETA */
}

#endif /* CONFIG_MV_ETHERNET */

/*******
 * RTC *
 *******/
static struct resource axp_rtc_resource[] = {
	{
		.start	= INTER_REGS_PHYS_BASE + MV_RTC_REGS_OFFSET,
		.end	= INTER_REGS_PHYS_BASE + MV_RTC_REGS_OFFSET + 32 - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= IRQ_AURORA_RTC,
		.flags	= IORESOURCE_IRQ,
	}
};

static void __init rtc_init(void)
{
	platform_device_register_simple("rtc-mv", -1, axp_rtc_resource, 2);
}

/********
 * SATA *
 ********/
#ifdef CONFIG_SATA_MV
#define SATA_PHYS_BASE (INTER_REGS_PHYS_BASE | 0xA0000)
#define IRQ_DSMP_SATA IRQ_AURORA_SATA0

static struct mv_sata_platform_data dbdsmp_sata_data = {
	.n_ports	= 2,
};

static struct resource armadaxp_sata_resources[] = {
	{
		.name	= "sata base",
		.start	= SATA_PHYS_BASE,
		.end	= SATA_PHYS_BASE + 0x5000 - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.name	= "sata irq",
		.start	= IRQ_DSMP_SATA,
		.end	= IRQ_DSMP_SATA,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device armadaxp_sata = {
	.name		= "sata_mv",
	.id		= 0,
	.dev		= {
		.coherent_dma_mask	= 0xffffffff,
	},
	.num_resources	= ARRAY_SIZE(armadaxp_sata_resources),
	.resource	= armadaxp_sata_resources,
};

void __init armadaxp_sata_init(struct mv_sata_platform_data *sata_data)
{

	armadaxp_sata.dev.platform_data = sata_data;
	sata_data->dram = &armadaxp_mbus_dram_info;
	platform_device_register(&armadaxp_sata);
}
#endif
/*****************************************************************************
 * SoC hwmon Thermal Sensor
 ****************************************************************************/
void __init armadaxp_hwmon_init(void)
{
	platform_device_register_simple("axp-temp", 0, NULL, 0);
}

/*************
 * 7-Segment *
 *************/
static struct timer_list axp_db_timer;
static void axp_db_7seg_event(unsigned long data)
{
	static int count = 0;

	/* Update the 7 segment */
	mvBoardDebugLed(count);

	/* Incremnt count and arm the timer*/
	count = (count + 1) & 7;
	mod_timer(&axp_db_timer, jiffies + 1 * HZ);
}

static int __init axp_db_7seg_init(void)
{
	/* Create the 7segment timer */
	setup_timer(&axp_db_timer, axp_db_7seg_event, 0);

	/* Arm it expire in 1 second */
	mod_timer(&axp_db_timer, jiffies + 1 * HZ);

	return 0;
}
__initcall(axp_db_7seg_init);

#ifdef CONFIG_FB_DOVE
/*****************************************************************************
 * LCD
 ****************************************************************************/

/*
 * LCD HW output Red[0] to LDD[0] when set bit [19:16] of reg 0x190
 * to 0x0. Which means HW outputs BGR format default. All platforms
 * uses this controller should enable .panel_rbswap. Unless layout
 * design connects Blue[0] to LDD[0] instead.
 */
static struct dovefb_mach_info kw_lcd0_dmi = {
	.id_gfx			= "GFX Layer 0",
	.id_ovly		= "Video Layer 0",
	.pix_fmt		= PIX_FMT_RGB888PACK,
	.lcd_ref_clk		= 25000000,
#if defined(CONFIG_FB_DOVE_CLCD_DCONB_BYPASS0)
	.io_pin_allocation	= IOPAD_DUMB24,
	.panel_rgb_type		= DUMB24_RGB888_0,
#else
	.io_pin_allocation      = IOPAD_DUMB24,
	.panel_rgb_type         = DUMB24_RGB888_0,
#endif
	.panel_rgb_reverse_lanes = 0,
	.gpio_output_data	= 3,
	.gpio_output_mask	= 3,
	.ddc_polling_disable	= 1,
	.ddc_i2c_address	= 0x50,
	.ddc_i2c_adapter	= 0,
	.invert_composite_blank	= 0,
	.invert_pix_val_ena	= 0,
	.invert_pixclock	= 0,
	.invert_vsync		= 0,
	.invert_hsync		= 0,
	.panel_rbswap		= 1,
	.active			= 1,
	.lvds_info = {
		.lvds_24b_option = 1,
		.lvds_tick_drv = 2
	}
};

static struct dovefb_mach_info kw_lcd0_vid_dmi = {
	.id_ovly		= "Video Layer 0",
	.pix_fmt		= PIX_FMT_RGB888PACK,
	.io_pin_allocation	= IOPAD_DUMB24,
	.panel_rgb_type		= DUMB24_RGB888_0,
	.panel_rgb_reverse_lanes = 0,
	.gpio_output_data	= 3,
	.gpio_output_mask	= 3,
	.ddc_i2c_adapter	= -1,
	.invert_composite_blank	= 0,
	.invert_pix_val_ena	= 0,
	.invert_pixclock	= 0,
	.invert_vsync		= 0,
	.invert_hsync		= 0,
	.panel_rbswap		= 0,
	.active			= 1,
	.enable_lcd0		= 0,
};

/*****************************************************************************
 * BACKLIGHT
 ****************************************************************************/
static struct dovebl_platform_data dsmp_backlight_data = {
	.default_intensity = 0xa,
	.gpio_pm_control = 1,

	.lcd_start = LCD_PHYS_BASE,	/* lcd power control reg base. */
	.lcd_end = LCD_PHYS_BASE+0x1C8,	/* end of reg map. */
	.lcd_offset = LCD_SPU_DUMB_CTRL,/* register offset */
	.lcd_mapped = 0,		/* va = 0, pa = 1 */
	.lcd_mask = 0x0,		/* mask, bit[21] */
	.lcd_on = 0x0,			/* value to enable lcd power */
	.lcd_off = 0x0,			/* value to disable lcd power */

	.blpwr_start = LCD_PHYS_BASE, /* bl pwr ctrl reg base. */
	.blpwr_end = LCD_PHYS_BASE+0x1C8,/* end of reg map. */
	.blpwr_offset = LCD_SPU_DUMB_CTRL,/* register offset */
	.blpwr_mapped = 0,		/* pa = 0, va = 1 */
	.blpwr_mask = 0x0,		/* mask */
	.blpwr_on = 0x0,		/* value to enable bl power */
	.blpwr_off = 0x0,		/* value to disable bl power */

	.btn_start = LCD_PHYS_BASE, /* brightness control reg base. */
	.btn_end = LCD_PHYS_BASE+0x1C8,	/* end of reg map. */
	.btn_offset = LCD_CFG_GRA_PITCH,	/* register offset */
	.btn_mapped = 0,		/* pa = 0, va = 1 */
	.btn_mask = 0xF0000000,	/* mask */
	.btn_level = 15,	/* how many level can be configured. */
	.btn_min = 0x1,	/* min value */
	.btn_max = 0xF,	/* max value */
	.btn_inc = 0x1,	/* increment */
};

#endif /* CONFIG_FB_DOVE */

#ifdef CONFIG_MTD_NAND_NFC
/*****************************************************************************
 * NAND controller
 ****************************************************************************/
static struct resource axp_nfc_resources[] = {
	{
		.start  = INTER_REGS_BASE + MV_NFC_REGS_OFFSET,
		.end    = INTER_REGS_BASE + MV_NFC_REGS_OFFSET + 0x400 -1,
		.flags  = IORESOURCE_MEM,
	}
};


static struct mtd_partition nand_parts_info[] = {
	{
		.name		= "UBoot",
		.offset		= 0,
		.size		= 1 * SZ_1M
	},
	{
		.name		= "UImage",
		.offset	= MTDPART_OFS_APPEND,
		.size		= 4 * SZ_1M },
	{
		.name		= "Root",
		.offset	= MTDPART_OFS_APPEND,
		.size         = MTDPART_SIZ_FULL
	},
};


static struct nfc_platform_data axp_nfc_data = {
	.nfc_width	= 8,
	.num_devs	= 1,
	.num_cs		= 1,
	.use_dma	= 0,
	.ecc_type	= MV_NFC_ECC_BCH_2K,
	.parts		= nand_parts_info,
	.nr_parts	= ARRAY_SIZE(nand_parts_info),
};

static struct platform_device axp_nfc = {
	.name           = "armada-nand",
	.id             = 0,
	.dev            = {
							.platform_data = &axp_nfc_data,
						},
	.num_resources  = ARRAY_SIZE(axp_nfc_resources),
	.resource       = axp_nfc_resources,

};

static void __init axp_db_nfc_init(void)
{
	/* Check for ganaged mode */
	if (nfcConfig) {
		if (strncmp(nfcConfig, "ganged", 6) == 0) {
			axp_nfc_data.nfc_width = 16;
			axp_nfc_data.num_devs = 2;
			nfcConfig += 7;
		}

		/* Check for ECC type directive */
		if (strcmp(nfcConfig, "8bitecc") == 0) {
			axp_nfc_data.ecc_type = MV_NFC_ECC_BCH_1K;
		} else if (strcmp(nfcConfig, "12bitecc") == 0) {
			axp_nfc_data.ecc_type = MV_NFC_ECC_BCH_704B;
		} else if (strcmp(nfcConfig, "16bitecc") == 0) {
			axp_nfc_data.ecc_type = MV_NFC_ECC_BCH_512B;
		}
	}

	axp_nfc_data.tclk = mvBoardTclkGet();

	platform_device_register(&axp_nfc);
}
#endif
/*********************************************************************************/
/**************                      Helper Routines                **************/
/*********************************************************************************/
#ifdef CONFIG_MV_INCLUDE_CESA
unsigned char*  mv_sram_usage_get(int* sram_size_ptr)
{
	int used_size = 0;

#if defined(CONFIG_MV_CESA)
	used_size = sizeof(MV_CESA_SRAM_MAP);
#endif

	if(sram_size_ptr != NULL)
		*sram_size_ptr = _8K - used_size;

	return (char *)(mv_crypto_virt_base_get(0) + used_size);
}
#endif

void print_board_info(void)
{
	char name_buff[50];
	printk("\n");
	printk("  Marvell Armada-XP");

	mvBoardNameGet(name_buff);
	printk(" %s Board - ",name_buff);

	mvCtrlModelRevNameGet(name_buff);
	printk(" Soc: %s",  name_buff);
#if defined(MV_CPU_LE)
	printk(" LE\n");
#else
	printk(" BE\n");
#endif
	printk("  Detected Tclk %d, SysClk %d, FabricClk %d, PClk %d\n",mvTclk, mvSysclk, mvCpuL2ClkGet(), mvCpuPclkGet());
	printk("  LSP version: %s\n", LSP_VERSION);
	printk("\n");
}

#ifdef	CONFIG_AURORA_IO_CACHE_COHERENCY
static void io_coherency_init(void)
{
	MV_U32 reg;

	/* set CIB read snoop command to ReadUnique */
	reg = MV_REG_READ(MV_CIB_CTRL_CFG_REG);
	reg &= ~(7 << 16);
	reg |= (7 << 16);
	MV_REG_WRITE(MV_CIB_CTRL_CFG_REG, reg);

#ifndef CONFIG_SMP
        /* enable CPUs in SMP group on Fabric coherency */
	reg = MV_REG_READ(MV_COHERENCY_FABRIC_CTRL_REG);
	reg &= ~(0x3<<24);
	reg |= 1<<24;
	MV_REG_WRITE(MV_COHERENCY_FABRIC_CTRL_REG, reg);

	reg = MV_REG_READ(MV_COHERENCY_FABRIC_CFG_REG);
	reg &= ~(0x3<<24);
	reg |= 1<<24;
	MV_REG_WRITE(MV_COHERENCY_FABRIC_CFG_REG, reg);
#endif
}
#endif

#ifdef CONFIG_DEBUG_LL
extern void printascii(const char *);
static void check_cpu_mode(void)
{
                u32 cpu_id_code_ext;
                int cpu_mode = 0;
                asm volatile("mrc p15, 1, %0, c15, c12, 0": "=r"(cpu_id_code_ext));

                if (((cpu_id_code_ext >> 16) & 0xF) == 0x2)
                        cpu_mode = 6;
                else if (((cpu_id_code_ext >> 16) & 0xF) == 0x3)
                        cpu_mode = 7;
                else
                        pr_err("unknow cpu mode!!!\n");
#ifdef CONFIG_DEBUGGER_MODE_V6
		if (cpu_mode != 6) {
			printascii("cpu mode (ARMv7) doesn't mach kernel configuration\n");
			panic("cpu mode mismatch");
		}
#else
#ifdef CONFIG_CPU_V7
                if (cpu_mode != 7) {
                        printascii("cpu mode (ARMv6) doesn't mach kernel configuration\n");
                        panic("cpu mode mismatch");
                }
#endif
#endif
	printk("Aurora: Working in ARMv%d mode\n",cpu_mode);
}
#endif

/*****************************************************************************
 * XOR
 ****************************************************************************/
static struct mv_xor_platform_shared_data armadaxp_xor_shared_data = {
	.dram		= &armadaxp_mbus_dram_info,
};

static u64 armadaxp_xor_dmamask = DMA_BIT_MASK(32);

/*****************************************************************************
 * XOR0
 ****************************************************************************/
#ifdef XOR0_ENABLE
static struct resource armadaxp_xor0_shared_resources[] = {
	{
		.name	= "xor 0 low",
		.start	= XOR0_PHYS_BASE,
		.end	= XOR0_PHYS_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	}, {
		.name	= "xor 0 high",
		.start	= XOR0_HIGH_PHYS_BASE,
		.end	= XOR0_HIGH_PHYS_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device armadaxp_xor0_shared = {
	.name		= MV_XOR_SHARED_NAME,
	.id		= 0,
	.dev		= {
		.platform_data = &armadaxp_xor_shared_data,
	},
	.num_resources	= ARRAY_SIZE(armadaxp_xor0_shared_resources),
	.resource	= armadaxp_xor0_shared_resources,
};

static struct resource armadaxp_xor00_resources[] = {
	[0] = {
		.start	= IRQ_AURORA_XOR0,
		.end	= IRQ_AURORA_XOR0,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct mv_xor_platform_data armadaxp_xor00_data = {
	.shared		= &armadaxp_xor0_shared,
	.hw_id		= 0,
	.pool_size	= PAGE_SIZE,
};

static struct platform_device armadaxp_xor00_channel = {
	.name		= MV_XOR_NAME,
	.id		= 0,
	.num_resources	= ARRAY_SIZE(armadaxp_xor00_resources),
	.resource	= armadaxp_xor00_resources,
	.dev		= {
		.dma_mask		= &armadaxp_xor_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &armadaxp_xor00_data,
	},
};

static struct resource armadaxp_xor01_resources[] = {
	[0] = {
		.start	= IRQ_AURORA_XOR1,
		.end	= IRQ_AURORA_XOR1,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct mv_xor_platform_data armadaxp_xor01_data = {
	.shared		= &armadaxp_xor0_shared,
	.hw_id		= 1,
	.pool_size	= PAGE_SIZE,
};

static struct platform_device armadaxp_xor01_channel = {
	.name		= MV_XOR_NAME,
	.id		= 1,
	.num_resources	= ARRAY_SIZE(armadaxp_xor01_resources),
	.resource	= armadaxp_xor01_resources,
	.dev		= {
		.dma_mask		= &armadaxp_xor_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &armadaxp_xor01_data,
	},
};

static void __init armadaxp_xor0_init(void)
{
	platform_device_register(&armadaxp_xor0_shared);

	/*
	 * two engines can't do memset simultaneously, this limitation
	 * satisfied by removing memset support from one of the engines.
	 */
	dma_cap_set(DMA_MEMCPY, armadaxp_xor00_data.cap_mask);
	dma_cap_set(DMA_XOR, armadaxp_xor00_data.cap_mask);
	platform_device_register(&armadaxp_xor00_channel);

	dma_cap_set(DMA_MEMCPY, armadaxp_xor01_data.cap_mask);
	dma_cap_set(DMA_MEMSET, armadaxp_xor01_data.cap_mask);
	dma_cap_set(DMA_XOR, armadaxp_xor01_data.cap_mask);
	platform_device_register(&armadaxp_xor01_channel);
}
#endif

/*****************************************************************************
 * XOR1
 ****************************************************************************/
static struct resource armadaxp_xor1_shared_resources[] = {
	{
		.name	= "xor 1 low",
		.start	= XOR1_PHYS_BASE,
		.end	= XOR1_PHYS_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	}, {
		.name	= "xor 1 high",
		.start	= XOR1_HIGH_PHYS_BASE,
		.end	= XOR1_HIGH_PHYS_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device armadaxp_xor1_shared = {
	.name		= MV_XOR_SHARED_NAME,
	.id		= 1,
	.dev		= {
		.platform_data = &armadaxp_xor_shared_data,
	},
	.num_resources	= ARRAY_SIZE(armadaxp_xor1_shared_resources),
	.resource	= armadaxp_xor1_shared_resources,
};

static struct resource armadaxp_xor10_resources[] = {
	[0] = {
		.start	= IRQ_AURORA_XOR0,
		.end	= IRQ_AURORA_XOR0,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct mv_xor_platform_data armadaxp_xor10_data = {
	.shared		= &armadaxp_xor1_shared,
	.hw_id		= 0,
	.pool_size	= PAGE_SIZE,
};

static struct platform_device armadaxp_xor10_channel = {
	.name		= MV_XOR_NAME,
	.id		= 2,
	.num_resources	= ARRAY_SIZE(armadaxp_xor10_resources),
	.resource	= armadaxp_xor10_resources,
	.dev		= {
		.dma_mask		= &armadaxp_xor_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &armadaxp_xor10_data,
	},
};

static struct resource armadaxp_xor11_resources[] = {
	[0] = {
		.start	= IRQ_AURORA_XOR1,
		.end	= IRQ_AURORA_XOR1,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct mv_xor_platform_data armadaxp_xor11_data = {
	.shared		= &armadaxp_xor1_shared,
	.hw_id		= 1,
	.pool_size	= PAGE_SIZE,
};

static struct platform_device armadaxp_xor11_channel = {
	.name		= MV_XOR_NAME,
	.id		= 3,
	.num_resources	= ARRAY_SIZE(armadaxp_xor11_resources),
	.resource	= armadaxp_xor11_resources,
	.dev		= {
		.dma_mask		= &armadaxp_xor_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &armadaxp_xor11_data,
	},
};

static void __init armadaxp_xor1_init(void)
{
	platform_device_register(&armadaxp_xor1_shared);

	/*
	 * two engines can't do memset simultaneously, this limitation
	 * satisfied by removing memset support from one of the engines.
	 */
	dma_cap_set(DMA_MEMCPY, armadaxp_xor10_data.cap_mask);
	dma_cap_set(DMA_MEMSET, armadaxp_xor11_data.cap_mask);
	/* removed due to better performance
	 * dma_cap_set(DMA_XOR, armadaxp_xor10_data.cap_mask); */
	platform_device_register(&armadaxp_xor10_channel);

	/* removed due to better performance
	 * dma_cap_set(DMA_MEMCPY, armadaxp_xor11_data.cap_mask); */
	dma_cap_set(DMA_XOR, armadaxp_xor11_data.cap_mask);
	platform_device_register(&armadaxp_xor11_channel);
}

static void cpu_fabric_common_init(void)
{
	MV_U32	reg;

#ifdef CONFIG_DEBUG_LL
        check_cpu_mode();
#endif

#ifdef CONFIG_SHEEVA_ERRATA_ARM_CPU_4948
	printk("L0 cache Disabled (by Errata #4948)\n");
#else
	__asm volatile ("mrc p15, 1, %0, c15, c1, 0" : "=r" (reg));
	if (l0_disable_flag) {
		printk("L0 cache Disabled\n");
		reg |= (1 << 0);
	} else {
		printk("L0 cache Enabled\n");
		reg &= ~(1 << 0);
	}
	__asm volatile ("mcr p15, 1, %0, c15, c1, 0" : : "r" (reg));
#endif

#ifdef CONFIG_SHEEVA_ERRATA_ARM_CPU_5315
	printk("Speculative Prefetch Disabled (by Errata #5315)\n");
#else
	__asm volatile ("mrc p15, 1, %0, c15, c2, 0" : "=r" (reg));
	if (sp_enable_flag) {
		printk("Speculative Prefetch Enabled\n");
		reg &= ~(1 << 7);
	} else {
		printk("Speculative Prefetch Disabled\n");
		reg |= (1 << 7);
	}
	__asm volatile ("mcr p15, 1, %0, c15, c2, 0" : : "r" (reg));
#endif

#ifdef CONFIG_CACHE_AURORA_L2
	if (!noL2)
		aurora_l2_init((void __iomem *)(INTER_REGS_BASE + MV_AURORA_L2_REGS_OFFSET));
#endif

#ifdef	CONFIG_AURORA_IO_CACHE_COHERENCY
	printk("Support IO coherency.\n");
	io_coherency_init();
#endif
}

/*****************************************************************************
 * DB BOARD: Main Initialization
 ****************************************************************************/
static void __init axp_db_init(void)
{
	/* Call Aurora/cpu special configurations */
	cpu_fabric_common_init();

	/* Select appropriate Board ID for Machine */
	gBoardId = DB_88F78XX0_BP_ID;

	/* Before initializing the HAL, select Z1A serdes cfg if needed */
	if (support_Z1A_serdes_cfg)
		mvBoardSerdesZ1ASupport();

	/* init the Board environment */
	mvBoardEnvInit();

	/* init the controller environment */
	if( mvCtrlEnvInit() ) {
		printk( "Controller env initialization failed.\n" );
		return;
	}

	armadaxp_setup_cpu_mbus();

	/* Init the CPU windows setting and the access protection windows. */
	if( mvCpuIfInit(mv_sys_map())) {
		printk( "Cpu Interface initialization failed.\n" );
		return;
	}

	/* Init Tclk & SysClk */
	mvTclk = mvBoardTclkGet();
	mvSysclk = mvBoardSysClkGet();

	elf_hwcap &= ~HWCAP_JAVA;

	serial_initialize();

	/* At this point, the CPU windows are configured according to default definitions in mvSysHwConfig.h */
	/* and cpuAddrWinMap table in mvCpuIf.c. Now it's time to change defaults for each platform.         */
	/*mvCpuIfAddDecShow();*/

	print_board_info();

	mv_gpio_init();

	/* RTC */
	rtc_init();

	/* SPI */
	mvSysSpiInit(0, _16M);

	/* ETH-PHY */
	mvSysEthPhyInit();

	/* Sata */
#ifdef CONFIG_SATA_MV
	armadaxp_sata_init(&dbdsmp_sata_data);
#endif
#ifdef CONFIG_MTD_NAND_NFC
	/* NAND */
	axp_db_nfc_init();
#endif
	/* HWMON */
	armadaxp_hwmon_init();

	/* XOR */
#ifdef XOR0_ENABLE
	armadaxp_xor0_init();
#endif
	armadaxp_xor1_init();

	/* I2C */
#ifdef CONFIG_FB_DOVE
	if ((lcd0_enable == 1) && (lcd_panel == 0))
		axp_i2c.resource = axp_i2c_1_resources;
#endif
	platform_device_register(&axp_i2c);

#if defined(CONFIG_MV_INCLUDE_SDIO)
	if (MV_TRUE == mvCtrlPwrClckGet(SDIO_UNIT_ID, 0)) {
		int irq_detect = mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_DETECT);
		MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];

		if (irq_detect != MV_ERROR) {
			mvsdio_data.gpio_card_detect = mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_DETECT);
			irq_int_type[mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_DETECT)+IRQ_AURORA_GPIO_START] = GPP_IRQ_TYPE_CHANGE_LEVEL;
		}

		if(mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_WP) != MV_ERROR)
			mvsdio_data.gpio_write_protect = mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_WP);

		if(MV_OK == mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1))
			if (MV_OK == mvSdmmcWinInit(addrWinMap))
				mvsdio_data.clock = mvBoardTclkGet();
		platform_device_register(&mv_sdio_plat);
       }
#endif

#ifdef CONFIG_MV_ETHERNET
	/* Ethernet */
	eth_init();
#endif

#ifdef CONFIG_FB_DOVE
	kw_lcd0_dmi.dram = &armadaxp_mbus_dram_info;
	if (lcd_panel) {
		kw_lcd0_dmi.lvds_info.enabled = 1;
		kw_lcd0_dmi.fixed_full_div = 1;
		kw_lcd0_dmi.full_div_val = 7;
		printk(KERN_INFO "LCD Panel enabled.\n");
	}
	clcd_platform_init(&kw_lcd0_dmi, &kw_lcd0_vid_dmi, &dsmp_backlight_data);
#endif

	return;
}

MACHINE_START(ARMADA_XP_DB, "Marvell Armada XP Development Board")
	/* MAINTAINER("MARVELL") */
	.phys_io	= INTER_REGS_PHYS_BASE,
	.io_pg_offst	= ((INTER_REGS_BASE) >> 18) & 0xfffc,
	.boot_params	= 0x00000100,
	.map_io		= axp_map_io,
	.init_irq	= axp_init_irq,
	.timer		= &axp_timer,
	.init_machine	= axp_db_init,
MACHINE_END


/*****************************************************************************
* RDSRV BOARD: Main Initialization
 ****************************************************************************/
static void __init axp_rdsrv_init(void)
{
	/* Call Aurora/cpu special configurations */
	cpu_fabric_common_init();

	/* Select appropriate Board ID for Machine */
	gBoardId = RD_78460_SERVER_ID;

	/* init the Board environment */
	mvBoardEnvInit();

	/* init the controller environment */
	if( mvCtrlEnvInit() ) {
		printk( "Controller env initialization failed.\n" );
		return;
	}

	armadaxp_setup_cpu_mbus();

	/* Init the CPU windows setting and the access protection windows. */
	if( mvCpuIfInit(mv_sys_map())) {
		printk( "Cpu Interface initialization failed.\n" );
		return;
	}

	/* Init Tclk & SysClk */
	mvTclk = mvBoardTclkGet();
	mvSysclk = mvBoardSysClkGet();

	elf_hwcap &= ~HWCAP_JAVA;

	serial_initialize();

	/* At this point, the CPU windows are configured according to default definitions in mvSysHwConfig.h */
	/* and cpuAddrWinMap table in mvCpuIf.c. Now it's time to change defaults for each platform.         */
	/*mvCpuIfAddDecShow();*/

	print_board_info();

	mv_gpio_init();

	/* RTC */
	rtc_init();

	/* SPI */
	mvSysSpiInit(0, _16M);

	/* ETH-PHY */
	mvSysEthPhyInit();

	/* Sata */
#ifdef CONFIG_SATA_MV
	armadaxp_sata_init(&dbdsmp_sata_data);
#endif

	/* HWMON */
	armadaxp_hwmon_init();

	/* I2C */
	platform_device_register(&axp_i2c);

#if defined(CONFIG_MV_INCLUDE_SDIO)
	if (MV_TRUE == mvCtrlPwrClckGet(SDIO_UNIT_ID, 0)) {
		int irq_detect = mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_DETECT);
		MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];

		if (irq_detect != MV_ERROR) {
			mvsdio_data.gpio_card_detect = mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_DETECT);
			irq_int_type[mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_DETECT)+IRQ_AURORA_GPIO_START] = GPP_IRQ_TYPE_CHANGE_LEVEL;
		}

		if(mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_WP) != MV_ERROR)
			mvsdio_data.gpio_write_protect = mvBoardSDIOGpioPinGet(BOARD_GPP_SDIO_WP);

		if(MV_OK == mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1))
			if (MV_OK == mvSdmmcWinInit(addrWinMap))
				mvsdio_data.clock = mvBoardTclkGet();
		platform_device_register(&mv_sdio_plat);
	}
#endif

#ifdef CONFIG_MV_ETHERNET
	/* Ethernet */
	eth_init();
#endif

	return;
}

MACHINE_START(ARMADA_XP_RDSRV, "Marvell Armada XP Server Board")
		/* MAINTAINER("MARVELL") */
		.phys_io	= INTER_REGS_PHYS_BASE,
  .io_pg_offst	= ((INTER_REGS_BASE) >> 18) & 0xfffc,
					.boot_params	= 0x00000100,
	 .map_io		= axp_map_io,
  .init_irq	= axp_init_irq,
  .timer		= &axp_timer,
  .init_machine	= axp_rdsrv_init,
  MACHINE_END

/*****************************************************************************
 * FPGA BOARD: Main Initialization
 ****************************************************************************/
extern MV_TARGET_ATTRIB mvTargetDefaultsArray[];
static void __init axp_fpga_init(void)
{
	/* Call Aurora/cpu special configurations */
	cpu_fabric_common_init();

	/* Select appropriate Board ID for Machine */
	gBoardId = FPGA_88F78XX0_ID;

        /* init the Board environment */
	mvBoardEnvInit();

        /* init the controller environment */
        if( mvCtrlEnvInit() ) {
            printk( "Controller env initialization failed.\n" );
            return;
        }

	/* Replace PCI-0 Attribute for FPGA 0xE => 0xD */
	mvTargetDefaultsArray[PEX0_MEM].attrib = 0xD8;

	/* Init the CPU windows setting and the access protection windows. */
	/*if( mvCpuIfInit(mv_sys_map())) {
		printk( "Cpu Interface initialization failed.\n" );
		return;
	}*/

	armadaxp_setup_cpu_mbus();

	/* Init the CPU windows setting and the access protection windows. */
	if( mvCpuIfInit(mv_sys_map())) {
		printk( "Cpu Interface initialization failed.\n" );
		return;
	}

	/* Init Tclk & SysClk */
	mvTclk = mvBoardTclkGet();
	mvSysclk = mvBoardSysClkGet();

	elf_hwcap &= ~HWCAP_JAVA;

	serial_initialize();

	/* At this point, the CPU windows are configured according to default definitions in mvSysHwConfig.h */
	/* and cpuAddrWinMap table in mvCpuIf.c. Now it's time to change defaults for each platform.         */
	/*mvCpuIfAddDecShow();*/

	print_board_info();

	mv_gpio_init();

	/* RTC */
	rtc_init();

	return;
}

MACHINE_START(ARMADA_XP_FPGA, "Marvell Armada XP FPGA Board")
	/* MAINTAINER("MARVELL") */
	.phys_io	= INTER_REGS_PHYS_BASE,
	.io_pg_offst	= ((INTER_REGS_BASE) >> 18) & 0xfffc,
	.boot_params	= 0x00000100,
	.map_io		= axp_map_io,
	.init_irq	= axp_init_irq,
	.timer		= &axp_timer,
	.init_machine	= axp_fpga_init,
MACHINE_END
