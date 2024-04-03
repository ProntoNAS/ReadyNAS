#include "usibootup.h"
#include "usi_mem_test.h"	//Jean 2011-05-12
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <asm/byteorder.h>
#include "eth-phy/mvEthPhy.h"

extern int usi_use_defaultenv;
int recovery_status = 0;

#ifdef USI_BOARD_NVPLUS_V4	//libo 2011-06-07
typedef struct {
    MV_U32 reg;
    MV_U32 val;
} USI_SATAHC_REG_T;

USI_SATAHC_REG_T USI_SATAHC_port0_settings[] = {
/* These settings below will affect the signal quality of 4140 port0*/
{0x00082344, 0xf}, {0x00082104, 0x0}, {0x00082104, 0x8c}, {0x00082108, 0x0}, {0x00082108, 0x0},
{0x0008210c, 0x0}, {0x0008210c, 0x0}, {0x00082110, 0x0}, {0x00082110, 0x0}, {0x00082114, 0x0},
{0x00082114, 0x0}, {0x00082118, 0xf}, {0x0008211c, 0xe8}, {0x00082104, 0x0}, {0x00082104, 0x91},
{0x00082108, 0x0}, {0x00082108, 0xAF}, {0x0008210c, 0x0}, {0x0008210c, 0x90}, {0x00082110, 0x0},
{0x00082110, 0x91}, {0x00082114, 0x0}, {0x00082114, 0x24}, {0x00082118, 0xf}, {0x0008211c, 0xe8},
/* These settings below will affect the signal quality of 4140 port1*/
{0x00082344, 0xf}, {0x00082104, 0x0}, {0x00082104, 0x8c}, {0x00082108, 0x0}, {0x00082108, 0x1},
{0x0008210c, 0x0}, {0x0008210c, 0x0}, {0x00082110, 0x0}, {0x00082110, 0x0}, {0x00082114, 0x0},
{0x00082114, 0x0}, {0x00082118, 0xf}, {0x0008211c, 0xe8}, {0x00082104, 0x0}, {0x00082104, 0x91},
{0x00082108, 0x0}, {0x00082108, 0xAF}, {0x0008210c, 0x0}, {0x0008210c, 0x90}, {0x00082110, 0x0},
{0x00082110, 0x91}, {0x00082114, 0x0}, {0x00082114, 0x24}, {0x00082118, 0xf}, {0x0008211c, 0xe8},
/* These settings below will affect the signal quality of 4140 port2*/
{0x00082344, 0xf}, {0x00082104, 0x0}, {0x00082104, 0x8c}, {0x00082108, 0x0}, {0x00082108, 0x2},
{0x0008210c, 0x0}, {0x0008210c, 0x0}, {0x00082110, 0x0}, {0x00082110, 0x0}, {0x00082114, 0x0},
{0x00082114, 0x0}, {0x00082118, 0xf}, {0x0008211c, 0xe8}, {0x00082104, 0x0}, {0x00082104, 0x91},
{0x00082108, 0x0}, {0x00082108, 0xAF}, {0x0008210c, 0x0}, {0x0008210c, 0x90}, {0x00082110, 0x0},
{0x00082110, 0x91}, {0x00082114, 0x0}, {0x00082114, 0x24}, {0x00082118, 0xf}, {0x0008211c, 0xe8},
/* These settings below will affect the signal quality of 4140 port3*/
{0x00082344, 0xf}, {0x00082104, 0x0}, {0x00082104, 0x8c}, {0x00082108, 0x0}, {0x00082108, 0x3},
{0x0008210c, 0x0}, {0x0008210c, 0x0}, {0x00082110, 0x0}, {0x00082110, 0x0}, {0x00082114, 0x0},
{0x00082114, 0x0}, {0x00082118, 0xf}, {0x0008211c, 0xe8}, {0x00082104, 0x0}, {0x00082104, 0x91},
{0x00082108, 0x0}, {0x00082108, 0xAF}, {0x0008210c, 0x0}, {0x0008210c, 0x90}, {0x00082110, 0x0},
{0x00082110, 0x91}, {0x00082114, 0x0}, {0x00082114, 0x24}, {0x00082118, 0xf}, {0x0008211c, 0xe8},
};
#endif

