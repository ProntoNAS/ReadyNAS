/*
 * SX8635 specifics.
 * NOTE: the includes listed below are required for the development
 * board this was written on using the OMAP processor. These defines
 * may be different depending on the functions the specific processor
 * uses.
 *
 * Copyright 2012 Semtech Corp.
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/i2c/twl.h>
#include <linux/regulator/machine.h>
#include <linux/gpio.h>

#include <linux/input/smtc/misc/sx8635_platform_data.h>
#include <linux/input/smtc/misc/sx8635_i2c_reg.h>
#include <linux/input/smtc/misc/sx8635_spm_cfg.h>


/* IO Used for NIRQ */
#define ICH10_FOX



#include <linux/input/smtc/misc/sx8635_platform_data.h>
#include <linux/input/smtc/misc/sx8635_i2c_reg.h>
#include <linux/input/smtc/misc/sx8635_spm_cfg.h>

#ifdef ICH10_FOX
//ICH10
//#define GPIO_SX8635_NIRQ 205 //GPIO0
#define GPIO_SX8635_NIRQ 206 //GPIO0
#else
//ICH9
#define GPIO_SX8635_NIRQ 196	//GPIO1
#endif

#define SX8635_NIRQ 9//16//2
static int sx8635_get_nirq_state(void)
{	int ret = 0;
	ret=gpio_get_value(GPIO_SX8635_NIRQ);
	return !ret;
}

static inline void __init s_sx8635_init(void)
{
  printk(KERN_INFO "s_sx8635_init()\n");
  if ((gpio_request(GPIO_SX8635_NIRQ, "SX8635_NIRQ") == 0) &&
	    	(gpio_direction_input(GPIO_SX8635_NIRQ) == 0)) {
    gpio_export(GPIO_SX8635_NIRQ, 0);
    printk(KERN_ERR "obtained gpio for SX8635_NIRQ:GPIO_SX8635_NIRQ=0x%x\n",GPIO_SX8635_NIRQ);
  } else {
	  return;
  }
}

/* Define Registers that need to be initialized to values different than
 * the default
 *************************************
 * Add registers to this by writing
 * {
 *   .reg = REG_ADDRESS,
 *   .val = REG_VALUE,
 * },
 ************************
 * defines and values of register address can be found in
 * sx863x_i2c_reg.h and sx8635_i2c_reg.h
 *************************************
 */
