/*
 *  drivers/mtd/nand.c
 *
 *  Overview:
 *   This is the generic MTD driver for NAND flash devices. It should be
 *   capable of working with almost all NAND chips currently available.
 *
 *  Additional technical information is available on
 *  http://www.linux-mtd.infradead.org/tech/nand.html
 *
 *  Copyright (C) 2000 Steven J. Hill (sjhill@realitydiluted.com)
 *        2002 Thomas Gleixner (tglx@linutronix.de)
 *
 *  10-29-2001  Thomas Gleixner (tglx@linutronix.de)
 *      - Changed nand_chip structure for controlline function to
 *      support different hardware structures (Access to
 *      controllines ALE,CLE,NCE via hardware specific function.
 *      - exit out of "failed erase block" changed, to avoid
 *      driver hangup
 *      - init_waitqueue_head added in function nand_scan !!
 *
 *  01-30-2002  Thomas Gleixner (tglx@linutronix.de)
 *      change in nand_writev to block invalid vecs entries
 *
 *  02-11-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - major rewrite to avoid duplicated code
 *        common nand_write_page function
 *        common get_chip function
 *      - added oob_config structure for out of band layouts
 *      - write_oob changed for partial programming
 *      - read cache for faster access for subsequent reads
 *      from the same page.
 *      - support for different read/write address
 *      - support for device ready/busy line
 *      - read oob for more than one page enabled
 *
 *  02-27-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - command-delay can be programmed
 *      - fixed exit from erase with callback-function enabled
 *
 *  03-21-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - DEBUG improvements provided by Elizabeth Clarke
 *      (eclarke@aminocom.com)
 *      - added zero check for this->chip_delay
 *
 *  04-03-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - added added hw-driver supplied command and wait functions
 *      - changed blocking for erase (erase suspend enabled)
 *      - check pointers before accessing flash provided by
 *      John Hall (john.hall@optionexist.co.uk)
 *
 *  04-09-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - nand_wait repaired
 *
 *  04-28-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - OOB config defines moved to nand.h
 *
 *  08-01-2002  Thomas Gleixner (tglx@linutronix.de)
 *      - changed my mailaddress, added pointer to tech/nand.html
 *
 *  08-07-2002  Thomas Gleixner (tglx@linutronix.de)
 *      forced bad block location to byte 5 of OOB, even if
 *      CONFIG_MTD_NAND_ECC_JFFS2 is not set, to prevent
 *      erase /dev/mtdX from erasing bad blocks and destroying
 *      bad block info
 *
 *  08-10-2002  Thomas Gleixner (tglx@linutronix.de)
 *      Fixed writing tail of data. Thanks to Alice Hennessy
 *      <ahennessy@mvista.com>.
 *
 *  08-10-2002  Thomas Gleixner (tglx@linutronix.de)
 *      nand_read_ecc and nand_write_page restructured to support
 *      hardware ECC. Thanks to Steven Hein (ssh@sgi.com)
 *      for basic implementation and suggestions.
 *      3 new pointers in nand_chip structure:
 *      calculate_ecc, correct_data, enabled_hwecc
 *      forcing all hw-drivers to support page cache
 *      eccvalid_pos is now mandatory
 *
 *  08-17-2002  tglx: fixed signed/unsigned missmatch in write.c
 *      Thanks to Ken Offer <koffer@arlut.utexas.edu>
 *
 *  08-29-2002  tglx: use buffered read/write only for non pagealigned
 *      access, speed up the aligned path by using the fs-buffer
 *      reset chip removed from nand_select(), implicit done
 *      only, when erase is interrupted
 *      waitfuntion use yield, instead of schedule_timeout
 *      support for 6byte/512byte hardware ECC
 *      read_ecc, write_ecc extended for different oob-layout
 *      selections: Implemented NAND_NONE_OOB, NAND_JFFS2_OOB,
 *      NAND_YAFFS_OOB. fs-driver gives one of these constants
 *      to select the oob-layout fitting the filesystem.
 *      oobdata can be read together with the raw data, when
 *      the fs-driver supplies a big enough buffer.
 *      size = 12 * number of pages to read (256B pagesize)
 *             24 * number of pages to read (512B pagesize)
 *      the buffer contains 8/16 byte oobdata and 4/8 byte
 *      returncode from calculate_ecc
 *      oobdata can be given from filesystem to program them
 *      in one go together with the raw data. ECC codes are
 *      filled in at the place selected by oobsel.
 *
 *  09-04-2002  tglx: fixed write_verify (John Hall (john.hall@optionexist.co.uk))
 *
 *  11-11-2002  tglx: fixed debug output in nand_write_page
 *      (John Hall (john.hall@optionexist.co.uk))
 *
 *  11-25-2002  tglx: Moved device ID/ manufacturer ID from nand_ids.h
 *      Splitted device ID and manufacturer ID table.
 *      Removed CONFIG_MTD_NAND_ECC, as it defaults to ECC_NONE for
 *      mtd->read / mtd->write and is controllable by the fs driver
 *      for mtd->read_ecc / mtd->write_ecc
 *      some minor cleanups
 *
 *  12-05-2002  tglx: Dave Ellis (DGE@sixnetio) provided the fix for
 *      WRITE_VERIFY long time ago. Thanks for remembering me.
 *
 *  02-14-2003  tglx: Reject non page aligned writes
 *      Fixed ecc select in nand_write_page to match semantics.
 *
 *  02-18-2003  tglx: Changed oobsel to pointer. Added a default oob-selector
 *
 *  02-18-2003  tglx: Implemented oobsel again. Now it uses a pointer to
 +      a structure, which will be supplied by a filesystem driver
 *      If NULL is given, then the defaults (none or defaults
 *      supplied by ioctl (MEMSETOOBSEL) are used.
 *      For partitions the partition defaults are used (mtdpart.c)
 *
 *  06-04-2003  tglx: fix compile errors and fix write verify problem for
 *      some chips, which need either a delay between the readback
 *      and the next write command or have the CE removed. The
 *      CE disable/enable is much faster than a 20us delay and
 *      it should work on all available chips.
 *
 * $Id: nand.c,v 1.1.1.1 2010-09-22 00:23:47 jmaggard Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/compatmac.h>
#include <linux/interrupt.h>
#include <asm/io.h>

extern unsigned int g_sys_type;
static int nand_init_bbt_start;
int vpd392_override = 0;
int samsung_rev_c_support = 0;
int bbn_previous=0xffff, bbn_current, bbn_counter=0, bbt_reconstruct=0;

struct bad_block_table bbt;

static int get_good_page(int page)
{
    int block = page >> 5;
    if( nand_init_bbt_start)
        return page;
    if( bbt.real_block[block] < 0)
        return -1;
    return (bbt.real_block[block] << 5) + (page & 31);
}

/*
 * Macros for low-level register control
 */
#define nand_select(x)  this->hwcontrol(x, NAND_CTL_SETNCE);
#define nand_deselect(x) this->hwcontrol(x, NAND_CTL_CLRNCE);

/*
 * NAND low-level MTD interface functions
 */
static int nand_read (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf);
static int nand_read_ecc (struct mtd_info *mtd, loff_t from, size_t len,
              size_t * retlen, u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel);
static int nand_read_oob (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf);
static int nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf);
static int nand_write_ecc (struct mtd_info *mtd, loff_t to, size_t len,
               size_t * retlen, const u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel);
static int bbt_nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf);
static int bbt_nand_write_ecc (struct mtd_info *mtd, loff_t to, size_t len,
        size_t * retlen, const u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel);
static int nand_write_oob (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char *buf);
static int nand_writev (struct mtd_info *mtd, const struct iovec *vecs,
            unsigned long count, loff_t to, size_t * retlen);
static int nand_writev_ecc (struct mtd_info *mtd, const struct iovec *vecs,
            unsigned long count, loff_t to, size_t * retlen, u_char *eccbuf, struct nand_oobinfo *oobsel);
static int nand_erase (struct mtd_info *mtd, struct erase_info *instr);
static void nand_sync (struct mtd_info *mtd);
static int nand_write_page (struct mtd_info *mtd, struct nand_chip *this, int page, u_char *oob_buf,  struct nand_oobinfo *oobsel);
static int bbt_nand_write_page (struct mtd_info *mtd, struct nand_chip *this, int page, u_char *oob_buf,  struct nand_oobinfo *oobsel);
void nand_init_bbt(struct mtd_info *mtd);

/*
 * Send command to NAND device
 */