void usi_set_bootup_fail_led(void)
{
        MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~(0x1 << 6));
        MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | (0x1 << 6));
        MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) | (0x1 << 5));
        MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~(0x1 << 5));
}
void usi_set_burn_nand_led(void)
{
        MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~(0x1 << 5));
        MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | (0x1 << 5));
        MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~(0x1 << 6));
        MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) & ~(0x1 << 6));
}
void usi_set_burn_nand_led_success(void)
{
        MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~(0x1 << 5));
        MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | (0x1 << 5));
        MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) & ~(0x1 << 6));
        MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | (0x1 << 6));
}
void boot_to_usb_drive(MV_VOID)
{
	int delaytime=0;

	if (MV_REG_READ(GPP_DATA_IN_REG(1))& BIT13) {
		/*sys led light on*/
#ifdef USI_BOARD_NVPLUS_V4
		MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT22);
#else
		MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~BIT29);
#endif
		while(1)
		{
			if ((MV_REG_READ(GPP_DATA_IN_REG(1))& BIT13) == 0)
			{
				break;
			}
			udelay(100000);	//every 100ms
			delaytime++;
			if (delaytime >= 10)
				break;
		}
		/*sys led light off*/
#ifdef USI_BOARD_NVPLUS_V4
		MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT22);
#else
		MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT29);
#endif
	}
	/*press button > 5s to usb recovery*/
	if( delaytime >= 10 )
	{
		recovery_status = 1;
		setenv("bootcmd","usb start;fatload usb 0:1 0x1200000 /uImage-recovery;fatload usb 0:1 0x2000000 /initrd-recovery.gz;bootm 1200000 2000000");
#ifdef USI_BOARD_NVPLUS_V4
		MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT22);
#else
		MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT29);
#endif
	}

	/*sys led blink*/
	//MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | BIT22);
}

#ifdef USI_BOARD_NVPLUS_V4

int LCD_interface_old = 0;

static void W83601G_init(void)
{
	unsigned char byte=0;

	byte = 0x40;
	if(i2c_write(0, 0x1f, 0x14, 1, &byte, 1) != 0) 
	    puts ("Error writing the chip.\n");

	byte = 0x0;
	if(i2c_write(0, 0x1f, 0x3, 1, &byte, 1) != 0) 
	    puts ("Error writing the chip.\n");

	byte = 0x0;
	if(i2c_write(0, 0x1f, 0x2, 1, &byte, 1) != 0) 
	    puts ("Error writing the chip.\n");

	byte = 0x0;
	if(i2c_write(0, 0x1f, 0x1, 1, &byte, 1) != 0) 
	    puts ("Error writing the chip.\n");
}

static char LCD_init_tbl_old[] = {
0x00,0x38,0x30, 0x00,0x08,0x00, 0x00,0x38,0x30, 0x00,0x08,0x00, 0x00,0x38,0x30, 0x00,0x08,0x00,
0x00,0x28,0x20, 0x00,0x08,0x00, 0x00,0x28,0x20, 0x00,0x88,0x80,
0x00,0x08,0x00, 0x00,0x88,0x80,
0x00,0x08,0x00, 0x00,0x18,0x10, 0x00,0x08,0x00, 0x00,0x78,0x70,
0x00,0xA8,0xA0, 0x00,0x08,0x00,
0x0a,0x02,
};

MV_VOID LCD_write_data_old(MV_U8 data)
{
	MV_U8 tmp_data;

	tmp_data = 0x02;
	if(i2c_write(0, 0x1f, 0x1, 1, &tmp_data, 1) != 0) 
	    puts ("Error writing the chip.\n");
	udelay(1);

	tmp_data = 0x0a;
	if(i2c_write(0, 0x1f, 0x1, 1, &tmp_data, 1) != 0) 
	    puts ("Error writing the chip.\n");
	udelay(1);
	
	tmp_data = data | 0xa;
	if(i2c_write(0, 0x1f, 0x1, 1, &tmp_data, 1) != 0) 
	    puts ("Error writing the chip.\n");
	udelay(1);
	
	tmp_data = data | 0x2;
	if(i2c_write(0, 0x1f, 0x1, 1, &tmp_data, 1) != 0) 
	    puts ("Error writing the chip.\n");

	return;
}

