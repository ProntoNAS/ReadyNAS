/******************************************************************************
 *
 * Name:	skspilole.c
 * Project:	Flash Programmer, Manufacturing and Diagnostic Tools
 * Version:	$Revision: 1.2 $
 * Date:	$Date: 2012-07-17 16:49:07 $
 * Purpose:	Contains Low Level Functions for the integration of the skspi.c module
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

static const char SysKonnectFileId[] =
	"@(#) $Id: 000-sk98lin_v10.70.1.3_K3.0.patch,v 1.2 2012-07-17 16:49:07 jmaggard Exp $ (C) Marvell.";

#include "h/skdrv1st.h"
#include "h/skdrv2nd.h"
#include "h/skgespi.h"


SK_AC *lpAC;
static  SK_U32 timebuf;

int fl_type;
/*
 * global vars
 */
long max_pages = 0;
long max_faddr = 0;


/* low level SPI programming interface */

void spi_init_pac( SK_AC *pAC )  {
	lpAC = pAC;
}

void spi_in8(unsigned short offs, unsigned char *val )  {
	SK_IN8( lpAC->IoBase, offs, val ); 
}

void spi_in16(unsigned short offs, unsigned short *val ){
	SK_IN16( lpAC->IoBase, offs, val ); 
}

void spi_in32(unsigned short offs, unsigned long *val ){
	SK_IN32( lpAC->IoBase, offs, val ); 
}

void spi_out8(unsigned short offs, unsigned char val ){
	SK_OUT8( lpAC->IoBase, offs, val ); 
}

void spi_out16(unsigned short offs, unsigned short val ){
	SK_OUT16( lpAC->IoBase, offs, val ); 
}

void spi_out32(unsigned short offs, unsigned long val ){
	SK_OUT32( lpAC->IoBase, offs, val ); 
}

int  spi_timer(unsigned int t){
	if(t)
	{
		timebuf = (SK_U32)SkOsGetTime(lpAC)+(SK_U32)t*SK_TICKS_PER_SEC ; 
	} else
	{
		if((timebuf <= (SK_U32)SkOsGetTime(lpAC)))
		{
			return(1); 
		}
	}
	return(0); 
}

/*  dummies */
void fl_print(char *msg, ...) {
}

unsigned char *spi_malloc( unsigned short size )  {
	return( kmalloc(size,GFP_KERNEL) );
}

void spi_free( unsigned char *buf )  {
	kfree(buf);
}

