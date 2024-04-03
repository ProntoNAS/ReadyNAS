/*  $Id: setup.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 *  linux/arch/sparc/kernel/setup.c
 *
 *  Copyright (C) 1995  David S. Miller (davem@caip.rutgers.edu)
 *  Copyright (C) 2000  Anton Blanchard (anton@samba.org)
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <asm/smp.h>
#include <linux/user.h>
#include <linux/a.out.h>
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/config.h>
#include <linux/fs.h>
#include <linux/seq_file.h> 
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/blkdev.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/console.h>
#include <linux/spinlock.h>
#include <linux/highmem.h>
#include <linux/bootmem.h>
#include <linux/root_dev.h>
#include <linux/initrd.h>

#include <linux/proc_fs.h>
#include <linux/sysctl.h>

#include <asm/segment.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/kgdb.h>
#include <asm/processor.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/traps.h>
#include <asm/kdebug.h>
#include <asm/cache.h>
#include <asm/cpudata.h>
#include <asm/setup.h>
#include <asm/padre_hw_io.h>

static char user_cmd[USER_CMD_SIZE];

DEFINE_PER_CPU(cpuinfo_sparc, __cpu_data) = { 0 };
int stop_a_enabled = 1;
int __dc3data padre_cachefix = 0;
padre_hwio_module_t padre_NSPIO;
extern unsigned int g_sys_type;
extern unsigned int g_sys_asicid;
static void dummy_func(void){}
struct led_ctrl_func padre_led_handler __dc3data = {
        (void (*)(int)) dummy_func,
        (void (*)(int, int)) dummy_func,
        (void (*)(int, int)) dummy_func,
        (void (*)(int, int)) dummy_func,
        (void (*)(int, int, int, int)) dummy_func,
        (void (*)(void)) dummy_func
};

int debug_var1;
int debug_var2;

void init_padre_NSPIO(padre_hwio_module_t *the_NSPIO)
{
        the_NSPIO->state        = 0;
        the_NSPIO->ssl          = 0;
        the_NSPIO->load_level   = 0;
        //padre_NSPIO.name;
        //padre_NSPIO.version;
        the_NSPIO->init         = NULL;
        the_NSPIO->level1       = NULL;
        the_NSPIO->level2       = NULL;

        the_NSPIO->ssl_opt      = NULL;
        the_NSPIO->ssl_dma      = NULL;
        the_NSPIO->ssl_cnt      = NULL;
        the_NSPIO->ssl_dbg      = NULL;
        the_NSPIO->ssl_n_s      = NULL;
        the_NSPIO->ssl_old      = NULL;
        the_NSPIO->ssl_add      = NULL;

        the_NSPIO->hwifs        = NULL;

        the_NSPIO->scan         = NULL;
        the_NSPIO->init_c       = NULL;
        the_NSPIO->init         = NULL;
	the_NSPIO->setback	= NULL;
        the_NSPIO->proc         = NULL;
        the_NSPIO->read         = NULL;
        the_NSPIO->write        = NULL;
        the_NSPIO->plug         = NULL;
        the_NSPIO->intr         = NULL;
        the_NSPIO->gintr        = NULL;
        the_NSPIO->flush        = NULL;
        the_NSPIO->chn_scan     = NULL;
        the_NSPIO->diskin       = NULL;
        the_NSPIO->diskout      = NULL;
        the_NSPIO->mport_proc   = NULL;
        the_NSPIO->mrxan_proc   = NULL;
        the_NSPIO->mdjnm_proc   = NULL;
        the_NSPIO->mlpnm_proc   = NULL;
        the_NSPIO->read_proc    = NULL;
        the_NSPIO->read_user_cmd        = NULL;
        the_NSPIO->write_user_cmd       = NULL;

        the_NSPIO->user_cmd       = user_cmd;
	the_NSPIO->LCD_display	= NULL;
}

static void init_padre_fifo(void)
{
#define WRITE_FIFO_CTRL(size,ch)		writel(PADRE_DMA_FFCTRL_EN|(size),PADRE_MC_DMA_FFCTRL(ch))
#define WRITE_FIFO_BASE(base,ch)		writel(base,PADRE_MC_DMA_BASE(ch))
	
	unsigned long fifo_in_base, fifo_in_size;
	unsigned long fifo_out_base, fifo_out_size;
	unsigned long i;
        unsigned long ide_dma_ch;
	unsigned char boardid = padre_boot_param.vpd_extend[0];

#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
        printk("PADRE: fifo init...\n");
#endif // CONFIG_PADRE_DEBUG_LEVEL_3

	// GMAC
	if(g_sys_type == SYSTEM_TYPE_PZERO)
	{
		fifo_in_base = fifo_out_base = 0x0c00;
		fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__512;
	}
	else
	{
		fifo_in_base = fifo_out_base = 0x1800;
                if(boardid == PADRE_BOARD_IT71008)
                        fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__512;
                else
                        fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__1K;
	}

	// PADRE_DMA_CH_GMAC0RX
	WRITE_FIFO_BASE(fifo_in_base,PADRE_DMA_CH_GMAC0RX);
	WRITE_FIFO_CTRL(fifo_in_size,PADRE_DMA_CH_GMAC0RX);

	// PADRE_DMA_CH_GMAC0TX
	WRITE_FIFO_BASE(fifo_out_base,PADRE_DMA_CH_GMAC0TX);
	WRITE_FIFO_CTRL(fifo_out_size,PADRE_DMA_CH_GMAC0TX);

        if(g_sys_type == SYSTEM_TYPE_PADRE && boardid == PADRE_BOARD_IT71008) {
                // PADRE_DMA_CH_GMAC1RX
                WRITE_FIFO_BASE(fifo_in_base+(1<<fifo_in_size),PADRE_DMA_CH_GMAC1RX);
                WRITE_FIFO_CTRL(fifo_in_size,PADRE_DMA_CH_GMAC1RX);

                // PADRE_DMA_CH_GMAC1TX
                WRITE_FIFO_BASE(fifo_out_base+(1<<fifo_out_size),PADRE_DMA_CH_GMAC1TX);
                WRITE_FIFO_CTRL(fifo_out_size,PADRE_DMA_CH_GMAC1TX);
        }

	// PADRE_DMA_CH_IDE0 to PADRE_DMA_CH_IDE7
	fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__512;
	fifo_in_base = 0x0400;

        if(g_sys_type == SYSTEM_TYPE_PZERO)
        {
                ide_dma_ch = PADRE_DMA_CH_IDE3;
        }
        else
        {
                if (padre_NSPIO.load_level)
                        ide_dma_ch = PADRE_DMA_CH_IDE3;
                else
                        ide_dma_ch = PADRE_DMA_CH_IDE7;
        }

        for( i = PADRE_DMA_CH_IDE0; i <= ide_dma_ch ; i++) {
                WRITE_FIFO_BASE(fifo_in_base,i);
                WRITE_FIFO_CTRL(fifo_in_size,i);
                fifo_in_base += 512;
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
                printk("Set ide_dma_ch i=%lu, base=%p, size=%p\n", i,
                        (void *)fifo_in_base, (void *)fifo_in_size);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
        }


        // PADRE_DMA_CH_RXAIN and PADRE_DMA_CH_RXAOUT
        fifo_out_base = fifo_in_base = 0;
        fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__512;
        WRITE_FIFO_BASE(fifo_in_base,PADRE_DMA_CH_RXAIN);
        WRITE_FIFO_CTRL(fifo_in_size,PADRE_DMA_CH_RXAIN);
        WRITE_FIFO_BASE(fifo_out_base,PADRE_DMA_CH_RXAOUT);
        WRITE_FIFO_CTRL(fifo_out_size,PADRE_DMA_CH_RXAOUT);

        if(g_sys_type == SYSTEM_TYPE_PZERO)
        {
                fifo_out_base = fifo_in_base = 0x0e00;
        }
        else
        {
                fifo_out_base = fifo_in_base = 0x1c00;
        }

        fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__512;
        WRITE_FIFO_BASE(fifo_in_base,PADRE_DMA_CH_AUIN);
        WRITE_FIFO_CTRL(fifo_in_size,PADRE_DMA_CH_AUIN);
        WRITE_FIFO_BASE(fifo_out_base,PADRE_DMA_CH_AUOUT);
        WRITE_FIFO_CTRL(fifo_out_size,PADRE_DMA_CH_AUOUT);

        if(g_sys_type == SYSTEM_TYPE_PZERO)
        {
                fifo_in_base = fifo_out_base = 0x0200;
        }
        else
        {
                fifo_in_base = fifo_out_base = 0x1400;
        }

        fifo_in_size = fifo_out_size = PADRE_DMA_FFCTRL_LFFS__512;
        WRITE_FIFO_BASE(fifo_in_base,PADRE_DMA_CH_CP0IN);
        WRITE_FIFO_CTRL(fifo_in_size,PADRE_DMA_CH_CP0IN);
        WRITE_FIFO_BASE(fifo_out_base,PADRE_DMA_CH_CP0OUT);
        WRITE_FIFO_CTRL(fifo_out_size,PADRE_DMA_CH_CP0OUT);
}

struct screen_info screen_info = {
	0, 0,			/* orig-x, orig-y */
	0,			/* unused */
	0,			/* orig-video-page */
	0,			/* orig-video-mode */
	128,			/* orig-video-cols */
	0,0,0,			/* ega_ax, ega_bx, ega_cx */
	54,			/* orig-video-lines */
	0,                      /* orig-video-isVGA */
	16                      /* orig-video-points */
};

