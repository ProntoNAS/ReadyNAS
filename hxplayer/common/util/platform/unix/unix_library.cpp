/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: unix_library.cpp,v 1.1.1.1 2006/03/29 16:45:31 hagi Exp $
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

////////////////////
//
//	This file contains functions to load & release Shared Libraries.
//	It is used primarily by the codec loader/unloader code.
//

#include "unix_library.h"

#include "hxtypes.h"
#include "hxstring.h"
#include "carray.h"

#include "dllacces.h"

//
// CSharedLibMgr is a class which manages the mapping between the HMODULE
// and the DLLAccess object which is used to make the actual OS calls to
// open, close, etc. the library.
//

class CSharedLibMgr
{
	public:
		static ULONG32 	LoadLibrary 	(const char *libname);
		static void 	FreeLibrary 	(HMODULE lib);
		static void *	GetProcAddress	(HMODULE lib, char *funcName);

	private:
		DLLAccess	*dll;

		static CHXPtrArray mLibList;
		static DLLAccess * GetLib (ULONG32 index);
};

CHXPtrArray CSharedLibMgr::mLibList;

ULONG32 CSharedLibMgr::LoadLibrary (const char *libName)
{
	DLLAccess *pAccess = NULL;
	ULONG32 libIndex = DL_BAD_LIB_ID;

	if (libName == NULL)
		return DL_BAD_LIB_ID;

	pAccess = new DLLAccess;
	HX_ASSERT (pAccess);

	if (pAccess->open (libName) == DLLAccess::DLL_OK)
	{
		libIndex = mLibList.Add (pAccess);
	}
	else
	{
		delete pAccess;
	}

	return libIndex;
}

void CSharedLibMgr::FreeLibrary (HMODULE libHandle)
{
	if (libHandle == DL_BAD_LIB_ID)
		return;

	DLLAccess *pAccess = GetLib (libHandle);

	if (pAccess != NULL)
	{
		pAccess->close();
		delete pAccess;

		mLibList.SetAt (libHandle, NULL);
	}
}

void *CSharedLibMgr::GetProcAddress (HMODULE libHandle, char *funcName)
{
	if (libHandle == DL_BAD_LIB_ID || funcName == NULL)
		return NULL;

	DLLAccess *pAccess = GetLib (libHandle);
	HX_ASSERT (pAccess);

	return (pAccess->getSymbol (funcName));
}

DLLAccess * CSharedLibMgr::GetLib (ULONG32 index)
{
	if (index == DL_BAD_LIB_ID)
		return NULL;

	return ((DLLAccess *)mLibList.GetAt (index));
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		LoadLibrary()
//
//	Purpose:
//
//		Called to load a Shared Library given the library's name.
//
//	Parameters:
//
//		char* dllname
//		The name of the library.
//
//	Return:
//
//		ULONG32
//		Returns a handle to the library.
//
ULONG32 LoadLibrary(const char* dllname)
{
	return CSharedLibMgr::LoadLibrary (dllname);
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		FreeLibrary()
//
//	Purpose:
//
//		Called to free a Shared Library. If this is not called the library
//		will be freed when the application quits.
//
//	Parameters:
//
//		HMODULE lib
//
//	Return:
//
//		none
//
void FreeLibrary(HMODULE lib)
{
	CSharedLibMgr::FreeLibrary (lib);
}

/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		GetProcAddress()
//
//	Purpose:
//
//		Called to get a function pointer in a Shared Library.
//
//	Parameters:
//
//		HMODULE lib
//
//		char* function
//		The function name
//
//	Return:
//
//		void*
//		The address of the function.
//
void* GetProcAddress(HMODULE lib, char* function)
{
	return CSharedLibMgr::GetProcAddress (lib, function);
}


