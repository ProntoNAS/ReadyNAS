/*
 * MTD map driver for Atmel compatible flash chips (non-CFI)
 *
 * Author: Bin Zhang <bzhang@infrant.com>
 *
 * Copyright (c) 2003 Infrant Technologies Inc.,
 *
 * This file is under GPL.
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/flashchip.h>

/* Addresses */
#define ADDR_MANUFACTURER		0x0000
#define ADDR_DEVICE_ID			0x0001
#define ADDR_UNLOCK_1			0x5555
#define ADDR_UNLOCK_2			0x2AAA

/* Commands */
#define CMD_UNLOCK_DATA_1		0x00AA
#define CMD_UNLOCK_DATA_2		0x0055
#define CMD_MANUFACTURER_UNLOCK_DATA	0x0090
#define CMD_RESET_DATA			0x00F0
#define CMD_SECTOR_ERASE_UNLOCK_DATA	0x0080
#define CMD_SECTOR_ERASE_UNLOCK_DATA_1	0x0010
#define CMD_PROGRAM_UNLOCK_DATA	0xA0

/* Manufacturers */
#define MANUFACTURER_AMD	0x0001
#define MANUFACTURER_ATMEL	0x001F
#define MANUFACTURER_FUJITSU	0x0004
#define MANUFACTURER_ST		0x0020
#define MANUFACTURER_SST	0x00BF
#define MANUFACTURER_TOSHIBA	0x0098

/* Atmel */
#define AT49LV040	0x13
#define AT29LV512	0x3d
#define SST29LE512  0x3d


#define D6_MASK	0x40

struct atmel_flash_info {
	const __u8 mfr_id;
	const __u8 dev_id;
	const char *name;
	const u_long size;
};



static int atmel_flash_read(struct mtd_info *, loff_t, size_t, size_t *,
			  u_char *);
static int atmel_flash_write(struct mtd_info *, loff_t, size_t, size_t *,
			   const u_char *);
static int atmel29_flash_write(struct mtd_info *, loff_t, size_t, size_t *,
			   const u_char *);
static int atmel_flash_erase(struct mtd_info *, struct erase_info *);
static void atmel_flash_sync(struct mtd_info *);
static int atmel_flash_suspend(struct mtd_info *);
static void atmel_flash_resume(struct mtd_info *);
static void atmel_flash_destroy(struct mtd_info *);
static struct mtd_info *atmel_flash_probe(struct map_info *map);


static struct mtd_chip_driver atmel_flash_chipdrv = {
	probe: atmel_flash_probe,
	destroy: atmel_flash_destroy,
	name: "atmel_flash",
	module: THIS_MODULE
};

static inline map_word atmel_build_cmd(u16 cmd)
{
	map_word val = { {0} };
	val.x[0] = cmd & 0xFFFF;
	return val;
}
#define CMD(x)  atmel_build_cmd(x)

static inline u8 atmel_parse_ret(map_word val)
{
	return val.x[0] & 0xFF;
}
#define RET(x)  atmel_parse_ret(x)

static const char im_name[] = "atmel_flash";


static inline void send_unlock(struct map_info *map)
{
	map->write(map, CMD(CMD_UNLOCK_DATA_1), ADDR_UNLOCK_1);
	map->write(map, CMD(CMD_UNLOCK_DATA_2), ADDR_UNLOCK_2);
}

static inline void send_cmd(struct map_info *map, __u8 cmd)
{
	send_unlock(map);
	map->write(map, CMD(cmd), ADDR_UNLOCK_1);
}

static inline int flash_is_busy(struct map_info *map, unsigned long addr)
{
	return ((RET(map->read(map, addr)) & D6_MASK) !=
		(RET(map->read(map, addr)) & D6_MASK));
}

static int atmel_flash_unlock(struct mtd_info *mtd, loff_t ofs, size_t len)
{
	printk(KERN_ERR "atmel_flash_unlock(): not implemented!\n");
	return -EINVAL;
}

static int atmel_flash_lock(struct mtd_info *mtd, loff_t ofs, size_t len)
{
	printk(KERN_ERR "atmel_flash_lock(): not implemented!\n");
	return -EINVAL;
}


/*
 * Reads JEDEC manufacturer ID and device ID and returns the index of the first
 * matching table entry (-1 if not found or alias for already found chip).
 */ 