/* Exported for mm/init.c:paging_init. */
unsigned long cmdline_memory_size __initdata = 0;


/* This routine will in the future do all the nasty prom stuff
 * to probe for the mmu type and its parameters, etc. This will
 * also be where SMP things happen plus the Sparc specific memory
 * physical memory probe as on the alpha.
 */

extern unsigned long start, end;
extern void panic_setup(char *, int *);

extern unsigned short root_flags;
extern unsigned short root_dev;
extern unsigned short ram_flags;

struct boot_param padre_boot_param;

#ifdef CONFIG_BLK_DEV_INITRD
#define COMMAND_LINE "root=/dev/ram0 init=/linuxrc rw raid=noautodetect profile=2"
#else
//#define COMMAND_LINE "root=/dev/hda1 rootflags=data=journal init=/linuxrc rw"
#define COMMAND_LINE "root=/dev/hda1 init=/linuxrc rw raid=noautodetect"
#endif
//#define COMMAND_LINE "root=/dev/hda1 init=/linuxrc rw profile=2"

#define RAMDISK_IMAGE_START_MASK	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000

extern int root_mountflags;
extern char _stext, _etext, _edata, _end;
static char command_line[COMMAND_LINE_SIZE];
extern char saved_command_line[];
char reboot_command[COMMAND_LINE_SIZE];

