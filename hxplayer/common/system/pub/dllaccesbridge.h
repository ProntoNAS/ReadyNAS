/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: dllaccesbridge.h,v 1.1.1.1 2006/03/29 16:45:36 hagi Exp $
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

#ifndef _DLLACCESBRIDGE_H_
#define _DLLACCESBRIDGE_H_

/************************************************************************
 *  Defines
 */
#if defined(HELIX_FEATURE_DLLACCESS_CLIENT)
#define DLL_ACCESSBRIDGE_IMPL	DLLAccessClient
#define DLL_ACCESSBRIDGE_OPEN(x, y, z)	open((x), (y), (z))
#include "dllaccesclient.h"
#else	// HELIX_FEATURE_DLLACCESS_CLIENT
#define DLL_ACCESSBRIDGE_IMPL	DLLAccess
#define DLL_ACCESSBRIDGE_OPEN(x, y, z)	open((x), (y))
#include "dllacces.h"
#endif	// HELIX_FEATURE_DLLACCESS_CLIENT

/************************************************************************
 *  Includes
 */
#include "hxcom.h"

class DLLAccessBridge
{
public:
    /////////////////////////////////////////////////////////////
    // Function: 
    //     DLLAccessBridge
    //
    // Parameters:
    //     None
    //
    // Returns:
    //     Nothing
    //
    // Notes:
    //     Default constructor initializes internal structures for
    //     subsequent call to open()
    //
    DLLAccessBridge()
    {
	;
    }

    /////////////////////////////////////////////////////////////
    // Function:
    //     DLLAccessBridge
    //
    // Parameters:
    //     dllName - Name of shared library
    //
    // Returns:
    //     Nothing
    //
    // Notes:
    //     Attempts to load library dllName. If unsuccessful, m_curError
    //     is set to DLLAccess::NO_LOAD and platform specific error
    //     info is stored in m_curStringError.
    //     
    DLLAccessBridge(const char* dllName, 
		    UINT16 nLibType = 0, 
		    IUnknown* pContext = NULL)
    {
	m_Impl.DLL_ACCESSBRIDGE_OPEN(dllName, nLibType, pContext);
    }

    //////////////////////////////////////////////////////////////
    // Function:
    //     ~DLLAccessBridge
    //
    // Paramters:
    //     None
    //
    // Returns:
    //     Nothing
    //
    // Notes:
    //     Unloads library from memory. See 'close' below.
    //
    ~DLLAccessBridge()
    {
	;
    }

    ///////////////////////////////////////////////////////////////
    // Function:
    //     open(const char* dllName)
    //
    // Parameters:
    //     dllName - Name of shared library
    //
    // Returns:
    //     DLLAccess::OK if successful, else DLLAccess::NO_LOAD.
    //     Platform specific error info is stored in m_curStringError.
    //
    // Notes:
    //     
    int open(const char* dllName, 
	     UINT16 nLibType = 0, 
	     IUnknown* pContext = NULL)
    {
	return m_Impl.DLL_ACCESSBRIDGE_OPEN(dllName, nLibType, pContext);
    }

    ///////////////////////////////////////////////////////////////
    // Function:
    //     close()
    //
    // Parameters:
    //     none
    //
    // Returns:
    //     DLLAccess::OK if successful, else DLL_ACCESS::NO_LOAD.
    //
    // Notes:
    //     Shared library usage is typically reference counted by the
    //     OS: the library is actually unloaded when the reference count
    //     reaches zero. Thus this call does not guarantee that the 
    //     library will be removed from memory.
    //
    int close()
    {
	return m_Impl.close();
    }


    ///////////////////////////////////////////////////////////////
    // Function:
    //     getSymbol(const char* symName)
    //
    // Parameters:
    //     symName: symbol to retrieve from shared library
    //
    // Returns:
    //     ptr to code in library if successful, else returns NULL
    //     and m_curError is set to DLLAccess::BAD_SYMBOL.
    //
    // Notes:
    //
    void* getSymbol(const char* symName)
    {
	return m_Impl.getSymbol(symName);
    }

    ///////////////////////////////////////////////////////////////
    // Function:
    //     getError
    //
    // Parameters:
    //     none
    //
    // Returns:
    //     value of m_curError
    //
    // Notes:
    //     none
    //
    int getError()
    {
	return m_Impl.getError();
    }


    ///////////////////////////////////////////////////////////////
    // Function:
    //     getErrorString
    //
    // Parameters:
    //     none
    //
    // Returns:
    //     value of m_curErrorString
    //
    // Notes:
    //     none
    //
    const char* getErrorString() 
    { 
	return m_Impl.getErrorString();
    }

    ///////////////////////////////////////////////////////////////
    // Function:
    //     getDLLName
    //
    // Parameters:
    //     none
    //
    // Returns:
    //     value of m_DLLName
    //
    // Notes:
    //     none
    //
    const char* getDLLName()
    {
	return m_Impl.getDLLName();
    }

    ///////////////////////////////////////////////////////////////
    // Function:
    //     getVersion
    //
    // Parameters:
    //     none
    //
    // Returns:
    //     value of m_Version
    //
    // Notes:
    //     none
    //
    const char* getVersion() 
    { 
	return m_Impl.getVersion();
    }

    void CreateName(const char* short_name, 
		    const char* long_name, 
		    char* out_buf,
		    UINT32& out_buf_len)
    {
	m_Impl.CreateName(short_name, 
			  long_name, 
			  out_buf, 
			  out_buf_len);
    }

    // This overloaded version has been added to allow user to specify major and minor
    // version of the DLL name you are trying to create. It will append major and minor version
    // to the name. If you use the other function, then the major and minor versions of pnmisc
    // will be used. See pnmisc.ver in that case.
    void CreateName(const char* short_name, 
		    const char* long_name, 
		    char* out_buf,
		    UINT32& out_buf_len, 
		    UINT32 nMajor, 
		    UINT32 nMinor)
    {
	m_Impl.CreateName(short_name, 
			  long_name, 
			  out_buf, 
			  out_buf_len,
			  nMajor,
			  nMinor);
    }

    ///////////////////////////////////////////////////////////////
    // Function:
    //     isOpen
    //
    // Parameters:
    //     none
    //
    // Returns:
    //     value of m_isOpen
    //
    // Notes:
    //     none
    //
    BOOL isOpen(void)
    { 
	return m_Impl.isOpen(); 
    }
    
private:
    DLL_ACCESSBRIDGE_IMPL m_Impl;
};

#endif	// _DLLACCESBRIDGE_H_
