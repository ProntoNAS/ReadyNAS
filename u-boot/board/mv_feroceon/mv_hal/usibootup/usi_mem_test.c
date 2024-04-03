/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Jean created this file, 2011-05-12
 * 
 */

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
	(TIMER_LOAD_VAL/(mvTclkGet()/1000))

/* get current TCLK counter, in milliseconds */
#define GET_CUR_MSEC() \
	(mvCntmrRead(UBOOT_CNTR)/(mvTclkGet()/1000))

static inline void led_set_in_stage1(void)
{
#ifdef USI_BOARD_NVPLUS_V4
	/*
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT20);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT23);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT24);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT29);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT23);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT24);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT29);
	*/
#else
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT31);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT20);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT23);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT31);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT21);
#endif
}

static inline void led_set_in_stage2(void)
{
#ifdef USI_BOARD_NVPLUS_V4
	/*
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT23);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT24);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT29);
	*/
#else
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT31);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT21);
#endif
}

static inline void led_set_in_stage3(void)
{
#ifdef USI_BOARD_NVPLUS_V4
	/*
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT23);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT24);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT29);
	*/
#else
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT31);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT21);
#endif
}

static inline void led_set_in_stage4(void)
{
#ifdef USI_BOARD_NVPLUS_V4
	/*
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT23);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT24);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT29);
	*/
#else
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT31);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT21);
#endif
}

static inline void led_set_success(void)
{
#ifdef USI_BOARD_NVPLUS_V4
	/*
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT23);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT24);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT29);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT23);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT24);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT29);
	*/
#else
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT31);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) & ~BIT21);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT31);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT20);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT22);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT21);
#endif
}

static inline void led_set_failure(void)
{
#ifdef USI_BOARD_NVPLUS_V4
	/*
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT23);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT24);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT29);
	*/
#else
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT31);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT20);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT22);
	MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT21);
#endif
}

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
#ifdef USI_BOARD_NVPLUS_V4
		LCD_show(MEMORY_TEST_POWEROFF);
#endif
		MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~(0x1 << 30));
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
				led_set_in_stage4();
				break;
			case 5:
				puts("\nSUCCESS !!!\n");
				led_set_success();
#ifdef USI_BOARD_NVPLUS_V4
				LCD_show(MEMORY_TEST_SUCCESS);
#endif
				while (!is_btn_pressed(POWER));
#ifdef USI_BOARD_NVPLUS_V4
				LCD_show(MEMORY_TEST_POWEROFF);
#endif
				udelay(500000);
				MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~(0x1 << 30));
				while(1);
				return 0;
			default:
				return 1;
		}
	}

#ifdef USI_BOARD_NVPLUS_V4
	sprintf (MSG[MEMORY_TEST_TIME],
		"    %02d:%02d:%02d    ",
		msec/3600000,
		(msec%3600000)/60000,
		((msec%3600000)/1000)%60);
	LCD_show(MEMORY_TEST_TIME);
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
#ifdef USI_BOARD_NVPLUS_V4
				sprintf (MSG[MEMORY_TEST_FAILURE],
					" FAIL: %08lX ",(uint)addr);
				LCD_show(MEMORY_TEST_FAILURE);
#endif
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
