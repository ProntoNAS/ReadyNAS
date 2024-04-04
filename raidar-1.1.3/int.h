/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * int.h                                                      July 2001
 * Copyright (C) 2005, Infrant Technologies, Inc.        
 * All rights reserved 
 **********************************************************************/

#ifndef _RAIDARD_INT_H
#define _RAIDARD_INT_H

#include "config.h"

/* Define int8, uint8, int16, uint16, int32 and uint32 */

#ifndef int8
#if (SIZEOF_CHAR != 1)
#error Cannot determine type for int8
#else
#define int8 char
#endif
#endif

#ifndef uint8
#if (SIZEOF_CHAR != 1)
#define Cannot determine type for uint8
#else
#define uint8 unsigned char
#endif
#endif

#ifndef int16 
#if (SIZEOF_SHORT != 2)
#error Cannot determine type for int16
#else 
#define int16 short
#endif 
#endif

#ifndef uint16 
#if (SIZEOF_SHORT != 2)
#error Cannot determine type for uint16
#else 
#define uint16 unsigned short
#endif 
#endif

#ifndef int32 
#if (SIZEOF_INT == 4)
#define int32 int
#elif (SIZEOF_LONG == 4)
#define int32 long
#error Cannot determing tupe for int32
#endif
#endif

#ifndef uint32 
#if (SIZEOF_INT == 4)
#define uint32 unsigned int
#elif (SIZEOF_LONG == 4)
#define uint32 unsigned long
#else
#error Cannot determing tupe for uint32
#endif
#endif

#endif /* _RAIDARD_INT_H */
