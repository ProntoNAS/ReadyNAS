/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: baserend.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

// include
#include "hxtypes.h"
#include "hxwintyp.h"
#include "hxcom.h"
#include "hxcomm.h"
#include "ihxpckts.h"
#include "hxhyper.h"
#include "hxupgrd.h"
#include "hxprefs.h"
#include "hxplugn.h"
#include "hxengin.h"
#include "hxrendr.h"
#include "hxwin.h"
#include "hxmon.h"
#include "hxcore.h"
#include "hxasm.h"
#include "hxerror.h"
#include "hxstrutl.h"
#include "addupcol.h"
#include "hxver.h"
#include "hxbuffer.h"
#include "pckunpck.h"
#include "baseobj.h"
#include "baserend.h"
#include "debugout.h"
#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif

const char* const CRNBaseRenderer::m_pszBaseCopyright   = HXVER_COPYRIGHT;
const char* const CRNBaseRenderer::m_pszBaseMoreInfoURL = HXVER_MOREINFO;

CRNBaseRenderer::CRNBaseRenderer()
{
    m_lRefCount            = 0;
    m_pContext             = NULL;
    m_pCommonClassFactory  = NULL;
    m_pHyperNavigate       = NULL;
    m_pPreferences         = NULL;
    m_pScheduler           = NULL;
    m_pStream              = NULL;
    m_pPlayer              = NULL;
    m_pBackChannel         = NULL;
    m_pASMStream           = NULL;
    m_pErrorMessages       = NULL;
    m_pValues              = NULL;
    m_ulRegistryID         = 0;
    m_lTimeOffset          = 0;
    m_ulContentVersion     = HX_ENCODE_PROD_VERSION(0, 0, 0, 0);
    m_ulStreamVersion      = HX_ENCODE_PROD_VERSION(0, 0, 0, 0);
    m_ulLastTimeSync       = 0;
    m_pStreamSourceURL     = NULL;
}

CRNBaseRenderer::~CRNBaseRenderer()
{
    HX_RELEASE(m_pContext);
    HX_RELEASE(m_pCommonClassFactory);
    HX_RELEASE(m_pHyperNavigate);
    HX_RELEASE(m_pPreferences);
    HX_RELEASE(m_pScheduler);
    HX_RELEASE(m_pStream);
    HX_RELEASE(m_pPlayer);
    HX_RELEASE(m_pBackChannel);
    HX_RELEASE(m_pASMStream);
    HX_RELEASE(m_pErrorMessages);
    HX_RELEASE(m_pValues);
    HX_RELEASE(m_pStreamSourceURL);
}

STDMETHODIMP CRNBaseRenderer::QueryInterface(REFIID riid, void** ppvObj)
{
    HX_RESULT retVal = HXR_OK;

    if (ppvObj)
    {
        // Set default
        *ppvObj = NULL;
        // Check for IID type
        if (IsEqualIID(riid, IID_IUnknown))
        {
            AddRef();
            *ppvObj = (IUnknown*) (IHXPlugin*) this;
        }
        else if (IsEqualIID(riid, IID_IHXPlugin))
        {
            AddRef();
            *ppvObj = (IHXPlugin*) this;
        }
        else if (IsEqualIID(riid, IID_IHXStatistics))
        {
            AddRef();
            *ppvObj = (IHXStatistics*) this;
        }
        else if (IsEqualIID(riid, IID_IHXRenderer))
        {
            AddRef();
            *ppvObj = (IHXRenderer*) this;
        }
        else if (IsEqualIID(riid, IID_IHXValues))
        {
            AddRef();
            *ppvObj = (IHXValues*) this;
        }
        else if (IsEqualIID(riid, IID_IHXUpdateProperties))
        {
            AddRef();
            *ppvObj = (IHXUpdateProperties*) this;
        }
        else
        {
            retVal = HXR_NOINTERFACE;
        }
    }
    else
    {
        retVal = HXR_FAIL;
    }

    return retVal;
}

STDMETHODIMP_(UINT32) CRNBaseRenderer::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}


STDMETHODIMP_(UINT32) CRNBaseRenderer::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }

    delete this;

    return 0;
}

