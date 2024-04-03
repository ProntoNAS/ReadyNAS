/*
 *	Real Time Clock interface for Linux	
 *
 *	Copyright (C) 1996 Paul Gortmaker
 *
 *	This driver allows use of the real time clock (built into
 *	nearly all computers) from user space. It exports the /dev/rtc
 *	interface supporting various ioctl() and also the
 *	/proc/driver/rtc pseudo-file for status information.
 *
 *	The ioctls can be used to set the interrupt behaviour and
 *	generation rate from the RTC via IRQ 8. Then the /dev/rtc
 *	interface can be used to make use of these timer interrupts,
 *	be they interval or alarm based.
 *
 *	The /dev/rtc interface will block on reads until an interrupt
 *	has been received. If a RTC interrupt has already happened,
 *	it will output an unsigned long and then block. The output value
 *	contains the interrupt status in the low byte and the number of
 *	interrupts since the last read in the remaining high bytes. The 
 *	/dev/rtc interface can also be used with the select(2) call.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 *	Based on other minimal char device drivers, like Alan's
 *	watchdog, Ted's random, etc. etc.
 *
 *	1.07	Paul Gortmaker.
 *	1.08	Miquel van Smoorenburg: disallow certain things on the
 *		DEC Alpha as the CMOS clock is also used for other things.
 *	1.09	Nikita Schmidt: epoch support and some Alpha cleanup.
 *	1.09a	Pete Zaitcev: Sun SPARC
 *	1.09b	Jeff Garzik: Modularize, init cleanup
 *	1.09c	Jeff Garzik: SMP cleanup
 *	1.10    Paul Barton-Davis: add support for async I/O
 *	1.10a	Andrea Arcangeli: Alpha updates
 *	1.10b	Andrew Morton: SMP lock fix
 *	1.10c	Cesar Barros: SMP locking fixes and cleanup
 *	1.10d	Paul Gortmaker: delete paranoia check in rtc_exit
 *	1.10e	Maciej W. Rozycki: Handle DECstation's year weirdness.
 */

#define RTC_VERSION		"1.10e"
#define RTC_IO_EXTENT	0x10	/* Only really two ports, but...	*/

/*
 *	Note that *all* calls to CMOS_READ and CMOS_WRITE are done with
 *	interrupts disabled. Due to the index-port/data-port (0x70/0x71)
 *	design of the RTC, we don't want two different things trying to
 *	get to it at once. (e.g. the periodic 11 min sync from time.c vs.
 *	this driver.)
 */

#include "config.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/sysctl.h>
#include <linux/rtc.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/ds1337.h>
#include "asm/padre0.h"
#include "asm/padre_paseo.h"

/*
 *	We sponge a minor off of the misc major. No need slurping
 *	up another valuable major dev number for this. If you add
 *	an ioctl, make sure you don't conflict with SPARC's RTC
 *	ioctls.
 */

static struct fasync_struct *rtc_async_queue;

static DECLARE_WAIT_QUEUE_HEAD(rtc_wait);

static ssize_t rtc_read(struct file *file, char *buf,
			size_t count, loff_t *ppos);

static int rtc_ioctl(struct inode *inode, struct file *file,
		     unsigned int cmd, unsigned long arg);


static void get_rtc_time (struct rtc_time *rtc_tm);
static void get_rtc_alm_time (struct rtc_time *rtc_tm);
static unsigned long get_rts_time(void);

//static inline unsigned char rtc_is_updating(void);

static int rtc_read_proc(char *page, char **start, off_t off,
                         int count, int *eof, void *data);

static void set_rtc_irq_bit(unsigned char bit);
static void mask_rtc_irq_bit(unsigned char bit);

static void sys_sync_rts_time(void);

/*
 *	Bits in rtc_status. (6 bits of room for future expansion)
 */

#define RTC_IS_OPEN		0x01	/* means /dev/rtc is in use	*/
#define RTC_TIMER_ON		0x02	/* missed irq timer active	*/

