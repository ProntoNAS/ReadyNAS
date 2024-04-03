/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: vidrend.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

/****************************************************************************
 *  Debug Settings
 */
// #define ENABLE_TRACE
// #define ENABLE_SYNC_TRACE
// #define ENABLE_SCHED_TRACE
// #define ENABLE_FETCH_TRACE
// #define ENABLE_INPUT_TRACE


/****************************************************************************
 *  Operational Compile-time Settings
 */
// #define SYNC_RESIZE_OK
// #define SYNC_VS_SWITCHING
// #define REBUFFER_ON_VIDEO
#define SET_NONZERO_VIEWFRAME_ONLY
#define RESIZE_AFTER_SITE_ATTACHED
#define DO_ABSOLUTE_TIMING
#ifdef HELIX_FEATURE_VIDREND_SYNCSMOOTHING
#define SYNC_SMOOTHING
#define SYNC_PRE_SMOOTHING
#endif	// HELIX_FEATURE_VIDREND_SYNCSMOOTHING
#define DEFAULT_VS2_TARGETRECT

#define MAX_ULONG32_AS_DOUBLE	((double) ((ULONG32) 0xFFFFFFFF))
#define MAX_LONG32_AS_DOUBLE	((double) ((LONG32) 0x7FFFFFFF))
#define MIN_LONG32_AS_DOUBLE	((double) ((LONG32) 0x80000000))

#if defined(_MACINTOSH) || defined(_MAC_UNIX)
#define OSGRANULE_BOOSTING_ENABLED  FALSE
#else	// _MACINTOSH
#define OSGRANULE_BOOSTING_ENABLED  TRUE
#endif	// _MACINTOSH

/****************************************************************************
 *  Debug Macros
 */
#ifdef ENABLE_TRACE
#define HX_TRACE_THINGY(x, m, l)					\
    {							\
	FILE* f1;					\
	f1 = ::fopen(x, "a+");				\
	(f1)?(::fprintf(f1, "%ld - %s = %ld \n", HX_GET_BETTERTICKCOUNT(), m, l), ::fclose(f1)):(0);\
    }
#else	// ENABLE_TRACE
#define HX_TRACE_THINGY(x, m, l)
#endif	// ENABLE_TRACE


/****************************************************************************
 *  Defines
 */
#define BASE_VIDEO_RENDERER_NAME    "Basic Video"
#define NAME_STATS_EXT		    ".name"
#define C4CC_STATS_EXT		    ".CodecFourCC"

#if defined(HELIX_FEATURE_VIDREND_NO_DEFAULT_WINDOW_SIZE)
#define DEFAULT_WIN_SIZE_X	    0
#define DEFAULT_WIN_SIZE_Y	    0
#else
#define DEFAULT_WIN_SIZE_X	    160
#define DEFAULT_WIN_SIZE_Y	    120
#endif

#define SYNC_INTERVAL		    20	    // in milliseconds
#define EARLY_FRAME_TOL		    3	    // in milliseconds
#define LATE_FRAME_TOL		    30	    // in milliseconds
#define NO_FRAMES_POLLING_INTERVAL  20	    // in milliseconds
#define MAX_SLEEP_TIME		    132	    // in milliseconds
#define BLT_PACKET_QUEUE_SIZE	    3	    // in packets
#define SYNC_GOAL_SMOOTHING_DEPTH   30	    // number of samples
#define SPEEDUP_GOAL_SMOOTHING_DEPTH 8	    // number of samples
#define MAX_BAD_SAMPLE_INTERVAL	    1000    // in milliseconds

#define VIDEO_STAT_INTERVAL	    1000    // in milliseconds
#define VIDEO_STAT_INTERVAL_COUNT   2	    // number of intervals to average over

#define MAX_OPTIMIZED_VIDEO_LEAD	200 // in milliseconds
#define DEFAULT_HARDWARE_BUFFER_COUNT	4

#define AUDIO_SKEW_POWER	    4	    // Maximum Audio Speedup Slope as
					    // 1/(2^AUDIO_SKEW_POWER)
#define MIN_BAD_PERSISTENCE_COUNT   0	    // Minimum number of consecutive
					    // bad sync samples for the
					    // resync probation period to start
#define MIN_GOOD_PERSISTENCE_COUNT  0	    // Minimum number of consecutive
					    // good sync samples for the
					    // resync probation period to be
					    // cancelled

#define MAX_BLT_LOOPS		    3
#define BLT_RELIEF_DELAY	    1	    // in milliseconds

// Absolute timing settings
#define N_STABILIZATION_ITERATIONS  5
#define MAX_ALLOWED_TIMING_ERROR    2
#define SMALLEST_TIMABLE_PERIOD	    2

// Default Decoding Priority
#ifdef _WIN32
    #ifdef HELIX_FEATURE_VIDREND_BOOSTDECODE_ON_STARTUP
    // on ce we need to get this started quickly, otherwise the initial packets expire
    // we set the priority so we can check in onpace to reset only once
    #define DFLT_DECODE_PRIORITY	    THREAD_PRIORITY_HIGHEST
    #define DFLT_PRESENT_PRIORITY	    THREAD_PRIORITY_ABOVE_NORMAL
    #else
    #define DFLT_DECODE_PRIORITY	    THREAD_PRIORITY_BELOW_NORMAL
    #define DFLT_PRESENT_PRIORITY	    THREAD_PRIORITY_ABOVE_NORMAL
    #endif // HELIX_FEATURE_VIDREND_BOOSTDECODE_ON_STARTUP
#else	// _WIN32
#define DFLT_DECODE_PRIORITY	    0
#define DFLT_PRESENT_PRIORITY	    0
#endif	// _WIN32

// Lowest acceptable version numbers
#define STREAM_MAJOR_VERSION  0
#define STREAM_MINOR_VERSION  0

#define CONTENT_MAJOR_VERSION 0
#define CONTENT_MINOR_VERSION 0

#define DECODER_INTERVAL	5
#define BLTR_INTERVAL		5

#ifdef _WIN32
#define GETBITMAPCOLOR(x) GetBitmapColor( (LPBITMAPINFO)(x) )
#else	// _WIN32
#define GETBITMAPCOLOR(x) GetBitmapColor( (HXBitmapInfo*)(x))
#endif	// _WIN32


/****************************************************************************
 *  Includes
 */
#include "hlxclib/stdio.h"
#include "vidrend.ver"
#include "hxtypes.h"
#ifdef _WINDOWS
#include <windows.h>
#endif


#if defined(_UNIX) && !defined(_MAC_UNIX)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef QWS
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#endif /* QWS */

#endif

#include "hxwintyp.h"
#include "hxcom.h"
#include "hxcomm.h"
#include "hxthread.h"

// for sync
#include "hxengin.h"
#include "hxprefs.h"
#include "hxtick.h"
#include "timeval.h"
#include "hxevent.h"
#include "hxvsurf.h"

#include "ihxpckts.h"
#include "hxfiles.h"
#include "hxcore.h"
#include "hxerror.h"
#include "hxrendr.h"
#include "addupcol.h"
// #include "hxhyper.h"
#include "hxplugn.h"
#include "hxwin.h"
// #include "hxasm.h"
#include "hxmon.h"
#include "hxbuffer.h"	// for CHXBuffer

#include "hxassert.h"	// for HX_ASSERT()
#include "hxheap.h"	// for heap checking
#include "hxslist.h"	// CHXSimpleList

#include "chxbufpl.h"

#include "sdpchunk.h"

#include "vidrend.h"
#include "cpacemkr.h"
#include "hxver.h"

#if defined(_WINDOWS) && !defined(WIN32_PLATFORM_PSPC)
#include "ddraw.h"
#endif	/* defined(_WINDOWS) && !defined(WIN32_PLATFORM_PSPC) */

// #include "coloracc.h"
#include "colormap.h"

#include "chxxtype.h"   // For CHXxSize

#if defined(HELIX_FEATURE_PREFERENCES)
#include "hxprefs.h"
#include "hxprefutil.h"
#endif /* HELIX_FEATURE_PREFERENCES */

/****************************************************************************
 *  Debug
 */
#ifdef ENABLE_SYNC_TRACE
#define MAX_SYNC_TRACE_ENTRIES	10000
ULONG32 ulSyncTraceIdx = 0;
ULONG32 syncTraceArray[MAX_SYNC_TRACE_ENTRIES][3];

void DumpSyncEntries(void)
{
    FILE* pFile = NULL;
    ULONG32 ulIdx;

    if (ulSyncTraceIdx > 0)
    {
	pFile = fopen("\\helix\\sync.txt", "wb");
    }

    if (pFile)
    {
	for (ulIdx = 0; ulIdx < ulSyncTraceIdx; ulIdx++)
	{
	    fprintf(pFile, "%u\t%u\t%u\n", syncTraceArray[ulIdx][0],
					   syncTraceArray[ulIdx][1],
					   syncTraceArray[ulIdx][2]);
	}

	fclose(pFile);
    }

    ulSyncTraceIdx = 0;
}
#endif	// ENABLE_SYNC_TRACE


#ifdef ENABLE_SCHED_TRACE
#define MAX_SCHED_TRACE_ENTRIES	10000
ULONG32 ulSchedTraceIdx = 0;
LONG32 schedTraceArray[MAX_SCHED_TRACE_ENTRIES];

void DumpSchedEntries(void)
{
    FILE* pFile = NULL;
    ULONG32 ulIdx;

    if (ulSchedTraceIdx > 0)
    {
	pFile = fopen("\\helix\\sched.txt", "wb");
    }

    if (pFile)
    {
	for (ulIdx = 0; ulIdx < ulSchedTraceIdx; ulIdx++)
	{
	    fprintf(pFile, "%d\n", schedTraceArray[ulIdx]);
	}

	fclose(pFile);
    }

    ulSchedTraceIdx = 0;
}
#endif	// ENABLE_SCHED_TRACE


#ifdef ENABLE_FETCH_TRACE
#define MAX_FETCH_TRACE_ENTRIES	10000
ULONG32 ulFetchTraceIdx = 0;
LONG32 fetchTraceArray[MAX_FETCH_TRACE_ENTRIES];

void DumpFetchEntries(void)
{
    FILE* pFile = NULL;
    ULONG32 ulIdx;

    if (ulFetchTraceIdx > 0)
    {
	pFile = fopen("\\helix\\fetch.txt", "wb");
    }

    if (pFile)
    {
	for (ulIdx = 0; ulIdx < ulFetchTraceIdx; ulIdx++)
	{
	    fprintf(pFile, "%d\n", fetchTraceArray[ulIdx]);
	}

	fclose(pFile);
    }

    ulFetchTraceIdx = 0;
}
#endif	// ENABLE_FETCH_TRACE


#ifdef ENABLE_INPUT_TRACE
#define MAX_INPUT_TRACE_ENTRIES	10000
ULONG32 ulInputTraceIdx = 0;
LONG32 inputTraceArray[MAX_INPUT_TRACE_ENTRIES];

void DumpInputEntries(void)
{
    FILE* pFile = NULL;
    ULONG32 ulIdx;

    if (ulInputTraceIdx > 0)
    {
	pFile = fopen("\\helix\\input.txt", "wb");
    }

    if (pFile)
    {
	for (ulIdx = 0; ulIdx < ulInputTraceIdx; ulIdx++)
	{
	    fprintf(pFile, "%d\n", inputTraceArray[ulIdx]);
	}

	fclose(pFile);
    }

    ulInputTraceIdx = 0;
}
#endif	// ENABLE_INPUT_TRACE


/****************************************************************************
 *  Constants
 */
const char* const CVideoRenderer::zm_pDescription    = "RealNetworks Video Renderer Plugin";
const char* const CVideoRenderer::zm_pCopyright      = HXVER_COPYRIGHT;
const char* const CVideoRenderer::zm_pMoreInfoURL    = HXVER_MOREINFO;

const char* const CVideoRenderer::zm_pStreamMimeTypes[] =
{
    NULL
};


/************************************************************************
 *  CVideoRenderer
 */
/************************************************************************
 *  Constructor/Destructor
 */
CVideoRenderer::CVideoRenderer(void)
	: m_lRefCount(0)
	, m_pContext(NULL)
	, m_pStream(NULL)
	, m_pHeader(NULL)
	, m_pMutex(NULL)
	, m_pBltMutex(NULL)
	, m_pVSMutex(NULL)
	, m_pDecoderPump(NULL)
	, m_pBltrPump(NULL)
	, m_ulDecoderPacemakerId(0)
	, m_ulBltrPacemakerId(0)
	, m_pDecoderVideoFormat(NULL)
	, m_pBltrVideoFormat(NULL)
	, m_pBackChannel(0)
	, m_pCommonClassFactory(0)
	, m_ulRegistryID(0)
	, m_pVideoStats(NULL)
	, m_pMISUS(NULL)
	, m_pMISUSSite(NULL)
	, m_ulEarlyFrameTol(EARLY_FRAME_TOL)
	, m_ulLateFrameTol(LATE_FRAME_TOL)
	, m_ulNoFramesPollingInterval(NO_FRAMES_POLLING_INTERVAL)
	, m_ulMaxSleepTime(MAX_SLEEP_TIME)
	, m_ulBltPacketQueueSize(BLT_PACKET_QUEUE_SIZE)
	, m_ulSyncGoalSmoothingDepth(SYNC_GOAL_SMOOTHING_DEPTH)
	, m_ulSpeedupGoalSmoothingDepth(SPEEDUP_GOAL_SMOOTHING_DEPTH)
	, m_ulMaxBadSeqSamples(0)
	, m_lTimeLineOffset(0)
	, m_lDecodePriority(DFLT_DECODE_PRIORITY)
	, m_ulStreamBaseTime(0)
	, m_ulBaseTime(0)
	, m_ulTimeNormalizationOffset(0)
	, m_bBitmapSet(FALSE)
	, m_bFrameSizeInitialized(FALSE)
	, m_bWinSizeFixed(FALSE)
	, m_bOSGranuleBoost(OSGRANULE_BOOSTING_ENABLED)
	, m_bOSGranuleBoostVS2(OSGRANULE_BOOSTING_ENABLED)
	, m_bOptimizedBlt(FALSE)
	, m_bUseVideoSurface2(FALSE)
#if defined(_WINDOWS) && defined(HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO)
	, m_bTryVideoSurface2(TRUE)
#else	// _WINDOWS
	, m_bTryVideoSurface2(FALSE)
#endif	// _WINDOWS
	, m_bVideoSurface2Transition(FALSE)
	, m_bVideoSurface1Requested(FALSE)
	, m_bVideoSurfaceInitialized(FALSE)
	, m_bVideoSurfaceReinitRequested(FALSE)
	, m_bPresentInProgress(FALSE)
	, m_bVS2BufferUnavailableOnLastBlt(FALSE)
	, m_ulHWBufCount(DEFAULT_HARDWARE_BUFFER_COUNT)
	, m_ulConfigHWBufCount(DEFAULT_HARDWARE_BUFFER_COUNT)
	, m_pClipRect(NULL)
	, m_pPreferences(NULL)
	, m_pRegistry(NULL)
	, m_pScheduler(NULL)
	, m_pOptimizedScheduler(NULL)
	, m_bPendingCallback(FALSE)
	, m_bSchedulerStartRequested(FALSE)
	, m_hPendingHandle(0)
	, m_ulCallbackCounter(0)
	, m_bDecoderRunning(FALSE)
	, m_bSiteAttached(FALSE)
	, m_PlayState(Stopped)
	, m_ulBytesToBuffer(0)
	, m_ulAvgBitRate(0)
	, m_ulPreroll(0)
	, m_ulBufferingStartTime(0)
	, m_ulBufferingStopTime(0)
	, m_ulBufferingTimeOut(0)
	, m_bBufferingOccured(FALSE)
	, m_bBufferingNeeded(FALSE)
	, m_pVideoFormat(NULL)
	, m_pBltPacketQueue(NULL)
	, m_pActiveVideoPacket(NULL)
	, m_ulActiveVideoTime(0)
	, m_bFirstFrame(TRUE)
	, m_bBaseTimeSet(FALSE)
	, m_bFirstSurfaceUpdate(TRUE)
	, m_bPendingRedraw(FALSE)
	, m_bVS1UpdateInProgress(FALSE)
	, m_ulSyncSmoothingDepth(0)
	, m_ulSyncInterval(SYNC_INTERVAL)
	, m_ulBadSeqSampleCount(0)
	, m_ulGoodSeqSampleCount(0)
	, m_fTrendSyncDelta(0.0)
	, m_pResizeCB(NULL)
	, m_pVSurf2InputBIH(NULL)
	, m_bIsScheduledCB(0)
#ifdef HELIX_FEATURE_VIDREND_UNTIMED_DECODE
	, m_bUntimedRendering(FALSE)
#endif /* HELIX_FEATURE_VIDREND_UNTIMED_DECODE */
	, m_bActiveVideoPacketLocalized(FALSE)
{
    m_SetWinSize.cx = 0;
    m_SetWinSize.cy = 0;
    m_LastSetSize.cx = 0;
    m_LastSetSize.cy = 0;

    m_rViewRect.left   = 0;
    m_rViewRect.top    = 0;
    m_rViewRect.right  = 0;
    m_rViewRect.bottom = 0; 

    memset(&m_BitmapInfoHeader, 0, sizeof(HXBitmapInfoHeader));
}