static int probe_new_chip(struct mtd_info *mtd, 
			  const struct atmel_flash_info *table, int table_size)
{
	__u8 mfr_id;
	__u8 dev_id;
	struct map_info *map = mtd->priv;
	int i;
	

	udelay(20000);  // bug#2381: pause after reset

	/* Enter autoselect mode. */
	send_cmd(map, CMD_MANUFACTURER_UNLOCK_DATA);

	udelay(20000);  // bug#2381: pause after identify

	mfr_id = RET(map->read(map, ADDR_MANUFACTURER));
	dev_id = RET(map->read(map, ADDR_DEVICE_ID));


	for (i = 0; i < table_size; i++) {
		
		if ((mfr_id == (table[i].mfr_id)) &&
		    (dev_id == (table[i].dev_id))) {
			mtd->size += table[i].size;
			mtd->numeraseregions = 1;

			break;
		}
	}

	/* Exit autoselect mode. */
	send_cmd(map, CMD_RESET_DATA);

	if (i == table_size) {
		printk(KERN_DEBUG "%s: unknown flash device, "
		       "mfr id 0x%x, dev id 0x%x\n", map->name,
		       mfr_id, dev_id);
		return -1;
	}

	return i;
}



static struct mtd_info *atmel_flash_probe(struct map_info *map)
{
	/* Keep this table on the stack so that it gets deallocated after the
	 * probe is done.
	 */
	const struct atmel_flash_info table[] = {
	{
		mfr_id: MANUFACTURER_ATMEL,
		dev_id: AT49LV040,
		name: "Atmel AT49LV040",
		size: 0x00080000
	}, {
		mfr_id: MANUFACTURER_ATMEL,
		dev_id: AT29LV512,
		name: "Atmel AT29LV512",
		size: 0x00010000
	}, {	
		mfr_id: MANUFACTURER_SST,
		dev_id: SST29LE512,
		name: "SST 29LE512",
		size: 0x00010000
	   }
	};

	struct mtd_info *mtd;
	struct flchip chip,*private;
	int table_pos;

	mtd = (struct mtd_info*)kmalloc(sizeof(*mtd), GFP_KERNEL);
	if (!mtd) {
		printk(KERN_WARNING
		       "%s: kmalloc failed for info structure\n", map->name);
		return NULL;
	}
	memset(mtd, 0, sizeof(*mtd));
	mtd->priv = map;

	if ((table_pos = probe_new_chip(mtd, table,
					   sizeof(table)/sizeof(table[0])))
	    == -1) {
		printk(KERN_WARNING
		       "%s: Found no Atmel device at location zero\n",
		       map->name);
		kfree(mtd);

		return NULL;
	}

	chip.start = 0;
	chip.state = FL_READY;
	chip.mutex = &chip._spinlock;

	mtd->eraseregions = kmalloc(sizeof(struct mtd_erase_region_info) *
				    mtd->numeraseregions, GFP_KERNEL);
	if (!mtd->eraseregions) { 
		printk(KERN_WARNING "%s: Failed to allocate "
		       "memory for MTD erase region info\n", map->name);
		kfree(mtd);
		map->fldrv_priv = NULL;
		return 0;
	}

	mtd->eraseregions[0].offset = 0;
	mtd->eraseregions[0].erasesize = mtd->size;
	mtd->eraseregions[0].numblocks = 1;
	mtd->erasesize = mtd->size;
	mtd->type = MTD_NORFLASH;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->name = map->name;
	mtd->erase = atmel_flash_erase;	
	mtd->read = atmel_flash_read;
	if(table_pos != 0)
		mtd->write = atmel29_flash_write;
	else
		mtd->write = atmel_flash_write;	
	mtd->sync = atmel_flash_sync;	
	mtd->suspend = atmel_flash_suspend;	
	mtd->resume = atmel_flash_resume;	
	mtd->lock = atmel_flash_lock;
	mtd->unlock = atmel_flash_unlock;

	private = kmalloc(sizeof(struct flchip), GFP_KERNEL);
	if (!private) {
		printk(KERN_WARNING
		       "%s: kmalloc failed for private structure\n", map->name);
		kfree(mtd);
		map->fldrv_priv = NULL;
		return NULL;
	}

