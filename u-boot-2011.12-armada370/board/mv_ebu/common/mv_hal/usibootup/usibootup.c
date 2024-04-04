#include "usibootup.h"
#include "usi_mem_test.h"
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <asm/byteorder.h>
#include "eth-phy/mvEthPhy.h"
#if defined(CONFIG_CMD_NAND)
#include <nand.h>
#endif

#define ENVVER 3
#define ENVVER_STR "3"

MV_U32 usi_board_type_flag;
static inline void turn_on_backup_led(void);
static inline void turn_off_backup_led(void);

static inline void turn_on_backup_led(void)
{
#ifdef NETGEAR_RN104
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(BACKUP_LED_GRP), BACKUP_LED);
#else
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(BACKUP_LED_GRP), BACKUP_LED);
#endif
}

static inline void turn_off_backup_led(void)
{
#ifdef NETGEAR_RN104
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(BACKUP_LED_GRP), BACKUP_LED);
#else
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(BACKUP_LED_GRP), BACKUP_LED);
#endif
}

inline void turn_on_power_led(void)
{
	MV_REG_BIT_RESET(GPP_DATA_OUT_REG(POWER_LED_GRP), POWER_LED);
}

extern MV_32 usi_use_defaultenv;
MV_U32 boot_to_usb_drive(void)
{

	MV_32 delaytime = 0;

	if (MV_REG_READ(GPP_DATA_IN_REG(BACKUP_BTN_GRP)) & BACKUP_BTN) {
		turn_on_backup_led();

		while (1) {
			if ((MV_REG_READ(GPP_DATA_IN_REG(BACKUP_BTN_GRP)) & BACKUP_BTN) == 0)
				break;

			udelay(100000); //every 100ms
			delaytime++;
			if (delaytime >= 10)
				break;
		}
		turn_off_backup_led();
	}
	/*press button > 5s to usb recovery*/
	if ( delaytime >= 10 ) {
		setenv("bootcmd", "usb start;fatload usb 0:1 0x2000000 /uImage-recovery;fatload usb 0:1 0x3000000 /initrd-recovery.gz;nand read 0x1000000 0xD4000 0x6000; bootm 0x2000000 0x3000000 0x1000000");
		/*backup led blink*/
		MV_REG_BIT_SET(GPP_BLINK_EN_REG(BACKUP_LED_GRP), BACKUP_LED);
#ifdef NETGEAR_RN104
		LCD_show(BOOT_FROM_USB, 1);
		LCD_show(SPACE, 2);
#endif
		return 1;
	}
	return 0;
}

#ifdef NETGEAR_RN104
static MV_8 LCD_init_tbl[] = {
	0x30, 0x30, 0x30,
	0x20, 0x28,
	0x08,
	0x01, 0x07,
	0xA0,
};

#define LCD_udelay              udelay
#define LCD_BL_GPP_GROUP        1
#define LCD_BL_BIT              BIT21
#define LCD_BL_BIT_SHIFT        21

#define LCD_RS_GPP_GROUP        1
#define LCD_RS_BIT              BIT22
#define LCD_RS_BIT_SHIFT        22

#define LCD_RW_GPP_GROUP        1
#define LCD_RW_BIT              BIT23
#define LCD_RW_BIT_SHIFT        23

#define LCD_E_GPP_GROUP         1
#define LCD_E_BIT               BIT24
#define LCD_E_BIT_SHIFT         24

#define LCD_INSTRUCTION         0
#define LCD_DATA                1
#define LCD_WR                  0
#define LCD_RD

static void LCD_set_BL(MV_U32 on_off)
{
	/* BL is low active */
	MV_U32 val = on_off ^ 1;

	MV_U32 gppData;

	gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP));
	gppData &= ~LCD_BL_BIT;
	gppData |= ((val << LCD_BL_BIT_SHIFT) & LCD_BL_BIT);
	MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP), gppData);
}
#if 0
static MV_U32 LCD_get_BL_status(void)
{
	MV_U32 gppData;

	gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP));
	gppData &= LCD_BL_BIT;
	gppData >>= LCD_BL_BIT_SHIFT;
	return (gppData ^ 1);
}
#endif
static void LCD_set_RS(MV_U32 val)
{
	MV_U32 gppData;

	gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_RS_GPP_GROUP));
	gppData &= ~LCD_RS_BIT;
	gppData |= ((val << LCD_RS_BIT_SHIFT) & LCD_RS_BIT);
	MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_RS_GPP_GROUP), gppData);
}

