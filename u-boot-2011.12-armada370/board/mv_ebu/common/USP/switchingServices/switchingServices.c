/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

#include <common.h>
#include <config.h>
#include <command.h>
#include <pci.h>
#include <net.h>
#include <spi_flash.h>
#include <bzlib.h>

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"

#if defined(MV_INC_BOARD_NOR_FLASH)
		#include "norflash/mvFlash.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
		#include "eth-phy/mvEthPhy.h"
#endif

#if defined(MV_INCLUDE_PEX)
		#include "pex/mvPex.h"
#endif

#if defined(MV_INCLUDE_PDMA)
		#include "pdma/mvPdma.h"
		#include "mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_XOR)
		#include "xor/mvXorRegs.h"
		#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_PMU)
		#include "pmu/mvPmuRegs.h"
#endif

#include "cntmr/mvCntmrRegs.h"
#include "switchingServices.h"

int msys_spi_regs;

void _run_command(char *str)
{
	printf("%s\n", str);
	run_command(str, 0);
}


static void mv_print_appl_partitions(void)
{
		run_command("sf probe 0", 0);
		printf("\n");
		printf("%s partitions on spi flash\n", CFG_APPL_SPI_FLASH_PARTITIONS_NAME);
		printf("--------------------------------------\n\n");
		printf("Spi flash Address (rx)   : 0x%08x\n", CFG_APPL_SPI_FLASH_START_DIRECT);
		printf("Spi flash size           : %dMB\n", CFG_APPL_SPI_FLASH_SIZE/(1<<20));
		printf("u-boot               : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_UBOOT_START,
			   CFG_APPL_SPI_FLASH_PART_UBOOT_SIZE,
			   CFG_APPL_SPI_FLASH_PART_UBOOT_SIZE/(1<<10));

		printf("kernel/vxWorks-image : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE/(1<<10));

		printf("Linux rootfs         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
			   CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE/(1<<10));

		printf("\n");
		printf("Single Image         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START,
			   CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE,
			   (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)/(1<<10));

		run_command("nand info", 0);
		printf("\n");
		printf("%s partitions on nand flash\n", CFG_APPL_SPI_FLASH_PARTITIONS_NAME);
		printf("--------------------------------------\n\n");
		printf("Nand flash size           : %dMB\n", CFG_APPL_NAND_FLASH_SIZE/(1<<20));

		printf("kernel/vxWorks-image : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE/(1<<10));

		printf("Linux rootfs         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
			   CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE/(1<<10));

		printf("\n");
		printf("Single Image         : offset=0x%08x, size=0x%08x (%dK)\n",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			   CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE,
			   (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)/(1<<10));

		printf("\n");
}

static int do_flinfo ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[] )
{
		mv_print_appl_partitions();
		return 1;
}

U_BOOT_CMD(
		  flinfo,    2,    1,    do_flinfo,
		  "flinfo  - print FLASH memory information\n",
		  "\n    - print information for all FLASH memory banks\n"
		  "flinfo N\n    - print information for FLASH memory bank # N\n"
		  );

void  hsuRebootBootRomAsm(void);
static int resetw_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[] )
{
		printf("\nPermorming warm restart\n\n");
		hsuRebootBootRomAsm();
		return 1;
}

U_BOOT_CMD(
		  resetw,      2,     1,      resetw_cmd,
		  "perform warm restart\n",
		  ""
		  );

static int do_cpss_env( cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[] )
{
	char buf[1024];

	printf("Saving cpss environment variable\n");
	setenv("setenv standalone", "");
	setenv("bootcmd", "$standalone_mtd");
	setenv("consoledev","ttyS0");
	setenv("linux_loadaddr","0x2000000");
	setenv("netdev","eth0");
	setenv("rootpath","/tftpboot/rootfs_arm-mv7sft");
	setenv("othbootargs","null=null");

	setenv("nfsboot","setenv bootargs root=/dev/nfs rw nfsroot=$serverip:$rootpath ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off console=$consoledev,$baudrate $othbootargs $linux_parts; tftp $linux_loadaddr $image_name;bootm $linux_loadaddr");

	sprintf(buf,"'spi_flash:%dm(spi_uboot)ro,%dm(spi_kernel),%dm(spi_rootfs),-(remainder)"
		";armada-nand:%dm(nand_kernel),-(nand_rootfs)'",
		CFG_APPL_SPI_FLASH_PART_UBOOT_SIZE / _1M,
		CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE / _1M,
		CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE / _1M,
		CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE / _1M);

	setenv("mtdparts", buf);
	printf("mtdparts = %s\n", buf);
	setenv("linux_parts", buf);

#ifdef MV_INCLUDE_SPI
	sprintf(buf,
		"sf probe 0; sf read ${loadaddr} 0x%x 0x%x; setenv bootargs mem=500M ${console} "
		"root=/dev/mtdblock2 rw init=/linuxrc rootfstype=jffs2 rootwait mtdparts=$mtdparts "
		"${mvNetConfig}; bootm ${loadaddr} ",
		CFG_APPL_SPI_FLASH_PART_KERNEL_START, CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE);
#ifndef MV_NAND
	setenv("standalone_mtd", buf);
	printf("standalone_mtd = %s\n", buf);
#else
	setenv("standalone_mtd_spi", buf);
	printf("standalone_mtd_spi = %s\n", buf);
#endif
#endif /* MV_INCLUDE_SPI */

#ifdef MV_NAND
	sprintf(buf,
		"nand read ${loadaddr} 0x%x 0x%x; setenv bootargs  mem=500M $console mtdparts=$mtdparts "
		"ubi.mtd=5 root=ubi0:rootfs_nand rw rootfstype=ubifs ${mvNetConfig}; bootm 0x2000000;" ,
		CFG_APPL_NAND_FLASH_PART_KERNEL_START,
		CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);

	setenv("standalone_mtd", buf);
	printf("standalone_mtd = %s\n", buf);
#endif

	run_command("saveenv", 0);
	return 1;
}

