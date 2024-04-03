/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: vidrendf.h,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#ifndef __VIDRENDF_H__
#define __VIDRENDF_H__

/****************************************************************************
 *  Includes
 */
#include "hxtypes.h"
#include "hxcom.h"
#include "hxslist.h"
#include "hxwintyp.h"
#include "hxcomm.h"
#include "ihxpckts.h"
#include "hxthread.h"
#include "hxvsurf.h"
#include "chxbufpl.h"
#include "cringbuf.h"
#include "mdpkt.h"


/****************************************************************************
 *  Globals
 */
class CVideoRenderer;


/****************************************************************************
 *  CVideoFormat
 */
class CVideoFormat
{
public:
    /*
     *	Constructor/Destructor
     */
    CVideoFormat(IHXCommonClassFactory* pCommonClassFactory,
		 CVideoRenderer* pVideoRenderer);

    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj);

    STDMETHOD_(ULONG32,AddRef)	(THIS);

    STDMETHOD_(ULONG32,Release)	(THIS);

    /*
     *	Public but Fixed Core functionality - not deriveable
     */
    BOOL Enqueue(IHXPacket* pCodecData);
    void ReturnAssembledPacket(CMediaPacket* pFramePacket);
    HX_RESULT Requeue(CMediaPacket* pFramePacket);
    CMediaPacket* Dequeue(void);
    BOOL DecodeFrame(void);
    BOOL ReturnDecodedPacket(CMediaPacket* pDecodedPacket);

    void OnRawPacketsEnded(void)    { m_bRawPacketsDone = TRUE; }

    BOOL GetNextFrameTime(UINT32 &ulTime)
    {
	BOOL bRetVal = FALSE;
	CMediaPacket* pFrame = PeekOutQueueHeadPacket();
	
	if (pFrame)
	{
	    ulTime = pFrame->m_ulTime;
	    bRetVal = TRUE;
	}
	
	return bRetVal;
    }

    BOOL IsNextFrameSkippable(BOOL &bIsSkippable)
    {
	BOOL bRetVal = FALSE;
	CMediaPacket* pFrame = PeekOutQueueHeadPacket();
	
	if (pFrame)
	{
	    bIsSkippable = IsFrameSkippable(pFrame);
	    bRetVal = TRUE;
	}
	
	return bRetVal;
    }

    CHXBufferPool* GetFramePool(void)	{ return m_pFramePool; }
    virtual void OnDecodedPacketRelease(CMediaPacket* &pPacket);

    ULONG32 GetAssembledPacketQueueDepth(void)	{ return m_InputQueue.GetCount(); }

    ULONG32 GetDecodedFrameQueueDepth(void) { return m_pOutputQueue->Count(); }
    void AdjustDecodedFrameQueueCapacity(LONG32 ulMaxCount)
    {
	m_pOutputQueue->SetMaxCount(ulMaxCount);
    }

    void SetStartTime(ULONG32 ulTime)	{ m_ulStartTime = ulTime; }
    ULONG32 GetStartTime(void)		{ return m_ulStartTime; }

    void SuspendDecode(BOOL bSuspend)	{ m_bDecodeSuspended = bSuspend; }
    BOOL IsDecodeSuspended(void)	{ return m_bDecodeSuspended; }

    BOOL AreRawPacketsDone(void)	{ return m_bRawPacketsDone; }


    /*
     *	Public and Customizable functionality - derive to customize
     */
    virtual HX_RESULT Init(IHXValues* pHeader);

    virtual BOOL IsFrameSkippable(CMediaPacket* pFrame)
    {
	return (pFrame->m_pSampleDesc == NULL);
    }

    virtual void Reset(void);

    virtual HX_RESULT InitBitmapInfoHeader(HXBitmapInfoHeader &BitmapInfoHeader,
					   CMediaPacket* pVideoPacket);
    
    virtual BOOL IsBitmapFormatChanged(HXBitmapInfoHeader &BitmapInfoHeader,
				       CMediaPacket* pVideoPacket)
    {
	return (pVideoPacket->m_pSampleDesc != NULL);
    }

    virtual ULONG32 GetDefaultPreroll(IHXValues* pValues);

    virtual ULONG32 GetMinimumPreroll(IHXValues* pValues);

    virtual ULONG32 GetMaximumPreroll(IHXValues* pValues);

    virtual const char** GetMimeTypes(void);

    virtual ULONG32 GetMaxDecodedFrames(void);

    HX_RESULT GetLastError();

protected:
    virtual ~CVideoFormat();

    /*
     *	Protected but Customizable functionality - derive to customize
     */
    virtual CHXBufferPool* CreateBufferPool(void);
    virtual CMediaPacket* CreateAssembledPacket(IHXPacket* pPayloadData);
    virtual CMediaPacket* CreateDecodedPacket(CMediaPacket* pCodedPacket);

    /*
     *	Usable from the derived renderer format
     */
    IHXCommonClassFactory*	m_pCommonClassFactory;
    IHXValues*			m_pHeader;
    CHXBufferPool*		m_pFramePool;
    HX_RESULT			m_LastError;

private:
    /*
     *	Private and Fixed functionality
     */
    void _Reset(void);

    CMediaPacket* PeekOutQueueHeadPacket(void)
    {
	return (CMediaPacket*) m_pOutputQueue->Peek();
    }

    void FlushOutputQueue(void);

    /*
     *	Controls of the renderer format packet pump
     */
    HXMutex*			m_pMutex;
    HXMutex*			m_pAssemblerMutex;
    HXMutex*			m_pDecoderMutex;
    CHXSimpleList		m_InputQueue;
    CRingBuffer*		m_pOutputQueue;
    LONG32			m_lMaxBufferedDecodedFrames;
    ULONG32			m_ulStartTime;
    BOOL			m_bDecodeSuspended;
    BOOL			m_bRawPacketsDone;

    CVideoRenderer*		m_pVideoRenderer;

    LONG32			m_lRefCount;
};

#endif	// __VIDRENDF_H__