static void LCD_set_RW(MV_U32 val)
{
	MV_U32 gppData;

	gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_RW_GPP_GROUP));
	gppData &= ~LCD_RW_BIT;
	gppData |= ((val << LCD_RW_BIT_SHIFT) & LCD_RW_BIT);
	MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_RW_GPP_GROUP), gppData);
}

static void LCD_set_E(MV_U32 val)
{
	MV_U32 gppData;

	gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_E_GPP_GROUP));
	gppData &= ~LCD_E_BIT;
	gppData |= ((val << LCD_E_BIT_SHIFT) & LCD_E_BIT);
	MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_E_GPP_GROUP), gppData);
}

#define LCD_DB4                 BIT25
#define LCD_DB4_GPP_GROUP       1
#define LCD_DB4_SHIFT           25

#define LCD_DB5                 BIT26
#define LCD_DB5_GPP_GROUP       1
#define LCD_DB5_SHIFT           26

#define LCD_DB6                 BIT27
#define LCD_DB6_GPP_GROUP       1
#define LCD_DB6_SHIFT           27

#define LCD_DB7                 BIT29
#define LCD_DB7_GPP_GROUP       1
#define LCD_DB7_SHIFT           29

static void LCD_set_onedb(MV_U32 group, MV_U32 mask, MV_U32 shift, MV_U32 val)
{
	MV_U32 gppData;

	gppData = MV_REG_READ(GPP_DATA_OUT_REG(group));
	gppData &= ~mask;
	gppData |= ((val << shift) & mask);
	MV_REG_WRITE(GPP_DATA_OUT_REG(group), gppData);
}

static void LCD_fill_db(MV_U32 val)
{
	LCD_set_onedb(LCD_DB4_GPP_GROUP, LCD_DB4, LCD_DB4_SHIFT, (val & 0x1));
	LCD_set_onedb(LCD_DB5_GPP_GROUP, LCD_DB5, LCD_DB5_SHIFT, (val & 0x2) >> 1);
	LCD_set_onedb(LCD_DB6_GPP_GROUP, LCD_DB6, LCD_DB6_SHIFT, (val & 0x4) >> 2);
	LCD_set_onedb(LCD_DB7_GPP_GROUP, LCD_DB7, LCD_DB7_SHIFT, (val & 0x8) >> 3);
}

static void LCD_send_data(MV_U8 val, MV_U32 flag)
{

	MV_U32 data_h = val >> 4;
	MV_U32 data_l = val;

	LCD_set_RW(LCD_WR);
	LCD_set_RS(flag);
	LCD_set_E(1);
	LCD_udelay(1);

	LCD_fill_db(data_h);
	LCD_udelay(1000);
	LCD_set_E(0);
	LCD_udelay(1000);

	LCD_set_E(1);
	LCD_udelay(1);
	LCD_fill_db(data_l);
	LCD_udelay(1000);
	LCD_set_E(0);
	LCD_udelay(1000);
}

static void LCD_init(void)
{
	MV_32 i;

	for (i = 0; i < sizeof(LCD_init_tbl) / sizeof(MV_8); i++) {
		LCD_udelay(5000);
		LCD_send_data(LCD_init_tbl[i], LCD_INSTRUCTION);
	}
	LCD_set_E(1);
	LCD_set_RS(LCD_DATA);
	LCD_udelay(10);

	LCD_set_E(0);
}
static void LCD_select_line(MV_32 line);
#endif

