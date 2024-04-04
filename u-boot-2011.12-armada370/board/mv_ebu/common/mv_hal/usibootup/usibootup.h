#ifndef __INCusibootuph
#define __INCusibootuph
#include "mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvAhbToMbus.h"
#include "gpp/mvGppRegs.h"
#include "rtc/integ_rtc/mvRtcReg.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

/* Rn102 */
/* Power LED : MPP57
 * Sata1 LED : MPP15
 * Sata2 LED : MPP14
 * Backup LED: MPP56
 * CPUPW off : MPP8
 * Reset BTN : MPP6
 * Backup BTN: MPP58
 * Power BTN : MPP62
 */
#ifdef NETGEAR_RN102
#define POWER_LED       BIT25
#define POWER_LED_GRP	1
#define SATA1_LED       BIT15
#define SATA1_LED_GRP	0
#define SATA2_LED       BIT14
#define SATA2_LED_GRP	0
#define BACKUP_LED      BIT24
#define BACKUP_LED_GRP	1

#define CPUPW_OFF       BIT8
#define CPUPW_OFF_GRP	0

#define RESET_BTN	BIT6
#define RESET_BTN_GRP	0
#define BACKUP_BTN	BIT26
#define BACKUP_BTN_GRP	1
#define POWER_BTN	BIT30
#define POWER_BTN_GRP	1
#endif

/* Rn104 */
/* CPUPW off : MPP47
 * Backup LED: MPP63
 * Power LED : MPP64
 * Reset BTN : MPP65
 * Backup BTN: MPP52
 * Power BTN : MPP62
 */
#ifdef NETGEAR_RN104
#define CPUPW_OFF_MVT_MP	BIT28
#define CPUPW_OFF_EVT_DVT	BIT15
#define CPUPW_OFF_GRP	1

#define BACKUP_LED      BIT31
#define BACKUP_LED_GRP	1
#define POWER_LED	BIT0
#define POWER_LED_GRP	2

#define RESET_BTN	BIT1
#define RESET_BTN_GRP	2
#define BACKUP_BTN	BIT20
#define BACKUP_BTN_GRP	1
#define POWER_BTN	BIT30
#define POWER_BTN_GRP	1
#endif

/* RN25 */
/* Power LED : MPP53
 * Sata1 LED : MPP14
 * Sata2 LED : MPP15
 * Backup LED: MPP56
 * CPUPW off : MPP8
 * Reset BTN : MPP6
 * Backup BTN: MPP58
 * Power BTN : MPP62
 */
#ifdef NETGEAR_RN25
#define POWER_LED       BIT21
#define POWER_LED_GRP   1
#define SATA1_LED       BIT15
#define SATA1_LED_GRP   0
#define SATA2_LED       BIT14
#define SATA2_LED_GRP   0
#define BACKUP_LED      BIT24
#define BACKUP_LED_GRP  1

#define CPUPW_OFF       BIT8
#define CPUPW_OFF_GRP   0

#define RESET_BTN       BIT6
#define RESET_BTN_GRP   0
#define BACKUP_BTN      BIT26
#define BACKUP_BTN_GRP  1
#define POWER_BTN       BIT30
#define POWER_BTN_GRP   1
#endif

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
    VOL_RO,
    MEMORY_TEST,
    TEST_DISK,
#ifdef NETGEAR_RN104
    SPACE,
    BOOTING,
    MEMORY_TEST_TIME,
    MEMORY_TEST_SUCCESS,
    MEMORY_TEST_FAILURE,
    MEMORY_TEST_POWEROFF,
    BRAND,
    WELCOME,
    BOOT_FROM_USB,
    FAILED,
#endif
};

enum  USI_BOARD_TYPE {
	USI_EVT_DVT = 0,
	USI_MVT_MP,
};

#ifdef NETGEAR_RN104
extern MV_U8 MSG[][16];
void LCD_show(MV_U32 bootmenu_item, MV_U32 line);
#endif
 
MV_U32 is_btn_pressed(enum BTN_ID);
void menu_show(MV_U32);
void save_user_selection(MV_U32);

#endif