/*
 * rtc_status is never changed by rtc_interrupt, and ioctl/open/close is
 * protected by the big kernel lock. However, ioctl can still disable the timer
 * in rtc_status and then with del_timer after the interrupt has read
 * rtc_status but before mod_timer is called, which would then reenable the
 * timer (but you would need to have an awful timing before you'd trip on it)
 */
static unsigned long rtc_status = 0;	/* bitmapped status byte.	*/
static unsigned long rtc_freq = 0;	/* Current periodic IRQ rate	*/
static unsigned long rtc_irq_data = 0;	/* our output to the world	*/
static unsigned long rtc_max_user_freq = 64; /* > this, need CAP_SYS_RESOURCE */

/*
 *	If this driver ever becomes modularised, it will be really nice
 *	to make the epoch retain its value across module reload...
 */

static unsigned long epoch = 1900;	/* year corresponding to 0x00	*/

static const unsigned char days_in_mo[] = 
{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


/*
 * sysctl-tuning infrastructure.
 */
static ctl_table rtc_table[] = {
    { 1, "max-user-freq", &rtc_max_user_freq, sizeof(int), 0644, NULL,
      &proc_dointvec, NULL, },
    { 0, }
};

static ctl_table rtc_root[] = {
    { 1, "rtc", NULL, 0, 0555, rtc_table, },
    { 0, }
};

static ctl_table dev_root[] = {
    { CTL_DEV, "dev", NULL, 0, 0555, rtc_root, },
    { 0, }
};

static struct ctl_table_header *sysctl_header;

static int __init init_sysctl(void)
{
    sysctl_header = register_sysctl_table(dev_root, 0);
    return 0;
}

static void __exit cleanup_sysctl(void)
{
    unregister_sysctl_table(sysctl_header);
}

/*
 *	Now all the various file operations that we export.
 */

static ssize_t rtc_read(struct file *file, char *buf,
			size_t count, loff_t *ppos)
{
	return -EIO;
}

static int rtc_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		     unsigned long arg)
{
	struct rtc_time wtime; 

	switch (cmd) {
        case RTC_PIE_OFF:       /* defaultoff    */
        {
                mask_rtc_irq_bit(RTC_INTCN);
                return 0;
        }
        case RTC_PIE_ON:        /* defaulton      */
        {
                set_rtc_irq_bit(RTC_INTCN);
                return 0;
        }
        case RTC_AIE_OFF:       /* Mask alarm int. enab. bit    */
        {
                mask_rtc_irq_bit(RTC_AIE);
                return 0;
        }
        case RTC_AIE_ON:        /* Allow alarm interrupts.      */
        {
                set_rtc_irq_bit(RTC_AIE);
                return 0;
        }
	case RTC_ALM_READ:	/* Read  the present alarm time	*/
	{
		get_rtc_alm_time(&wtime);
		break;
	}
	case RTC_ALM_SET:	/* Set the ALARM */
	{
		struct rtc_time alm_tm;
		unsigned char day, hrs, min, sec;

		if (copy_from_user(&alm_tm, (struct rtc_time*)arg,
				   sizeof(struct rtc_time)))
			return -EFAULT;

		day = alm_tm.tm_mday;
		hrs = alm_tm.tm_hour;
		min = alm_tm.tm_min;
		sec = alm_tm.tm_sec;

		if ((hrs >= 24) || (min >= 60) || (sec >= 60))
			return -EINVAL;

		spin_lock_irq(&rtc_lock);

		/* A1M4 A1M3 A1M2 A1M1will automatically be zero(0), which means */
		/* date, hour, minute and second must be match for alarm */		
		BIN_TO_BCD(sec);
		BIN_TO_BCD(min);
		BIN_TO_BCD(hrs);
		BIN_TO_BCD(day);

		CMOS_WRITE(day, RTC_DAY_DATE_ALARM1);
		CMOS_WRITE(hrs, RTC_HOURS_ALARM1);
		CMOS_WRITE(min, RTC_MINUTES_ALARM1);
		CMOS_WRITE(sec, RTC_SECONDS_ALARM1);

	        CMOS_WRITE(0, RTC_STATUS);      // clear OSF

		spin_unlock_irq(&rtc_lock);
		return 0;
	}
	case RTC_RD_TIME:	/* Read the time/date from RTC	*/
	{
		get_rtc_time(&wtime);
		break;
	}
	case RTC_SET_TIME:	/* Set the RTC */
	{
		struct rtc_time rtc_tm;
		unsigned char mon, day, hrs, min, sec, leap_yr;
		unsigned int yrs;
		int century = 0;

		if (!capable(CAP_SYS_TIME))
			return -EACCES;

		if (copy_from_user(&rtc_tm, (struct rtc_time*)arg,
				   sizeof(struct rtc_time)))
			return -EFAULT;

		yrs = rtc_tm.tm_year + 1900;
		mon = rtc_tm.tm_mon + 1;   /* tm_mon starts at zero */
		day = rtc_tm.tm_mday;
		hrs = rtc_tm.tm_hour;
		min = rtc_tm.tm_min;
		sec = rtc_tm.tm_sec;

		if (yrs < 1970)
			return -EINVAL;

		leap_yr = ((!(yrs % 4) && (yrs % 100)) || !(yrs % 400));

		if ((mon > 12) || (day == 0))
			return -EINVAL;

		if (day > (days_in_mo[mon] + ((mon == 2) && leap_yr)))
			return -EINVAL;
			
		if ((hrs >= 24) || (min >= 60) || (sec >= 60))
			return -EINVAL;

		if ((yrs -= epoch) > 255)    /* They are unsigned */
			return -EINVAL;

		spin_lock_irq(&rtc_lock);
		
		/* These limits and adjustments are independant of
		 * whether the chip is in binary mode or not.
		 */
		if (yrs > 169) {
			spin_unlock_irq(&rtc_lock);
			return -EINVAL;
		}
		if (yrs >= 100) {
			yrs -= 100;
			century = 1;
		}
		else
			century = 0;

		BIN_TO_BCD(sec);
		BIN_TO_BCD(min);
		BIN_TO_BCD(hrs);
		BIN_TO_BCD(day);
		BIN_TO_BCD(mon);
		BIN_TO_BCD(yrs);

		if(century)
			mon |= RTC_MONTH_CENTURY;

		CMOS_WRITE(yrs, RTC_YEAR);
		CMOS_WRITE(mon, RTC_MONTH);
		CMOS_WRITE(day, RTC_DAY_OF_MONTH);
		CMOS_WRITE(hrs, RTC_HOURS);
		CMOS_WRITE(min, RTC_MINUTES);
		CMOS_WRITE(sec, RTC_SECONDS);

        CMOS_WRITE(0, RTC_STATUS);      // clear OSF

		spin_unlock_irq(&rtc_lock);
		return 0;
	}
	case RTC_EPOCH_READ:	/* Read the epoch.	*/
	{
		return put_user (epoch, (unsigned long *)arg);
	}
	case RTC_EPOCH_SET:	/* Set the epoch.	*/
	{
		/* 
		 * There were no RTC clocks before 1900.
		 */
		if (arg < 1900)
			return -EINVAL;

		if (!capable(CAP_SYS_TIME))
			return -EACCES;

		epoch = arg;
		return 0;
	}
	default:
		return -ENOTTY;
	}
	return copy_to_user((void *)arg, &wtime, sizeof wtime) ? -EFAULT : 0;
}