static void nand_command (struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
    register struct nand_chip *this = mtd->priv;
    register unsigned long NAND_IO_ADDR = this->IO_ADDR_W;

    if(g_sys_type == SYSTEM_TYPE_PZERO)
    {
        NAND_IO_ADDR += 0x10000;
    }

    /* Begin command latch cycle */
    this->hwcontrol (mtd, NAND_CTL_SETCLE);
    /*
     * Write out the command to the device.
     */
    if (command != NAND_CMD_SEQIN)
        writeb (command, NAND_IO_ADDR);
    else {
        if (mtd->oobblock == 256 && column >= 256) {
            column -= 256;
            writeb (NAND_CMD_READOOB, NAND_IO_ADDR);
            writeb (NAND_CMD_SEQIN, NAND_IO_ADDR);
        } else if (mtd->oobblock == 512 && column >= 256) {
            if (column < 512) {
                column -= 256;
                writeb (NAND_CMD_READ1, NAND_IO_ADDR);
                writeb (NAND_CMD_SEQIN, NAND_IO_ADDR);
            } else {
                column -= 512;
                writeb (NAND_CMD_READOOB, NAND_IO_ADDR);
                writeb (NAND_CMD_SEQIN, NAND_IO_ADDR);
            }
        } else {
            writeb (NAND_CMD_READ0, NAND_IO_ADDR);
            writeb (NAND_CMD_SEQIN, NAND_IO_ADDR);
        }
    }

    if(g_sys_type == SYSTEM_TYPE_PZERO)
    {
        NAND_IO_ADDR += 0x10000;
    }

    /* Set ALE and clear CLE to start address cycle */
    this->hwcontrol (mtd, NAND_CTL_CLRCLE);

    if (column != -1 || page_addr != -1) {
        this->hwcontrol (mtd, NAND_CTL_SETALE);

        if( page_addr != -1)
            page_addr = get_good_page(page_addr);

        /* Serially input address */
        if (column != -1)
            writeb (column, NAND_IO_ADDR);
        if (page_addr != -1) {
            writeb ((unsigned char) (page_addr & 0xff), NAND_IO_ADDR);
            writeb ((unsigned char) ((page_addr >> 8) & 0xff), NAND_IO_ADDR);
            /* One more address cycle for higher density devices */
            if (mtd->size & 0x0c000000)
                writeb ((unsigned char) ((page_addr >> 16) & 0x0f), NAND_IO_ADDR);
        }
        /* Latch in address */
        this->hwcontrol (mtd, NAND_CTL_CLRALE);
    }
    if(g_sys_type == SYSTEM_TYPE_PZERO)
    {
        NAND_IO_ADDR -= 0x10000;
    }
    /*
     * program and erase have their own busy handlers
     * status and sequential in needs no delay
    */
    switch (command) {

    case NAND_CMD_PAGEPROG:
    case NAND_CMD_ERASE1:
    case NAND_CMD_ERASE2:
    case NAND_CMD_SEQIN:
    case NAND_CMD_STATUS:
        return;

    case NAND_CMD_RESET:
        if (this->dev_ready)
            break;
        this->hwcontrol (mtd, NAND_CTL_SETCLE);
        writeb (NAND_CMD_STATUS, NAND_IO_ADDR);
        this->hwcontrol (mtd, NAND_CTL_CLRCLE);
        while ( !(readb (this->IO_ADDR_R) & 0x40));
        return;

    /* This applies to read commands */
    default:
        /*
         * If we don't have access to the busy pin, we apply the given
         * command delay
        */
        if (!this->dev_ready) {
            udelay (this->chip_delay);
            return;
        }
    }

    /* wait until command is processed */
    while (!this->dev_ready(mtd));
}

/*
 *  Get chip for selected access
 */
static inline void nand_get_chip (struct nand_chip *this, struct mtd_info *mtd, int new_state, int *erase_state)
{

    DECLARE_WAITQUEUE (wait, current);

    /*
     * Grab the lock and see if the device is available
     * For erasing, we keep the spinlock until the
     * erase command is written.
    */
retry:
    spin_lock_bh (&this->chip_lock);

    if (this->state == FL_READY) {
        this->state = new_state;
        if (new_state != FL_ERASING)
            spin_unlock_bh (&this->chip_lock);
        return;
    }

    if (this->state == FL_ERASING) {
        if (new_state != FL_ERASING) {
            this->state = new_state;
            spin_unlock_bh (&this->chip_lock);
            nand_select (mtd);  /* select in any case */
            this->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
            return;
        }
    }

    set_current_state (TASK_UNINTERRUPTIBLE);
    add_wait_queue (&this->wq, &wait);
    spin_unlock_bh (&this->chip_lock);
    schedule ();
    remove_wait_queue (&this->wq, &wait);
    goto retry;
}

/*
 * Wait for command done. This applies to erase and program only
 * Erase can take up to 400ms and program up to 20ms according to
 * general NAND and SmartMedia specs
 *
*/
static int nand_wait(struct mtd_info *mtd, struct nand_chip *this, int state)
{

    unsigned long   timeo = jiffies;
    int status;

    if (state == FL_ERASING) 
         timeo += (HZ * 400) / 1000;
    else
         timeo += (HZ * 20) / 1000;

    spin_lock_bh (&this->chip_lock);
    this->cmdfunc (mtd, NAND_CMD_STATUS, -1, -1);

    while (time_before(jiffies, timeo)) {
        /* Check, if we were interrupted */
        /* Attention: State handling is simplfied, do not check interrupt
        if (this->state != state) {
            spin_unlock_bh (&this->chip_lock);
            return 0;
        }
        if (this->dev_ready) {
            if (this->dev_ready (mtd))
                break;
        }
        */
        if (readb (this->IO_ADDR_R) & 0x40)
            break;

        spin_unlock_bh (&this->chip_lock);
        yield ();
        spin_lock_bh (&this->chip_lock);
    }
    status = (int) readb (this->IO_ADDR_R);
    spin_unlock_bh (&this->chip_lock);

    return status;
}

/*
 *  Nand_page_program function is used for write and writev !
 *  This function will always program a full page of data
 *  If you call it with a non page aligned buffer, you're lost :)
 */
static int nand_write_page (struct mtd_info *mtd, struct nand_chip *this, int page, u_char *oob_buf,  struct nand_oobinfo *oobsel)
{
    int     i, status;
    u_char  ecc_code[6], *oob_data=NULL;
    int     eccmode = NAND_ECC_SOFT;
    int     *oob_config = oobsel->eccpos;
    int     good_page;
    u_char *saved_data_poi;

    DEBUG (MTD_DEBUG_LEVEL3, "nand_write_page: page = %d oobblock = %d\n", (int) page, mtd->oobblock);
    /* pad oob area, if we have no oob buffer from fs-driver */
    if (!oob_buf) {
        oob_data = &this->data_buf[mtd->oobblock];
        for (i = 0; i < mtd->oobsize; i++)
            oob_data[i] = 0xff;
    } else
        oob_data = oob_buf;

    /* Send command to begin auto page programming */
    this->cmdfunc (mtd, NAND_CMD_SEQIN, 0x00, page);

    /* Write out complete page of data, take care of eccmode */
    switch (eccmode) {
    /* No ecc and software ecc 3/256, write all */
    case NAND_ECC_NONE:
        printk (KERN_WARNING "Writing data without ECC to NAND-FLASH is not recommended\n");
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        break;
    case NAND_ECC_SOFT:
        this->calculate_ecc (mtd, &this->data_poi[0], &(ecc_code[0]));
        for (i = 0; i < 3; i++)
            oob_data[oob_config[i]] = ecc_code[i];
        /* Calculate and write the second ECC for 512 Byte page size */
        if (mtd->oobblock == 512) {
            this->calculate_ecc (mtd, &this->data_poi[256], &(ecc_code[3]));
            for (i = 3; i < 6; i++)
                oob_data[oob_config[i]] = ecc_code[i];
        }
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        break;

    /* Hardware ecc 3 byte / 256 data, write first half, get ecc, then second, if 512 byte pagesize */
    case NAND_ECC_HW3_256:
        this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic for write */
        for (i = 0; i < mtd->eccsize; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);

        this->calculate_ecc (mtd, NULL, &(ecc_code[0]));
        for (i = 0; i < 3; i++)
            oob_data[oob_config[i]] = ecc_code[i];

        if (mtd->oobblock == 512) {
            this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic for write*/
            for (i = mtd->eccsize; i < mtd->oobblock; i++)
                writeb ( this->data_poi[i] , this->IO_ADDR_W);
            this->calculate_ecc (mtd, NULL, &(ecc_code[3]));
            for (i = 3; i < 6; i++)
                oob_data[oob_config[i]] = ecc_code[i];
        }
        break;

    /* Hardware ecc 3 byte / 512 byte data, write full page */
    case NAND_ECC_HW3_512:
        this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic */
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        this->calculate_ecc (mtd, NULL, &(ecc_code[0]));
        for (i = 0; i < 3; i++)
            oob_data[oob_config[i]] = ecc_code[i];
        break;

    /* Hardware ecc 6 byte / 512 byte data, write full page */
    case NAND_ECC_HW6_512:
        this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic */
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        this->calculate_ecc (mtd, NULL, &(ecc_code[0]));
        for (i = 0; i < 6; i++)
            oob_data[oob_config[i]] = ecc_code[i];
        break;

    default:
        printk (KERN_WARNING "Invalid NAND_ECC_MODE %d\n", this->eccmode);
        BUG();
    }

    /* Write out OOB data */
    for (i = 0; i <  mtd->oobsize; i++){
        writeb (oob_data[i] , this->IO_ADDR_W);
    }

    /* Send command to actually program the data */
    this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);

    /* call wait ready function */
    status = this->waitfunc (mtd, this, FL_WRITING);

    /* See if device thinks it succeeded */
    if (status & 0x01) {
        DEBUG (MTD_DEBUG_LEVEL0, "%s: " "Failed write, page 0x%08x, ", __FUNCTION__, page);
        goto write_or_verify_failed;
        return -EIO;
    }

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
    /*
     * The NAND device assumes that it is always writing to
     * a cleanly erased page. Hence, it performs its internal
     * write verification only on bits that transitioned from
     * 1 to 0. The device does NOT verify the whole page on a
     * byte by byte basis. It is possible that the page was
     * not completely erased or the page is becoming unusable
     * due to wear. The read with ECC would catch the error
     * later when the ECC page check fails, but we would rather
     * catch it early in the page write stage. Better to write
     * no data than invalid data.
     */

    /* Send command to read back the page */
    this->cmdfunc (mtd, NAND_CMD_READ0, 0, page);
    /* Loop through and verify the data */
    for (i = 0; i < mtd->oobblock; i++) {
        if (this->data_poi[i] != readb (this->IO_ADDR_R)) {
            DEBUG (MTD_DEBUG_LEVEL0, "%s: " "Failed write verify, page 0x%08x ", __FUNCTION__, page);
            goto write_or_verify_failed;
            return -EIO;
        }
    }

    /* check, if we have a fs-supplied oob-buffer */
    if (oob_buf) {
        for (i = 0; i < mtd->oobsize; i++) {
            if (oob_data[i] != readb (this->IO_ADDR_R)) {
                DEBUG (MTD_DEBUG_LEVEL0, "%s: " "Failed write verify, page 0x%08x ", __FUNCTION__, page);
                goto write_or_verify_failed;
                return -EIO;
            }
        }
    } else {
        if (eccmode != NAND_ECC_NONE) {
            int ecc_bytes = 0;

            switch (this->eccmode) {
            case NAND_ECC_SOFT:
            case NAND_ECC_HW3_256: ecc_bytes = (mtd->oobblock == 512) ? 6 : 3; break;
            case NAND_ECC_HW3_512: ecc_bytes = 3; break;
            case NAND_ECC_HW6_512: ecc_bytes = 6; break;
            }

            for (i = 0; i < mtd->oobsize; i++)
                oob_data[i] = readb (this->IO_ADDR_R);

            for (i = 0; i < ecc_bytes; i++) {
                if (oob_data[oob_config[i]] != ecc_code[i]) {
                    DEBUG (MTD_DEBUG_LEVEL0,
                           "%s: Failed ECC write "
                       "verify, page 0x%08x, " "%6i bytes were succesful\n", __FUNCTION__, page, i);
                goto write_or_verify_failed;
                return -EIO;
                }
            }
        }
    }
    /*
     * Terminate the read command. This is faster than sending a reset command or
     * applying a 20us delay before issuing the next programm sequence.
     * This is not a problem for all chips, but I have found a bunch of them.
     */
    nand_deselect(mtd);
    if (samsung_rev_c_support) udelay (this->chip_delay);
    nand_select(mtd);
    if (samsung_rev_c_support) udelay (this->chip_delay);
