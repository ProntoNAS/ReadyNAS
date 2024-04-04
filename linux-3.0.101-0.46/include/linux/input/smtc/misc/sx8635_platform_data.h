/* !
 * \file sx8635_platform_data.h
 *
 * SX8635 Platform Data
 *
 * Copyright 2012 Semtech Corp.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef _SX8635_PLATFORM_DATA_H_
#define _SX8635_PLATFORM_DATA_H_

#include <linux/slab.h>
#include <linux/input.h>
#include "sx86xx.h" /* main struct, interrupt,init,pointers */

/*
 * \defgroup INTENSITY_GROUP LED Intensities
 * \{
 */
/*! Off Intensity Code
 */
#define SMTC_OFF_INTENSITY 0
/*! On Intensity Code
 */
#define SMTC_ON_INTENSITY 255
/*! Proximity Intensity Code
 */
#define SMTC_PROX_INTENSITY 76  // ~ 30%
/*!  \} 
 */

/*
 * \defgroup IO_GROUP IO Numbers for LEDs
 * \{
 */
/*! IO for the LED corresponding to Wheel Up
 */
#define WHEEL_UP_LED 0
/*! IO for the LED corresponding to Wheel Right
 */
#define WHEEL_RIGHT_LED 1
/*! IO for the LED corresponding to Wheel Down
 */
#define WHEEL_DOWN_LED 2
/*! IO for the LED corresponding to Wheel Left
 */
#define WHEEL_LEFT_LED 3
/*! IO for the LED corresponding to the middle button
 */
#define MIDDLE_BUTTON_LED 4
/*! IO for the LED corresponding to the LOGO
 */
#define LOGO_LED 5
/*!  \} 
 */


/*! \brief Section ID codes used for Wheel and Button information
 */
enum _touchSectionIDs {
  UNKNOWN_SECTION,ALL_OFF,WHEEL_UP,WHEEL_DOWN,WHEEL_LEFT,WHEEL_RIGHT,MAX_WHEEL_STATES,BUTTON_MIDDLE,BUTTON_PROX
}; 

/*! \brief State codes used for Wheel and Button information
 */
enum _states { 
  /*! In case an error or something else unknown occurs, use this 
   */
  UNKNOWN,
  /*! When user releases the touch, this is the event to use.
   * If previous was TOUCH, send touch/no touch.
   * If previous was MOVE or STOPPED, do nothing
   */
  RELEASED,
  /*! When user touches a button or the wheel, go here. Does not mean to send
   * data but the next event will determine what to send (MOVE or RELEASED)
   * Record initial position
   * No events should be sent out
   * LEDs will be processed on the other section
   */
  TOUCH,
  /*! When user moves passed threshold in clock wise motion send this.
   */
  MOVE_CWISE,
  /*! When user moves passed threshold in counter clock wise motion send this.
   */
  MOVE_CCWISE,
  /*! This is the event that is used after a wheel event occurred and user is
   * not moving the wheel.  This only should be used as the main event IF a 
   * move has already occurred. For example, if the user TOUCHES but does not
   * trigger a move, the main event should stay as TOUCHED so that when
   * RELEASE occurs, the touch event gets sent.
   */
  STOPPED
};


/************************************************/
/*! \brief Input struct. Right now just need a keycode
 */
struct _inputEvent {
  int key;
};
typedef struct _inputEvent inputEvent_t;
/************************************************/



/************************************************/
/*! \brief LED IO mapping struct
 */
struct _ledInfo {
  /*! The IO the variable is referring to
   */
  u8 io;
  /*! The intensity to set the IO to
   */
  u8 intensity;
};
/*! \brief Struct to hold the size and array of LED IO mappings
 */
struct _ioGroup {
  /*! Size of the array
   */
  int size;
  /*! Array of LED IO mappings
   */
  struct _ledInfo *led;
};
typedef struct _ioGroup ioGroup_t;
typedef struct _ioGroup *pioGroup_t;
/************************************************/


/********************************************************/
/*! \brief Struct containing information related to a speific
 * wheel or button section.
 */
