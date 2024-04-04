/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <stdbool.h>
#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>
#include <libfdt.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_MARVELL_TAG) || \
    defined (CONFIG_REVISION_TAG)
static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

#if defined (CONFIG_MARVELL_TAG)
#ifdef MV88F78X60
extern void mvCpuIfMbusWindowSet(unsigned int base, unsigned int size);
#endif
extern void mvEgigaStrToMac( char *source , char *dest );
static void setup_marvell_tag(void);
#endif
static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

static ulong get_sp(void);
#if defined(CONFIG_OF_LIBFDT)
static int bootm_linux_fdt(int machid, bootm_headers_t *images);
#endif

#ifdef CONFIG_AMP_SUPPORT
extern int amp_enable;
extern int amp_group_id;
int  amp_boot(int mach_id, int load_addr, int param_addr);
void amp_wait_to_boot(void);
int  mv_amp_group_setup(int group_id, int load_addr);
#endif
#ifdef CONFIG_DB_78X60_AMC
extern int mvBoardIsBC2(void);
#endif

/* #define DEBUG_ATAG */
#ifdef DEBUG_ATAG
void printAtagParam(struct tag *pParams);
#define DB(x) x
#else
#define DB(x)
#endif /* DEBUG */

void arch_lmb_reserve(struct lmb *lmb)
{
	ulong sp;

	/*
	 * Booting a (Linux) kernel image
	 *
	 * Allocate space for command line and board info - the
	 * address should be as high as possible within the reach of
	 * the kernel (see CONFIG_SYS_BOOTMAPSZ settings), but in unused
	 * memory, which means far enough below the current stack
	 * pointer.
	 */
	sp = get_sp();
	debug("## Current stack ends at 0x%08lx ", sp);

	/* adjust sp by 1K to be safe */
	sp -= 1024;
	lmb_reserve(lmb, sp,
		    gd->bd->bi_dram[0].start + gd->bd->bi_dram[0].size - sp);
}

static void announce_and_cleanup(void)
{
	printf("\nStarting kernel ...\n\n");

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect(void);
		udc_disconnect();
	}
#endif
	cleanup_before_linux();
}

int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	bd_t	*bd = gd->bd;
	char	*s;
	int	machid = bd->bi_arch_number;
	void	(*kernel_entry)(int zero, int arch, uint params);

#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv ("bootargs");
#endif

	if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
		return 1;
#ifdef CONFIG_DB_78X60_AMC
	if (mvBoardIsBC2()) {
		machid = 3039; /* set to 3039 = armadaxp_amc which conforms to new uboot. */
		printf ("Using machid 0xbdf(3039) for amradaxp-amc for upward compat with new uboot\n");
	}
	else
#endif
	{
		s = getenv ("machid");
		if (s) {
			machid = simple_strtoul (s, NULL, 16);
			printf ("Using machid 0x%x from environment\n", machid);
		}
	}
	show_boot_progress (15);

#ifdef CONFIG_AMP_SUPPORT
	if(amp_enable){
		if(mv_amp_group_setup(amp_group_id, (int)images->ep)){
			return 1; // in case of error stop boot flow
		}
	}
#endif


#ifdef CONFIG_OF_LIBFDT
	if (images->ft_len)
		return bootm_linux_fdt(machid, images);
#endif

	kernel_entry = (void (*)(int, int, uint))images->ep;

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) kernel_entry);

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_MARVELL_TAG)
	setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag (&params);
#endif
#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
	if (images->rd_start && images->rd_end)
		setup_initrd_tag (bd, images->rd_start, images->rd_end);
#endif
#if defined (CONFIG_MARVELL_TAG)
        /* Linux open port doesn't support the Marvell TAG */
	char *env = getenv("mainlineLinux");
	if(!env || ((strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0)))
	    setup_marvell_tag ();
#endif
	setup_end_tag(bd);
#endif

#ifdef CONFIG_AMP_SUPPORT
	if(amp_enable){
		/* Boot AMP group. if boot completed (group_id > 0), return.*/
		if(amp_boot(machid, (int)kernel_entry, bd->bi_boot_params))
			return 0;
	}
#endif

	announce_and_cleanup();

#ifdef CONFIG_AMP_SUPPORT
	/*
	 * Function amp_wait_to_boot() decreases amp_barrier. If the amp_barrier
	 * reach 0, other cores will start and may change internal register base.
	 * Internal registers (console) cannot be written after this call.
	 */
	if(amp_enable)
		amp_wait_to_boot();
#endif
	#ifdef DEBUG_ATAG
	printAtagParam((struct tag *)bd->bi_boot_params);
	#endif
	kernel_entry(0, machid, bd->bi_boot_params);
	/* does not return */

	return 1;
}