static struct smtc_reg_data sx8635_i2c_reg_setup[] = {
  {
    .reg = SX863X_COMPOPMODE_REG,
    .val = SX863X_COMPOPMODE_OPERATINGMODE_ACTIVE,
  }
};
/* Define SPM configuration map to be intialized to values different than
 * default. NOTE: Programming is done in 8 blocks. 
 * However, the driver is setup to perform needed reads and adjustments so
 * that it is not required to add in extra default values.
 * When final SPM values are obtained it may be advantageous to set them in
 * NVM so that the driver does not need to initialize them.
 *************************************
 * Add spm registers to this by writing
 * {
 *   .reg = SPM_ADDRESS,
 *   .val = SPM_VALUE,
 * },
 ************************
 * defines and values of spm register address can be found in
 * sx863x_spm_reg.h and sx8635_spm_reg.h
 *************************************
*/ 
static struct smtc_reg_data sx8635_spm_cfg_setup[] = {
  {
    .reg = 0x00,
    .val = 0x2F,
  },
  {
    .reg =  0x01,
    .val =  0x00,
  },
  {
    .reg =  0x02,
    .val =  0x19,
  },
  {
    .reg =  0x03,
    .val =  0x0E,
  },
  {
    .reg =  SX863X_I2CADDRESS_SPM,
    .val =  0x2B,
  },
  {
    .reg =  SX863X_ACTIVESCANPERIOD_SPM,
    //.val =  0x01,
    .val =  0x02,
  },
  {
    .reg =  SX863X_DOZESCANPERIOD_SPM,
    .val =  0x0D,
  },
  {
    .reg = SX863X_PASSIVETIMER_SPM,
    .val =  0x00,
  },
  {
    .reg =  0x08,
    .val =  0x00,
  },
  {
    .reg =  SX863X_CAPEMODEMISC_SPM,
    .val =  0x04,
  },
  {
    .reg =  SX863X_CAPMODE11_8_SPM,
    .val =  0x0F,
  },
  {
    .reg =  SX863X_CAPMODE7_4_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_CAPMODE3_0_SPM,
    .val =  0xF5,
  },
  {
    .reg =  SX863X_CAPSENSITIVITY0_1_SPM,
    .val =  0x75,
  },
  {
    .reg =  SX863X_CAPSENSITIVITY2_3_SPM,
    //.val =  0x44,
    .val =  0x55,
  },
  {
    .reg =  SX863X_CAPSENSITIVITY4_5_SPM,
    //.val =  0x44,
    .val =  0x55,
  },
  {
    .reg =  SX863X_CAPSENSITIVITY6_7_SPM,
    //.val =  0x44,
    .val =  0x55,
  },
  {
    .reg =  SX863X_CAPSENSITIVITY8_9_SPM,
    //.val =  0x44,
    .val =  0x55,
  },
  {
    .reg =  SX863X_CAPSENSITIVITY10_11_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_CAPTHRESH0_SPM,
    //.val =  0x70,
    .val =  0x80,
  },
  {
    .reg =  SX863X_CAPTHRESH1_SPM,
    .val =  0xB0,
  },
  {
    .reg =  SX863X_CAPTHRESH2_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH3_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH4_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH5_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH6_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH7_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH8_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH9_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH10_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPTHRESH11_SPM,
    .val =  0x98,
  },
  {
    .reg =  SX863X_CAPPERCOMP_SPM,
    .val =  0x00,
  },
  {
    .reg =  0x20,
    .val =  0x00,
  },
  {
    .reg =  SX863X_BTNCFG_SPM,
    //.val =  0x38,
    .val =  0x30,
  },
  {
    .reg =  SX863X_BTNAVGTHRESH_SPM,
    .val =  0x48,
  },
  {
    .reg =  SX863X_BTNCOMPNEGTHRESH_SPM,
    //.val =  0x60,
    .val =  0x48,
  },
  {
    .reg =  SX863X_BTNCOMPNEGCNTMAX_SPM,
    .val =  0x01,
  },
  {
    .reg =  SX863X_BTNHYSTERESIS_SPM,
    .val =  0x05,
  },
  {
    .reg =  SX863X_BTNSTUCKATTIMEOUT_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_WHLCFG_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_WHLSTUCKATTIMEOUT_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_WHLHYSTERESIS_SPM,
    .val =  0x05,
  },
  {
    .reg =  0x2A,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_WHLNORMMSB_SPM,
    .val =  0x01,
  },
  {
    .reg =  SX863X_WHLNORMLSB_SPM,
    .val =  0x40,
  },
  {
    .reg =  SX863X_WHLAVGTHRESH_SPM,
    .val =  0x50,
  },
  {
    .reg =  SX863X_WHLCOMPNEGTHRESH_SPM,
    //.val =  0x60,
    .val =  0x50,
  },
  {
    .reg =  SX863X_WHLCOMPNEGCNTMAX_SPM,
    .val =  0x03,
  },
  {
    .reg =  SX863X_WHLROTATETHRESH_SPM,
    .val =  0x0A,
  },
  {
    .reg =  SX863X_WHLOFFSET_SPM,
    .val =  0x00,
  },
  {
    .reg =  0x32,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPWAKEUPSIZE_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPWAKEUPVALUE0_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPWAKEUPVALUE1_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPWAKEUPVALUE2_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHT0_SPM,
    .val =  0xBB,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHT1_SPM,
    .val =  0xBB,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHT2_SPM,
    .val =  0xBB,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHT3_SPM,
    .val =  0xBB,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHTGRP0MSB_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHTGRP0LSB_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHTGRP1MSB_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_MAPAUTOLIGHTGRP1LSB_SPM,
    .val =  0x00,
  },
  {
    .reg =  0x3F,
    .val =  0x02,
  },
  {
    .reg =  SX863X_GPIOMODE7_4_SPM,
    .val =  0x55,
  },
  {
    .reg =  SX863X_GPIOMODE3_0_SPM,
    .val =  0x55,
  },
  {
    .reg =  SX863X_GPIOOUTPWRUP_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOAUTOLIGHT_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOPOLARITY_SPM,
    .val =  0x3F,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON0_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON1_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON2_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON3_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON4_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON5_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON6_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYON7_SPM,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF0_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF1_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF2_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF3_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF4_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF5_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF6_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTENSITYOFF7_SPM,
    .val =  0x00,
  },
  {
    .reg =  0x55,
    .val =  0xFF,
  },
  {
    .reg =  SX863X_GPIOFUNCTION_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINCFACTOR_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIODECFACTOR_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINCTIME7_6_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINCTIME5_4_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINCTIME3_2_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINCTIME1_0_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIODECTIME7_6_SPM,
    .val =  0x44,
  },
  {
    .reg =  SX863X_GPIODECTIME5_4_SPM,
    .val =  0x44,
  },
  {
    .reg =  SX863X_GPIODECTIME3_2_SPM,
    .val =  0x44,
  },
  {
    .reg =  SX863X_GPIODECTIME1_0_SPM,
    .val =  0x44,
  },
  {
    .reg =  SX863X_GPIOOFFDELAY7_6_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOOFFDELAY5_4_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOOFFDELAY3_2_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOOFFDELAY1_0_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOPULLUPDOWN7_4_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOPULLUPDOWN3_0_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTERRUPT7_4_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIOINTERRUPT3_0_SPM,
    .val =  0x00,
  },
  {
    .reg =  SX863X_GPIODEBOUNCE_SPM,
    .val =  0x00,
  },
  {
    .reg =  0x6A,
    .val =  0x00,
  },
  {
    .reg =  0x6B,
    .val =  0x00,
  },
  {
    .reg =  0x6C,
    .val =  0x00,
  },
  {
    .reg =  0x6D,
    .val =  0x00,
  },
  {
    .reg =  0x6E,
    .val =  0x00,
  },
  {
    .reg =  0x6F,
    .val =  0x50,
  },
  {
    .reg =  SX863X_CAPPROXENABLE_SPM,
    .val =  0x74,
  },
  {
    .reg =  0x71,
    .val =  0x10,
  },
  {
    .reg =  0x72,
    .val =  0x45,
  },
  {
    .reg =  0x73,
    .val =  0x02,
  },
  {
    .reg =  0x74,
    .val =  0xFF,
  },
  {
    .reg =  0x75,
    .val =  0xFF,
  },
  {
    .reg =  0x76,
    .val =  0xFF,
  },
  {
    .reg =  0x77,
    .val =  0xD5,
  },
  {
    .reg =  0x78,
    .val =  0x55,
  },
  {
    .reg =  0x79,
    .val =  0x55,
  },
  {
    .reg =  0x7A,
    .val =  0x7F,
  },
  {
    .reg =  0x7B,
    .val =  0x23,
  },
  {
    .reg =  0x7C,
    .val =  0x22,
  },
  {
    .reg =  0x7D,
    .val =  0x41,
  },
  {
    .reg =  0x7E,
    .val =  0xFF,
  },
};
/***********************************************************/
/***********************************************************/
/* Define the intensities for each action. The name of the
 * variable corresponds to what action the intensities are
 * for. 
 *************************************
 * Adding or removing different LEDs from being set can be
 * done by adding or removing the io section.
 * IO sections are defined by
 * {
 *   .io = IO_CHANGING,
 *   .intensity = INTENSITY_TO_PROGRAM_TO_DEVICE,
 * },
 ************************
 * defines of the currently used intensities and leds can be found in
 * sx8635_platform_data.h
 *************************************
 */