CVideoRenderer::~CVideoRenderer()
{
    // NOTE: You should do your renderer cleanup here, instead of
    // in EndStream(), because your renderer is still left around
    // after the stream is ended in case it is a display renderer
    // and it needs to "paint" it's display area.
    EndOptimizedBlt();

    if (m_pActiveVideoPacket)
    {
	m_pActiveVideoPacket->Clear();
	delete m_pActiveVideoPacket;
	m_pActiveVideoPacket = NULL;
	m_ulActiveVideoTime = 0;
    }

    HX_RELEASE(m_pHeader);
    HX_RELEASE(m_pContext);
    HX_RELEASE(m_pCommonClassFactory);

    if (m_pVideoFormat)
    {
        m_pVideoFormat->Release();
        m_pVideoFormat = NULL;
    }

    HX_DELETE(m_pVSurf2InputBIH);
    HX_DELETE(m_pClipRect);
    HX_DELETE(m_pMutex);
    HX_DELETE(m_pBltMutex);
    HX_DELETE(m_pVSMutex);

    RemoveCallback(m_hPendingHandle);
    m_bPendingCallback = FALSE;

    HX_RELEASE(m_pOptimizedScheduler);
    HX_RELEASE(m_pScheduler);

    HX_RELEASE(m_pPreferences);
    HX_RELEASE(m_pRegistry);

    ClearBltPacketQueue();
    HX_DELETE(m_pBltPacketQueue);

    HX_RELEASE(m_pResizeCB);

#if defined(HELIX_FEATURE_STATS)
    HX_DELETE(m_pVideoStats);
#endif /* HELIX_FEATURE_STATS */
}


/************************************************************************
 *  IHXPlugin Methods
 */
/************************************************************************
 *  Method:
 *    IHXPlugin::InitPlugin
 *  Purpose:
 *    Initializes the plugin for use. This interface must always be
 *    called before any other method is called. This is primarily needed
 *    so that the plugin can have access to the context for creation of
 *    IHXBuffers and IMalloc.
 */
STDMETHODIMP CVideoRenderer::InitPlugin(IUnknown* /*IN*/ pContext)
{
    HX_RESULT retVal = HXR_OK;

    HX_ASSERT(pContext);

    m_pContext = pContext;
    m_pContext->AddRef();

    retVal = m_pContext->QueryInterface(IID_IHXCommonClassFactory,
					(void**) &m_pCommonClassFactory);

    if (SUCCEEDED(retVal))
    {
	m_pContext->QueryInterface(IID_IHXPreferences,
                                   (void**) &m_pPreferences);
    }

    m_pContext->QueryInterface(IID_IHXRegistry, (void**) &m_pRegistry);

    if (SUCCEEDED(retVal))
    {
	retVal = m_pContext->QueryInterface(IID_IHXScheduler,
					    (void **) &m_pScheduler);
    }

    if (SUCCEEDED(retVal) && !m_pMutex)
    {
#ifdef THREADS_SUPPORTED
	retVal = HXMutex::MakeMutex(m_pMutex);
#else  // THREADS_SUPPORTED
	retVal = HXMutex::MakeStubMutex(m_pMutex);
#endif  // THREADS_SUPPORTED
    }

    if (SUCCEEDED(retVal) && !m_pBltMutex)
    {
#ifdef THREADS_SUPPORTED
	retVal = HXMutex::MakeMutex(m_pBltMutex);
#else  // THREADS_SUPPORTED
	retVal = HXMutex::MakeStubMutex(m_pBltMutex);
#endif  // THREADS_SUPPORTED
    }

    if (SUCCEEDED(retVal) && !m_pVSMutex)
    {
#ifdef THREADS_SUPPORTED
	retVal = HXMutex::MakeMutex(m_pVSMutex);
#else  // THREADS_SUPPORTED
	retVal = HXMutex::MakeStubMutex(m_pVSMutex);
#endif  // THREADS_SUPPORTED
    }

#if defined(HELIX_FEATURE_STATS)
    if (SUCCEEDED(retVal))
    {
	m_pVideoStats = new CVideoStatistics(m_pContext,
					     VIDEO_STAT_INTERVAL_COUNT);

	retVal = HXR_OUTOFMEMORY;
	if (m_pVideoStats)
	{
	    retVal = HXR_OK;
	}
    }
#endif /* HELIX_FEATURE_STATS */

    if (FAILED(retVal))
    {
	HX_RELEASE(m_pCommonClassFactory);
	HX_RELEASE(m_pPreferences);
	HX_RELEASE(m_pRegistry);
	HX_RELEASE(m_pScheduler);
    }

    return retVal;
}

/************************************************************************
 *  Method:
 *    IHXPlugin::GetPluginInfo
 *  Purpose:
 *    Returns the basic information about this plugin. Including:
 *
 *    bLoadMultiple	whether or not this plugin DLL can be loaded
 *			multiple times. All File Formats must set
 *			this value to TRUE.
 *    pDescription	which is used in about UIs (can be NULL)
 *    pCopyright	which is used in about UIs (can be NULL)
 *    pMoreInfoURL	which is used in about UIs (can be NULL)
 */
STDMETHODIMP CVideoRenderer::GetPluginInfo
(
   REF(BOOL)        /*OUT*/ bLoadMultiple,
   REF(const char*) /*OUT*/ pDescription,
   REF(const char*) /*OUT*/ pCopyright,
   REF(const char*) /*OUT*/ pMoreInfoURL,
   REF(ULONG32)     /*OUT*/ ulVersionNumber
)
{
    bLoadMultiple = TRUE;   // Must be true for file formats.

    pDescription    = zm_pDescription;
    pCopyright	    = zm_pCopyright;
    pMoreInfoURL    = zm_pMoreInfoURL;
    ulVersionNumber = TARVER_ULONG32_VERSION;

    return HXR_OK;
}

/************************************************************************
 *  Method:
 *    IHXPlugin::GetRendererInfo
 *  Purpose:
 *    If this object is a file format object this method returns
 *    information vital to the instantiation of file format plugins.
 *    If this object is not a file format object, it should return
 *    HXR_UNEXPECTED.
 */
STDMETHODIMP CVideoRenderer::GetRendererInfo
(
 REF(const char**) /*OUT*/ pStreamMimeTypes,
 REF(UINT32)      /*OUT*/ unInitialGranularity
)
{
    pStreamMimeTypes = (const char**) zm_pStreamMimeTypes;
    unInitialGranularity = SYNC_INTERVAL;
    return HXR_OK;
}




/************************************************************************
 *  IHXRenderer Methods
 */
