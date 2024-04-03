/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: winthrd.cpp,v 1.1.1.1 2006/03/29 16:45:36 hagi Exp $
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

#include "hxtypes.h"
#include "hlxclib/windows.h"
#if defined(_WIN32) && !defined(WIN32_PLATFORM_PSPC)
#undef _MT
#define _MT
#include <process.h>
#endif /*_WIN32*/

#include "hxresult.h"
#include "hxassert.h"

#include "hxthread.h"
#include "hxmsgs.h"
#include "winthrd.h"
#include "conn.h"

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif									 

#define ALLFS	0xFFFFFFFF

#if defined( WIN32_PLATFORM_PSPC )
#define _beginthreadex(a1, a2, a3, a4, a5, a6) (ULONG32) ::CreateThread(a1, a2, (unsigned long(__stdcall *)(void*)) a3, a4, a5, (unsigned long*) a6)
#define _endthreadex ::ExitThread
#endif

HXWinThread::HXWinThread	(void)
    : m_ThreadHandle(0)
    , m_ulThreadId(0)
    , m_ulFlags(0)
{
#if defined(HELIX_FEATURE_NETWORK_USE_SELECT)
    m_bUseReaderWriter = FALSE;
    m_pConn = NULL;
#endif //defined(HELIX_FEATURE_NETWORK_USE_SELECT)
}

HXWinThread::~HXWinThread	(void)
{
    Exit(0);
}


HX_RESULT	
HXWinThread::CreateThread(void* (pExecAddr(void*)), void* pArg, ULONG32 ulCreationFlags)
{
#ifdef _WIN32
    if (m_ArgsAndAddr.m_pExecAddr)
    {
	return HXR_UNEXPECTED;
    }

    if (!pExecAddr)
    {
	return HXR_INVALID_PARAMETER;
    }

    m_ArgsAndAddr.m_pExecAddr	= pExecAddr;
    m_ArgsAndAddr.m_pArg	= pArg;

    if (ulCreationFlags & HX_CREATE_SUSPENDED)
    {
	m_ulFlags = CREATE_SUSPENDED;
    }

    if (!ThreadCreated())
    {
	return HXR_FAIL;
    }

#endif
    return HXR_OK;
}


HX_RESULT	
HXWinThread::Suspend		(void)
{
#ifdef _WIN32
    HX_ASSERT(m_ThreadHandle);

    if (::SuspendThread((HANDLE) m_ThreadHandle) == ALLFS)
    {
	return HXR_FAIL;
    }

#endif
    return HXR_OK;
}
			    
HX_RESULT	
HXWinThread::Resume		(void)
{
#ifdef _WIN32
    HX_ASSERT(m_ThreadHandle);

    if (::ResumeThread((HANDLE) m_ThreadHandle) == ALLFS)
    {
	return HXR_FAIL;
    }

#endif
    return HXR_OK;
}

HX_RESULT	
HXWinThread::SetPriority	(UINT32 ulPriority)
{
#ifdef _WIN32
    HX_ASSERT(m_ThreadHandle);

    if (::SetThreadPriority((HANDLE) m_ThreadHandle, ulPriority) == FALSE)
    {
	return HXR_FAIL;
    }

#endif
    return HXR_OK;
}

HX_RESULT	
HXWinThread::GetPriority	(UINT32& ulPriority)
{
#ifdef _WIN32
    HX_ASSERT(m_ThreadHandle);

    if ((ulPriority = ::GetThreadPriority((HANDLE) m_ThreadHandle)) == 
	    THREAD_PRIORITY_ERROR_RETURN)
    {
	return HXR_FAIL;
    }

#endif
    return HXR_OK;
}

HX_RESULT	
HXWinThread::YieldTimeSlice	(void)
{
#ifdef _WIN32
//    HX_ASSERT(m_ThreadHandle);

    ::Sleep(0);

#endif
    return HXR_OK;
}

HX_RESULT	
HXWinThread::Exit  (UINT32 ulExitCode)
{
#ifdef _WIN32
    if (!m_ThreadHandle)
    {
	return HXR_UNEXPECTED;
    }

    // Do not allow us to exit until this thread terminates
    if (GetCurrentThreadId() != m_ulThreadId)
    {
	WaitForSingleObject((HANDLE)m_ThreadHandle, INFINITE);
    }

    CloseHandle((HANDLE) m_ThreadHandle);
    m_ThreadHandle  = 0;
    m_ulThreadId    = 0;

#endif
    return HXR_OK;
}

HX_RESULT	
HXWinThread::GetThreadId	(UINT32& ulThreadId)
{
#ifdef _WIN32
    HX_ASSERT(m_ThreadHandle);

    ulThreadId = m_ulThreadId; 
#endif
    return HXR_OK;
}

