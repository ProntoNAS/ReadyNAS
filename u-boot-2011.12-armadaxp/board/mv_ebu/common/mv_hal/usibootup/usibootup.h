#ifndef __INCusibootuph
#define __INCusibootuph
#include "mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvAhbToMbus.h"
#include "gpp/mvGppRegs.h"
#include "rtc/integ_rtc/mvRtcReg.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

/* Reset BTN : MPP41
 * Power BTN : MPP27
 * ERROR LEN : MPP45
 * CPUPW_OFF : MPP42
 */
#define RESET_BTN	BIT9
#define RESET_BTN_GRP	1
#define POWER_BTN	BIT27
#define POWER_BTN_GRP	0
#define ERROR_LED       BIT13
#define ERROR_LED_GRP	1

#define HOLDON_LEN 20
enum BTN_ID {
    RESET,
    POWER,
};

enum BOOTMENU {
    BOOTMENU,
    NORMAL,
    FACTORY_DEFAULT,
    OS_REINSTALL,
    TECH_SUPPORT,
    VOL_RO,
    MEMORY_TEST,
    TEST_DISK,
};

MV_U32 is_btn_pressed(enum BTN_ID btn_id, MV_U32 max_msec);
void set_power_off_mode(void);
void menu_show(MV_U32);
void save_user_selection(MV_U32);
#endif