void cpu_power_off(void)
{
	MV_U8 value = 0;
	MV_U16 phyval;

	if (i2c_read(0x68, 0x0b, 1, &value, 1) != 0)
		puts("Error reading the ISL12057.\n");
	if (usi_board_type_flag == USI_MVT_MP) {
		if (!(value & BIT1)) {
			/* Power off mode2(WOL disabled) */
			//Force LED0 on (LED0 out put low)
			mvEthPhyRegWrite(0, 0x16, 0x3);
			mvEthPhyRegRead(0, 0x10, &phyval);
			phyval &= ~0xf;
			phyval |= 0x9;
			mvEthPhyRegWrite(0, 0x10, phyval);
			mvEthPhyRegWrite(0, 0x16, 0x0);
		} else {
			/* Power off mode1(WOL enabled) */
			mvEthPhyRegWrite(0, 22, 17);
			mvEthPhyRegWrite(0, 16, 0x5500);
			mvEthPhyRegWrite(0, 22, 0);
			mvEthPhyRegWrite(0, 9, 0x1800);
			/* 0_0.10 = 1. The leakage is from PHY 88E1318 RGMII TX. */
			mvEthPhyRegWrite(0, 0, 0x500);
#ifdef NETGEAR_RN104
			mvEthPhyRegWrite(1, 22, 17);
			mvEthPhyRegWrite(1, 16, 0x5500);
			mvEthPhyRegWrite(1, 22, 0);
			mvEthPhyRegWrite(1, 9, 0x1800);
			/* 0_0.10 = 1. The leakage is from PHY 88E1318 RGMII TX. */
			mvEthPhyRegWrite(1, 0, 0x500);
#endif
			//Force LED0 off (LED0 out put high)
			mvEthPhyRegWrite(0, 0x16, 0x3);
			mvEthPhyRegRead(0, 0x10, &phyval);
			phyval &= ~0xf;
			phyval |= 0x8;
			mvEthPhyRegWrite(0, 0x10, phyval);
			mvEthPhyRegWrite(0, 0x16, 0x0);
		}
		udelay(100000);
#ifdef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(CPUPW_OFF_GRP), CPUPW_OFF_MVT_MP);
#else
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(CPUPW_OFF_GRP), (CPUPW_OFF));
#endif
	} else {
		if (!(value & BIT1)) {
			/* Disable WOL */
			phyval = 0x8d00;
			mvEthPhyRegWrite(0, 0x0, phyval);
			phyval = 0xd00;
			mvEthPhyRegWrite(0, 0x0, phyval);
#ifdef NETGEAR_RN104
			phyval = 0x8d00;
			mvEthPhyRegWrite(1, 0x0, phyval);
			phyval = 0xd00;
			mvEthPhyRegWrite(1, 0x0, phyval);
#endif
		} else {
			/* Enable WOL */
			mvEthPhyRegWrite(0, 22, 17);
			mvEthPhyRegWrite(0, 16, 0x5500);
			mvEthPhyRegWrite(0, 22, 0);
			mvEthPhyRegWrite(0, 9, 0x1800);
			/* 0_0.10 = 1. The leakage is from PHY 88E1318 RGMII TX. */
			mvEthPhyRegWrite(0, 0, 0x500);
#ifdef NETGEAR_RN104
			mvEthPhyRegWrite(1, 22, 17);
			mvEthPhyRegWrite(1, 16, 0x5500);
			mvEthPhyRegWrite(1, 22, 0);
			mvEthPhyRegWrite(1, 9, 0x1800);
			/* 0_0.10 = 1. The leakage is from PHY 88E1318 RGMII TX. */
			mvEthPhyRegWrite(1, 0, 0x500);
#endif
		}
		udelay(100000);
#ifdef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(CPUPW_OFF_GRP), CPUPW_OFF_EVT_DVT);
#else
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(CPUPW_OFF_GRP), (CPUPW_OFF));
#endif
	}
	while (1) ;
}