/*
 *	We enforce only one user at a time here with the open/close.
 *	Also clear the previous interrupt data on an open, and clean
 *	up things on a close.
 */

/* We use rtc_lock to protect against concurrent opens. So the BKL is not
 * needed here. Or anywhere else in this driver. */
static int rtc_open(struct inode *inode, struct file *file)
{
	spin_lock_irq (&rtc_lock);

	if(rtc_status & RTC_IS_OPEN)
		goto out_busy;

	rtc_status |= RTC_IS_OPEN;

	rtc_irq_data = 0;
	spin_unlock_irq (&rtc_lock);
	return 0;

out_busy:
	spin_unlock_irq (&rtc_lock);
	return -EBUSY;
}

static int rtc_fasync (int fd, struct file *filp, int on)
{
	return fasync_helper (fd, filp, on, &rtc_async_queue);
}

static int rtc_release(struct inode *inode, struct file *file)
{
	spin_lock_irq (&rtc_lock);
	rtc_irq_data = 0;
	spin_unlock_irq (&rtc_lock);

	/* No need for locking -- nobody else can do anything until this rmw is
	 * committed, and no timer is running. */
	rtc_status &= ~RTC_IS_OPEN;
	return 0;
}

/*
 *	The various file operations we support.
 */

static struct file_operations rtc_fops = {
	owner:		THIS_MODULE,
	llseek:		no_llseek,
	read:		rtc_read,
	ioctl:		rtc_ioctl,
	open:		rtc_open,
	release:	rtc_release,
	fasync:		rtc_fasync,
};