#endif
    return 0;

/* If write or verify keep failing on same block for 3 times, do following
   1. mark this block as bad block
   2. re-do bad block init
   3. always erase before program bbt entry(if needed)
   4. always erase before program bbt table(if needed)
*/


write_or_verify_failed:
// nand is alreay locked for page write, no need to take care lock/unlock here
    good_page = get_good_page(page);


    // Check how many times it fails on the block
    bbn_current = good_page / (mtd->erasesize / mtd->oobblock);
    if(bbn_current == bbn_previous) bbn_counter += 1;
     else bbn_counter = 0;
    bbn_previous = bbn_current;
    if(bbn_counter < 3) return -EIO;


    // too many fails, mark it bad and reconstruct bad block table
    printk("Mark page %d(%d) bad\n", page, good_page);
    saved_data_poi = this->data_poi;
    bbn_counter = 0;
    // select device: use deselect and select to terminate possible pending read command
    nand_deselect(mtd);
    nand_select(mtd);
    // issue oob page write command
    this->state = FL_WRITING;
    this->cmdfunc (mtd, NAND_CMD_SEQIN, mtd->oobblock, good_page);
    // blindly write 0xde to entire oob block
    for(i=0; i< mtd->oobsize; i++) writeb (0xde, this->IO_ADDR_W);
    // program it
    this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);
    // wait ready
    status = this->waitfunc (mtd, this, FL_WRITING);
    // issue oob page write command
    this->cmdfunc (mtd, NAND_CMD_SEQIN, mtd->oobblock, good_page+1);
    // blindly write 0xad to entire oob block
    for(i=0; i< mtd->oobsize; i++) writeb (0xad, this->IO_ADDR_W);
    // program it
    this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);
    // wait ready
    status = this->waitfunc (mtd, this, FL_WRITING);
    this->state = FL_READY;

    // re-construct bad block table
    bbt_reconstruct = 1;
    nand_init_bbt(mtd);
    bbt_reconstruct = 0;

    // Restore state, Return error, expect retry to be succeed
    nand_select(mtd);
    this->state = FL_WRITING;
    this->data_poi = saved_data_poi;
    return -EIO;
}

static int bbt_nand_write_page (struct mtd_info *mtd, struct nand_chip *this, int page, u_char *oob_buf,  struct nand_oobinfo *oobsel)
{
    int     i, status;
    u_char  ecc_code[6], *oob_data;
    int     eccmode = NAND_ECC_SOFT;
    int     *oob_config = oobsel->eccpos;

    DEBUG (MTD_DEBUG_LEVEL0, "bbt_nand_write_page: page = %d oobblock = %d\n", (int) page, mtd->oobblock);
    /* pad oob area, if we have no oob buffer from fs-driver */
    if (!oob_buf) {
        oob_data = &this->data_buf[mtd->oobblock];
        for (i = 0; i < mtd->oobsize; i++)
            oob_data[i] = 0xff;
    } else
        oob_data = oob_buf;

    /* Send command to begin auto page programming */
    this->cmdfunc (mtd, NAND_CMD_SEQIN, 0x00, page);

    /* Write out complete page of data, take care of eccmode */
    switch (eccmode) {
    /* No ecc and software ecc 3/256, write all */
    case NAND_ECC_NONE:
        printk (KERN_WARNING "%s: " "Writing data without ECC to NAND-FLASH is not recommended at page %d\n", __FUNCTION__, page);
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        break;
    case NAND_ECC_SOFT:
        this->calculate_ecc (mtd, &this->data_poi[0], &(ecc_code[0]));
        for (i = 0; i < 3; i++)
            oob_data[oob_config[i]] = ecc_code[i];
        /* Calculate and write the second ECC for 512 Byte page size */
        if (mtd->oobblock == 512) {
            this->calculate_ecc (mtd, &this->data_poi[256], &(ecc_code[3]));
            for (i = 3; i < 6; i++)
                oob_data[oob_config[i]] = ecc_code[i];
        }
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        break;

    /* Hardware ecc 3 byte / 256 data, write first half, get ecc, then second, if 512 byte pagesize */
    case NAND_ECC_HW3_256:
        this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic for write */
        for (i = 0; i < mtd->eccsize; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);

        this->calculate_ecc (mtd, NULL, &(ecc_code[0]));
        for (i = 0; i < 3; i++)
            oob_data[oob_config[i]] = ecc_code[i];

        if (mtd->oobblock == 512) {
            this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic for write*/
            for (i = mtd->eccsize; i < mtd->oobblock; i++)
                writeb ( this->data_poi[i] , this->IO_ADDR_W);
            this->calculate_ecc (mtd, NULL, &(ecc_code[3]));
            for (i = 3; i < 6; i++)
                oob_data[oob_config[i]] = ecc_code[i];
        }
        break;

    /* Hardware ecc 3 byte / 512 byte data, write full page */
    case NAND_ECC_HW3_512:
        this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic */
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        this->calculate_ecc (mtd, NULL, &(ecc_code[0]));
        for (i = 0; i < 3; i++)
            oob_data[oob_config[i]] = ecc_code[i];
        break;

    /* Hardware ecc 6 byte / 512 byte data, write full page */
    case NAND_ECC_HW6_512:
        this->enable_hwecc (mtd, NAND_ECC_WRITE);   /* enable hardware ecc logic */
        for (i = 0; i < mtd->oobblock; i++)
            writeb ( this->data_poi[i] , this->IO_ADDR_W);
        this->calculate_ecc (mtd, NULL, &(ecc_code[0]));
        for (i = 0; i < 6; i++)
            oob_data[oob_config[i]] = ecc_code[i];
        break;

    default:
        printk (KERN_WARNING "Invalid NAND_ECC_MODE %d\n", this->eccmode);
        BUG();
    }

    /* Write out OOB data */
    for (i = 0; i <  mtd->oobsize; i++)
        writeb ( oob_data[i] , this->IO_ADDR_W);

    /* Send command to actually program the data */
    this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);

    /* call wait ready function */
    status = this->waitfunc (mtd, this, FL_WRITING);

    /* See if device thinks it succeeded */
    if (status & 0x01) {
        DEBUG (MTD_DEBUG_LEVEL0, "%s: " "Failed write, page 0x%08x, ", __FUNCTION__, page);
        return -EIO;
    }

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
    /*
     * The NAND device assumes that it is always writing to
     * a cleanly erased page. Hence, it performs its internal
     * write verification only on bits that transitioned from
     * 1 to 0. The device does NOT verify the whole page on a
     * byte by byte basis. It is possible that the page was
     * not completely erased or the page is becoming unusable
     * due to wear. The read with ECC would catch the error
     * later when the ECC page check fails, but we would rather
     * catch it early in the page write stage. Better to write
     * no data than invalid data.
     */

    /* Send command to read back the page */
    this->cmdfunc (mtd, NAND_CMD_READ0, 0, page);
    /* Loop through and verify the data */
    for (i = 0; i < mtd->oobblock; i++) {
        if (this->data_poi[i] != readb (this->IO_ADDR_R)) {
            DEBUG (MTD_DEBUG_LEVEL0, "%s: " "Failed write verify, page 0x%08x ", __FUNCTION__, page);
            return -EIO;
        }
    }

    /* check, if we have a fs-supplied oob-buffer */
    if (oob_buf) {
        for (i = 0; i < mtd->oobsize; i++) {
            if (oob_data[i] != readb (this->IO_ADDR_R)) {
                DEBUG (MTD_DEBUG_LEVEL0, "%s: " "Failed write verify, page 0x%08x ", __FUNCTION__, page);
                return -EIO;
            }
        }
    } else {
        if (eccmode != NAND_ECC_NONE) {
            int ecc_bytes = 0;

            switch (this->eccmode) {
            case NAND_ECC_SOFT:
            case NAND_ECC_HW3_256: ecc_bytes = (mtd->oobblock == 512) ? 6 : 3; break;
            case NAND_ECC_HW3_512: ecc_bytes = 3; break;
            case NAND_ECC_HW6_512: ecc_bytes = 6; break;
            }

            for (i = 0; i < mtd->oobsize; i++)
                oob_data[i] = readb (this->IO_ADDR_R);

            for (i = 0; i < ecc_bytes; i++) {
                if (oob_data[oob_config[i]] != ecc_code[i]) {
                    DEBUG (MTD_DEBUG_LEVEL0,
                           "%s: Failed ECC write "
                       "verify, page 0x%08x, " "%6i bytes were succesful\n", __FUNCTION__, page, i);
                return -EIO;
                }
            }
        }
    }
    /*
     * Terminate the read command. This is faster than sending a reset command or
     * applying a 20us delay before issuing the next programm sequence.
     * This is not a problem for all chips, but I have found a bunch of them.
     */
    nand_deselect(mtd);
    nand_select(mtd);