static void LCD_init_old(void)
{
	int i;

	for(i = 0; i < sizeof(LCD_init_tbl_old)/sizeof(char); i++)
	{
	    if(i2c_write(0, 0x1f, 0x1, 1, &LCD_init_tbl_old[i], 1) != 0) 
	        puts ("Error writing the chip.\n");
	    udelay(1);
	}
}

static char LCD_line_cfg_old[2][6] = {
    {0x08, 0x88, 0x80, 0x08, 0x08, 0x00,},
    {0x08, 0xC8, 0xC0, 0x08, 0x08, 0x00,},
};

static void LCD_select_line_old(int line)
{
    int i;

    if(1 == line || 2 == line)
    {
	for(i = 0; i < sizeof(LCD_line_cfg_old[line-1])/sizeof(char); i++)
	{
	    if(i2c_write(0, 0x1f, 0x1, 1, &LCD_line_cfg_old[line-1][i], 1) != 0)
		puts ("Error writing the chip.\n");
	    udelay(1);
	}

    }
}

void LCD_show_old(MV_U32 bootmenu_item)
{
    int i;
    MV_U8 tmp_data;

    if (bootmenu_item == BOOTMENU || bootmenu_item == SPACE)
    {
	LCD_select_line_old(1);
    } else
    {
	LCD_select_line_old(2);
    }

    for(i = 0; i < 16; i++)
    {
	LCD_write_data_old(MSG[bootmenu_item][i] & 0xF0);
	udelay(1);
	LCD_write_data_old((MSG[bootmenu_item][i] & 0x0F) << 4);
	udelay(1);
    }
}

static char LCD_init_tbl[] = {
0x30, 0x30, 0x30,
0x20, 0x28,
0x08,
0x01, 0x07,
0xA0,
};

#define LCD_udelay		udelay
#define LCD_BL_GPP_GROUP	0
#define LCD_BL_BIT		BIT12
#define LCD_BL_BIT_SHIFT	12

#define LCD_RS_GPP_GROUP        0
#define LCD_RS_BIT              BIT14
#define LCD_RS_BIT_SHIFT        14

#define LCD_RW_GPP_GROUP        0
#define LCD_RW_BIT              BIT15
#define LCD_RW_BIT_SHIFT        15

#define LCD_E_GPP_GROUP		0
#define LCD_E_BIT		BIT16
#define LCD_E_BIT_SHIFT		16

#define LCD_INSTRUCTION		0
#define LCD_DATA		1
#define LCD_WR			0
#define LCD_RD			1
static void LCD_set_BL(MV_U32 on_off)
{
    /* BL is low active on Netgear NV+ */
    MV_U32 val = on_off ^ 1;

    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP));
    gppData &= ~LCD_BL_BIT;
    gppData |= ((val << LCD_BL_BIT_SHIFT) & LCD_BL_BIT);
    MV_REG_WRITE(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP), gppData);
}

static MV_U32 LCD_get_BL_status(void)
{
    MV_U32 gppData;
    gppData = MV_REG_READ(GPP_DATA_OUT_REG(LCD_BL_GPP_GROUP));
    gppData &= LCD_BL_BIT;
    gppData >>= LCD_BL_BIT_SHIFT;
    return (gppData ^ 1);
}

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

#define LCD_DB4			BIT17
#define LCD_DB4_GPP_GROUP	0
#define LCD_DB4_SHIFT		17

#define LCD_DB5			BIT1
#define LCD_DB5_GPP_GROUP	1
#define LCD_DB5_SHIFT		1

#define LCD_DB6			BIT3
#define LCD_DB6_GPP_GROUP	1
#define LCD_DB6_SHIFT		3

#define LCD_DB7			BIT17
#define LCD_DB7_GPP_GROUP	1
#define LCD_DB7_SHIFT		17
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
    /* Jean 2011-7-22 */
    if (LCD_interface_old)
    {
	LCD_write_data_old(val & 0xF0);
	udelay(1);
	LCD_write_data_old((val & 0x0F) << 4);
	udelay(1);
        return;
    }

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
	int i;

	/* Jean 2011-7-22 */
	if (LCD_interface_old)
	{
		W83601G_init();
		LCD_init_old();
		return;
	}

	for(i = 0; i < sizeof(LCD_init_tbl)/sizeof(char); i++)
	{
	    LCD_udelay(5000);
	    LCD_send_data(LCD_init_tbl[i], LCD_INSTRUCTION);
	}
