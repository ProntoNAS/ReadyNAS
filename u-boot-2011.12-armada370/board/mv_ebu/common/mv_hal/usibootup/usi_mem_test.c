#include "usibootup.h"
#include "cntmr/mvCntmr.h"
#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <rtc.h>

#define DEBUGP(fmt, args...)	printf(fmt, ##args)

#define USI_MEM_TEST_RANGE_MIN	0x1000000	/*  16M */
#ifdef NETGEAR_RN25
#define USI_MEM_TEST_RANGE_MAX  0x10000000      /* 256M */
#else
#define USI_MEM_TEST_RANGE_MAX	0x20000000	/* 512M */
#endif
#define MSEC_IN_EACH_STAGE	3600000

#define TIMER_LOAD_VAL 0xffffffff
/* the maximum value of TCLK counter contains, in milliseconds */
#define TIMER_LOAD_VAL_MSEC \
	(TIMER_LOAD_VAL/(mvTclkGet()/1000))

/* get current TCLK counter, in milliseconds */
#define GET_CUR_MSEC() \
	(mvCntmrRead(UBOOT_CNTR)/(mvTclkGet()/1000))

extern void cpu_power_off(void);

static inline void led_set_in_stage1(void)
{
#ifdef NETGEAR_RN104
	/* Rn104 show status via the LCD */
#else
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(1), (POWER_LED | BACKUP_LED));
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), (SATA1_LED | SATA2_LED));
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(1), (POWER_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(1), (BACKUP_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(0), (SATA2_LED|SATA1_LED));
#endif
}

static inline void led_set_in_stage2(void)
{
#ifdef NETGEAR_RN104
	/* Rn104 show status via the LCD */
#else
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(1), (POWER_LED | BACKUP_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(0), (SATA2_LED));
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(0), (SATA1_LED));
#endif
}

static inline void led_set_in_stage3(void)
{
#ifdef NETGEAR_RN104
	/* Rn104 show status via the LCD */
#else
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(0), (SATA2_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(1), (POWER_LED | BACKUP_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(0), (SATA1_LED));
#endif
}

static inline void led_set_in_stage4(void)
{
#ifdef NETGEAR_RN104
	/* Rn104 show status via the LCD */
#else
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(1), (BACKUP_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(1), (POWER_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(0), (SATA1_LED| SATA2_LED));
#endif
}

static inline void led_set_success(void)
{
#ifdef NETGEAR_RN104
	/* Rn104 show status via the LCD */
#else
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(0), (SATA1_LED| SATA2_LED));
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(1), (POWER_LED| BACKUP_LED));
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(1), (POWER_LED | BACKUP_LED));
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(0), (SATA1_LED | SATA2_LED));
#endif
}

static inline void led_set_failure(void)
{
#ifdef NETGEAR_RN104
	/* Rn104 show status via the LCD */
#else
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(0), (SATA1_LED| SATA2_LED));
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(1), (POWER_LED| BACKUP_LED));
#endif
}

/* return 1 : go on test
 * return 0 : stop test
 * return -1 : fake a failure
 */