U_BOOT_CMD(
		  cpss_env,      2,     1,      do_cpss_env,
		  "set cpss environment variables permanently\n",
		  ""
		  );

extern struct spi_flash *flash;

static void _spi_flash_protect(void)
{
#ifdef CONFIG_SPI_FLASH_PROTECTION
		spi_flash_protect(flash, 1);
#endif
}

static void _spi_flash_unprotect(void)
{
#ifdef CONFIG_SPI_FLASH_PROTECTION
		spi_flash_protect(flash, 0);
#endif
}

static int do_spi_mtdburn(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		int filesize;
		ulong addr, src_addr, dest_addr;
		uint kernel_unc_len, rootfs_unc_len = 0, unc_len, src_len;
		uint kernel_addr = 0x6000000;
		uint rootfs_addr = 0x7000000;
		uint total_in;
		int rc;
		int single_file = 0;
		ulong erase_end_offset;
		int bz2_file = 0;

		addr = load_addr = 0x2000000;

		if (!flash) {
				flash = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
				if (!flash) {
						printf("Failed to probe SPI Flash\n");
						return 0;
				}
		}


		if (argc == 2) {
				copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {
				copy_filename (BootFile, "jffs2_arm.image", sizeof(BootFile));
				printf("\nUsing default file \"jffs2_arm.image\" \n");
		}

		if ((filesize = NetLoop(TFTPGET)) < 0)
				return 0;

		if (filesize > CFG_APPL_SPI_FLASH_SIZE) {
				printf("file too big\n");
				return 0;
		}

		printf("\nTrying separation of kernel/vxWorks-image and root_fs. "
			   "Work areas=0x%08x,0x%08x\n",
			   kernel_addr, rootfs_addr);

		dest_addr = kernel_addr; // uncompress the kernel here.
		src_addr = addr;
		src_len = unc_len = kernel_addr - addr;

		rc = BZ2_bzBuffToBuffDecompress_extended (
												 (char*)dest_addr,
												 &unc_len, (char *)src_addr, src_len, &total_in,
												 0, 0);
		printf("\n");
		kernel_unc_len = unc_len;

		if (rc == 0) {
				printf("kernel separation ended ok. unc_len=%d, total_in=%d\n",
					   unc_len, total_in);
				bz2_file++;
		}

		else if (rc == -5) {
				printf("Not a bz2 file, assuming plain single image file\n");
				single_file = 1;
				kernel_unc_len = filesize;
				kernel_addr = load_addr;
		}

		else {
				printf("Uncompress of kernel ended with error. rc=%d\n", rc);
				return 0;
		}

		if (!single_file) {
				//
				// now try to separate the rootfs. If we get -5 then we have a single file.
				//
				dest_addr = rootfs_addr; // uncompress the rootfs here.
				src_addr += total_in;
				src_len = unc_len = kernel_addr - addr - total_in;

				rc = BZ2_bzBuffToBuffDecompress_extended (
														 (char*)dest_addr,
														 &unc_len, (char *)src_addr, src_len, &total_in,
														 0, 0);
				printf("\n");
				rootfs_unc_len = unc_len;
				if (rc == 0) {
						bz2_file++;

						printf("rootfs separation ended ok. unc_len=%d, total_in=%d\n",
							   unc_len, total_in);
						if (unc_len > CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE) {
								printf("rootfs too big\n");
								return 0;
						}
				}

				else if (rc == -5) {
						printf("One single bz2 file detected\n");
						single_file = 1;
				}

				else {
						printf("Uncompress of rootfs ended with error. rc=%d\n", rc);
						return 0;
				}
		}

		if (!single_file && kernel_unc_len > CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE) {
				printf("kernel too big to fit in flash\n");
				return 0;
		} else if (kernel_unc_len > (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)) {
				// we are now dealing with single file
				printf("Single image too big to fit in flash\n");

				if (bz2_file) {
						printf("Trying to fit the compressed image on flash\n");
						if (filesize  > (CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE + CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE)) {
								printf("Single image compressed format too big to fit in flash\n");
								return 0;
						}

						// point to the bz2 image in memory
						kernel_unc_len = filesize;
						kernel_addr = load_addr;
				}

				else {
						return 0;
				}
		}

		printf("\nBurning %s on flash at 0x%08x, length=%dK\n",
			   (single_file) ? "single image" : "kernel",
			   CFG_APPL_SPI_FLASH_PART_KERNEL_START, kernel_unc_len/(1<<10));

		_spi_flash_unprotect();

		erase_end_offset =  CFG_APPL_SPI_FLASH_PART_KERNEL_START +
							(kernel_unc_len & 0xfff00000) + 0x100000;

		printf("Erasing 0x%x - 0x%lx: ", CFG_APPL_SPI_FLASH_PART_KERNEL_START, erase_end_offset);
		spi_flash_erase(flash, CFG_APPL_SPI_FLASH_PART_KERNEL_START,
						(erase_end_offset - CFG_APPL_SPI_FLASH_PART_KERNEL_START));
		printf("\t\t[Done]\n");

		printf("Copy to Flash... ");

		spi_flash_write(flash, CFG_APPL_SPI_FLASH_PART_KERNEL_START,
						kernel_unc_len, (const void *)kernel_addr);

		_spi_flash_protect();

		printf("\n");

		if (!single_file) {
        printf("\nBurning jffs2 rootfs on flash at 0x%08x\n",
             CFG_APPL_SPI_FLASH_PART_ROOTFS_START);

				_spi_flash_unprotect();

				erase_end_offset =  CFG_APPL_SPI_FLASH_PART_ROOTFS_START +
									(rootfs_unc_len & 0xfff00000) + 0x100000;

				printf("Erasing 0x%x - 0x%lx: ", CFG_APPL_SPI_FLASH_PART_ROOTFS_START, erase_end_offset);
				spi_flash_erase(flash, CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
								(erase_end_offset - CFG_APPL_SPI_FLASH_PART_ROOTFS_START));

				printf("Copy to Flash... ");

				spi_flash_write(flash, CFG_APPL_SPI_FLASH_PART_ROOTFS_START,
								rootfs_unc_len, (const void *)rootfs_addr);

				_spi_flash_protect();
		}
		return 1;
}

static int do_spi_mtdburn_legacy(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		char buf[128];
		if (argc == 2) {
				copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {
				copy_filename (BootFile, "jffs2_arm.image", sizeof(BootFile));
				printf("\nUsing default file \"jffs2_arm.image\" \n");
		}

		sprintf(buf, "mtdburn_spi %s", BootFile);
		run_command(buf, 0);
		return 0;
}

U_BOOT_CMD(
		  mtdburn_spi,      2,     1,      do_spi_mtdburn,
		  "Burn a Linux/VxWorks image image on the spi flash.\n",
		  " file-name \n"
		  "\tburn a Linux/Vxworks image the flash.\n\tdefault file-name is ubifs_arm.image.\n"
		  );

U_BOOT_CMD(
		  mtdburn,      2,     1,      do_spi_mtdburn_legacy,
		  "Burn a Linux/VxWorks image image on the spi flash.\n",
		  " file-name \n"
		  "\tburn a Linux/Vxworks image the flash.\n\tdefault file-name is jffs2_arm.image.\n"
		  );


#define MSYS_KERNEL_FLASH_START 0x200000
#define MSYS_KERNEL_FLASH_MAX_LEN 0x300000
#define MSYS_ROOTFS_FLASH_START 0x500000
#define MSYS_ROOTFS_FLASH_MAX_LEN 0xc00000


int mvLoadFile4cpss(int loadfrom, const char* file_name, char * devPart, int fstype)
{
	MV_U32 filesize;

	switch(loadfrom) {
	case 0:
		if ((filesize = NetLoop(TFTPGET)) < 0)
			return 0;
		return filesize;;
	case 1:
		printf("USB is not supported\n");
		return 0;
	case 2:
		printf("MMC is not supported\n");
		return 0;
	}
	return 0;
}

static int do_mtdburn_msys(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 filesize, loadfrom, targetFlash, i;
	MV_U32 targetFlashKernelSize, targetFlashRootFsSize, targetApplSize;
	MV_U32 addr, src_addr, dest_addr;
	MV_U32 kernel_unc_len, rootfs_unc_len = 0, unc_len, src_len;
	MV_U32 kernel_addr = 0x6000000;
	MV_U32 rootfs_addr = 0x7000000;
	MV_U32 total_in;
	MV_U32 rc, single_file = 0;
	MV_U32 erase_end_offset;
	MV_U32 bz2_file = 0;
	char buf[256];

	addr = load_addr = 0x2000000;


	loadfrom = 0;
	targetFlash = TARGET_FLASH_NAND;
	for (i = 1 ; i < argc ; i++) {
		if ((strcmp(argv[i], "spi") == 0) || (strcmp(argv[i], "SPI") == 0)) {
			targetFlash = TARGET_FLASH_SPI;
			argc--; /* disregard 'spi' for next steps */
		}
		if ((strcmp(argv[i], "nand") == 0) || (strcmp(argv[i], "NAND") == 0)) {
			targetFlash = TARGET_FLASH_NAND;
			argc--; /* disregard 'nand' for next steps */
		}
	}

	switch(argc) {
	case 3:/* arg#3 is flash destination, scanned previously --> fall to 2*/
	case 2:
		copy_filename (BootFile, argv[1], sizeof(BootFile));
		/* fall to 1*/
	case 1:    /* no parameter all default */
		if(argc < 2) {
			if (targetFlash == TARGET_FLASH_NAND)
				copy_filename (BootFile, "ubifs_arm_nand.image", sizeof(BootFile));
			else
				copy_filename (BootFile, "ubifs_arm_spi.image", sizeof(BootFile));
			printf("\nUsing default file  \"%s\" \n", BootFile);
		}
		break;
	default:
		return 0;
	}

	printf(" - Load from device \t: %s", "tftp");
	printf("\n - Filename\t\t: %s \n" ,BootFile);
	printf(" - Flash destination\t: %s\n" ,
	       (targetFlash == TARGET_FLASH_NAND) ? "NAND" : "SPI");

	printf("\nDo you want to continue ? [Y/n]");
	readline(" ");
	if( strcmp(console_buffer,"n") == 0 ||
		strcmp(console_buffer,"No") == 0 ||
		strcmp(console_buffer,"N") == 0 )
		return 0;
	printf("\n");

	filesize = mvLoadFile4cpss(loadfrom, BootFile, 0, 0);
	if(filesize <=0 )
		return 0;
	if (targetFlash == TARGET_FLASH_NAND) {
		targetApplSize = CFG_APPL_NAND_FLASH_SIZE;
		targetFlashKernelSize = CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE;
		targetFlashRootFsSize = CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE;
	} else {
		targetApplSize = CFG_APPL_SPI_FLASH_SIZE;
		targetFlashKernelSize = CFG_APPL_SPI_FLASH_PART_KERNEL_SIZE;
		targetFlashRootFsSize = CFG_APPL_SPI_FLASH_PART_ROOTFS_SIZE;
	}

	if (filesize > targetApplSize) {
		printf("file too big\n");
		return 0;
	}

	printf("\nTrying separation of kernel/vxWorks-image and root_fs. Work areas=0x%08x,0x%08x\n",
	       kernel_addr, rootfs_addr);

	dest_addr = kernel_addr; // uncompress the kernel here.
	src_addr = addr;
	src_len = unc_len = kernel_addr - addr;

	rc = BZ2_bzBuffToBuffDecompress_extended ((char*)dest_addr, &unc_len,
						  (char *)src_addr, src_len,
						  &total_in, 0, 0);
	printf("\n");
	kernel_unc_len = unc_len;

	if (rc == 0) {
		printf("kernel separation ended ok. unc_len=%d, total_in=%d\n",unc_len, total_in);
		bz2_file++;
	} else if (rc == -5) {
		printf("Not a bz2 file, assuming plain single image file\n");
		single_file = 1;
		kernel_unc_len = filesize;
		kernel_addr = load_addr;
	} else {
		printf("Uncompress of kernel ended with error. rc=%d\n", rc);
		return 0;
	}

	if (!single_file) {
	/* now try to separate the rootfs. If we get -5 then we have a single file. */
		dest_addr = rootfs_addr; // uncompress the rootfs here.
		src_addr += total_in;
		src_len = unc_len = kernel_addr - addr - total_in;

		rc = BZ2_bzBuffToBuffDecompress_extended ((char*)dest_addr, &unc_len, (char *)src_addr,
							  src_len, &total_in, 0, 0);
		printf("\n");
		rootfs_unc_len = unc_len;
		if (rc == 0) {
			bz2_file++;

			printf("rootfs separation ended ok. unc_len=%d, total_in=%d\n", unc_len, total_in);
			if (unc_len > targetFlashRootFsSize) {
				printf("rootfs too big\n");
				return 0;
			}
		} else if (rc == -5) {
			printf("One single bz2 file detected\n");
			single_file = 1;
		} else {
			printf("Uncompress of rootfs ended with error. rc=%d\n", rc);
			return 0;
		}
	}

	if (!single_file && kernel_unc_len > targetFlashKernelSize) {
		printf("kernel too big to fit in flash\n");
		return 0;
	} else if (kernel_unc_len > (targetFlashKernelSize + targetFlashRootFsSize)) {
		// we are now dealing with single file
		printf("Single image too big to fit in flash\n");
		if (bz2_file) {
			printf("Trying to fit the compressed image on flash\n");
			if (filesize  > (targetFlashKernelSize + targetFlashRootFsSize)) {
				printf("Single image compressed format too big to fit in flash\n");
				return 0;
			}
			/* point to the bz2 image in memory */
			kernel_unc_len = filesize;
			kernel_addr = load_addr;
		} else
			return 0;
	}

	msys_spi_regs = 1;
	_run_command("fix_pex");
	if (targetFlash == TARGET_FLASH_NAND) {
		printf("\nBurning %s on msys NAND flash at 0x%08x, length=%dK\n",
		       (single_file) ? "single image" : "kernel",
		       CFG_APPL_NAND_FLASH_PART_KERNEL_START, kernel_unc_len/(1<<10));

		sprintf(buf, "nand erase 0x%08x 0x%08x",
			CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);
		printf("Erasing kernel partition: %s\n", buf);
		run_command(buf, 0);

		sprintf(buf, "nand write 0x%08x 0x%08x 0x%08x",
			kernel_addr,
			CFG_APPL_NAND_FLASH_PART_KERNEL_START,
			CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);
		printf("Copy to Flash: %s \n", buf);
		run_command(buf, 0);

		if (!single_file) {
			printf("\nBurning ubifs rootfs on flash at 0x%08x\n",
			       CFG_APPL_NAND_FLASH_PART_ROOTFS_START);

			sprintf(buf, "nand erase 0x%08x 0x%08x",
				CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
				CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE);
			printf("Erasing rootfs partition: %s\n", buf);
			run_command(buf, 0);

			sprintf(buf, "nand write 0x%08x 0x%08x 0x%08x",
				rootfs_addr,
				CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
				rootfs_unc_len);
			printf("Copy to Flash: %s \n", buf);
			run_command(buf, 0);
		}
		msys_spi_regs = 0;
		return 1;
	}
	_run_command("sf probe 0");
	printf("\nBurning %s on msys SPI flash at 0x%08x, length=0x%08X\n",
	       (single_file) ? "single image" : "kernel",
	       CFG_APPL_SPI_FLASH_PART_KERNEL_START, kernel_unc_len);

	erase_end_offset = (kernel_unc_len + CFG_APPL_SPI_FLASH_BLOCK_ALIMENT) & ~(CFG_APPL_SPI_FLASH_BLOCK_ALIMENT-1);
	sprintf(buf, "sf erase %x %x", CFG_APPL_SPI_FLASH_PART_KERNEL_START, erase_end_offset);
	_run_command(buf);
	printf("\t\t[Done]\n");

	sprintf(buf, "sf write %x %x %x", (unsigned int)kernel_addr, CFG_APPL_SPI_FLASH_PART_KERNEL_START, kernel_unc_len);
	_run_command(buf);
	printf("\n");

	if (!single_file) {

		erase_end_offset = (rootfs_unc_len + CFG_APPL_SPI_FLASH_BLOCK_ALIMENT) & ~(CFG_APPL_SPI_FLASH_BLOCK_ALIMENT-1);
		printf("\nBurning rootfs on flash at 0x%08X, length=0x%08X\n",
		       CFG_APPL_SPI_FLASH_PART_ROOTFS_START, rootfs_unc_len);
		sprintf(buf, "sf erase %x %x", CFG_APPL_SPI_FLASH_PART_ROOTFS_START, erase_end_offset);
		_run_command(buf);
		printf("\t\t[Done]\n");

		sprintf(buf, "sf write %x %x %x",
			(unsigned int)rootfs_addr, CFG_APPL_SPI_FLASH_PART_ROOTFS_START, rootfs_unc_len);
		_run_command(buf);
		printf("\n");
	}
	msys_spi_regs = 0;
	return 1;

}

U_BOOT_CMD(
      mtdburn_msys,      3,     1,      do_mtdburn_msys,
	"Burn a Linux/VxWorks image image on the msys spi/nand flash.\n",
	"[filename [flash target]]\n"
	"\tdefault file-name is jffs2_arm.image.\n"
	"\tTarget flash can be spi or nand, default is spi.\n"
      );

/* Boot from SPI flash */
/* Write u-boot image into the SPI flash */
int spi_burn_uboot_cmd_msys(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int filesize;
	extern char console_buffer[];
	char buf[128];

	load_addr = CONFIG_SYS_LOAD_ADDR;

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	} else {
		copy_filename (BootFile, "u-boot-msys.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot-msys.bin\" \n");
	}

	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if(filesize == -1) {
		printf("\t\t[ERR!]\n");
		return 0;
	}

	printf("\nDo you realy want to burn a new uboot on the msys flash? [Y/n]");
	readline(" ");
	if( strcmp(console_buffer,"n") == 0 ||
		strcmp(console_buffer,"No") == 0 ||
		strcmp(console_buffer,"N") == 0 )
		return 0;
	msys_spi_regs = 1;		/* indicating access over PCIe to remote internal registers */
	_run_command("fix_pex");	/* enable PCIe access to msys BC2 */

	sprintf(buf, "sf write %x 0 %x", (unsigned int)load_addr, filesize);

	_run_command("sf probe 0");
	_run_command("sf erase 0 100000");
	printf("\t[Done]\n");
	_run_command(buf);
	msys_spi_regs = 0;	/* indicating Finished with access over PCIe to remote internal registers */

	printf("\t\t[Done]\n");

	return 1;
}

U_BOOT_CMD(
        bubt_msys,      2,     1,      spi_burn_uboot_cmd_msys,
        "bubt	- Burn a U-Boot image on the msys Boot SPI Flash, over PCIE\n",
        " file-name \n"
        "\tBurn a U-Boot image on the msys Boot SPI Flash, over PCIE,\n"
	"\tdefault file-name is u-boot-msys.bin .\n"
);


static int do_nand_mtdburn(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		char buf[1024];
		int filesize;
		ulong addr, src_addr, dest_addr;
		uint kernel_unc_len, rootfs_unc_len = 0, unc_len, src_len;
		uint kernel_addr = 0x6000000;
		uint rootfs_addr = 0x7000000;
		uint total_in;
		int rc;
		int single_file = 0;
		int bz2_file = 0;

		addr = load_addr = 0x5000000;

		if (argc == 2) {
				copy_filename (BootFile, argv[1], sizeof(BootFile));
		} else {
				copy_filename (BootFile, "ubifs_arm_nand.image", sizeof(BootFile));
				printf("\nUsing default file \"ubifs_arm_nand.image\" \n");
		}

		if ((filesize = NetLoop(TFTPGET)) < 0)
				return 0;

		if (filesize > CFG_APPL_NAND_FLASH_SIZE) {
				printf("file too big\n");
				return 0;
		}

		printf("\nSeparating kernel/vxWorks-image and root_fs. "
			   "Work areas=0x%08x,0x%08x\n",
			   kernel_addr, rootfs_addr);

		dest_addr = kernel_addr; // uncompress the kernel here.
		src_addr = addr;
		src_len = unc_len = 0x1000000*3; //16MB*3 = max+

		rc = BZ2_bzBuffToBuffDecompress_extended (
												 (char*)dest_addr,
												 &unc_len, (char *)src_addr, src_len, &total_in,
												 0, 0);
		printf("\n");
		kernel_unc_len = unc_len;

		if (rc == 0) {
				printf("kernel separation ended ok. unc_len=%d, total_in=%d\n",
					   unc_len, total_in);
				bz2_file++;
		}

		else if (rc == -5) {
				printf("Not a bz2 file, assuming plain single image file\n");
				single_file = 1;
				kernel_unc_len = filesize;
				kernel_addr = load_addr;
		}

		else {
				printf("Uncompress of kernel ended with error. rc=%d\n", rc);
				return 0;
		}

		if (!single_file) {
				//
				// now try to separate the rootfs. If we get -5 then we have a single file.
				//
				dest_addr = rootfs_addr; // uncompress the rootfs here.
				src_addr += total_in;
				src_len = unc_len = 0x1000000*3; //16MB*3 = max+

				rc = BZ2_bzBuffToBuffDecompress_extended (
														 (char*)dest_addr,
														 &unc_len, (char *)src_addr, src_len, &total_in,
														 0, 0);
				printf("\n");
				rootfs_unc_len = unc_len;
				if (rc == 0) {
						bz2_file++;

						printf("rootfs separation ended ok. unc_len=%d, total_in=%d\n",
							   unc_len, total_in);
						if (unc_len > CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE) {
								printf("rootfs too big\n");
								return 0;
						}
				}

				else if (rc == -5) {
						printf("One single bz2 file detected\n");
						single_file = 1;
				}

				else {
						printf("Uncompress of rootfs ended with error. rc=%d\n", rc);
						return 0;
				}
		}

		if (!single_file && kernel_unc_len > CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE) {
				printf("kernel too big to fit in flash\n");
				return 0;
		} else if (kernel_unc_len > (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)) {
				// we are now dealing with single file
				printf("Single image too big to fit in flash\n");

				if (bz2_file) {
						printf("Trying to fit the compressed image on flash\n");
						if (filesize  > (CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE + CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE)) {
								printf("Single image compressed format too big to fit in flash\n");
								return 0;
						}

						// point to the bz2 image in memory
						kernel_unc_len = filesize;
						kernel_addr = load_addr;
				}

				else {
						return 0;
				}
		}

		printf("\nBurning %s on flash at 0x%08x, length=%dK\n",
			   (single_file) ? "single image" : "kernel",
			   CFG_APPL_NAND_FLASH_PART_KERNEL_START, kernel_unc_len/(1<<10));

		sprintf(buf, "nand erase 0x%08x 0x%08x",
				CFG_APPL_NAND_FLASH_PART_KERNEL_START,
				CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);
		printf("Erasing kernel partition: %s\n", buf);
		run_command(buf, 0);

		sprintf(buf, "nand write 0x%08x 0x%08x 0x%08x",
				kernel_addr,
				CFG_APPL_NAND_FLASH_PART_KERNEL_START,
				CFG_APPL_NAND_FLASH_PART_KERNEL_SIZE);
		printf("Copy to Flash: %s \n", buf);
		run_command(buf, 0);

		if (!single_file) {
				printf("\nBurning ubifs rootfs on flash at 0x%08x\n",
					   CFG_APPL_NAND_FLASH_PART_ROOTFS_START);

				sprintf(buf, "nand erase 0x%08x 0x%08x",
						CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
						CFG_APPL_NAND_FLASH_PART_ROOTFS_SIZE);
				printf("Erasing rootfs partition: %s\n", buf);
				run_command(buf, 0);

				sprintf(buf, "nand write 0x%08x 0x%08x 0x%08x",
						rootfs_addr,
						CFG_APPL_NAND_FLASH_PART_ROOTFS_START,
						rootfs_unc_len);
				printf("Copy to Flash: %s \n", buf);
				run_command(buf, 0);
		}
		return 1;
}

U_BOOT_CMD(
		  mtdburn_nand,      2,     1,      do_nand_mtdburn,
		  "Burn a Linux/VxWorks image image on the nand flash.\n",
		  " file-name \n"
		  "\tburn a Linux/Vxworks image the flash.\n\tdefault file-name is ubifs_arm.image.\n"
		  );


#define  SMI_WRITE_ADDRESS_MSB_REGISTER   (0x00)
#define  SMI_WRITE_ADDRESS_LSB_REGISTER   (0x01)
#define  SMI_WRITE_DATA_MSB_REGISTER      (0x02)
#define  SMI_WRITE_DATA_LSB_REGISTER      (0x03)

#define  SMI_READ_ADDRESS_MSB_REGISTER    (0x04)
#define  SMI_READ_ADDRESS_LSB_REGISTER    (0x05)
#define  SMI_READ_DATA_MSB_REGISTER       (0x06)
#define  SMI_READ_DATA_LSB_REGISTER       (0x07)

#define  SMI_STATUS_REGISTER              (0x1f)

#define SMI_STATUS_WRITE_DONE             (0x02)
#define SMI_STATUS_READ_READY             (0x01)

//#define SMI_WAIT_FOR_STATUS_DONE
#define SMI_TIMEOUT_COUNTER  10000

static int phy_in_use(int phy_addr)
{
		int i;
		for (i = 0; i < mvCtrlEthMaxPortGet(); i++) {
				if (mvBoardPhyAddrGet(i) == phy_addr)
						return 1;
		}
		return 0;
}

static inline MV_STATUS ethPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data)
{
		return mvEthPhyRegRead(phyAddr, regOffs, data);
}