//FIXME 
	LCD_set_E(1);
	LCD_set_RS(LCD_DATA);
        LCD_udelay(10);

	LCD_set_E(0);
}
#endif

static void LCD_select_line(int line);
MV_U32 usiBootup_to_kernel(MV_VOID)
{

	MV_U32 i = 0;
#ifdef USI_BOARD_NVPLUS_V4
	/* Jean 2011-7-22 */
	if (i2c_probe(0, 0x1f) == 0)
	{
		LCD_interface_old = 1;
	}

	LCD_init();
	LCD_select_line(1);
        for(i = 0; i < 16; i++)
	    LCD_send_data(MSG[SPACE][i], LCD_DATA);
	LCD_select_line(2);
        for(i = 0; i < 16; i++)
	    LCD_send_data(MSG[SPACE][i], LCD_DATA);
#endif
	MV_U16 bootflag;	//by led2 polar
	mvEthPhyRegWrite(0x0,0x16,0x3); //page3
	mvEthPhyRegRead(0x0,0x11,&bootflag);	//led2 polity
	mvEthPhyRegWrite(0x0,0x16,0x0); //back to page0
	bootflag &= 0x1<<4;
      
	/******WOL init start ***********/
	char *env;
      	MV_U8 ethaddrhex[6];
	env = getenv("ethaddr");
	mvMacStrToHex(env,ethaddrhex);

	MV_U16 ethaddrphyval[3];
	ethaddrphyval[0] = ethaddrhex[5];
	ethaddrphyval[0] = (ethaddrphyval[0] << 8) + ethaddrhex[4];
	ethaddrphyval[1] = ethaddrhex[3];
	ethaddrphyval[1] = (ethaddrphyval[1] << 8) + ethaddrhex[2];
	ethaddrphyval[2] = ethaddrhex[1];
	ethaddrphyval[2] = (ethaddrphyval[2] << 8) + ethaddrhex[0];

	MV_U16 phyval;
	mvEthPhyRegWrite(0x0,0x16,0x3);	//page3
	//for led2 off
       	mvEthPhyRegRead(0x0,0x10,&phyval);
	phyval &= ~(0xf << 8);
	phyval |= (0x8 << 8);
	mvEthPhyRegWrite(0x0,0x10,phyval);

	//0x4905 bit7 led2 interupt
	mvEthPhyRegWrite(0x0,0x12,0x4985);	//bit7 led interrupt
	//intrupt wol enable
	mvEthPhyRegWrite(0x0,0x16,0x0);		//page 0
	mvEthPhyRegWrite(0x0,0x12,0x80);
	//set phyaddress
	mvEthPhyRegWrite(0x0,0x16,0x11);	//page 17
	mvEthPhyRegWrite(0x0,0x17,ethaddrphyval[0]);
	mvEthPhyRegWrite(0x0,0x18,ethaddrphyval[1]);
	mvEthPhyRegWrite(0x0,0x19,ethaddrphyval[2]);

	mvEthPhyRegWrite(0x0,0x10,0x4500);	//enable mac WOL and into low powerstate

	MV_U16 tmpval;
	/* To correct the RGMII output impedance */
	mvEthPhyRegWrite(0x0,0x16,0x2);		//page2
	mvEthPhyRegRead(0x0,0x18,&tmpval);
	tmpval |= 0x0141;			//set bit6=1;bits 2:0=001;bits 10:8=001
	mvEthPhyRegWrite(0x0,0x18,tmpval);
	mvEthPhyRegWrite(0x0,0x16,0x0);		//back to page0
	
#if 1
	/*pull led2 level down*/
	mvEthPhyRegWrite(0x0,0x16,0x3);		//page3
	//mvEthPhyRegWrite(0x0,0x11,0x4410);	//led2 polity
	mvEthPhyRegWrite(0x0,0x11,0x8010);	//libo 2011-03-08 set led2 polity and Bi-Color LED Mixing(LED1 on and LED0 off)
	mvEthPhyRegWrite(0x0,0x16,0x0);		//back to page0
#endif 
        /*do disable interupt of WOL*/
#ifdef USI_BOARD_NVPLUS_V4
#else
        MV_REG_WRITE(GPP_DATA_OUT_EN_REG(1),MV_REG_READ(GPP_DATA_OUT_EN_REG(1)) & ~(0x1 << 3)); //set GPIO35 datein
#endif
	/*resume eth*/
	/*FIXME if clear interrupt by phyWrite 0x0 0x10 0x5500;*/ 
	mvEthPhyRegWrite(0x0,0x16,0x11);	//page3
	mvEthPhyRegWrite(0x0,0x10,0x5500);	//have no effect?
	/*FIXME neccesery????*/
	mvEthPhyRegWrite(0x0,0x16,0x0);

	env = getenv("AC_Power_fail_detect");

	/* Using SL2(BIT5) of RS5C372a control 1 register for ACPower_fail record */
	MV_U8 value = 0;
	if (i2c_read(0, 0x32, 0xe0, 1, &value, 1) != 0)
		puts("Error reading the RS5C372a.\n");
	
	/*justify plug on or switch on*/
	if( (bootflag == 0) && (!env || (strcmp(env,"close") == 0) || ((strcmp(env,"open") == 0) && ((value & BIT5) == 0))))	//power plug in firstly
	{

		/*force 1000M to 100M and wait WOL for power consideration */
#if 1
		mvEthPhyRegWrite(0x0,0x9,0x1800);
#ifdef USI_BOARD_NVPLUS_V4
		mvEthPhyRegWrite(0x0,0x0,0x140);
		mvEthPhyRegWrite(0x0,0x0,0x3100);	//100M
#else
		mvEthPhyRegWrite(0x0,0x0,0x100);
		//mvEthPhyRegWrite(0x0,0x0,0x1100);	//10M
#endif
#else
		//power down mode.
		mvEthPhyRegRead(0x0,0x0,&tmpval);
		tmpval |= BIT11;	//Control register power down.
		mvEthPhyRegWrite(0x0,0x0,tmpval);

		//mvEthPhyRegRead(0x0,0x10,&value);
		//value |= BIT2;	//Copper specific control register power down.
		//mvEthPhyRegWrite(0x0,0x10,value);
#endif
		/* 
		 * 0xe0 is the Control register 1. BIT7 is Alarm A enable bit.
		 * The pin INTA of RS5C372a will be set to '0' and hold when the Alarm A is raising.
		 *
		 * Reset the Alarm A.
		 */
		if (value & BIT7)
		{
			value &= (~BIT7);
			if(i2c_write(0, 0x32, 0xe0, 1, &value, 1) != 0)
				puts("Error writing the RS5C372a.\n");
			udelay(1000000);
			value |= BIT7;
			if(i2c_write(0, 0x32, 0xe0, 1, &value, 1) != 0)
				puts("Error writing the RS5C372a.\n");
		}

		printf("Plug On and Power down, Please Switch On !\n\n");

		/*set i2c chip to 0s delay from 6s delay*/
		unsigned char byte=0;
		if(i2c_write(0, 0x36, 0x15, 1, &byte, 1) != 0) 
			puts ("Error writing the chip.\n");
		/*CPU power down*/
		MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~(0x1 << 30));
		//MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & ~(0x1 << 24));
		udelay(1000);
       	}
	else
	{
		/*for 100M to 1000M back*/
		mvEthPhyRegWrite(0x0,0x9,0x300);	//back to 1000M	
		mvEthPhyRegWrite(0x0,0x0,0x9140);	//auto-negotiation and reset	

		printf("Switch On !\n\n");

		unsigned char byte=6;

		if(i2c_write(0, 0x36, 0x15, 1, &byte, 1) != 0) 
			puts ("Error writing the chip.\n");
		/*disable WOL in kernel*/
		mvEthPhyRegWrite(0x0,0x16,0x11);        //page17
		mvEthPhyRegWrite(0x0,0x10,0x1500);      //have no effect?
		/*FIXME neccesery????*/
		mvEthPhyRegWrite(0x0,0x16,0x0);


	}

	/*modify 1318 PHY VOD value for singal which is neccesery*/
	mvEthPhyRegWrite(0,22,0xff);
	mvEthPhyRegWrite(0,24,0xeeee);
	mvEthPhyRegWrite(0,23,0x2012);
	mvEthPhyRegWrite(0,22,0x0);