static struct _ledInfo wheelUpIntensities[] = {
  {
    .io = WHEEL_UP_LED,
    .intensity = SMTC_ON_INTENSITY,
  },
  {
    .io = WHEEL_RIGHT_LED,
    .intensity = SMTC_PROX_INTENSITY,
  },
  {
    .io = WHEEL_DOWN_LED,
    .intensity = SMTC_PROX_INTENSITY,
  },
  {
    .io = WHEEL_LEFT_LED,
    .intensity = SMTC_PROX_INTENSITY,
  },
  {
    .io = MIDDLE_BUTTON_LED,
    .intensity = SMTC_PROX_INTENSITY,
  },
  {
    .io = LOGO_LED,
    .intensity = SMTC_ON_INTENSITY,
  },
};
static struct _ledInfo wheelRightIntensities[] = {
    {
      .io = WHEEL_UP_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = WHEEL_RIGHT_LED,
      .intensity = SMTC_ON_INTENSITY,
    },
    {
      .io = WHEEL_DOWN_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = WHEEL_LEFT_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = MIDDLE_BUTTON_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = LOGO_LED,
      .intensity = SMTC_ON_INTENSITY,
    },
};

static struct _ledInfo wheelDownIntensities[] = {
    {
      .io = WHEEL_UP_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = WHEEL_RIGHT_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = WHEEL_DOWN_LED,
      .intensity = SMTC_ON_INTENSITY,
    },
    {
      .io = WHEEL_LEFT_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = MIDDLE_BUTTON_LED,
      .intensity = SMTC_PROX_INTENSITY,
    },
    {
      .io = LOGO_LED,
      .intensity = SMTC_ON_INTENSITY,
    },
};
static struct _ledInfo wheelLeftIntensities[] = {
  {
      .io = WHEEL_UP_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_RIGHT_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_DOWN_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_LEFT_LED,
      .intensity = SMTC_ON_INTENSITY,
  },
  {
      .io = MIDDLE_BUTTON_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = LOGO_LED,
      .intensity = SMTC_ON_INTENSITY,
  },
};


