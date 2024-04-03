/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: statinfo.cpp,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#include "safestring.h"
#include "hxtypes.h"
#include "hxresult.h"
#include "hxcom.h"
#include "hxcomm.h"
#include "hxbuffer.h"
#include "hxmon.h"
#include "hxstrutl.h"
#include "watchlst.h"
#include "statinfo.h"

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif

CStatisticEntry::CStatisticEntry(IHXRegistry*    pRegistry,
				 char*		    pszRegKey,
				 UINT32		    ulType)
    : m_pRegistry(NULL)
    , m_ulRegistryID(0)
    , m_ulType(REG_TYPE_UNKNOWN)
    , m_bAddKey(FALSE)
{
    if (pRegistry)
    {
	m_pRegistry = pRegistry;
	m_pRegistry->AddRef();

	m_ulType = ulType;

	/*
	 * Check to see if this Registry ID already exists
         */

	m_ulRegistryID = m_pRegistry->GetId(pszRegKey);

	if (!m_ulRegistryID)
	{
	    m_bAddKey = TRUE;

	    if (REG_TYPE_STRING == ulType)
	    {
		m_ulRegistryID = m_pRegistry->AddStr(pszRegKey, NULL);
	    }
	    else if (REG_TYPE_NUMBER == ulType)
	    {
		m_ulRegistryID = m_pRegistry->AddInt(pszRegKey, 0);
	    }
	    else if (REG_TYPE_COMPOSITE == ulType)
	    {
		m_ulRegistryID = m_pRegistry->AddComp(pszRegKey);
	    }	    
	    else
	    {
		m_ulType = REG_TYPE_UNKNOWN;
		m_ulRegistryID = 0;
	    }
	}
    }
    else
    {
	m_pRegistry = NULL;
	m_ulRegistryID = 0;
    }
}

CStatisticEntry::~CStatisticEntry(void)
{
    if (m_pRegistry)
    {
	if (m_ulRegistryID && m_bAddKey)
	{
	    m_pRegistry->DeleteById(m_ulRegistryID);
	    m_ulRegistryID = 0;
	}

	m_pRegistry->Release();
	m_pRegistry = NULL;
    }
}

HX_RESULT
CStatisticEntry::SetInt(INT32 lValue)
{    
    HX_RESULT	theErr = HXR_OK;

    if (!m_pRegistry || !m_ulRegistryID ||
	(REG_TYPE_NUMBER != m_ulType))
    {
	theErr = HXR_FAILED;
	goto cleanup;
    }

    theErr = m_pRegistry->SetIntById(m_ulRegistryID, lValue);

cleanup:
    
    return theErr;
}

HX_RESULT
CStatisticEntry::SetStr(char* pszValue)
{    
    IHXBuffer*	pValue = NULL;
    HX_RESULT	theErr = HXR_OK;

    if (!m_pRegistry || !m_ulRegistryID ||
	(REG_TYPE_STRING != m_ulType))
    {
	theErr = HXR_FAILED;
	goto cleanup;
    }

    if (NULL == pszValue)
    {
	theErr = m_pRegistry->SetStrById(m_ulRegistryID, NULL);
    }
    else
    {
	if (!(pValue = new CHXBuffer()))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
	pValue->AddRef();

	pValue->Set((const UCHAR*)pszValue, strlen(pszValue)+1);
	theErr = m_pRegistry->SetStrById(m_ulRegistryID, pValue);

	pValue->Release();
    }

cleanup:
    
    return theErr;
}

INT32
CStatisticEntry::GetInt(void)
{
    INT32	lValue = 0;

    if (!m_pRegistry || !m_ulRegistryID || (REG_TYPE_NUMBER != m_ulType))
    {
	goto cleanup;
    }

    m_pRegistry->GetIntById(m_ulRegistryID, lValue);
   
cleanup:

    return lValue;
}

