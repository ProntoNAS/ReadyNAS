#include "usibootup.h"
#include "usi_mem_test.h"
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <nand.h>
#include <asm/byteorder.h>
#include "eth-phy/mvEthPhy.h"

#define ENVVER 3
#define ENVVER_STR "3"

/* SATA detection, Jean Zou */
#define SATA0_PRSNT_BIT                BIT0
#define SATA0_POWER_BIT                BIT24
#define SATA1_PRSNT_BIT                BIT1
#define SATA1_POWER_BIT                BIT25
#define SATA2_PRSNT_BIT                BIT2
#define SATA2_POWER_BIT                BIT26
#define SATA3_PRSNT_BIT                BIT3
#define SATA3_POWER_BIT                BIT28
#define SATA_POWER_GPP_GROUP         0
#define SATA_PRSNT_GPP_GROUP         1
#define mdelay(n) ({unsigned long msec = (n); while (msec--) udelay(1000); })

extern MV_32 usi_use_defaultenv;
extern MV_32 USI_HW_version;

void set_power_off_mode()
{
	unsigned char byte;
	MV_U16 phyval;

	/* 0_0.10 = 1. The leakage is from PHY 88E1318 RGMII TX. */
	mvEthPhyRegWrite(0, 0, 0x500);
	mvEthPhyRegWrite(1, 0, 0x500);

	if (i2c_read(0x68, 0x0b, 1, &byte, 1) != 0)
		puts("Error reading the ISL12057.\n");
	mvEthPhyRegWrite(0, 0x16, 0x3);
	if (byte & BIT1)
	{
		printf("Power off mode2(WOL disabled)\n");
		//Force LED0 on (LED0 out put low)
		mvEthPhyRegRead(0, 0x10, &phyval);
		phyval &= ~0xf;
		phyval |= 0x9;
		mvEthPhyRegWrite(0,0x10,phyval);
	} else {
		printf("Power off mode1(WOL enabled)\n");
		//Force LED0 off (LED0 out put high)
		mvEthPhyRegRead(0, 0x10, &phyval);
		phyval &= ~0xf;
		phyval |= 0x8;
		mvEthPhyRegWrite(0,0x10,phyval);
	}
	mvEthPhyRegWrite(0, 0x16, 0x0);

}

/* Modified this function to return pressed time (in msec)
 * Add max_msec argument
 * Jean 2013/01/23
 */
MV_U32 is_btn_pressed(enum BTN_ID btn_id, MV_U32 max_msec) {

    MV_U32 ret = 0;
    MV_U32 btn2gpp_bitmap = 0;
    MV_U32 gpp_group = 0;
    
    switch (btn_id) {
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
	udelay(10000);//10 ms
	if (MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap) {
		ret = 10;
		//printf("%4d ", ret);
		/* If the usb or reset button was pressed, wait until button up then respond*/
	    while(MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap) {
		if (max_msec != 0 && ret >= max_msec)
			return ret;
		udelay(1000);//1 ms
		ret++;
		//printf("\b\b\b\b\b%4d ", ret);
	    }
	}
    }
    return ret;
}

