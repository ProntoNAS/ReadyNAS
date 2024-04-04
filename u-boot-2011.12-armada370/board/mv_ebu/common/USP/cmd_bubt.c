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

#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include <command.h>
#include <net.h>
#include <environment.h>

#if defined(CONFIG_CMD_NAND)
#include <nand.h>
extern nand_info_t nand_info[];       /* info for NAND chips */
#endif

#ifdef CONFIG_CMD_SF
#include <spi_flash.h>
extern struct spi_flash *flash;
#endif

#ifdef CONFIG_CMD_FLASH
#include <flash.h>
extern flash_info_t flash_info[];       /* info for FLASH chips */
#endif


#if defined(MV_NAND)
/* Write u-boot image into the nand flash */
int nand_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifndef MV_NAND_READ_OOB
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	nand_info_t *nand = &nand_info[0];
	//u_char *load_addr;
	size_t size = CONFIG_UBOOT_SIZE;
	size_t offset = 0;
	nand_erase_options_t opts;

	
	offset = 0;
#if defined(CONFIG_SKIP_BAD_BLOCK)
	int i = 0;
	int sum = 0;
	size_t blocksize;
	blocksize = nand_info[0].erasesize;

	while(i * blocksize < nand_info[0].size) {
		if (!nand_block_isbad(&nand_info[0], (i * blocksize)))
			sum += blocksize;
		else {
			sum = 0;
			offset = (i + 1) * blocksize;
		}

		if (sum >= CONFIG_UBOOT_SIZE)
			break;
		i++;
	}
#endif
	if(argc >= 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else {
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}
	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[Fail]\n");
		return 0;
	}
	if (filesize > CONFIG_UBOOT_SIZE) {
		printf("Boot image is too big. Maxumum size is %d bytes\n", CONFIG_UBOOT_SIZE);
		return 0;
	}
	printf("\t[Done]\n");
	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 ||
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing 0x%x - 0x%x:",CONFIG_UBOOT_SIZE + offset, CONFIG_ENV_RANGE_NAND);
		memset(&opts, 0, sizeof(opts));
		opts.length = CONFIG_ENV_RANGE_NAND;
		opts.quiet = 1;
		opts.offset = CONFIG_ENV_OFFSET;

		if (nand_erase_opts(&nand_info[0], &opts))
			return 1;

		printf("\t[Done]\n");
	}

	printf("Erasing 0x%x - 0x%x: ", offset, offset + CONFIG_UBOOT_SIZE);
	memset(&opts, 0, sizeof(opts));
	opts.length = CONFIG_UBOOT_SIZE;
	opts.quiet = 1;
	opts.offset = offset;
	if (nand_erase_opts(&nand_info[0], &opts))
		return 1;
	printf("\t[Done]\n");

	printf("Writing image to NAND:");
	ret = nand_write(nand, offset, &size, (u_char *)load_addr);
	if (ret)
		printf("\t[Fail]\n");
	else
		printf("\t[Done]\n");
#else
	printf("\nError: This image compiled without ECC.\nThe write commands to nand is disabled \n\n");
#endif
	return 1;
}

#endif /* defined(MV_NAND) */


#if defined(MV_INCLUDE_SPI)
/* Write u-boot image into the SPI flash */
int spi_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	load_addr = CONFIG_SYS_LOAD_ADDR;

	if(argc >= 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else {
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}

	if(!flash) {
		flash = spi_flash_probe(0, 0, CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
		if (!flash) {
			printf("Failed to probe SPI Flash\n");
			set_default_env("!spi_flash_probe() failed");
			return 0;
		}
	}

	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t\t[ERR!]\n");
		return 0;
	}
	printf("\t\t[Done]\n");
	printf("Override Env parameters to default? [y/N]");
	readline(" ");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Unprotecting flash:");
	spi_flash_protect(flash, 0);
	printf("\t\t[Done]\n");
#endif
	if( strcmp(console_buffer,"Y") == 0 ||
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing 0x%x - 0x%x:",CONFIG_ENV_OFFSET_SPI, CONFIG_ENV_OFFSET_SPI + CONFIG_ENV_SIZE_SPI);
		spi_flash_erase(flash, CONFIG_ENV_OFFSET_SPI, CONFIG_ENV_SIZE_SPI);
		printf("\t[Done]\n");
	}
	if (filesize > CONFIG_ENV_OFFSET_SPI)
	{
		printf("Error Image size(%x) bigger than CONFIG_ENV_OFFSET=%x ",filesize,CONFIG_ENV_OFFSET_SPI);
		return 0;
	}
	printf("Erasing 0x%x - 0x%x: ",0, 0 + CONFIG_ENV_OFFSET_SPI);
	spi_flash_erase(flash, 0, CONFIG_ENV_OFFSET_SPI);
	printf("\t\t[Done]\n");

	printf("Writing image to flash:");
	ret = spi_flash_write(flash, 0, filesize, (const void *)load_addr);

	if (ret)
		printf("\t\t[Err!]\n");
	else
		printf("\t\t[Done]\n");

#ifdef CONFIG_SPI_FLASH_PROTECTION
	printf("Protecting flash:");
	spi_flash_protect(flash, 1);
	printf("\t\t[Done]\n");
#endif
	return 1;
}
#endif /*  defined(MV_INCLUDE_SPI) */