static struct miscdevice rtc_dev=
{
	RTC_MINOR,
	"rtc",
	&rtc_fops
};

static int __init rtc_init(void)
{
	struct timespec ts;

	if(!i2c_device_exist(PADRE_PASEO_DS1337))
		return 1;
	misc_register(&rtc_dev);
	create_proc_read_entry ("driver/rtc", 0, 0, rtc_read_proc, NULL);

	(void) init_sysctl();

#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
	printk(KERN_INFO "Real Time Clock Driver v" RTC_VERSION "\n");
#endif // CONFIG_PADRE_DEBUG_LEVEL_3

	ts.tv_sec = get_rts_time();
	ts.tv_nsec = 0;
	do_settimeofday(&ts);

	//LCD_display("RTC loaded");
	return 0;
}

static void __exit rtc_exit (void)
{
	cleanup_sysctl();
	remove_proc_entry ("driver/rtc", NULL);
	misc_deregister(&rtc_dev);
}

module_init(rtc_init);
module_exit(rtc_exit);
//EXPORT_NO_SYMBOLS;


/*
 *	Info exported via "/proc/driver/rtc".
 */

static int rtc_proc_output (char *buf)
{
#define YN(bit) ((ctrl & bit) ? "yes" : "no")
#define NY(bit) ((ctrl & bit) ? "no" : "yes")
	char *p;
	struct rtc_time tm;
    int osf;

   	spin_lock(&rtc_lock);     
    osf = CMOS_READ(RTC_STATUS) & RTC_STATUS_OSF;
    spin_unlock(&rtc_lock);

	p = buf;

	get_rtc_time(&tm);

	/*
	 * There is no way to tell if the luser has the RTC set for local
	 * time or for Universal Standard Time (GMT). Probably local though.
	 */
	p += sprintf(p,
		     "rtc_time\t: %02d:%02d:%02d\n"
		     "rtc_date\t: %04d-%02d-%02d\n"
	 	     "rtc_epoch\t: %04lu\n"
             "rtc_osf\t\t: %d\n",
		     tm.tm_hour, tm.tm_min, tm.tm_sec,
		     tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, epoch, osf?1:0);

	return  p - buf;
#undef YN
#undef NY
}

static int rtc_read_proc(char *page, char **start, off_t off,
                         int count, int *eof, void *data)
{
        int len = rtc_proc_output (page);
        if (len <= off+count) *eof = 1;
        *start = page + off;
        len -= off;
        if (len>count) len = count;
        if (len<0) len = 0;
        return len;
}