#if 1 //libo 2011-05-17
	/* Turn G76x(FAN controller, i2c address 0x3e) on.
	 * The FAN_SET_CNT register's offset is 0x0.
	 * Set [1300(rpm) == 65% == 5a(FAN_SET_CNT)] as default.
	 */
	unsigned char byte=0x5a;
	if(i2c_write(0, 0x3e, 0x0, 1, &byte, 1) != 0) 
		puts ("Error writing the i2c chip : G76x(Fan controller).\n");
#endif
#if defined(CFG_ENV_IS_IN_NAND)
	/* usi_use_defaultenv == 1 means there is no env in nand flash, so we need to
	 * save it using default value
	 */
	if(usi_use_defaultenv)
	    run_command("saveenv", 0);
#endif
#ifdef USI_BOARD_NVPLUS_V4	//libo 2011-06-07
	/*
	 * The code below aims to improve the SATA signal quality.
	 */
	ide_init ();
	udelay(1000000);
        for (i = 0; i < (sizeof(USI_SATAHC_port0_settings) / sizeof(USI_SATAHC_REG_T)); i++)
        {
                MV_REG_WRITE(USI_SATAHC_port0_settings[i].reg, 
				USI_SATAHC_port0_settings[i].val);
		udelay(5000);
        }
#endif

