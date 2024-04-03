/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ihxcookies2.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef _ihxcookies2_h_
#define _ihxcookies2_h_

/*
 * Forward declarations of some interfaces defined or used here-in.
 */
typedef _INTERFACE	IHXBuffer			IHXBuffer;

/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXCookies2
 *
 *  Purpose:
 *	Interface to manage Cookies database
 *
 *  IID_IHXCookies2:
 *
 *	{5634537F-162F-41b0-93FC-6E0F77107181}
 *
 */
DEFINE_GUID(IID_IHXCookies2, 0x5634537f, 0x162f, 0x41b0, 0x93, 0xfc, 0x6e, 0xf, 0x77, 0x10, 0x71, 0x81);

DECLARE_INTERFACE_(IHXCookies2, IUnknown)
{
    /*
     * IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     * IRPCookies methods
     */

    /************************************************************************
     *	Method:
     *	    IHXCookies2::GetExpiredCookies
     *	Purpose:
     *	    Get expired cookies
     */
    STDMETHOD(GetExpiredCookies)(THIS_
				 const char*	    pHost,
				 const char*	    pPath,
				 REF(IHXBuffer*)   pCookies) PURE;
    /************************************************************************
     *	Method:
     *	    IHXCookies2::GetCookies
     *	Purpose:
     *	    Get cookies
     */
    STDMETHOD(GetCookies)	(THIS_
				 const char*	    pHost,
				 const char*	    pPath,
				 REF(IHXBuffer*)   pCookies,
				 REF(IHXBuffer*)   pPlayerCookies) PURE;

};

#endif //#ifndef _ihxcookies2_h_