	memcpy(private, &chip, sizeof(struct flchip));
	init_waitqueue_head(&private->wq);
	spin_lock_init(&private->_spinlock);

	map->fldrv_priv = private;

	map->fldrv = &atmel_flash_chipdrv;
//	MOD_INC_USE_COUNT;

	return mtd;
}



static inline int read_one_chip(struct map_info *map, struct flchip *chip,
			       loff_t adr, size_t len, u_char *buf)
{
	DECLARE_WAITQUEUE(wait, current);
	unsigned long timeo = jiffies + HZ;

retry:
	spin_lock_bh(chip->mutex);

	if (chip->state != FL_READY){
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&chip->wq, &wait);
                
		spin_unlock_bh(chip->mutex);

		schedule();
		remove_wait_queue(&chip->wq, &wait);

		if(signal_pending(current)) {
			return -EINTR;
		}

		timeo = jiffies + HZ;

		goto retry;
	}	

	chip->state = FL_READY;

	map->copy_from(map, buf, adr, len);

	wake_up(&chip->wq);
	spin_unlock_bh(chip->mutex);

	return 0;
}



static int atmel_flash_read(struct mtd_info *mtd, loff_t from, size_t len,
			  size_t *retlen, u_char *buf)
{
	struct map_info *map = mtd->priv;
	struct flchip *chip = map->fldrv_priv;
	int ret = 0;

	if ((from + len) > mtd->size) {
		printk(KERN_WARNING "%s: read request past end of device "
		       "(0x%lx)\n", map->name, (unsigned long)from + len);

		return -EINVAL;
	}

	*retlen = 0;

	ret = read_one_chip(map, chip, from, len, buf);
	if (ret) 
		return ret;
	*retlen = len;

	return ret;
}


static int write_one_sector(struct map_info *map, struct flchip *chip,
			  unsigned long adr, __u8 *sector)
{
	unsigned long timeo = jiffies + HZ;
	DECLARE_WAITQUEUE(wait, current);
	int ret = 0,i;
	int times_left;
	unsigned long flags;

retry:
	spin_lock_bh(chip->mutex);

	if (chip->state != FL_READY){
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&chip->wq, &wait);
                
		spin_unlock_bh(chip->mutex);

		schedule();
		remove_wait_queue(&chip->wq, &wait);
		if(signal_pending(current))
			return -EINTR;

		timeo = jiffies + HZ;

		goto retry;
	}	

	chip->state = FL_WRITING;

	local_irq_save(flags);
	send_cmd(map, CMD_PROGRAM_UNLOCK_DATA);
	for( i = 0 ; i < 128; i++) 
		map->write(map, CMD(*(sector+i)), adr+i);
	local_irq_restore(flags);

	times_left = 500000;
	while (times_left-- && flash_is_busy(map, adr)) { 
		if (need_resched()) {
			spin_unlock_bh(chip->mutex);
			schedule();
			spin_lock_bh(chip->mutex);
		}
	}

	if (!times_left) {
		printk(KERN_WARNING "%s: write to 0x%lx timed out!\n",
		       map->name, adr);
		ret = -EIO;
	} else {
		__u8 verify;
		for( i = 0 ; i < 128; i++) {
			if ((verify = RET(map->read(map, adr+i))) != *(sector+i)) {
				printk(KERN_WARNING "%s: write to 0x%lx failed. "
			       "datum[%d] = %x, verify = %x\n",
			       map->name, adr+i, i, *(sector+i), verify);
				ret = -EIO;
			}
		}
	}

	chip->state = FL_READY;
	wake_up(&chip->wq);
	spin_unlock_bh(chip->mutex);

	return ret;
}

