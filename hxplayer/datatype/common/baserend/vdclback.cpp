/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: vdclback.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

/*
 *         Class Implementation for CQTVideoRenderer::CVideoSchedulerCallback
 */

#include "hxtypes.h"
#include "hxwintyp.h"
#include "hxresult.h"
#include "hxcom.h"
#include "hxevent.h"
#include "hxcomm.h"
#include "hxengin.h"
#include "hxerror.h"

#include "hxtick.h"
#include "hxassert.h"
#include "timeval.h"
#include "vdclback.h"

CVideoSchedulerCallback::CVideoSchedulerCallback
(
    FP_CALLBACK	pFunc,
    void*	cookie,
    IUnknown*	pIUnknown,
    BOOL bUseOptimizedScheduler
)
    : m_lRefCount(0) 
    , m_pScheduler(NULL)
    , m_pFunc(pFunc)
    , m_pOptimizedScheduler(NULL)
    , m_bPendingCallback(FALSE)
    , m_bInCallback(FALSE)
    , m_PendingHandle(0)
    , m_bIsInterruptSafe(bUseOptimizedScheduler)
    , m_cookie(cookie)
{
    if (bUseOptimizedScheduler)
    {
        if (HXR_OK != pIUnknown->QueryInterface(IID_IHXOptimizedScheduler, 
				(void **) &m_pOptimizedScheduler))
        {
    	    // just for good luck
	    m_pOptimizedScheduler = NULL;
	}
    }

    if (m_pOptimizedScheduler == NULL)
    {
	if (HXR_OK != pIUnknown->QueryInterface(IID_IHXScheduler, 
				(void **) &m_pScheduler))
	{
	    m_pScheduler = NULL;
	}
    }

}

STDMETHODIMP
CVideoSchedulerCallback::Close()
{
    // the optimized scheduler has presidence so we check it first
    if (m_bPendingCallback && m_pOptimizedScheduler)
    {
	m_pOptimizedScheduler->Remove(m_PendingHandle);
	m_bPendingCallback = FALSE;
	m_PendingHandle = 0;
    }
    HX_RELEASE(m_pOptimizedScheduler);

    if (m_bPendingCallback && m_pScheduler)
    {
	m_pScheduler->Remove(m_PendingHandle);
	m_bPendingCallback = FALSE;
	m_PendingHandle = 0;
    }
    HX_RELEASE(m_pScheduler);
    return HXR_OK;
}

CVideoSchedulerCallback::~CVideoSchedulerCallback()
{
    Close();
}

STDMETHODIMP
CVideoSchedulerCallback::ScheduleCallback
(
    UINT32 ulRelativeTime, 
    UINT32 ulTicksTime
)
{
    // this new callback is going to take presidence over the
    // current one so remove it.
    if (m_pOptimizedScheduler != NULL || m_pScheduler != NULL)
    {
	IHXCallback* pCallback = NULL;

	if (HXR_OK == QueryInterface(IID_IHXCallback, (void**)&pCallback))
	{
	    HXTimeval lTime;

	    // Always use the optimized scheduler if we have one
	    if (m_pOptimizedScheduler != NULL)
	    {
		lTime = m_pOptimizedScheduler->GetCurrentSchedulerTime();
	    }
	    else
	    {
		lTime = m_pScheduler->GetCurrentSchedulerTime();
	    }

	    UINT32 ulCurrentTime = HX_GET_BETTERTICKCOUNT();
	    // if ulTicksTime is in the future, add the difference to lTime,
	    // if it's in the past, don't add anything to lTime so we schedule
	    // an imediate callback.
	    if (ulCurrentTime < ulTicksTime)
	    {
		UINT32 ulTimeDiff = CALCULATE_ELAPSED_TICKS(ulCurrentTime,
		    ulTicksTime);

		lTime.tv_usec += ulTimeDiff * MILLISECOND;
		if (lTime.tv_usec >= SECOND)
		{
		    lTime.tv_sec += lTime.tv_usec / SECOND;
		    lTime.tv_usec %= SECOND;
		}
	    }
	    
	    // we want to do the right thing here and remove a callback before we schedule a new one
	    // but we should never do that.
	    HX_ASSERT(!m_bPendingCallback);
	    if (m_bPendingCallback)
	    {
		// Always use the optimized scheduler if we have one
		if (m_pOptimizedScheduler != NULL)
		{
		    m_pOptimizedScheduler->Remove(m_PendingHandle);
		}
		else if (m_pScheduler)
		{
		    m_pScheduler->Remove(m_PendingHandle);
		}
	    }
	    
	    m_bPendingCallback = TRUE;

	    // Always use the optimized scheduler if we have one
	    if (m_pOptimizedScheduler != NULL)
	    {
	        m_PendingHandle = m_pOptimizedScheduler->AbsoluteEnter(pCallback, lTime);
	    }
	    else
	    {
	        m_PendingHandle = m_pScheduler->AbsoluteEnter(pCallback, lTime);
	    }
	}
	HX_RELEASE(pCallback);
    }

    return HXR_OK;
}

STDMETHODIMP
CVideoSchedulerCallback::Func(void)
{
    m_bInCallback = TRUE;
    if (m_pFunc != NULL )
    {
	HX_ASSERT(m_bPendingCallback);
	

	HXTimeval lTime;

	// Always use the optimized scheduler if we have one
	if (m_pOptimizedScheduler != NULL)
	{
	    lTime = m_pOptimizedScheduler->GetCurrentSchedulerTime();
	}
	else
	{
	    lTime = m_pScheduler->GetCurrentSchedulerTime();
	}

	m_PendingHandle = 0;
	m_bPendingCallback = FALSE;
	m_pFunc(m_cookie);
    }
    m_bInCallback = FALSE;
    return HXR_OK;
}

STDMETHODIMP_(BOOL)
CVideoSchedulerCallback::ShouldKickStartScheduler()
{
    return !(m_bPendingCallback || (!m_bPendingCallback && m_bInCallback));
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//      IUnknown::QueryInterface
//  Purpose:
//      Implement this to export the interfaces supported by your 
//      object.
//
STDMETHODIMP 
CVideoSchedulerCallback::QueryInterface
(
    REFIID riid, 
    void** ppvObj
)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
	AddRef();
	*ppvObj = this;
	return HXR_OK;
    }
    else if (IsEqualIID(riid, IID_IHXCallback))
    {
	AddRef();
	*ppvObj = (IHXCallback*)this;
	return HXR_OK;
    }
    else if (IsEqualIID(riid, IID_IHXInterruptSafe))
    {
	AddRef();
	*ppvObj = (IHXInterruptSafe*)this;
	return HXR_OK;
    }
    *ppvObj = NULL;
    return HXR_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//      IUnknown::AddRef
//  Purpose:
//      Everyone usually implements this the same... feel free to use
//      this implementation.
//
STDMETHODIMP_(ULONG32) 
CVideoSchedulerCallback::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//      IUnknown::Release
//  Purpose:
//      Everyone usually implements this the same... feel free to use
//      this implementation.
//
STDMETHODIMP_(ULONG32)
CVideoSchedulerCallback::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
	return m_lRefCount;
    }

    delete this;
    return 0;
}


STDMETHODIMP_(BOOL)
CVideoSchedulerCallback::IsInterruptSafe()
{
    return m_bIsInterruptSafe;
}