#if defined(MV_INCLUDE_NOR)
/* Boot from Nor flash */
int nor_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	load_addr = CONFIG_SYS_LOAD_ADDR;

	ulong stop_addr;
	ulong start_addr;

	if(argc >= 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else {
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}

	filesize = NetLoop(TFTPGET);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[ERR!]\n");
		return 0;
	}
	printf("\t[Done]\n");
	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 ||
		   strcmp(console_buffer,"yes") == 0 ||
		   strcmp(console_buffer,"y") == 0 ) {

		start_addr = CONFIG_ENV_ADDR_NOR;
		stop_addr = start_addr + CONFIG_ENV_SIZE_NOR - 1;

		printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET_NOR);
		flash_sect_protect (0, start_addr, stop_addr);

		flash_sect_erase (start_addr, stop_addr);

		flash_sect_protect (1, start_addr, stop_addr);
		printf("\t[Done]\n");
	}

	start_addr = NOR_CS_BASE;
	stop_addr = start_addr + CONFIG_ENV_OFFSET_NOR - 1;

	flash_sect_protect (0, start_addr, stop_addr);

	printf("Erasing 0x%x - 0x%x: ", (unsigned int)start_addr, (unsigned int)(start_addr + CONFIG_ENV_OFFSET_NOR));
	flash_sect_erase (start_addr, stop_addr);
	printf("\t[Done]\n");

	printf("Writing image to NOR:");
	ret = flash_write((char *)CONFIG_SYS_LOAD_ADDR, start_addr, filesize);

	if (ret)
	   printf("\t[Err!]\n");
	else
	   printf("\t[Done]\n");

	flash_sect_protect (1, start_addr, stop_addr);
	return 1;
}
#endif /* defined(MV_INCLUDE_NOR) */


/* Write u-boot image into the flash */
int burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if(argc >= 3) {
#if defined(MV_NAND)
		if ((memcmp("nand", argv[2], 4)==0) || (memcmp("NAND", argv[2], 4)==0))
			 return nand_burn_uboot_cmd(cmdtp, flag, argc, argv);
#endif
#if defined(MV_INCLUDE_SPI)
		if ((memcmp("spi", argv[2], 3)==0) || (memcmp("SPI", argv[2], 3)==0))
			  return spi_burn_uboot_cmd(cmdtp, flag, argc, argv);
#endif
#if defined(MV_INCLUDE_NOR)
		if ((memcmp("nor", argv[2], 3)==0) || (memcmp("NOR", argv[2], 3)==0))
			  return nor_burn_uboot_cmd(cmdtp, flag, argc, argv);
#endif
	}
#if defined(MV_NAND_BOOT)
	return nand_burn_uboot_cmd(cmdtp, flag, argc, argv);
#endif
#if defined(MV_SPI_BOOT)
	return spi_burn_uboot_cmd(cmdtp, flag, argc, argv);
#endif
#if defined(MV_NOR_BOOT)
	return nor_burn_uboot_cmd(cmdtp, flag, argc, argv);
#endif
	return 1;
}

U_BOOT_CMD(
        bubt,      3,     1,      burn_uboot_cmd,
        "bubt	- Burn an image on the Boot Flash.\n",
        " file-name [target flash(SPI/NOR/NAND)] \n"
        "\tBurn a binary image on the Boot Flash, default file-name is u-boot.bin.\n"
		  );