#if 1 //libo 2011-04-22
	boot_to_usb_drive();
	if (recovery_status)
	    goto HDD_power_on;
#endif

	/*power on usb port for auto loading firware from usb disk*/
	MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | (0x1 << 3));

	/*sys led blink*/
	MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) | (0x1 << 6));
	/*sys_fail led blink*/
	MV_REG_WRITE(GPP_BLINK_EN_REG(1),MV_REG_READ(GPP_BLINK_EN_REG(1)) | BIT5 );

#if 1
	/* Assert the CPIO status is settled down correctly. */
	/* MPP13 mapping reset btn */
	/* MPP45 mapping backup btn */
	MV_U32 bootmenu_wait_window_cnt = 30;
	MV_U32 holdon_cnt = 0;

	MV_U32 is_bootmenu_flag = 0;
	MV_U32 bootmenu_item_len = 7;
	MV_32 bootmenu_item_cnt = -1;

	while(bootmenu_wait_window_cnt) {
	    //printf("In bootmenu...\n");
	    holdon_cnt = 0;
	    if (MV_REG_READ(GPP_DATA_IN_REG(0)) & BIT13) {//reset button was pressed.
	        for(i = 0; i < HOLDON_LEN; i++) {
		    udelay(100000);
		    //bootmenu_wait_window_cnt--;
		    if (MV_REG_READ(GPP_DATA_IN_REG(0)) & BIT13)
		        holdon_cnt++;
		    else
		        break;
		}
		if (holdon_cnt == HOLDON_LEN) {
		    //MV_REG_WRITE(GPP_BLINK_EN_REG(0),MV_REG_READ(GPP_BLINK_EN_REG(0)) | (0x01 << 27));
		    is_bootmenu_flag = 1;
		    break;
		}
	    }
	    udelay(100000);
	    bootmenu_wait_window_cnt--;
	}
	if (is_bootmenu_flag) {
	    menu_show(bootmenu_item_cnt);
	    while (1) {
	        if (is_btn_pressed(USB)) {
		    bootmenu_item_cnt = (bootmenu_item_cnt + 1) % bootmenu_item_len;
		    //save_user_selection(bootmenu_item_cnt);
		    menu_show(bootmenu_item_cnt);
		    break;
		}
	    }

	    while (1) {
		if (is_btn_pressed(USB)) {
		    bootmenu_item_cnt = (bootmenu_item_cnt + 1) % bootmenu_item_len;
		    //save_user_selection(bootmenu_item_cnt);
		    menu_show(bootmenu_item_cnt);
		}
		if (is_btn_pressed(RESET)) {
		    save_user_selection(bootmenu_item_cnt);
		    break;
		}
	    }
	}
#endif

#ifdef USI_BOARD_NVPLUS_V4
	//menu_show(0);//set the LED's status to normal.
	LCD_show(SPACE);
	LCD_show(BOOTING);
#else
        MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT22 | BIT23 | BIT29 | BIT31);
        udelay(100);
        MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) & (~BIT31));
#endif
	MV_8 * cmdline;
	MV_8 new_cmdline[512];
	memset(new_cmdline, 0, sizeof(new_cmdline) / sizeof(MV_8));

	cmdline = getenv("bootargs");
	if (!cmdline)
	{
		cmdline = "console=ttyS0,115200 reason=normal";
		setenv("bootargs", cmdline);
	}
	else if (!strstr(cmdline, "reason="))
	{
		sprintf(new_cmdline, "%s reason=%s", cmdline, "normal");
		setenv("bootargs", new_cmdline);
	}