static void get_rtc_alm_time(struct rtc_time *rtc_tm)
{
	int century ;
	spin_lock_irq(&rtc_lock);
	rtc_tm->tm_sec = CMOS_READ(RTC_SECONDS_ALARM1) & 0x7f;
	rtc_tm->tm_min = CMOS_READ(RTC_MINUTES_ALARM1) & 0x7f;
	rtc_tm->tm_hour = CMOS_READ(RTC_HOURS_ALARM1)  & 0x7f;
	rtc_tm->tm_mday = CMOS_READ(RTC_DAY_DATE_ALARM1)  & 0x3f;
	rtc_tm->tm_mon = CMOS_READ(RTC_MONTH);
	rtc_tm->tm_year = CMOS_READ(RTC_YEAR);
	spin_unlock_irq(&rtc_lock);
    	
	if(rtc_tm->tm_mon & RTC_MONTH_CENTURY) {
        	century = 1;
            	rtc_tm->tm_mon &= ~RTC_MONTH_CENTURY;
        }
        else	 century = 0;

	/* RTC always BCD */
	BCD_TO_BIN(rtc_tm->tm_sec);
	BCD_TO_BIN(rtc_tm->tm_min);
	BCD_TO_BIN(rtc_tm->tm_hour);
	BCD_TO_BIN(rtc_tm->tm_mday);
	BCD_TO_BIN(rtc_tm->tm_mon);
	BCD_TO_BIN(rtc_tm->tm_year);

	rtc_tm->tm_year += century*100;
	rtc_tm->tm_mon--;
}
static void get_rtc_time(struct rtc_time *rtc_tm)
{
	int century ;
	/*
	 * Only the values that we read from the RTC are set. We leave
	 * tm_wday, tm_yday and tm_isdst untouched. Even though the
	 * RTC has RTC_DAY_OF_WEEK, we ignore it, as it is only updated
	 * by the RTC when initially set to a non-zero value.
	 */
	spin_lock_irq(&rtc_lock);
	do {
		/* DS1337 Doesn't have UIP bit */
		rtc_tm->tm_sec = CMOS_READ(RTC_SECONDS);
		rtc_tm->tm_min = CMOS_READ(RTC_MINUTES);
		rtc_tm->tm_hour = CMOS_READ(RTC_HOURS);
		rtc_tm->tm_mday = CMOS_READ(RTC_DAY_OF_MONTH);
		rtc_tm->tm_mon = CMOS_READ(RTC_MONTH);
		rtc_tm->tm_year = CMOS_READ(RTC_YEAR);
	} while( rtc_tm->tm_sec != CMOS_READ(RTC_SECONDS));
	
	spin_unlock_irq(&rtc_lock);

    if(rtc_tm->tm_mon & RTC_MONTH_CENTURY) {
    	century = 1;
	    rtc_tm->tm_mon &= ~RTC_MONTH_CENTURY;
	}
	else
		century = 0;
	
	/* RTC always BCD */
	BCD_TO_BIN(rtc_tm->tm_sec);
	BCD_TO_BIN(rtc_tm->tm_min);
	BCD_TO_BIN(rtc_tm->tm_hour);
	BCD_TO_BIN(rtc_tm->tm_mday);
	BCD_TO_BIN(rtc_tm->tm_mon);
	BCD_TO_BIN(rtc_tm->tm_year);

	rtc_tm->tm_year += century*100;
	rtc_tm->tm_mon--;
}

static unsigned long get_rts_time()
{
	unsigned int year, mon, day, hour, min, sec;
	unsigned int century=0;

   	spin_lock(&rtc_lock);     
	if( CMOS_READ(RTC_CONTROL) & RTC_CONTROL_EOSC) {
		CMOS_WRITE(0,RTC_CONTROL);
	}
	if( CMOS_READ(RTC_STATUS) & RTC_STATUS_OSF) {
		printk(KERN_INFO "RTC may be invalid.\n");
	}

   	do 
   	{ /* Isn't this overkill ? UIP above should guarantee consistency */
   		sec = CMOS_READ(RTC_SECONDS);
	    min = CMOS_READ(RTC_MINUTES);
	    hour = CMOS_READ(RTC_HOURS);
	    day = CMOS_READ(RTC_DAY_OF_MONTH);
	    mon = CMOS_READ(RTC_MONTH);
        year = CMOS_READ(RTC_YEAR);
    } while (sec != CMOS_READ(RTC_SECONDS));		  
	if(mon & RTC_MONTH_CENTURY) {
		century = 100;
		mon &= ~RTC_MONTH_CENTURY;
	}
    BCD_TO_BIN(sec);
    BCD_TO_BIN(min);
	BCD_TO_BIN(hour);
	BCD_TO_BIN(day);
	BCD_TO_BIN(mon);
	BCD_TO_BIN(year);
	year += (century+1900);
    spin_unlock(&rtc_lock);

	if( year < 2003) year = 2003;

	return mktime(year, mon, day, hour, min, sec);      
}

