
/******************************************************************************
 *
 * Name:    skfops.c
 * Project: Gigabit Ethernet Adapters, Common Modules
 * Version: $Revision: 1.2 $
 * Date:    $Date: 2012-07-17 16:49:07 $
 * Purpose: Kernel mode file read functions.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *	(C)Copyright 1998-2002 SysKonnect GmbH.
 *	(C)Copyright 2002-2007 Marvell.
 *
 *	Driver for Marvell Yukon/2 chipset and SysKonnect Gigabit Ethernet 
 *      Server Adapters.
 *
 *	Address all question to: gr-msgg-linux@marvell.com
 *
 *      LICENSE:
 *      (C)Copyright Marvell.
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      The information in this file is provided "AS IS" without warranty.
 *      /LICENSE
 *
 *****************************************************************************/
#define __SK_FOPS_H__

#ifndef USE_ASF_DASH_FW
#define FLASH_FROM_BINARY
#endif

SK_BOOL fw_read(	SK_AC *pAC,    /* Pointer to adapter context */
	char *name, SK_U8 **addr, SK_U32 *len );
SK_BOOL fw_file_exists(	SK_AC *pAC,    /* Pointer to adapter context */
	char *name );


#ifndef FLASH_FROM_BINARY
#define SK_DASHFM_1_LENGTH 65536
#define SK_DASHFM_2_LENGTH 53248
#endif /* FLASH_FROM_BINARY */