struct tt_entry *sparc_ttable;

struct pt_regs fake_swapper_regs;

#define UART1_PUTCHAR(ch)    do {                                           \
	                           while(!(readl(PADRE_UART1_STAT) & 0x4));    \
							   writel(ch,PADRE_UART1_DATA);                \
							} while(0)
#define UART0_PUTCHAR(ch)    do {                                           \
	                           while(!(readl(PADRE_UART0_STAT) & 0x4));    \
							   writel(ch,PADRE_UART0_DATA);                \
							} while(0)
static void
ser_console_write(struct console *con, const char *s, unsigned n)
{
	int i;
#ifndef CONFIG_PADRE_KCONSOLE1
	if( readl(PADRE_UART0_STAT) & 0x38) 
		writel(0,PADRE_UART0_STAT);
	for( i = 0 ; i < n ; i++) {
		if( s[i] == 10) 
			UART0_PUTCHAR(13);
		UART0_PUTCHAR(s[i]);
	}
#else
	if( readl(PADRE_UART1_STAT) & 0x38) 
		writel(0,PADRE_UART1_STAT);
	for( i = 0 ; i < n ; i++) {
		if( s[i] == 10) 
			UART1_PUTCHAR(13);
		UART1_PUTCHAR(s[i]);
	}
#endif
	
}