extern seqlock_t xtime_lock;
static void sys_sync_rts_time()
{
        unsigned int year, mon, day, hour, min, sec;
        unsigned int century=0;
        unsigned long rts_secs = 0;

        if(!spin_trylock(&rtc_lock))
                return;

#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
        if( CMOS_READ(RTC_CONTROL) & RTC_CONTROL_EOSC) {
                CMOS_WRITE(0,RTC_CONTROL);
        }
        if( CMOS_READ(RTC_STATUS) & RTC_STATUS_OSF) {
                printk(KERN_INFO "RTC may be invalid.\n");
        }

        printk("control=%x,status=%x\n",CMOS_READ(RTC_CONTROL), CMOS_READ(RTC_STATUS));
#endif // CONFIG_PADRE_DEBUG_LEVEL_3

        do
        { /* Isn't this overkill ? UIP above should guarantee consistency */
                sec = CMOS_READ(RTC_SECONDS);
                min = CMOS_READ(RTC_MINUTES);
                hour = CMOS_READ(RTC_HOURS);
                day = CMOS_READ(RTC_DAY_OF_MONTH);
                mon = CMOS_READ(RTC_MONTH);
                year = CMOS_READ(RTC_YEAR);
        }
        while (sec != CMOS_READ(RTC_SECONDS));

#ifdef CONFIG_PADRE_DEBUG_LEVEL_3
        printk("sec=%x,min=%x,hour=%x,mon=%x,day=%x,year=%x\n",
                sec,min,hour,mon,day,year);
#endif // CONFIG_PADRE_DEBUG_LEVEL_3

        if(mon & RTC_MONTH_CENTURY) {
                century = 100;
                mon &= ~RTC_MONTH_CENTURY;
        }

        BCD_TO_BIN(sec);
        BCD_TO_BIN(min);
        BCD_TO_BIN(hour);
        BCD_TO_BIN(day);
        BCD_TO_BIN(mon);
        BCD_TO_BIN(year);
        year += (century+1900);

        spin_unlock(&rtc_lock);

        if( year < 2003) year = 2003;

        rts_secs = mktime(year, mon, day, hour, min, sec);

        //update xtime
        write_lock(&xtime_lock);
        xtime.tv_sec = rts_secs;
        write_unlock(&xtime_lock);
}
//EXPORT_SYMBOL(sys_sync_rts_time);

static void mask_rtc_irq_bit(unsigned char bit)
{
        unsigned char val;

        spin_lock_irq(&rtc_lock);
        val = CMOS_READ(RTC_CONTROL);
        val &=  ~bit;
	/* make sure that alarm2 is independent to alarm1 */
	/* Do not touch INTCN, for defaulton/defaultoff.
	val |= RTC_INTCN;
	*/
        CMOS_WRITE(val, RTC_CONTROL);

	if(bit == RTC_AIE) {
        	val = CMOS_READ(RTC_STATUS);
		val &= ~RTC_AIF;
		CMOS_WRITE(val, RTC_STATUS);
	}
        rtc_irq_data = 0;
        spin_unlock_irq(&rtc_lock);
}

static void set_rtc_irq_bit(unsigned char bit)
{
        unsigned char val;

        spin_lock_irq(&rtc_lock);
        val = CMOS_READ(RTC_CONTROL);
        val |= bit;
	/* make sure that alarm2 is independent to alarm1 */
	/* Do not touch INTCN, for defaulton/defaultoff.
	val |= RTC_INTCN;
	*/
        CMOS_WRITE(val, RTC_CONTROL);
        CMOS_READ(RTC_STATUS);

        rtc_irq_data = 0;
        spin_unlock_irq(&rtc_lock);
}

MODULE_AUTHOR("Paul Gortmaker");
MODULE_LICENSE("GPL");
