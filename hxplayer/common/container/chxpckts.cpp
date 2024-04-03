/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: chxpckts.cpp,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#include "hxcppflags.h"

#ifndef HX_CPP_BASIC_TEMPLATES
#include "old_hxpckts.cpp"
#else

/************************************************************************
 *  Defines
 */
#define EXTREME_RTP_TIME    0xFFFFFFFF

#include "hxtypes.h"
#include "hxresult.h"
#include "hxcom.h"
#include "ihxpckts.h"
#include "chxpckts.h"
#include "hlxclib/string.h"

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE
static const char HX_THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////
//	CHXPacket
/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::QueryInterface
//	Purpose:
//		Implement this to export the interfaces supported by your 
//		object.
//
STDMETHODIMP
CHXPacket::QueryInterface(REFIID riid, void** ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return HXR_OK;
    }
    else if (IsEqualIID(riid, IID_IHXPacket))
    {
        AddRef();
        *ppvObj = (IHXPacket*)this;
        return HXR_OK;
    }

    *ppvObj = NULL;
    return HXR_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::AddRef
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32)
CHXPacket::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::Release
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32)
CHXPacket::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }

    delete this;
    return 0;
}

/************************************************************************
 *	Method:
 *		IHXPacket::Get
 *	Purpose:
 *		TBD
 */