void menu_show(MV_U32 bootmenu_item_cnt) {
    MV_U32 bootmenu_item = bootmenu_item_cnt + 1;//Mapping

    mvEthPhyRegWrite(1,0x16,0x3);
    mvEthPhyRegWrite(0,0x16,0x3);
    mvEthPhyRegWrite(1,0x10,0x1888);	//PHY1 LED[1] Solid OFF
    mvEthPhyRegWrite(0,0x10,0x1888);	//PHY0 LED[1] Solid OFF
    MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED OFF
    udelay(100);

    switch (bootmenu_item) {
        case BOOTMENU:
		mvEthPhyRegWrite(1,0x10,0x181d);	//PHY1 LED[1] Blink
		mvEthPhyRegWrite(0,0x10,0x181d);	//PHY0 LED[1] Blink
    		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED ON
		MV_REG_BIT_SET(GPP_BLINK_EN_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED Blink
	    puts("Enter Bootmenu...\n");
	    break;
	case NORMAL:
		mvEthPhyRegWrite(1,0x10,0x181c);	//PHY1 LED[1] Solid ON
		mvEthPhyRegWrite(0,0x10,0x1888);	//PHY0 LED[1] Solid OFF
		MV_REG_BIT_RESET(GPP_BLINK_EN_REG(ERROR_LED_GRP), ERROR_LED);	//disable ERROR_LED Blink
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED OFF
	    puts("Bootmenu : NORMAL\n");
	    break;
	case FACTORY_DEFAULT:
		mvEthPhyRegWrite(1,0x10,0x1888);	//PHY1 LED[1] Solid OFF
		mvEthPhyRegWrite(0,0x10,0x181c);	//PHY0 LED[1] Solid ON
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED OFF
	    puts("Bootmenu : FACTORY_DEFAULT\n");
	    break;
	case OS_REINSTALL:
		mvEthPhyRegWrite(1,0x10,0x181c);	//PHY1 LED[1] Solid ON
		mvEthPhyRegWrite(0,0x10,0x181c);	//PHY0 LED[1] Solid ON
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED OFF
	    puts("Bootmenu : OS_REINSTALL\n");
	    break;
	case TECH_SUPPORT:
		mvEthPhyRegWrite(1,0x10,0x1888);	//PHY1 LED[1] Solid OFF
		mvEthPhyRegWrite(0,0x10,0x1888);	//PHY0 LED[1] Solid OFF
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED ON
	    puts("Bootmenu : TECH_SUPPORT\n");
	    break;
	case VOL_RO:
		mvEthPhyRegWrite(1,0x10,0x181c);	//PHY1 LED[1] Solid ON
		mvEthPhyRegWrite(0,0x10,0x1888);	//PHY0 LED[1] Solid OFF
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED ON
	    puts("Bootmenu : VOL_RO\n");
	    break;
	case MEMORY_TEST:
		mvEthPhyRegWrite(1,0x10,0x1888);	//PHY1 LED[1] Solid OFF
		mvEthPhyRegWrite(0,0x10,0x181c);	//PHY0 LED[1] Solid ON
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED ON
	    puts("Bootmenu : MEMORY_TEST\n");
	    break;
	case TEST_DISK:
		mvEthPhyRegWrite(1,0x10,0x181c);	//PHY1 LED[1] Solid ON
		mvEthPhyRegWrite(0,0x10,0x181c);	//PHY0 LED[1] Solid ON
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED ON
	    puts("Bootmenu : TEST_DISK\n");
	    break;
	default:
	    break;
    }
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x0);
    return;
}

void save_user_selection(MV_U32 bootmenu_item_cnt) {
    MV_U32 bootmenu_item = bootmenu_item_cnt + 1;//Mapping

    switch (bootmenu_item) {
	case MEMORY_TEST:
	    usi_mem_test(0, 0xffffffff);
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
	case TEST_DISK:
            setenv("boot_reason", "test_disks");
	    break;
	default:
	    break;
    }
}

void sata_detect(void)
{
	MV_BOOL SATA0_prsnt, SATA1_prsnt,SATA2_prsnt,SATA3_prsnt;
	int ndisks;

	SATA0_prsnt = !(MV_REG_READ(GPP_DATA_IN_REG(SATA_PRSNT_GPP_GROUP)) & SATA0_PRSNT_BIT);
	SATA1_prsnt = !(MV_REG_READ(GPP_DATA_IN_REG(SATA_PRSNT_GPP_GROUP)) & SATA1_PRSNT_BIT);
	SATA2_prsnt = !(MV_REG_READ(GPP_DATA_IN_REG(SATA_PRSNT_GPP_GROUP)) & SATA2_PRSNT_BIT);
	SATA3_prsnt = !(MV_REG_READ(GPP_DATA_IN_REG(SATA_PRSNT_GPP_GROUP)) & SATA3_PRSNT_BIT);
	ndisks = SATA0_prsnt + SATA1_prsnt + SATA2_prsnt + SATA3_prsnt;
	printf("Found %d disks present\n", ndisks);

	if (SATA0_prsnt)
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA_POWER_GPP_GROUP), SATA0_POWER_BIT);
	if (SATA1_prsnt)
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA_POWER_GPP_GROUP), SATA1_POWER_BIT);
	if(ndisks > 2)
	{
		int i;
		printf("Delay 7s then power on another group of HDDs   ");
		for(i = 7; i > 0; i--)
		{
			printf("\b\b\b%2d ", i);
			mdelay(1000);
		}
		printf("\n");
	}
	if (SATA2_prsnt)
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA_POWER_GPP_GROUP), SATA2_POWER_BIT);
	if (SATA3_prsnt)
		MV_REG_BIT_SET(GPP_DATA_OUT_REG(SATA_POWER_GPP_GROUP), SATA3_POWER_BIT);
}