static int write_one_word(struct map_info *map, struct flchip *chip,
			  unsigned long adr, __u8 datum)
{
	unsigned long timeo = jiffies + HZ;
	DECLARE_WAITQUEUE(wait, current);
	int ret = 0;
	int times_left;

retry:
	spin_lock_bh(chip->mutex);

	if (chip->state != FL_READY){
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&chip->wq, &wait);
                
		spin_unlock_bh(chip->mutex);

		schedule();
		remove_wait_queue(&chip->wq, &wait);
		if(signal_pending(current))
			return -EINTR;

		timeo = jiffies + HZ;

		goto retry;
	}	

	chip->state = FL_WRITING;

	send_cmd(map, CMD_PROGRAM_UNLOCK_DATA);
	map->write(map, CMD(datum), adr);

	times_left = 500000;
	while (times_left-- && flash_is_busy(map, adr)) { 
		if (need_resched()) {
			spin_unlock_bh(chip->mutex);
			schedule();
			spin_lock_bh(chip->mutex);
		}
	}

	if (!times_left) {
		printk(KERN_WARNING "%s: write to 0x%lx timed out!\n",
		       map->name, adr);
		ret = -EIO;
	} else {
		__u8 verify;
		if ((verify = RET(map->read(map, adr))) != datum) {
			printk(KERN_WARNING "%s: write to 0x%lx failed. "
			       "datum = %x, verify = %x\n",
			       map->name, adr, datum, verify);
			ret = -EIO;
		}
	}

	chip->state = FL_READY;
	wake_up(&chip->wq);
	spin_unlock_bh(chip->mutex);

	return ret;
}



static int atmel_flash_write(struct mtd_info *mtd, loff_t to , size_t len,
			   size_t *retlen, const u_char *buf)
{
	struct map_info *map = mtd->priv;
	struct flchip *chip = map->fldrv_priv;
	int ret = 0;

	*retlen = 0;

	while(len > 0) {
		__u8 datum;

		datum = *(__u8*)buf;

		ret = write_one_word(map, chip, to, datum);

		if (ret) {
			return ret;
		}

		to++;
		buf++;
		(*retlen) += 1;
		len --;
	}

	return 0;
}


static int atmel29_flash_write(struct mtd_info *mtd, loff_t to , size_t len,
			   size_t *retlen, const u_char *buf)
{
	struct map_info *map = mtd->priv;
	struct flchip *chip = map->fldrv_priv;
	int ret = 0;
	loff_t sector_start, sector_end,i,tmp;
	size_t sector_retlen;
	u_char sector[128];

	*retlen = 0;

	if ((to + len) > mtd->size) {
		printk(KERN_WARNING "%s: write request past end of device "
		       "(0x%lx)\n", map->name, (unsigned long)to + len);

		return -EINVAL;
	}

	sector_start = to / 128;
	sector_end = (to+len-1) /128;
	for(i= sector_start; i<sector_end+1;i++) {
		if( (i == sector_start) || (i == sector_end)) {
			ret = atmel_flash_read(mtd,i*128,128,&sector_retlen,sector);
			if(ret)return ret;
			if(sector_retlen != 128)
				return -EIO;
			
			if(sector_start==sector_end) {
				memcpy(sector+to%128,buf,len);
				(*retlen) += len;
			}
			else if( i == sector_start) {
				memcpy(sector+to%128,buf,128-to%128);
				(*retlen) += (128-to%128);
			}
			else if(i==sector_end) {
				tmp = (len+to) % 128;
				if(tmp == 0) tmp = 128;
				memcpy(sector,buf+sector_end*128-to,tmp);
				*(retlen) += tmp;
			}
			ret = write_one_sector(map,chip,i*128,sector);
			
		}
		else {
			ret = write_one_sector(map,chip,i*128,
						(__u8*)(buf+i*128-to));
			*(retlen) += 128;
		}
		ret = 0;
		if(ret) return ret;
	}
	return 0;
}