void usiBootup_to_kernel(void)
{
	MV_U16 bootflag;
	MV_U32 i;
	MV_8 *env;
	MV_U16 phyval;
	MV_U8 ethaddrhex[6];
	MV_U8 phy_num = 1;
	MV_U16 eth1addrphyval[3];

#ifdef NETGEAR_RN104
	MV_U8 pca9554_byte = 0x0;
	MV_U32 ndisks = 0;
	MV_U8 leds = 0xF0;
	MV_U16 ethaddrphyval[3];
	phy_num = 2;
#endif
	MV_U32 hdds;
	MV_U8 value_rtc = 0;
	MV_U32 VCC_12V_first_plugin_flag = MV_REG_READ(GPP_DATA_IN_POL_REG(1)) & BIT0;

#if defined(NETGEAR_RN104) || defined(NETGEAR_RN102)
	env = getenv("cpu");
	if (!env || strcmp(env, "high") != 0) {
		MV_U32 reg = 0x7F80;
		MV_REG_WRITE(SSCG_CONF_REG, reg);
		reg = MV_REG_READ(0x1871c);
		MV_REG_WRITE(0x1871c, reg - 4);
	}
#endif

	/* Close IRQ1/FOUT */
	if (i2c_read(0x68, 0x0e, 1, &value_rtc, 1) != 0)
		puts("Error reading the ISL12057.\n");
	if (!(value_rtc & BIT2)) {
		value_rtc |= BIT2;
		if (i2c_write(0x68, 0x0e, 1, &value_rtc, 1) != 0)
			puts("Error writing the ISL12057.\n");
	}
#ifdef NETGEAR_RN104
	/* Set the pca9554's PIN0 - PIN7 to output. */
	if (i2c_write(0x23, 0x3, 1, &pca9554_byte, 1) != 0)
		puts("Error writing the i2c chip : pca9554.\n");

	hdds = (MV_REG_READ(GPP_DATA_IN_REG(1)) & 0xF0000) >> 16;
	for (i = 0; i < 4; i++) {
		if (hdds & (1 << i))
			leds |= (1 << -(i - 3));
		else
			ndisks++;
	}

	if (i2c_read(0x23, 0x1, 1, &pca9554_byte, 1) != 0)
		puts("Error reading the i2c chip : pca9554.\n");
	/* If some HDD LEDs are still turned on, it must be a reboot;
	 * so no need to cut power and spin the disks back up. */
	if ((pca9554_byte & 0xF) == 0xF && ndisks > 2) {
		/* Enable power on the first two SATA channels */
		pca9554_byte = 0x3F;
		if (i2c_write(0x23, 0x1, 1, &pca9554_byte, 1) != 0)
			puts("Error writing the i2c chip : pca9554.\n");
	}
#else
	hdds = MV_REG_READ(GPP_DATA_IN_REG(0));
#endif
	/* Get board status information. */
	mvEthPhyRegWrite(0x0, 0x16, 0x2);
	mvEthPhyRegRead(0x0, 0x18, &bootflag);
	mvEthPhyRegWrite(0x0, 0x16, 0x0);
	bootflag &= 0x0141;

	env = getenv("eth1addr");
	mvMacStrToHex(env, ethaddrhex);

	eth1addrphyval[0] = ethaddrhex[5];
	eth1addrphyval[0] = (eth1addrphyval[0] << 8) + ethaddrhex[4];
	eth1addrphyval[1] = ethaddrhex[3];
	eth1addrphyval[1] = (eth1addrphyval[1] << 8) + ethaddrhex[2];
	eth1addrphyval[2] = ethaddrhex[1];
	eth1addrphyval[2] = (eth1addrphyval[2] << 8) + ethaddrhex[0];

#ifdef NETGEAR_RN104
	env = getenv("ethaddr");
	mvMacStrToHex(env, ethaddrhex);

	ethaddrphyval[0] = ethaddrhex[5];
	ethaddrphyval[0] = (ethaddrphyval[0] << 8) + ethaddrhex[4];
	ethaddrphyval[1] = ethaddrhex[3];
	ethaddrphyval[1] = (ethaddrphyval[1] << 8) + ethaddrhex[2];
	ethaddrphyval[2] = ethaddrhex[1];
	ethaddrphyval[2] = (ethaddrphyval[2] << 8) + ethaddrhex[0];
#endif

	for (i = 0; i < phy_num; i++) {
		/* To correct the RGMII output impedance */
		mvEthPhyRegWrite(i, 0x16, 0x2);         //page 2
		mvEthPhyRegRead(i, 0x18, &phyval);
		phyval |= 0x0141;                       //set bit6=1;bits 2:0=001;bits 10:8=001
		mvEthPhyRegWrite(i, 0x18, phyval);
		/* Close 125M clk */
		mvEthPhyRegWrite(i, 0x10, 0x74a);
		mvEthPhyRegWrite(i, 0x16, 0x0);           //back to page 0

		/****** WOL init start ***********/
		mvEthPhyRegWrite(i, 0x16, 0x3);
		//Force led2 off
		mvEthPhyRegRead(i, 0x10, &phyval);
		phyval &= ~(0xf << 8);
		phyval |= (0x8 << 8);
		mvEthPhyRegWrite(i, 0x10, phyval);
		//set led2 polarity and Bi-Color LED Mixing(LED1 on and LED0 off)
		mvEthPhyRegWrite(i, 0x11, 0x8000);
		//bit7 led interrupt and bit11 INTn active low
		mvEthPhyRegWrite(i, 0x12, 0x4985);

		//WOL interrupt enable
		mvEthPhyRegWrite(i, 0x16, 0x0);
		mvEthPhyRegWrite(i, 0x12, 0x80);
#ifdef NETGEAR_RN104
		if (i == 0) {
			mvEthPhyRegWrite(i, 0x16, 0x11);
			mvEthPhyRegWrite(i, 0x17, ethaddrphyval[0]);
			mvEthPhyRegWrite(i, 0x18, ethaddrphyval[1]);
			mvEthPhyRegWrite(i, 0x19, ethaddrphyval[2]);
		} else {
			mvEthPhyRegWrite(i, 0x16, 0x11);
			mvEthPhyRegWrite(i, 0x17, eth1addrphyval[0]);
			mvEthPhyRegWrite(i, 0x18, eth1addrphyval[1]);
			mvEthPhyRegWrite(i, 0x19, eth1addrphyval[2]);
		}
#else
		mvEthPhyRegWrite(i, 0x16, 0x11);
		mvEthPhyRegWrite(i, 0x17, eth1addrphyval[0]);
		mvEthPhyRegWrite(i, 0x18, eth1addrphyval[1]);
		mvEthPhyRegWrite(i, 0x19, eth1addrphyval[2]);
#endif
		//enable mac WOL and into low powerstate
		mvEthPhyRegWrite(i, 0x16, 0x11);
		mvEthPhyRegWrite(i, 0x10, 0x5500);        //FIXME 0x4500 -> 0x5500
		mvEthPhyRegWrite(i, 0x16, 0x0);
		/****** WOL init end ***********/
	}

	env = getenv("HW_version");
	if ((env != NULL) && (strcmp(env, "MVT") == 0))
		usi_board_type_flag = USI_MVT_MP;
	else if ((env != NULL) && (strcmp(env, "DVT") == 0))
		usi_board_type_flag = USI_EVT_DVT;
	else {
		/* Auto detect board type and save it */
		MV_U32 VCC_12V_first_plugin_flag_stage2 = MV_REG_READ(GPP_DATA_IN_REG(1)) & BIT0;
		if (VCC_12V_first_plugin_flag && VCC_12V_first_plugin_flag_stage2) {
			/* one more 3 seconds to prevent incorrect judgement*/
			udelay(3000000);
			VCC_12V_first_plugin_flag_stage2 = MV_REG_READ(GPP_DATA_IN_REG(1)) & BIT0;
			if (VCC_12V_first_plugin_flag_stage2) {
				usi_board_type_flag = USI_EVT_DVT;
				setenv("HW_version", "DVT");
				printf("Detect EVT/DVT board...\n");
			} else {
				usi_board_type_flag = USI_MVT_MP;
				setenv("HW_version", "MVT");
				printf("ho! Detect MVT/MP board...\n");
			}
		} else {
			usi_board_type_flag = USI_MVT_MP;
			setenv("HW_version", "MVT");
			printf("Detect MVT/MP board...\n");
		}
		run_command("saveenv", 0);
	}

	env = getenv("AC_Power_fail_detect");
	/* Using A2MN(bit0) of ISL12057 for ACPower_fail record, if bit0 is '1', ACPower failed */
	MV_U8 value = 0;
	if (i2c_read(0x68, 0x0b, 1, &value, 1) != 0)
		puts("Error reading the ISL12057.\n");

	if (usi_board_type_flag != USI_MVT_MP)
		/* EVT/DVT board */
		VCC_12V_first_plugin_flag = (bootflag != 0x0141);

	/* If it's a new plugin event, always power on if one of the auxiliary buttons are pressed.
	 * This is a failsafe for non-production hardware. */
	if (VCC_12V_first_plugin_flag) {
		if ((MV_REG_READ(GPP_DATA_IN_REG(BACKUP_BTN_GRP)) & BACKUP_BTN) ||
		    (MV_REG_READ(GPP_DATA_IN_REG(RESET_BTN_GRP)) & RESET_BTN) ||
		    (MV_REG_READ(GPP_DATA_IN_REG(POWER_BTN_GRP)) & POWER_BTN))
			VCC_12V_first_plugin_flag = 0;
	}

	if (VCC_12V_first_plugin_flag && (!env || (strcmp(env, "close") == 0) || ((strcmp(env, "open") == 0) && ((value & BIT0) == 0x0)))) {
		for (i = 0; i < phy_num; i++) {
			//power down mode.
			mvEthPhyRegWrite(i, 0x9, 0x1800);
			/* 0_0.10 = 1. The leakage is from PHY 88E1318 RGMII TX. */
			mvEthPhyRegWrite(i, 0x0, 0x500); //10M
		}

		/*
		 * To clear the Alarm 1.
		 */
		if (i2c_read(0x68, 0x0f, 1, &value, 1) != 0)
			puts("Error reading the ISL12057.\n");
		if (value & BIT0) {
			value &= (~BIT0);
			if (i2c_write(0x68, 0x0f, 1, &value, 1) != 0)
				puts("Error writing the ISL12057.\n");
			udelay(1000000);
			value |= BIT0;
			if (i2c_write(0x68, 0x0f, 1, &value, 1) != 0)
				puts("Error writing the ISL12057.\n");
		}
		printf("Power supply plug on, please press power button!\n\n");
		cpu_power_off();
	} else{
#ifdef NETGEAR_RN104
		LCD_init();
		LCD_set_BL(1);
		LCD_show(BRAND, 1);
		LCD_show(WELCOME, 2);
#endif
		for (i = 0; i < phy_num; i++) {
			mvEthPhyRegWrite(i, 0x16, 0x0);
			mvEthPhyRegWrite(i, 0x9, 0x300);        //back to 1G
			mvEthPhyRegWrite(i, 0x0, 0x9140);       //auto-negotiation and reset
			/*disable WOL in kernel*/
			mvEthPhyRegWrite(i, 0x16, 0x11);
			mvEthPhyRegWrite(i, 0x10, 0x1500);
			mvEthPhyRegWrite(i, 0x16, 0x0);
		}
		/* modify 1318 PHY VOD value for signal which is neccesery */
#ifdef NETGEAR_RN104
		mvEthPhyRegWrite(0, 22, 0xff);
		mvEthPhyRegWrite(0, 24, 0x2b09);
		mvEthPhyRegWrite(0, 23, 0x2012);
		mvEthPhyRegWrite(0, 22, 0x0);
		mvEthPhyRegWrite(1, 22, 0xff);
		mvEthPhyRegWrite(1, 24, 0x000d);
		mvEthPhyRegWrite(1, 23, 0x2012);
		mvEthPhyRegWrite(1, 22, 0x0);
#elif defined(NETGEAR_RN102) || defined(NETGEAR_RN25)
		mvEthPhyRegWrite(0, 22, 0xff);
		mvEthPhyRegWrite(0, 24, 0xe99a);
		mvEthPhyRegWrite(0, 23, 0x2012);
		mvEthPhyRegWrite(0, 22, 0x0);
#endif
		MV_REG_BIT_SET(GPP_BLINK_EN_REG(POWER_LED_GRP), POWER_LED);
		printf("Power On!\n\n");
	}
	/* Turn G76x(FAN controller, i2c address 0x3e) on.
	 * The FAN_SET_CNT register's offset is 0x0.
	 * Set [1300(rpm) == 65% == 5a(FAN_SET_CNT)] as default.
	 */
	MV_U8 byte = 0x5a;
#ifndef NETGEAR_RN25
	if (i2c_write(0x3e, 0x0, 1, &byte, 1) != 0)
		puts("Error writing the i2c chip : G76x(Fan controller).\n");
	/* Tune the FAN_STARTV */
	if (i2c_read(0x3e, 0x5, 1, &byte, 1) != 0)
		puts("Error reading the i2c chip : G76x(Fan controller).\n");
	byte |= 0x3;
	if (i2c_write(0x3e, 0x5, 1, &byte, 1) != 0)
		puts("Error writing the i2c chip : G76x(Fan controller).\n");
#else
	/* FAN present? MPP65# */
	if (MV_REG_READ(GPP_DATA_IN_REG(2)) & BIT1) { /* Not detected */
		/* FAN power control: FAN_PW_ON, MPP7 */
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(0), BIT7);
		byte = 0;
	} else { /* Detected */
		/* FAN power control: FAN_PW_ON, MPP7 */
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), BIT7);
		byte = 0x3a;      /* Fan version 1.0 */
	}
	if (i2c_write(0x3e, 0x0, 1, &byte, 1) != 0)
		puts ("Error writing the i2c chip : G76x(Fan controller).\n");