STDMETHODIMP
CHXPacket::Get(REF(IHXBuffer*)  pBuffer, 
               REF(ULONG32)     ulTime,
               REF(UINT16)      uStreamNumber,
               REF(UINT8)       unASMFlags,
               REF(UINT16)      unASMRuleNumber)
{
    return _Get(pBuffer,
                ulTime,
                uStreamNumber,
                unASMFlags,
                unASMRuleNumber);
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetBuffer
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(IHXBuffer*)
CHXPacket::GetBuffer()
{
    // Whenever a COM object is returned out of an
    // interface, it should be AddRef()'d.
    if (m_pBuffer) m_pBuffer->AddRef();
    return m_pBuffer; 
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetTime
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(ULONG32)
CHXPacket::GetTime()
{
    return m_ulTime;
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetFlags
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(UINT8)
CHXPacket::GetASMFlags()
{
    return m_unASMFlags;
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetStreamNumber
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(UINT16)
CHXPacket::GetStreamNumber()
{
    return m_uStreamNumber;
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetASMRuleNumber
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(UINT16)
CHXPacket::GetASMRuleNumber()
{
    return m_unASMRuleNumber;
}

/************************************************************************
 *	Method:
 *		IHXPacket::IsLost
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(BOOL)
CHXPacket::IsLost()
{ 
    return m_bIsLost;
}

/************************************************************************
 *	Method:
 *		IHXPacket::SetAsLost
 *	Purpose:
 *		TBD
 */
STDMETHODIMP
CHXPacket::SetAsLost()
{
    HX_ASSERT(m_pBuffer == NULL);

    // Dropped packets are allowed to have a non-zero timestamp
    if (!(m_unASMFlags & HX_ASM_DROPPED_PKT))
    {
	HX_ASSERT(m_ulTime == 0);
    }

    if (m_pBuffer)
    {
        return HXR_UNEXPECTED;
    }

    m_bIsLost = TRUE;
    return HXR_OK; 
}

/************************************************************************
 *	Method:
 *		IHXPacket::Set
 *	Purpose:
 *		TBD
 */
STDMETHODIMP
CHXPacket::Set (IHXBuffer*  pBuffer, 
                ULONG32     ulTime,
                UINT16      uStreamNumber,
                UINT8       unASMFlags,
                UINT16      unASMRuleNumber
)
{
    return _Set(pBuffer, 
                ulTime,
                uStreamNumber,
                unASMFlags,
                unASMRuleNumber);
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::GetRTP
 */
STDMETHODIMP
CHXPacket::GetRTP(REF(IHXBuffer*) pBuffer, 
                  REF(ULONG32)    ulTime,
                  REF(ULONG32)    ulRTPTime,
                  REF(UINT16)     uStreamNumber,
                  REF(UINT8)      unASMFlags,
                  REF(UINT16)     unASMRuleNumber)
{
    HX_ASSERT(HXR_UNEXPECTED == HXR_OK);
    return HXR_UNEXPECTED;
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::GetRTPTime
 */
STDMETHODIMP_(ULONG32)
CHXPacket::GetRTPTime()
{
    HX_ASSERT(HXR_UNEXPECTED == HXR_OK);
    return EXTREME_RTP_TIME;
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::SetRTP
 */
STDMETHODIMP
CHXPacket::SetRTP (IHXBuffer* pBuffer, 
                   ULONG32    ulTime,
                   ULONG32    ulRTPTime,
                   UINT16     uStreamNumber,
                   UINT8      unASMFlags,
                   UINT16     unASMRuleNumber)
{
    HX_ASSERT(HXR_UNEXPECTED == HXR_OK);
    return HXR_UNEXPECTED;
}

inline HX_RESULT
CHXPacket::_Get(IHXBuffer*  &pBuffer,
                ULONG32     &ulTime,
                UINT16      &uStreamNumber,
                UINT8       &unASMFlags,
                UINT16      &unASMRuleNumber)
{
    pBuffer         = m_pBuffer;
    ulTime          = m_ulTime;
    uStreamNumber   = m_uStreamNumber;
    unASMFlags      = m_unASMFlags;
    unASMRuleNumber = m_unASMRuleNumber;

    // Whenever a COM object is returned out of an
    // interface, it should be AddRef()'d.
    if (m_pBuffer) m_pBuffer->AddRef();

    return HXR_OK;
}

inline HX_RESULT
CHXPacket::_Set(IHXBuffer*  pBuffer, 
                ULONG32     ulTime,
                UINT16      uStreamNumber,
                UINT8       unASMFlags,
                UINT16      unASMRuleNumber)
{
    /* We allow changing the packet info when it is owned
     * by atmost one user.
     */
    if (m_lRefCount > 1)
    {
        HX_ASSERT(FALSE);
        return HXR_UNEXPECTED;
    }

    if (m_pBuffer)
    {
        m_pBuffer->Release();
    }
    m_pBuffer = pBuffer;
    if (m_pBuffer)
    {
        m_pBuffer->AddRef();
    }

    m_ulTime            = ulTime;
    m_uStreamNumber     = uStreamNumber;
    m_unASMFlags        = unASMFlags;
    m_unASMRuleNumber   = unASMRuleNumber;

    return HXR_OK;
}

// serialization method
void
CHXPacket::Pack(IHXPacket* pPacket, char* pData, UINT32& ulSize)
{
    UINT16      uValue = 0;
    UINT32      ulValue = 0;
    UINT32      ulBufferSize = 0;
    IHXBuffer*  pBuffer = NULL;

    if (!pPacket)
    {
        goto cleanup;
    }

    pBuffer = pPacket->GetBuffer();

    if (pBuffer)
    {
        ulBufferSize = pBuffer->GetSize();
    }

    // figure out the size 
    if (!pData)
    {
        ulSize = sizeof(HXPacketInfo) + ulBufferSize;
    }
    // pack the data
    else
    {
        *pData++ = (BYTE)pPacket->IsLost();					ulSize++;
        *pData++ = (BYTE)pPacket->GetASMFlags();				ulSize++;

        uValue = pPacket->GetASMRuleNumber();
        *pData++ = (BYTE)uValue; *pData++ = (BYTE)(uValue >> 8); 		ulSize += 2;

        uValue = pPacket->GetStreamNumber();
        *pData++ = (BYTE)uValue; *pData++ = (BYTE)(uValue >> 8); 		ulSize += 2;

        ulValue = pPacket->GetTime();
        *pData++ = (BYTE)ulValue; *pData++ = (BYTE)(ulValue >> 8);
        *pData++ = (BYTE)(ulValue >> 16); *pData++ = (BYTE)(ulValue >> 24);	ulSize += 4;

        if (pBuffer)
        {
            memcpy(pData, (char*)pBuffer->GetBuffer(), ulBufferSize); /* Flawfinder: ignore */
            pData += ulBufferSize;						ulSize += ulBufferSize;
        }
    }

cleanup:

    HX_RELEASE(pBuffer);

    return;
}   

void
CHXPacket::UnPack(IHXPacket*& pPacket, char* pData, UINT32 ulSize)
{
    UINT16          uValue = 0;
    UINT32          ulValue = 0;
    IHXBuffer*      pBuffer = NULL;
    HXPacketInfo   packetInfo;

    pPacket = NULL;

    if (!pData || !ulSize)
    {
        goto cleanup;
    }

    HX_ASSERT(ulSize >= sizeof(HXPacketInfo));

    packetInfo.lost = (BYTE)*pData++;	    ulSize--;
    packetInfo.ASMFlags = (BYTE)*pData++;   ulSize--;

    uValue = (BYTE)*pData++; uValue |= (((BYTE)*pData++) << 8);
    packetInfo.ASMRuleNumber = uValue;	    ulSize -= 2;

    uValue = (BYTE)*pData++; uValue |= (((BYTE)*pData++) << 8);
    packetInfo.streamNum = uValue;	    ulSize -= 2;

    ulValue = (BYTE)*pData++; ulValue |= (((BYTE)*pData++) << 8);
    ulValue |= (((BYTE)*pData++) << 16); ulValue |= (((BYTE)*pData++) << 24);
    packetInfo.time = ulValue;		    ulSize -= 4;

    if (ulSize)
    {
        pBuffer = new CHXBuffer();
        pBuffer->AddRef();

        pBuffer->Set((const UCHAR*)pData, ulSize);
    }
        
    pPacket = new CHXPacket();
    pPacket->AddRef();

    pPacket->Set(pBuffer, 
                 packetInfo.time, 
                 packetInfo.streamNum,
                 packetInfo.ASMFlags,
                 packetInfo.ASMRuleNumber);

    if (packetInfo.lost)
    {
        pPacket->SetAsLost();
    }

cleanup:

    HX_RELEASE(pBuffer);

    return;
}

/////////////////////////////////////////////////////////////////////////
//	CHXRTPPacket
/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::QueryInterface
//	Purpose:
//		Implement this to export the interfaces supported by your 
//		object.
//
STDMETHODIMP
CHXRTPPacket::QueryInterface(REFIID riid, void** ppvObj)
{
	QInterfaceList qiList[] =
	{
		{ GET_IIDHANDLE(IID_IUnknown), this },
		{ GET_IIDHANDLE(IID_IHXRTPPacket), (IHXRTPPacket*) this },
		{ GET_IIDHANDLE(IID_IHXPacket), (IHXPacket*) this },
	};	
    return QIFind(qiList, QILISTSIZE(qiList), riid, ppvObj);   
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::Release
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32)
CHXRTPPacket::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }

    delete this;
    return 0;
}

/************************************************************************
 *	Method:
 *		IHXPacket::Set
 */
STDMETHODIMP
CHXRTPPacket::Set(IHXBuffer*  pBuffer, 
                  ULONG32     ulTime,
                  UINT16      uStreamNumber,
                  UINT8       unASMFlags,
                  UINT16      unASMRuleNumber)
{
    m_ulRTPTime = ulTime;

    return _Set(pBuffer, 
                ulTime,
                uStreamNumber,
                unASMFlags,
                unASMRuleNumber);
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::GetRTP
 */
STDMETHODIMP
CHXRTPPacket::GetRTP(REF(IHXBuffer*)  pBuffer, 
                     REF(ULONG32)     ulTime,
                     REF(ULONG32)     ulRTPTime,
                     REF(UINT16)      uStreamNumber,
                     REF(UINT8)       unASMFlags,
                     REF(UINT16)      unASMRuleNumber)
{
    ulRTPTime = m_ulRTPTime;

    return _Get(pBuffer,
                ulTime,
                uStreamNumber,
                unASMFlags,
                unASMRuleNumber);
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::GetRTPTime
 */
STDMETHODIMP_(ULONG32)
CHXRTPPacket::GetRTPTime()
{
    return m_ulRTPTime;
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::SetRTP
 */
STDMETHODIMP
CHXRTPPacket::SetRTP(IHXBuffer*  pBuffer, 
                     ULONG32     ulTime,
                     ULONG32     ulRTPTime,
                     UINT16      uStreamNumber,
                     UINT8       unASMFlags,
                     UINT16      unASMRuleNumber)
{
    m_ulRTPTime = ulRTPTime;

    return _Set(pBuffer, 
                ulTime,
                uStreamNumber,
                unASMFlags,
                unASMRuleNumber);
}

/////////////////////////////////////////////////////////////////////////
//	CHXHeader
//
/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::QueryInterface
//	Purpose:
//		Implement this to export the interfaces supported by your 
//		object.
//
STDMETHODIMP
CHXHeader::QueryInterface(REFIID riid, void** ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return HXR_OK;
    }
    else if (IsEqualIID(riid, IID_IHXValues))
    {
        AddRef();
        *ppvObj = (IHXValues*)this;
        return HXR_OK;
    }

    *ppvObj = NULL;
    return HXR_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::AddRef
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32)
CHXHeader::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::Release
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32)
CHXHeader::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }

    delete this;
    return 0;
}

CHXHeader::CValue&
CHXHeader::CValue::operator= (CHXHeader::CValue const& rhs)
{
    CValue(rhs).Swap(*this);
    return *this;
}

CHXHeader::~CHXHeader()
{
    for (Map::iterator i = m_Map.begin(); i != m_Map.end(); ++i)
    {
        delete[] (char*)i->first;
    }
}

CHXHeader::CValue::~CValue()
{
    HX_RELEASE(m_pIBuffer);
    HX_RELEASE(m_pIString);
}

bool
CHXHeader::MyCompare::operator()(const char* p1, const char* p2) const
{
    return (m_PreserveCase ? strcmp(p1, p2) : strcmpi(p1, p2)) < 0;
}

STDMETHODIMP
CHXHeader::SetPropertyULONG32(const char* pPropertyName,
                              ULONG32     uPropertyValue)
{
    return SetProperty(pPropertyName, &CValue::m_lVal, uPropertyValue);
}

STDMETHODIMP
CHXHeader::GetPropertyULONG32(const char*  pPropertyName,
                              REF(ULONG32) uPropertyValue)
{
    return GetProperty(pPropertyName, &CValue::m_lVal, uPropertyValue);
}

STDMETHODIMP
CHXHeader::GetFirstPropertyULONG32(REF(const char*) pPropertyName,
                                   REF(ULONG32)     uPropertyValue)
{
    m_ULONG32Position = m_Map.begin();
    return GetNextProperty(
                pPropertyName,
                &CValue::m_lVal,
                uPropertyValue,
                m_ULONG32Position);
}

STDMETHODIMP
CHXHeader::GetNextPropertyULONG32(REF(const char*) pPropertyName,
                                  REF(ULONG32)     uPropertyValue)
{
    if (m_ULONG32Position == m_Map.end())
    {
        return HXR_FAIL;
    }

    return GetNextProperty(
                pPropertyName,
                &CValue::m_lVal,
                uPropertyValue,
                ++m_ULONG32Position);
}

STDMETHODIMP
CHXHeader::SetPropertyBuffer(const char* pPropertyName,
                             IHXBuffer*  pPropertyValue)
{
    if (!pPropertyValue)
    {
        return HXR_FAIL;
    }

    return SetProperty(pPropertyName, &CValue::m_pIBuffer, pPropertyValue);
}

STDMETHODIMP
CHXHeader::GetPropertyBuffer(const char*     pPropertyName,
                             REF(IHXBuffer*) pPropertyValue)
{
    return GetProperty(pPropertyName, &CValue::m_pIBuffer, pPropertyValue);
}

STDMETHODIMP
CHXHeader::GetFirstPropertyBuffer(REF(const char*) pPropertyName,
                                  REF(IHXBuffer*)  pPropertyValue)
{
    m_BufferPosition = m_Map.begin();
    return GetNextProperty(
                pPropertyName,
                &CValue::m_pIBuffer,
                pPropertyValue,
                m_BufferPosition);
}

STDMETHODIMP
CHXHeader::GetNextPropertyBuffer(REF(const char*) pPropertyName,
                                 REF(IHXBuffer*)  pPropertyValue)
{
    if (m_BufferPosition == m_Map.end())
    {
        return HXR_FAIL;
    }

    return GetNextProperty(
               pPropertyName,
               &CValue::m_pIBuffer,
               pPropertyValue,
               ++m_BufferPosition);
}

STDMETHODIMP
CHXHeader::SetPropertyCString(const char* pPropertyName,
                              IHXBuffer*  pPropertyValue)
{
    if (!pPropertyValue)
    {
        return HXR_FAIL;
    }

    return SetProperty(pPropertyName, &CValue::m_pIString, pPropertyValue);
}

STDMETHODIMP
CHXHeader::GetPropertyCString(const char*     pPropertyName,
                              REF(IHXBuffer*) pPropertyValue)
{
        return GetProperty(pPropertyName, &CValue::m_pIString, pPropertyValue);
}

STDMETHODIMP
CHXHeader::GetFirstPropertyCString(REF(const char*) pPropertyName,
                                   REF(IHXBuffer*)  pPropertyValue)
{
    m_CStringPosition = m_Map.begin();
    return GetNextProperty(
               pPropertyName,
               &CValue::m_pIString,
               pPropertyValue,
               m_CStringPosition);
}

STDMETHODIMP
CHXHeader::GetNextPropertyCString(REF(const char*) pPropertyName,
                                  REF(IHXBuffer*)  pPropertyValue)
{
    if (m_CStringPosition == m_Map.end())
    {
        return HXR_FAIL;
    }

    return GetNextProperty(
               pPropertyName,
               &CValue::m_pIString,
               pPropertyValue,
               ++m_CStringPosition);
}

void
CHXHeader::PreserveCase(BOOL bPreserve)
{
    HX_ASSERT(m_Map.empty());
    Map(MyCompare(bPreserve)).swap(m_Map);
}

void
CHXHeader::mergeHeaders(IHXValues* pDestHeaders, 
                        IHXValues* pSrcHeaders)
{
#if 0
    if (!pDestHeaders || !pSrcHeaders) return;
    // XXXHP - caveat emptor
    CHXHeader& sourceHeader = *((CHXHeader*) pSrcHeaders);
    CHXHeader& destHeader = *((CHXHeader*) pDestHeaders);

    for (Map::const_iterator i = sourceHeader.m_Map.begin (); i != sourceHeader.m_Map.end (); ++i)
    {
        Map::iterator j = destHeader.m_Map.find(i->first);
        if (j != destHeader.m_Map.end())
        {
            CValue& lhs = j->second;
            const CValue& rhs = i->second;
            if (rhs.m_lValValid) lhs.Set(&CValue::m_lVal, rhs.m_lVal);
            if (rhs.m_pIBuffer) lhs.Set(&CValue::m_pIBuffer, rhs.m_pIBuffer);
            if (rhs.m_pIString) lhs.Set(&CValue::m_pIString, rhs.m_pIString);
        }
        else
        {
            char* pTmp = new char[strlen(i->first) + 1];
            strcpy(pTmp, i->first); /* Flawfinder: ignore */
            Map::value_type newVal(pTmp, i->second);
            newVal.second.m_Key = newVal.first;
            destHeader.m_Map.insert(newVal);
        }
    }
#else
    if (pSrcHeaders && pDestHeaders)
    {
        // add these headers to the existing ones
        //

        const char* pPropName = NULL;
        UINT32 ulPropValue = 0;

        HX_RESULT rc =
            pSrcHeaders->GetFirstPropertyULONG32(pPropName, ulPropValue);

        while (SUCCEEDED(rc))
        {
            pDestHeaders->SetPropertyULONG32(pPropName, ulPropValue);
            rc = pSrcHeaders->GetNextPropertyULONG32(pPropName, ulPropValue);
        }

        IHXBuffer* pBuffer = NULL;
        rc = pSrcHeaders->GetFirstPropertyBuffer(pPropName, pBuffer);

        while (SUCCEEDED(rc))
        {
            pDestHeaders->SetPropertyBuffer(pPropName, pBuffer);
            pBuffer->Release();
            rc = pSrcHeaders->GetNextPropertyBuffer(pPropName, pBuffer);
        }

        rc = pSrcHeaders->GetFirstPropertyCString(pPropName, pBuffer);

        while (SUCCEEDED(rc))
        {
            pDestHeaders->SetPropertyCString(pPropName, pBuffer);
            pBuffer->Release();
            rc = pSrcHeaders->GetNextPropertyCString(pPropName, pBuffer);
        }
    }
#endif
}

_CStoreName::_CStoreName() : m_strName()
{
}
_CStoreName::~_CStoreName()
{
}

const
CHXString& _CStoreName::GetName() const
{
    return m_strName;
}
void
_CStoreName::SetName(const char* szName)
{
    m_strName = szName;
}

_CStoreNameUINT32Pair::_CStoreNameUINT32Pair() : m_ulValue(0)
{
}
_CStoreNameUINT32Pair::~_CStoreNameUINT32Pair()
{
}

UINT32
_CStoreNameUINT32Pair::GetValue()
{
    return m_ulValue;
}

void
_CStoreNameUINT32Pair::SetValue(UINT32 ulValue)
{
    m_ulValue = ulValue;
}

_CStoreNameBufferPair::_CStoreNameBufferPair() : m_pbufValue(NULL)
{
}

_CStoreNameBufferPair::~_CStoreNameBufferPair()
{
    HX_RELEASE(m_pbufValue);
}

IHXBuffer*
_CStoreNameBufferPair::GetValue()
{
    if (m_pbufValue)
    {
        m_pbufValue->AddRef();
    }
    return m_pbufValue;
}

void
_CStoreNameBufferPair::SetValue(IHXBuffer* pbufValue)
{
    HX_RELEASE(m_pbufValue);

    m_pbufValue = pbufValue;

    if (m_pbufValue)
    {
        m_pbufValue->AddRef();
    }
}

#if 0
IHXBuffer*
CHXHeader::encodeBuffer(IHXBuffer* pBuffer)
{
    //XXXBAB - come back to this soon...
    CHXBuffer* pEncodedBuffer = new CHXBuffer;
    pEncodedBuffer->AddRef();
    pEncodedBuffer->Set(pBuffer->GetBuffer(), pBuffer->GetSize());
    return pEncodedBuffer;
}

IHXBuffer*
CHXHeader::encodeCString(IHXBuffer* pBuffer)
{
    CHXString tmp;
    const char* pStr = (const char*)pBuffer->GetBuffer();
    BOOL inEscape = FALSE;
    for(size_t i=0;i<strlen(pStr);++i)
    {
        switch(pStr[i])
        {
            case '\n':
            {
                tmp += "\\n";
            }
            break;

            case '\t':
            {
                tmp += "\\t";
            }
            break;

            case '\r':
            {
                tmp += "\\r";
            }
            break;

            case '\"':
            {
                tmp += "\\\"";
            }
            break;

            case '\\':
            {
                tmp += "\\\\";
            }
            break;

            default:
            {
                tmp += pStr[i];
            }
        }
    }
    CHXBuffer* pEncodedBuffer = new CHXBuffer;
    pEncodedBuffer->AddRef();
    pEncodedBuffer->Set((const BYTE*)(const char*)tmp, tmp.GetLength()+1);
    return pEncodedBuffer;
}
#endif

#endif
