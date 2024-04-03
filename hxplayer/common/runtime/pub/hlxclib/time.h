/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: time.h,v 1.1.1.1 2006/03/29 16:45:35 hagi Exp $
 * 
 * Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the RealNetworks Public
 * Source License (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the current version of the RealNetworks Community
 * Source License (the "RCSL") available at
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL
 * will apply. You may also obtain the license terms directly from
 * RealNetworks.  You may not use this file except in compliance with
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for
 * the rights, obligations and limitations governing use of the
 * contents of the file.
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 or later (the
 * "GPL") in which case the provisions of the GPL are applicable
 * instead of those above. If you wish to allow use of your version of
 * this file only under the terms of the GPL, and not to allow others
 * to use your version of this file under the terms of either the RPSL
 * or RCSL, indicate your decision by deleting the provisions above
 * and replace them with the notice and other provisions required by
 * the GPL. If you do not delete the provisions above, a recipient may
 * use your version of this file under the terms of any one of the
 * RPSL, the RCSL or the GPL.
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the
 * portions it created.
 * 
 * This file, and the files included with this file, is distributed
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#ifndef HLXSYS_TIME_H
#define HLXSYS_TIME_H

#if defined(_SYMBIAN)
# include <sys/time.h>
#endif 

#if !defined(WIN32_PLATFORM_PSPC) && !defined(_OPENWAVE)
#include <time.h>
#endif /* !defined(WIN32_PLATFORM_PSPC) && !defined(_OPENWAVE) */

#if defined(_OPENWAVE)
#include "platform/openwave/hx_op_timeutil.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************
 * Types
 */

#if defined(_OPENWAVE)

#define NO_TM_ISDST
typedef U32 time_t;
#define tm op_tm                // XXXSAB any other way for 'struct tm' to
                                // work in a C-includeable file?
struct timeval {
	time_t tv_sec;
	time_t tv_usec;
};


#elif defined(WIN32_PLATFORM_PSPC)
#include "hxtypes.h"
#include <windows.h>


struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

#define timezone _timezone
extern long _timezone;

#endif /* defined(WIN32_PLATFORM_PSPC) */


/*******************************
 * Helix declarations
 */
long __helix_time(long *t);
struct tm* __helix_localtime(long* timep);
void __helix_tzset();
long __helix_mktime(struct tm* tm);
struct tm *__helix_gmtime(long *timep);
int __helix_gettimeofday(struct timeval *tv, void *tz);
char * __helix_ctime(long *timer);

#if defined(_WINCE)
char * __helix_asctime (struct tm *tm);

/*******************************
 * platform specifics declarations
 */

_inline char * ctime(time_t *timp)
{
    return __helix_ctime((long*)timp);
}

_inline char * asctime (struct tm *tm)
{
    return __helix_asctime(tm);
}

_inline
void _tzset()
{
    __helix_tzset();
}

_inline
struct tm* localtime(time_t* timep)
{
    return __helix_localtime((long *)timep);
}

_inline
long time(time_t *t) 
{
    return __helix_time((long *)t);
}


_inline
long mktime(struct tm* tm)
{
    return __helix_mktime(tm);
}

_inline
struct tm* gmtime(time_t *timep)
{
    return __helix_gmtime((long*)timep);
}

#elif defined(_OPENWAVE)
#define time(t)			__helix_time(t)
#define ctime(t)		__helix_ctime(t)
#define gmtime(t)		__helix_gmtime(t)
#define localtime(t)	__helix_gmtime(t) // XXXSAB is there a _local_ time call?
#define mktime(tm)		__helix_mktime(tm)
#define gettimeofday	__helix_gettimeofday

#define strftime op_strftime

#endif /* defined(WIN32_PLATFORM_PSPC) */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* HLXSYS_TIME_H */