static struct _ledInfo offIntensities[] = {
  {
      .io = WHEEL_UP_LED,
      .intensity = SMTC_OFF_INTENSITY,
  },
  {
      .io = WHEEL_RIGHT_LED,
      .intensity = SMTC_OFF_INTENSITY,
  },
  {
      .io = WHEEL_DOWN_LED,
      .intensity = SMTC_OFF_INTENSITY,
  },
  {
      .io = WHEEL_LEFT_LED,
      .intensity = SMTC_OFF_INTENSITY,
  },
  {
      .io = MIDDLE_BUTTON_LED,
      .intensity = SMTC_OFF_INTENSITY,
  },
  {
      .io = LOGO_LED,
      .intensity = SMTC_OFF_INTENSITY,
  },
};

static struct _ledInfo buttonMiddleIntensities[] = {
  {
      .io = WHEEL_UP_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_RIGHT_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_DOWN_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_LEFT_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = MIDDLE_BUTTON_LED,
      .intensity = SMTC_ON_INTENSITY,
  },
  {
      .io = LOGO_LED,
      .intensity = SMTC_ON_INTENSITY,
  },
};

static struct _ledInfo buttonProxIntensities[] = {
  {
      .io = WHEEL_UP_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_RIGHT_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_DOWN_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = WHEEL_LEFT_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = MIDDLE_BUTTON_LED,
      .intensity = SMTC_PROX_INTENSITY,
  },
  {
      .io = LOGO_LED,
      .intensity = SMTC_ON_INTENSITY,
  },
};
/***********************************************************/
/***********************************************************/




/***********************************************************/
/***********************************************************/
/* Define the input key to be used for each section (or action).
 * The driver defines the action to be a touch and release when
 * sending the action.
 *************************************
 * To change this, simply change the following.
 *   .key = CORRESPONDING_KEY_CODE,
 ************************
 * defines of the inputs can be found in
 * input.h
 *************************************
 */
