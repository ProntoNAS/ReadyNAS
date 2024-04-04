/*
 * SX8635 Cap Touch specific registers
 *
 * Copyright 2011 Semtech Corp.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef _SX8635_I2C_REG_H_
#define _SX8635_I2C_REG_H_
#include "sx863x_i2c_reg.h"
/*
 *  I2C Registers
 */
#define SX863X_CAPSTATMSB_REG     0x01

#define SX863X_WHLPOSMSB_REG  0x03
#define SX863X_WHLPOSLSB_REG  0x04

#define SX863X_CAPSTAT_BUTTON11        0x800
#define SX863X_CAPSTAT_BUTTON10        0x400
#define SX863X_CAPSTAT_BUTTON9         0x200
#define SX863X_CAPSTAT_BUTTON8         0x100


/*
 *  I2C Register Values
 */


#define SX863X_IRQSRC_WHEEL_FLAG      0x08

/*      CapStatMsb */
/* Did wheel rotate clockwise? */
#define SX863X_CAPSTATMSB_WHL_CWISE   0x40
/* Did wheel rotate counter-clockwise? */
#define SX863X_CAPSTATMSB_WHL_CCWISE  0x20
/* Was wheel touched or released? */
#define SX863X_CAPSTATMSB_WHL_TOUCHED 0x10

#endif /* _SX8635_I2C_REG_H_*/



