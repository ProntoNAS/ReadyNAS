/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ihxtlogcontextobserver.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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



#ifndef _IHXTLOGCONTEXTOBSERVER_H
#define _IHXTLOGCONTEXTOBSERVER_H

#include "ihxtlogsystem.h"
#include "hxtsmartpointer.h"

/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogContextObserver
 *
 *  Purpose:
 *
 *		Extended observer interface that receives thread context information
 *
 */

// {D7BF2F2C-019A-4642-893C-9B8CB7542945}
DEFINE_GUID(IID_IHXTLogContextObserver, 
0xd7bf2f2c, 0x19a, 0x4642, 0x89, 0x3c, 0x9b, 0x8c, 0xb7, 0x54, 0x29, 0x45);

#undef INTERFACE
#define INTERFACE IHXTLogContextObserver

DECLARE_INTERFACE_(IHXTLogContextObserver, IUnknown)
{
	STDMETHOD(OnEndService) (THIS_) PURE;
	STDMETHOD(ReceiveMsg) (THIS_ const char* szNamespace, EHXTLogCode nCode, UINT32 unFuncArea, const char* szFuncArea, 
															INT32 nTimeStamp, UINT32 nLineNum,	UINT32 nMsg, const char* szMsg,	
															const char* szFilename, UINT32 nThreadId, UINT32 nContextLevel, 
															const char* szContext, const char* szThreadName) PURE; 
};

HXT_MAKE_SMART_PTR(IHXTLogContextObserver)

#endif // _IHXTLOGCONTEXTOBSERVER_H