#if defined(CONFIG_OF_LIBFDT)
int fixup_memory_node(void *blob)
{
#ifndef CONFIG_MARVELL
	bd_t	*bd = gd->bd;
#endif
	int bank;
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

#ifdef CONFIG_MARVELL
	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = gd->dram_hw_info[bank].start;
		size[bank] = gd->dram_hw_info[bank].size;
		if ((start[bank] + size[bank]) == _4G) {		/* Save 256M for IO at 4G-256M */
			size[bank] = ((_4G - _256M) - start[bank]);
			#if defined (CONFIG_MARVELL_TAG) && defined(MV88F78X60)
			mvCpuIfMbusWindowSet(size[bank], _256M);
			#endif
		}
	}
#else
	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = bd->bi_dram[bank].start;
		size[bank] = bd->bi_dram[bank].size;
	}
#endif

	return fdt_fixup_memory_banks(blob, start, size, CONFIG_NR_DRAM_BANKS);
}

static int bootm_linux_fdt(int machid, bootm_headers_t *images)
{
	ulong rd_len;
	void (*kernel_entry)(int zero, int dt_machid, void *dtblob);
	ulong of_size = images->ft_len;
	char **of_flat_tree = &images->ft_addr;
	ulong *initrd_start = &images->initrd_start;
	ulong *initrd_end = &images->initrd_end;
	struct lmb *lmb = &images->lmb;
	int ret;

	kernel_entry = (void (*)(int, int, void *))images->ep;

	boot_fdt_add_mem_rsv_regions(lmb, *of_flat_tree);

	rd_len = images->rd_end - images->rd_start;
	ret = boot_ramdisk_high(lmb, images->rd_start, rd_len,
				initrd_start, initrd_end);
	if (ret)
		return ret;

	ret = boot_relocate_fdt(lmb, of_flat_tree, &of_size);
	if (ret)
		return ret;

	debug("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) kernel_entry);

	/* Always act as if 'fdt_skip_update = yes' */
	if (true) {
		debug("\n   Skipping Device Tree update ('fdt_skip_update' = yes)\n");

		fdt_chosen(*of_flat_tree, 1);

		fixup_memory_node(*of_flat_tree);

		fdt_fixup_ethernet(*of_flat_tree);

		fdt_initrd(*of_flat_tree, *initrd_start, *initrd_end, 1);
#ifdef CONFIG_OF_BOARD_SETUP
		ft_board_setup(*of_flat_tree, gd->bd);
#endif
	}
	announce_and_cleanup();

	kernel_entry(0, machid, *of_flat_tree);
	/* does not return */

	return 1;
}
#endif

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_MARVELL_TAG)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;
	char *env = getenv("enaLPAE");
	int lpae_en;

	if (!env || (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0))
		lpae_en = 0;
	else
		lpae_en = 1;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {

		if (lpae_en) {
			uint64_t start, size;
			if (gd->dram_hw_info[i].size == 0ll)
				continue;
			params->hdr.tag = ATAG_MEM64;
			params->hdr.size = tag_size (tag_mem64);
			start = gd->dram_hw_info[i].start;
			size = gd->dram_hw_info[i].size;
			if ((start + size) == 0x100000000ll) {
				params->u.mem64.start = start;
				params->u.mem64.size = (0xF0000000ll - start);
				size = 0;
				params = tag_next (params);
				params->hdr.tag = ATAG_MEM64;
				params->hdr.size = tag_size (tag_mem64);
				#if defined (CONFIG_MARVELL_TAG) && defined(MV88F78X60)
				mvCpuIfMbusWindowSet(0xF0000000, _256M);
				#endif

			}
			if (size) {
				params->u.mem64.start = start;
				params->u.mem64.size = size;
				params = tag_next (params);
			}
		} else {
			u32 start, size;
			if (gd->dram_hw_info[i].size == 0x0ll)
				continue;
			if (gd->dram_hw_info[i].start >= 0x100000000ll)
				continue;
			params->hdr.tag = ATAG_MEM;
			params->hdr.size = tag_size (tag_mem32);
			start = (u32)gd->dram_hw_info[i].start;
			size = (u32)gd->dram_hw_info[i].size;
			if ((start - 1 + size) == 0xFFFFFFFF) {
				params->u.mem.start = start;
				params->u.mem.size = (0xF0000000 - start);
				size = 0;
				params = tag_next (params);
				params->hdr.tag = ATAG_MEM;
				params->hdr.size = tag_size (tag_mem32);
				#if defined (CONFIG_MARVELL_TAG) && defined(MV88F78X60)
				mvCpuIfMbusWindowSet(0xF0000000, _256M);
				#endif
			}

			if (size) {
				params->u.mem.start = start;
				params->u.mem.size = size;
		params = tag_next (params);
	}
}

	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}


#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */

#if defined(CONFIG_MARVELL_TAG)

extern unsigned int mvBoardIdGet(void);
extern void mvBoardModuleConfigGet(u32 *modConfig);