static inputEvent_t buttonMiddleInput = { 
  .key = KEY_OK,
};

static inputEvent_t wheelDownInput = { 
  .key = KEY_DOWN,
};

static inputEvent_t wheelRightInput = { 
  .key = KEY_FORWARD,
};

static inputEvent_t wheelUpInput = { 
  .key = KEY_UP,
};

static inputEvent_t wheelLeftInput = { 
  .key = KEY_BACK,
};

static inputEvent_t wheelMoveCWiseInput = { 
  .key = KEY_SCROLLDOWN,
};

static inputEvent_t wheelMoveCCWiseInput = { 
  .key = KEY_SCROLLUP,
};
/***********************************************************/
/***********************************************************/

/*************************************
 * The following structs and arrays normally should not be changed as they
 * group the above settings in their correct sections and actions.
 * If items such as sending a specific input is needed when proximity
 * occurs, it can be done here..
 * Below is a description of the variables
 *  .sectionID = SECTION_ID_FOR_DRIVER_IDENTIFYING,
 *  .pLEDState = POINTER_TO_INTENSITY_TO_SEND,
 *  .nLEDStateSize = SIZE_OF_LED_POINTER_AS_A_VARIABLE_LED_AMOUNT_CAN_BE_USED,
 *  .pInputState = POINTER_TO_INPUT_STATE_TO_SEND,
 ************************
 * The pointers are defined above
 *************************************
*/
/***********************************************************/
/***********************************************************/
/* Define some specific sections such as the off and proximity.
 * Generally only the LED part will be used but this allows a
 * more generic way to handle it in case later requirements
 * change.
  */
static struct _touchSection allOffSection = {
  .sectionID = ALL_OFF,
  .pLEDState = offIntensities,
  .nLEDStateSize = ARRAY_SIZE(offIntensities),
  .pInputState = NULL,
};
static struct _touchSection proxSection = {
    .sectionID = BUTTON_PROX,
    .pLEDState = buttonProxIntensities,
    .nLEDStateSize = ARRAY_SIZE(buttonProxIntensities),
    .pInputState = NULL,
};
/* Define array of the sections specific to button sensors.
 * This is the proximity and middle.
 */
static struct _touchSection buttonSectionArray[] = {
  {
    .sectionID = BUTTON_PROX,
    .pLEDState = buttonProxIntensities,
    .nLEDStateSize = ARRAY_SIZE(buttonProxIntensities),
    .pInputState = NULL,
  },
  {
    .sectionID = BUTTON_MIDDLE,
    .pLEDState = buttonMiddleIntensities,
    .nLEDStateSize = ARRAY_SIZE(buttonMiddleIntensities),
    .pInputState = &buttonMiddleInput,
  },
 
};
/* Define array of the sections specific to wheel sensors.
 * This is WHEEL UP, DOWN, LEFT, RIGHT
 */
static struct _touchSection wheelSectionArray[] = {
  {
    .sectionID = WHEEL_DOWN,
    .pLEDState = wheelDownIntensities,
    .nLEDStateSize = ARRAY_SIZE(wheelDownIntensities),
    .pInputState = &wheelDownInput,
  },
  {
    .sectionID = WHEEL_RIGHT,
    .pLEDState = wheelRightIntensities,
    .nLEDStateSize = ARRAY_SIZE(wheelRightIntensities),
    .pInputState = &wheelRightInput,
  },
  { 
    .sectionID = WHEEL_UP,
    .pLEDState = wheelUpIntensities,
    .nLEDStateSize = ARRAY_SIZE(wheelUpIntensities),
    .pInputState = &wheelUpInput,
  },
  {
    .sectionID = WHEEL_LEFT,
    .pLEDState =   wheelLeftIntensities,
     .nLEDStateSize = ARRAY_SIZE(wheelLeftIntensities),
    .pInputState = &wheelLeftInput,
  },
};
/***********************************************************/
/***********************************************************/