static inline MV_STATUS ethPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data)
{
		return mvEthPhyRegWrite(phyAddr, regOffs, data);
}


int smiReadReg(unsigned int  devSlvId, unsigned int  regAddr, volatile unsigned int *value)
{
		int ret;
		MV_U16    temp1;

		ret = ethPhyRegRead(devSlvId, regAddr, &temp1);
		*value = temp1;
		return(0 == ret)? 0 : 1;
}

int smiWriteReg(unsigned int devSlvId, unsigned int regAddr, unsigned int value)
{
		/* Perform direct smi write reg */
		int ret;

		ret = ethPhyRegWrite(devSlvId, regAddr, value);
		return(0 == ret)? 0 : 1;
}

static inline void smiWaitForStatus(unsigned int devSlvId)
{
#ifdef SMI_WAIT_FOR_STATUS_DONE
		volatile unsigned int stat;
		unsigned int timeOut;
		int rc;

		/* wait for write done */
		timeOut = SMI_TIMEOUT_COUNTER;
		do {
				rc = smiReadReg(devSlvId, SMI_STATUS_REGISTER, &stat);
				if (rc != 0)
						return;
				if (--timeOut < 1) {
						printf("bspSmiWaitForStatus timeout !\n");
						return;
				}
		} while ((stat & SMI_STATUS_WRITE_DONE) == 0);
#endif
}

