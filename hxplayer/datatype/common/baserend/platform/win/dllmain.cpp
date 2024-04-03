/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: dllmain.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

///////////////
/////////////////////////////////////////////////////////////////////////////
//  DLLMAIN.CPP
//
//  Copyright (C) 1994,1995,1996,1997,1998 Progressive Networks.
//  All rights reserved.
//
//
//
//

#include <windows.h>

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static char HX_THIS_FILE[] = __FILE__;
#endif

HINSTANCE g_hInstance = NULL;

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//
//	InitInstance()
//
//  Purpose:
//
//	Performs any per-DLL initialization. Called in 16bit or 32bit case.
//
BOOL InitInstance(HINSTANCE hDLL)
{
    g_hInstance = hDLL;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//
//	ExitInstance()
//
//  Purpose:
//
//	Performs any per-DLL cleanup. Called in 16bit or 32bit case.
//
int ExitInstance() 
{
    return 0;
}

#ifdef _WIN32
/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//
//	DllMain()
//
//  Purpose:
//
//	Standard 32bit DLL entry point. Handles initialization and cleanup
//	of DLL instances. Only called in 32bit case.
//
extern "C" BOOL WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
	case DLL_PROCESS_ATTACH:
	{
	    //
	    // DLL is attaching to the address space of the current process.
	    //
	    InitInstance(hDLL);
	}
	break;

	case DLL_THREAD_ATTACH:
	{
	    //
	    // A new thread is being created in the current process.
	    //
	}
	break;

	case DLL_THREAD_DETACH:
	{	
	    //
	    // A thread is exiting cleanly.
	    //
	}
	break;

	case DLL_PROCESS_DETACH:
	{
	    //
	    // The calling process is detaching the DLL from its address space.
	    //
	    ExitInstance();
	}
	break;
    }

    return TRUE;
}

#else
			       
/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//
//  	LibMain()
//
//  Purpose:
//
//	Standard 16bit DLL entry point. Handles initialization of DLL 
//	instances. Only called in 16bit case.
//
extern "C" HANDLE WINAPI LibMain(HANDLE hInstance, WORD wDataSeg, WORD cbHeapSize, LPSTR lpCmdLine)
{
    if (InitInstance((HINSTANCE)hInstance))
    {
	if (0!=cbHeapSize)
	{
	    UnlockData(0);
	}
    }
    return hInstance;
}

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//
//  	_WEP()
//
//  Purpose:
//
//	Standard 16bit DLL entry point. Handles clean up of DLL 
//	instances. Only called in 16bit case.
//
extern "C" int WINAPI _WEP (int bSystemExit);
#pragma alloc_text(FIXEDSEG, _WEP)
extern "C" int WINAPI _WEP (int bSystemExit)
{
    ExitInstance();
    return(1);
}

#endif