unsigned char 
uart_getchar_timeout(void)
{
  int i;
  unsigned char loop, ch;

  for( i=0 ; i<100; i++);
  i = 0;
  /* Solve the problem when serial cable not connected,
     a garbage character is received */
  for(loop=0; loop<3; loop++) {
    while( !(readl(PADRE_UART0_STAT) & 0x1)) {
        i++;
        if( i > 0x30000) return 0;
     }
     ch = readl(PADRE_UART0_DATA);
     if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) return ch;
  }
  return 0;
}

static void
ser_console_test(char c)
{

#ifndef CONFIG_PADRE_KCONSOLE1
        if( readl(PADRE_UART0_STAT) & 0x38)
                writel(0,PADRE_UART0_STAT);
        UART0_PUTCHAR(c);
#else
        if( readl(PADRE_UART1_STAT) & 0x38)
                writel(0,PADRE_UART1_STAT);
        UART1_PUTCHAR(c);
#endif

}

int gs_console_detect = 0;

static void
console_detect(void)
{
	ser_console_test(5);
	gs_console_detect = uart_getchar_timeout();
//	ser_console_test(gs_console_detect);
}

static struct console ser_console = {
	name:		"debug",
	write:		ser_console_write,
	flags:		CON_PRINTBUFFER,
	index:		-1,
};

extern void paging_init(void);

static inline void parse_mem_cmdline (char ** cmdline_p)
{
        /* Save unparsed command line copy for /proc/cmdline */
        memcpy(saved_command_line, COMMAND_LINE, COMMAND_LINE_SIZE);
        saved_command_line[COMMAND_LINE_SIZE-1] = '\0';
    *cmdline_p = command_line;
        strcpy(*cmdline_p,saved_command_line);
}

void __init dc3_ic3_init(void)
{
        __asm__ __volatile__(
                "lda [%1]%2, %0"
                : "=r"(dcmu_ctrl)
                : "r" (PADRE_DCMU_CTRL),"i"(ASI_DCMU));
        __asm__ __volatile__(
                "lda [%1]%2, %0"
                : "=r"(icmu_ctrl)
                : "r" (PADRE_ICMU_CTRL),"i"(ASI_ICMU));
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
        if( dcmu_ctrl & PADRE_DCMU_CTRL_EN) {
                printk("D-cache on ");
                if( dcmu_ctrl & PADRE_DCMU_CTRL_WT)
                        printk("write-through mode ");
                else
                        printk("write-back mode ");
        }

        if( dcmu_ctrl & PADRE_DCMU_CTRL_DC3)
                printk("D-ram on %x ",dcmu_ctrl & PADRE_DCMU_CTRL_DC3TAG);
        if( icmu_ctrl & PADRE_ICMU_CTRL_EN)
                printk("I-cache on ");
        if( icmu_ctrl & PADRE_ICMU_CTRL_IC3)
                printk("I-ram on %x ",icmu_ctrl & PADRE_ICMU_CTRL_IC3TAG);

        printk("\n");
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
}

static int __init get_mmu_control(void)
{
	int mmu_control;
	__asm__ __volatile__(
	          "set %1, %%l0\n\t"
			  "lda [%%l0]%2, %0\n\t"
			  : "=r"(mmu_control)
			  : "i"(PADRE_MMU_CONTROL),"i"(ASI_MMU));
	return mmu_control;
}

static unsigned long mmu_version;
static unsigned short lp_version;
static unsigned short ahbarb;
unsigned long cpu_id;

unsigned short fpga_date,fpga_month,fpga_year,fpga_build,fpga_config;
static unsigned short padre_switch;
unsigned long board_version;

static void __init set_ahbarb(void)
{
    unsigned long flags;

        ahbarb = CONFIG_PADRE_AHBARB;
        if( ahbarb != 0x0 && ahbarb != 0x1 && ahbarb != 0x3 && ahbarb != 0x7) {
#ifdef CONFIG_PADRE_DEBUG_LEVEL_1
                printk("AHB Arbitration setting is not correct, using default value\n");
#endif // CONFIG_PADRE_DEBUG_LEVEL_1
                ahbarb = 0;
        }
    local_irq_save(flags);
        writel(ahbarb << 16,PADRE_SYS_STRAP);
        local_irq_restore(flags);
}