#endif
    return 0;

}

/*
*   Use NAND read ECC
*/
static int nand_read (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
    return (nand_read_ecc (mtd, from, len, retlen, buf, NULL, NULL));
}


/*
 * NAND read with ECC
 */
static int nand_read_ecc (struct mtd_info *mtd, loff_t from, size_t len,
              size_t * retlen, u_char * buf, u_char * oob_buf, struct nand_oobinfo *oobsel)
{
    int j, col, page, end, ecc;
    int erase_state = 0;
    int read = 0, oob = 0, ecc_status = 0, ecc_failed = 0, ecc_corr = 0, ecc_igno = 0;
    struct nand_chip *this = mtd->priv;
    u_char *data_poi, *oob_data = oob_buf;
    u_char ecc_calc[6];
    u_char ecc_code[6];
    int     eccmode=0;
    int *oob_config;

    // use chip default if zero
    if (oobsel == NULL)
        oobsel = &mtd->oobinfo;

    eccmode = oobsel->useecc ? this->eccmode : NAND_ECC_NONE;
    oob_config = oobsel->eccpos;

    //DEBUG (MTD_DEBUG_LEVEL0, "nand_read_ecc: from = 0x%08x, len = %d ecc = %d\n", (unsigned int) from, (int) len, eccmode);

    /* Do not allow reads past end of device */
    if ((from + len) > mtd->size) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_read_ecc: Attempt read beyond end of device\n");
        *retlen = 0;
        return -EINVAL;
    }

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd ,FL_READING, &erase_state);

    /* Select the NAND device */
    nand_select (mtd);

    /* First we calculate the starting page */
    page = from >> this->page_shift;

    /* Get raw starting column */
    col = from & (mtd->oobblock - 1);

    end = mtd->oobblock;
    ecc = mtd->eccsize;

    /* Send the read command */
    this->cmdfunc (mtd, NAND_CMD_READ0, 0x00, page);

    /* Loop until all data read */
    while (read < len) {

        /* If we have consequent page reads, apply delay or wait for ready/busy pin */
        if (read) {
            if (!this->dev_ready)
                udelay (this->chip_delay);
            else
                while (!this->dev_ready(mtd));
        }

        /*
         * If the read is not page aligned, we have to read into data buffer
         * due to ecc, else we read into return buffer direct
         */
        if (!col && (len - read) >= end) {
            data_poi = &buf[read];
        }
        else {
            data_poi = this->data_buf;
        }

        /* get oob area, if we have no oob buffer from fs-driver */
        if (!oob_buf) {
            oob_data = &this->data_buf[end];
            oob = 0;
        }

        j = 0;
        switch (eccmode) {
        case NAND_ECC_NONE: /* No ECC, Read in a page */
            //printk (KERN_WARNING "Reading data from NAND FLASH without ECC is not recommended\n");
            while (j < end){
                data_poi[j++] = readb (this->IO_ADDR_R);
            }
            break;

        case NAND_ECC_SOFT: /* Software ECC 3/256: Read in a page + oob data */
            while (j < end)
                data_poi[j++] = readb (this->IO_ADDR_R);
            this->calculate_ecc (mtd, &data_poi[0], &ecc_calc[0]);
            if (mtd->oobblock == 512)
                this->calculate_ecc (mtd, &data_poi[256], &ecc_calc[3]);
            break;

        case NAND_ECC_HW3_256: /* Hardware ECC 3 byte /256 byte data: Read in first 256 byte, get ecc, */
            this->enable_hwecc (mtd, NAND_ECC_READ);
            while (j < ecc)
                data_poi[j++] = readb (this->IO_ADDR_R);
            this->calculate_ecc (mtd, &data_poi[0], &ecc_calc[0]);  /* read from hardware */

            if (mtd->oobblock == 512) { /* read second, if pagesize = 512 */
                this->enable_hwecc (mtd, NAND_ECC_READ);
                while (j < end)
                    data_poi[j++] = readb (this->IO_ADDR_R);
                this->calculate_ecc (mtd, &data_poi[256], &ecc_calc[3]); /* read from hardware */
            }
            break;

        case NAND_ECC_HW3_512:
        case NAND_ECC_HW6_512: /* Hardware ECC 3/6 byte / 512 byte data : Read in a page  */
            this->enable_hwecc (mtd, NAND_ECC_READ);
            while (j < end)
                data_poi[j++] = readb (this->IO_ADDR_R);
            this->calculate_ecc (mtd, &data_poi[0], &ecc_calc[0]);  /* read from hardware */
            break;

        default:
            printk (KERN_WARNING "Invalid NAND_ECC_MODE %d\n", this->eccmode);
            BUG();
        }

        /* read oobdata */
        for (j = 0; j <  mtd->oobsize; j++)
        {
            oob_data[oob + j] = readb (this->IO_ADDR_R);
        }

        /* Skip ECC, if not active */
        if (eccmode == NAND_ECC_NONE)
            goto readdata;

        /* Pick the ECC bytes out of the oob data */
        for (j = 0; j < 6; j++)
        {
            ecc_code[j] = oob_data[oob + oob_config[j]];
        }

        /* correct data, if neccecary */
        ecc_status = this->correct_data (mtd, &data_poi[0], &ecc_code[0], &ecc_calc[0]);
        /* check, if we have a fs supplied oob-buffer */
        if (oob_buf) {
            oob += mtd->oobsize;
            *((int *)&oob_data[oob]) = ecc_status;
            oob += sizeof(int);
        }

        switch(ecc_status){
            case 0:
                //DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC good page 0x%08x 1st half\n", page);
                break;
            case -1:
                DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC BAD page 0x%08x 1st half\n", page);
                ecc_failed++;
                break;
            case 1:
                DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC recover page 0x%08x 1st half\n", page);
                ecc_corr++;
                break;
            case 2:
                DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC ignore page 0x%08x 1st half\n", page);
                ecc_igno++;
                break;
            default:
                break;
        }

        if (mtd->oobblock == 512 && eccmode != NAND_ECC_HW3_512) {
            ecc_status = this->correct_data (mtd, &data_poi[256], &ecc_code[3], &ecc_calc[3]);
            switch(ecc_status){
                case 0:
                    //DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC good page 0x%08x 2nd half\n", page);
                    break;
                case -1:
                    DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC BAD page 0x%08x 2nd half\n", page);
                    ecc_failed++;
                    break;
                case 1:
                    DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC recover page 0x%08x 2nd half\n", page);
                    ecc_corr++;
                    break;
                case 2:
                    DEBUG (MTD_DEBUG_LEVEL0, "NAND ECC ignore page 0x%08x 2nd half\n", page);
                    ecc_igno++;
                    break;
                default:
                    break;
            }
            if (oob_buf) {
                *((int *)&oob_data[oob]) = ecc_status;
                oob += sizeof(int);
            }
        }
readdata:
        if (col || (len - read) < end) {
            for (j = col; j < end && read < len; j++)
                buf[read++] = data_poi[j];
        } else
            read += mtd->oobblock;
        /* For subsequent reads align to page boundary. */
        col = 0;
        /* Increment page address */
        page++;
    }

    /* De-select the NAND device */
    nand_deselect (mtd);

    /* Wake up anyone waiting on the device */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    wake_up (&this->wq);
    spin_unlock_bh (&this->chip_lock);

    /*
     * Return success, if no ECC failures, else -EIO
     * fs driver will take care of that, because
     * retlen == desired len and result == -EIO
     */
    *retlen = read;
    return ecc_failed ? -EIO : 0;
}

/*
 * NAND read out-of-band
 */
static int nand_read_oob (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
    int i, col, page;
    int erase_state = 0;
    struct nand_chip *this = mtd->priv;

    DEBUG (MTD_DEBUG_LEVEL3, "nand_read_oob: from = 0x%08x, len = %i\n", (unsigned int) from, (int) len);

    /* Shift to get page */
    page = ((int) from) >> this->page_shift;

    /* Mask to get column */
    col = from & 0x0f;

    /* Initialize return length value */
    *retlen = 0;

    /* Do not allow reads past end of device */
    if ((from + len) > mtd->size) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_read_oob: Attempt read beyond end of device\n");
        *retlen = 0;
        return -EINVAL;
    }

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd , FL_READING, &erase_state);

    /* Select the NAND device */
    nand_select (mtd);

    /* Send the read command */
    this->cmdfunc (mtd, NAND_CMD_READOOB, col, page);
    /*
     * Read the data, if we read more than one page
     * oob data, let the device transfer the data !
     */
    for (i = 0; i < len; i++) {
        buf[i] = readb (this->IO_ADDR_R);
        if ((col++ & (mtd->oobsize - 1)) == (mtd->oobsize - 1))
            udelay (this->chip_delay);
    }
    /* De-select the NAND device */
    nand_deselect (mtd);

    /* Wake up anyone waiting on the device */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    wake_up (&this->wq);
    spin_unlock_bh (&this->chip_lock);

    /* Return happy */
    *retlen = len;
    return 0;
}

