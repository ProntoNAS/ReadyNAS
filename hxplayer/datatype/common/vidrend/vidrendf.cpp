/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: vidrendf.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#ifdef ENABLE_TRACE
#define HX_TRACE_THINGY(x, m, l)					\
    {							\
	FILE* f1;					\
	f1 = ::fopen(x, "a+");				\
	(f1)?(::fprintf(f1, "%ld - %s = %ld \n", HX_GET_BETTERTICKCOUNT(), m, l), ::fclose(f1)):(0);\
    }
#define STAMPBUF(x, m)					\
    {							\
	FILE* f1;					\
	f1 = ::fopen(x, "a+");				\
	(f1)?(::fprintf(f1, "%ld - %s\n", HX_GET_BETTERTICKCOUNT(), m), ::fclose(f1)):(0);\
    }
#else	// ENABLE_TRACE
#define HX_TRACE_THINGY(x, m, l)

#ifdef MEASURE_PERF
#define LOGBUFFSIZE 100000
char textmem[LOGBUFFSIZE];
char *textptr = textmem;
#define STAMPBUF(x, txt) { if (textptr - textmem < LOGBUFFSIZE) textptr += SafeSprintf(textptr, LOGBUFFSIZE - (textptr - textmem), "%s: %ld*\n", txt, HX_GET_BETTERTICKCOUNT()); }
#else	// MEASURE_PERF
#define STAMPBUF(x, txt)
#endif	// MEASURE_PERF

#endif	// ENABLE_TRACE

/****************************************************************************
 *  Defines
 */
#if !defined(HELIX_FEATURE_MIN_HEAP)
#define MAX_BUFFERED_DECODED_FRAMES		12
#else	// HELIX_FEATURE_MIN_HEAP
#define MAX_BUFFERED_DECODED_FRAMES		2
#endif	// HELIX_FEATURE_MIN_HEAP

#if defined(HELIX_FEATURE_MIN_HEAP)
#define FORMAT_MINIMUM_PREROLL			1000
#define FORMAT_DEFAULT_PREROLL			2000
#define FORMAT_MAXIMUM_PREROLL			3000
#else
#define FORMAT_MINIMUM_PREROLL			3000
#define FORMAT_DEFAULT_PREROLL			5000
#define FORMAT_MAXIMUM_PREROLL			15000
#endif

#define MAX_DECODE_SPIN				100


/****************************************************************************
 *  Includes
 */
#include "vidrendf.h"
#include "vidrend.h"
#include "hxtick.h"
#include "hxassert.h"
#include "hxstrutl.h"


/****************************************************************************
 *  Debug
 */
#ifdef ENABLE_FRAME_TRACE
#define MAX_FRAME_TRACE_ENTRIES	100000
ULONG32 ulFrameTraceIdx = 0;
LONG32 frameTraceArray[MAX_FRAME_TRACE_ENTRIES][3];

void DumpFrameEntries(void)
{
    FILE* pFile = NULL;
    ULONG32 ulIdx;

    if (ulFrameTraceIdx > 0)
    {
	pFile = fopen("c:\\buf.txt", "wb");
    }

    if (pFile)
    {
	for (ulIdx = 0; ulIdx < ulFrameTraceIdx; ulIdx++)
	{
	    fprintf(pFile, "%c(%d) = %x\n", (char) frameTraceArray[ulIdx][1], 
					   frameTraceArray[ulIdx][2], 
					   frameTraceArray[ulIdx][0]);
	}

	fclose(pFile);
    }

    ulFrameTraceIdx = 0;
}
#endif	// ENABLE_FRAME_TRACE


/****************************************************************************
 *  Method:
 *    CVideoFormat::CVideoFormat
 *
 */