int bspSmiReadReg
(
unsigned int  devSlvId,
unsigned int  actSmiAddr,
unsigned int  regAddr,
unsigned int *valuePtr
)
{
		/* Perform indirect smi read reg */
		int           rc;
		unsigned int        msb;
		unsigned int        lsb;

		/* write addr to read */
		msb = regAddr >> 16;
		lsb = regAddr & 0xFFFF;
		rc = smiWriteReg(devSlvId, SMI_READ_ADDRESS_MSB_REGISTER,msb);
		if (rc != 0)
				return rc;

		rc = smiWriteReg(devSlvId, SMI_READ_ADDRESS_LSB_REGISTER,lsb);
		if (rc != 0)
				return rc;

		smiWaitForStatus(devSlvId);

		/* read data */
		rc = smiReadReg(devSlvId, SMI_READ_DATA_MSB_REGISTER, &msb);
		if (rc != 0)
				return rc;

		rc = smiReadReg(devSlvId, SMI_READ_DATA_LSB_REGISTER, &lsb);
		if (rc != 0)
				return rc;

		*valuePtr = ((msb & 0xFFFF) << 16) | (lsb & 0xFFFF);
		return 0;
}

int bspSmiScan(int instance, int noisy)
{
		int found1 = 0;
		int found2 = 0;
		int i;
		unsigned int data;

		/* scan for SMI devices */
		for (i = 0; i < 32;  i++) {
				bspSmiReadReg(i, 0, 0x3, &data);
				if (data != 0xffffffff && data != 0xffff) {
						bspSmiReadReg(i, 0, 0x50, &data);
						if (data == 0x000011ab  || data == 0xab110000) {
								if (instance == found1++) {
										bspSmiReadReg(i, 0, 0x4c, &data);
										printf("Smi Scan found Marvell device at smi_addr 0x%x, "
											   "reg 0x4c=0x%08x\n", i, data);
										found2 = 1;
										break;
								}
						}
				}
		}

		if (!found2) {
				if (noisy)
						printf("Smi scan found no device\n");
				return 0;
		}

		return  i;
}