static void __init get_fpga_version(void)
{
#ifdef CONFIG_PADRE_PASEO_FPGA
	u32 uart0_data, uart0_stat;
	uart0_data = readl(PADRE_UART0_DATA);
	uart0_stat = readl(PADRE_UART0_STAT);
	fpga_month = (uart0_data >> 24) & 0xff;
	fpga_date = (uart0_data >> 16) & 0xff;
	fpga_year = (uart0_stat >> 24) & 0xff;
	fpga_build = (uart0_stat >> 20) & 0xf;
	fpga_config = (uart0_stat >> 16) & 0xf;
	padre_switch = readl(X2_SWITCH) & 0xff;
#endif
}

static void __init semaphore_test(void)
{
	int i;
	u32 s0,s1,s2;
	int pass = 1;
	for( i = 0 ; i < 32 ; i++) {
		writel(0,PADRE_S3_UNLOCK(i));

		s0 = readl(PADRE_S3_LOCK(i));
		s1 = readl(PADRE_S3_LOCK(i));
		writel(0,PADRE_S3_UNLOCK(i));
		s2 = readl(PADRE_S3_LOCK(i));
		if( (s0 == 0) && (s1==8) && (s2==0))
			continue;
		pass = 0;
		break;
			
	}
	if( pass ) {
#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
		printk("Passing 32 semaphore testing\n");
#endif // CONFIG_PADRE_DEBUG_LEVEL_3
	}
	else {
#ifdef CONFIG_PADRE_DEBUG_LEVEL_1
		printk("Semaphore %d has problem %d %d %d\n",i,s0,s1,s2);
#endif // CONFIG_PADRE_DEBUG_LEVEL_1
	}
}

unsigned int g_sys_type __dc3data;
unsigned int g_sys_asicid;

static void init_system_type(void)
{
        unsigned int sysid = *(volatile unsigned long *)PADRE_SYS_ID;
        switch(sysid>>16)
        {
                case 0x1008:
                        g_sys_type = SYSTEM_TYPE_PADRE;
                        g_sys_asicid = ((sysid & 0x3000)==0x3000) ? 0x1008 : 0x1004;
                        printk("You system is PADRE.\n");
                        break;
                case 0x3107:
                        g_sys_type = SYSTEM_TYPE_PZERO;
                        g_sys_asicid = (sysid & 0x1000) ? 0x3107 : 0x3102;
                        printk("You system is PZERO.\n");
                        break;
                default:
                        printk("System type id is 0x%lx\n", (*(volatile unsigned long *)PADRE_SYS_ID)>>16);
                        g_sys_type = 0; //SYSTEM_TYPE_PADRE;
                        g_sys_asicid = 0x0;
                        break;
        }
        printk(KERN_INFO "ASIC=IT%04x\n", g_sys_asicid);
}