#define NOTALIGNED(x) (x & (mtd->oobblock-1)) != 0

/*
*   Use NAND write ECC
*/
static int nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
    return (nand_write_ecc (mtd, to, len, retlen, buf, NULL, NULL));
}
static int bbt_nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
    return (bbt_nand_write_ecc (mtd, to, len, retlen, buf, NULL, NULL));
}
/*
 * NAND write with ECC
 */
static int nand_write_ecc (struct mtd_info *mtd, loff_t to, size_t len,
               size_t * retlen, const u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel)
{
    int page, ret = 0, oob = 0, written = 0;
    struct nand_chip *this = mtd->priv;

    DEBUG (MTD_DEBUG_LEVEL1, "nand_write_ecc: to = 0x%08x, len = %i\n", (unsigned int) to, (int) len);

    /* Do not allow write past end of device */
    if ((to + len) > mtd->size) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_write_ecc: Attempt to write past end of page\n");
        return -EINVAL;
    }

    /* reject writes, which are not page aligned */
    if (NOTALIGNED (to) || NOTALIGNED(len)) {
        printk (KERN_NOTICE "nand_write_ecc: Attempt to write not page aligned data\n");
        return -EINVAL;
    }

    // if oobsel is NULL, use chip defaults
    if (oobsel == NULL)
        oobsel = &mtd->oobinfo;

    /* Shift to get page */
    page = ((int) to) >> this->page_shift;

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd, FL_WRITING, NULL);

    /* Select the NAND device */
    nand_select (mtd);

    /* Check the WP bit */
    this->cmdfunc (mtd, NAND_CMD_STATUS, -1, -1);
    if (!(readb (this->IO_ADDR_R) & 0x80)) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_write_ecc: Device is write protected!!!\n");
        ret = -EIO;
        goto out;
    }

    /* Loop until all data is written */
    while (written < len) {
        int cnt = mtd->oobblock;
        this->data_poi = (u_char*) &buf[written];
        /* We use the same function for write and writev */
        if (eccbuf) {
            ret = nand_write_page (mtd, this, page, &eccbuf[oob], oobsel);
            oob += mtd->oobsize;
        } else
            ret = nand_write_page (mtd, this, page, NULL, oobsel);

        if (ret)
            goto out;

        /* Update written bytes count */
        written += cnt;
        /* Increment page address */
        page++;
    }

out:
    /* De-select the NAND device */
    nand_deselect (mtd);

    /* Wake up anyone waiting on the device */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    wake_up (&this->wq);
    spin_unlock_bh (&this->chip_lock);

    *retlen = written;
    return ret;
}

static int bbt_nand_write_ecc (struct mtd_info *mtd, loff_t to, size_t len,
               size_t * retlen, const u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel)
{
    int page, ret = 0, oob = 0, written = 0;
    struct nand_chip *this = mtd->priv;

    DEBUG (MTD_DEBUG_LEVEL3, "bbt_nand_write_ecc: to = 0x%08x, len = %i\n", (unsigned int) to, (int) len);

    /* Do not allow write past end of device */
    if ((to + len) > mtd->size) {
        DEBUG (MTD_DEBUG_LEVEL0, "bbt_nand_write_ecc: Attempt to write past end of page\n");
        return -EINVAL;
    }

    /* reject writes, which are not page aligned */
    if (NOTALIGNED (to) || NOTALIGNED(len)) {
        printk (KERN_NOTICE "bbt_nand_write_ecc: Attempt to write not page aligned data\n");
        return -EINVAL;
    }

    // if oobsel is NULL, use chip defaults
    if (oobsel == NULL)
        oobsel = &mtd->oobinfo;

    /* Shift to get page */
    page = ((int) to) >> this->page_shift;

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd, FL_WRITING, NULL);

    /* Select the NAND device */
    nand_select (mtd);

    /* Check the WP bit */
    this->cmdfunc (mtd, NAND_CMD_STATUS, -1, -1);
    if (!(readb (this->IO_ADDR_R) & 0x80)) {
        DEBUG (MTD_DEBUG_LEVEL0, "bbt_nand_write_ecc: Device is write protected!!!\n");
        ret = -EIO;
        goto out;
    }

    /* Loop until all data is written */
    while (written < len) {
        int cnt = mtd->oobblock;
        this->data_poi = (u_char*) &buf[written];
        /* We use the same function for write and writev */
        if (eccbuf) {
            ret = bbt_nand_write_page (mtd, this, page, &eccbuf[oob], oobsel);
            oob += mtd->oobsize;
        } else
            ret = bbt_nand_write_page (mtd, this, page, NULL, oobsel);

        if (ret)
            goto out;

        /* Update written bytes count */
        written += cnt;
        /* Increment page address */
        page++;
    }

out:
    /* De-select the NAND device */
    nand_deselect (mtd);

    /* Wake up anyone waiting on the device */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    wake_up (&this->wq);
    spin_unlock_bh (&this->chip_lock);

    *retlen = written;
    return ret;
}
/*
 * NAND write out-of-band
 */
static int nand_write_oob (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
    int i, column, page, status, ret = 0;
    struct nand_chip *this = mtd->priv;

    DEBUG (MTD_DEBUG_LEVEL3, "nand_write_oob: to = 0x%08x, len = %i\n", (unsigned int) to, (int) len);

    /* Shift to get page */
    page = ((int) to) >> this->page_shift;

    /* Mask to get column */
    column = to & 0x1f;

    /* Initialize return length value */
    *retlen = 0;

    /* Do not allow write past end of page */
    if ((column + len) > mtd->oobsize) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_write_oob: Attempt to write past end of page\n");
        return -EINVAL;
    }

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd, FL_WRITING, NULL);

    /* Select the NAND device */
    nand_select (mtd);

    /* Check the WP bit */
    this->cmdfunc (mtd, NAND_CMD_STATUS, -1, -1);
    if (!(readb (this->IO_ADDR_R) & 0x80)) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_write_oob: Device is write protected!!!\n");
        ret = -EIO;
        goto out;
    }

    /* Write out desired data */
    this->cmdfunc (mtd, NAND_CMD_SEQIN, mtd->oobblock, page);
    /* prepad 0xff for partial programming */
    for (i = 0; i < column; i++)
        writeb (0xff, this->IO_ADDR_W);
    /* write data */
    for (i = 0; i < len; i++)
        writeb (buf[i], this->IO_ADDR_W);
    /* postpad 0xff for partial programming */
    for (i = len + column; i < mtd->oobsize; i++)
        writeb (0xff, this->IO_ADDR_W);

    /* Send command to program the OOB data */
    this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);

    status = this->waitfunc (mtd, this, FL_WRITING);

    /* See if device thinks it succeeded */
    if (status & 0x01) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_write_oob: " "Failed write, page 0x%08x\n", page);
        ret = -EIO;
        goto out;
    }
    /* Return happy */
    *retlen = len;

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
    /* Send command to read back the data */
    this->cmdfunc (mtd, NAND_CMD_READOOB, column, page);

    /* Loop through and verify the data */
    for (i = 0; i < len; i++) {
        if (buf[i] != readb (this->IO_ADDR_R)) {
            DEBUG (MTD_DEBUG_LEVEL0, "nand_write_oob: " "Failed write verify, page 0x%08x\n", page);
            ret = -EIO;
            goto out;
        }
    }
#endif

out:
    /* De-select the NAND device */
    nand_deselect (mtd);

    /* Wake up anyone waiting on the device */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    wake_up (&this->wq);
    spin_unlock_bh (&this->chip_lock);

    return ret;
}


/*
 * NAND write with iovec
 */
static int nand_writev (struct mtd_info *mtd, const struct iovec *vecs, unsigned long count,
        loff_t to, size_t * retlen)
{
    return (nand_writev_ecc (mtd, vecs, count, to, retlen, NULL, NULL));
}

static int nand_writev_ecc (struct mtd_info *mtd, const struct iovec *vecs, unsigned long count,
        loff_t to, size_t * retlen, u_char *eccbuf, struct nand_oobinfo *oobsel)
{
    int i, page, len, total_len, ret = 0, written = 0;
    struct nand_chip *this = mtd->priv;

    /* Calculate total length of data */
    total_len = 0;
    for (i = 0; i < count; i++)
        total_len += (int) vecs[i].iov_len;

    DEBUG (MTD_DEBUG_LEVEL0,
           "nand_writev: to = 0x%08x, len = %i, count = %ld\n", (unsigned int) to, (unsigned int) total_len, count);

    /* Do not allow write past end of page */
    if ((to + total_len) > mtd->size) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_writev: Attempted write past end of device\n");
        return -EINVAL;
    }

    /* reject writes, which are not page aligned */
    if (NOTALIGNED (to) || NOTALIGNED(total_len)) {
        printk (KERN_NOTICE "nand_write_ecc: Attempt to write not page aligned data\n");
        return -EINVAL;
    }

    // if oobsel is NULL, use chip defaults
    if (oobsel == NULL)
        oobsel = &mtd->oobinfo;

    /* Shift to get page */
    page = ((int) to) >> this->page_shift;

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd, FL_WRITING, NULL);

    /* Select the NAND device */
    nand_select (mtd);

    /* Check the WP bit */
    this->cmdfunc (mtd, NAND_CMD_STATUS, -1, -1);
    if (!(readb (this->IO_ADDR_R) & 0x80)) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_writev: Device is write protected!!!\n");
        ret = -EIO;
        goto out;
    }

    /* Loop until all iovecs' data has been written */
    len = 0;
    while (count) {
        /*
         *  Check, if the tuple gives us not enough data for a
         *  full page write. Then we can use the iov direct,
         *  else we have to copy into data_buf.
         */
        if ((vecs->iov_len - len) >= mtd->oobblock) {
            this->data_poi = (u_char *) vecs->iov_base;
            this->data_poi += len;
            len += mtd->oobblock;
            /* Check, if we have to switch to the next tuple */
            if (len >= (int) vecs->iov_len) {
                vecs++;
                len = 0;
                count--;
            }
        } else {
            /*
             * Read data out of each tuple until we have a full page
             * to write or we've read all the tuples.
            */
            int cnt = 0;
            while ((cnt < mtd->oobblock) && count) {
                if (vecs->iov_base != NULL && vecs->iov_len) {
                    this->data_buf[cnt++] = ((u_char *) vecs->iov_base)[len++];
                }
                /* Check, if we have to switch to the next tuple */
                if (len >= (int) vecs->iov_len) {
                    vecs++;
                    len = 0;
                    count--;
                }
            }
            this->data_poi = this->data_buf;
        }

        /* We use the same function for write and writev !) */
        ret = nand_write_page (mtd, this, page, NULL, oobsel);
        if (ret)
            goto out;

        /* Update written bytes count */
        written += mtd->oobblock;

        /* Increment page address */
        page++;
    }