static void setup_marvell_tag (void)
{
	char *env;
	char temp[20];
	int i;
	unsigned int boardId;

	params->hdr.tag = ATAG_MARVELL;
	params->hdr.size = tag_size (tag_mv_uboot);

	params->u.mv_uboot.uboot_version = VER_NUM;
	if(strcmp(getenv("nandEcc"), "4bit") == 0)
		params->u.mv_uboot.nand_ecc = 4;
	else if(strcmp(getenv("nandEcc"), "1bit") == 0)
		params->u.mv_uboot.nand_ecc = 1;

	boardId = mvBoardIdGet();
	params->u.mv_uboot.uboot_version |= boardId;
	params->u.mv_uboot.tclk = CONFIG_SYS_TCLK;
	params->u.mv_uboot.sysclk = CONFIG_SYS_BUS_CLK;

#if defined(MV78XX0)
	/* Dual CPU Firmware load address */
	env = getenv("fw_image_base");
	if(env)
		params->u.mv_uboot.fw_image_base = simple_strtoul(env, NULL, 16);
	else
		params->u.mv_uboot.fw_image_base = 0;

	/* Dual CPU Firmware size */
	env = getenv("fw_image_size");
	if(env)
		params->u.mv_uboot.fw_image_size = simple_strtoul(env, NULL, 16);
	else
		params->u.mv_uboot.fw_image_size = 0;
#endif

#if defined(MV_INCLUDE_USB)
	extern unsigned int mvCtrlUsbMaxGet(void);

	for (i = 0 ; i < mvCtrlUsbMaxGet(); i++)
	{
		sprintf( temp, "usb%dMode", i);
		env = getenv(temp);
		if((!env) || (strcmp(env,"Host") == 0 ) || (strcmp(env,"host") == 0) )
			params->u.mv_uboot.isUsbHost |= (1 << i);
		else
			params->u.mv_uboot.isUsbHost &= ~(1 << i);
	}
#endif /*#if defined(MV_INCLUDE_USB)*/

#if defined(MV_INCLUDE_SATA) && defined (RD_88F6710)
	env = getenv("enaExtDisk");
	if((!env) || (strcmp(env,"no") == 0 ))
		params->u.mv_uboot.board_module_config &= ~(1 << 0);
	else
		params->u.mv_uboot.board_module_config |= (1 << 0);
#else
	params->u.mv_uboot.board_module_config =-1;
#endif /*#if defined(MV_INCLUDE_SATA) && defined (RD_88F6710)*/

#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
	extern unsigned int mvCtrlEthMaxPortGet(void);
	extern int mvMacStrToHex(const char* macStr, unsigned char* macHex);

	for (i = 0 ;i < 4;i++)
	{
		memset(params->u.mv_uboot.macAddr[i], 0, sizeof(params->u.mv_uboot.macAddr[i]));
		params->u.mv_uboot.mtu[i] = 0;
	}

	for (i = 0 ;i < mvCtrlEthMaxPortGet();i++)
	{
/* only on RD-6281-A egiga0 defined as eth1 */
#if defined (RD_88F6281A)
		sprintf( temp,(i==0 ? "eth1addr" : "ethaddr"));
#else
		sprintf( temp,(i ? "eth%daddr" : "ethaddr"), i);
#endif
		DB(printf("i=%d, temp=%s = ",i,temp));
#if defined(MV_KW2)
		if(i == 2)
			sprintf(temp, "mv_pon_addr");
#endif

		env = getenv(temp);
		if (env)
			mvMacStrToHex(env, (unsigned char*)params->u.mv_uboot.macAddr[i]);
		DB(printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
			   ((unsigned char)params->u.mv_uboot.macAddr[i][0]),((unsigned char)params->u.mv_uboot.macAddr[i][1]),
			   ((unsigned char)params->u.mv_uboot.macAddr[i][2]),((unsigned char)params->u.mv_uboot.macAddr[i][3]),
			   ((unsigned char)params->u.mv_uboot.macAddr[i][4]),((unsigned char)params->u.mv_uboot.macAddr[i][5])));

/* only on RD-6281-A egiga0 defined as eth1 */
#if defined (RD_88F6281A)
		sprintf( temp,(i==0 ? "eth1mtu" : "ethmtu"));
#else
		sprintf( temp,(i ? "eth%dmtu" : "ethmtu"), i);
#endif
		DB(printf("i=%d, temp=%s",i,temp));
		env = getenv(temp);
		if (env)
			params->u.mv_uboot.mtu[i] = simple_strtoul(env, NULL, 10);
		DB(printf("=%d\n", params->u.mv_uboot.mtu[i]));
	}
#endif /* (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

	params = tag_next (params);
}
#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#endif  /* CONFIG_REVISION_TAG */

static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