static int do_smi_scan(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		int i;
		int rc = 0;

		printf("Performing SMI scan\n");
		for (i = 0; i < 32; i++) {
				if (!phy_in_use(i))
						rc += bspSmiScan(i, 0);
		}

		if (rc == 0)
				printf("smiscan found not device\n");
		return 1;
}

U_BOOT_CMD(
		  smiscan,      1,     1,      do_smi_scan,
		  "smiscan - Scan for marvell smi devices.\n",
		  ""
		  );

SILICON_TYPE get_attached_silicon_type(void)
{
		int Device;
		int Function;
		int BusNum;
		unsigned char HeaderType;
		unsigned short VendorID;
		unsigned short DeviceID;
		pci_dev_t dev;
		SILICON_TYPE silt = SILT_OTHER;

		for (BusNum = 0; BusNum < 10; BusNum ++)
				for (Device = 0; Device < PCI_MAX_PCI_DEVICES; Device++) {
						HeaderType = 0;
						VendorID = 0;
						for (Function = 0; Function < PCI_MAX_PCI_FUNCTIONS; Function++) {
								/*
								 * If this is not a multi-function device, we skip the rest.
								 */
								if (Function && !(HeaderType & 0x80))
										break;

								dev = PCI_BDF(BusNum, Device, Function);

								pci_read_config_word(dev, PCI_VENDOR_ID, &VendorID);
								if ((VendorID == 0xFFFF) || (VendorID == 0x0000))
										continue;

								pci_read_config_word(dev, PCI_DEVICE_ID, &DeviceID);
								if ((DeviceID == 0xFFFF) || (DeviceID == 0x0000))
										continue;

								if ( (VendorID == MARVELL_VENDOR_ID) &&
									 (((DeviceID & MV_PP_CHIP_TYPE_MASK) >> MV_PP_CHIP_TYPE_OFFSET) ==
									  MV_PP_CHIP_TYPE_XCAT2)
								   )
										silt = SILT_XCAT2;

								else if ((VendorID == MARVELL_VENDOR_ID) && (DeviceID == NP5_DEVICE_ID))
										silt = SILT_NP5;

								else if ((VendorID == MARVELL_VENDOR_ID) && ( (DeviceID & 0xffe0) == BC2_DEVICE_ID))
										silt = SILT_BC2;

								return silt;
						}
				}
		return silt;
}

void hwServicesLateInit(void)
{
	char *env;
	char buf[128];
	printf("hwServices late init: ");

	if (get_attached_silicon_type() == SILT_BC2)
	{
		printf("running fix_pex: open twsi channel to bc2 wa\n");
		run_command("fix_pex", 0);
	}

	if ((env = getenv("qsgmii_ports_def"))) {
		sprintf(buf, "qsgmii_sel %s\n", env);
		printf(buf);
		run_command(buf, 0);
	}
	printf("\n");
}