out:
    /* De-select the NAND device */
    nand_deselect (mtd);

    /* Wake up anyone waiting on the device */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    wake_up (&this->wq);
    spin_unlock_bh (&this->chip_lock);

    *retlen = written;
    return ret;
}

/*
 * NAND erase a block
 */
static int nand_erase (struct mtd_info *mtd, struct erase_info *instr)
{
    int page, len, status, pages_per_block, ret;
    struct nand_chip *this = mtd->priv;
    DECLARE_WAITQUEUE (wait, current);

    DEBUG (MTD_DEBUG_LEVEL0,
           "nand_erase: start = 0x%08x, len = %i\n", (unsigned int) instr->addr, (unsigned int) instr->len);

    /* Start address must align on block boundary */
    if (instr->addr & (mtd->erasesize - 1)) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_erase: Unaligned address\n");
        return -EINVAL;
    }

    /* Length must align on block boundary */
    if (instr->len & (mtd->erasesize - 1)) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_erase: Length not block aligned\n");
        return -EINVAL;
    }

    /* Do not allow erase past end of device */
    if ((instr->len + instr->addr) > mtd->size) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_erase: Erase past end of device\n");
        return -EINVAL;
    }

    /* Grab the lock and see if the device is available */
    nand_get_chip (this, mtd, FL_ERASING, NULL);

    /* Shift to get first page */
    page = (int) (instr->addr >> this->page_shift);

    /* Calculate pages in each block */
    pages_per_block = mtd->erasesize / mtd->oobblock;

    /* Select the NAND device */
    nand_select (mtd);

    /* Check the WP bit */
    this->cmdfunc (mtd, NAND_CMD_STATUS, -1, -1);
    if (!(readb (this->IO_ADDR_R) & 0x80)) {
        DEBUG (MTD_DEBUG_LEVEL0, "nand_erase: Device is write protected!!!\n");
        instr->state = MTD_ERASE_FAILED;
        goto erase_exit;
    }

    /* Loop through the pages */
    len = instr->len;

    instr->state = MTD_ERASING;

    while (len) {
        /* Check if we have a bad block, we do not erase bad blocks ! */
        this->cmdfunc (mtd, NAND_CMD_READOOB, NAND_BADBLOCK_POS, page);
        if (readb (this->IO_ADDR_R) != 0xff) {
#ifdef CONFIG_PADRE
            printk (KERN_WARNING "nand_erase: attempt to erase a bad block at page 0x%08x(%08x)\n",
                 page, get_good_page(page));
            // zz:: with bbt support, we will try to erase bad block unless this block is
            // scaned as good but actually flaky
            // instr->state = MTD_ERASE_FAILED;
            // goto erase_exit;
#else
            printk (KERN_WARNING "nand_erase: attempt to erase a bad block at page 0x%08x\n", page);
            instr->state = MTD_ERASE_FAILED;
            goto erase_exit;
#endif
        }

        /* Send commands to erase a page */
        this->cmdfunc (mtd, NAND_CMD_ERASE1, -1, page);
        this->cmdfunc (mtd, NAND_CMD_ERASE2, -1, -1);

        spin_unlock_bh (&this->chip_lock);
        status = this->waitfunc (mtd, this, FL_ERASING);

        /* Get spinlock, in case we exit */
        spin_lock_bh (&this->chip_lock);
        /* See if block erase succeeded */
        if (status & 0x01) {
            DEBUG (MTD_DEBUG_LEVEL0, "nand_erase: " "Failed erase, page 0x%08x\n", page);
            instr->state = MTD_ERASE_FAILED;
            goto erase_exit;
        }

        /* Check, if we were interupted */
        if (this->state == FL_ERASING) {
            /* Increment page address and decrement length */
            len -= mtd->erasesize;
            page += pages_per_block;
        }
        /* Release the spin lock */
        spin_unlock_bh (&this->chip_lock);
erase_retry:
        spin_lock_bh (&this->chip_lock);
        /* Check the state and sleep if it changed */
        if (this->state == FL_ERASING || this->state == FL_READY) {
            /* Select the NAND device again, if we were interrupted */
            this->state = FL_ERASING;
            nand_select (mtd);
            continue;
        } else {
            set_current_state (TASK_UNINTERRUPTIBLE);
            add_wait_queue (&this->wq, &wait);
            spin_unlock_bh (&this->chip_lock);
            schedule ();
            remove_wait_queue (&this->wq, &wait);
            goto erase_retry;
        }
    }
    instr->state = MTD_ERASE_DONE;

erase_exit:
    /* De-select the NAND device */
    nand_deselect (mtd);
    spin_unlock_bh (&this->chip_lock);

    ret = instr->state == MTD_ERASE_DONE ? 0 : -EIO;
    /* Do call back function */
    if (!ret && instr->callback)
        instr->callback (instr);

    /* The device is ready */
    spin_lock_bh (&this->chip_lock);
    this->state = FL_READY;
    spin_unlock_bh (&this->chip_lock);

    /* Return more or less happy */
    return ret;
}


/*
 * NAND sync
 */
static void nand_sync (struct mtd_info *mtd)
{
    struct nand_chip *this = mtd->priv;
    DECLARE_WAITQUEUE (wait, current);

    DEBUG (MTD_DEBUG_LEVEL3, "nand_sync: called\n");

retry:
    /* Grab the spinlock */
    spin_lock_bh (&this->chip_lock);

    /* See what's going on */
    switch (this->state) {
    case FL_READY:
    case FL_SYNCING:
        this->state = FL_SYNCING;
        spin_unlock_bh (&this->chip_lock);
        break;

    default:
        /* Not an idle state */
        add_wait_queue (&this->wq, &wait);
        spin_unlock_bh (&this->chip_lock);
        schedule ();

        remove_wait_queue (&this->wq, &wait);
        goto retry;
    }

    /* Lock the device */
    spin_lock_bh (&this->chip_lock);

    /* Set the device to be ready again */
    if (this->state == FL_SYNCING) {
        this->state = FL_READY;
        wake_up (&this->wq);
    }

    /* Unlock the device */
    spin_unlock_bh (&this->chip_lock);
}

int nand_find_block(int block)
{
    int i;
    if( block >= bbt.total_good_blocks)
        return -1;
    for( i = 0 ; i < MAX_BLOCK - bbt.total_good_blocks; i++)
        if( bbt.table[i].bad_block == block)
            return bbt.table[i].good_block;
    return block;
}

void nand_build_bbt(void)
{
    int i;

    for( i = 0 ; i < 1024 ; i++)
        if(bbt.table[i].bad_block == 0xffff)
            break;
        else printk("bad block %d replacing by %d\n",
            bbt.table[i].bad_block,bbt.table[i].good_block);
    printk("total bad block %d\n", i);
    bbt.total_good_blocks = MAX_BLOCK - i;
    for( i = 0; i < MAX_BLOCK; i++)
        bbt.real_block[i] = nand_find_block(i);

    for( i = 0; i < MAX_BLOCK; i++) {
        if( bbt.real_block[i] != i)
             printk("bad %d = %d ", i, bbt.real_block[i]);
        if( i % 7 == 7) printk("\n");
    }

}

void nand_add_reserve_block(int block)
{
    int i;
    for( i = 1; i < bbt.total_reserve_blocks; i++)
        if( bbt.reserve_blocks[i-1] == block)
            return;

    for( i = bbt.total_reserve_blocks ; i > 0 ; i--) {
        if( bbt.reserve_blocks[i-1] < block)
            break;
        bbt.reserve_blocks[i] = bbt.reserve_blocks[i-1];
    }
    bbt.reserve_blocks[i] = block;
    bbt.total_reserve_blocks++;
}

int nand_get_last_good(void)
{
    int good , i ;

    good = MAX_BLOCK - 1;

    for( i = bbt.total_reserve_blocks-1; i >= 0 ; i--)
        if( bbt.reserve_blocks[i] == good )
            good--;
    nand_add_reserve_block(good);
    return good;
}

int nand_get_first_good(void)
{
    int good , i ;

    good = 0;

    for( i = 0; i < bbt.total_reserve_blocks ; i++)
        if( bbt.reserve_blocks[i] == good )
            good++;
    nand_add_reserve_block(good);
    return good;
}

