/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: xmlvalid.h,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#ifndef _XMLVALID_H_
#define _XMLVALID_H_

#define BETWEEN(c, l, h)  ( c >= l && c <= h )

class ISO8859Valid
{
public:
    static BOOL IsNameValid(const BYTE* p, UINT32 len);
    static BOOL IsNmtokenValid(const BYTE*p, UINT32 len);
    static BOOL IsEntityValueValid(const BYTE* p, UINT32 len);
    static BOOL IsAttValueValid(const BYTE* p, UINT32 len);
    static BOOL IsSystemLiteralValid(const BYTE* p, UINT32 len);
    static BOOL IsPubidLiteralValid(const BYTE* p, UINT32 len);
    static BOOL IsRefValid(const BYTE* p, UINT32 len);

    static inline BOOL IsLetter(const BYTE c);
    static inline BOOL IsBaseChar(const BYTE c);
    static inline BOOL IsIdeographic(const BYTE c);
    static inline BOOL IsCombiningChar(const BYTE c);
    static inline BOOL IsDigit(const BYTE c);
    static inline BOOL IsExtender(const BYTE c);
};

class UTF16Valid
{
public:
    static BOOL IsNameValid(const UINT16* p, UINT32 len);
    static BOOL IsNmtokenValid(const UINT16*p, UINT32 len);

    // these four functions assume that the ' or " deliminators have
    // allready been parsed.
    static BOOL IsEntityValueValid(const UINT16* p, UINT32 len);
    static BOOL IsAttValueValid(const UINT16* p, UINT32 len);
    static BOOL IsSystemLiteralValid(const UINT16* p, UINT32 len);
    static BOOL IsPubidLiteralValid(const UINT16* p, UINT32 len);

    static inline BOOL IsLetter(const UINT16 c);
    static inline BOOL IsBaseChar(const UINT16 c);
    static inline BOOL IsIdeographic(const UINT16 c);
    static inline BOOL IsCombiningChar(const UINT16 c);
    static inline BOOL IsDigit(const UINT16 c);
    static inline BOOL IsExtender(const UINT16 c);
};

#endif // _XMLVALID_H_