char*
CStatisticEntry::GetStr(void)
{
    HX_RESULT	theErr = HXR_OK;
    IHXBuffer*	pValue = NULL;
    char*       pszValue = new char[MAX_DISPLAY_NAME];

    if (!pszValue || !m_pRegistry || !m_ulRegistryID || (REG_TYPE_STRING != m_ulType))
    {
	theErr = HXR_UNEXPECTED;
	goto cleanup;
    }

    if (HXR_OK != m_pRegistry->GetStrById(m_ulRegistryID, pValue) || !pValue)
    {
	theErr = HXR_UNEXPECTED;
	goto cleanup;
    }

    SafeStrCpy(pszValue, (const char*)pValue->GetBuffer(), MAX_DISPLAY_NAME);
    
cleanup:

    HX_RELEASE(pValue);

    if (HXR_OK != theErr)
    {
	if (pszValue) delete [] pszValue;
	pszValue = NULL;

	return NULL;
    }

    return pszValue;
}

STATS::STATS(IHXRegistry* /*IN*/  pRegistry,
	     UINT32	     /*IN*/  ulRegistryID)
    : m_lastError(HXR_OK)
    , m_pRegistry(NULL)
    , m_ulRegistryID(0)
    , m_bInitialized(FALSE)
    , m_pNormal(0)
    , m_pRecovered(0)
    , m_pReceived(0)
    , m_pOutOfOrder(0)
    , m_pLost(0)
    , m_pLate(0)
    , m_pDuplicate(0)
    , m_pTotal(0)
    , m_pLost30(0)
    , m_pTotal30(0)
    , m_pResendRequested(0)
    , m_pResendReceived(0)
    , m_pClipBandwidth(0)
    , m_pAvgBandwidth(0)
    , m_pCurBandwidth(0)
    , m_pHighLatency(0)
    , m_pLowLatency(0)
    , m_pAvgLatency(0)
{
    HX_RESULT	theErr = HXR_OK;
    IHXBuffer*	pszParentName = NULL;
    char	szRegKeyName[MAX_DISPLAY_NAME] = {0}; /* Flawfinder: ignore */

    if (!pRegistry)
    {
	goto cleanup;
    }
    
    m_pRegistry = pRegistry;
    m_pRegistry->AddRef();
 
    m_ulRegistryID = ulRegistryID;

    if (HXR_OK == m_pRegistry->GetPropName(m_ulRegistryID, pszParentName))
    {
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Normal", pszParentName->GetBuffer());
	if (!(m_pNormal = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Recovered", pszParentName->GetBuffer());
	if (!(m_pRecovered = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Received", pszParentName->GetBuffer());
	if (!(m_pReceived = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
	
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.OutOfOrder", pszParentName->GetBuffer());
	if (!(m_pOutOfOrder = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Lost", pszParentName->GetBuffer());
	if (!(m_pLost = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Late", pszParentName->GetBuffer());
	if (!(m_pLate = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Duplicate", pszParentName->GetBuffer());
	if (!(m_pDuplicate = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Total", pszParentName->GetBuffer());
	if (!(m_pTotal = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Lost30", pszParentName->GetBuffer());
	if (!(m_pLost30 = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Total30", pszParentName->GetBuffer());
	if (!(m_pTotal30 = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.ResendRequested", pszParentName->GetBuffer());
	if (!(m_pResendRequested = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.ResendReceived", pszParentName->GetBuffer());
	if (!(m_pResendReceived = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.ClipBandwidth", pszParentName->GetBuffer());
	if (!(m_pClipBandwidth = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.AverageBandwidth", pszParentName->GetBuffer());
	if (!(m_pAvgBandwidth = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.CurrentBandwidth", pszParentName->GetBuffer());
	if (!(m_pCurBandwidth = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.HighLatency", pszParentName->GetBuffer());
	if (!(m_pHighLatency = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.LowLatency", pszParentName->GetBuffer());
	if (!(m_pLowLatency = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.AverageLatency", pszParentName->GetBuffer());
	if (!(m_pAvgLatency = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
    }
  
cleanup:

    HX_RELEASE(pszParentName);

    if (HXR_OK == theErr)
    {
	m_lastError = HXR_OK;
	m_bInitialized = TRUE;
    }
    else
    {
	m_lastError = theErr;
	m_bInitialized = FALSE;
    }
}

STATS::~STATS()
{
    HX_RELEASE(m_pRegistry);

    HX_DELETE(m_pNormal);
    HX_DELETE(m_pRecovered);
    HX_DELETE(m_pReceived);
    HX_DELETE(m_pOutOfOrder);
    HX_DELETE(m_pLost);
    HX_DELETE(m_pLate);
    HX_DELETE(m_pDuplicate);
    HX_DELETE(m_pTotal);
    HX_DELETE(m_pLost30);
    HX_DELETE(m_pTotal30);
    HX_DELETE(m_pClipBandwidth);
    HX_DELETE(m_pResendRequested);
    HX_DELETE(m_pResendReceived);
    HX_DELETE(m_pAvgBandwidth);
    HX_DELETE(m_pCurBandwidth);
    HX_DELETE(m_pHighLatency);
    HX_DELETE(m_pLowLatency);
    HX_DELETE(m_pAvgLatency);
}

STATS& STATS::operator=(const STATS& rhs )
{
    if( this != &rhs )
    {
        m_pNormal->SetInt( rhs.m_pNormal->GetInt());
        m_pRecovered->SetInt( rhs.m_pRecovered->GetInt());
        m_pReceived->SetInt( rhs.m_pReceived->GetInt());
        m_pOutOfOrder->SetInt( rhs.m_pOutOfOrder->GetInt());
        m_pLost->SetInt( rhs.m_pLost->GetInt());
        m_pLate->SetInt( rhs.m_pLate->GetInt());
	m_pDuplicate->SetInt( rhs.m_pDuplicate->GetInt());
        m_pTotal->SetInt( rhs.m_pTotal->GetInt());
        m_pLost30->SetInt( rhs.m_pLost30->GetInt());
        m_pTotal30->SetInt( rhs.m_pTotal30->GetInt());
        m_pClipBandwidth->SetInt( rhs.m_pClipBandwidth->GetInt());
        m_pResendRequested->SetInt( rhs.m_pResendRequested->GetInt());
        m_pResendReceived->SetInt( rhs.m_pResendReceived->GetInt());
        m_pAvgBandwidth->SetInt( rhs.m_pAvgBandwidth->GetInt());
        m_pCurBandwidth->SetInt( rhs.m_pCurBandwidth->GetInt());
        m_pHighLatency->SetInt( rhs.m_pHighLatency->GetInt());
        m_pLowLatency->SetInt( rhs.m_pLowLatency->GetInt());
        m_pAvgLatency->SetInt( rhs.m_pAvgLatency->GetInt());
    }
    
    return *this;
}

void
STATS::Reset()
{
    if (m_bInitialized)
    {
	m_pNormal->SetInt(0);
	m_pRecovered->SetInt(0);
	m_pReceived->SetInt(0);
	m_pOutOfOrder->SetInt(0);
	m_pLost->SetInt(0);
	m_pLate->SetInt(0);
	m_pDuplicate->SetInt(0);
	m_pTotal->SetInt(0);
	m_pLost30->SetInt(0);
	m_pTotal30->SetInt(0);
	m_pClipBandwidth->SetInt(0);
	m_pResendRequested->SetInt(0);
	m_pResendReceived->SetInt(0);
	m_pAvgBandwidth->SetInt(0);
	m_pCurBandwidth->SetInt(0);
	m_pHighLatency->SetInt(0);
	m_pLowLatency->SetInt(0);
	m_pAvgLatency->SetInt(0);
    }
}

PLAYER_STATS::PLAYER_STATS(IHXRegistry*   /*IN*/  pRegistry, 
			   UINT32	     /*IN*/  ulRegistryID) 
	     :STATS(pRegistry, ulRegistryID)
	     ,m_pBufferingMode(NULL)
{
    HX_RESULT	theErr = HXR_OK;
    IHXBuffer*	pszParentName = NULL;
    char	szRegKeyName[MAX_DISPLAY_NAME] = {0}; /* Flawfinder: ignore */

    if (!pRegistry)
    {
	goto cleanup;
    }

    if (HXR_OK == m_pRegistry->GetPropName(m_ulRegistryID, pszParentName))
    {
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.BufferingMode", pszParentName->GetBuffer());
	if (!(m_pBufferingMode = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
    }

cleanup:

    HX_RELEASE(pszParentName);

    if (HXR_OK == theErr)
    {
	m_lastError = HXR_OK;
	m_bInitialized = TRUE;
    }
    else
    {
	m_lastError = theErr;
	m_bInitialized = FALSE;
    }
}

PLAYER_STATS::~PLAYER_STATS()
{
    HX_DELETE(m_pBufferingMode);
}

PLAYER_STATS& PLAYER_STATS::operator=(const PLAYER_STATS& rhs )
{
    if( this != &rhs )
    {
        STATS::operator=(rhs);
        m_pBufferingMode->SetInt( rhs.m_pBufferingMode->GetInt() );
    }
    
    return *this;
}

void
PLAYER_STATS::Reset()
{
    if (m_bInitialized)
    {
	m_pBufferingMode->SetInt(0);

	STATS::Reset();
    }
}

SOURCE_STATS::SOURCE_STATS(IHXRegistry*   /*IN*/  pRegistry,
			   UINT32	     /*IN*/  ulRegistryID)
	     :STATS(pRegistry, ulRegistryID)
{
    HX_RESULT	theErr = HXR_OK;
    IHXBuffer*	pszParentName = NULL;
    char	szRegKeyName[MAX_DISPLAY_NAME] = {0}; /* Flawfinder: ignore */

    m_pTransportMode = NULL;
    m_pBufferingMode = NULL;
    m_pTitle = NULL;
    m_pAuthor = NULL;
    m_pCopyright = NULL;
    m_pAbstract = NULL;
    m_pDescription = NULL;
    m_pKeywords = NULL;
    m_pSourceName = NULL;
    m_pServerInfo = NULL;
    m_pProtocolVersion = NULL;
    m_pProtocol = NULL;

    if (!pRegistry)
    {
	goto cleanup;
    }
   
    if (HXR_OK == m_pRegistry->GetPropName(m_ulRegistryID, pszParentName))
    {
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.TransportMode", pszParentName->GetBuffer());
	if (!(m_pTransportMode = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.BufferingMode", pszParentName->GetBuffer());
	if (!(m_pBufferingMode = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.SourceName", pszParentName->GetBuffer());
	if (!(m_pSourceName = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.ServerInfo", pszParentName->GetBuffer());
	if (!(m_pServerInfo = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.ProtocolVersion", pszParentName->GetBuffer());
	if (!(m_pProtocolVersion = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_NUMBER)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Protocol", pszParentName->GetBuffer());
	if (!(m_pProtocol = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
	
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Title", pszParentName->GetBuffer());
	if (!(m_pTitle = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Author", pszParentName->GetBuffer());
	if (!(m_pAuthor = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Copyright", pszParentName->GetBuffer());
	if (!(m_pCopyright = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Abstract", pszParentName->GetBuffer());
	if (!(m_pAbstract = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
    
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Description", pszParentName->GetBuffer());
	if (!(m_pDescription = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Keywords", pszParentName->GetBuffer());
	if (!(m_pKeywords = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
    }

cleanup:

    HX_RELEASE(pszParentName);

    if (HXR_OK == theErr)
    {
	m_lastError = HXR_OK;
	m_bInitialized = TRUE;
    }
    else
    {
	m_lastError = theErr;
	m_bInitialized = FALSE;
    }
}

SOURCE_STATS::~SOURCE_STATS()
{
    HX_DELETE(m_pTransportMode);
    HX_DELETE(m_pBufferingMode);
    HX_DELETE(m_pSourceName);
    HX_DELETE(m_pServerInfo);
    HX_DELETE(m_pProtocolVersion);
    HX_DELETE(m_pProtocol);
    HX_DELETE(m_pTitle);
    HX_DELETE(m_pAuthor);
    HX_DELETE(m_pCopyright);
    HX_DELETE(m_pAbstract);
    HX_DELETE(m_pDescription);
    HX_DELETE(m_pKeywords);
}

SOURCE_STATS& SOURCE_STATS::operator=(const SOURCE_STATS& rhs )
{
    if( this != &rhs )
    {
        STATS::operator=(rhs);

        HX_ASSERT( m_bInitialized );
        if (m_bInitialized)
        {
            m_pTransportMode->SetStr(rhs.m_pTransportMode->GetStr());
            m_pBufferingMode->SetInt(rhs.m_pBufferingMode->GetInt());
            m_pSourceName->SetStr(rhs.m_pSourceName->GetStr());
            m_pServerInfo->SetStr(rhs.m_pServerInfo->GetStr());
            m_pProtocolVersion->SetInt(rhs.m_pProtocolVersion->GetInt());
            m_pProtocol->SetStr(rhs.m_pProtocol->GetStr());
            m_pTitle->SetStr(rhs.m_pTitle->GetStr());
            m_pAuthor->SetStr(rhs.m_pAuthor->GetStr());
            m_pCopyright->SetStr(rhs.m_pCopyright->GetStr());
            m_pAbstract->SetStr(rhs.m_pAbstract->GetStr());
            m_pDescription->SetStr(rhs.m_pDescription->GetStr());
            m_pKeywords->SetStr(rhs.m_pKeywords->GetStr());
        }
    }
    
    return *this;
}

void
SOURCE_STATS::Reset()
{
    if (m_bInitialized)
    {
	m_pTransportMode->SetStr(NULL);
	m_pBufferingMode->SetInt(0);
	m_pSourceName->SetStr(NULL);
	m_pServerInfo->SetStr(NULL);
	m_pProtocolVersion->SetInt(0);
	m_pProtocol->SetStr(NULL);
	m_pTitle->SetStr(NULL);
	m_pAuthor->SetStr(NULL);
	m_pCopyright->SetStr(NULL);
	m_pAbstract->SetStr(NULL);
	m_pDescription->SetStr(NULL);
	m_pKeywords->SetStr(NULL);

	STATS::Reset();
    }
}

STREAM_STATS::STREAM_STATS(IHXRegistry*   /*IN*/  pRegistry,
			   UINT32	     /*IN*/  ulRegistryID)
	     :STATS(pRegistry, ulRegistryID)
{
    HX_RESULT	theErr = HXR_OK;
    IHXBuffer*	pszParentName = NULL;
    char	szRegKeyName[MAX_DISPLAY_NAME] = {0}; /* Flawfinder: ignore */
    
    m_pRenderer = NULL;
    m_pMimeType = NULL;

    if (!pRegistry)
    {
	goto cleanup;
    }
    
    if (HXR_OK == m_pRegistry->GetPropName(m_ulRegistryID, pszParentName))
    {
	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.Renderer", pszParentName->GetBuffer());
	if (!(m_pRenderer = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_COMPOSITE)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}

	SafeSprintf(szRegKeyName, MAX_DISPLAY_NAME, "%s.MimeType", pszParentName->GetBuffer());
	if (!(m_pMimeType = new CStatisticEntry(m_pRegistry, szRegKeyName, REG_TYPE_STRING)))
	{
	    theErr = HXR_OUTOFMEMORY;
	    goto cleanup;
	}
	
    }
        
cleanup:

    HX_RELEASE(pszParentName);

    if (HXR_OK == theErr)
    {
	m_lastError = HXR_OK;
	m_bInitialized = TRUE;
    }
    else
    {
	m_lastError = theErr;
	m_bInitialized = FALSE;
    }
}

STREAM_STATS::~STREAM_STATS()
{
    HX_DELETE(m_pRenderer);
    HX_DELETE(m_pMimeType);
}

STREAM_STATS& STREAM_STATS::operator=(const STREAM_STATS& rhs )
{
    if( this != &rhs )
    {
        HX_ASSERT( m_bInitialized );
        if (m_bInitialized)
        {
	    STATS::operator=(rhs);
            m_pMimeType->SetStr( rhs.m_pMimeType->GetStr() );
        }
    }
    
    return *this;
}

void
STREAM_STATS::Reset()
{
    if (m_bInitialized)
    {
	m_pMimeType->SetStr(NULL);
	STATS::Reset();
    }
}

BOOL SetIntIfNecessary(CStatisticEntry* pEntry, INT32 lValue)
{
    BOOL bResult = FALSE;

    if (!pEntry)
    {
	goto cleanup;
    }

    if (pEntry->GetInt() != lValue)
    {
	pEntry->SetInt(lValue);
	bResult = TRUE;
    }

cleanup:

    return bResult;
}