CVideoFormat::CVideoFormat(IHXCommonClassFactory* pCommonClassFactory,
			   CVideoRenderer* pVideoRenderer)
	: m_pCommonClassFactory(pCommonClassFactory)
	, m_pVideoRenderer(pVideoRenderer)
	, m_pMutex(NULL)
	, m_pAssemblerMutex(NULL)
	, m_pOutputQueue(NULL)
	, m_pFramePool(NULL)
	, m_lMaxBufferedDecodedFrames(MAX_BUFFERED_DECODED_FRAMES)
	, m_pHeader(NULL)
	, m_ulStartTime(0)
	, m_bDecodeSuspended(FALSE)
	, m_bRawPacketsDone(FALSE)
	, m_lRefCount(0)
	, m_LastError(HXR_OK)
{
    HX_ASSERT(m_pCommonClassFactory);

    m_pCommonClassFactory->AddRef();
    m_pVideoRenderer->AddRef();

#ifdef THREADS_SUPPORTED
    HXMutex::MakeMutex(m_pMutex);
    HXMutex::MakeMutex(m_pAssemblerMutex);
    HXMutex::MakeMutex(m_pDecoderMutex);
#else
    HXMutex::MakeStubMutex(m_pMutex);
    HXMutex::MakeStubMutex(m_pAssemblerMutex);
    HXMutex::MakeStubMutex(m_pDecoderMutex);
#endif
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::~CVideoFormat
 *
 */
CVideoFormat::~CVideoFormat()
{
    _Reset();

    HX_DELETE(m_pFramePool);
    HX_DELETE(m_pOutputQueue);

    HX_DELETE(m_pMutex);
    HX_DELETE(m_pAssemblerMutex);
    HX_DELETE(m_pDecoderMutex);

    HX_RELEASE(m_pHeader);
    HX_RELEASE(m_pVideoRenderer);
    HX_RELEASE(m_pCommonClassFactory);

#ifdef ENABLE_FRAME_TRACE
    DumpFrameEntries();
#endif	// ENABLE_FRAME_TRACE
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::Init
 *
 */
HX_RESULT CVideoFormat::Init(IHXValues* pHeader)
{
    HX_RESULT retVal = HXR_OK;

    HX_RELEASE(m_pHeader);
    m_pHeader = pHeader;
    if (m_pHeader)
    {
	m_pHeader->AddRef();
    }

    if (SUCCEEDED(retVal))
    {
	m_pFramePool = CreateBufferPool();
    }

    if (SUCCEEDED(retVal))
    {
	m_lMaxBufferedDecodedFrames = GetMaxDecodedFrames();

	FlushOutputQueue();
	HX_DELETE(m_pOutputQueue);

	m_pOutputQueue = new CRingBuffer(m_lMaxBufferedDecodedFrames);

	retVal = HXR_OUTOFMEMORY;
	if (m_pOutputQueue)
	{
	    retVal = HXR_OK;
	}
    }

    return retVal;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::CreateBufferPool
 *
 */
CHXBufferPool* CVideoFormat::CreateBufferPool(void)
{
    return new CHXBufferPool(
	    (BufferSizeFunc) CMediaPacket::GetBufferSize,
	    (BufferKillerFunc) CMediaPacket::DeletePacket);
}

/****************************************************************************
 *  Method:
 *    CVideoFormat::CreateAssembledPacket
 *
 */
CMediaPacket* CVideoFormat::CreateAssembledPacket(IHXPacket* pPayloadData)
{
    CMediaPacket* pFramePacket = NULL;

    if (pPayloadData->IsLost())
    {
	pFramePacket = NULL;
    }
    else
    {
	IHXBuffer* pBuffer = pPayloadData->GetBuffer();

	if (pBuffer)
	{
	    pFramePacket = new CMediaPacket(pBuffer->GetBuffer(),
					    pBuffer->GetBuffer(), 
					    pBuffer->GetSize(),
					    pBuffer->GetSize(),
					    pPayloadData->GetTime(),
					    MDPCKT_USES_IHXBUFFER_FLAG,
					    NULL);

	    pBuffer->Release();
	}
    }

    return pFramePacket;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::ReturnAssembledPacket
 *
 */
void CVideoFormat::ReturnAssembledPacket(CMediaPacket* pFramePacket)
{
    if (pFramePacket)
    {
	m_pMutex->Lock();
	m_InputQueue.AddTail(pFramePacket);
	m_pMutex->Unlock();
    }
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::Enqueue
 *
 */
BOOL CVideoFormat::Enqueue(IHXPacket* pPayloadData)
{
    BOOL bEnqueued = FALSE;
    CMediaPacket* pFramePacket = NULL;
    
    m_pAssemblerMutex->Lock();

    pFramePacket = CreateAssembledPacket(pPayloadData);
  
    if (pFramePacket != NULL)
    {
	m_pMutex->Lock();
	m_InputQueue.AddTail(pFramePacket);
	m_pMutex->Unlock();

	bEnqueued = TRUE;
    } 

    m_pAssemblerMutex->Unlock();
     
    return bEnqueued;
}

/****************************************************************************
 *  Method:
 *    CVideoFormat::Requeue
 *
 */
HX_RESULT CVideoFormat::Requeue(CMediaPacket* pFramePacket)
{
    if (pFramePacket != NULL)
    {
	m_pMutex->Lock();
	m_InputQueue.AddHead(pFramePacket);
	m_pMutex->Unlock();
    }

    return HXR_OK;
}

/****************************************************************************
 *  Method:
 *    CVideoFormat::Dequeue
 *
 */
CMediaPacket* CVideoFormat::Dequeue(void)
{
    CMediaPacket* pPacket = (CMediaPacket*) m_pOutputQueue->Get();

#ifdef ENABLE_FRAME_TRACE
    if (pPacket && (ulFrameTraceIdx < MAX_FRAME_TRACE_ENTRIES))
    {
	frameTraceArray[ulFrameTraceIdx][2] = pPacket->m_ulTime;
	frameTraceArray[ulFrameTraceIdx][0] = 
	    (LONG32) pPacket->m_pData;
	frameTraceArray[ulFrameTraceIdx++][1] = 'G';
    }
#endif	// ENABLE_FRAME_TRACE

    return pPacket;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::Reset
 *
 */
void CVideoFormat::Reset(void)
{
    _Reset();
}

void CVideoFormat::_Reset()
{
    m_pMutex->Lock();

    CMediaPacket* pFrame;
    
    while (!m_InputQueue.IsEmpty())
    {
	pFrame = (CMediaPacket*) m_InputQueue.RemoveHead();
	
	pFrame->Clear();
	delete pFrame;
    }
    
    FlushOutputQueue();

    m_pMutex->Unlock();
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::DecodeFrame
 *
 */
void CVideoFormat::FlushOutputQueue(void)
{
    CMediaPacket* pFrame;

    if (m_pOutputQueue)
    {
	while ((pFrame = (CMediaPacket*) m_pOutputQueue->Get()) != NULL)
	{
#ifdef ENABLE_FRAME_TRACE
	    if (ulFrameTraceIdx < MAX_FRAME_TRACE_ENTRIES)
	    {
		frameTraceArray[ulFrameTraceIdx][2] = pFrame->m_ulTime;
		frameTraceArray[ulFrameTraceIdx][0] = 
		    (LONG32) pFrame->m_pData;
		frameTraceArray[ulFrameTraceIdx++][1] = 'F';
	    }
#endif	// ENABLE_FRAME_TRACE

	    pFrame->Clear();
	    delete pFrame;
	}
    }
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::DecodeFrame
 *
 */
BOOL CVideoFormat::DecodeFrame(void)
{
    CMediaPacket* pEncodedPacket;
    CMediaPacket* pDecodedPacket = NULL;
    ULONG32 ulLoopCounter = 0;

    m_pVideoRenderer->BltIfNeeded();

    m_pDecoderMutex->Lock();
    m_pMutex->Lock();
    
    if ((!m_InputQueue.IsEmpty()) &&
	(!m_pOutputQueue->IsFull()) &&
	(!m_bDecodeSuspended))
    {
	do
	{
	    pEncodedPacket = (CMediaPacket*) m_InputQueue.RemoveHead();

	    m_pMutex->Unlock();

	    pDecodedPacket = CreateDecodedPacket(pEncodedPacket);

	    if (pDecodedPacket)
	    {
#ifdef ENABLE_FRAME_TRACE
		if (ulFrameTraceIdx < MAX_FRAME_TRACE_ENTRIES)
		{
		    frameTraceArray[ulFrameTraceIdx][2] = pDecodedPacket->m_ulTime;
		    frameTraceArray[ulFrameTraceIdx][0] = 
			(LONG32) pDecodedPacket->m_pData;
		    frameTraceArray[ulFrameTraceIdx++][1] = 'D';
		}
#endif	// ENABLE_FRAME_TRACE

		m_pOutputQueue->Put(pDecodedPacket);
		
		if (pDecodedPacket->m_pData)
		{
		    m_pDecoderMutex->Unlock();
		    m_pVideoRenderer->BltIfNeeded();
		    return TRUE;
		}
	    }

	    m_pDecoderMutex->Unlock();

            if( m_LastError == HXR_OUTOFMEMORY )
            {
                m_bDecodeSuspended = TRUE;
            }
            else
            {
                m_pVideoRenderer->BltIfNeeded();
            }
	    ulLoopCounter++;

	    m_pDecoderMutex->Lock();
	    m_pMutex->Lock();
	} while ((!m_InputQueue.IsEmpty()) &&
		 (!m_pOutputQueue->IsFull()) &&
		 ((ulLoopCounter++) < MAX_DECODE_SPIN) &&
		 (!m_bDecodeSuspended));
    }

    m_pMutex->Unlock();
    m_pDecoderMutex->Unlock();
    
    return pDecodedPacket ? TRUE : FALSE;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::ReturnDecodedPacket
 *
 */
BOOL CVideoFormat::ReturnDecodedPacket(CMediaPacket* pDecodedPacket)
{
    BOOL bPacketAccepted = FALSE;

    if (pDecodedPacket)
    {
#ifdef ENABLE_FRAME_TRACE
	if (ulFrameTraceIdx < MAX_FRAME_TRACE_ENTRIES)
	{
	    frameTraceArray[ulFrameTraceIdx][2] = pDecodedPacket->m_ulTime;
	    frameTraceArray[ulFrameTraceIdx][0] = 
		(LONG32) pDecodedPacket->m_pData;
	    frameTraceArray[ulFrameTraceIdx++][1] = 'D';
	}
#endif	// ENABLE_FRAME_TRACE
	    
	bPacketAccepted = m_pOutputQueue->Put(pDecodedPacket);
    }

    return bPacketAccepted;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::GetMaxDecodedFrames
 *
 */
ULONG32 CVideoFormat::GetMaxDecodedFrames(void)
{
    return MAX_BUFFERED_DECODED_FRAMES;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::CreateDecodedPacket
 *
 */
CMediaPacket* CVideoFormat::CreateDecodedPacket(CMediaPacket* pCodedPacket)
{
    CMediaPacket* pDecodedPacket = NULL;

    if (pCodedPacket != NULL)
    {
	pCodedPacket->Clear();
	delete pCodedPacket;
	pCodedPacket = NULL;
    }

    return pDecodedPacket;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::OnDecodedPacketRelease
 *
 */
void CVideoFormat::OnDecodedPacketRelease(CMediaPacket* &pPacket)
{
#ifdef ENABLE_FRAME_TRACE
    if (ulFrameTraceIdx < MAX_FRAME_TRACE_ENTRIES)
    {
	frameTraceArray[ulFrameTraceIdx][2] = pPacket->m_ulTime;
	frameTraceArray[ulFrameTraceIdx][0] = 
	    (LONG32) pPacket->m_pData;
	frameTraceArray[ulFrameTraceIdx++][1] = 'R';
    }
#endif	// ENABLE_FRAME_TRACE
    ;
}

/****************************************************************************
 *  Method:
 *    CVideoFormat::InitBitmapInfoHeader
 *
 */
HX_RESULT CVideoFormat::InitBitmapInfoHeader(
    HXBitmapInfoHeader &BitmapInfoHeader,
    CMediaPacket* pVideoPacket)

{
    HX_RESULT retVal = HXR_OK;

    return retVal;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::GetDefaultPreroll
 *
 */
ULONG32 CVideoFormat::GetDefaultPreroll(IHXValues* pValues)
{
    return FORMAT_DEFAULT_PREROLL;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::GetMinimumPreroll
 *
 */
ULONG32 CVideoFormat::GetMinimumPreroll(IHXValues* pValues)
{
    return FORMAT_MINIMUM_PREROLL;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::GetMaximumPreroll
 *
 */
ULONG32 CVideoFormat::GetMaximumPreroll(IHXValues* pValues)
{
    return FORMAT_MAXIMUM_PREROLL;
}

/****************************************************************************
 *  Method:
 *    CVideoFormat::GetMimeTypes
 *
 */
const char** CVideoFormat::GetMimeTypes(void)
{
    return NULL;
}


// *** IUnknown methods ***

/****************************************************************************
*  IUnknown::AddRef                                            ref:  hxcom.h
*
*  This routine increases the object reference count in a thread safe
*  manner. The reference count is used to manage the lifetime of an object.
*  This method must be explicitly called by the user whenever a new
*  reference to an object is used.
*/
STDMETHODIMP_(ULONG32) CVideoFormat::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

/****************************************************************************
*  IUnknown::Release                                           ref:  hxcom.h
*
*  This routine decreases the object reference count in a thread safe
*  manner, and deletes the object if no more references to it exist. It must
*  be called explicitly by the user whenever an object is no longer needed.
*/
STDMETHODIMP_(ULONG32) CVideoFormat::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }
    
    delete this;
    return 0;
}

/****************************************************************************
*  IUnknown::QueryInterface                                    ref:  hxcom.h
*
*  This routine indicates which interfaces this object supports. If a given
*  interface is supported, the object's reference count is incremented, and
*  a reference to that interface is returned. Otherwise a NULL object and
*  error code are returned. This method is called by other objects to
*  discover the functionality of this object.
*/
STDMETHODIMP CVideoFormat::QueryInterface(REFIID riid, void** ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
	AddRef();
	*ppvObj = (IUnknown*)(IHXPlugin*) this;
	return HXR_OK;
    }
    
    *ppvObj = NULL;
    return HXR_NOINTERFACE;
}

HX_RESULT
CVideoFormat::GetLastError()
{
    return m_LastError;
}