#endif
	/* usi_use_defaultenv == 1 means there is no env in nand flash, so we need to
	 * save it using default value
	 */
	env = getenv("envver");
	if (!env || usi_use_defaultenv || simple_strtoul(env, NULL, 0) < ENVVER) {
		printf("Updating env to v" ENVVER_STR "...\n");
		setenv("mtdparts", "mtdparts=armada-nand:0x180000@0(u-boot),0x20000@0x180000(u-boot-env),0x600000@0x200000(uImage),0x400000@0x800000(minirootfs),-(ubifs)");
		setenv("bootcmd_ubi", "ubi part ubifs; ubifsmount rootfs; ubifsload 0x2000000 kernel; ubifsload 0x3000000 initrd.gz; bootm 0x2000000 0x3000000 0x1000000");
		setenv("bootcmd", "nand read 0x2000000 0x200000 0x400000; nand read 0x3000000 0x800000 0x400000; bootm 0x2000000 0x3000000 0x1000000");
		setenv("bootdelay", "0");
		setenv("envver", ENVVER_STR);
		run_command("saveenv", 0);
	}

	env = getenv("bootargs");
	if (!env || strstr(env, "mtdparts") || strstr(env, "reason")) {
		printf("The parameter <mtdparts> and <reason> are dynamic, and can not be programmed into nand flash. Erase it!\n");
		setenv("bootargs", "console=ttyS0,115200");
		run_command("saveenv", 0);
	}

	if (nand_load_fdt(0xD4000, 0x6000, (u_char *)0x1000000) == 0)
		printf("FDT loaded successfully\n");
	else
		printf("Loading FDT from NAND 0xEC000 to 0x1000000 failed!\n");

	if (boot_to_usb_drive())
		goto HDD_power_on;

	/* BOOT MENU */
	MV_U32 is_bootmenu_flag = 0;

	if (MV_REG_READ(GPP_DATA_IN_REG(RESET_BTN_GRP)) & RESET_BTN) {
		MV_U32 holdon_cnt = 0;
		for (i = 0; i < HOLDON_LEN; i++) {
			udelay(100000);
			if (MV_REG_READ(GPP_DATA_IN_REG(RESET_BTN_GRP)) & RESET_BTN)
				holdon_cnt++;
			else
				break;
		}
		if (holdon_cnt == HOLDON_LEN)
			is_bootmenu_flag = 1;
	}
	if (is_bootmenu_flag) {
		MV_U32 bootmenu_item_len = 7;
		MV_32 bootmenu_item_cnt = -1;

		setenv("Startup", "Manual");
		run_command("saveenv", 0);

#ifdef NETGEAR_RN104
		LCD_show(SPACE, 2);
#else
		MV_REG_BIT_RESET(GPP_BLINK_EN_REG(POWER_LED_GRP), POWER_LED);
#endif
		menu_show(bootmenu_item_cnt);
		while (1) {
			if (is_btn_pressed(USB)) {
				bootmenu_item_cnt = (bootmenu_item_cnt + 1) % bootmenu_item_len;
				menu_show(bootmenu_item_cnt);
				break;
			}
		}

		while (1) {
			if (is_btn_pressed(USB)) {
				bootmenu_item_cnt = (bootmenu_item_cnt + 1) % bootmenu_item_len;
				menu_show(bootmenu_item_cnt);
			}
			if (is_btn_pressed(RESET)) {
				save_user_selection(bootmenu_item_cnt);
				break;
			}
		}
#ifdef NETGEAR_RN104
		//menu_show(0);//set the LED's status to normal.
		LCD_show(BOOTING, 1);
		LCD_show(SPACE, 2);
#endif
	} else {
		env = getenv("Startup");
		if ((env == NULL) || (strcmp(env, "Normal") != 0)) {
			setenv("Startup", "Normal");
			run_command("saveenv", 0);
		}
	}