#if 1 //libo 2011-03-28
HDD_power_on:
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT26);
	MV_REG_WRITE(GPP_DATA_OUT_REG(0),MV_REG_READ(GPP_DATA_OUT_REG(0)) | BIT28);
#ifdef USI_BOARD_NVPLUS_V4
	udelay(7000000);	//delay 7s then power on another group of HDDs
	MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | BIT8);
	MV_REG_WRITE(GPP_DATA_OUT_REG(1),MV_REG_READ(GPP_DATA_OUT_REG(1)) | BIT10);
#endif
#endif
}

#if 1 //libo 2011-02-17
MV_U32 is_btn_pressed(enum BTN_ID btn_id) {

    MV_U32 ret = 0;
    MV_U32 btn2gpp_bitmap = 0;
    MV_U32 gpp_group = 0;
    
    switch (btn_id) {
        case USB:
	    gpp_group = 1;
	    btn2gpp_bitmap = BIT13;//MPP45
	    break;
	case RESET:
	    gpp_group = 0;
	    btn2gpp_bitmap = BIT13;//MPP13
	    break;
	case POWER:
	    gpp_group = 1;
	    btn2gpp_bitmap = BIT15;//MPP47
	    break;
	default:
	    return 0;
    }
    /* GPP attributes must have been set before */
    if (MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap) {
	/* eliminate jitter */
	udelay(10000);//10 ms
	if (MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap) {
	    if (btn_id == USB || btn_id == RESET)
	    {
		/* If the usb or reset button was pressed, wait until button up then respond*/
	        while(MV_REG_READ(GPP_DATA_IN_REG(gpp_group)) & btn2gpp_bitmap)
	        ;
	    }
	    ret = 1;
	}
    }
    return ret;
}

void menu_show(MV_U32 bootmenu_item_cnt) {
    MV_U32 bootmenu_item = bootmenu_item_cnt + 1;//Mapping
#ifdef USI_BOARD_NVPLUS_V4
    MV_U32 bits = BIT20 | BIT23 | BIT22 | BIT24 | BIT29 | BIT31;
#else
    MV_U32 GPP_group = 0;
    MV_U32 bits = BIT22 | BIT23 | BIT29 | BIT31;
    MV_REG_WRITE(GPP_DATA_OUT_REG(GPP_group),MV_REG_READ(GPP_DATA_OUT_REG(GPP_group)) | bits);
#endif
    udelay(100);

    switch (bootmenu_item) {
        case BOOTMENU:
#ifdef USI_BOARD_NVPLUS_V4
	    bits = BIT20 | BIT23 | BIT22 | BIT24 | BIT29 | BIT31;
#else
	    bits = BIT22 | BIT23 | BIT29 | BIT31;
#endif
	    puts("Enter Bootmenu...\n");
	    break;
	case NORMAL:
	    bits = BIT31;
	    puts("Bootmenu : NORMAL\n");
	    break;
	case FACTORY_DEFAULT:
	    bits = BIT23;
	    puts("Bootmenu : FACTORY_DEFAULT\n");
	    break;
	case OS_REINSTALL:
#ifdef USI_BOARD_NVPLUS_V4
	    bits = BIT20;
#else
	    bits = BIT22;
#endif
	    puts("Bootmenu : OS_REINSTALL\n");
	    break;
	case TECH_SUPPORT:
#ifdef USI_BOARD_NVPLUS_V4
	    bits = BIT22;
#else
	    bits = BIT29;
#endif
	    puts("Bootmenu : TECH_SUPPORT\n");
	    break;
	case SKIP_VOL_CHECK:
	    bits = BIT23 | BIT31;
	    puts("Bootmenu : SKIP_VOL_CHECK\n");
	    break;
	case MEMORY_TEST:
#ifdef USI_BOARD_NVPLUS_V4
	    bits = BIT20 | BIT31;
#else
	    bits = BIT22 | BIT31;
#endif
	    puts("Bootmenu : MEMORY_TEST\n");
	    break;
	case TEST_DISK:
#ifdef USI_BOARD_NVPLUS_V4
	    bits = BIT22 | BIT31;
#else
	    bits = BIT29 | BIT31;
#endif
	    puts("Bootmenu : TEST_DISK\n");
	    break;
	default:
	    bits = 0;
	    break;
    }
    /* GPP attributes must have been set before */
#ifdef USI_BOARD_NVPLUS_V4
    LCD_show(bootmenu_item);
#else
    MV_REG_WRITE(GPP_DATA_OUT_REG(GPP_group),MV_REG_READ(GPP_DATA_OUT_REG(GPP_group)) & ~bits);
    udelay(100);
#endif
    return;
}