ULONG32 HXWinThread::GetCurrentThreadID()
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#endif
    return 0;
}

#if defined(HELIX_FEATURE_NETWORK_USE_SELECT)
void 
HXWinThread::SetNetworkMessageConnection(conn* pConn)
{
    m_pConn = pConn;
}

HX_RESULT	
HXWinThread::PostNetworkMessage(HXThreadMessage* pMsg)
{
    if (!m_pConn || !m_pConn->get_callback())
	return HXR_FAIL;

    m_pConn->get_callback()->Func(SEND_BUFFER_NOTIFICATION, (BOOL)pMsg, (conn *)m_pConn);
//    m_pConn->get_callback()->Func(WRITE_NOTIFICATION, TRUE, (conn *)m_pConn);

    return HXR_OK;
}
#endif //defined(HELIX_FEATURE_NETWORK_USE_SELECT)


HX_RESULT	
HXWinThread::PostMessage(HXThreadMessage* pMsg, void* pWindowHandle)
{
    BOOL    bResult	= TRUE;

#if defined(HELIX_FEATURE_NETWORK_USE_SELECT)
	if (m_bUseReaderWriter && m_pConn)
	{
	    return PostNetworkMessage(pMsg);
	}
#endif //defined(HELIX_FEATURE_NETWORK_USE_SELECT)

#ifdef _WIN32
    if (pWindowHandle)
    {
	bResult = ::PostMessage((HWND)pWindowHandle, pMsg->m_ulMessage, 
	    (WPARAM) pMsg->m_pParam1, (LPARAM) pMsg->m_pParam2);
    }
    else 
    {
	if(!m_ThreadHandle)
	{
	    return HXR_NOT_INITIALIZED;
	}
	/*
	 * Must wait until this thread has a message queue
	 */
	while(!m_ArgsAndAddr.m_bThreadCanReceiveMessages)
	{
	    Sleep(0);
	}
	bResult = ::PostThreadMessage(m_ulThreadId, pMsg->m_ulMessage, 
	    (WPARAM) pMsg->m_pParam1, (LPARAM) pMsg->m_pParam2);
    }

    if (!bResult)
    {
	UINT32 ulErrorCode = ::GetLastError();
	ulErrorCode += 1;
    }
#endif
    return bResult ? HXR_OK : HXR_FAIL;
}

HX_RESULT	
HXWinThread::GetMessage(HXThreadMessage* pMsg, UINT32 ulMsgFilterMix, UINT32 ulMsgFilterMax)
{
#ifdef _WIN32
    MSG msg;
    if (::GetMessage(&msg, NULL, ulMsgFilterMix, ulMsgFilterMax) == TRUE)
    {
	pMsg->m_ulMessage = msg.message;
	pMsg->m_pParam1	= (void*) msg.wParam;
	pMsg->m_pParam2	= (void*) msg.lParam;
	pMsg->m_pPlatformSpecificData	= (void*) msg.hwnd;
	return HXR_OK;
    }
    else
    {
	return HXR_FAIL;
    }
#endif 
    return HXR_OK;
}

HX_RESULT	
HXWinThread::DispatchMessage(HXThreadMessage* pMsg)
{
#ifdef _WIN32
    MSG msg;
    msg.message = pMsg->m_ulMessage;
    msg.wParam	= (WPARAM) pMsg->m_pParam1;
    msg.lParam	= (LPARAM) pMsg->m_pParam2;
    msg.hwnd	= (HWND)   pMsg->m_pPlatformSpecificData;
    ::DispatchMessage(&msg);
#endif 
    return HXR_OK;
}

unsigned int 
HXWinThread::HXWinThreadStartRoutine(void* p)
{
#ifdef _WIN32
    MSG msg;
    HXWinThreadArgsAndAddr* pArgsAndAddr = (HXWinThreadArgsAndAddr *)p;
    /*
     * Need to call this to make sure our thread creates his message queue.
     */
    PeekMessage(&msg,  NULL, WM_USER, WM_USER, PM_NOREMOVE);
    pArgsAndAddr->m_bThreadCanReceiveMessages = TRUE;
    
    UINT32 ulReturnValue = ((LPTHREAD_START_ROUTINE)pArgsAndAddr->m_pExecAddr)(
	pArgsAndAddr->m_pArg);

    _endthreadex(ulReturnValue);
    return ulReturnValue;
#else
    return 0;
#endif
}