static inline int erase_all(struct map_info *map, struct flchip *chip,
				  unsigned long adr, u_long size)
{
	unsigned long timeo = jiffies + HZ;
	DECLARE_WAITQUEUE(wait, current);

retry:
	spin_lock_bh(chip->mutex);

	if (chip->state != FL_READY){
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&chip->wq, &wait);
                
		spin_unlock_bh(chip->mutex);

		schedule();
		remove_wait_queue(&chip->wq, &wait);

		if (signal_pending(current)) {
			return -EINTR;
		}

		timeo = jiffies + HZ;

		goto retry;
	}	

	chip->state = FL_ERASING;

	send_cmd(map, CMD_SECTOR_ERASE_UNLOCK_DATA);
	send_cmd(map, CMD_SECTOR_ERASE_UNLOCK_DATA_1);
	
	timeo = jiffies + (HZ * 20);

	spin_unlock_bh(chip->mutex);
	schedule_timeout(HZ);
	spin_lock_bh(chip->mutex);
	
	while (flash_is_busy(map,adr)) {

		if (chip->state != FL_ERASING) {
			/* Someone's suspended the erase. Sleep */
			set_current_state(TASK_UNINTERRUPTIBLE);
			add_wait_queue(&chip->wq, &wait);
			
			spin_unlock_bh(chip->mutex);
			schedule();
			remove_wait_queue(&chip->wq, &wait);
			
			if (signal_pending(current)) {
				return -EINTR;
			}
			
			timeo = jiffies + (HZ*2); /* FIXME */
			spin_lock_bh(chip->mutex);
			continue;
		}

		/* OK Still waiting */
		if (time_after(jiffies, timeo)) {
			chip->state = FL_READY;
			spin_unlock_bh(chip->mutex);
			printk(KERN_WARNING "%s: waiting for erase to complete "
			       "timed out.\n", map->name);

			return -EIO;
		}
		
		/* Latency issues. Drop the lock, wait a while and retry */
		spin_unlock_bh(chip->mutex);

		if (need_resched())
			schedule();
		else
			udelay(1);
		
		spin_lock_bh(chip->mutex);
	}

	/* Verify every single word */
	{
		int address;
		int error = 0;
		__u8 verify;

		for (address = adr; address < (adr + size); address++) {
			if ((verify = RET(map->read(map, address))) != 0xFF) {
				error = 1;
				break;
			}
		}
		if (error) {
			chip->state = FL_READY;
			spin_unlock_bh(chip->mutex);
			printk(KERN_WARNING
			       "%s: verify error at 0x%x, size %ld.\n",
			       map->name, address, size);

			return -EIO;
		}
	}
	
	chip->state = FL_READY;
	wake_up(&chip->wq);
	spin_unlock_bh(chip->mutex);

	return 0;
}



static int atmel_flash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct map_info *map = mtd->priv;
	struct flchip *chip = map->fldrv_priv;
	int ret = 0;

	if (instr->addr != 0 || instr->len != mtd->size)
		return -EINVAL;
	

	ret = erase_all(map,chip, 0, mtd->erasesize);
	if (ret)
		return ret;

	instr->state = MTD_ERASE_DONE;
	if (instr->callback) {
		instr->callback(instr);
	}
	
	return 0;
}



static void atmel_flash_sync(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct flchip *chip = map->fldrv_priv;
	
	DECLARE_WAITQUEUE(wait, current);

retry:
	spin_lock_bh(chip->mutex);

	switch(chip->state) {
	case FL_READY:
	case FL_STATUS:
	case FL_CFI_QUERY:
	case FL_JEDEC_QUERY:
		chip->oldstate = chip->state;
		chip->state = FL_SYNCING;
		/* No need to wake_up() on this state change - 
		 * as the whole point is that nobody can do anything
		 * with the chip now anyway.
		 */
	case FL_SYNCING:
		spin_unlock_bh(chip->mutex);
		break;

	default:
		/* Not an idle state */
		add_wait_queue(&chip->wq, &wait);
			
		spin_unlock_bh(chip->mutex);

		schedule();

        remove_wait_queue(&chip->wq, &wait);
			
		goto retry;
	}

	/* Unlock the chips again */

	spin_lock_bh(chip->mutex);
		
	if (chip->state == FL_SYNCING) {
		chip->state = chip->oldstate;
		wake_up(&chip->wq);
	}
	spin_unlock_bh(chip->mutex);
}



static int atmel_flash_suspend(struct mtd_info *mtd)
{
	printk(KERN_ERR "atmel_flash_suspend(): not implemented!\n");
	return -EINVAL;
}



static void atmel_flash_resume(struct mtd_info *mtd)
{
	printk(KERN_ERR "atmel_flash_resume(): not implemented!\n");
}



static void atmel_flash_destroy(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct flchip *private = map->fldrv_priv;
	kfree(private);
}

int __init atmel_flash_init(void)
{
	register_mtd_chip_driver(&atmel_flash_chipdrv);
	return 0;
}

void __exit atmel_flash_exit(void)
{
	unregister_mtd_chip_driver(&atmel_flash_chipdrv);
}

module_init(atmel_flash_init);
module_exit(atmel_flash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bin Zhang <bzhang@infrant.com>");
MODULE_DESCRIPTION("MTD chip driver for Atmel flash chips");