void __init setup_arch(char **cmdline_p)
{

	init_system_type();

	memcpy(&padre_boot_param,(void*)0x80000000,sizeof(struct boot_param));
/*
        if(g_sys_type == SYSTEM_TYPE_PZERO)
        {
                p0_boot_init(&padre_boot_param);
        }
        else
        {
	    	padre_boot_init(&padre_boot_param);
	}
*/

        init_padre_NSPIO(&padre_NSPIO);
	padre_NSPIO.load_level = padre_boot_param.product_features & 1;
#ifndef CONFIG_PADRE_CPU1
	set_ahbarb();
	lp_version = readl(PADRE_LP_VERSION) & 0xffff;
	semaphore_test();
	init_padre_fifo();
	get_fpga_version();	
#endif
	mmu_version = (get_mmu_control() >> 24) & 0xf;
	cpu_id = (get_mmu_control() >> 20) & 0xf;
	
	dc3_ic3_init();

#ifdef CONFIG_PCI
        if(g_sys_type == SYSTEM_TYPE_PZERO)
        {
		writel(0x10000,PADRE_SYS_CTRL);	// PCI Reset
		writel(0x302c,PADRE_PCI_FEATURE);
		writel(0x00000,PADRE_SYS_CTRL);	// PCI Un-Reset
	}
	else
	{
		writel(0x2e,PADRE_PCI_FEATURE);
	}
#endif

    register_console(&ser_console);  

#if 0 //JM: Temporarily disabled until the reboot issue is figured out.
	console_detect();
	if(gs_console_detect)
	{
		console_printk[0] = 7;
		console_printk[3] = 7;
	}
#else
		console_printk[0] = 7;
		console_printk[3] = 7;
#endif

#if 0
	ROOT_DEV = to_kdev_t(ORIG_ROOT_DEV);
#else
	ROOT_DEV = MKDEV(3,1);
#endif
//	drive_info = DRIVE_INFO;
#ifdef CONFIG_BLK_DEV_RAM
	rd_image_start = 0;
	rd_prompt = 0;
	rd_doload = 1;
	//rd_image_start = RAMDISK_FLAGS & RAMDISK_IMAGE_START_MASK;
	//rd_prompt = ((RAMDISK_FLAGS & RAMDISK_PROMPT_FLAG) != 0);
	//rd_doload = ((RAMDISK_FLAGS & RAMDISK_LOAD_FLAG) != 0);
#endif
	init_mm.start_code = (unsigned long) &_stext;
	init_mm.end_code = (unsigned long) &_etext;
	init_mm.end_data = (unsigned long) &_edata;
	init_mm.brk = (unsigned long) &_end;
	parse_mem_cmdline(cmdline_p);

	sparc_ttable = (struct tt_entry *) &start;

	init_task.thread.kregs = &fake_swapper_regs;

	paging_init();
}

/*
asmlinkage int sys_ioperm(unsigned long from, unsigned long num, int on)
{
	return -EIO;
}

asmlinkage int sys_pciconfig_read(unsigned long bus,unsigned long dfn,
						          unsigned long off,unsigned long len,
								  unsigned char *buf)
{
	return -EINVAL;
}
 
asmlinkage int sys_pciconfig_write(unsigned long bus,unsigned long dfn,
						           unsigned long off,unsigned long len,
								   unsigned char *buf)
{
	return -EINVAL;
}
*/

static int show_cpuinfo(struct seq_file *m, void *__unused)
{
        seq_printf(m, "cpu\t\t: Infrant Technologics, Inc. - neon version: %d\n"
                              "fpu\t\t: Softfpu\n"
                              "ncpus probed\t: 1\n"
                              "ncpus active\t: 1\n"
                              "BogoMips\t: %lu.%02lu\n"
                              "MMU\t\t: version: %lu\n"
                                  "LP\t\t: HW.FW version: %d.%d\n"
                                  "FPGA\t\t: fpga%02x%02x%02x-%x Configuration: %x\n"
                                  "AHB arbitraion\t: %x\n"
                                  "CPU id\t\t: %lu\n"
                                  "Switch\t\t: %x\n"
                                "ASIC\t\t: IT%04x\n",
                              (get_psr() >> 24) & 0xf,
                              loops_per_jiffy/(500000/HZ), (loops_per_jiffy/(5000/HZ)) % 100,
                              mmu_version,
                                  (lp_version & PADRE_LP_VERSION_HWVER) >> 12,
                                  lp_version & PADRE_LP_VERSION_FWVER,
                                  fpga_month,fpga_date,fpga_year,fpga_build,fpga_config,
                                  ahbarb,cpu_id,padre_switch,g_sys_asicid
                        );
        return 0;
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
    	return *pos == 0 ? &c_start : NULL;
}
 
static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}
 
static void c_stop(struct seq_file *m, void *v)
{
}
 
struct seq_operations cpuinfo_op = {
	start:  c_start,
	next:   c_next,
	stop:   c_stop,
	show:   show_cpuinfo,
};

void LCD_display(unsigned char * info)
{
        //if((padre_boot_param.boot_reason == PADRE_BOOT_KDEBUG) && padre_NSPIO.LCD_display)
        if(padre_NSPIO.LCD_display)
                padre_NSPIO.LCD_display(info);
}