#ifdef USI_BOARD_NVPLUS_V4
MV_U8 MSG[][16] = {
    "   Boot  Menu   ",
    "     Normal     ",
    "Factory  Default",
    "  OS Reinstall  ",
    "  Tech Support  ",
    " Skip Vol Check ",
    "  Memory  Test  ",
    "   Test Disks   ",
    "                ",
    "   Booting...   ",
    /* Jean, 2011-06-01 */
    "    00:00:00    ",
    "*** SUCCESS! ***",
    " FAIL: 00000000 ",
    "   Power  Off   ",

};

static void LCD_select_line(int line)
{
    /* Jean 2011-7-22 */
    if (LCD_interface_old)
    {
	    return LCD_select_line_old(line);
    }

    if (1 == line)
	LCD_send_data(0x80, LCD_INSTRUCTION);
    else if (2 == line)
	LCD_send_data(0xc0, LCD_INSTRUCTION);
}

void LCD_show(MV_U32 bootmenu_item)
{
    int i;

    /* Jean 2011-7-22 */
    if (LCD_interface_old)
    {
	    return LCD_show_old(bootmenu_item);
    }

    if (bootmenu_item == BOOTMENU || bootmenu_item == SPACE)
    {
	LCD_select_line(1);
    } else
    {
	LCD_select_line(2);
    }

    for(i = 0; i < 16; i++)
    {
        LCD_send_data(MSG[bootmenu_item][i], LCD_DATA);
    }
}


#endif

void save_user_selection(MV_U32 bootmenu_item_cnt) {
    MV_U32 bootmenu_item = bootmenu_item_cnt + 1;//Mapping
    MV_U8 i = 0;
    MV_8 * cmdline;
    MV_8 new_cmdline[512];
    memset(new_cmdline, 0, sizeof(new_cmdline) / sizeof(MV_8));
    cmdline = getenv("bootargs");
    /* If the uboot use the default env, the cmdline must be NULL */
    if (!cmdline) {
    	//FIXME
	cmdline = "console=ttyS0,115200";
    }
    else if (strstr(cmdline, "reason=")) {
	cmdline = "console=ttyS0,115200";
    }

    switch (bootmenu_item) {
	case NORMAL:
	    sprintf(new_cmdline, "%s reason=%s", cmdline, "normal");
	    /* set but not save */
            setenv("bootargs", new_cmdline);
	    break;
	case FACTORY_DEFAULT:
	    sprintf(new_cmdline, "%s reason=%s", cmdline, "factory");
            setenv("bootargs", new_cmdline);
	    break;
	case OS_REINSTALL:
	    sprintf(new_cmdline, "%s reason=%s", cmdline, "os_reinstall");
            setenv("bootargs", new_cmdline);
	    break;
	case TECH_SUPPORT:
	    sprintf(new_cmdline, "%s reason=%s", cmdline, "diag");
            setenv("bootargs", new_cmdline);
	    break;
	case SKIP_VOL_CHECK:
	    sprintf(new_cmdline, "%s reason=%s", cmdline, "skip_fsck");
            setenv("bootargs", new_cmdline);
	    break;
	case MEMORY_TEST:
#ifdef USI_BOARD_NVPLUS_V4
	    LCD_select_line(1);
	    for(i = 0; i < 16; i++)
	    {
                LCD_send_data(MSG[MEMORY_TEST][i], LCD_DATA);
            }
#endif
	    usi_mem_test(0, 0xffffffff);	//Jean 2011-05-12
	    break;
	case TEST_DISK:
	    sprintf(new_cmdline, "%s reason=%s", cmdline, "test_disks");
            setenv("bootargs", new_cmdline);
	    break;
	default:
	    break;
    }
}
#endif