/////////////////////////////////////////////////////////////////////////
//  Method:
//		IHXRenderer::StartStream
//  Purpose:
//		Called by client engine to inform the renderer of the stream it
//		will be rendering. The stream interface can provide access to
//		its source or player. This method also provides access to the
//		primary client controller interface.
//
STDMETHODIMP CVideoRenderer::StartStream(IHXStream* pStream,
					   IHXPlayer* pPlayer)
{
    m_pStream  = pStream;

    if (m_pStream)
    {
	m_pStream->AddRef();
    }

#if defined (HELIX_FEATURE_MISU)
    m_pCommonClassFactory->CreateInstance(
	CLSID_IHXMultiInstanceSiteUserSupplier,
	(void**) &m_pMISUS);

    if (m_pMISUS)
    {
	m_pMISUS->SetSingleSiteUser((IUnknown*)(IHXSiteUser*) this);
    }
#endif //HELIX_FEATURE_MISU

    if (m_pStream)
    {
	IHXStreamSource* pSource = 0;
	if (m_pStream->GetSource(pSource) == HXR_OK)
	{
	    /* It is OK if the source does not support backchannel. Reasons:
	     *
	     * 1. This stream may not be coming from h261 fileformat.
	     *	  It may instead be merged into a container fileformat which
	     *	  may be does not support BackChannel.
	     *
	     * 2. The protocol used to serve this stream may not support
	     *	  BackChannel.
	     */
	    pSource->QueryInterface(IID_IHXBackChannel, (void**) &m_pBackChannel);

	    pSource->Release();
	}
    }

    return HXR_OK;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//		IHXRenderer::EndStream
//  Purpose:
//		Called by client engine to inform the renderer that the stream
//		is was rendering is closed.
//
STDMETHODIMP CVideoRenderer::EndStream()
{
    // Stop Blts by changing state
    m_pMutex->Lock();
    m_PlayState = Stopped;
    m_pMutex->Unlock();

    // Stop Decoder
    if (m_pDecoderPump)
    {
	m_pDecoderPump->Stop();
	m_pDecoderPump->Signal();
	m_pDecoderPump->WaitForStop();
	m_pDecoderPump->Release();
	m_pDecoderPump = NULL;
    }

    // Wait for Blt if in progress and then flush packet queues
    DisplayMutex_Lock();
    if (m_pVideoFormat)
    {
	m_pVideoFormat->Reset();
    }
    DisplayMutex_Unlock();

    // IHXStream, IHXSourceStream or IHXBackChannel
    // cannot be used after EndStream has been called.
    HX_RELEASE(m_pStream);
    HX_RELEASE(m_pBackChannel);

    // Stop Bltr pump
    if (m_pBltrPump)
    {
	m_pBltrPump->Stop();
	m_pBltrPump->Signal();
    }

    // Flush Optimized Video Surface if used
    if (m_bUseVideoSurface2 && m_pMISUSSite)
    {
	FlushVideoSurface2(m_pMISUSSite);
    }

    if (m_pBltrPump)
    {
	m_pBltrPump->WaitForStop();
	m_pBltrPump->Release();
	m_pBltrPump = NULL;
    }

    DisplayMutex_Lock();
    if (m_pVideoFormat)
    {
        m_pVideoFormat->Release();
        m_pVideoFormat = NULL;
    }
    DisplayMutex_Unlock();

#ifdef ENABLE_SYNC_TRACE
    DumpSyncEntries();
#endif	// ENABLE_SYNC_TRACE

#ifdef ENABLE_SCHED_TRACE
    DumpSchedEntries();
#endif	// ENABLE_SCHED_TRACE

#ifdef ENABLE_FETCH_TRACE
    DumpFetchEntries();
#endif	// ENABLE_FETCH_TRACE

#ifdef ENABLE_INPUT_TRACE
    DumpInputEntries();
#endif	// ENABLE_INPUT_TRACE

    return HXR_OK;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//		IHXRenderer::OnHeader
//  Purpose:
//		Called by client engine when a header for this renderer is
//		available. The header will arrive before any packets.
//
STDMETHODIMP CVideoRenderer::OnHeader(IHXValues* pHeader)
{
    HX_RESULT retVal = HXR_OK;

    HX_DELETE(m_pClipRect);

    // Keep this for later use...
    HX_RELEASE(m_pHeader);
    m_pHeader = pHeader;
    m_pHeader->AddRef();

    m_ulStreamBaseTime = 0;
    m_ulBaseTime = 0;
    m_ulTimeNormalizationOffset = 0;

    // check the stream versions
    m_pHeader->AddRef();
    retVal = CheckStreamVersions(m_pHeader);
    m_pHeader->Release();

#ifdef HELIX_FEATURE_QUICKTIME
    IHXBuffer* pSDPData = NULL;

    if (SUCCEEDED(retVal) &&
	SUCCEEDED(pHeader->GetPropertyCString("SDPData", pSDPData)))
    {
	char *pData = (char*) pSDPData->GetBuffer();
	IHXValues *pValues = NULL;

	if (pData &&
	    SUCCEEDED(SDPParseChunk(pData,
				    strlen(pData),
				    pValues,
				    m_pCommonClassFactory,
				    SDPCTX_Renderer)))
	{
	    ULONG32 ulLeft;
	    ULONG32 ulRight;
	    ULONG32 ulTop;
	    ULONG32 ulBottom;

	    if (SUCCEEDED(pValues->GetPropertyULONG32(
		    "ClipFrameLeft", ulLeft)) &&
		SUCCEEDED(pValues->GetPropertyULONG32(
		    "ClipFrameRight", ulRight)) &&
		SUCCEEDED(pValues->GetPropertyULONG32(
		    "ClipFrameTop", ulTop)) &&
		SUCCEEDED(pValues->GetPropertyULONG32(
		    "ClipFrameBottom", ulBottom)))
	    {
		retVal = HXR_OUTOFMEMORY;

		m_pClipRect = (HXxRect*) new HXxRect;
		if (m_pClipRect)
		{
		    m_pClipRect->left = (INT32) ulLeft;
		    m_pClipRect->right = (INT32) ulRight;
		    m_pClipRect->top = (INT32) ulTop;
		    m_pClipRect->bottom = (INT32) ulBottom;
		    retVal = HXR_OK;
		}
	    }
	}

	HX_RELEASE(pValues);
    }

    HX_RELEASE(pSDPData);
#endif	// HELIX_FEATURE_QUICKTIME

    if (SUCCEEDED(retVal))
    {
	m_pVideoFormat = CreateFormatObject(m_pHeader);

	retVal = HXR_OUTOFMEMORY;
	if (m_pVideoFormat)
	{
	    m_pVideoFormat->AddRef();
	    retVal = HXR_OK;
	}
    }

    if (SUCCEEDED(retVal))
    {
	retVal = m_pVideoFormat->Init(pHeader);
    }

    if (SUCCEEDED(retVal))
    {
	m_ulEarlyFrameTol = GetEarlyFrameTolerance();
	m_ulLateFrameTol = GetLateFrameTolerance();
	m_ulMaxOptimizedVideoLead = GetMaxOptimizedVideoLead();

	m_ulMaxSleepTime = GetMaxSleepTime();
	m_ulNoFramesPollingInterval = GetNoFramesPollingInterval();
	m_ulBltPacketQueueSize = GetBltPacketQueueSize();
	m_ulSyncGoalSmoothingDepth = GetSyncGoalSmoothingDepth();
	m_ulSpeedupGoalSmoothingDepth = GetSpeedupGoalSmoothingDepth();
	m_ulMaxBadSeqSamples = GetMaxBadSeqSamples();
    }

    // Setup preroll
    if (SUCCEEDED(retVal))
    {
	BOOL bSetNewPreroll = FALSE;
        m_ulPreroll = 0;
	ULONG32 ulMinPreroll = m_pVideoFormat->GetMinimumPreroll(pHeader);
	ULONG32 ulMaxPreroll = m_pVideoFormat->GetMaximumPreroll(pHeader);

        // Check that the stream header has a preroll value. If not...
	pHeader->GetPropertyULONG32("Preroll", m_ulPreroll);
	if (m_ulPreroll == 0)
	{
            // ... let's use default value.
	    m_ulPreroll = m_pVideoFormat->GetDefaultPreroll(pHeader);
	    bSetNewPreroll = TRUE;
	}
        else if( m_ulPreroll > ulMaxPreroll )
        {
            m_ulPreroll = ulMaxPreroll;
	    bSetNewPreroll = TRUE;
        }
	else if (m_ulPreroll < ulMinPreroll)
	{
	    m_ulPreroll = ulMinPreroll;
	    bSetNewPreroll = TRUE;
	}

	if (bSetNewPreroll)
	{
	    pHeader->SetPropertyULONG32("Preroll", m_ulPreroll);
	}
    }

    // Determine Average Bitrate
    if (SUCCEEDED(retVal))
    {
	if (FAILED(pHeader->GetPropertyULONG32("AvgBitRate", m_ulAvgBitRate)))
	{
	    m_ulAvgBitRate = 0;
	}
    }

    // Create Blt Queue
    if (SUCCEEDED(retVal))
    {
	m_pBltPacketQueue = new CRingBuffer(m_ulBltPacketQueueSize);

	retVal = HXR_OUTOFMEMORY;
	if (m_pBltPacketQueue)
	{
	    retVal = HXR_OK;
	}
    }

    return retVal;
}


/////////////////////////////////////////////////////////////////////////////
//  Method:
//	CVideoRenderer::CheckStreamVersions
//  copied from CRealAudioRenderer
HX_RESULT CVideoRenderer::CheckStreamVersions(IHXValues* pHeader)
{
    // check stream and content versions so an upgrade can
    // be called if necessary...
    HX_RESULT pnr = HXR_OK;

    BOOL bVersionOK = TRUE;

    UINT32 ulStreamVersion = 0;
    UINT32 ulContentVersion = 0;

    if(HXR_OK == pHeader->GetPropertyULONG32("StreamVersion",
	ulStreamVersion))
    {
	UINT32 ulMajorVersion = HX_GET_MAJOR_VERSION(ulStreamVersion);
	UINT32 ulMinorVersion = HX_GET_MINOR_VERSION(ulStreamVersion);
	ULONG32 ulThisMajorVersion = 0;
	ULONG32 ulThisMinorVersion = 0;

	GetStreamVersion(ulThisMajorVersion, ulThisMinorVersion);

	if((ulMajorVersion > ulThisMajorVersion) ||
	   ((ulMinorVersion > ulThisMinorVersion) &&
	    (ulMajorVersion == ulThisMajorVersion)))
	{
	    bVersionOK = FALSE;
	}
    }

    if(bVersionOK &&
       (HXR_OK == pHeader->GetPropertyULONG32("ContentVersion",
					      ulContentVersion)))
    {
	UINT32 ulMajorVersion = HX_GET_MAJOR_VERSION(ulContentVersion);
	UINT32 ulMinorVersion = HX_GET_MINOR_VERSION(ulContentVersion);
	ULONG32 ulThisMajorVersion = 0;
	ULONG32 ulThisMinorVersion = 0;

	GetContentVersion(ulThisMajorVersion, ulThisMinorVersion);

	if((ulMajorVersion > ulThisMajorVersion) ||
	   ((ulMinorVersion > ulThisMinorVersion) &&
	    (ulMajorVersion == ulMajorVersion)))
	{
	    bVersionOK = FALSE;
	}
    }

    if(!bVersionOK)
    {
        AddToAutoUpgradeCollection(GetUpgradeMimeType(), m_pContext);

	pnr = HXR_FAIL;
    }

    return pnr;
}


/////////////////////////////////////////////////////////////////////////
//  Method:
//		IHXRenderer::OnPacket
//  Purpose:
//		Called by client engine when a packet for this renderer is
//		due.
//
STDMETHODIMP CVideoRenderer::OnPacket(IHXPacket* pPacket, LONG32 lTimeOffset)
{
    m_lTimeLineOffset = lTimeOffset;

#ifdef ENABLE_INPUT_TRACE
    if (ulInputTraceIdx < MAX_INPUT_TRACE_ENTRIES)
    {
	inputTraceArray[ulInputTraceIdx++] =
	    ComputeTimeAhead(pPacket->GetTime(), 0);
    }
#endif	// ENABLE_INPUT_TRACE

    if (m_bSchedulerStartRequested)
    {
	StartSchedulers();
    }

    BOOL bQueueRet = m_pVideoFormat->Enqueue(pPacket);
    if(bQueueRet == FALSE && m_pVideoFormat->GetLastError() == HXR_OUTOFMEMORY)
    {
        return HXR_OUTOFMEMORY;
    }

    // try to decode a frame
    if (m_PlayState == Playing)
    {
	if (!IsDecoderRunning() || NULL == m_pDecoderPump)
	{
	    BOOL bDecRet = m_pVideoFormat->DecodeFrame();
            if( bDecRet == FALSE && m_pVideoFormat->GetLastError() == HXR_OUTOFMEMORY )
            {
                return HXR_OUTOFMEMORY;
            }
	}
	else
	{
	    m_pDecoderPump->Signal();
	}
    }
    else
    {
	if (!m_bBaseTimeSet)
	{
	    m_pMutex->Lock();
	    if (m_PlayState != Playing)
	    {
		m_ulBaseTime = pPacket->GetTime();
	    }
	    m_pMutex->Unlock();
	    m_bBaseTimeSet = TRUE;
	}

	// If we are seeking, this is a pre-seek packet and there is
	// no need to decode it
	if (m_PlayState != Seeking)
	{
	    if (!IsDecoderRunning() || NULL == m_pDecoderPump)
	    {
		m_pVideoFormat->DecodeFrame();
	    }
	    else
	    {
		m_pDecoderPump->Signal();
	    }

	    if (m_PlayState == Buffering)
	    {
		if (IsBufferingComplete(pPacket))
		{
		    RequestBufferingEnd();
		}
	    }
	}
    }

    return HXR_OK;
}


/////////////////////////////////////////////////////////////////////////
//  Method:
//		IHXRenderer::OnTimeSync
//  Purpose:
//		Called by client engine to inform the renderer of the current
//		time relative to the streams synchronized time-line. The
//		renderer should use this time value to update its display or
//		render it's stream data accordingly.
//
STDMETHODIMP CVideoRenderer::OnTimeSync(ULONG32 ulTime)
{
    ULONG32 ulStreamBaseTime = HX_GET_BETTERTICKCOUNT();
    ULONG32 ulBaseTime = ulTime;
    BOOL bGoodSample = TRUE;

    if (m_bSchedulerStartRequested)
    {
	StartSchedulers();
    }

#ifdef HELIX_FEATURE_STATS
    if (((ULONG32) (ulStreamBaseTime - m_pVideoStats->GetLastSyncTime())) >=
	VIDEO_STAT_INTERVAL)
    {
	m_pVideoStats->SyncStats(ulStreamBaseTime);
    }
#endif /* HELIX_FEATURE_STATS */

#ifdef ENABLE_SYNC_TRACE
    ULONG32 ulOrigStreamBaseTime = ulStreamBaseTime;
#endif // ENABLE_SYNC_TRACE

#ifdef SYNC_SMOOTHING_OLD_SCHEME
    LONG32 lNewSyncDelta = (LONG32) (ulStreamBaseTime - m_lTrendSyncDelta - ulTime);
    m_lTrendSyncDelta += lNewSyncDelta / ((LONG32) (m_ulSyncSmoothingDepth + 1));

    ulStreamBaseTime = ulTime + m_lTrendSyncDelta;

    if (m_ulSyncSmoothingDepth < m_ulSyncGoalSmoothingDepth)
    {
	m_ulSyncSmoothingDepth++;
    }
#endif	// SYNC_SMOOTHING_OLD_SCHEME

#ifdef SYNC_PRE_SMOOTHING
    if (m_ulSyncSmoothingDepth > 0)
    {
	LONG32 lStreamBaseDelta = ((LONG32) (ulStreamBaseTime - m_ulStreamBaseTime));
	ULONG32 ulBaseDelta = ulBaseTime - m_ulBaseTime;
	BOOL bSkipSmoothing = FALSE;

	bGoodSample = ((lStreamBaseDelta <= 0) ||
		       (((ULONG32) lStreamBaseDelta) <=
		        (ulBaseDelta + (ulBaseDelta >> AUDIO_SKEW_POWER))) ||
		       (bSkipSmoothing = ((m_ulBadSeqSampleCount++) > m_ulMaxBadSeqSamples)));

	if (bSkipSmoothing)
	{
	    m_ulSyncSmoothingDepth = 0;
	    m_fTrendSyncDelta = 0.0;
	}
    }
#endif	// SYNC_PRE_SMOOTHING

    if (bGoodSample)
    {
#ifdef SYNC_SMOOTHING
	double fNewSyncDelta = (((double) ((ULONG32) (ulStreamBaseTime - ulTime))) -
			       m_fTrendSyncDelta);

	// If we have a m_fTrendSyncDelta, make sure we consider
	// m_fTrendSyncDelta wrap-around in relation to fNewSyncDelta
	if (m_ulSyncSmoothingDepth > 0)
	{
	    if (fNewSyncDelta > MAX_LONG32_AS_DOUBLE)
	    {
		fNewSyncDelta = MAX_ULONG32_AS_DOUBLE - fNewSyncDelta;
	    }
	    else if (fNewSyncDelta < MIN_LONG32_AS_DOUBLE)
	    {
		fNewSyncDelta += MAX_ULONG32_AS_DOUBLE;
	    }
	}

	if (fNewSyncDelta < 0.0)
	{
	    // We are trying to speed up: use speed up smoothing criteria
	    m_fTrendSyncDelta += (fNewSyncDelta /
				  ((m_ulSyncSmoothingDepth >= m_ulSpeedupGoalSmoothingDepth) ?
				   m_ulSpeedupGoalSmoothingDepth + 1 : m_ulSyncSmoothingDepth + 1));
	}
	else
	{
	    m_fTrendSyncDelta += fNewSyncDelta / (m_ulSyncSmoothingDepth + 1);
	}

	if (m_fTrendSyncDelta > MAX_ULONG32_AS_DOUBLE)
	{
	    m_fTrendSyncDelta -= MAX_ULONG32_AS_DOUBLE;
	}
	else if (m_fTrendSyncDelta < 0)
	{
	     m_fTrendSyncDelta += MAX_ULONG32_AS_DOUBLE;
	}

	ulStreamBaseTime = ulTime + ((ULONG32) (m_fTrendSyncDelta));

	if (m_ulSyncSmoothingDepth < m_ulSyncGoalSmoothingDepth)
	{
	    m_ulSyncSmoothingDepth++;
	}
#endif	// SYNC_SMOOTHING

	m_ulGoodSeqSampleCount++;

	m_ulStreamBaseTime = ulStreamBaseTime;
	m_ulBaseTime = ulBaseTime;
	m_ulTimeNormalizationOffset = m_ulStreamBaseTime -
				      m_ulBaseTime -
				      m_lTimeLineOffset;

	if (m_ulGoodSeqSampleCount >= MIN_GOOD_PERSISTENCE_COUNT)
	{
	    m_ulBadSeqSampleCount = 0;
	}
    }
    else
    {
	// This is a bad sample
	if (m_ulBadSeqSampleCount >= MIN_BAD_PERSISTENCE_COUNT)
	{
	    m_ulGoodSeqSampleCount = 0;
	}
    }

#ifdef ENABLE_SYNC_TRACE
    if (ulSyncTraceIdx < MAX_SYNC_TRACE_ENTRIES)
    {
	syncTraceArray[ulSyncTraceIdx][0] = m_ulBaseTime;
	syncTraceArray[ulSyncTraceIdx][1] = m_ulStreamBaseTime;
	syncTraceArray[ulSyncTraceIdx++][2] = ulOrigStreamBaseTime;
    }
#endif	// ENABLE_SYNC_TRACE

    if (m_PlayState == Playing)
    {
	if (!IsDecoderRunning())
	{
	    m_pVideoFormat->DecodeFrame();
	}

	if (m_bBufferingNeeded)
	{
	    m_pMutex->Lock();

	    if (m_PlayState == Playing)
	    {
		BeginBuffering();
	    }

	    m_pMutex->Unlock();
	}
    }
    else if ((m_PlayState == PlayStarting) ||
	     (m_PlayState == Buffering))
    {
	m_pMutex->Lock();

	m_bBaseTimeSet = TRUE;

	// Reset the offset to avoid race condition
	// with m_ulTimeNormalizationOffset setting in OnPacket()
	if (bGoodSample)
	{
	    m_ulTimeNormalizationOffset = m_ulStreamBaseTime -
					  m_ulBaseTime -
					  m_lTimeLineOffset;
	}

	if (m_PlayState == Buffering)
	{
	    EndBuffering();
	    m_PlayState = PlayStarting;
	}

	if (m_PlayState == PlayStarting)
	{
	    m_PlayState = Playing;

	    BltIfNeeded();

	    StartSchedulers();

	    if (m_pBltrPump)
	    {
		m_pBltrPump->Signal();
	    }
	}

	m_pMutex->Unlock();
    }

    return HXR_OK;
}


HX_RESULT CVideoRenderer::StartSchedulers(void)
{
    HX_RESULT retVal = HXR_OK;


#ifdef HELIX_FEATURE_VIDREND_UNTIMED_DECODE

    // When running the decoder as fast as possible, we dont want to start the scheduler, so short

    // circuit this code. Additionally, calls such as BltIfNeeded() and ScheduleCallback() (which

    // may still be called) make assumptions based on the state of our interactions with the

    // scheduler, so we satisfy them by setting a pending handle representing a nonexistant callback.

    if( m_bUntimedRendering )

    {

	m_hPendingHandle = 1;

	return HXR_OK;

    }

#endif /* HELIX_FEATURE_VIDREND_UNTIMED_DECODE */


    m_bSchedulerStartRequested = FALSE;

    DisplayMutex_Lock();

    if (ShouldKickStartScheduler())
    {
	m_bBufferingNeeded = FALSE;
	ScheduleCallback(0);
    }

    if (SUCCEEDED(retVal) && (m_pDecoderPump == NULL))
    {
	CVideoPaceMaker* pVideoPaceMaker;
	retVal = HXR_OUTOFMEMORY;

	pVideoPaceMaker = new CVideoPaceMaker;

	if (pVideoPaceMaker)
	{
	    retVal = pVideoPaceMaker->QueryInterface(
		IID_IHXPaceMaker,
		(void**) &m_pDecoderPump);

	    if (SUCCEEDED(retVal))
	    {
		pVideoPaceMaker = NULL;
		m_pDecoderPump->Start(this,
		    GetDecodePriority(),
		    DECODER_INTERVAL,
		    m_ulDecoderPacemakerId);
	    }
	}

	HX_DELETE(pVideoPaceMaker);
    }

    if (SUCCEEDED(retVal) && (m_pBltrPump == NULL) && m_bTryVideoSurface2)
    {
	CVideoPaceMaker* pVideoPaceMaker;
	retVal = HXR_OUTOFMEMORY;

	pVideoPaceMaker = new CVideoPaceMaker;

	if (pVideoPaceMaker)
	{
	    retVal = pVideoPaceMaker->QueryInterface(
		IID_IHXPaceMaker,
		(void**) &m_pBltrPump);

	    if (SUCCEEDED(retVal))
	    {
		pVideoPaceMaker = NULL;
		m_pBltrPump->Start(this,
		    DFLT_PRESENT_PRIORITY,
		    BLTR_INTERVAL,
		    m_ulBltrPacemakerId);
	    }
	}

	HX_DELETE(pVideoPaceMaker);
    }

    DisplayMutex_Unlock();

    return retVal;
}


/////////////////////////////////////////////////////////////////////////
//  Method:
//	IHXRenderer::OnPreSeek
//  Purpose:
//	Called by client engine to inform the renderer that a seek is
//	about to occur. The render is informed the last time for the
//	stream's time line before the seek, as well as the first new
//	time for the stream's time line after the seek will be completed.
//
STDMETHODIMP CVideoRenderer::OnPreSeek(ULONG32 ulOldTime, ULONG32 ulNewTime)
{
    // Change state to stop Blts
    m_pMutex->Lock();
    m_PlayState = Seeking;
    m_pMutex->Unlock();

    // Suspend the Decoder pump
    if (m_pDecoderPump)
    {
	m_pDecoderPump->Suspend(TRUE);
	m_pDecoderPump->Signal();
	m_pDecoderPump->WaitForSuspend();
    }

    // Wait for Blt in progress to complete and reset
    // packet queues
    DisplayMutex_Lock();

    m_pVideoFormat->SetStartTime(ulNewTime);
    m_pVideoFormat->Reset();
#if defined(HELIX_FEATURE_STATS)
    m_pVideoStats->ResetSequence();
#endif /* HELIX_FEATURE_STATS */

    m_bFirstSurfaceUpdate = TRUE;
    m_bFirstFrame = TRUE;
    m_bBaseTimeSet = FALSE;

    DisplayMutex_Unlock();

    return HXR_OK;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//	IHXRenderer::OnPostSeek
//  Purpose:
//	Called by client engine to inform the renderer that a seek has
//	just occured. The render is informed the last time for the
//	stream's time line before the seek, as well as the first new
//	time for the stream's time line after the seek.
//
STDMETHODIMP CVideoRenderer::OnPostSeek(ULONG32 ulOldTime, ULONG32 ulNewTime)
{
    DisplayMutex_Lock();

    // clean up the packet lists
    m_pVideoFormat->SetStartTime(ulNewTime);
    m_pVideoFormat->Reset();

#if defined(HELIX_FEATURE_STATS)
    m_pVideoStats->ResetSequence();
#endif /* HELIX_FEATURE_STATS */

    m_bFirstSurfaceUpdate = TRUE;
    m_bFirstFrame = TRUE;
    m_ulBaseTime = ulNewTime;
    m_bBaseTimeSet = TRUE;
    m_bVS2BufferUnavailableOnLastBlt = FALSE;

    if (m_pDecoderPump)
    {
	m_pDecoderPump->Suspend(FALSE);
	m_pDecoderPump->Signal();
    }

    if (m_bUseVideoSurface2 && m_pMISUSSite)
    {
	FlushVideoSurface2(m_pMISUSSite);
    }

    DisplayMutex_Unlock();

    // PostSeek signals the proper packets are to start arriving
    m_pMutex->Lock();
    m_PlayState = PlayStarting;
    m_pMutex->Unlock();

    return HXR_OK;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//	IHXRenderer::OnPause
//  Purpose:
//	Called by client engine to inform the renderer that a pause has
//	just occured. The render is informed the last time for the
//	stream's time line before the pause.
//
STDMETHODIMP CVideoRenderer::OnPause(ULONG32 ulTime)
{
    m_pMutex->Lock();

    m_PlayState = Paused;

    m_pMutex->Unlock();

    return HXR_OK;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//	IHXRenderer::OnBegin
//  Purpose:
//	Called by client engine to inform the renderer that a begin or
//	resume has just occured. The render is informed the first time
//	for the stream's time line after the resume.
//
STDMETHODIMP CVideoRenderer::OnBegin(ULONG32 ulTime)
{
    HX_RESULT retVal = HXR_OK;

    m_pMutex->Lock();

    m_bBufferingOccured = FALSE;
    // If we are seeking, PostSeek will notify us when the play
    // will be starting
    if (m_PlayState != Seeking)
    {
	m_PlayState = PlayStarting;
    }

    // No need to clear the Blt Packet queue here since
    // The Refresh event will always clear it and clearing
    // it here would create a race condition on Blt Packet
    // Queue (ring buffer) read.
    // ClearBltPacketQueue();
    m_ulSyncSmoothingDepth = 0;
    m_ulBadSeqSampleCount = 0;
    m_ulGoodSeqSampleCount = 0;
    m_bIsScheduledCB = 0;
    m_bVS2BufferUnavailableOnLastBlt = FALSE;

    retVal = StartSchedulers();

    m_pMutex->Unlock();

#ifdef HELIX_FEATURE_VIDREND_BOOSTDECODE_ON_STARTUP
    if (m_pDecoderPump)
    {
       //m_pDecoderPump->Suspend(TRUE);
       //m_pDecoderPump->WaitForSuspend();  // Wait for decoder thread to start and suspends itself
       //m_pDecoderPump->Suspend(FALSE);
       m_pDecoderPump->Signal();   // Kick-out decoder pump from suspension
    }
#endif  // HELIX_FEATURE_VIDREND_BOOSTDECODE_ON_STARTUP

    return retVal;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//	IHXRenderer::OnBuffering
//  Purpose:
//	Called by client engine to inform the renderer that buffering
//	of data is occuring. The render is informed of the reason for
//	the buffering (start-up of stream, seek has occured, network
//	congestion, etc.), as well as percentage complete of the
//	buffering process.
//
STDMETHODIMP CVideoRenderer::OnBuffering(ULONG32 ulFlags, UINT16 unPercentComplete)
{
    HX_RESULT retVal = HXR_OK;

    m_pMutex->Lock();

    if (m_PlayState == Buffering)
    {
	if (IsBufferingComplete())
	{
	    EndBuffering();
	}
    }
    else if (m_PlayState == Playing)
    {
	m_PlayState = PlayStarting;
    }

    m_pMutex->Unlock();

    return retVal;
}

/////////////////////////////////////////////////////////////////////////
//  Method:
//	IHXRenderer::GetDisplayType
//  Purpose:
//	Called by client engine to ask the renderer for it's preferred
//	display type. When layout information is not present, the
//	renderer will be asked for it's prefered display type. Depending
//	on the display type a buffer of additional information may be
//	needed. This buffer could contain information about preferred
//	window size.
//
STDMETHODIMP CVideoRenderer::GetDisplayType(REF(HX_DISPLAY_TYPE) ulFlags,
					   REF(IHXBuffer*) pBuffer)
{
    ulFlags = HX_DISPLAY_WINDOW |
	      HX_DISPLAY_SUPPORTS_RESIZE |
	      HX_DISPLAY_SUPPORTS_FULLSCREEN |
	      HX_DISPLAY_SUPPORTS_VIDCONTROLS;

    return HXR_OK;
}

/************************************************************************
*	Method:
*	    IHXRenderer::OnEndofPackets
*	Purpose:
*	    Called by client engine to inform the renderer that all the
*	    packets have been delivered. However, if the user seeks before
*	    EndStream() is called, renderer may start getting packets again
*	    and the client engine will eventually call this function again.
*/
STDMETHODIMP CVideoRenderer::OnEndofPackets(void)
{
    if (m_pVideoFormat)
    {
	m_pVideoFormat->OnRawPacketsEnded();
    }

    return HXR_OK;
}


/************************************************************************
 *  IHXStatistics Methods
 */
/************************************************************************
 *  InitializeStatistics
 */
#define MAX_STAT_ENTRY_PAIRS	32
STDMETHODIMP CVideoRenderer::InitializeStatistics(UINT32 ulRegistryID)
{
    m_ulRegistryID = ulRegistryID;

#if defined(HELIX_FEATURE_STATS)
    BOOL bCodecNameKnown = FALSE;
    char* pValue = NULL;
    HX_RESULT retVal = HXR_UNEXPECTED;

    if (m_pVideoStats)
    {
	retVal = HXR_OK;
    }

    if (SUCCEEDED(retVal))
    {
	pValue = (char*) GetCodecName();
	if (pValue != NULL)
	{
	    ReportStat(VS_CODEC_NAME, pValue);
	    bCodecNameKnown = TRUE;
	}
    }

    if (SUCCEEDED(retVal))
    {
	pValue = (char*) GetRendererName();
	if (pValue != NULL)
	{
	    ReportStat(VS_REND_NAME, pValue);
	    // If Codec name is unknown, use a more generic renderer name
	    if (!bCodecNameKnown)
	    {
		ReportStat(VS_CODEC_NAME, pValue);
	    }
	}
    }

    if (SUCCEEDED(retVal))
    {
	pValue = (char*) GetCodecFourCC();
	if (pValue != NULL)
	{
	    ReportStat(VS_CODEC_4CC, pValue);
	}
    }

    if (SUCCEEDED(retVal))
    {
	ReportStat(VS_CURRENT_FRAMERATE, "0.0");
	ReportStat(VS_FRAMES_DISPLAYED, "100.0");
	ReportStat(VS_FRAMES_DROPPED, (INT32) 0);
	ReportStat(VS_FRAMES_LOST, (INT32) 0);
	ReportStat(VS_SURESTREAM, "FALSE");
	ReportStat(VS_IMAGE_WIDTH, (INT32) 0);
	ReportStat(VS_IMAGE_HEIGHT, (INT32) 0);
    }

    if (SUCCEEDED(retVal))
    {
	InitExtraStats();
    }

    if (SUCCEEDED(retVal))
    {
	retVal = m_pVideoStats->DisplayStats(m_ulRegistryID);
    }

    return retVal;
#else
    return HXR_NOTIMPL;
#endif /* HELIX_FEATURE_STATS */
}

/************************************************************************
 *  UpdateStatistics
 */
STDMETHODIMP CVideoRenderer::UpdateStatistics()
{
#if defined(HELIX_FEATURE_STATS)
    HX_RESULT retVal = HXR_UNEXPECTED;

    if (m_pVideoStats)
    {
	retVal = HXR_OK;
    }

    if (SUCCEEDED(retVal))
    {
	retVal = m_pVideoStats->DisplayStats(m_ulRegistryID);
    }

    return retVal;
#else
    return HXR_NOTIMPL;
#endif /* HELIX_FEATURE_STATS */
}


/************************************************************************
 *  IHXUntimedDecoder Methods
 */
#ifdef HELIX_FEATURE_VIDREND_UNTIMED_DECODE
STDMETHODIMP_(BOOL) CVideoRenderer::IsUntimedRendering()
{
    return m_bUntimedRendering;
}
STDMETHODIMP_(HX_RESULT) CVideoRenderer::SetUntimedRendering(BOOL bUntimedRendering)
{

    if( m_PlayState != Stopped && m_PlayState != PlayStarting )

    {

	return HXR_UNEXPECTED;

    }


    m_bUntimedRendering = bUntimedRendering;


    return HXR_OK;
}
#endif /* HELIX_FEATURE_VIDREND_UNTIMED_DECODE */

/************************************************************************
 *  IHXUpdateProperties Methods
 */
/************************************************************************
 *  UpdatePacketTimeOffset
 *	Call this method to update the timestamp offset of cached packets
 */
STDMETHODIMP CVideoRenderer::UpdatePacketTimeOffset(INT32 lTimeOffset)
{
    m_lTimeLineOffset -= lTimeOffset;

    return HXR_OK;
}


/************************************************************************
 *	Method:
 *	    IHXUpdateProperties::UpdatePlayTimes
 *	Purpose:
 *	    Call this method to update the playtime attributes
 */
STDMETHODIMP
CVideoRenderer::UpdatePlayTimes(IHXValues* pProps)
{
    return HXR_OK;
}


/************************************************************************
 *  IHXRenderTimeLine Methods
 */
/************************************************************************
 *  GetTimeLineValue
 */
STDMETHODIMP CVideoRenderer::GetTimeLineValue(REF(UINT32) ulTime)
{
    ulTime = ((ULONG32) -ComputeTimeAhead(0, 0));

    if (m_PlayState != Playing)
    {
        return HXR_TIMELINE_SUSPENDED;
    }

    return HXR_OK;
}


/************************************************************************
 *  IHXSiteUser Methods
 */
/************************************************************************
 *  AttachSite
 */
STDMETHODIMP CVideoRenderer::AttachSite(IHXSite* /*IN*/ pSite)
{
    if (m_pMISUSSite)
    {
	return HXR_UNEXPECTED;
    }

    m_bSiteAttached = TRUE;

    m_pMISUSSite = pSite;
    m_pMISUSSite->AddRef();

    // for sync
    IHXInterruptSafe* pIHXInterruptSafe = NULL;

    if (HXR_OK == m_pMISUSSite->QueryInterface(IID_IHXInterruptSafe,
					       (void**)&pIHXInterruptSafe))
    {
	// Get the pref to use the optimized scheduler or not
	BOOL bUseOptimized = TRUE;
	IHXBuffer* pPrefBuffer;

	if( m_pPreferences &&
            m_pPreferences->ReadPref("UseOptimizedScheduler",
		pPrefBuffer) == HXR_OK)
	{
	    bUseOptimized = *(pPrefBuffer->GetBuffer()) == '1';
	    HX_RELEASE(pPrefBuffer);
	}

	if (pIHXInterruptSafe->IsInterruptSafe() && bUseOptimized)
	{
	    HX_RELEASE(m_pOptimizedScheduler);
	    if (HXR_OK !=
		    m_pContext->QueryInterface(IID_IHXOptimizedScheduler,
		    (void **) &m_pOptimizedScheduler))
	    {
		// just for good luck
		m_pOptimizedScheduler = NULL;
	    }
	}
    }
    HX_RELEASE(pIHXInterruptSafe);

#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    // Get Run Configuration
    // Use of VideoSurface2
    if (m_pPreferences)
    {
	ReadPrefBOOL(m_pPreferences, "VideoBoost\\NoFlip", m_bTryVideoSurface2);
    }

    // Use of OS Granule Boosting
    if (m_pPreferences)
    {
	ReadPrefBOOL(m_pPreferences, "VideoBoost\\NoOSGranuleBoost", m_bOSGranuleBoost);
	m_bOSGranuleBoostVS2 = m_bOSGranuleBoost;
    }

    // Hardware buffer count to request from VideoSurface2
    if (m_bTryVideoSurface2 && m_pPreferences)
    {
	ReadPrefINT32(m_pPreferences, "VideoBoost\\InitialHSCount", m_ulConfigHWBufCount);
    }

#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    /*
     * This is the best time to set the size of the site, we
     * know for example the the header has already been received
     * at this point.
     *
     * In most display renderers, you will store size information
     * in your stream header. In this example, we assume a specific
     * size, but we will initialize that size here as if it had
     * come from our header.
     *
     */
    SetupBitmapDefaults(m_pHeader, m_BitmapInfoHeader);
    FormatAndSetViewFrame(m_pClipRect,
			  m_BitmapInfoHeader,
			  m_rViewRect);

    m_bBitmapSet = ((m_BitmapInfoHeader.biWidth > 0) &&
		    (m_BitmapInfoHeader.biHeight > 0));

    return HXR_OK;
}


STDMETHODIMP
CVideoRenderer::DetachSite()
{
    m_bSiteAttached = FALSE;

    HX_RELEASE(m_pMISUSSite);

    // We're done with these...
    if (m_pMISUS)
    {
	m_pMISUS->ReleaseSingleSiteUser();
    }

    HX_RELEASE(m_pMISUS);

    return HXR_OK;
}

STDMETHODIMP_(BOOL)
CVideoRenderer::NeedsWindowedSites()
{
    return FALSE;
};

STDMETHODIMP
CVideoRenderer::HandleEvent(HXxEvent* /*IN*/ pEvent)
{
    HX_RESULT retVal = HXR_OK;

    pEvent->handled = FALSE;
    pEvent->result  = 0;

    switch (pEvent->event)
    {
    case HX_SURFACE_UPDATE:
	m_pVSMutex->Lock();
	retVal = UpdateDisplay(pEvent, TRUE);
	m_pVSMutex->Unlock();
	break;

#if defined(HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO)
    case HX_SURFACE_MODE_CHANGE:
	switch ((int) pEvent->param2)
	{
	case HX_VIDEOSURFACE1_RECOMMENDED:
	    pEvent->result = SwitchToVideoSurface1();

	    if (SUCCEEDED(pEvent->result))
	    {
		pEvent->handled = TRUE;
	    }
	    break;

	case HX_VIDEOSURFACE1_NOT_RECOMMENDED:
	    pEvent->result = SwitchToVideoSurface2();

	    if (SUCCEEDED(pEvent->result))
	    {
		pEvent->handled = TRUE;
	    }
	    break;

	default:
	    HX_ASSERT(FALSE);
	    break;
	}
	break;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    default:
	// nothing to do
	break;
    }

    return retVal;
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
STDMETHODIMP_(ULONG32) CVideoRenderer::AddRef()
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
STDMETHODIMP_(ULONG32) CVideoRenderer::Release()
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
STDMETHODIMP CVideoRenderer::QueryInterface(REFIID riid, void** ppvObj)
{
    QInterfaceList  qiList[] =
    {
	{ GET_IIDHANDLE(IID_IHXCallback), (IHXCallback*)this},
	{ GET_IIDHANDLE(IID_IHXInterruptSafe), (IHXInterruptSafe*)this},
	{ GET_IIDHANDLE(IID_IHXUpdateProperties), (IHXUpdateProperties*)this},
	{ GET_IIDHANDLE(IID_IHXRenderTimeLine), (IHXRenderTimeLine*)this},

#ifdef HELIX_FEATURE_VIDREND_UNTIMED_DECODE

	{ GET_IIDHANDLE(IID_IHXUntimedRenderer), (IHXUntimedRenderer*)this},

#endif

	{ GET_IIDHANDLE(IID_IHXPaceMakerResponse), (IHXPaceMakerResponse*)this},
	{ GET_IIDHANDLE(IID_IUnknown), (IUnknown*)(IHXPlugin*) this},
	{ GET_IIDHANDLE(IID_IHXPlugin), (IHXPlugin*)this},
	{ GET_IIDHANDLE(IID_IHXRenderer), (IHXRenderer*)this},
	{ GET_IIDHANDLE(IID_IHXSiteUser), (IHXSiteUser*)this},
	{ GET_IIDHANDLE(IID_IHXStatistics), (IHXStatistics*)this},
    };

    if (QIFind(qiList, QILISTSIZE(qiList), riid, ppvObj) == HXR_OK)
    {
	return HXR_OK ;
    }
    else if (m_pMISUS && IsEqualIID(riid, IID_IHXSiteUserSupplier))
    {
	return m_pMISUS->QueryInterface(IID_IHXSiteUserSupplier,ppvObj);
    }
    else
    {
	*ppvObj = NULL;
	return HXR_NOINTERFACE;
    }
}


/****************************************************************************
 *  Renderer's customizable fuctions - can be called any time
 */
/****************************************************************************
 *  GetStreamVersion
 */
void CVideoRenderer::GetStreamVersion(ULONG32 &ulThisMajorVersion,
					      ULONG32 &ulThisMinorVersion)
{
    ulThisMajorVersion = STREAM_MAJOR_VERSION;
    ulThisMinorVersion = STREAM_MINOR_VERSION;
}

/****************************************************************************
 *  GetContentVersion
 */
void CVideoRenderer::GetContentVersion(ULONG32 &ulThisMajorVersion,
					       ULONG32 &ulThisMinorVersion)
{
    ulThisMajorVersion = CONTENT_MAJOR_VERSION;
    ulThisMinorVersion = CONTENT_MINOR_VERSION;
}

/****************************************************************************
 *  GetUpgradeMimeType
 */
const char* CVideoRenderer::GetUpgradeMimeType(void)
{
    const char** pStreamMimeTypes = NULL;
    UINT32 ulInitialGranularity;

    GetRendererInfo(pStreamMimeTypes, ulInitialGranularity);

    if (pStreamMimeTypes)
    {
	return pStreamMimeTypes[0];
    }

    return NULL;
}

/****************************************************************************
 *  GetRendererName
 */
const char* CVideoRenderer::GetRendererName(void)
{
    return BASE_VIDEO_RENDERER_NAME;
}

/****************************************************************************
 *  GetCodecName
 */
const char* CVideoRenderer::GetCodecName(void)
{
    return NULL;
}

/****************************************************************************
 *  GetCodecFourCC
 */
const char* CVideoRenderer::GetCodecFourCC(void)
{
    return NULL;
}

/****************************************************************************
 *  GetLateFrameTolerance
 */
ULONG32 CVideoRenderer::GetLateFrameTolerance(void)
{
    return LATE_FRAME_TOL;
}

/****************************************************************************
 *  GetEarlyFrameTolerance
 */
ULONG32 CVideoRenderer::GetEarlyFrameTolerance(void)
{
    return EARLY_FRAME_TOL;
}

/****************************************************************************
 *  GetMaxOptimizedVideoLead
 */
ULONG32 CVideoRenderer::GetMaxOptimizedVideoLead(void)
{
    return MAX_OPTIMIZED_VIDEO_LEAD;
}

/****************************************************************************
 *  GetBltPacketQueueSize
 */
ULONG32 CVideoRenderer::GetBltPacketQueueSize(void)
{
    ULONG32 ulSize = BLT_PACKET_QUEUE_SIZE;

    if (m_pVideoFormat)
    {
	m_pVideoFormat->GetMaxDecodedFrames();
    }

    return ulSize;
}

/****************************************************************************
 *  GetSyncGoalSmoothingDepth
 */
ULONG32 CVideoRenderer::GetSyncGoalSmoothingDepth(void)
{
    return SYNC_GOAL_SMOOTHING_DEPTH;
}

/****************************************************************************
 *  GetSpeedupGoalSmoothingDepth
 */
ULONG32 CVideoRenderer::GetSpeedupGoalSmoothingDepth(void)
{
    return SPEEDUP_GOAL_SMOOTHING_DEPTH;
}

/****************************************************************************
 *  GetNoFramesPollingInterval
 */
ULONG32 CVideoRenderer::GetNoFramesPollingInterval(void)
{
    return NO_FRAMES_POLLING_INTERVAL;
}

/****************************************************************************
 *  GetMaxSleepTime
 */
ULONG32 CVideoRenderer::GetMaxSleepTime(void)
{
    return m_ulMaxSleepTime;
}


/****************************************************************************
 *  GetMaxSleepTime
 */
ULONG32 CVideoRenderer::GetMaxBadSeqSamples(void)
{
    return MAX_BAD_SAMPLE_INTERVAL / m_ulSyncInterval;
}


/****************************************************************************
 *  GetDecodePriority
 */
LONG32 CVideoRenderer::GetDecodePriority(void)
{
    return m_lDecodePriority;
}


HX_RESULT CVideoRenderer::SetDecodePriority(LONG32 lPriority)
{
    HX_RESULT retVal = HXR_OK;

    if (m_pDecoderPump)
    {
	retVal = m_pDecoderPump->SetPriority(lPriority);
    }

    if (SUCCEEDED(retVal))
    {
	m_lDecodePriority = lPriority;
    }

    return retVal;
}


/****************************************************************************
 *  CreateFormatObject
 */
CVideoFormat* CVideoRenderer::CreateFormatObject(IHXValues* pHeader)
{
    return new CVideoFormat(m_pCommonClassFactory,
			    this);
}

/****************************************************************************
 *  SetupBitmapDefaults
 */
void CVideoRenderer::SetupBitmapDefaults(IHXValues* pHeader,
					   HXBitmapInfoHeader &bitmapInfoHeader)
{
    // size calculation is taken from crvvideo
    bitmapInfoHeader.biSize = sizeof (HXBitmapInfoHeader);
    bitmapInfoHeader.biWidth = 0; // 352;	// unknown
    bitmapInfoHeader.biHeight = 0; // 288;	// unknown
    bitmapInfoHeader.biPlanes = 1;
    bitmapInfoHeader.biBitCount = 24;
    bitmapInfoHeader.biCompression = HX_I420;
    bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth *
				     bitmapInfoHeader.biHeight *
				     bitmapInfoHeader.biBitCount /
				     8;
    bitmapInfoHeader.biXPelsPerMeter = 0;
    bitmapInfoHeader.biYPelsPerMeter = 0;
    bitmapInfoHeader.biClrUsed = 0;
    bitmapInfoHeader.biClrImportant = 0;
    bitmapInfoHeader.rcolor	= 0;
    bitmapInfoHeader.gcolor	= 0;
    bitmapInfoHeader.bcolor	= 0;
}

/****************************************************************************
 *  FormatAndSetViewFrame
 */
void CVideoRenderer::FormatAndSetViewFrame(HXxRect* pClipRect,
					   HXBitmapInfoHeader &bitmapInfoHeader,
					   HXxRect &rViewRect,
					   BOOL bMutex)
{
    BOOL bAsDefault = TRUE;
    HXxSize szViewFrame;

    if (bMutex)
    {
	DisplayMutex_Lock();
    }

    if (pClipRect)
    {
	rViewRect = *pClipRect;

	// Insure the ViewRect is inside the bitmap Rect
	// Clip x
	rViewRect.left = (rViewRect.left > 0) ?
	    rViewRect.left : 0;
	rViewRect.right = (rViewRect.right > 0) ?
	    rViewRect.right : 0;
	rViewRect.left = (rViewRect.left < bitmapInfoHeader.biWidth) ?
	    rViewRect.left : bitmapInfoHeader.biWidth;
	rViewRect.right = (rViewRect.right < bitmapInfoHeader.biWidth) ?
	    rViewRect.right : bitmapInfoHeader.biWidth;

	// Clip y
	rViewRect.top = (rViewRect.top > 0) ?
	    rViewRect.top : 0;
	rViewRect.bottom = (rViewRect.bottom > 0) ?
	    rViewRect.bottom : 0;
	rViewRect.top = (rViewRect.top < bitmapInfoHeader.biHeight) ?
	    rViewRect.top : bitmapInfoHeader.biHeight;
	rViewRect.bottom = (rViewRect.bottom < bitmapInfoHeader.biHeight) ?
	    rViewRect.bottom : bitmapInfoHeader.biHeight;
    }
    else
    {
	rViewRect.left = 0;
	rViewRect.top = 0;
	rViewRect.right = bitmapInfoHeader.biWidth;
	rViewRect.bottom = bitmapInfoHeader.biHeight;
    }

    // Compute Size
    szViewFrame.cx = rViewRect.right - rViewRect.left;
    szViewFrame.cy = rViewRect.bottom - rViewRect.top;

    if ((szViewFrame.cx <= 0) || (szViewFrame.cy <= 0))
    {
	if (m_pClipRect)
	{
	    szViewFrame.cx = m_pClipRect->right - m_pClipRect->left;
	    szViewFrame.cy = m_pClipRect->bottom - m_pClipRect->top;
	}

	if ((szViewFrame.cx <= 0) || (szViewFrame.cy <= 0))
	{
	    szViewFrame.cx = DEFAULT_WIN_SIZE_X;
	    szViewFrame.cy = DEFAULT_WIN_SIZE_Y;
	}
    }

    if (m_pClipRect)
    {
	bAsDefault = FALSE;
    }

    _ResizeViewFrame(szViewFrame, FALSE, TRUE, bAsDefault);

    if (bMutex)
    {
	DisplayMutex_Unlock();
    }
}


/****************************************************************************
 *  ResizeViewFrame
 */
BOOL CVideoRenderer::ResizeViewFrame(HXxSize szViewFrame,
				     BOOL bMutex)
{
    HX_RESULT retVal;

    retVal = _ResizeViewFrame(szViewFrame,
			      bMutex,
			      FALSE,
			      FALSE);

    return retVal;
}

BOOL CVideoRenderer::_ResizeViewFrame(HXxSize szViewFrame,
				      BOOL bMutex,
				      BOOL bForceSyncResize,
				      BOOL bAsDefault)
{
    BOOL bRetVal = FALSE;

    if (m_bFrameSizeInitialized)
    {
	return bRetVal;
    }

    if (bMutex)
    {
	DisplayMutex_Lock();
    }

    if (!m_bFrameSizeInitialized)
    {
	// If window size is already set, ignore further attempts to
	// resize
	if (m_bWinSizeFixed)
	{
	    szViewFrame.cx = m_SetWinSize.cx;
	    szViewFrame.cy = m_SetWinSize.cy;
	}

	// If resulting size invalid, default to cliprect or bitmap size
	if ((szViewFrame.cx <= 0) || (szViewFrame.cy <= 0))
	{
	    if (((szViewFrame.cx <= 0) || (szViewFrame.cy <= 0)) &&
		m_pClipRect)
	    {
		szViewFrame.cx = m_pClipRect->right - m_pClipRect->left;
		szViewFrame.cy = m_pClipRect->bottom - m_pClipRect->top;
	    }

	    if ((szViewFrame.cx <= 0) || (szViewFrame.cy <= 0))
	    {
		szViewFrame.cx = m_BitmapInfoHeader.biWidth;
		szViewFrame.cy = m_BitmapInfoHeader.biHeight;
	    }
	}

	m_SetWinSize.cx = szViewFrame.cx;
	m_SetWinSize.cy = szViewFrame.cy;

#if !defined(HELIX_FEATURE_VIDREND_DYNAMIC_RESIZE)
	m_bWinSizeFixed = (m_bWinSizeFixed || (!bAsDefault));
#else
        HX_ASSERT(!m_bWinSizeFixed);
#endif

#ifdef RESIZE_AFTER_SITE_ATTACHED
	if (m_bSiteAttached)
#endif	// RESIZE_AFTER_SITE_ATTACHED
	{
#ifdef SET_NONZERO_VIEWFRAME_ONLY
	    if ((szViewFrame.cx > 0) && (szViewFrame.cy > 0))
#endif	// SET_NONZERO_VIEWFRAME_ONLY
	    {
#ifdef SYNC_RESIZE_OK
		bForceSyncResize = TRUE;
#endif	// SYNC_RESIZE_OK

		if ((m_LastSetSize.cx != szViewFrame.cx) ||
		    (m_LastSetSize.cy != szViewFrame.cy))
		{
		    m_LastSetSize = szViewFrame;

		    if (bForceSyncResize)
		    {
			m_pMISUSSite->SetSize(szViewFrame);
		    }
		    else
		    {
			if (m_pResizeCB == NULL)
			{
			    m_pResizeCB = new CSetSizeCB(m_pMISUSSite);

			    HX_ASSERT(m_pResizeCB);

			    if (m_pResizeCB)
			    {
				m_pResizeCB->AddRef();
			    }
			}

			if (m_pResizeCB)
			{
			    m_pResizeCB->SetSize(szViewFrame);

			    HX_ASSERT(m_pScheduler);

			    if (m_pScheduler)
			    {
				m_pScheduler->RelativeEnter(m_pResizeCB, 0);
			    }
			}
		    }
		}
	    }

	    // Once the the frame size is initialized, it is no longer
	    // changable by the renderer.
	    // The frame size can become initialzied only of the window
	    // size is fixed. It can become fixed only if explicitly set
	    // by non-default mode call to ResizeViewFrame.  ResizeViewFrame
	    // can be called in non-default mode by either the video format
	    // or the call can be made internally based on meta-header
	    // specified information (e.g. clip rect.)
	    if (m_bWinSizeFixed)
	    {
		m_bFrameSizeInitialized = TRUE;
		bRetVal = TRUE;
	    }
	}
    }

    if (bMutex)
    {
	DisplayMutex_Unlock();
    }

    return bRetVal;
}


/****************************************************************************
 *  SetSyncInterval
 */
void CVideoRenderer::SetSyncInterval(ULONG32 ulSyncInterval)
{
    if (ulSyncInterval != 0)
    {
	m_ulSyncInterval = ulSyncInterval;
    }
}


/****************************************************************************
 *  InitExtraStats
 */
HX_RESULT CVideoRenderer::InitExtraStats(void)
{
    return HXR_OK;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::AdjustVideoMapping
 *
 */
void CVideoRenderer::AdjustVideoMapping(HXxRect &destRect,
					HXxRect &sorcRect,
					CMediaPacket*pActiveVideoPacket)
{
    return;
}

/****************************************************************************
 *  Renderer's private fuctions
 */
BOOL CVideoRenderer::ForceRefresh(void)
{
    BOOL bIsVisible;

    // Ask what the size was really set to!
    HXxSize finalSize;
    m_pMISUSSite->GetSize(finalSize);

    /* We now damage the entire rect all the time             */
    HXxRect damageRect = {0, 0, finalSize.cx, finalSize.cy};

    // Mark associated screen area as damaged as well...
    m_pMISUSSite->DamageRect(damageRect);

    HX_ASSERT(!m_bPendingRedraw);
    HX_ASSERT(!m_bVS1UpdateInProgress);

    m_bPendingRedraw = TRUE;
    m_bVS1UpdateInProgress = TRUE;

    m_pVSMutex->Unlock();
    m_pMISUSSite->ForceRedraw();
    m_pVSMutex->Lock();
    m_bVS1UpdateInProgress = FALSE;

    // If the redraw is still pending, it did not occur so assume the
    // surface is invisible
    bIsVisible = !m_bPendingRedraw;
    m_bPendingRedraw = FALSE;

    return bIsVisible;
}

HX_RESULT CVideoRenderer::UpdateDisplay(HXxEvent* pEvent,
					BOOL bSystemEvent,
					BOOL bIsVisible)
{
    IHXVideoSurface* pVideoSurface;
    CMediaPacket* pTmpPacket;
    CMediaPacket* pVideoPacket = NULL;
    BOOL bBitmapFormatChanged = FALSE;
    BOOL bNewFrameBlt = FALSE;
    HXxSize windowSize = {0, 0};
    LONG32 lFramesInBuffer;
    HX_RESULT retVal = HXR_OK;

    if (m_bVS1UpdateInProgress && m_bUseVideoSurface2)
    {
	// Ignore update for incorrect surface
	return HXR_OK;
    }

    lFramesInBuffer = m_pBltPacketQueue->Count();

    m_bPendingRedraw = FALSE;

    HX_TRACE_THINGY("C:/trace.txt", "Handle", m_hPendingHandle);

    if (!m_bUseVideoSurface2 || !bSystemEvent)
    {
	do
	{
	    pVideoPacket = (CMediaPacket*) m_pBltPacketQueue->Get();
	    lFramesInBuffer--;


	    if ((lFramesInBuffer <= 0) ||
		(!m_pVideoFormat->IsFrameSkippable(pVideoPacket)))
	    {
		bNewFrameBlt = TRUE;
		break;
	    }

#if defined(HELIX_FEATURE_STATS)
	    m_pVideoStats->ReportDroppedFrame();
#endif /* HELIX_FEATURE_STATS */

	    ReleasePacket(pVideoPacket);
	} while (TRUE);
    }

    pVideoSurface = (IHXVideoSurface*) (pEvent->param1);

    if (pVideoPacket)
    {
	if (m_bVS2BufferUnavailableOnLastBlt &&
	    m_pActiveVideoPacket)
	{
	    m_bVS2BufferUnavailableOnLastBlt = FALSE;
#if defined(HELIX_FEATURE_STATS)
	    m_pVideoStats->ReportDroppedFrame();
#endif /* HELIX_FEATURE_STATS */
	}

	pTmpPacket = pVideoPacket;
	pVideoPacket = m_pActiveVideoPacket;
	m_pActiveVideoPacket = pTmpPacket;
	m_ulActiveVideoTime = pTmpPacket->m_ulTime;

	bBitmapFormatChanged = (m_pActiveVideoPacket->m_pSampleDesc &&
				m_pVideoFormat->IsBitmapFormatChanged(
					m_BitmapInfoHeader,
					m_pActiveVideoPacket));
    }

    if (((!m_bVideoSurfaceInitialized) && m_pActiveVideoPacket) ||
	m_bVideoSurfaceReinitRequested ||
	bBitmapFormatChanged)
    {
	BOOL bUsedVideoSurface2 = m_bUseVideoSurface2;

	if (bBitmapFormatChanged)
	{
	    m_pVideoFormat->InitBitmapInfoHeader(m_BitmapInfoHeader,
						 m_pActiveVideoPacket);
	    m_bBitmapSet = TRUE;
	    m_bVideoSurfaceInitialized = FALSE;	// Force full reinit
	}

	FormatAndSetViewFrame(m_pClipRect,
			      m_BitmapInfoHeader,
			      m_rViewRect,
			      FALSE);

	HX_ASSERT(m_bBitmapSet);
	
	// If we do not have information on the bitmap configuration or if
	// we do not have surface to cofigure - do not intialize
	if (m_bBitmapSet && pVideoSurface)
	{
	    // Try VideoSurface2 first unless not desired
	    if (m_bTryVideoSurface2 && (!m_bVideoSurface1Requested))
	    {
		InitVideoSurface2(m_SetWinSize.cx, m_SetWinSize.cy);
	    }

	    // Try VideoSurface1 if prefered or if VideoSurface2 would
	    // not initialize
	    if ((!m_bUseVideoSurface2) || m_bVideoSurface1Requested)
	    {
		// Video Surface1 must be used
		InitVideoSurface1(bUsedVideoSurface2, pVideoSurface);
	    }

	    m_bVideoSurfaceInitialized = TRUE;
	    m_bVideoSurfaceReinitRequested = FALSE;

	    m_bSchedulerStartRequested = TRUE;


	    // Force the cycle in the decoder pump to expedite the completion
	    // of the scheduler start request.
	    if (IsDecoderRunning())
	    {
		if (m_pDecoderPump)
		{
		    m_pDecoderPump->Signal();
		}
	    }

#if defined(HELIX_FEATURE_STATS)
	    ReportStat(VS_IMAGE_WIDTH, (INT32) (m_rViewRect.right - m_rViewRect.left));
	    ReportStat(VS_IMAGE_HEIGHT, (INT32) (m_rViewRect.bottom - m_rViewRect.top));
#endif /* #if defined(HELIX_FEATURE_STATS) */
	}

        // Now that we have the video dimensions, check to see that this clip
        // is not going to be too taxing on the poor CPU.
        UINT32 ulMaxVidWidth = 0;
        UINT32 ulMaxVidHeight = 0;
        if( m_pPreferences && 
            ( HXR_OK == ReadPrefINT32( m_pPreferences, "MaxVideoWidth", ulMaxVidWidth ) ) &&
            ( HXR_OK == ReadPrefINT32( m_pPreferences, "MaxVideoHeight", ulMaxVidHeight ) ) )
        {
            ULONG32 ulMaxVidArea = ulMaxVidWidth * ulMaxVidHeight;
            INT32 nWidth = (INT32) (m_rViewRect.right - m_rViewRect.left);
            INT32 nHeight = (INT32) (m_rViewRect.bottom - m_rViewRect.top);
            if( (ULONG32)(nWidth*nHeight) > ulMaxVidArea )
            {
                IHXErrorMessages * pErrMsg = NULL;
                m_pContext->QueryInterface(IID_IHXErrorMessages, (void**)&pErrMsg);
                if( pErrMsg )
                {
                    pErrMsg->Report(HXLOG_ERR, HXR_SLOW_MACHINE, 0, NULL, NULL);
                    HX_RELEASE(pErrMsg);
                }
            }
        }
    }
    
    if (m_pActiveVideoPacket && bIsVisible && pVideoSurface)
    {
	HX_ASSERT(m_bVideoSurfaceInitialized);

	if (m_bVideoSurfaceInitialized && m_pActiveVideoPacket->m_pData)
	{
#ifdef DEFAULT_VS2_TARGETRECT
	    if (!m_bUseVideoSurface2)
#endif	// DEFAULT_VS2_TARGETRECT
	    {
		m_pMISUSSite->GetSize(windowSize);
	    }

	    HXxRect rDestRect = { 0, 0, windowSize.cx, windowSize.cy };
	    HXxRect rSrcRect = m_rViewRect;

	    AdjustVideoMapping(rDestRect, rSrcRect, m_pActiveVideoPacket);

	    if (m_bUseVideoSurface2)
	    {
		pEvent->result = UpdateVideoSurface2(pVideoSurface,
						     rDestRect,
						     rSrcRect,
						     !bNewFrameBlt,
						     bSystemEvent);

		if (pEvent->result == HXR_ABORT)
		{
		    pEvent->result = HXR_OK;
		    retVal = HXR_ABORT;
		}
	    }
	    else if (bSystemEvent)
	    {
		pEvent->result = UpdateVideoSurface(pVideoSurface,
						    m_pActiveVideoPacket,
						    rDestRect,
						    rSrcRect,
						    m_bOptimizedBlt);

		if (FAILED(pEvent->result))
		{
		    if (m_bVideoSurfaceReinitRequested)
		    {
			HX_ASSERT(!m_bVideoSurface1Requested);

			InitVideoSurface2(m_SetWinSize.cx, m_SetWinSize.cy);

			if (m_bUseVideoSurface2)
			{
			    m_bVideoSurfaceInitialized = TRUE;
			    m_bVideoSurfaceReinitRequested = FALSE;

			    pEvent->result = UpdateVideoSurface2(pVideoSurface,
								 rDestRect,
								 rSrcRect,
								 FALSE,
								 bSystemEvent);

			    if (pEvent->result == HXR_ABORT)
			    {
				pEvent->result = HXR_OK;
			    }
			}
		    }
		    else
		    {
			InitVideoSurface1(FALSE, pVideoSurface);

			pEvent->result = UpdateVideoSurface(pVideoSurface,
							    m_pActiveVideoPacket,
							    rDestRect,
							    rSrcRect,
							    m_bOptimizedBlt);
		    }
		}
	    }
	    else
	    {
		// This is not a system event and this call is not under
		// the protection of the top level site (TLS) mutex and
		// it needs to be.
		// Bring it under the TLS mutex protection by envoking
		// forced refresh
		bIsVisible = ForceRefresh();

		pEvent->result = HXR_OK;
		retVal = HXR_ABORT;
	    }

	    if (bNewFrameBlt && bIsVisible)
	    {
#if defined(HELIX_FEATURE_STATS)
		if (pEvent->result == HXR_OK)
		{
		    m_pVideoStats->ReportBlt(m_pActiveVideoPacket->m_ulTime);
		}
		else if (!m_bVS2BufferUnavailableOnLastBlt)
		{
		    // If buffer wasn't available, we'll try again
		    m_pVideoStats->ReportDroppedFrame();
		}
#endif /* HELIX_FEATURE_STATS */
	    }
	}
    }

    if (pVideoPacket)
    {
	ReleasePacket(pVideoPacket, m_bActiveVideoPacketLocalized);
	m_bActiveVideoPacketLocalized = FALSE;
    }

    pEvent->handled = TRUE;

    return retVal;
}

HX_RESULT CVideoRenderer::UpdateVideoSurface(IHXVideoSurface* pVideoSurface,
					     CMediaPacket* pVideoPacket,
					     HXxRect &destRect,
					     HXxRect &sorcRect,
					     BOOL bOptimizedBlt)
{
    HX_RESULT retVal;

    if (bOptimizedBlt)
    {
	retVal = pVideoSurface->OptimizedBlt(
	    pVideoPacket->m_pData,
	    destRect,
	    sorcRect);
    }
    else
    {
	retVal = pVideoSurface->Blt(
	    pVideoPacket->m_pData,
	    &m_BitmapInfoHeader,
	    destRect,
	    sorcRect);
    }

    return retVal;
}


HX_RESULT CVideoRenderer::InitVideoSurface1(BOOL bUsedVideoSurface2,
					    IHXVideoSurface* pVideoSurface)
{
    HX_RESULT retVal = HXR_OK;

    if (bUsedVideoSurface2)
    {
	OffOptimizedVideo();
    }

    /***
    if (pVideoSurface)
    {
	retVal = pVideoSurface->EndOptimizedBlt();
    }
    else
    {
	EndOptimizedBlt();
    }
    ***/

    // Video Surface1 is needed - try optimized Blt setup
    if (!m_bVideoSurfaceInitialized)
    {
	if (pVideoSurface)
	{
	    retVal = pVideoSurface->BeginOptimizedBlt(&m_BitmapInfoHeader);
	}
	else
	{
	    retVal = BeginOptimizedBlt(&m_BitmapInfoHeader);
	}

	if (SUCCEEDED(retVal))
	{
	    m_bOptimizedBlt = TRUE;
	}
	else
	{
	    m_bOptimizedBlt = FALSE;
	}

	retVal = HXR_OK;
    }

    if (SUCCEEDED(retVal))
    {
	m_ulEarlyFrameTol = GetEarlyFrameTolerance();
	HX_DELETE(m_pVSurf2InputBIH);
	m_bUseVideoSurface2 = FALSE;
	if (m_bVS2BufferUnavailableOnLastBlt)
	{
#if defined(HELIX_FEATURE_STATS)
	    m_pVideoStats->ReportDroppedFrame();
#endif /* HELIX_FEATURE_STATS */
	    m_bVS2BufferUnavailableOnLastBlt = FALSE;
	}
    }

    return retVal;
}


inline BOOL CVideoRenderer::ShouldKickStartScheduler()
{
    BOOL bShouldStart;

    bShouldStart = (!m_bPendingCallback && !m_bUseVideoSurface2);

    return bShouldStart;
}


HX_RESULT CVideoRenderer::ScheduleCallback(UINT32 ulRelativeTime,
					   BOOL bIsScheduled,
					   UINT32 ulBaseTime)
{
    IHXCallback* pCallback = (IHXCallback*) this;
    CallbackHandle hPendingHandle;
    ULONG32 ulThisCallbackCount;

    if (m_hPendingHandle == (CallbackHandle)NULL)
    {
	m_bPendingCallback = TRUE;
	m_ulCallbackCounter++;
	if (m_ulCallbackCounter == 0)
	{
	    m_ulCallbackCounter++;
	}
	ulThisCallbackCount = m_ulCallbackCounter;
	m_hPendingHandle = ulThisCallbackCount;
	m_bIsScheduledCB = bIsScheduled;

	if (ulRelativeTime > m_ulMaxSleepTime)
	{
	    ulRelativeTime = m_ulMaxSleepTime;
	    m_bIsScheduledCB = FALSE;
	}

	HX_TRACE_THINGY("C:/trace.txt", "SchedCBTime", ulRelativeTime);

#ifdef DO_ABSOLUTE_TIMING
	LONG32 lTimeOffset;
	HXTimeval hxTime;
	ULONG32 ulNTries = N_STABILIZATION_ITERATIONS;
	ULONG32 ulCurrentTime1;
	ULONG32 ulCurrentTime2;

	if (bIsScheduled)
	{
	    // Obtain atomic time reading (avoid context switch)
	    do
	    {
		ulCurrentTime1 = HX_GET_BETTERTICKCOUNT();

		// Always use the optimized scheduler if we have one
		if (m_pOptimizedScheduler != NULL)
		{
		    hxTime = m_pOptimizedScheduler->GetCurrentSchedulerTime();
		}
		else
		{
		    hxTime = m_pScheduler->GetCurrentSchedulerTime();
		}

		ulCurrentTime2 = HX_GET_BETTERTICKCOUNT();

		lTimeOffset = (LONG32) (ulCurrentTime2 - ulBaseTime + ulRelativeTime);

		ulNTries--;
	    } while (((ulCurrentTime2 - ulCurrentTime1) > MAX_ALLOWED_TIMING_ERROR) &&
		(ulNTries != 0) &&
		(lTimeOffset > 0));

	    if (lTimeOffset >= SMALLEST_TIMABLE_PERIOD)
	    {
		hxTime.tv_usec += (lTimeOffset * MILLISECOND);
		if (hxTime.tv_usec >= SECOND)
		{
		    hxTime.tv_sec += (hxTime.tv_usec / SECOND);
		    hxTime.tv_usec %= SECOND;
		}
	    }

	    hPendingHandle = ScheduleAbsoluteCallback(hxTime, pCallback);
	}
	else
#endif  // DO_ABSOLUTE_TIMING
	{

	    hPendingHandle = ScheduleRelativeCallback(ulRelativeTime, pCallback);

	}

	HX_ASSERT(hPendingHandle);

	// Remember the handle if callback did not already fire synchronously
	if (m_hPendingHandle == ulThisCallbackCount)
	{
	    HX_TRACE_THINGY("C:/trace.txt", "SchedCBHandle", hPendingHandle);

	    m_hPendingHandle = hPendingHandle;
	    m_bPendingCallback = (m_hPendingHandle != ((CallbackHandle) NULL));
	}
    }

    return HXR_OK;
}


inline CallbackHandle CVideoRenderer::ScheduleRelativeCallback
(
    UINT32 ulRelativeTime,
    IHXCallback* pCallback
)
{
    CallbackHandle hCallback;

    // Always use the optimized scheduler if we have one
    if (m_pOptimizedScheduler != NULL)
    {
	hCallback = m_pOptimizedScheduler->RelativeEnter(
	    pCallback, ulRelativeTime);
    }
    else
    {
	hCallback = m_pScheduler->RelativeEnter(
	    pCallback, ulRelativeTime);
    }

    return hCallback;
}


inline CallbackHandle CVideoRenderer::ScheduleAbsoluteCallback
(
    HXTimeval &hxTime,
    IHXCallback* pCallback
)
{
    CallbackHandle hCallback;

    // Always use the optimized scheduler if we have one
    if (m_pOptimizedScheduler != NULL)
    {
	hCallback = m_pOptimizedScheduler->AbsoluteEnter(
	    pCallback, hxTime);
    }
    else
    {
	hCallback = m_pScheduler->AbsoluteEnter(
	    pCallback, hxTime);
    }

    return hCallback;
}


void CVideoRenderer::RemoveCallback(CallbackHandle &hCallback)
{
    if (hCallback != (CallbackHandle)NULL)
    {
	if (m_pOptimizedScheduler != NULL)
	{
	    m_pOptimizedScheduler->Remove(hCallback);
	}
	else if (m_pScheduler)
	{
	    m_pScheduler->Remove(hCallback);
	}

	hCallback = NULL;
    }
}


/*
 *   Draw any frame that's due, and schedule a new callback for the next
 *   frame
 */
void CVideoRenderer::SchedulerCallback(BOOL bIsScheduled,
				       BOOL bResched,
				       BOOL bIsVS2Call,
				       BOOL bProcessUndisplayableFramesOnly)
{
    CMediaPacket* pPkt;
    LONG32 lTimeDelta;
    ULONG32 ulNextFrameTime;
    BOOL bFrameIsSkippable;
    ULONG32 ulBaseTime;
    BOOL bHaveNextFrame;
    BOOL bDisplayFrame;
    ULONG32 ulLoopCounter = 0;

    DisplayMutex_Lock();

    while ((m_PlayState == Playing) ||
	   ((m_PlayState == PlayStarting) && m_bFirstFrame))
    {
	if (m_bUseVideoSurface2)
	{
	    if (bIsVS2Call)
	    {
		m_bVideoSurface2Transition = FALSE;
	    }
	    else
	    {
		m_bPendingCallback = FALSE;
		DisplayMutex_Unlock();
		return;
	    }
	}
	else
	{
	    if (bIsVS2Call)
	    {
		DisplayMutex_Unlock();
		return;
	    }

	    m_bVS2BufferUnavailableOnLastBlt = FALSE;
	}

#ifdef ENABLE_FETCH_TRACE
	ULONG32 ulFetchTraceStart = HX_GET_BETTERTICKCOUNT();
#endif	// ENABLE_FETCH_TRACE

	bDisplayFrame = TRUE;

#ifdef HELIX_FEATURE_VIDREND_UNTIMED_DECODE
	if( !m_bUntimedRendering )
	{
#endif /* HELIX_FEATURE_VIDREND_UNTIMED_DECODE */
	while ((bHaveNextFrame = m_pVideoFormat->GetNextFrameTime(ulNextFrameTime)) != 0)
	{

#ifdef ENABLE_FETCH_TRACE
	    if (ulFetchTraceIdx < MAX_FETCH_TRACE_ENTRIES)
	    {
		fetchTraceArray[ulFetchTraceIdx++] =
		    HX_GET_BETTERTICKCOUNT() - ulFetchTraceStart;
	    }
#endif	// ENABLE_FETCH_TRACE

	    lTimeDelta = ComputeTimeAhead(ulNextFrameTime,
					  0,
					  &ulBaseTime);

	    if (((lTimeDelta + ((LONG32) m_ulLateFrameTol)) >= 0) ||
		bIsScheduled ||
		m_bVS2BufferUnavailableOnLastBlt)
	    {
		break;
	    }
	    else
	    {
#ifdef ENABLE_SCHED_TRACE
		if (ulSchedTraceIdx < MAX_SCHED_TRACE_ENTRIES)
		{
		    schedTraceArray[ulSchedTraceIdx++] = lTimeDelta;
		}
#endif	// ENABLE_SCHED_TRACE

		bFrameIsSkippable = TRUE;

		m_pVideoFormat->IsNextFrameSkippable(bFrameIsSkippable);

		if (!bFrameIsSkippable)
		{
		    if (m_bFirstFrame)
		    {
			bDisplayFrame = FALSE;
		    }
		    break;
		}
	    }

#ifdef ENABLE_FETCH_TRACE
	    ulFetchTraceStart = HX_GET_BETTERTICKCOUNT();
#endif	// ENABLE_FETCH_TRACE

	    pPkt = m_pVideoFormat->Dequeue();

#ifdef ENABLE_FETCH_TRACE
	    if (ulFetchTraceIdx < MAX_FETCH_TRACE_ENTRIES)
	    {
		fetchTraceArray[ulFetchTraceIdx++] =
		    HX_GET_BETTERTICKCOUNT() - ulFetchTraceStart;
	    }
#endif	// ENABLE_FETCH_TRACE

	    if (!m_bFirstFrame)
	    {
#if defined(HELIX_FEATURE_STATS)
		m_pVideoStats->ReportDroppedFrame();
#endif /* HELIX_FEATURE_STATS */
	    }

	    ReleasePacket(pPkt);
	}
#ifdef HELIX_FEATURE_VIDREND_UNTIMED_DECODE
	} else {
    	    bHaveNextFrame = m_pVideoFormat->GetNextFrameTime(ulNextFrameTime);
	    lTimeDelta = 0;

	    ComputeTimeAhead( 0, 0, &ulBaseTime );

	}
#endif /* HELIX_FEATURE_VIDREND_UNTIMED_DECODE */

	if (bProcessUndisplayableFramesOnly && bDisplayFrame)
	{
	    DisplayMutex_Unlock();
	    return;
	}

	if (!m_bVS2BufferUnavailableOnLastBlt)
	{
	    if (bHaveNextFrame)
	    {
		if ((lTimeDelta > ((LONG32) m_ulEarlyFrameTol)) &&
		    (!m_bFirstFrame))
		{
		    if (bResched)
		    {
			ScheduleCallback(lTimeDelta, TRUE, ulBaseTime);
		    }

		    DisplayMutex_Unlock();

		    return;
		}
	    }
	    else
	    {
		if (bResched)
		{
		    // m_pVideoFormat->DecodeFrame();  MBO: Give user Input priority
		    ScheduleCallback(m_ulNoFramesPollingInterval);
		}

		DisplayMutex_Unlock();

		return;
	    }

	    // Render
	    pPkt = m_pVideoFormat->Dequeue();
	}
	else
	{
	    // We need to reblt - but if there is a frame that's up to date
	    // use it instead of old failed frame.
	    pPkt = NULL;
	    if (bHaveNextFrame && (lTimeDelta <= 0))
	    {
		pPkt = m_pVideoFormat->Dequeue();
	    }
	}

	if (pPkt != NULL)
	{
#ifdef ENABLE_SCHED_TRACE
	    if (ulSchedTraceIdx < MAX_SCHED_TRACE_ENTRIES)
	    {
		schedTraceArray[ulSchedTraceIdx++] = lTimeDelta;
	    }
#endif	// ENABLE_SCHED_TRACE

	    if (m_pMISUSSite &&
		m_pBltPacketQueue->Put(pPkt))
	    {
		if (pPkt->m_pData && bDisplayFrame)
		{
		    m_bFirstFrame = FALSE;
		}

		if (m_bUseVideoSurface2)
		{
		    ForceDisplayUpdate(FALSE, bDisplayFrame);

		    if (m_bVS2BufferUnavailableOnLastBlt)
		    {
			// We couldn't blt frame because video buffer
			// wasn't available, try again a bit later
			DisplayMutex_Unlock();
			return;
		    }
		}
		else
		{
		    if ((!bDisplayFrame) || (!ForceRefresh()))
		    {
			// Site redraw did not occur - treat this as if the surface
			// is invisible
			ForceDisplayUpdate(FALSE, FALSE);
			HX_ASSERT(!m_bPendingRedraw);
		    }
		}
	    }
	    else
	    {
#if defined(HELIX_FEATURE_STATS)
		m_pVideoStats->ReportDroppedFrame();
#endif /* HELIX_FEATURE_STATS */
		ReleasePacket(pPkt);
	    }
	}
	else
	{
	    // This is a reblt servicing
	    if (m_bUseVideoSurface2)
	    {
		HX_ASSERT(bDisplayFrame);

		ForceDisplayUpdate(FALSE, bDisplayFrame);

		if (m_bVS2BufferUnavailableOnLastBlt)
		{
		    // We couldn't blt frame because video buffer
		    // wasn't available, try again a bit later
		    DisplayMutex_Unlock();
		    return;
		}
	    }
	}

	if (bResched)
	{
	    ulLoopCounter++;

	    if (ulLoopCounter > MAX_BLT_LOOPS)
	    {
		ScheduleCallback(BLT_RELIEF_DELAY);

		DisplayMutex_Unlock();

		return;
	    }
	}
	else
	{
	    if (!bIsVS2Call)
	    {
		DisplayMutex_Unlock();

		return;
	    }
	}

	bIsScheduled = FALSE;
    }

    if ((m_PlayState != Stopped) &&
	bResched)
    {
	ScheduleCallback(NO_FRAMES_POLLING_INTERVAL);

	DisplayMutex_Unlock();

	return;
    }

    m_bPendingCallback = FALSE;

    DisplayMutex_Unlock();
}

void CVideoRenderer::ForceDisplayUpdate(BOOL bInternalSurfaceUpdateOnly,
					BOOL bHasVisibleSurface)
{
    // Create fake events for HandleSurfaceUpdate:
    HX_ASSERT(m_pMISUSSite);

#if defined (HELIX_FEATURE_MISU)
    IHXSiteEnumerator* pSiteEnumerator;
    if (SUCCEEDED(m_pMISUSSite->QueryInterface(IID_IHXSiteEnumerator,
					       (void**) &pSiteEnumerator)))
    {
	IHXSite* pSite;
	IHXSiteEnumerator::SitePosition nextPosition;

	if (FAILED(pSiteEnumerator->GetFirstSite(pSite, nextPosition)))
	{
	    HX_ASSERT(FALSE);
	}
	else
	{
	    BOOL bKeepUpdating = TRUE;

	    do
	    {
		bKeepUpdating = (ForceDisplayUpdateOnSite(pSite, 
							  bInternalSurfaceUpdateOnly, 
							  bHasVisibleSurface) != HXR_ABORT);
		pSite->Release();
	    }
	    while (bKeepUpdating &&
		   SUCCEEDED(pSiteEnumerator->GetNextSite(pSite, nextPosition)));
	}

	pSiteEnumerator->Release();
    }
    else
    {
	ForceDisplayUpdateOnSite(m_pMISUSSite,
				 bInternalSurfaceUpdateOnly,
				 bHasVisibleSurface);
    }
#else	//HELIX_FEATURE_MISU

    ForceDisplayUpdateOnSite(m_pMISUSSite,
			     bInternalSurfaceUpdateOnly,
			     bHasVisibleSurface);

#endif	//HELIX_FEATURE_MISU

}

HX_RESULT CVideoRenderer::ForceDisplayUpdateOnSite(IHXSite* pSite, 
						   BOOL bInternalSurfaceUpdateOnly,
						   BOOL bHasVisibleSurface)
{
    HX_RESULT retVal = HXR_OK;

    IHXVideoSurface2* pVideoSurface2;
    IHXSite2* pSite2 = NULL;
    IHXVideoSurface* pVideoSurface = NULL;
    
    if (SUCCEEDED(pSite->QueryInterface(IID_IHXSite2, (void**) &pSite2)))
    {
	pSite2->GetVideoSurface(pVideoSurface);
    }

    if (bInternalSurfaceUpdateOnly)
    {
	if (pVideoSurface)
	{
	    if (SUCCEEDED(pVideoSurface->QueryInterface(IID_IHXVideoSurface2, 
							(void**) &pVideoSurface2)))
	    {
		pVideoSurface2->PresentIfReady();
		pVideoSurface2->Release();
	    }
	}
    }
    else
    {
	HXxEvent fakeEvent;

	fakeEvent.param1 = (void*) pVideoSurface;
	retVal = UpdateDisplay(&fakeEvent, FALSE, bHasVisibleSurface);
    }
    
    HX_RELEASE(pVideoSurface);
    HX_RELEASE(pSite2);

    return retVal;
}


STDMETHODIMP CVideoRenderer::Func(void)
{
    HX_TRACE_THINGY("C:/trace.txt", "Func", 0);

    m_hPendingHandle = NULL;

    SchedulerCallback(m_bIsScheduledCB);

    return HXR_OK;
}


HX_RESULT CVideoRenderer::BeginOptimizedBlt(HXBitmapInfoHeader* pBitmapInfo)
{
    HX_RESULT retVal = HXR_UNEXPECTED;
    IHXSite2* pMISUSSite2   = NULL;
    IHXVideoSurface* pVideoSurface = NULL;

    if (m_pMISUSSite)
    {
        m_pMISUSSite->QueryInterface(IID_IHXSite2, (void**)&pMISUSSite2);
        if (pBitmapInfo && pMISUSSite2)
        {
            if (SUCCEEDED(pMISUSSite2->GetVideoSurface(pVideoSurface)))
            {
                retVal = pVideoSurface->BeginOptimizedBlt(pBitmapInfo);
                if (retVal == HXR_OK)
                {
                    m_bOptimizedBlt = TRUE;
                }
                HX_RELEASE(pVideoSurface);
            }
        }
	HX_RELEASE(pMISUSSite2);
    }

    return retVal;
}


void CVideoRenderer::EndOptimizedBlt(void)
{
    IHXSite2* pMISUSSite2 = NULL;
    IHXVideoSurface* pVideoSurface = NULL;

    if (m_bOptimizedBlt && m_pMISUSSite)
    {
	m_pMISUSSite->QueryInterface(IID_IHXSite2, (void**)&pMISUSSite2);

	if (pMISUSSite2)
	{
	    pMISUSSite2->GetVideoSurface(pVideoSurface);
	    if (pVideoSurface)
	    {
		pVideoSurface->EndOptimizedBlt();
		m_bOptimizedBlt = FALSE;
		HX_RELEASE(pVideoSurface);
	    }
	}
    }

    HX_RELEASE(pMISUSSite2);
}


inline void CVideoRenderer::RequestBuffering(void)
{
    m_bBufferingNeeded = TRUE;
}


inline void CVideoRenderer::RequestBufferingEnd(void)
{
    m_bBufferingNeeded = FALSE;
}


HX_RESULT CVideoRenderer::BeginBuffering(void)
{
    HX_RESULT retVal = HXR_FAIL;
#ifdef REBUFFER_ON_VIDEO

    m_ulBufferingStartTime = HX_GET_BETTERTICKCOUNT();

    // If this is a reccuring rebuffer, start buffering only if
    // the preroll period passed since the last buffering completion.
    // This is done to prevent buffering all the time.
    if (!m_bBufferingOccured ||
	((m_ulBufferingStartTime - m_ulBufferingStopTime) > m_ulPreroll))
    {
	// Bytes to Buffer could be used for Predata
	// - Predata was not found beneficial yet
	m_ulBytesToBuffer = (ULONG32) (((double) m_ulPreroll) *
				       ((double) m_ulAvgBitRate) /
				       8000.0);

	m_ulBufferingTimeOut = m_ulPreroll * 2;

	if (m_ulBufferingTimeOut > 0)
	{
	    m_PlayState = Buffering;

	    retVal = m_pStream->ReportRebufferStatus(1, 0);
	}
    }
#endif	// REBUFFER_ON_VIDEO

    return retVal;
}


HX_RESULT CVideoRenderer::EndBuffering(void)
{
    HX_RESULT retVal = HXR_UNEXPECTED;

#ifdef REBUFFER_ON_VIDEO

    if (m_PlayState == Buffering)
    {
	m_PlayState = PlayStarting;
	m_bBufferingOccured = TRUE;
	m_ulBufferingStopTime = HX_GET_BETTERTICKCOUNT();

	HX_ASSERT(m_pStream);
	retVal = m_pStream->ReportRebufferStatus(1,1);
    }
#endif	// REBUFFER_ON_VIDEO

    return retVal;
}


inline BOOL CVideoRenderer::IsBufferingComplete(IHXPacket *pPacket)
{
#ifdef REBUFFER_ON_VIDEO
    ULONG32 ulTimeNow = HX_GET_BETTERTICKCOUNT();

    if ((m_PlayState == Buffering) && (m_bBufferingNeeded))
    {
	if (pPacket &&
	    ((pPacket->GetTime() - m_ulBaseTime) > m_ulPreroll))
	{
	    return TRUE;
	}

	return ((ulTimeNow - m_ulBufferingStartTime) > m_ulBufferingTimeOut);
    }
#endif	// REBUFFER_ON_VIDEO

    return TRUE;
}


HX_RESULT CVideoRenderer::LocalizeActiveVideoPacket(void)
{
    HX_RESULT retVal = HXR_IGNORE;

    DisplayMutex_Lock();

    if (m_pActiveVideoPacket && (!m_bActiveVideoPacketLocalized))
    {
	CMediaPacket* pLocalPacket = NULL;
	ULONG32 ulDataSize = m_pActiveVideoPacket->m_ulDataSize;
	UINT8* pData = new UINT8 [ulDataSize];

	retVal = HXR_OUTOFMEMORY;
	if (pData)
	{
	    retVal = HXR_OK;
	}

	if (retVal == HXR_OK)
	{
	    pLocalPacket = new CMediaPacket(pData,
					    pData,
					    ulDataSize,
					    ulDataSize,
					    m_pActiveVideoPacket->m_ulTime,
					    m_pActiveVideoPacket->m_ulFlags,
					    NULL);

	    retVal = HXR_OUTOFMEMORY;
	    if (pLocalPacket)
	    {
		retVal = HXR_OK;
	    }
	}

	if (retVal == HXR_OK)
	{
	    memcpy(pData, m_pActiveVideoPacket->m_pData, ulDataSize); /* Flawfinder: ignore */
	    ReleasePacket(m_pActiveVideoPacket);
	    m_pActiveVideoPacket = pLocalPacket;
	    m_bActiveVideoPacketLocalized = TRUE;
	}
	else
	{
	    HX_VECTOR_DELETE(pData);
	    HX_DELETE(pLocalPacket);
	}
    }

    DisplayMutex_Unlock();

    return retVal;
}


void CVideoRenderer::ReleaseFramePacket(CMediaPacket* pPacket)
{
    ReleasePacket(pPacket);
}


void CVideoRenderer::ReleasePacket(CMediaPacket* pPacket,
					  BOOL bForceKill)
{
    CHXBufferPool* pFramePool = NULL;

    if (m_pVideoFormat)
    {
	m_pVideoFormat->OnDecodedPacketRelease(pPacket);
	pFramePool = m_pVideoFormat->GetFramePool();
    }

    if (pPacket)
    {
	if (pFramePool && (!bForceKill))
	{
	    pFramePool->Put(pPacket);
	}
	else
	{
	    pPacket->Clear();
	    delete pPacket;
	}
    }

    if (IsDecoderRunning())
    {
	if (m_pDecoderPump)
	{
	    m_pDecoderPump->Signal();
	}
    }
    else
    {
	if (m_pVideoFormat)
	{
	    m_pVideoFormat->DecodeFrame();
	}
    }
}


void CVideoRenderer::ClearBltPacketQueue(void)
{
    CMediaPacket* pVideoPacket;

    if (m_pBltPacketQueue)
    {
	while ((pVideoPacket = (CMediaPacket*) m_pBltPacketQueue->Get()) != 0)
	{
	    ReleasePacket(pVideoPacket);
	}
    }
}


/****************************************************************************
 *  CVideoRenderer::CSetSizeCB
 *  This routine increases the object reference count in a thread safe
 *  manner. The reference count is used to manage the lifetime of an object.
 *  This method must be explicitly called by the user whenever a new
 *  reference to an object is used.
 */
/****************************************************************************
 *  CVideoRenderer::CSetSizeCB::QueryInterface
 */
STDMETHODIMP CVideoRenderer::CSetSizeCB::QueryInterface(REFIID riid,
							  void** ppvObj)
{
    QInterfaceList  qiList[] =
    {
	{ GET_IIDHANDLE(IID_IUnknown), (IUnknown*)(IHXPlugin*) this},
	{ GET_IIDHANDLE(IID_IHXCallback), (IHXCallback*)this},
    };

    return ::QIFind(qiList, QILISTSIZE(qiList), riid, ppvObj);
}

/****************************************************************************
 *  CVideoRenderer::CSetSizeCB::AddRef                             \
 */
STDMETHODIMP_(ULONG32) CVideoRenderer::CSetSizeCB::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

/****************************************************************************
 *  CVideoRenderer::CSetSizeCB::Release
 */
STDMETHODIMP_(ULONG32) CVideoRenderer::CSetSizeCB::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }

    delete this;
    return 0;
}

/****************************************************************************
 *  CVideoRenderer::CSetSizeCB::Func
 */
STDMETHODIMP CVideoRenderer::CSetSizeCB::Func()
{
    return m_pSite->SetSize(m_szViewFrame);
}


/****************************************************************************
 *  CVideoRenderer::Pacemaker Responses
 */
STDMETHODIMP CVideoRenderer::OnPaceStart(ULONG32 ulId)
{
    if (ulId == m_ulDecoderPacemakerId)
    {
	if (m_pVideoFormat)
	{
	    m_pDecoderVideoFormat = m_pVideoFormat;
	    m_pDecoderVideoFormat->AddRef();
	}
	m_bDecoderRunning = TRUE;
    }
    else if (ulId == m_ulBltrPacemakerId)
    {
	if (m_pVideoFormat)
	{
	    m_pBltrVideoFormat = m_pVideoFormat;
	    m_pBltrVideoFormat->AddRef();
	}
    }

    return HXR_OK;
}

STDMETHODIMP CVideoRenderer::OnPaceEnd(ULONG32 ulId)
{
    if (ulId == m_ulDecoderPacemakerId)
    {
	m_bDecoderRunning = FALSE;
        if (m_pDecoderVideoFormat)
        {
            m_pDecoderVideoFormat->Release();
            m_pDecoderVideoFormat = NULL;
        }
    }
    else if (ulId == m_ulBltrPacemakerId)
    {
        if (m_pBltrVideoFormat)
        {
            m_pBltrVideoFormat->Release();
            m_pBltrVideoFormat = NULL;
        }
    }

    return HXR_OK;
}

STDMETHODIMP CVideoRenderer::OnPace(ULONG32 ulId)
{
    if (ulId == m_ulBltrPacemakerId)
    {
	PresentFrame();
    }
    else if (ulId == m_ulDecoderPacemakerId)
    {
	if (m_bSchedulerStartRequested)
	{
	    StartSchedulers();
	}

	while ((m_pDecoderVideoFormat->DecodeFrame()) != 0) ;

#ifdef HELIX_FEATURE_VIDREND_BOOSTDECODE_ON_STARTUP
	// on ce we need to get this started quickly, otherwise the initial packets expire
	if (THREAD_PRIORITY_BELOW_NORMAL != GetDecodePriority())
	{
	    SetDecodePriority(THREAD_PRIORITY_BELOW_NORMAL);
	    return HXR_OK;
	}
#endif
    }

    return HXR_OK;
}


/****************************************************************************
 *  Video Surface 2 Support
 */
HX_RESULT CVideoRenderer::InitVideoSurface2(ULONG32 ulWidth, ULONG32 ulHeight)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    HX_RESULT retVal = HXR_FAIL;

    BOOL bTryVideoSurface2 = m_bTryVideoSurface2;

    if (bTryVideoSurface2)
    {
        HX_ASSERT(m_pMISUSSite);
        IHXVideoSurface* pVideoSurface = NULL;
	IHXVideoSurface2* pVideoSurface2 = NULL;
        IHXSite2* pSite2 = NULL;

	if (m_bUseVideoSurface2 &&
	    m_pVSurf2InputBIH &&
	    (m_pVSurf2InputBIH->biWidth == ((LONG32) ulWidth)) &&
	    (m_pVSurf2InputBIH->biHeight == ((LONG32) ulHeight)))
	{
	    return HXR_OK;
	}

	// EndOptimizedBlt();

	bTryVideoSurface2 = FALSE;

        m_pMISUSSite->QueryInterface(IID_IHXSite2, (void**) &pSite2);
        if (pSite2)
        {
            pSite2->GetVideoSurface(pVideoSurface);
        }
	HX_RELEASE(pSite2);

        HX_ASSERT(pVideoSurface);
        if (pVideoSurface)
        {
            pVideoSurface->QueryInterface(IID_IHXVideoSurface2,
                                          (void**) &pVideoSurface2);
        }
        HX_RELEASE(pVideoSurface);

        if (pVideoSurface2)
        {
	    HXBitmapInfoHeader userBIH;
	    ULONG32 ulUserBufCount;
            IHXBuffer* pInitialCountBuffer = NULL;
	    HX_RESULT status;

	    m_ulHWBufCount = m_ulConfigHWBufCount;

	    HX_RELEASE(pInitialCountBuffer);

	    if (m_pVSurf2InputBIH == NULL)
	    {
		m_pVSurf2InputBIH = new HXBitmapInfoHeader;
	    }

	    retVal = HXR_OUTOFMEMORY;
	    if (m_pVSurf2InputBIH)
	    {
		*m_pVSurf2InputBIH = m_BitmapInfoHeader;
		m_pVSurf2InputBIH->biWidth = ulWidth;
		m_pVSurf2InputBIH->biHeight = ulHeight;
		retVal = HXR_OK;
	    }

            if (SUCCEEDED(retVal))
	    {
		do
		{
		    // SetProperties should modify m_pVSurf2InputBIH
		    status = pVideoSurface2->SetProperties(m_pVSurf2InputBIH,
							   m_ulHWBufCount,
							   (IHXRenderTimeLine*) this);

		    /*** This code creates a leak - the renderer is not released
		     *** Needs investigation.
		    if (SUCCEEDED(status) && (m_ulHWBufCount > 0))
		    {
			UINT8* pVideoMem;
			INT32 iPitch;

			status = pVideoSurface2->GetVideoMem(pVideoMem,
							     iPitch,
							     HX_WAIT_FOREVER,
							     m_pVSurf2InputBIH);

			if (SUCCEEDED(status))
			{
			    pVideoSurface2->ReleaseVideoMem(pVideoMem);
			}
		    }
		     ***/

		    userBIH = *m_pVSurf2InputBIH;
		    ulUserBufCount = m_ulHWBufCount;

		    retVal = OnOptimizedVideo(status,
					      m_BitmapInfoHeader,
					      userBIH,
					      ulUserBufCount);

		    if (retVal == HXR_RETRY)
		    {
			*m_pVSurf2InputBIH = userBIH;
			m_ulHWBufCount = ulUserBufCount;
		    }
		} while (retVal == HXR_RETRY);
	    }

	    if (SUCCEEDED(retVal) &&
		SUCCEEDED(status) &&
		(m_ulHWBufCount != 0))
	    {
		bTryVideoSurface2 = TRUE;
	    }
	}

	if (bTryVideoSurface2)
	{
	    // Turning on video surface 2
	    m_ulEarlyFrameTol = m_ulMaxOptimizedVideoLead;
	    if (!m_bUseVideoSurface2)
	    {
		m_bVideoSurface2Transition = TRUE;
		m_bUseVideoSurface2 = TRUE;
		if (m_pBltrPump)
		{
		    // Wake up Bltr thread
		    m_pBltrPump->Suspend(FALSE);
		    m_pBltrPump->Signal();
		}
	    }
	}

	HX_RELEASE(pVideoSurface2);
    }

    if (FAILED(retVal))
    {
	HX_DELETE(m_pVSurf2InputBIH);
    }

    m_bUseVideoSurface2 = bTryVideoSurface2;

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


HX_RESULT CVideoRenderer::UpdateVideoSurface2(IHXVideoSurface* pVideoSurface,
					      HXxRect &destRect,
					      HXxRect &sorcRect,
					      BOOL bRefresh,
					      BOOL bSystemEvent)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    VideoMemStruct videoMemoryInfo;
    IHXVideoSurface2* pVideoSurface2 = NULL;
    HX_RESULT retVal;

    HX_ASSERT(pVideoSurface);

    videoMemoryInfo.pVidMem = NULL;

    retVal = pVideoSurface->QueryInterface(IID_IHXVideoSurface2,
					   (void**) &pVideoSurface2);

    if (!bRefresh)
    {
	m_bPresentInProgress = TRUE;

	// If we are in transition to video surface 2, we are not in
	// natural Bltr thread, thus do not unlock the mutex to prevent
	// other Blt entry points for interfering until the transition
	// to natural queue thread is complete.
	if (!m_bVideoSurface2Transition)
	{
	    DisplayMutex_Unlock();
	}

	if (SUCCEEDED(retVal))
	{
	    HX_ASSERT(m_bVideoSurface2Transition ? bSystemEvent : TRUE);

	    retVal = pVideoSurface2->GetVideoMem(&videoMemoryInfo,
                                                 m_bVideoSurface2Transition ?
						    HX_WAIT_NEVER : HX_WAIT_FOREVER);

	    m_bVS2BufferUnavailableOnLastBlt = FAILED(retVal);
	}

	if (!m_bVideoSurface2Transition)
	{
	    DisplayMutex_Lock();

	    // If the switch to VS1 occured after we successfully obtained video
	    // memory, fail this VS2 Blt
	    if ((!m_bUseVideoSurface2) && SUCCEEDED(retVal))
	    {
		retVal = HXR_FAIL;
	    }
	}

	if (SUCCEEDED(retVal))
	{
	    retVal = TransferOptimizedVideo(pVideoSurface2,
                                            &videoMemoryInfo,
					    m_pActiveVideoPacket,
					    m_BitmapInfoHeader,
					    sorcRect,
					    sorcRect);
	}

	if (SUCCEEDED(retVal))
	{
	    if (m_bFirstSurfaceUpdate)
	    {
		m_bFirstSurfaceUpdate = FALSE;

		if (bSystemEvent)
		{
		    retVal = pVideoSurface2->Present(&videoMemoryInfo,
						     m_pActiveVideoPacket->m_ulTime,
						     HX_MODE_IMMEDIATE,
						     &destRect,
						     &sorcRect);

		    // We'll fail if the site is hidden by SMIL:
		    if (FAILED(retVal))
		    {
			retVal = pVideoSurface2->Present(&videoMemoryInfo,
							 m_pActiveVideoPacket->m_ulTime,
							 HX_MODE_TIMED,
							 &destRect,
							 &sorcRect);
		    }
		}
		else
		{
		    // Unless this is a system event we are not under the protection of the
		    // top level site mutex and present immediate will acquire TLS mutex.
		    // We must avoid deadlock with HandleEvent by relinquishing
		    // VS mutex and forcing refresh.
		    m_bPresentInProgress = FALSE;
		    BOOL bIsVisible = ForceRefresh();

		    // We'll fail if the site is not visible
		    if (bIsVisible)
		    {
			// Abort any further updates for multiple site users since
			// ForceRefresh will take care of all of them
			retVal = HXR_ABORT;
		    }
		    else
		    {
			retVal = pVideoSurface2->Present(&videoMemoryInfo,
							 m_pActiveVideoPacket->m_ulTime,
							 HX_MODE_TIMED,
							 &destRect,
							 &sorcRect);
		    }
		}
	    }
	    else
	    {
		retVal = pVideoSurface2->Present(&videoMemoryInfo,
						 m_pActiveVideoPacket->m_ulTime,
						 HX_MODE_TIMED,
						 &destRect,
						 &sorcRect);
	    }

	    if (SUCCEEDED(retVal))
	    {
		videoMemoryInfo.pVidMem = NULL;
	    }
	}

	m_bPresentInProgress = FALSE;
    }
    else if (!m_bPresentInProgress)
    {
	HX_ASSERT(bSystemEvent);

	retVal = pVideoSurface2->Present(NULL,
					 0,
					 HX_MODE_REFRESH,
					 &destRect,
					 &sorcRect);

	// If no frames in video surface - make one and present it
	if (FAILED(retVal) && bSystemEvent)
	{
	    retVal = pVideoSurface2->GetVideoMem(&videoMemoryInfo,
                                                 HX_WAIT_NEVER);

	    if (SUCCEEDED(retVal))
	    {
		retVal = TransferOptimizedVideo(pVideoSurface2,
					        &videoMemoryInfo,
						m_pActiveVideoPacket,
						m_BitmapInfoHeader,
						sorcRect,
						sorcRect);

		if (SUCCEEDED(retVal))
		{
		    retVal = pVideoSurface2->Present(&videoMemoryInfo,
						     m_pActiveVideoPacket->m_ulTime,
						     HX_MODE_IMMEDIATE,
						     &destRect,
						     &sorcRect);

		    if (FAILED(retVal))
		    {
			pVideoSurface2->ReleaseVideoMem(&videoMemoryInfo);
			retVal = pVideoSurface2->Present(NULL,
							 0,
							 HX_MODE_REFRESH,
							 &destRect,
							 &sorcRect);
		    }

		    videoMemoryInfo.pVidMem = NULL;
		}
	    }
	    else
	    {
		retVal = pVideoSurface2->Present(NULL,
						 0,
						 HX_MODE_REFRESH,
						 &destRect,
						 &sorcRect);
	    }
	}
    }

    if (videoMemoryInfo.pVidMem != NULL)
    {
	pVideoSurface2->ReleaseVideoMem(&videoMemoryInfo);
    }

    HX_RELEASE(pVideoSurface2);

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


HX_RESULT CVideoRenderer::FlushVideoSurface2(IHXSite* pSite)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    HX_RESULT retVal = HXR_FAIL;

    IHXVideoSurface* pVideoSurface;
    IHXVideoSurface2* pVideoSurface2;
    IHXSite2* pSite2;

    HX_ASSERT(pSite);

    if (SUCCEEDED(pSite->QueryInterface(IID_IHXSite2, (void**) &pSite2)))
    {
	if (SUCCEEDED(pSite2->GetVideoSurface(pVideoSurface)))
	{
	    if (SUCCEEDED(pVideoSurface->QueryInterface(IID_IHXVideoSurface2,
							(void**) &pVideoSurface2)))
	    {
		pVideoSurface2->Flush();
		pVideoSurface2->Release();
		retVal = HXR_OK;
	    }

	    pVideoSurface->Release();
	}

	pSite2->Release();
    }

    HX_ASSERT(SUCCEEDED(retVal));

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


inline HX_RESULT CVideoRenderer::SwitchToVideoSurface1(void)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    HX_RESULT retVal = HXR_OK;

    m_pVSMutex->Lock();

    m_bVideoSurface1Requested = TRUE;
    if (m_bUseVideoSurface2)
    {
	HX_TRACE_THINGY("C:/trace.txt", "-->Switch to VS1", 0);

#ifdef SYNC_VS_SWITCHING
	InitVideoSurface1(TRUE);
	StartSchedulers();
#else	// SYNC_VS_SWITCHING
	// Force Video Surface reinitialization
	m_bVideoSurfaceReinitRequested = TRUE;
#endif	// SYNC_VS_SWITCHING
    }

    m_pVSMutex->Unlock();

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


inline HX_RESULT CVideoRenderer::SwitchToVideoSurface2(void)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    HX_RESULT retVal = HXR_OK;

    m_pVSMutex->Lock();

    m_bVideoSurface1Requested = FALSE;
    if (!m_bUseVideoSurface2)
    {
	if (m_bTryVideoSurface2)
	{
	    HX_TRACE_THINGY("C:/trace.txt", "-->Switch to VS2", 0);

#ifdef SYNC_VS_SWITCHING
	    InitVideoSurface2(m_SetWinSize.cx, m_SetWinSize.cy);
	    StartSchedulers();
#else	// SYNC_VS_SWITCHING
	    // Force Video Surface reinitialization
	    m_bVideoSurfaceReinitRequested = TRUE;
#endif	// SYNC_VS_SWITCHING
	}
	else
	{
	    // VideoSurface2 is not allowed
	    HX_TRACE_THINGY("C:/trace.txt", "-->Switch to VS2 Denied!", 0);

	    retVal = HXR_FAIL;
	}
    }

    m_pVSMutex->Unlock();

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::OnOptimizedVideo
 *
 */
HX_RESULT CVideoRenderer::OnOptimizedVideo(HX_RESULT status,
					   const HXBitmapInfoHeader& sourceBIH,
					   HXBitmapInfoHeader &targetBIH,
					   ULONG32 &ulTargetBufCount)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    HX_RESULT retVal = HXR_FAIL;

    if (SUCCEEDED(status) &&
	(ulTargetBufCount > 0) &&
	(GETBITMAPCOLOR(&sourceBIH) == CID_I420))
    {
        retVal = HXR_OK;
    }

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::OffOptimizedVideo
 *
 */
void CVideoRenderer::OffOptimizedVideo(void)
{
    return;
}


/****************************************************************************
 *  Method:
 *    CVideoFormat::TransferOptimizedVideo
 *
 */
HX_RESULT CVideoRenderer::TransferOptimizedVideo(IHXVideoSurface2* pVideoSurface2,
                                                 VideoMemStruct* pVideoMemoryInfo,
                                                 CMediaPacket* pVideoPacket,
                                                 const HXBitmapInfoHeader& sorcBIH,
                                                 HXxRect &destRect,
                                                 HXxRect &sorcRect)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    SourceInputStruct input;
    UINT8* aSrcInput[3];
    INT32 aSrcPitch[3];
    HX_RESULT retVal = HXR_FAIL;

    input.aSrcInput = aSrcInput;
    input.aSrcPitch = aSrcPitch;
    input.nNumInputs = 3;

    if (GETBITMAPCOLOR(&sorcBIH) == CID_I420)
    {
	aSrcInput[0] = pVideoPacket->m_pData;
	aSrcInput[1] = aSrcInput[0] + (sorcBIH.biWidth * sorcBIH.biHeight);
	aSrcInput[2] = aSrcInput[1] +
		       ((sorcBIH.biWidth / 2) * (sorcBIH.biHeight / 2));

	aSrcPitch[0] = sorcBIH.biWidth;
	aSrcPitch[1] = sorcBIH.biWidth / 2;
	aSrcPitch[2] = sorcBIH.biWidth / 2;

        CHXxSize sorcSize(sorcBIH.biWidth, sorcBIH.biHeight);
        CHXxSize destSize(pVideoMemoryInfo->bmi.biWidth, pVideoMemoryInfo->bmi.biHeight);

        retVal = pVideoSurface2->ColorConvert(HX_I420,
                                              &sorcSize,
                                              &sorcRect,
                                              &input,
                                              pVideoMemoryInfo->bmi.biCompression,
                                              pVideoMemoryInfo->pVidMem,
                                              &destSize,
                                              &destRect,
                                              pVideoMemoryInfo->lPitch);

        // Alpha blend subrects:
        for (UINT32 i = 0; i < pVideoMemoryInfo->ulCount; ++i)
        {
                       destSize.SetSize(pVideoMemoryInfo->pAlphaList[i].ulImageWidth, pVideoMemoryInfo->pAlphaList[i].ulImageHeight);
            HX_RESULT localResult = pVideoSurface2->ColorConvert(
                             HX_I420,
                             &sorcSize,
                             &pVideoMemoryInfo->pAlphaList[i].rcImageRect,
                             &input,
                             pVideoMemoryInfo->pAlphaList[i].ulFourCC,
                             pVideoMemoryInfo->pAlphaList[i].pBuffer,
                                                        &destSize,
                             &pVideoMemoryInfo->pAlphaList[i].rcImageRect,
                             pVideoMemoryInfo->pAlphaList[i].lPitch);
            if (SUCCEEDED(retVal))
            {
                retVal = localResult;
            }
        }
    }

    return retVal;
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    return HXR_NOTIMPL;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


void CVideoRenderer::PresentFrame(void)
{
#ifdef HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
    if (m_bUseVideoSurface2)
    {
	SchedulerCallback(FALSE,    // not scheduled
			  FALSE,    // do not resched
			  TRUE);    // is VS2 call
    }
    else
    {
	// Video Surface2 has been turned off
	IHXPaceMaker* pBltr = m_pBltrPump;

	if (pBltr)
	{
	    if (m_bTryVideoSurface2)
	    {
		// We might try VideoSurface2 again later - suspend the
		// Bltr thread for now
		pBltr->Suspend(TRUE);
	    }
	    else
	    {
		// We are not to try VideoSurface2 any more - stop the
		// Bltr thread
		pBltr->Stop();
		pBltr->Signal();
	    }
	}
    }
#else	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO

    ;
#endif	// HELIX_FEATURE_VIDREND_OPTIMIZEDVIDEO
}