static ulong get_sp(void)
{
	ulong ret;

	asm("mov %0, sp" : "=r"(ret) : );
	return ret;
}

#ifdef DEBUG_ATAG
void printAtagParam(struct tag *pParams)
{
	bd_t *bd = gd->bd;
	int i;

	printf("arch_number = 0x%08lX\n",	bd->bi_arch_number);
	printf("boot_params = 0x%08lX\n",	(ulong)bd->bi_boot_params);
	printf("boot_params = 0x%08lX\n",	(ulong)pParams);
	do {

	switch (pParams->hdr.tag) {
	case ATAG_NONE: printf("ATAG_NONE: \n"); break;
	case ATAG_CORE: printf("ATAG_CORE: \n"); break;

	case ATAG_MEM: printf("ATAG_MEM : ");
		printf("mem.start=0x%x, mem.size=0x%x\n",
			   pParams->u.mem.start,pParams->u.mem.size);
	break;
	case ATAG_MEM64: printf("ATAG_MEM64: ");
			printf("mem64.start=0x%llx, mem64.size=0x%llx\n",
				   pParams->u.mem64.start,pParams->u.mem64.size);
			break;
	case ATAG_VIDEOTEXT: printf("ATAG_VIDEOTEXT: "); break;
	case ATAG_RAMDISK: printf("ATAG_RAMDISK: "); break;
	case ATAG_INITRD: printf("ATAG_INITRD: "); break;
	case ATAG_INITRD2: printf("ATAG_INITRD2: "); break;
	case ATAG_SERIAL: printf("ATAG_SERIAL: "); break;
	case ATAG_REVISION: printf("ATAG_REVISION: "); break;
	case ATAG_VIDEOLFB: printf("ATAG_VIDEOLFB: "); break;

	case ATAG_CMDLINE: printf("ATAG_CMDLINE: ");
		printf("cmdline=%s\n", pParams->u.cmdline.cmdline);
	break;

	case ATAG_ACORN: printf("ATAG_ACORN: "); break;
	case ATAG_MEMCLK: printf("ATAG_MEMCLK : "); break;
	case ATAG_MARVELL:
		printf("ATAG_MARVELL: \n");
		printf("uboot_version=%x, TClock =%dMHz, sysclk=%dMHz, isUsbHost=0x%x\n",
			   pParams->u.mv_uboot.uboot_version,
			   pParams->u.mv_uboot.tclk/1000000,
			   pParams->u.mv_uboot.sysclk/1000000,
			   pParams->u.mv_uboot.isUsbHost);
		printf("Mtu[0]=%d Mtu[1]=%d Mtu[2]=%d Mtu[3]=%d\n",
			   pParams->u.mv_uboot.mtu[0],
			   pParams->u.mv_uboot.mtu[1],
			   pParams->u.mv_uboot.mtu[2],
			   pParams->u.mv_uboot.mtu[3]);
		for (i = 0 ;i < MV_UBOOT_MAX_PORTS;i++){
			printf("macAddr[%d]=%02x:%02x:%02x:%02x:%02x:%02x\n", i,
				   ((unsigned char)pParams->u.mv_uboot.macAddr[i][0]),((unsigned char)pParams->u.mv_uboot.macAddr[i][1]),
				   ((unsigned char)pParams->u.mv_uboot.macAddr[i][2]),((unsigned char)pParams->u.mv_uboot.macAddr[i][3]),
				   ((unsigned char)pParams->u.mv_uboot.macAddr[i][4]),((unsigned char)pParams->u.mv_uboot.macAddr[i][5]));
		}
		printf("board_module_config=%x\n",pParams->u.mv_uboot.board_module_config);
		break;
	default:
		printf("unknown ATAG: 0x%x   *****************\n", pParams->hdr.tag);
		break;

	} /* end switch */
	pParams = tag_next (pParams);
	} while (pParams->hdr.tag != ATAG_NONE);
}
#endif

#ifdef CONFIG_CMD_BOOTZ

struct zimage_header {
	uint32_t	code[9];
	uint32_t	zi_magic;
	uint32_t	zi_start;
	uint32_t	zi_end;
};

#define	LINUX_ARM_ZIMAGE_MAGIC	0x016f2818

int bootz_setup(void *image, void **start, void **end)
{
	struct zimage_header *zi = (struct zimage_header *)image;

	if (zi->zi_magic != LINUX_ARM_ZIMAGE_MAGIC) {
		puts("Bad Linux ARM zImage magic!\n");
		return 1;
	}

	*start = (void *)zi->zi_start;
	*end = (void *)zi->zi_end;

	debug("Kernel image @ 0x%08x [ 0x%08x - 0x%08x ]\n",
		(uint32_t)image, (uint32_t)*start, (uint32_t)*end);

	return 0;
}
#endif	/* CONFIG_CMD_BOOTZ */
