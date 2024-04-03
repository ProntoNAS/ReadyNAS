/*
 *  Copyright (C) 2004 Infrant Tech (bzhang@infrant.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Overview:
 *   This is a device driver for the NAND flash device found on the
 *   P0 board which utilizes the SMASUNG K9F1208D0A  part. This is
 *   a 512Mibit (64MiB x 8 bits) NAND flash device.
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>
#include <asm/padre0.h>

extern unsigned int g_sys_type;

// module parameter
extern int vpd392_override;
module_param(vpd392_override, int, S_IRUGO);
MODULE_PARM_DESC(vpd392_override, "Override VPD. 0=respect_vpd 1=force_on 2=force_off");

/*
 * MTD structure for P0 board
 */
static struct mtd_info *padre_p0_mtd = NULL;

/*
 * Values specific to the PADRE P0 board
 */

/*
 * Define partitions for flash device
 */
static struct mtd_partition partition_info[] = {
	{ name: "P0 flash partition 1",
	  offset: 0,
	  size: 1*1024*1024 },
	{ name: "P0 flash partition 2",
	  offset: 1*1024*1024,
	  size: 0 }
};
#define NUM_PARTITIONS 2

void padre_hwcontrol(struct mtd_info *mtd, int cmd)
{
    unsigned long tmp;

    switch(cmd){

        case NAND_CTL_SETNCE:
		tmp = readl(PADRE_PCI_FEATURE);
		tmp |= ( 2<<9);
		tmp &= ~PADRE_PCI_FEATURE_PCION;

		writel(tmp, PADRE_PCI_FEATURE);
		break;
        case NAND_CTL_CLRNCE:
		tmp = readl(PADRE_PCI_FEATURE);
		tmp &= ~( 2<<9);
		tmp |= PADRE_PCI_FEATURE_PCION;
		writel(tmp, PADRE_PCI_FEATURE);
		break;
	default:
		break;
    }
}


/*
 * Main initialization routine
 */
int __init padre_nand_flash_init (void)
{
	struct nand_chip *this;

        if(g_sys_type != SYSTEM_TYPE_PZERO)
        {
                printk("This is not P0, no NAND support, bail out.\n");
                return -ENODEV;
        }

	/* Allocate memory for MTD device structure and private data */
	padre_p0_mtd = kmalloc (sizeof(struct mtd_info) + sizeof (struct nand_chip),
				GFP_KERNEL);
	if (!padre_p0_mtd) {
		printk ("Unable to allocate P0 NAND MTD device structure.\n");
		return -ENOMEM;
	}

	/* Get pointer to private data */
	this = (struct nand_chip *) (&padre_p0_mtd[1]);

	/* Initialize structures */
	memset((char *) padre_p0_mtd, 0, sizeof(struct mtd_info));
	memset((char *) this, 0, sizeof(struct nand_chip));

	/* Link the private data with the MTD structure */
	padre_p0_mtd->priv = this;


	/* Set address of NAND IO lines */
	this->IO_ADDR_W = this->IO_ADDR_R = PADRE_ROM_SPACE + 0x8000000;

       /* Set address of hardware control function */
        this->hwcontrol = padre_hwcontrol;
        /* 15 us command delay time */
        this->chip_delay = 15;


	/* Scan to find existence of the device */
	if (nand_scan (padre_p0_mtd, 1)) {
		kfree (padre_p0_mtd);
		return -ENXIO;
	}

	/* Allocate memory for internal data buffer */
	this->data_buf = kmalloc (sizeof(u_char) * (padre_p0_mtd->oobblock + padre_p0_mtd->oobsize), GFP_KERNEL);
	if (!this->data_buf) {
		printk ("Unable to allocate NAND data buffer for P0.\n");
		kfree (padre_p0_mtd);
		return -ENOMEM;
	}

        nand_init_bbt(padre_p0_mtd);

	partition_info[1].size = bbt.total_good_blocks * 16 * 1024 - 1024*1024;
	/* Register the partitions */
	add_mtd_partitions(padre_p0_mtd, partition_info, NUM_PARTITIONS);

	/* Return happy */
	return 0;
}
module_init(padre_nand_flash_init);

/*
 * Clean up routine
 */
#ifdef MODULE
static void __exit padre_nand_flash_cleanup (void)
{
	struct nand_chip *this = (struct nand_chip *) &padre_p0_mtd[1];

	del_mtd_partitions(padre_p0_mtd);

	/* Unregister the device */
	del_mtd_device (padre_p0_mtd);

	/* Free internal data buffer */
	kfree (this->data_buf);

	/* Free the MTD device structure */
	kfree (padre_p0_mtd);
}
module_exit(padre_nand_flash_cleanup);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bin Zhang <bzhang@infrant.com");
MODULE_DESCRIPTION("Board-specific glue layer for NAND flash on P0 board");