#ifdef CONFIG_PADRE
void nand_init_bbt(struct mtd_info *mtd)
{
    int i, j, status;
    struct nand_chip *this = mtd->priv;
    loff_t from;
    u_char oob1,oob2,*pbuf;
    size_t retlen;
    unsigned long table_pos, table_pos_old;
    int bbt_rewrite_error;
    int bbt_need_modify, original_bad_one = 0;
    unsigned short checksum;

    nand_init_bbt_start = 1;
    printk("size of table %d\n", sizeof(bbt.table));

    // force create new bbt by comment out the next line
    nand_read(mtd, 508, 4, &retlen, (void *)&table_pos); //iboot1 is not ecc
    table_pos_old = table_pos & 0xffff;

    if( ((table_pos & 0xffff0000) == 0xdead0000) && (bbt_reconstruct == 0)) { // Normal existing bbt init
        printk("table is there 0x%0x\n", table_pos & 0xffff);
        table_pos &= 0xffff;
        nand_read(mtd,table_pos << 14, sizeof(bbt.table), &retlen, (void *)bbt.table);
        // Only bbt.table is saved! restore reserved block information here
        // 8 reserved
        // bbt reserved
        // bad and its replacement reserved
        // This code is for backward compatible, otherwise, I would save entire BBT other than table onlt
        bbt.total_reserve_blocks = 0;
        for( i = 0 ; i < 8; i++) nand_add_reserve_block(i);
        nand_add_reserve_block(table_pos);
        for( i = 0 ; i < 1024 ; i++) {
            if(bbt.table[i].bad_block == 0xffff) break;
            nand_add_reserve_block(bbt.table[i].bad_block);
            nand_add_reserve_block(bbt.table[i].good_block);
        }
    }
    else if(bbt_reconstruct == 0) {  // brand new bbt init
        printk("without table %08x\n", table_pos);
        table_pos &= 0xffff;
        bbt.total_reserve_blocks = 0;
        for( i = 0 ; i < 8; i++) nand_add_reserve_block(i);
        memset((void*)bbt.table, 0xff, sizeof(bbt.table));

        /* debug only erase all block but 1st and 2nd 
        for( i = 2 ; i < MAX_BLOCK; i++) {
           nand_select(mtd);
           this->state = FL_ERASING;
           this->cmdfunc (mtd, NAND_CMD_ERASE1, -1, i << 5);
           this->cmdfunc (mtd, NAND_CMD_ERASE2, -1, -1);
           status = this->waitfunc (mtd, this, FL_ERASING);
           this->state = FL_READY;
           
           if(status & 0x01) {
               printk("BBT re-construct failed: Erase block %d fail\n", i);
           } else {
               printk ("Erase block %d\n", i);
           }
        }
        */

        j = 0;
        for( i = 0 ; i < MAX_BLOCK; i++) {
            from = (i << (this->page_shift + 5)) + NAND_BADBLOCK_POS;
            nand_read_oob(mtd, from, 1, &retlen, &oob1);
            from = from + 512;
            nand_read_oob(mtd, from, 1, &retlen, &oob2);
            if( oob1 != 0xff || oob2 != 0xff) {
            /* dump bbt and its page
                int k;
                pbuf = kmalloc(528, GFP_KERNEL);
                if (oob1 != 0xff) from = from - 512;
                nand_read(mtd, from, 528, &retlen, pbuf);
                printk("retlen 0x%x\n", retlen);
                for (k=0; k<528; k++) {
                    if ((k & 0xf) == 0) {
                        printk("\n");
                        printk(" 0x%x -> ", k);
                    }
                    printk("%02x ",pbuf[k]);
                }
                nand_read_oob(mtd, from-5, 16, &retlen, pbuf);
                printk("retlen 0x%x", retlen);
                printk("\n oob\n");
                for (k=0; k<16; k++) {
                    printk("%02x ",pbuf[k]);
                }
                printk("\n");
                kfree(pbuf);
             */
                bbt.table[j].bad_block = i;
                nand_add_reserve_block(i);
                j++;
            }

        }
        printk("find %d bad block\n", j);
        table_pos = nand_get_first_good();

        for( i = 0 ; i < j ; i++)
            bbt.table[i].good_block = nand_get_last_good();

        pbuf = kmalloc(512, GFP_KERNEL);
        nand_read(mtd, 0, 512, &retlen, pbuf);
        *(unsigned long*)(pbuf+508) = (table_pos | 0xdead0000);
        nand_write(mtd, 0, 512, &retlen, pbuf);
        kfree(pbuf);
        // Note: bbt.table and bbt.reserve_blocks are valid
        // but bbt.real_block and total_good_blocks are not valid at this moment
        // we care only the frist two

        // Try to establish BBT onto NAND, erase first regardless its content
        nand_select(mtd);
        i = 0;
        do {
           nand_select(mtd);
           this->state = FL_ERASING;
           this->cmdfunc (mtd, NAND_CMD_ERASE1, -1, table_pos << 5);
           this->cmdfunc (mtd, NAND_CMD_ERASE2, -1, -1);
           status = this->waitfunc (mtd, this, FL_ERASING);
           this->state = FL_READY;
           if(status & 0x01) {
            printk("BBT re-construct failed: Erase BBT table fail\n");
            // get lost now, try write anyway
           }
           // write back
           bbt_rewrite_error = bbt_nand_write(mtd, table_pos << 14, sizeof(bbt.table), &retlen, (void *)bbt.table);
          if(bbt_rewrite_error) printk("BBT Init fail %d\n", i);
        } while( (i++ <3) && bbt_rewrite_error);
    }
    else {  // reconstruct: update bbt entry if needed
        printk("Re-Construct bbt due to write failure\n");
        // do not include this line in bbt retry loop
        table_pos = table_pos_old;

bbt_rewrite_retry:
        // Check if it is a replacing block for bad one, if yes,
        // bad block table need modify one entry and add a new entry,
        // if not, simply add a new entry
        bbt_need_modify = 0;
        for(i=0; i<(MAX_BLOCK - bbt.total_good_blocks); i++) {
            if(bbt.table[i].good_block == bbn_current) {
                bbt_need_modify = 1;
                original_bad_one = i;
                break;
            }
        }

        // Add a new entry
        bbt.table[MAX_BLOCK - bbt.total_good_blocks].bad_block = bbn_current;
        nand_add_reserve_block(bbn_current);
        bbt.table[MAX_BLOCK - bbt.total_good_blocks].good_block = nand_get_last_good();
        bbt.total_good_blocks --;

        // Modify old entry if neccessary, bbt.total_good_blocks need not modify though
        if(bbt_need_modify) {
            bbt.table[original_bad_one].good_block = nand_get_last_good();
        }

        // Try to update BBT onto NAND, erase first
        nand_select(mtd);
        i = 0;
        do {
           nand_select(mtd);
           this->state = FL_ERASING;
           this->cmdfunc (mtd, NAND_CMD_ERASE1, -1, table_pos << 5);
                   this->cmdfunc (mtd, NAND_CMD_ERASE2, -1, -1);
           status = this->waitfunc (mtd, this, FL_ERASING);
           this->state = FL_READY;
           if(status & 0x01) {
            printk("BBT re-construct failed: Erase BBT table fail\n");
            // get lost now, try write anyway
           }
           // write back
           bbt_rewrite_error = bbt_nand_write(mtd, table_pos << 14, sizeof(bbt.table), &retlen, (void *)bbt.table);
          if(bbt_rewrite_error) printk("BBT write fail %d\n", i);
        } while( (i++ <3) && bbt_rewrite_error);
        // about to panic: need to mark bad block table as bad block !!
        if(bbt_rewrite_error) {
            // issue oob page write command
            nand_select(mtd);
            this->state = FL_WRITING;
            this->cmdfunc (mtd, NAND_CMD_SEQIN, mtd->oobblock, table_pos << 5);
            // blindly write 0xde to entire oob block
            for(i=0; i< mtd->oobsize; i++) writeb (0xde, this->IO_ADDR_W);
            // program it
            this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);
            // wait ready
            status = this->waitfunc (mtd, this, FL_WRITING);
            // issue oob page write command
            this->cmdfunc (mtd, NAND_CMD_SEQIN, mtd->oobblock,  (table_pos << 5) +1);
            // blindly write 0xad to entire oob block
            for(i=0; i< mtd->oobsize; i++) writeb (0xad, this->IO_ADDR_W);
            // program it
            this->cmdfunc (mtd, NAND_CMD_PAGEPROG, -1, -1);
            // wait ready
            status = this->waitfunc (mtd, this, FL_WRITING);
            this->state = FL_READY;
            printk("Mark BBT block %d as bad :( !!!\n", table_pos);
            // Mark current BBT bad and relocate table possition
            // Note that table_pos already reserved, so that first good will be the next good
            // even though it is not updated yet here
            bbn_current = table_pos;
            table_pos = nand_get_first_good();
            goto bbt_rewrite_retry;
        } else printk("BBT successfully updated(%d) \n", table_pos);

        if(table_pos != table_pos_old ) {
            printk("!!! Caution: BBT entry(%d->%d) AND iboot1 re-write !!!\n", table_pos_old, table_pos);
            // read back
            pbuf = kmalloc(mtd->erasesize, GFP_KERNEL);
            if(pbuf == NULL) {
                printk("BBT re-construct failed: Out of memory\n");
                return;
            }
            nand_read(mtd, 0, mtd->erasesize, &retlen, pbuf);
            *(unsigned long*)(pbuf+508) = (table_pos | 0xdead0000);
            // erase before write
            i = 0;
            nand_select(mtd);
            do {
               this->state = FL_ERASING;
               this->cmdfunc (mtd, NAND_CMD_ERASE1, -1, 0);
                       this->cmdfunc (mtd, NAND_CMD_ERASE2, -1, -1);
               status = this->waitfunc (mtd, this, FL_ERASING);
               this->state = FL_READY;
            } while( (i++ < 3) && (status & 0x01) );
            if(status & 0x01) {
                printk("BBT re-construct failed: Erase BBT entry fail\n");
                // get lost now, try write anyway
            }
            // write back: block0 has to be correct, otherwise dead here
            if(bbt_nand_write(mtd, 0, mtd->erasesize, &retlen, pbuf)) {
                printk("Block0 error: Panic\n");
            }
            kfree(pbuf);
        }

    }
    nand_build_bbt();

    nand_init_bbt_start = 0;

    printk("Total bad block number %d\n", MAX_BLOCK-bbt.total_good_blocks);

    pbuf = kmalloc(512, GFP_KERNEL);

    memset((void*)pbuf, 0x0, 512);
    //nand_read(mtd, 2<<14, 512, &retlen, pbuf);
    nand_read(mtd, 32768, 512, &retlen, pbuf);
    printk("retlen = 0x%04x\n", retlen);
    /* need to check VPD checksum in the future */
    for (i = 256, checksum = 0; i < 512; i += 2) {
        checksum += (pbuf[i] << 8);
        //printk("i = 0x%02x c = 0x%02x checksum = 0x%04x\n", i, pbuf[i], checksum);
        j = i+1;
        checksum += pbuf[j];
        //printk("i = 0x%02x c = 0x%02x checksum = 0x%04x\n", j, pbuf[j], checksum);
    }

    printk("VPD checksum = 0x%x\n", checksum);

    if (vpd392_override == 1) {
        this->eccmode = NAND_ECC_SOFT;
        printk("ECC is forced ON (vpd392=%02x)\n", pbuf[392]);
    }
    else if (vpd392_override == 2) {
        this->eccmode = NAND_ECC_NONE;
        printk("ECC is forced OFF (vpd392=%02x)\n", pbuf[392]);
    }
    else if (pbuf[392] == 0xEC) {
        this->eccmode = NAND_ECC_SOFT;
        printk("ECC is ON\n");
    }
    else {
        this->eccmode = NAND_ECC_NONE;
        printk("ECC is OFF\n");
    }
    //this->eccmode = NAND_ECC_NONE;

    kfree(pbuf);
}
EXPORT_SYMBOL(vpd392_override);
#else
void nand_init_bbt(struct mtd_info *mtd)
{
    int i, j;
    struct nand_chip *this = mtd->priv;
    loff_t from;
    u_char oob1,oob2,*pbuf;
    size_t retlen;
    unsigned long table_pos;


    nand_init_bbt_start = 1;

    printk("sizeof table %d\n", sizeof(bbt.table));

    bbt.total_reserve_blocks = 0;
    for( i = 0 ; i < 8; i++)
        nand_add_reserve_block(i);
    nand_read(mtd, 508, 4, &retlen, (void *)&table_pos);
    if( (table_pos & 0xffff0000) == 0xdead0000) {
        printk("table is there %lud\n", table_pos & 0xffff);
        table_pos &= 0xffff;
        nand_read(mtd,table_pos * 1024 * 16, 4096, &retlen, (void*)bbt.table);
        for( i = 0 ; i < 1024 ; i++) {
            if(bbt.table[i].bad_block == 0xffff)
                break;
            else nand_add_reserve_block(bbt.table[i].bad_block);
        }
    }
    else {
        printk("without table\n");
        memset((void*)bbt.table, 0xff, sizeof(bbt.table));
        j = 0;
        for( i = 0 ; i < MAX_BLOCK; i++) {
            from = (i << (this->page_shift + 5)) + NAND_BADBLOCK_POS;
            nand_read_oob(mtd, from, 1, &retlen, &oob1);
            from = from + 512;
            nand_read_oob(mtd, from, 1, &retlen, &oob2);
            if( oob1 != 0xff || oob2 != 0xff) {
                bbt.table[j].bad_block = i;
                nand_add_reserve_block(i);
                j++;
            }

        }
        table_pos = nand_get_first_good();

        for( i = 0 ; i < j ; i++)
            bbt.table[i].good_block = nand_get_last_good();

        pbuf = kmalloc(512, GFP_KERNEL);
        nand_read(mtd, 0, 512, &retlen, pbuf);
        *(unsigned long*)(pbuf+508) = (table_pos | 0xdead0000);
        nand_write(mtd, 0, 512, &retlen, pbuf);
        kfree(pbuf);
        nand_write(mtd, table_pos << 14, sizeof(bbt.table), &retlen, (void*)bbt.table);
    }
    nand_build_bbt();

    nand_init_bbt_start = 0;

    printk("Total bad block number %d\n", MAX_BLOCK-bbt.total_good_blocks);
}
#endif
EXPORT_SYMBOL(nand_init_bbt);