STDMETHODIMP CRNBaseRenderer::GetPluginInfo(REF(BOOL)         rbLoadMultiple,
                                            REF(const char *) rpszDescription,
                                            REF(const char *) rpszCopyright,
                                            REF(const char *) rpszMoreInfoURL,
                                            REF(UINT32)       rulVersionNumber)
{
    rbLoadMultiple   = GetLoadMultiple();
    GetDescription(rpszDescription);
    GetCopyright(rpszCopyright);
    GetMoreInfoURL(rpszMoreInfoURL);
    rulVersionNumber = GetPluginVersion();

    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::InitPlugin(IUnknown *pContext)
{
    HX_RESULT retVal = HXR_OK;

    if (pContext)
    {
        // Save a copy of the calling context
        m_pContext = pContext;
        m_pContext->AddRef();

        // Get a IHXCommonClassFactory interface
        HX_RELEASE(m_pCommonClassFactory);
        retVal = m_pContext->QueryInterface(IID_IHXCommonClassFactory, (void **) &m_pCommonClassFactory);
        if (SUCCEEDED(retVal))
        {
            // Get an IHXScheduler interface
            HX_RELEASE(m_pScheduler);
            retVal = m_pContext->QueryInterface(IID_IHXScheduler, (void **) &m_pScheduler);
            if (SUCCEEDED(retVal))
            {
                // Get an IHXPreferences interface
                HX_RELEASE(m_pPreferences);
                retVal = m_pContext->QueryInterface(IID_IHXPreferences, (void **) &m_pPreferences);
                if (SUCCEEDED(retVal))
                {
                    // Get an IHXHyperNavigate interface - OK if TLC doesn't support it,
                    // so we don't check the return value
                    HX_RELEASE(m_pHyperNavigate);
                    m_pContext->QueryInterface(IID_IHXHyperNavigate, (void **) &m_pHyperNavigate);

                    // Get the IHXErrorMessages interface - OK if TLC doesn't support it,
                    // so we don't check the return value
                    HX_RELEASE(m_pErrorMessages);
                    m_pContext->QueryInterface(IID_IHXErrorMessages, (void**) &m_pErrorMessages);
                    // Create an IHXValues
                    HX_RELEASE(m_pValues);
                    retVal = m_pCommonClassFactory->CreateInstance(CLSID_IHXValues,
                                                                   (void**) &m_pValues);
                }
            }
        }
    }
    else
    {
        retVal = HXR_FAIL;
    }

    if (FAILED(retVal))
    {
        HX_RELEASE(m_pHyperNavigate);
        HX_RELEASE(m_pPreferences);
        HX_RELEASE(m_pScheduler);
        HX_RELEASE(m_pCommonClassFactory);
        HX_RELEASE(m_pContext);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::InitializeStatistics(UINT32 ulRegistryID)
{
    // Save a copy of the registry ID
    m_ulRegistryID = ulRegistryID;

    // Initialize the local variables
    IHXRegistry* pRegistry       = NULL;
    IHXBuffer*     pszRegistryName = NULL;
    IHXBuffer*     pValue          = NULL;
    HX_RESULT       retVal          = HXR_OK;

    // Add our renderer name to the HXRegistry
    retVal = m_pContext->QueryInterface(IID_IHXRegistry, (void **) &pRegistry);
    if (SUCCEEDED(retVal))
    {
        // Get the current registry key name
        retVal = pRegistry->GetPropName(m_ulRegistryID, pszRegistryName);
        if (SUCCEEDED(retVal))
        {
            // Create an IHXBuffer to hold the name
            pValue = new CHXBuffer();
            if (pValue)
            {
                // Addref the object
                pValue->AddRef();

                // Create the key name
                char szRegistryEntry[MAX_DISPLAY_NAME] = {0}; /* Flawfinder: ignore */
                SafeSprintf(szRegistryEntry, MAX_DISPLAY_NAME, "%s.name", pszRegistryName->GetBuffer());

                // Set the key value
                const char* pszBaseName = NULL;
                retVal = GetName(pszBaseName);
                if (SUCCEEDED(retVal))
                {
                    retVal = pValue->Set((const UCHAR *) pszBaseName, strlen(pszBaseName) + 1);
                    if (SUCCEEDED(retVal))
                    {
                        // Add the key/value pair to the registry
                        pRegistry->AddStr(szRegistryEntry, pValue);

                        HX_RELEASE(pValue);
                        HX_RELEASE(pszRegistryName);
                        HX_RELEASE(pRegistry);
                    }
                }
            }
            else
            {
                retVal = HXR_OUTOFMEMORY;
            }
        }
    }

    if (FAILED(retVal))
    {
        HX_RELEASE(pValue);
        HX_RELEASE(pszRegistryName);
        HX_RELEASE(pRegistry);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::UpdateStatistics()
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::GetRendererInfo(REF(const char**) pStreamMimeTypes,
                                              REF(UINT32)       rulInitialGranularity)
{
    GetMimeTypes(pStreamMimeTypes);
    rulInitialGranularity = GetInitialGranularity();

    return HXR_OK;
}


STDMETHODIMP CRNBaseRenderer::GetDisplayType(REF(HX_DISPLAY_TYPE) rulFlags,
                                             REF(IHXBuffer*)      pBuffer)
{
    rulFlags = GetDisplayFlags();

    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::StartStream(IHXStream *pStream, IHXPlayer *pPlayer)
{
    HX_RESULT retVal = HXR_OK;
 
    if (pStream && pPlayer)
    {
        // Save a copy of the IHXStream interface
        HX_RELEASE(m_pStream);
        m_pStream = pStream;
        m_pStream->AddRef();

        // Save a copy of the IHXPlayer interface
        HX_RELEASE(m_pPlayer);
        m_pPlayer = pPlayer;
        m_pPlayer->AddRef();

        // Get an IHXBackChannel interface if the source
        // supports it - it's OK if it doesn't
        IHXStreamSource* pSource = NULL;
        HX_RESULT         srcRet  = m_pStream->GetSource(pSource);
        if (SUCCEEDED(srcRet))
        {
            // Get the URL
            const char* pszURL = pSource->GetURL();
            if (pszURL)
            {
                HX_RELEASE(m_pStreamSourceURL);
                CreateStringBuffer(m_pStreamSourceURL, pszURL, m_pContext);
            }
            HX_RELEASE(m_pBackChannel);
            pSource->QueryInterface(IID_IHXBackChannel, (void**) &m_pBackChannel);
            HX_RELEASE(pSource);
        }

        // Get an IHXASMStream interface if it's support - OK if it doesn't
        HX_RELEASE(m_pASMStream);
        m_pStream->QueryInterface(IID_IHXASMStream, (void**) &m_pASMStream);
    }
    else
    {
        retVal = HXR_FAIL;
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::OnPreSeek(UINT32 ulOldTime, UINT32 ulNewTime)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::OnPostSeek(UINT32 ulOldTime, UINT32 ulNewTime)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::OnBegin(UINT32 ulTime)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::OnPause(UINT32 ulTime)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::OnBuffering(UINT32 ulFlags, UINT16 usPercentComplete)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::OnEndofPackets(void)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::SetPropertyULONG32(const char* pName, ULONG32 ulVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->SetPropertyULONG32(pName, ulVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetPropertyULONG32(const char* pName, REF(ULONG32) rulVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetPropertyULONG32(pName, rulVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetFirstPropertyULONG32(REF(const char*) rpName, REF(ULONG32) rulVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetFirstPropertyULONG32(rpName, rulVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetNextPropertyULONG32(REF(const char*) rpName, REF(ULONG32) rulVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetNextPropertyULONG32(rpName, rulVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::SetPropertyBuffer(const char* pName, IHXBuffer* pVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->SetPropertyBuffer(pName, pVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetPropertyBuffer(const char* pName, REF(IHXBuffer*) rpVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetPropertyBuffer(pName, rpVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetFirstPropertyBuffer(REF(const char*) rpName, REF(IHXBuffer*) rpVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetFirstPropertyBuffer(rpName, rpVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetNextPropertyBuffer(REF(const char*) rpName, REF(IHXBuffer*) rpVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetNextPropertyBuffer(rpName, rpVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::SetPropertyCString(const char* pName, IHXBuffer* pVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->SetPropertyCString(pName, pVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetPropertyCString(const char* pName, REF(IHXBuffer*) rpVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetPropertyCString(pName, rpVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetFirstPropertyCString(REF(const char*) rpName, REF(IHXBuffer*) rpVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetFirstPropertyCString(rpName, rpVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::GetNextPropertyCString(REF(const char*) rpName, REF(IHXBuffer*) rpVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (m_pValues)
    {
        retVal = m_pValues->GetNextPropertyCString(rpName, rpVal);
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::UpdatePacketTimeOffset(INT32 lTimeOffset)
{
    HX_RESULT retVal = HXR_OK;

    // Save the offset time
    m_lTimeOffset = -lTimeOffset;

    return retVal;
}

STDMETHODIMP
CRNBaseRenderer::UpdatePlayTimes(IHXValues* pProps)
{
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::EndStream()
{
    // Can't use IHXStream, IHXBackChannel, or IHXASMStream
    // after EndStream() is called.
    HX_RELEASE(m_pStream);
    HX_RELEASE(m_pBackChannel);
    HX_RELEASE(m_pASMStream);

    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::OnPacket(IHXPacket *pPacket, INT32 lTimeOffset)
{
    // Save the offset time
    m_lTimeOffset = lTimeOffset;

    return OnPacketNoOffset(pPacket);
}

STDMETHODIMP CRNBaseRenderer::OnTimeSync(UINT32 ulTime)
{
    HX_RESULT retVal = HXR_OK;
    
    // Adjust the time
    UINT32 ulAdjustedTime = 0;
    if (m_lTimeOffset < 0 && ulTime < (UINT32)(-m_lTimeOffset))
    {
        ulAdjustedTime = 0;
    }
    else
    {
        ulAdjustedTime = ulTime + m_lTimeOffset;
    }

    // Save the last time sync
    m_ulLastTimeSync = ulAdjustedTime;

    if(_IsValidRendererSurface())
    {
        retVal = OnTimeSyncOffset(ulAdjustedTime);
    }
    
    return retVal;
}

STDMETHODIMP CRNBaseRenderer::CheckStreamVersions(IHXValues* pHeader)
{
    BOOL bVersionOK = TRUE;

    HX_RESULT retVal = pHeader->GetPropertyULONG32("StreamVersion", m_ulStreamVersion);
    if (SUCCEEDED(retVal))
    {
        UINT32 ulDataMajor = HX_GET_MAJOR_VERSION(m_ulStreamVersion);
        UINT32 ulDataMinor = HX_GET_MINOR_VERSION(m_ulStreamVersion);
        UINT32 ulRendMajor = HX_GET_MAJOR_VERSION(GetHighestSupportedStreamVersion());
        UINT32 ulRendMinor = HX_GET_MINOR_VERSION(GetHighestSupportedStreamVersion());

        if((ulDataMajor >  ulRendMajor) ||
           (ulDataMajor == ulRendMajor && ulDataMinor > ulRendMinor))
        {
            bVersionOK = FALSE;
        }
    }

    retVal = pHeader->GetPropertyULONG32("ContentVersion", m_ulContentVersion);
    if(bVersionOK && SUCCEEDED(retVal))
    {
        UINT32 ulDataMajor = HX_GET_MAJOR_VERSION(m_ulContentVersion);
        UINT32 ulDataMinor = HX_GET_MINOR_VERSION(m_ulContentVersion);
        UINT32 ulRendMajor = HX_GET_MAJOR_VERSION(GetHighestSupportedContentVersion());
        UINT32 ulRendMinor = HX_GET_MINOR_VERSION(GetHighestSupportedContentVersion());

        if((ulDataMajor >  ulRendMajor) ||
           (ulDataMajor == ulRendMajor && ulDataMinor > ulRendMinor))
        {
            bVersionOK = FALSE;
        }
    }

    if(bVersionOK)
    {
        retVal = HXR_OK;
    }
    else
    {
        retVal = HXR_FAIL;
    }

    return retVal;
}

STDMETHODIMP CRNBaseRenderer::AddMimeToUpgradeCollection(const char* pszMimeType)
{
    return AddToAutoUpgradeCollection(pszMimeType, m_pContext);
}

STDMETHODIMP CRNBaseRenderer::GetPreference(const char*      pszPrefName,
                                            REF(BOOL)        rbPresent,
                                            REF(IHXBuffer*) rpBuffer)
{
    HX_RESULT retVal  = HXR_OK;

    if (pszPrefName)
    {
        // Set defaults
        rbPresent = FALSE;
        rpBuffer  = NULL;

        if (m_pPreferences)
        {
            HX_RESULT rv = m_pPreferences->ReadPref(pszPrefName, rpBuffer);
            if (SUCCEEDED(rv))
            {
                rbPresent = TRUE;
            }
        }
        else
        {
            retVal = HXR_NOT_INITIALIZED;
        }
    }
    else
    {
        retVal = HXR_INVALID_PARAMETER;
    }

    return retVal;
}

BOOL CRNBaseRenderer::_IsValidRendererSurface()
{
    return TRUE;
}

STDMETHODIMP_(BOOL) CRNBaseRenderer::GetLoadMultiple()
{
    return TRUE;
}

STDMETHODIMP CRNBaseRenderer::GetCopyright(REF(const char*) rpszCopyright)
{
    rpszCopyright = (const char*) m_pszBaseCopyright;
    return HXR_OK;
}

STDMETHODIMP CRNBaseRenderer::GetMoreInfoURL(REF(const char*) rpszMoreInfoURL)
{
    rpszMoreInfoURL = (const char*) m_pszBaseMoreInfoURL;
    return HXR_OK;
}

STDMETHODIMP_(UINT32) CRNBaseRenderer::GetInitialGranularity()
{
    return 33;
}

STDMETHODIMP_(UINT32) CRNBaseRenderer::GetDisplayFlags()
{
    UINT32 ulRet = HX_DISPLAY_WINDOW          |
                   HX_DISPLAY_SUPPORTS_RESIZE |
                   HX_DISPLAY_SUPPORTS_FULLSCREEN;
    return ulRet;
}

STDMETHODIMP_(UINT32) CRNBaseRenderer::GetHighestSupportedContentVersion()
{
    return HX_ENCODE_PROD_VERSION(0, 0, 0, 0);
}

STDMETHODIMP_(UINT32) CRNBaseRenderer::GetHighestSupportedStreamVersion()
{
    return HX_ENCODE_PROD_VERSION(0, 0, 0, 0);
}