void usiBootup_to_kernel(void)
{
	//MV_U16 bootflag;
	MV_U32 i;
	MV_8 *env;
	MV_U16 phyval;
        MV_U8 ethaddrhex[6];
	MV_U8 phy_num = 2;
        MV_U16 ethaddrphyval[3];
        MV_U16 eth1addrphyval[3];

	/* Get board status information. */
	//mvEthPhyRegWrite(0x0,0x16,0x2);
	//mvEthPhyRegRead(0x0,0x18,&bootflag);
	//mvEthPhyRegWrite(0x0,0x16,0x0);
	//bootflag &= 0x0141;

        env = getenv("ethaddr");
        mvMacStrToHex(env,ethaddrhex);

        ethaddrphyval[0] = ethaddrhex[5];
        ethaddrphyval[0] = (ethaddrphyval[0] << 8) + ethaddrhex[4];
        ethaddrphyval[1] = ethaddrhex[3];
        ethaddrphyval[1] = (ethaddrphyval[1] << 8) + ethaddrhex[2];
	ethaddrphyval[2] = ethaddrhex[1];
	ethaddrphyval[2] = (ethaddrphyval[2] << 8) + ethaddrhex[0];

	env = getenv("eth1addr");
	mvMacStrToHex(env,ethaddrhex);

	eth1addrphyval[0] = ethaddrhex[5];
	eth1addrphyval[0] = (eth1addrphyval[0] << 8) + ethaddrhex[4];
	eth1addrphyval[1] = ethaddrhex[3];
	eth1addrphyval[1] = (eth1addrphyval[1] << 8) + ethaddrhex[2];
	eth1addrphyval[2] = ethaddrhex[1];
	eth1addrphyval[2] = (eth1addrphyval[2] << 8) + ethaddrhex[0];

	for (i = 0; i < phy_num; i++)
	{
		/* To correct the RGMII output impedance */
		mvEthPhyRegWrite(i,0x16,0x2);		//page 2
		mvEthPhyRegRead(i,0x18,&phyval);
		phyval |= 0x0141;			//set bit6=1;bits 2:0=001;bits 10:8=001
		mvEthPhyRegWrite(i,0x18,phyval);
		mvEthPhyRegWrite(i,0x16,0x0);		//back to page 0

		/****** WOL init start ***********/
		mvEthPhyRegWrite(i, 0x16, 0x3);
		//Force led2 off
		mvEthPhyRegRead(i, 0x10, &phyval);
		phyval &= ~(0xf << 8);
		phyval |= (0x8 << 8);
		mvEthPhyRegWrite(i,0x10,phyval);

		//set led2 polarity and Bi-Color LED Mixing(LED1 on and LED0 off)
		mvEthPhyRegWrite(i,0x11,0x8000);

		//bit7 led interrupt and bit11 INTn active low
		//mvEthPhyRegWrite(i,0x12,0x4185);
		//bit7 led interrupt and bit11 INTn active high, Jean 2013/01/14
		mvEthPhyRegWrite(i,0x12,0x4985);

		//WOL interrupt enable
		mvEthPhyRegWrite(i,0x16,0x0);
		mvEthPhyRegWrite(i,0x12,0x80);

		if (i == 0)
		{
			mvEthPhyRegWrite(i,0x16,0x11);
			mvEthPhyRegWrite(i,0x17,ethaddrphyval[0]);
			mvEthPhyRegWrite(i,0x18,ethaddrphyval[1]);
			mvEthPhyRegWrite(i,0x19,ethaddrphyval[2]);
		} else {
			mvEthPhyRegWrite(i,0x16,0x11);
			mvEthPhyRegWrite(i,0x17,eth1addrphyval[0]);
			mvEthPhyRegWrite(i,0x18,eth1addrphyval[1]);
			mvEthPhyRegWrite(i,0x19,eth1addrphyval[2]);
		}

		//enable mac WOL and into low powerstate
		mvEthPhyRegWrite(i,0x16,0x11);
		mvEthPhyRegWrite(i,0x10,0x5500);	//FIXME 0x4500 -> 0x5500
		mvEthPhyRegWrite(i,0x16,0x0);
		/****** WOL init end ***********/
	}

	env = getenv("AC_Power_fail_detect");
	/* Using A2MN(bit0) of ISL12057 for ACPower_fail record, if bit0 is '1', ACPower failed */
	unsigned char byte;
	if (i2c_read(0x68, 0x0b, 1, &byte, 1) != 0)
		puts("Error reading the ISL12057.\n");

	extern MV_U32 VCC_12V_first_plugin_flag;
	if(VCC_12V_first_plugin_flag && (!env || (strcmp(env,"close") == 0) || ((strcmp(env,"open") == 0) && ((byte & BIT0) == 0x0))))
	//if(bootflag != 0x0141)
	{

		i2c_set_bus_num(0);
		/* 
		 * To clear the Alarm 1.
		 */
		if (i2c_read(0x68, 0x0f, 1, &byte, 1) != 0)
			puts("Error reading the ISL12057.\n");
		if (byte & BIT0)
		{
			byte &= (~BIT0);
			if(i2c_write(0x68, 0x0f, 1, &byte, 1) != 0)
				puts("Error writing the ISL12057.\n");
			udelay(1000000);
			byte |= BIT0;
			if(i2c_write(0x68, 0x0f, 1, &byte, 1) != 0)
				puts("Error writing the ISL12057.\n");
		}

		/*disable isl12057 irq1 32768 clock for power saving*/
		/* default value 0x18, set bit2 to 1, to disable fan clock */
		if (i2c_read(0x68, 0x0e, 1, &byte, 1) != 0)
			puts("Error reading the ISL12057.\n");

		byte |= BIT2;
		if(i2c_write(0x68, 0x0e, 1, &byte, 1) != 0)
			puts ("Error writing the isl12057 chip.\n");

		for (i = 0; i < phy_num; i++)
		{
			//power down mode.
			mvEthPhyRegWrite(i,0x9,0x1800);
			mvEthPhyRegWrite(i,0x0,0x100);//10M
		}
		set_power_off_mode();
		printf("Power supply plug on, please press power on button!\n\n");
#if 0	/* EUP chip moved off, Jean 2013/01/15 */
		i2c_set_bus_num(1);
		byte = 0;
                if(i2c_write(0x36, 0x15, 1, &byte, 1) != 0)
                        puts ("Error writing the EUP chip.\n");
#endif
		mdelay(100);
                /*CPU power down*/
                MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~(0x1 << 10));
		while(1);
	}
	else {
		/* enable fan controller clock which is from rtc*/
		/*change value to 0x18 from 0x98*/
		i2c_set_bus_num(0);
		if (i2c_read(0x68, 0x0e, 1, &byte, 1) != 0)
			puts("Error reading the ISL12057.\n");
		byte &= ~BIT2;
		if(i2c_write(0x68, 0x0e, 1, &byte, 1) != 0)
			puts ("Error writing the isl12057 chip.\n");
		/* Turn G76x(FAN controller, i2c address 0x3e 0x48 0x49) on.
		 * The FAN_SET_CNT register's offset is 0x0.
		 * Set [65% == 5a(FAN_SET_CNT)] as default.
		 * */

		byte=0x5a;
		if(i2c_write(0x3e, 0x0, 1, &byte, 1) != 0)
			puts ("Error writing the i2c chip : G76x(Fan controller) addr:0x3e.\n");
		if(i2c_write(0x48, 0x0, 1, &byte, 1) != 0)
			puts ("Error writing the i2c chip : G76x(Fan controller) addr:0x48.\n");
		if(i2c_write(0x49, 0x0, 1, &byte, 1) != 0)
			puts ("Error writing the i2c chip : G76x(Fan controller) addr:0x49.\n");


		for (i = 0; i < phy_num; i++)
		{
			mvEthPhyRegWrite(i,0x16,0x0);
			mvEthPhyRegWrite(i,0x9,0x300);	//back to 1G
			mvEthPhyRegWrite(i,0x0,0x9140);	//auto-negotiation and reset
			/*disable WOL in kernel*/
			mvEthPhyRegWrite(i,0x16,0x11);
			mvEthPhyRegWrite(i,0x10,0x1500);
			mvEthPhyRegWrite(i,0x16,0x0);
		}
		printf("Power On!\n\n");
	}

	setenv("HW_version", (USI_HW_version == 0) ? "MVT" : "DVT");

	env = getenv("envver");
	if (!env || usi_use_defaultenv || simple_strtoul(env, NULL, 0) < ENVVER) {
		printf("Updating env to v" ENVVER_STR "...\n");
		setenv("mtdparts", "mtdparts=armada-nand:0x180000@0(u-boot),0x20000@0x180000(u-boot-env),0x600000@0x200000(uImage),0x400000@0x800000(minirootfs),-(ubifs)");
		setenv("bootcmd_ubi", "ubi part ubifs; ubifsmount rootfs; ubifsload 0x2000000 kernel; ubifsload 0x3000000 initrd.gz; bootm 0x2000000 0x3000000 0x1000000");
		setenv("bootcmd", "nand read 0x2000000 0x200000 0x400000; nand read 0x3000000 0x800000 0x400000; bootm 0x2000000 0x3000000 0x1000000");
		setenv("bootargs", "console=ttyS0,115200 pm_disable=yes mv_cpu_count=2");
		setenv("bootdelay", "0");
		setenv("envver", ENVVER_STR);
		run_command("saveenv", 0);
        }
	if (nand_load_fdt(0xEC000, 0x6000, (u_char *)0x1000000) == 0)
		printf("FDT loaded successfully\n");
	else
		printf("Loading FDT from NAND 0xEC000 to 0x1000000 failed!\n");

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
	    MV_U32 pressed_time;

	    setenv("Startup", "Manual");
	    run_command("saveenv", 0);

	    menu_show(bootmenu_item_cnt);
	    while (1) {
		pressed_time = is_btn_pressed(RESET, 5000);
	        if (pressed_time >= 4900) {
			setenv("bootcmd","usb start;fatload usb 0:1 0x1200000 /uImage-recovery;fatload usb 0:1 0x2000000 /initrd-recovery.gz;bootm 1200000 2000000");
			goto HDD_power_on;
		}
		else
		{
			bootmenu_item_cnt = (bootmenu_item_cnt + 1) % bootmenu_item_len;
			menu_show(bootmenu_item_cnt);
			break;
		}
	    }

	    while (1) {
		pressed_time = is_btn_pressed(RESET, 3000);
	        if (pressed_time > 0 && pressed_time < 2900) {
		    bootmenu_item_cnt = (bootmenu_item_cnt + 1) % bootmenu_item_len;
		    menu_show(bootmenu_item_cnt);
		}
		else if (pressed_time >= 2900)
		{
		    save_user_selection(bootmenu_item_cnt);
		    break;
		}
	    }
	} else {
	    env = getenv("Startup");
	    if ((env == NULL) || (strcmp(env, "Normal") != 0))
	    {
		setenv ("Startup", "Normal");
		run_command("saveenv", 0);
	    }
	}

HDD_power_on:
	mvEthPhyRegWrite(1,0x16,0x3);
	mvEthPhyRegWrite(1,0x10,0x181f);	//PHY1 LED[1] Solid ON
	mvEthPhyRegWrite(1,0x16,0x0);
	mvEthPhyRegWrite(0,0x16,0x3);
	mvEthPhyRegWrite(0,0x10,0x181f);	//PHY0 LED[1] Solid ON
	mvEthPhyRegWrite(0,0x16,0x0);
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(ERROR_LED_GRP), ERROR_LED);	//ERROR_LED OFF

	sata_detect();
}
