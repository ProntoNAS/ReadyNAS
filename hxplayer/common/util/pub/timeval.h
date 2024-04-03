/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: timeval.h,v 1.1.1.1 2006/03/29 16:45:32 hagi Exp $
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

#ifndef	_TIMEVAL_H_
#define	_TIMEVAL_H_

#include "hxtime.h"

#define MILLISECOND                     1000
#define SECOND                          1000000

/*
 * Because of conflicts between the use of winsock.h and winsock2.h
 * we cannot derive the struct Timeval from struct timeval on WINDOWS
 * platforms
 */

class Timeval: public HXTime {
	static void i2t(tv_sec_t& sec, tv_usec_t& usec, 
			long32 s, long32 us);
public:
	Timeval(){}
	Timeval(long32 sec, long32 usec);
	Timeval(long32 usec);
#if __MWERKS__ && !defined(_CARBON)
// I added this to make Timeval t = sendperiod compile
	Timeval(time_t sec);
#endif

#if	!(defined _UNIX && (defined _LONG_IS_64 || defined _VXWORKS))
	Timeval(int sec, int usec);
	Timeval(int usec);
#endif
	Timeval(float t);
	Timeval(double t);
	Timeval& operator +=(const int b);
	Timeval& operator -=(const int b);
	Timeval& operator +=(const Timeval& b);
	Timeval& operator -=(const Timeval& b);
	
};

#if __MWERKS__ && !defined(_CARBON)
// I added this to make Timeval t = sendperiod compile
inline Timeval::Timeval(time_t t) {
	tv_sec = (tv_sec_t)t;
	tv_usec = (tv_usec_t)0;
}
#endif

inline Timeval::Timeval(float t) {
	tv_sec = (tv_sec_t)t;
	tv_usec = (tv_usec_t)((t-tv_sec)*SECOND);
}

inline Timeval::Timeval(double t) {
	tv_sec = (tv_sec_t)t;
	tv_usec = (tv_usec_t)((t-tv_sec)*SECOND);
}

inline void Timeval::i2t(tv_sec_t& sec, tv_usec_t& usec,
                         long32 s, long32 us) {
	sec = s;
	if (us >= SECOND) {
		sec +=  us / SECOND;
		usec = us % SECOND;
	} else {
		usec = us;
	}
}

inline Timeval::Timeval(long32 s, long32 us) {
	Timeval::i2t((tv_sec_t&)tv_sec, (tv_usec_t&)tv_usec, s, us);
}

inline Timeval::Timeval(long32 us) {
	Timeval::i2t((tv_sec_t&)tv_sec, (tv_usec_t&)tv_usec, 0L, us);
}

#if	!(defined _UNIX && (defined _LONG_IS_64 || defined _VXWORKS))

inline Timeval::Timeval(int s, int us) {
	Timeval::i2t((tv_sec_t&)tv_sec, (tv_usec_t&)tv_usec, s, us);
}

inline Timeval::Timeval(int us) {
	Timeval::i2t((tv_sec_t&)tv_sec, (tv_usec_t&)tv_usec, 0L, us);
}

#endif

inline Timeval& Timeval::operator +=(const int b) {
	tv_usec += b;
	if (tv_usec >= SECOND) {
		tv_sec += tv_usec / SECOND;
		tv_usec %= SECOND;
	}
	return *this;
}

inline Timeval& Timeval::operator -=(const int b) {
	tv_usec -= b;
	if (tv_usec < 0) {
		tv_sec += tv_usec / SECOND;
		tv_usec %= SECOND;
		if (tv_usec < 0) {
			tv_sec--;
			tv_usec += SECOND;
		}
	}
	return *this;
}

/*
 * The following functions don't always work for negative 'a' or 'b'
 */

inline Timeval& Timeval::operator +=(const Timeval& b) {
	tv_sec += b.tv_sec;
	tv_usec += b.tv_usec;
	while (tv_usec >= SECOND) {
		tv_sec++;
		tv_usec -= SECOND;
	}
	return *this;
}

inline Timeval& Timeval::operator -=(const Timeval& b) {
#ifdef __hpux
	// tv_sec is u_long on HPUX
	// HP just has to do everything different doesn't it? :-(
	if (tv_sec < b.tv_sec)
	{
	    tv_sec = 0;
	    tv_usec = 0;
	    return *this;
	}
	if ((tv_sec == b.tv_sec) && (tv_usec < b.tv_usec))
	{
	    tv_sec = 0;
	    tv_usec = 0;
	    return *this;
	}
#endif
	tv_sec -= b.tv_sec;
	tv_usec -= b.tv_usec;
	while (tv_usec < 0) {
		tv_sec--;
		tv_usec += SECOND;
	}
	return *this;
}

inline Timeval operator +(const Timeval& a, const Timeval& b) {
	Timeval c;
	c.tv_sec = a.tv_sec + b.tv_sec;
	c.tv_usec = a.tv_usec + b.tv_usec;
	while (c.tv_usec >= SECOND) {
		c.tv_sec++;
		c.tv_usec -= SECOND;
	}
	return c;
}

inline Timeval operator -(const Timeval& a, const Timeval& b) {
	Timeval c;
#ifdef __hpux
	// tv_sec is u_long on HPUX
	// HP just has to do everything different doesn't it? :-(
	if (a.tv_sec < b.tv_sec)
		return (Timeval(0,0));
	if ((a.tv_sec == b.tv_sec) && (a.tv_usec < b.tv_usec))
		return (Timeval(0,0));
#endif
	c.tv_sec = a.tv_sec - b.tv_sec;
	c.tv_usec = a.tv_usec - b.tv_usec;
	while (c.tv_usec < 0) {
		c.tv_sec--;
		c.tv_usec += SECOND;
	}
	return c;
}

inline int operator ==(const Timeval& a, const Timeval& b) {
	return a.tv_usec == b.tv_usec && a.tv_sec == b.tv_sec;
}

inline int operator !=(const Timeval& a, const Timeval& b) {
	return a.tv_usec != b.tv_usec || a.tv_sec != b.tv_sec;
}

inline int operator <=(const Timeval& a, const Timeval& b) {
	return a.tv_sec < b.tv_sec ||
	       (a.tv_sec == b.tv_sec && a.tv_usec <= b.tv_usec);
}

inline int operator >=(const Timeval& a, const Timeval& b) {
	return a.tv_sec > b.tv_sec ||
	       (a.tv_sec == b.tv_sec && a.tv_usec >= b.tv_usec);
}

inline int operator <(const Timeval& a, const Timeval& b) {
	return a.tv_sec < b.tv_sec ||
	       (a.tv_sec == b.tv_sec && a.tv_usec < b.tv_usec);
}

inline int operator >(const Timeval& a, const Timeval& b) {
	return a.tv_sec > b.tv_sec ||
	       (a.tv_sec == b.tv_sec && a.tv_usec > b.tv_usec);
}

#endif/*_TIMEVAL_H_*/
