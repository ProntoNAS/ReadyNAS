/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxthread.h,v 1.1.1.1 2006/03/29 16:45:36 hagi Exp $
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

#ifndef _HXTHREAD_H_
#define _HXTHREAD_H_

#include "hxtypes.h"
#include "hxresult.h"

#define ALLFS	0xFFFFFFFF

#if !defined( _WIN32 ) && !defined( _WINDOWS )
typedef void (*TIMERPROC)( void* , UINT32 , UINT32, ULONG32 );
#endif

#if defined(HELIX_FEATURE_NETWORK_USE_SELECT)
class conn;
#endif //defined(HELIX_FEATURE_NETWORK_USE_SELECT)

struct HXThreadMessage
{
    HXThreadMessage() 
	{
	    m_ulMessage		    = 0; 
	    m_pParam1		    = NULL; 
	    m_pParam2		    = NULL; 
	    m_pPlatformSpecificData = NULL;
	};

    HXThreadMessage(UINT32 ulMessage, void* pParam1, void* pParam2, 
		    void* pPlatformSpecificData = NULL)
	{
	    m_ulMessage		    = ulMessage; 
	    m_pParam1		    = pParam1; 
	    m_pParam2		    = pParam2;
	    m_pPlatformSpecificData = pPlatformSpecificData;
	};

  HXThreadMessage(HXThreadMessage* pMsg)
    : m_ulMessage(0),
      m_pParam1(NULL),
      m_pParam2(NULL),
      m_pPlatformSpecificData(NULL)
	{
	    if( pMsg )
	    {
	      m_ulMessage	    = pMsg->m_ulMessage; 
	      m_pParam1		    = pMsg->m_pParam1; 
	      m_pParam2		    = pMsg->m_pParam2;
	      m_pPlatformSpecificData = pMsg->m_pPlatformSpecificData;
	    }
	};

    UINT32  m_ulMessage;
    void*   m_pParam1;
    void*   m_pParam2;
    void*   m_pPlatformSpecificData;
};

enum
{
    HX_CREATE_SUSPENDED = 0x01
};

class HXThread
{
public:
    static  HX_RESULT	MakeThread(HXThread*& pThread);
    static  HX_RESULT	MakeStubThread(HXThread*& pThread);

    virtual		~HXThread	(void);

    virtual HX_RESULT	CreateThread
					(void* (pExecAddr(void*)), 
					 void* pArg,
					 ULONG32 ulCreationFlags = 0) = 0;

    virtual HX_RESULT	Suspend		(void) = 0;
			    
    virtual HX_RESULT	Resume		(void) = 0;

    virtual HX_RESULT	SetPriority	(UINT32 ulPriority) = 0;

    virtual HX_RESULT	GetPriority	(UINT32& ulPriority) = 0;

    virtual HX_RESULT	YieldTimeSlice	(void) = 0;

    virtual HX_RESULT	Exit		(UINT32 ulExitCode) = 0; 

    virtual HX_RESULT	GetThreadId	(UINT32& ulThreadId) = 0;

    //Returns *THE CALLING THREADS ID*, not the created thread's id.
    virtual ULONG32     GetCurrentThreadID() = 0;

    virtual HX_RESULT	PostMessage(HXThreadMessage* pMsg, void* pWindowHandle = 0) = 0;

    virtual HX_RESULT	GetMessage(HXThreadMessage* pMsg, UINT32 ulMsgFilterMix = 0, UINT32 ulMsgFilterMax = 0) = 0;

    virtual HX_RESULT	DispatchMessage(HXThreadMessage* pMsg) = 0;
#if defined(HELIX_FEATURE_NETWORK_USE_SELECT)
public:
    conn *  m_pConn;	// connection to send loopback msg
    BOOL    m_bUseReaderWriter; // set to true for network thread when local read/writer connected
    virtual void	SetNetworkMessageConnection(conn* pConn) {};
#endif // HELIX_FEATURE_NETWORK_USE_SELECT
};

class HXMutex
{
public:
    static  HX_RESULT	MakeNamedMutex	(HXMutex*& pMutex, char* pName);
    static  HX_RESULT	MakeMutex	(HXMutex*& pMutex);
    static  HX_RESULT	MakeStubMutex	(HXMutex*& pMutex);

    virtual		~HXMutex	(void) = 0;

    virtual HX_RESULT	Lock		(void) = 0;

    virtual HX_RESULT   Unlock		(void) = 0;

    virtual HX_RESULT   Trylock		(void) = 0;
};

class HXEvent
{
public:
    static  HX_RESULT	MakeEvent	(HXEvent*& pEvent, const char* pEventName = NULL, BOOL bManualReset = TRUE);
    static  HX_RESULT	MakeStubEvent	(HXEvent*& pEvent, const char* pEventName = NULL, BOOL bManualReset = TRUE);

    virtual		~HXEvent	(void) = 0;

    virtual HX_RESULT	SignalEvent	(void) = 0;

    virtual HX_RESULT	ResetEvent	(void) = 0;

    virtual void*	GetEventHandle	(void) = 0;

    virtual HX_RESULT	Wait		(UINT32 uTimeoutPeriod = ALLFS) = 0;
};



//providing async callbacks.
class HXAsyncTimer
{
public:
    // HXMSG_ASYNC_TIMER messages will be posted to
    // pReceivingThread's message queue every ulTimeOut milliseconds.
    // until KillTimer is called.
    // NOTE: under _WIN32 pReceivingThread is IGNORED and the calling
    // thread is the one to recieve the timer messages (WM_TIMER).
    static UINT32 SetTimer(ULONG32 ulTimeOut, HXThread* pReceivingThread );

    // pfExecFunc will be called every ulTimeOut milliseconds.
    static UINT32 SetTimer(ULONG32 ulTimeOut, TIMERPROC pfExecFunc );

    // ulTimeOut timer is killed/removed.
    static BOOL   KillTimer(UINT32 ulTimerID );
};



#endif /*_HXTHREAD_H_*/