struct _touchSection {
  /*! The Section ID associated with this variable
   */
  int sectionID;
  /*! Array of what to set the IOs for LEDs to when this section is active
   */
  struct _ledInfo *pLEDState;
  /*! Pointer to the Input to use for this section
   */
  inputEvent_t *pInputState;
  /*! Size of pLEDState
   */
  int nLEDStateSize;
  /*! \brief Current state of this section.
   * \description This does not need to be filled in as it is changed in the
   * code when copies of the section are made.
   */
  int state;
  /*! \brief Minimum section's position when relating to a wheel section.
   * \description This does not need to be filled in as it is changed in the
   * code when copies of the section are made.
   */
  int minSection;
  /*! \brief Maximum section's position when relating to a wheel section.
   * \description This does not need to be filled in as it is changed in the
   * code when copies of the section are made.
   */
  int maxSection;
  /*! \brief Current section's position when relating to a wheel section.
   * \description This does not need to be filled in as it is changed in the
   * code when copies of the section are made.
   */
  int position;
};
typedef struct _touchSection touchSection_t;
/********************************************************/


/********************************************************/
/*! \brief Struct containing information related to the wheel and
 * button information.
 */
struct _touchInformation {

  /*! Array for button sections (middle/prox)
   */
  touchSection_t *buttonSectionGroup;
  /*! Size of the button section array
   */
  int nButtonSectionGroupSize;
  /*! Array for the wheel sections
   */
  touchSection_t *wheelSectionGroup;
  /*! Size of the wheel section array
   */
  int nWheelSectionGroupSize;
  
  /*! Input key that should be used for a CWISE move
   */
  inputEvent_t *moveCWISE_InputState;
  /*! Input key that should be used for a CCWISE movement
   */
  inputEvent_t *moveCCWISE_InputState;
  
  /*! The number of wheel sensors (must coincide with SPM settings)
   */
  int nSensors;
  /*! A shift amount from the max value to define how far the wheel position
   * must be changed to be seen as a move (rotate)
   */
  int move_threshold_factor;
  
  /*! Section to use when nothing is touch or in proximity
   */
  touchSection_t *offSection;
  /*! Section to set when startig the proximity timer
   */
  touchSection_t *proxSection;
   /*! Amount of steps to use when fading out/in
   */
  int nLEDIncrementSteps;
  /*! Delay between each fade out/in steps (in milliseconds)
   */
  int nLEDDelayBetweenSteps;
  /*! Timeout delay for when using the proximity timer (in milliseconds)
   */
  int nLEDTimeoutOffDelay;
  
  /*! Whether to fade in when proximity first appears
   */
  char bLEDFadeInEnable;
  /*! Whether to fade out when proximity is removed
   */
  char bLEDFadeOutEnable;
  
  /*! \brief Max position value of the wheel
   *  \description This is calculated inside the driver.
   */
  int max_value;
  /*! \brief Move threshold for the wheel
   *  \description This is calculated inside the driver.
   */
  int move_threshold;
  /*! \brief Copy of the last section that was used
   *  \description This is calculated inside the driver.
   */
  touchSection_t lastSentSection;
  /*! \brief Input to use when sending key events
   *  \description This is calculated inside the driver.
   */
  struct input_dev *input;
};
typedef struct _touchInformation touchInformation_t;
/********************************************************/



/*! \brief Structure containing register address and value
 */
struct smtc_reg_data {
  /*! Register Address
   */
  unsigned char reg;
  /*! Register Value
   */
  unsigned char val;
};
typedef struct smtc_reg_data smtc_reg_data_t;
typedef struct smtc_reg_data *psmtc_reg_data_t;

/*! \brief Main struct containing spm, i2c, touch, nirq, etc
 */
struct sx8635_platform_data {
  /*! Number of i2c registers to initialize
   */
  int i2c_reg_num;
  /*! Number of spm registers to initialize
   */
  int spm_cfg_num;
  /*! Array of i2c registers
   */
  struct smtc_reg_data *i2c_reg;
  /*! Array of spm registers
   */
  struct smtc_reg_data *spm_cfg;

  /*! Pointer to touch information structure
   */
  struct _touchInformation *touchInformation;

  /*! \brief Function pointer to nirq status
   * \description Since we use ACPI this is actually not needed
   * but here in case things change
   */
  int (*get_is_nirq_low)(void);
  /*! Function pointer that can contain platform data to initialize
   */
  int     (*init_platform_hw)(void);
  /*! Function pointer that can contain platform data exiting information
   */
  void    (*exit_platform_hw)(void);
 
};
typedef struct sx8635_platform_data sx8635_platform_data_t;
typedef struct sx8635_platform_data *psx8635_platform_data_t;

#endif