/***********************************************************/
/***********************************************************/
/* This is the main struct for keeping track of wheel and button information.
 * .nSensors should be defined as the number of wheel sensors
 * .move_threshold_factor This is a shift amount from the max value to define
 *                         how far the wheel position must be changed to be
 *                         seen as a move (rotate)
 * .wheelSectionGroup This is the array defined above for the wheel sections
 * .nWheelSectionGroupSize This is how many sections are in the wheel array
 * .moveCWISE_InputState This is what input key should be used for a CWISE move
 * .moveCCWISE_InputState This is the input key for a CCWISE movement
 * .offSection This is what should be used when nothing is touching
 * .proxSection This is what should be used when running on the proximity timer
 * .buttonSectionGroup This is the array for button sections (middle/prox)
 * .nButtonSectionGroupSize This is the size of the button section array
 * .nLEDIncrementSteps The amount of steps to use when fade out/in
 * .nLEDDelayBetweenSteps The delay between each fade out/in step
 * .nLEDTimeoutOffDelay This is the delay for when using the proximity timer
 */
static struct _touchInformation touchInformation = {
  .nSensors   = 8, /* must coincide with SPM settings */
  .move_threshold_factor = 4,
  .wheelSectionGroup = wheelSectionArray,
  .nWheelSectionGroupSize = ARRAY_SIZE(wheelSectionArray),
  .moveCWISE_InputState = &wheelMoveCWiseInput,
  .moveCCWISE_InputState = &wheelMoveCCWiseInput,
  .offSection = &allOffSection,
  .proxSection = &proxSection,
  .buttonSectionGroup = buttonSectionArray,
  .nButtonSectionGroupSize = ARRAY_SIZE(buttonSectionArray),
  .nLEDIncrementSteps = 40,
  .nLEDDelayBetweenSteps = 1,
  .nLEDTimeoutOffDelay = 30000,
  .bLEDFadeInEnable = 0,
  .bLEDFadeOutEnable = 1,
};
/***********************************************************/
/***********************************************************/

/***********************************************************/
/* Main struct */
static sx8635_platform_data_t sx8635_config = {
  /* Function pointer to get the NIRQ state (1->NIRQ-low, 0->NIRQ-high) */
  .get_is_nirq_low = sx8635_get_nirq_state,
  /*  pointer to an initializer function. Here in case needed in the future */
  .init_platform_hw = 0,
  /*  pointer to an exit function. Here in case needed in the future */
  .exit_platform_hw = 0,
  /* number of i2c registers to initialize */
  .i2c_reg_num = ARRAY_SIZE(sx8635_i2c_reg_setup),
  /* number of spm registers to initialize */
  .spm_cfg_num = ARRAY_SIZE(sx8635_spm_cfg_setup),
  /* The i2c register array for initializing */
  .i2c_reg = sx8635_i2c_reg_setup,
  /* The spm register array for initializing */
  .spm_cfg = sx8635_spm_cfg_setup,
  /* Pointer to struct defined above */
  .touchInformation = &touchInformation,
};
/***********************************************************/



/********************************************************************/
/* Use this define in the board specific array initializer */
#define SX8635_BOARD_INFO \
    I2C_BOARD_INFO("sx8635", 0x2B), \
		.flags         = I2C_CLIENT_WAKE,\
		.irq           = SX8635_NIRQ, \
		.platform_data = &sx8635_config,
/********************************************************************/

/* Below is an example of how to initialize this */
static struct i2c_board_info __initdata smtc_i2c_boardinfo[] = {
  {
    SX8635_BOARD_INFO
  },
};
static int __init sx8635_i2c_init(struct i2c_adapter *adapter)
{
	s_sx8635_init();
	printk(KERN_INFO " sx8635 i2c_new_device()\n");
	i2c_new_device(adapter,smtc_i2c_boardinfo);
	return 0;
}