#ifndef NETGEAR_RN104
	if (hdds & BIT12)
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA1_LED_GRP), SATA1_LED);
	else
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA1_LED_GRP), SATA1_LED);
	if (hdds & BIT10)
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA2_LED_GRP), SATA2_LED);
	else
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA2_LED_GRP), SATA2_LED);
	turn_off_backup_led();
#endif
	MV_REG_BIT_SET(GPP_BLINK_EN_REG(POWER_LED_GRP), POWER_LED);

 HDD_power_on:
#ifdef NETGEAR_RN104
	if (pca9554_byte == 0x3F) {
		printf("Delay 7s then power on another group of HDDs   ");
		for (i = 7; i > 0; i--) {
			printf("\b\b\b%2d ", i);
			udelay(1000000);
		}
		printf("\n");
	}
	pca9554_byte = leds;
	if (i2c_write(0x23, 0x1, 1, &pca9554_byte, 1) != 0)
		puts("Error writing the i2c chip : pca9554.\n");
#else
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), (BIT11 | BIT13));
#endif
}

MV_U32 is_btn_pressed(enum BTN_ID btn_id)
{

	MV_U32 ret = 0;
	MV_U32 btn2gpp_bitmap = 0;
	MV_U32 gpp_group = 0;

	switch (btn_id) {
	case USB:
		gpp_group = BACKUP_BTN_GRP;
		btn2gpp_bitmap = BACKUP_BTN;
		break;
	case RESET:
		gpp_group = RESET_BTN_GRP;
		btn2gpp_bitmap = RESET_BTN;
		break;
	case POWER:
		gpp_group = POWER_BTN_GRP;
		btn2gpp_bitmap = POWER_BTN;
		break;
	default:
		return 0;
	}
	/* GPP attributes must have been set before */
	if (MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap) {
		/* eliminate jitter */
		udelay(10000); //10 ms
		if (MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap) {
			if (btn_id == USB || btn_id == RESET) {
				/* If the usb or reset button was pressed, wait until button up then respond*/
				while (MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap)
					;
			}
			ret = 1;
		}
	}
	return ret;
}

