#ifndef __INUSIBATH
#define __INUSIBATH

#include "mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvAhbToMbus.h"
#include "gpp/mvGppRegs.h"
#include "rtc/integ_rtc/mvRtcReg.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvSysPex.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"


#define SATA0_SCONTROL_REG     0x82308
#define SATA0_PHY_MODE2_REG    0x82330
#define SATA0_INT_CONFIG_REG   0x82050
#define SATA1_SCONTROL_REG     0x84308
#define SATA1_PHY_MODE2_REG    0x84330
#define SATA1_INT_CONFIG_REG   0x84050

#define USB0_PORT_SC1_REG      0x50184
#define USB1_PORT_SC1_REG      0x51184

#define CLK_CRTL_REG           0x2011C

//#define SMI_REG                      0x72004

#define USB0_PM
#undef ETH_PHY_PM

#if 1 //libo 2011-02-17
#define HOLDON_LEN 20
enum BTN_ID {
    RESET,
    USB,
    POWER,
};

enum BOOTMENU {
    BOOTMENU,
    NORMAL,
    FACTORY_DEFAULT,
    OS_REINSTALL,
    TECH_SUPPORT,
    SKIP_VOL_CHECK,
    MEMORY_TEST,
    TEST_DISK,
#ifdef USI_BOARD_NVPLUS_V4
    SPACE,
    BOOTING,
    /* Jean, 2011-06-01 */
    MEMORY_TEST_TIME,
    MEMORY_TEST_SUCCESS,
    MEMORY_TEST_FAILURE,
    MEMORY_TEST_POWEROFF,
#endif
};

#ifdef USI_BOARD_NVPLUS_V4
extern MV_U8 MSG[][16];
void LCD_show(MV_U32 bootmenu_item);
#endif
 
MV_U32 is_btn_pressed(enum BTN_ID);
void menu_show(MV_U32);
void save_user_selection(MV_U32);
#endif //if 1

#endif
