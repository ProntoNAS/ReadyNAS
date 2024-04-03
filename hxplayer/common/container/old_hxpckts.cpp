/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: old_hxpckts.cpp,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

/************************************************************************
 *  Defines
 */
#define EXTREME_RTP_TIME    0xFFFFFFFF


/************************************************************************
 *  Includes
 */
#include "hxtypes.h"
#include "hxresult.h"
#include "hxcom.h"
#include "ihxpckts.h"
#include "old_hxpckts.h"
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
STDMETHODIMP CHXPacket::QueryInterface(REFIID riid, void** ppvObj)
{
	QInterfaceList qiList[] =
	{
		{ GET_IIDHANDLE(IID_IUnknown), this },
		{ GET_IIDHANDLE(IID_IHXPacket), (IHXPacket*) this },
	};	
    return QIFind(qiList, QILISTSIZE(qiList), riid, ppvObj);   
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::AddRef
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32) CHXPacket::AddRef()
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
STDMETHODIMP_(ULONG32) CHXPacket::Release()
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
STDMETHODIMP CHXPacket::Get
(
    REF(IHXBuffer*)	pBuffer, 
    REF(ULONG32)	ulTime,
    REF(UINT16)		uStreamNumber,
    REF(UINT8)		unASMFlags,
    REF(UINT16)		unASMRuleNumber
)
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
STDMETHODIMP_(IHXBuffer*) CHXPacket::GetBuffer()
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
STDMETHODIMP_(ULONG32) CHXPacket::GetTime()
{
    return m_ulTime;
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetFlags
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(UINT8) CHXPacket::GetASMFlags()
{
    return m_unASMFlags;
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetStreamNumber
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(UINT16) CHXPacket::GetStreamNumber()
{
    return m_uStreamNumber;
}

/************************************************************************
 *	Method:
 *		IHXPacket::GetASMRuleNumber
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(UINT16) CHXPacket::GetASMRuleNumber()
{
    return m_unASMRuleNumber;
}

/************************************************************************
 *	Method:
 *		IHXPacket::IsLost
 *	Purpose:
 *		TBD
 */
STDMETHODIMP_(BOOL) CHXPacket::IsLost()
{ 
    return m_bIsLost;
}

/************************************************************************
 *	Method:
 *		IHXPacket::SetAsLost
 *	Purpose:
 *		TBD
 */
STDMETHODIMP CHXPacket::SetAsLost()
{
    HX_ASSERT(m_pBuffer == NULL);
    HX_ASSERT(m_ulTime == 0);
    if (m_pBuffer) return HXR_UNEXPECTED;
    m_bIsLost = TRUE;
    return HXR_OK; 
}

/************************************************************************
 *	Method:
 *		IHXPacket::Set
 *	Purpose:
 *		TBD
 */
STDMETHODIMP CHXPacket::Set
(
    IHXBuffer*		pBuffer, 
    ULONG32		ulTime,
    UINT16		uStreamNumber,
    UINT8		unASMFlags,
    UINT16		unASMRuleNumber
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
STDMETHODIMP CHXPacket::GetRTP				
(
    REF(IHXBuffer*)	pBuffer, 
    REF(ULONG32)	ulTime,
    REF(ULONG32)	ulRTPTime,
    REF(UINT16)		uStreamNumber,
    REF(UINT8)		unASMFlags,
    REF(UINT16)		unASMRuleNumber
)
{
    HX_ASSERT(HXR_UNEXPECTED == HXR_OK);
    return HXR_UNEXPECTED;
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::GetRTPTime
 */
STDMETHODIMP_(ULONG32) CHXPacket::GetRTPTime()
{
    HX_ASSERT(HXR_UNEXPECTED == HXR_OK);
    return EXTREME_RTP_TIME;
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::SetRTP
 */
STDMETHODIMP CHXPacket::SetRTP
(
    IHXBuffer*	    pBuffer, 
    ULONG32	    ulTime,
    ULONG32	    ulRTPTime,
    UINT16	    uStreamNumber,
    UINT8	    unASMFlags,
    UINT16	    unASMRuleNumber
)
{
    HX_ASSERT(HXR_UNEXPECTED == HXR_OK);
    return HXR_UNEXPECTED;
}

inline HX_RESULT CHXPacket::_Get(IHXBuffer*   &pBuffer,
				 ULONG32	    &ulTime,
				 UINT16	    &uStreamNumber,
				 UINT8	    &unASMFlags,
				 UINT16	    &unASMRuleNumber)
{
    pBuffer	    = m_pBuffer;
    ulTime	    = m_ulTime;
    uStreamNumber   = m_uStreamNumber;
    unASMFlags	    = m_unASMFlags;
    unASMRuleNumber = m_unASMRuleNumber;

    // Whenever a COM object is returned out of an
    // interface, it should be AddRef()'d.
    if (m_pBuffer) m_pBuffer->AddRef();

    return HXR_OK;
}

inline HX_RESULT CHXPacket::_Set(IHXBuffer*   pBuffer, 
				 ULONG32	    ulTime,
				 UINT16	    uStreamNumber,
				 UINT8	    unASMFlags,
				 UINT16	    unASMRuleNumber)
{
    /* We allow changing the packet info when it is owned
     * by atmost one user.
     */
    if (m_lRefCount > 1)
    {
	HX_ASSERT(FALSE);
	return HXR_UNEXPECTED;
    }

    if (m_pBuffer) m_pBuffer->Release();
    m_pBuffer = pBuffer;
    if (m_pBuffer)
    {
	m_pBuffer->AddRef();
    }

    m_ulTime		= ulTime;
    m_uStreamNumber	= uStreamNumber;
    m_unASMFlags	= unASMFlags;
    m_unASMRuleNumber	= unASMRuleNumber;

    return HXR_OK;
}

// serialization method
void
CHXPacket::Pack(IHXPacket* pPacket, char* pData, UINT32& ulSize)
{
    UINT16	uValue = 0;
    UINT32	ulValue = 0;
    UINT32	ulBufferSize = 0;
    IHXBuffer*	pBuffer = NULL;

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
    UINT16	    uValue = 0;
    UINT32	    ulValue = 0;
    IHXBuffer*	    pBuffer = NULL;
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
STDMETHODIMP CHXRTPPacket::QueryInterface(REFIID riid, void** ppvObj)
{
    if (IsEqualIID(riid, IID_IHXRTPPacket))
    {
	AddRef();
	*ppvObj = (IHXRTPPacket*) this;
	return HXR_OK;
    }
    else if (IsEqualIID(riid, IID_IHXPacket))
    {
	AddRef();
	*ppvObj = (IHXPacket*) this;
	return HXR_OK;
    }
    else if (IsEqualIID(riid, IID_IUnknown))
    {
	AddRef();
	*ppvObj = this;
	return HXR_OK;
    }

    *ppvObj = NULL;
    return HXR_NOINTERFACE;
}

/////////////////////////////////////////////////////////////////////////
//	Method:
//		IUnknown::Release
//	Purpose:
//		Everyone usually implements this the same... feel free to use
//		this implementation.
//
STDMETHODIMP_(ULONG32) CHXRTPPacket::Release()
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
STDMETHODIMP CHXRTPPacket::Set
(
    IHXBuffer*		pBuffer, 
    ULONG32		ulTime,
    UINT16		uStreamNumber,
    UINT8		unASMFlags,
    UINT16		unASMRuleNumber
)
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
STDMETHODIMP CHXRTPPacket::GetRTP				
(
    REF(IHXBuffer*)	pBuffer, 
    REF(ULONG32)	ulTime,
    REF(ULONG32)	ulRTPTime,
    REF(UINT16)		uStreamNumber,
    REF(UINT8)		unASMFlags,
    REF(UINT16)		unASMRuleNumber
)
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
STDMETHODIMP_(ULONG32) CHXRTPPacket::GetRTPTime()
{
    return m_ulRTPTime;
}

/************************************************************************
 *	Method:
 *		IHXRTPPacket::SetRTP
 */
STDMETHODIMP CHXRTPPacket::SetRTP
(
    IHXBuffer*	    pBuffer, 
    ULONG32	    ulTime,
    ULONG32	    ulRTPTime,
    UINT16	    uStreamNumber,
    UINT8	    unASMFlags,
    UINT16	    unASMRuleNumber
)
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
STDMETHODIMP CHXHeader::QueryInterface(REFIID riid, void** ppvObj)
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
STDMETHODIMP_(ULONG32) CHXHeader::AddRef()
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
STDMETHODIMP_(ULONG32) CHXHeader::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
	return m_lRefCount;
    }

    delete this;
    return 0;
}

CHXHeader::~CHXHeader()
{
    CHXMapStringToOb::Iterator i;

    // clean up UINT32 properties
    for (i = m_ULONG32Map.Begin(); i != m_ULONG32Map.End(); ++i)
    {
	_CStoreNameUINT32Pair* pnupExpired = (_CStoreNameUINT32Pair*)*i;
	delete pnupExpired;
    }
    m_ULONG32Map.RemoveAll();

    // clean up buffer properties
    for (i = m_BufferMap.Begin(); i != m_BufferMap.End(); ++i)
    {
	_CStoreNameBufferPair* pnbpExpired = (_CStoreNameBufferPair*)*i;
	delete pnbpExpired;
    }
    m_BufferMap.RemoveAll();

    // clean up string properties
    for (i = m_CStringMap.Begin(); i != m_CStringMap.End(); ++i)
    {
	_CStoreNameBufferPair* pnbpExpired = (_CStoreNameBufferPair*)*i;
	delete pnbpExpired;
    }
    m_CStringMap.RemoveAll();
}

STDMETHODIMP
CHXHeader::SetPropertyULONG32	     (
		const char*          pPropertyName,
		ULONG32              uPropertyValue)
{
    _CStoreNameUINT32Pair* pnupNew = new _CStoreNameUINT32Pair;
    if(!pnupNew)
    {
        return HXR_OUTOFMEMORY;
    }

    pnupNew->SetName(pPropertyName);
    pnupNew->SetValue(uPropertyValue);

    char* pMapString = new_string(pPropertyName);
    if(!pMapString)
    {
        HX_DELETE(pnupNew);
        return HXR_OUTOFMEMORY;
    }

    if(!m_bPreserveCase)
    {
	strlwr(pMapString);
    }

    // First make sure there isn't already a property by that name set!
    _CStoreNameUINT32Pair* pnupExpired = NULL;
    if (m_ULONG32Map.Lookup(pMapString, (void*&)pnupExpired))
    {
	/* Delete the previously added property */
	delete pnupExpired;
    }

    m_ULONG32Map[pMapString] = (void*)pnupNew;
    delete [] pMapString;
    return HXR_OK;
}

STDMETHODIMP
CHXHeader::GetPropertyULONG32	     (
		const char*          pPropertyName,
		REF(ULONG32)         uPropertyValue)
{
    HX_RESULT rc = HXR_OK;

    _CStoreNameUINT32Pair* pnupData = NULL;

    char* pMapString = new_string(pPropertyName);
    if( !pMapString )
    {
        return HXR_OUTOFMEMORY;
    }
   
    if(!m_bPreserveCase)
    {
	strlwr(pMapString);
    }

    if (!m_ULONG32Map.Lookup(pMapString, (void*&)pnupData))
    {
	rc = HXR_FAILED;
    }
    else
    {
	uPropertyValue = pnupData->GetValue();
    }

    delete[] pMapString;

    return rc;
}

STDMETHODIMP
CHXHeader::GetFirstPropertyULONG32   (
		REF(const char*)     pPropertyName,
		REF(ULONG32)         uPropertyValue)
{	
    if ((m_ULONG32Position = m_ULONG32Map.GetStartPosition()) == NULL)
	return HXR_FAILED;

    _CStoreNameUINT32Pair* pnupData = NULL;

    m_ULONG32Map.GetNextAssoc(m_ULONG32Position,
                              pPropertyName,
                              (void*&)pnupData);

    pPropertyName = pnupData->GetName();
    uPropertyValue = pnupData->GetValue();

    return HXR_OK;
}

STDMETHODIMP
CHXHeader::GetNextPropertyULONG32   (
		REF(const char*)    pPropertyName,
		REF(ULONG32)        uPropertyValue)
{	
    if (!m_ULONG32Position)
	return HXR_FAILED;

    _CStoreNameUINT32Pair* pnupData = NULL;

    m_ULONG32Map.GetNextAssoc(m_ULONG32Position,
                              pPropertyName,
                              (void*&)pnupData);

    pPropertyName = pnupData->GetName();
    uPropertyValue = pnupData->GetValue();

    return HXR_OK;
}

STDMETHODIMP
CHXHeader::SetPropertyBuffer	    (
		const char*         pPropertyName,
		IHXBuffer*         pPropertyValue)
{
    if (!pPropertyValue) return HXR_UNEXPECTED;

    _CStoreNameBufferPair* pnbpNew = new _CStoreNameBufferPair;
    if(!pnbpNew)
    {
        return HXR_OUTOFMEMORY;
    }

    pnbpNew->SetName(pPropertyName);
    pnbpNew->SetValue(pPropertyValue);

    char* pMapString = new_string(pPropertyName);
    if(!pMapString)
    {
        HX_DELETE(pnbpNew);
        return HXR_OUTOFMEMORY;
    }

    if(!m_bPreserveCase)
    {
	strlwr(pMapString);
    }

    // First make sure there isn't already a property by that name set!
    _CStoreNameBufferPair* pnbpExpired = NULL;
    if (m_BufferMap.Lookup(pMapString, (void*&)pnbpExpired))
    {
	/* Delete the previously added property */
	delete pnbpExpired;
    }

    m_BufferMap[pMapString] = (void*)pnbpNew;
    delete [] pMapString;
    return HXR_OK;
}

STDMETHODIMP
CHXHeader::GetPropertyBuffer	    (
		const char*         pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{
    HX_RESULT rc = HXR_OK;

    _CStoreNameBufferPair* pnbpData = NULL;

    char* pMapString = new_string(pPropertyName);
    if(!pMapString)
    {
        return HXR_OUTOFMEMORY;
    }

    if(!m_bPreserveCase)
    {
	strlwr(pMapString);
    }

    if (!m_BufferMap.Lookup(pMapString, (void*&)pnbpData))
    {
	rc = HXR_FAILED;
    }
    else
    {
	pPropertyValue = pnbpData->GetValue();
    }

    delete [] pMapString;
    return rc;
}

#if 0
STDMETHODIMP
CHXHeader::GetEncodedPropertyBuffer(
		const char*	    pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{
    IHXBuffer* pTempPropertyValue;
    char* lwr;
    lwr = new_string(pPropertyName);
    strlwr(lwr);

    if (!m_BufferMap.Lookup(lwr, (void*&)pTempPropertyValue))
    {
	delete [] lwr;
	return HXR_FAILED;
    }

    pPropertyValue = encodeBuffer(pTempPropertyValue);
    delete [] lwr;
    return HXR_OK;
} 
#endif

STDMETHODIMP
CHXHeader::GetFirstPropertyBuffer   (
		REF(const char*)    pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{	
    if ((m_BufferPosition = m_BufferMap.GetStartPosition()) == NULL)
	return HXR_FAILED;

    _CStoreNameBufferPair* pnbpData = NULL;

    m_BufferMap.GetNextAssoc(m_BufferPosition,
                             pPropertyName,
                             (void*&)pnbpData);

    pPropertyName = pnbpData->GetName();
    pPropertyValue = pnbpData->GetValue();

    return HXR_OK;
}

STDMETHODIMP
CHXHeader::GetNextPropertyBuffer    (
		REF(const char*)    pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{	
    if (!m_BufferPosition)
	return HXR_FAILED;

    _CStoreNameBufferPair* pnbpData = NULL;

    m_BufferMap.GetNextAssoc(m_BufferPosition,
                             pPropertyName,
                             (void*&)pnbpData);

    pPropertyName = pnbpData->GetName();
    pPropertyValue = pnbpData->GetValue();


    return HXR_OK;
}

STDMETHODIMP
CHXHeader::SetPropertyCString	    (
		const char*         pPropertyName,
		IHXBuffer*         pPropertyValue)
{
    if (!pPropertyValue) return HXR_UNEXPECTED;

    _CStoreNameBufferPair* pnbpNew = new _CStoreNameBufferPair;
    if(!pnbpNew)
    {
        return HXR_OUTOFMEMORY;
    }

    pnbpNew->SetName(pPropertyName);
    pnbpNew->SetValue(pPropertyValue);

    char* pMapString = new_string(pPropertyName);
    if(!pMapString)
    {
        HX_DELETE(pnbpNew);
        return HXR_OUTOFMEMORY;
    }

    if(!m_bPreserveCase)
    {
	strlwr(pMapString);
    }

    // First make sure there isn't already a property by that name set!
    _CStoreNameBufferPair* pnbpExpired = NULL;
    if (m_CStringMap.Lookup(pMapString, (void*&)pnbpExpired))
    {
	/* Delete the previously added property */
	delete pnbpExpired;
    }

    m_CStringMap[pMapString] = (void*)pnbpNew;
    delete[] pMapString;
    return HXR_OK;
}

STDMETHODIMP
CHXHeader::GetPropertyCString	    (
		const char*         pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{
    HX_RESULT rc = HXR_OK;

    _CStoreNameBufferPair* pnbpData = NULL;

    char* pMapString = new_string(pPropertyName);
    if(!pMapString)
    {
        return HXR_OUTOFMEMORY;
    }

    if(!m_bPreserveCase)
    {
	strlwr(pMapString);
    }

    if (!m_CStringMap.Lookup(pMapString, (void*&)pnbpData))
    {
	rc = HXR_FAILED;
    }
    else
    {
	pPropertyValue = pnbpData->GetValue();
    }

    delete[] pMapString;
    return rc;
}

#if 0
STDMETHODIMP
CHXHeader::GetEncodedPropertyCString (
		const char*	    pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{
    char* lwr;
    lwr = new_string(pPropertyName);
    strlwr(lwr);

    IHXBuffer* pTempPropertyValue;

    if (!m_CStringMap.Lookup(lwr, (void*&)pTempPropertyValue))
    {
	delete[] lwr;
	return HXR_FAILED;
    }

    pPropertyValue = encodeCString(pTempPropertyValue);

    delete[] lwr;
    return HXR_OK;
}
#endif

STDMETHODIMP
CHXHeader::GetFirstPropertyCString   (
		REF(const char*)    pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{	
    if ((m_CStringPosition = m_CStringMap.GetStartPosition()) == NULL)
	return HXR_FAILED;

    _CStoreNameBufferPair* pnbpData = NULL;

    m_CStringMap.GetNextAssoc(m_CStringPosition,
                              pPropertyName,
                              (void*&)pnbpData);

    pPropertyName = pnbpData->GetName();
    pPropertyValue = pnbpData->GetValue();

    return HXR_OK;
}

STDMETHODIMP
CHXHeader::GetNextPropertyCString    (
		REF(const char*)    pPropertyName,
		REF(IHXBuffer*)    pPropertyValue)
{	
    if (!m_CStringPosition)
	return HXR_FAILED;

    _CStoreNameBufferPair* pnbpData = NULL;

    m_CStringMap.GetNextAssoc(m_CStringPosition,
                              pPropertyName,
                              (void*&)pnbpData);

    pPropertyName = pnbpData->GetName();
    pPropertyValue = pnbpData->GetValue();

    return HXR_OK;
}

void
CHXHeader::mergeHeaders
(
    IHXValues* pIHXValuesDestHeaders, 
    IHXValues* pIHXValuesSourceHeaders
)
{
    if(pIHXValuesSourceHeaders && pIHXValuesDestHeaders)
    {
	// add these headers to the existing ones
	//

	const char* pPropName = NULL;
	UINT32 ulPropValue = 0;

	HX_RESULT rc = pIHXValuesSourceHeaders->GetFirstPropertyULONG32
	(
	    pPropName, 
	    ulPropValue
	);

	while(SUCCEEDED(rc))
	{
	    pIHXValuesDestHeaders->SetPropertyULONG32
	    (
		pPropName, 
		ulPropValue
	    );

	    rc = pIHXValuesSourceHeaders->GetNextPropertyULONG32
	    (
		pPropName, 
		ulPropValue
	    );
	}

	IHXBuffer* pBuffer = NULL;
	rc = pIHXValuesSourceHeaders->GetFirstPropertyBuffer
	(
	    pPropName, 
	    pBuffer
	);

	while(SUCCEEDED(rc))
	{
	    pIHXValuesDestHeaders->SetPropertyBuffer(pPropName, pBuffer);
	    pBuffer->Release();
	    rc = pIHXValuesSourceHeaders->GetNextPropertyBuffer
	    (
		pPropName, 
		pBuffer
	    );
	}

	rc = pIHXValuesSourceHeaders->GetFirstPropertyCString
	(
	    pPropName, 
	    pBuffer
	);

	while(SUCCEEDED(rc))
	{
	    pIHXValuesDestHeaders->SetPropertyCString(pPropName, pBuffer);
	    pBuffer->Release();
	    rc = pIHXValuesSourceHeaders->GetNextPropertyCString
	    (
		pPropName, 
		pBuffer
	    );
	}
    }
}

_CStoreName::_CStoreName() : m_strName()
{
}
_CStoreName::~_CStoreName()
{
}

const CHXString& _CStoreName::GetName() const
{
    return m_strName;
}
void _CStoreName::SetName(const char* szName)
{
    m_strName = szName;
}

_CStoreNameUINT32Pair::_CStoreNameUINT32Pair() : m_ulValue(0)
{
}
_CStoreNameUINT32Pair::~_CStoreNameUINT32Pair()
{
}

UINT32 _CStoreNameUINT32Pair::GetValue()
{
    return m_ulValue;
}
void _CStoreNameUINT32Pair::SetValue(UINT32 ulValue)
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

IHXBuffer* _CStoreNameBufferPair::GetValue()
{
    if(m_pbufValue)
    {
	m_pbufValue->AddRef();
    }
    return m_pbufValue;
}
void _CStoreNameBufferPair::SetValue(IHXBuffer* pbufValue)
{
    HX_RELEASE(m_pbufValue);

    m_pbufValue = pbufValue;

    if(m_pbufValue)
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
