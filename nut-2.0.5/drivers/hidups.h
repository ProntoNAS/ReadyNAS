/* hidups.h - prototype HID UPS driver for Network UPS Tools
 
   Copyright (C) 2001  Russell Kroll <rkroll@exploits.org>
 
   Based on evtest.c v1.10 - Copyright (c) 1999-2000 Vojtech Pavlik
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or 
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "timehead.h"

#include "config.h"

#ifndef HID_MAX_USAGES
#define HID_MAX_USAGES 1024	/* horrible workaround hack */
#endif

#include LINUX_HIDDEV		/* from configure */

#define DEFAULT_ONDELAY		13	/* delay between return of utility power */
								/* and powering up of load (10 seconds units for MGE) */
								/* ondelay > offdelay */
#define DEFAULT_OFFDELAY	120	/* delay befor power off, in SECONDS*/ 

#define APC_VENDOR_ID		0x051D
#define CPS_VENDOR_ID		0x0764
#define MGE_VENDOR_ID		0x0463
#define TRIPPLITE_VENDOR_ID	0x09AE  /* TrippLite used the wrong pages for some items, so handle them differently */
#define TRIPPLITE_PRODUCT_ID	0x2005 

#define FLAG_MANUAL_POLL	0x01

/* power device page: x84 */

#define UPS_USAGE			0x840004
#define POWER_USAGE			0x840020	/* wrong, but needed for MGE */
#define UPS_BATTVOLT			0x840030	/* voltage * 100             */
#define UPS_FREQ			0x840032	/* frequency                 */
#define UPS_LOADPCT			0x840035	/* load percentage           */
#define UPS_TEMPERATURE			0x840036	/* UPS temperature	     */
#define UPS_CONFIG_VOLTAGE		0x840040	/* UPS voltage config	     */
#define BATT_CHARGING_TL		0x840044	/* TrippLite's version of 0x850044 */
#define BATT_DISCHARGING_TL		0x840045	/* TrippLite's version of 0x850045 */
#define BATT_NEED_REPLACEMENT_TL	0x84004b	/* TrippLite's version of 0x85004b */
#define UPS_LOW_VOLTAGE_TRANSFER	0x840053	/* transfer on low voltage   */
#define UPS_HIGH_VOLTAGE_TRANSFER	0x840054	/* transfer on high voltage  */
#define UPS_TEST			0x840058	/* UPS test status	     */
#define UPS_AUDIBLE_ALARM		0x84005a	/* audible alarm control     */
#define UPS_STATE_GOOD			0x840061	/* UPS is good [MGE]        */
#define UPS_INTERNAL_FAILURE		0x840062	/* UPS had an internal failure [MGE] */
#define UPS_OVERLOAD			0x840065	/* UPS is overloaded	     */
#define UPS_SHUTDOWN_REQUESTED		0x840068	/* 1 = low battery           */
#define UPS_SHUTDOWN_IMMINENT		0x840069	/* 1 = low battery           */
#define UPS_BOOST			0x84006e	/* Don't care (CPS OR1000ELCDRM1U) */
#define BATT_AC_PRESENT_TL		0x8400d0	/* TrippLite's version of 0x8500d0 */
#define UPS_IMFR			0x8400FD	/* manufacturer name         */
#define UPS_IPRODUCT			0x8400FE	/* model name                */
#define UPS_ISERIAL			0x8400FF	/* serial number             */
#define UPS_WAKEDELAY			0x840056
#define UPS_GRACEDELAY			0x840057
#define UPS_COMM_LOST			0x840073	/* lost communication w/ UPS */

/* battery system page: x85 */

#define BATT_UNDEFINED			0x850000	/* No idea - alway 0 on MGE ellipse 500 */
#define BATT_REMAINING_TIME_LIMIT	0x85002a	/* Remaining time limit FSD  */
#define BATT_BELOW_RCL			0x850042	/* below remaining cap limit */
#define BATT_BELOW_RTL			0x850043	/* below remaining time limit */
#define BATT_CHARGING			0x850044	/* 0 = no longer charging    */
#define BATT_DISCHARGING		0x850045	/* 1 = on battery            */
#define BATT_FULLY_CHARGED		0x850046	/* 1 = battery fully charged */
#define BATT_NEED_REPLACEMENT		0x85004b	/* battery needs replacement */
#define BATT_REMAINING_CAPACITY		0x850066	/* battery percentage        */
#define BATT_RUNTIME_TO_EMPTY		0x850068	/* minutes                   */
#define BATT_MFRDATE			0x850085	/* manufacturer date         */
#define BATT_ICHEMISTRY			0x850089	/* battery type              */
#define BATT_AC_PRESENT			0x8500d0	/* 1 = on line               */
#define BATT_PRESENT			0x8500d1	/* battery present	     */
#define BATT_VOLT_UNREGULATED		0x8500db	/* voltage is not regulated  */
#define BATT_IOEMINFORMATION		0x85008f	/* battery OEM description   */

/* APC Vendor-specific page: x(ff)86 */
#define APC_GARBAGE_1			0xff860052	/* Don't know about this one */
#define APC_GARBAGE_2			0xff860060	/* APC Status Flag	     */