BOOL	
HXWinThread::ThreadCreated()
{
#ifdef _WIN32
    if (!m_ThreadHandle)
    {

	m_ThreadHandle = _beginthreadex( NULL, 0, HXWinThreadStartRoutine,
	    (void*) &m_ArgsAndAddr, m_ulFlags, &m_ulThreadId );
    }

    if (!m_ThreadHandle)
    {
	return FALSE;
    }

#endif
    return TRUE;
}


//===========================================================================

HXWinMutex::HXWinMutex    (void)
{
#ifdef _WIN32
    ::InitializeCriticalSection(&m_Mutex);
#endif
}

HXWinMutex::~HXWinMutex   (void)
{
#ifdef _WIN32
    ::DeleteCriticalSection(&m_Mutex);
#endif
}


HX_RESULT	
HXWinMutex::Lock	    (void)
{
#ifdef _WIN32
    ::EnterCriticalSection(&m_Mutex); 
#endif
    return HXR_OK;
}

HX_RESULT   
HXWinMutex::Unlock	    (void)
{
#ifdef _WIN32
    ::LeaveCriticalSection(&m_Mutex); 
#endif
    return HXR_OK;
}

HX_RESULT   
HXWinMutex::Trylock	    (void)
{
#ifdef _WIN32
#if(_WIN32_WINNT >= 0x0400)
    if (::TryEnterCriticalSection(&m_Mutex))
    {
	return HXR_OK;
    }
    else
    {
	return HXR_FAIL;
    }
#endif /*(_WIN32_WINNT >= 0x0400)*/
#endif
    return HXR_NOTIMPL;
}

//===========================================================================

HXWinNamedMutex::HXWinNamedMutex    (char* name)
{
#ifdef _WIN32
    m_Mutex = ::CreateMutex(NULL, FALSE, OS_STRING(name));
#endif
}

HXWinNamedMutex::~HXWinNamedMutex   (void)
{
#ifdef _WIN32
    ::CloseHandle(m_Mutex);
#endif
}


HX_RESULT	
HXWinNamedMutex::Lock	    (void)
{
#ifdef _WIN32
    ::WaitForSingleObject(m_Mutex, INFINITE);
#endif
    return HXR_OK;
}

HX_RESULT   
HXWinNamedMutex::Unlock	    (void)
{
#ifdef _WIN32
    ::ReleaseMutex(m_Mutex);
#endif
    return HXR_OK;
}

HX_RESULT   
HXWinNamedMutex::Trylock	    (void)
{
#ifdef _WIN32
#if(_WIN32_WINNT >= 0x0400)
    if (::WaitForSingleObject(0) == WAIT_OBJECT_0)
    {
	return HXR_OK;
    }
    else
    {
	return HXR_FAIL;
    }
#endif /*(_WIN32_WINNT >= 0x0400)*/
#endif
    return HXR_NOTIMPL;
}



HXWinEvent::HXWinEvent(const char* pEventName, BOOL bManualReset)
{
#ifdef _WIN32
    m_Handle = ::CreateEvent(NULL, bManualReset, FALSE, OS_STRING(pEventName));
    HX_ASSERT(m_Handle != NULL);
#endif /*_WIN32*/
}

HXWinEvent::~HXWinEvent(void)
{
#ifdef _WIN32
    if (m_Handle)
    {
	::CloseHandle(m_Handle);
    }
#endif /*_WIN32*/
}

HX_RESULT	
HXWinEvent::SignalEvent(void)
{
#ifdef _WIN32
    if (::SetEvent(m_Handle) == TRUE)
    {
	return HXR_OK;
    }
    else
    {
	return HXR_FAIL;
    }
#else
    return HXR_UNEXPECTED;
#endif /*_WIN32*/
}

HX_RESULT	
HXWinEvent::ResetEvent(void)
{
#ifdef _WIN32
    if (::ResetEvent(m_Handle) == TRUE)
    {
	return HXR_OK;
    }
    else
    {
	return HXR_FAIL;
    }
#else
    return HXR_UNEXPECTED;
#endif /*_WIN32*/
}

void*
HXWinEvent::GetEventHandle(void)
{  
    return (void*) m_Handle;
}


HX_RESULT	
HXWinEvent::Wait(UINT32 uTimeoutPeriod)
{
#ifdef _WIN32
    if (uTimeoutPeriod == ALLFS)
    {
	uTimeoutPeriod = INFINITE;
    }

    UINT32 ulReturnVal = ::WaitForSingleObject(m_Handle, uTimeoutPeriod);

    if (ulReturnVal == WAIT_OBJECT_0)
    {
	return HXR_OK;
    }
    else if ( ulReturnVal == WAIT_TIMEOUT )
    {
        return HXR_WOULD_BLOCK;
    }
    else
    {
	return HXR_FAIL;
    }
#else
    return HXR_UNEXPECTED;
#endif /*_WIN32*/
}

