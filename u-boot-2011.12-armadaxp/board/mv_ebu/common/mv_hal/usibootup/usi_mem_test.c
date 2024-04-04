#include "usibootup.h"
#include "cntmr/mvCntmr.h"
#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <rtc.h>

#define DEBUGP(fmt, args...)	printf(fmt, ##args)

#define USI_MEM_TEST_RANGE_MIN	0x1000000	/*  16M */
#define USI_MEM_TEST_RANGE_MAX	0x10000000	/* 256M */
#define MSEC_IN_EACH_STAGE	3600000

#define TIMER_LOAD_VAL 0xffffffff
/* the maximum value of TCLK counter contains, in milliseconds */
#define TIMER_LOAD_VAL_MSEC \
	(TIMER_LOAD_VAL/(MV_BOARD_REFCLK_25MHZ/1000))

/* get current TCLK counter, in milliseconds */
#define GET_CUR_MSEC() \
	(mvCntmrRead(UBOOT_CNTR)/(MV_BOARD_REFCLK_25MHZ/1000))

extern MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data);

static inline void led_set_in_stage1(void)
{
	mvEthPhyRegWrite(1,0x16,0x3);
	mvEthPhyRegWrite(1,0x10,0x181d);    //PHY1 LED[1] Blink
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x3);
	mvEthPhyRegWrite(0,0x10,0x1888);    //PHY0 LED[1] Solid OFF
	mvEthPhyRegWrite(0,0x16,0x0);
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED); //ERROR_LED OFF
}

static inline void led_set_in_stage2(void)
{
	mvEthPhyRegWrite(1,0x16,0x3);
	mvEthPhyRegWrite(1,0x10,0x1888);    //PHY1 LED[1] Solid OFF
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x3);
	mvEthPhyRegWrite(0,0x10,0x181d);    //PHY0 LED[1] Blink
	mvEthPhyRegWrite(0,0x16,0x0);
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED); //ERROR_LED OFF
}

static inline void led_set_in_stage3(void)
{
	mvEthPhyRegWrite(1,0x16,0x3);
	mvEthPhyRegWrite(1,0x10,0x1888);    //PHY1 LED[1] Solid OFF
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x3);
	mvEthPhyRegWrite(0,0x10,0x1888);    //PHY0 LED[1] Solid OFF
	mvEthPhyRegWrite(0,0x16,0x0);
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED ON
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED Blink
}

static inline void led_set_success(void)
{
	mvEthPhyRegWrite(1,0x16,0x3);
	mvEthPhyRegWrite(1,0x10,0x181c);    //PHY1 LED[1] Solid ON
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x3);
	mvEthPhyRegWrite(0,0x10,0x181c);    //PHY0 LED[1] Solid ON
	mvEthPhyRegWrite(0,0x16,0x0);
	MV_REG_BIT_RESET(GPP_BLINK_EN_REG(ERROR_LED_GRP), ERROR_LED);   //disable ERROR_LED Blink 
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);   //ERROR_LED ON
}

static inline void led_set_failure(void)
{
	mvEthPhyRegWrite(1,0x16,0x3);
	mvEthPhyRegWrite(1,0x10,0x181d);    //PHY1 LED[1] Blink
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x3);
	mvEthPhyRegWrite(0,0x10,0x181d);    //PHY0 LED[1] Blink
	mvEthPhyRegWrite(0,0x16,0x0);
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);   //ERROR_LED ON
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(ERROR_LED_GRP), ERROR_LED);     //ERROR_LED Blink
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

	if (is_btn_pressed(POWER, 0)) {
		/* cpu power down */
		set_power_off_mode();
		MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~(0x1 << 10));
		while(1);
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
				puts("\nSUCCESS !!!\n");
				puts("\nPlease press RESET Button to boot system\n");
				led_set_success();
				while (!is_btn_pressed(RESET, 0));
				/* cpu power down *
				udelay(500000);
				set_power_off_mode();
				MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~(0x1 << 10));
				while(1);
				*/
				return 0;
			default:
				return 1;
		}
	}

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
			if (addr == (end-start)/2) {
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