/*
 * Scan for the NAND device
 */
int nand_scan (struct mtd_info *mtd, int max_chips)
{
    int i, nand_maf_id, nand_dev_id;
    struct nand_chip *this = mtd->priv;

    /* check for proper chip_delay setup, set 20us if not */
    if (!this->chip_delay)
        this->chip_delay = 20;

    /* check, if a user supplied command function given */
    if (this->cmdfunc == NULL)
        this->cmdfunc = nand_command;

    /* check, if a user supplied wait function given */
    if (this->waitfunc == NULL)
        this->waitfunc = nand_wait;

    /* Select the device */
    nand_select (mtd);

    /* Send the command for reading device ID */
    this->cmdfunc (mtd, NAND_CMD_READID, 0x00, -1);

    /* Read manufacturer and device IDs */
    nand_maf_id = readb (this->IO_ADDR_R);
    nand_dev_id = readb (this->IO_ADDR_R);

    /* Print and store flash device information */
    for (i = 0; nand_flash_ids[i].name != NULL; i++) {
        if (nand_dev_id == nand_flash_ids[i].id && !mtd->size) {
            mtd->name = nand_flash_ids[i].name;
            mtd->erasesize = nand_flash_ids[i].erasesize;
            //mtd->size = (1 << nand_flash_ids[i].chipshift);
            mtd->size = nand_flash_ids[i].chipsize << 20;
            mtd->eccsize = 256;
            //if (nand_flash_ids[i].page256) {
            if (nand_flash_ids[i].pagesize == 256) {
                mtd->oobblock = 256;
                mtd->oobsize = 8;
                this->page_shift = 8;
            } else {
                mtd->oobblock = 512;
                mtd->oobsize = 16;
                this->page_shift = 9;
                mtd->oobinfo.useecc = 1;
                for (i=0; i<6; i++) {
                    mtd->oobinfo.eccpos[i]=i+6;
                }
            }
            /* Try to identify manufacturer */
            for (i = 0; nand_manuf_ids[i].id != 0x0; i++) {
                if (nand_manuf_ids[i].id == nand_maf_id)
                    break;
            }
            printk (KERN_INFO "NAND device: Manufacture ID:"
                " 0x%02x, Chip ID: 0x%02x (%s %s)\n", nand_maf_id, nand_dev_id,
                nand_manuf_ids[i].name , mtd->name);
            if ((nand_maf_id == 0xec) && (nand_dev_id == 0x76))
            {
                samsung_rev_c_support = 1;
                printk ("Samsung NAND flash rev C \n");
            }
            break;
        }
    }

    /*
     * check ECC mode, default to software
     * if 3byte/512byte hardware ECC is selected and we have 256 byte pagesize
     * fallback to software ECC
    */
    this->eccsize = 256;    /* set default eccsize */

    switch (this->eccmode) {

    case NAND_ECC_HW3_512:
        if (mtd->oobblock == 256) {
            printk (KERN_WARNING "512 byte HW ECC not possible on 256 Byte pagesize, fallback to SW ECC \n");
            this->eccmode = NAND_ECC_SOFT;
            this->calculate_ecc = nand_calculate_ecc;
            this->correct_data = nand_correct_data;
            break;
        } else
            this->eccsize = 512; /* set eccsize to 512 and fall through for function check */

    case NAND_ECC_HW3_256:
        if (this->calculate_ecc && this->correct_data && this->enable_hwecc)
            break;
        printk (KERN_WARNING "No ECC functions supplied, Hardware ECC not possible\n");
        BUG();

    case NAND_ECC_NONE:
        /*printk (KERN_WARNING "NAND_ECC_NONE selected by board driver. This is not recommended !!\n");*/
        this->eccmode = NAND_ECC_NONE;
        //break;

    case NAND_ECC_SOFT:
        this->calculate_ecc = nand_calculate_ecc;//ecc write is always enable
        this->correct_data = nand_correct_data;
        break;

    default:
        printk (KERN_WARNING "Invalid NAND_ECC_MODE %d\n", this->eccmode);
        BUG();
    }

    /* Initialize state, waitqueue and spinlock */
    this->state = FL_READY;
    init_waitqueue_head (&this->wq);
    spin_lock_init (&this->chip_lock);

    /* De-select the device */
    nand_deselect (mtd);

    /* Print warning message for no device */
    if (!mtd->size) {
        printk (KERN_WARNING "No NAND device found!!!\n");
        return 1;
    }

    /* Fill in remaining MTD driver data */
    mtd->type = MTD_NANDFLASH;
    mtd->flags = MTD_CAP_NANDFLASH | MTD_ECC;
    mtd->ecctype = MTD_ECC_SW;
    mtd->erase = nand_erase;
    mtd->point = NULL;
    mtd->unpoint = NULL;
    mtd->read = nand_read;
    mtd->write = nand_write;
//  mtd->read_ecc = nand_read_ecc;
//  mtd->write_ecc = nand_write_ecc;
    mtd->read_oob = nand_read_oob;
    mtd->write_oob = nand_write_oob;
    mtd->readv = NULL;
    mtd->writev = nand_writev;
//  mtd->writev_ecc = nand_writev_ecc;
    mtd->sync = nand_sync;
    mtd->lock = NULL;
    mtd->unlock = NULL;
    mtd->suspend = NULL;
    mtd->resume = NULL;

    /* Return happy */
    return 0;
}

EXPORT_SYMBOL (nand_scan);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Steven J. Hill <sjhill@realitydiluted.com>, Thomas Gleixner <tglx@linutronix.de>");
MODULE_DESCRIPTION ("Generic NAND flash driver code");