void menu_show(MV_U32 bootmenu_item_cnt)
{
	MV_U32 bootmenu_item = bootmenu_item_cnt + 1; //Mapping

#ifndef NETGEAR_RN104
	//Rn104 show the boot menu via the LCD
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA1_LED_GRP), SATA1_LED);
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA2_LED_GRP), SATA2_LED);
	turn_off_backup_led();
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(POWER_LED_GRP), POWER_LED);
	udelay(100);
#endif

	switch (bootmenu_item) {
	case BOOTMENU:
#ifndef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA1_LED_GRP), SATA1_LED);
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA2_LED_GRP), SATA2_LED);
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(POWER_LED_GRP), POWER_LED);
		turn_on_backup_led();
#endif
		puts("Enter Bootmenu...\n");
		break;
	case NORMAL:
#ifndef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(POWER_LED_GRP), (POWER_LED));
#endif
		puts("Bootmenu : NORMAL\n");
		break;
	case FACTORY_DEFAULT:
#ifndef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA1_LED_GRP), (SATA1_LED));
#endif
		puts("Bootmenu : FACTORY_DEFAULT\n");
		break;
	case OS_REINSTALL:
#ifndef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA2_LED_GRP), (SATA2_LED));
#endif
		puts("Bootmenu : OS_REINSTALL\n");
		break;
	case TECH_SUPPORT:
#ifndef NETGEAR_RN104
		turn_on_backup_led();
#endif
		puts("Bootmenu : TECH_SUPPORT\n");
		break;
	case VOL_RO:
#ifndef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA1_LED_GRP), SATA1_LED);
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(POWER_LED_GRP), POWER_LED);
#endif
		puts("Bootmenu : VOL_RO\n");
		break;
	case MEMORY_TEST:
#ifndef NETGEAR_RN104
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(SATA2_LED_GRP), SATA2_LED);
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(POWER_LED_GRP), (POWER_LED));
#endif
		puts("Bootmenu : MEMORY_TEST\n");
		break;
	case TEST_DISK:
#ifndef NETGEAR_RN104
		turn_on_backup_led();
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(POWER_LED_GRP), (POWER_LED));
#endif
		puts("Bootmenu : TEST_DISK\n");
		break;
	default:
		break;
	}
#ifdef NETGEAR_RN104
	LCD_show(bootmenu_item, (bootmenu_item == BOOTMENU) ? 1 : 2);
#endif
	return;
}

#ifdef NETGEAR_RN104
MV_U8 MSG[][16] = {
	"   Boot  Menu   ",
	"     Normal     ",
	"Factory  Default",
	"  OS Reinstall  ",
	"  Tech Support  ",
	"Volume Read Only",
	"  Memory  Test  ",
	"   Test Disks   ",
	"                ",
	"Booting..       ",
	"    00:00:00    ",
	"*** SUCCESS! ***",
	" FAIL: 00000000 ",
	"   Power  Off   ",
	"NETGEAR Storage ",
	"    Welcome     ",
	"Boot from USB   ",
	"Failed!         ",
};

inline void LCD_show_bootmsg(void)
{
	LCD_show(BOOTING, 1);
	LCD_show(SPACE, 2);
}

static void LCD_select_line(MV_32 line)
{
	if (1 == line)
		LCD_send_data(0x80, LCD_INSTRUCTION);
	else if (2 == line)
		LCD_send_data(0xc0, LCD_INSTRUCTION);
}

void LCD_show(MV_U32 bootmenu_item, MV_U32 line)
{
	MV_32 i;

	LCD_select_line(line);
	for (i = 0; i < 16; i++) {
		LCD_send_data(MSG[bootmenu_item][i], LCD_DATA);
	}
}

void LCD_show_failed(void)
{
	LCD_show(FAILED, 2);
}
#else
void LCD_show_failed(void)
{
	return;
}
#endif

void save_user_selection(MV_U32 bootmenu_item_cnt)
{
	MV_U32 bootmenu_item = bootmenu_item_cnt + 1; //Mapping

#ifdef NETGEAR_RN104
	MV_U8 i = 0;
#endif
	switch (bootmenu_item) {
	case NORMAL:
		/* set but not save */
		setenv("boot_reason", "normal");
		break;
	case FACTORY_DEFAULT:
		setenv("boot_reason", "factory");
		break;
	case OS_REINSTALL:
		setenv("boot_reason", "os_reinstall");
		break;
	case TECH_SUPPORT:
		setenv("boot_reason", "diag");
		break;
	case VOL_RO:
		setenv("boot_reason", "vol_ro");
		break;
	case MEMORY_TEST:
#ifdef NETGEAR_RN104
		LCD_select_line(1);
		for (i = 0; i < 16; i++) {
			LCD_send_data(MSG[MEMORY_TEST][i], LCD_DATA);
		}
#endif
		usi_mem_test(0, 0xffffffff);
		break;
	case TEST_DISK:
		setenv("boot_reason", "test_disks");
		break;
	default:
		break;
	}
}