static int status(int init)
{
	static uint msec = 0, tmp1, tmp2;
	static int cur_stage = 1;

	if (init == 1)
	{
		tmp1 = GET_CUR_MSEC();
		led_set_in_stage1();
		return 1;
	}

	if (is_btn_pressed(POWER)) {
#ifdef NETGEAR_RN104
		LCD_show(MEMORY_TEST_POWEROFF, 2);
#endif
		cpu_power_off();
		return 0;
	}

	if (ctrlc()) {
		putc ('\n');
		return 0;
	}

	/* check rtc, turn on/off LEDs, or exit with success */

	tmp2 = GET_CUR_MSEC();
	if (tmp2 < tmp1) {
		msec += (tmp1 - tmp2);
	}
	/* counted to zero and turned back */
	else {
		msec += (TIMER_LOAD_VAL_MSEC - tmp2 + tmp1);
	}
	tmp1 = tmp2;

	if (msec >= MSEC_IN_EACH_STAGE * cur_stage) {
		cur_stage++;
		switch (cur_stage) {
			case 2:
				led_set_in_stage2();
				break;
			case 3:
				led_set_in_stage3();
				break;
			case 4:
				led_set_in_stage4();
				break;
			case 5:
				puts("\nSUCCESS !!!\n");
				led_set_success();
#ifdef NETGEAR_RN104
				LCD_show(MEMORY_TEST_SUCCESS, 2);
#endif
				while (!is_btn_pressed(POWER));
#ifdef NETGEAR_RN104
				LCD_show(MEMORY_TEST_POWEROFF, 2);
#endif
				udelay(500000);
				cpu_power_off();
				return 0;
			default:
				return 1;
		}
	}

#ifdef NETGEAR_RN104
	sprintf ((char *)MSG[MEMORY_TEST_TIME],
			"    %02d:%02d:%02d   ",/* 16 - 1 */
			msec/3600000,
			(msec%3600000)/60000,
			((msec%3600000)/1000)%60);
	MSG[MEMORY_TEST_TIME][15] = 0x20;  //use space instead of '\0'
	
	LCD_show(MEMORY_TEST_TIME, 2);
#endif

	return 1;
}


int usi_mem_test(ulong mem_start, ulong mem_end)
{
	vu_long	*addr, *start, *end;
	int incr = 1;
	ulong pattern = 0x55555555;
	ulong val;
	ulong readback;
	int rcode = 0;

	start = (mem_start < USI_MEM_TEST_RANGE_MIN) ?
		(vu_long *)USI_MEM_TEST_RANGE_MIN :
		(vu_long *)mem_start;

	end = (mem_end > USI_MEM_TEST_RANGE_MAX) ?
		(vu_long *)USI_MEM_TEST_RANGE_MAX :
		(vu_long *)mem_end;

	status(1);

	for (;;) {

		printf ("\rPattern %08lX    Writing...  "
				"%12s"
				"\b\b\b\b\b\b\b\b\b\b",
				pattern, "");

		if (status(0) == 0)
		{
			return rcode;
		}

		for (addr=start,val=pattern; addr<end; addr++) {
			*addr = val;
			val  += incr;
			if ((long)addr == (end-start)/2) {
				if (status(0) == 0)
					return rcode;
			}
		}

		puts ("Reading...");

		if (status(0) == 0)
		{
			return rcode;
		}

		for (addr=start,val=pattern; addr<end; addr++) {
			readback = *addr;
			if (readback != val) {
				printf ("\nMem error @ 0x%08X: "
						"found %08lX, expected %08lX\n",
						(uint)addr, readback, val);
				rcode = 1;
				led_set_failure();
#ifdef NETGEAR_RN104
				sprintf ((char *)MSG[MEMORY_TEST_FAILURE],
						" FAIL: %08lX ",(ulong)addr);
				LCD_show(MEMORY_TEST_FAILURE, 2);
#endif
				while(1);
			}
			val += incr;
		}

		/*
		 * Flip the pattern each time to make lots of zeros and
		 * then, the next time, lots of ones.  We decrement
		 * the "negative" patterns and increment the "positive"
		 * patterns to preserve this feature.
		 */
		if(pattern & 0x80000000) {
			pattern = -pattern;	/* complement & increment */
		}
		else {
			pattern = ~pattern;
		}
		incr = -incr;
	}
	return rcode;
}
#if 0
int do_usimemtest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong start = 0, end = 0xffffffff;
	if (argc > 1) {
		start = (ulong)simple_strtoul(argv[1], NULL, 16);
	}
	if (argc > 2) {
		end = (ulong)simple_strtoul(argv[2], NULL, 16);
	}
	return usi_mem_test(start, end);
}

U_BOOT_CMD(
		usimemtest,     3,     1,      do_usimemtest,
		"usimemtest [start] [end] - USI Memory Test Tool\n",
		"\n"
);
#endif
