/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ihxpckts.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef _IHXPCKTS_H_
#define _IHXPCKTS_H_

// Define IHXUtilities
// $Private
#include "hxvalue.h"
// $EndPrivate

/* ASMFlags in IHXPacket */
#define HX_ASM_SWITCH_ON	 0x01
#define HX_ASM_SWITCH_OFF	 0x02
#define HX_ASM_DROPPED_PKT	 0x04


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXBuffer
 *
 *  Purpose:
 *
 *	Basic opaque data storage buffer. Used in interfaces where 
 *	object ownership is best managed through COM style reference 
 *	counting.
 *
 *  IID_IHXBuffer:
 *
 *	{00001300-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXBuffer, 0x00001300, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

/*
 *  The IHXCommonClassFactory supports creating an instance
 *  of this object.
 */
#define CLSID_IHXBuffer IID_IHXBuffer

#undef  INTERFACE
#define INTERFACE   IHXBuffer

DECLARE_INTERFACE_(IHXBuffer, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXBuffer methods
     */
    STDMETHOD(Get)		(THIS_
				REF(UCHAR*)	pData, 
				REF(ULONG32)	ulLength) PURE;

    STDMETHOD(Set)		(THIS_
				const UCHAR*	pData, 
				ULONG32		ulLength) PURE;

    STDMETHOD(SetSize)		(THIS_
				ULONG32		ulLength) PURE;

    STDMETHOD_(ULONG32,GetSize)	(THIS) PURE;

    STDMETHOD_(UCHAR*,GetBuffer)(THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXPacket
 *
 *  Purpose:
 *
 *	Basic data packet in the RealMedia system.
 *
 *  IID_IHXPacket:
 *
 *	{00001301-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXPacket, 0x00001301, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

/*
 *  The IHXCommonClassFactory supports creating an instance
 *  of this object.
 */
#define CLSID_IHXPacket IID_IHXPacket

#undef  INTERFACE
#define INTERFACE   IHXPacket

DECLARE_INTERFACE_(IHXPacket, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXPacket methods
     */
    STDMETHOD(Get)			(THIS_
					REF(IHXBuffer*)    pBuffer, 
					REF(UINT32)	    ulTime,
					REF(UINT16)	    unStreamNumber,
					REF(UINT8)	    unASMFlags,
					REF(UINT16)	    unASMRuleNumber
					) PURE;

    STDMETHOD_(IHXBuffer*,GetBuffer)	(THIS) PURE;

    STDMETHOD_(ULONG32,GetTime)		(THIS) PURE;

    STDMETHOD_(UINT16,GetStreamNumber)	(THIS) PURE;

    STDMETHOD_(UINT8,GetASMFlags)	(THIS) PURE;

    STDMETHOD_(UINT16,GetASMRuleNumber)	(THIS) PURE;

    STDMETHOD_(BOOL,IsLost)		(THIS) PURE;

    STDMETHOD(SetAsLost)		(THIS) PURE;

    STDMETHOD(Set)			(THIS_
					IHXBuffer* 	    pBuffer,
					UINT32	    	    ulTime,
					UINT16	    	    uStreamNumber,
					UINT8	    	    unASMFlags,
					UINT16	    	    unASMRuleNumber
					) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXPacket
 *
 *  Purpose:
 *
 *	RTP data packet in the RealMedia system.
 *
 *  IID_IHXRTPPacket:
 *
 *	{0169A731-1ED0-11d4-952B-00902742C923}
 *
 */
DEFINE_GUID(IID_IHXRTPPacket, 0x169a731, 0x1ed0, 0x11d4, 0x95, 0x2b, 0x0, 
	    0x90, 0x27, 0x42, 0xc9, 0x23);

/*
 *  The IHXCommonClassFactory supports creating an instance
 *  of this object.
 */
#define CLSID_IHXRTPPacket IID_IHXRTPPacket

#undef  INTERFACE
#define INTERFACE   IHXRTPPacket

DECLARE_INTERFACE_(IHXRTPPacket, IHXPacket)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXPacket methods
     */
    STDMETHOD(Get)			(THIS_
					REF(IHXBuffer*)    pBuffer, 
					REF(UINT32)	    ulTime,
					REF(UINT16)	    unStreamNumber,
					REF(UINT8)	    unASMFlags,
					REF(UINT16)	    unASMRuleNumber
					) PURE;

    STDMETHOD_(IHXBuffer*,GetBuffer)	(THIS) PURE;

    STDMETHOD_(ULONG32,GetTime)		(THIS) PURE;

    STDMETHOD_(UINT16,GetStreamNumber)	(THIS) PURE;

    STDMETHOD_(UINT8,GetASMFlags)	(THIS) PURE;

    STDMETHOD_(UINT16,GetASMRuleNumber)	(THIS) PURE;

    STDMETHOD_(BOOL,IsLost)		(THIS) PURE;

    STDMETHOD(SetAsLost)		(THIS) PURE;

    STDMETHOD(Set)			(THIS_
					IHXBuffer* 	    pBuffer,
					UINT32	    	    ulTime,
					UINT16	    	    uStreamNumber,
					UINT8	    	    unASMFlags,
					UINT16	    	    unASMRuleNumber
					) PURE;

    /*
     *	IHXRTPPacket methods
     */
    STDMETHOD_(ULONG32,GetRTPTime)	(THIS) PURE;

    STDMETHOD(GetRTP)			(THIS_
					REF(IHXBuffer*)    pBuffer, 
					REF(UINT32)	    ulTime,
					REF(UINT32)	    ulRTPTime,
					REF(UINT16)	    unStreamNumber,
					REF(UINT8)	    unASMFlags,
					REF(UINT16)	    unASMRuleNumber
					) PURE;

    STDMETHOD(SetRTP)			(THIS_
					IHXBuffer* 	    pBuffer,
					UINT32	    	    ulTime,
					UINT32		    ulRTPTime,
					UINT16	    	    uStreamNumber,
					UINT8	    	    unASMFlags,
					UINT16	    	    unASMRuleNumber
					) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXRTPPacketInfo
 *
 *  Purpose:
 *
 *	Provides complete RTP packet header info (RFC 1889)
 *
 *  IID_IHXPacket:
 *
 *	{EC7D67BB-2E79-49c3-B667-BA8A938DBCE0}
 *
 */
DEFINE_GUID(IID_IHXRTPPacketInfo, 
    0xec7d67bb, 0x2e79, 0x49c3, 0xb6, 0x67, 0xba, 0x8a, 0x93, 0x8d, 0xbc, 0xe0);

#undef  INTERFACE
#define INTERFACE   IHXRTPPacketInfo

DECLARE_INTERFACE_(IHXRTPPacketInfo, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXRTPPacketInfo methods
     */
    STDMETHOD_(UINT8, GetVersion)   (THIS) PURE;
    
    STDMETHOD(GetPaddingBit)	    (THIS_ REF(BOOL)bPadding) PURE;
    STDMETHOD(SetPaddingBit)	    (THIS_ BOOL bPadding) PURE;

    STDMETHOD(GetExtensionBit)	    (THIS_ REF(BOOL)bExtension) PURE;
    STDMETHOD(SetExtensionBit)	    (THIS_ BOOL bExtension) PURE;

    STDMETHOD(GetCSRCCount)	    (THIS_ REF(UINT8)unCSRCCount) PURE;
    STDMETHOD(SetCSRCCount)	    (THIS_ UINT8 unCSRCCount) PURE;

    STDMETHOD(GetMarkerBit)	    (THIS_ REF(BOOL)bMarker) PURE;
    STDMETHOD(SetMarkerBit)	    (THIS_ BOOL bMarker) PURE;

    STDMETHOD(GetPayloadType)	    (THIS_ REF(UINT8)unPayloadType) PURE;
    STDMETHOD(SetPayloadType)	    (THIS_ UINT8 unPayloadType) PURE;

    STDMETHOD(GetSequenceNumber)    (THIS_ REF(UINT16)unSeqNo) PURE;
    STDMETHOD(SetSequenceNumber)    (THIS_ UINT16 unSeqNo) PURE;

    STDMETHOD(GetTimeStamp)	    (THIS_ REF(UINT32)ulTS) PURE;
    STDMETHOD(SetTimeStamp)	    (THIS_ UINT32 ulTS) PURE;

    STDMETHOD(GetSSRC)		    (THIS_ REF(UINT32)ulSSRC) PURE;
    STDMETHOD(SetSSRC)		    (THIS_ UINT32 ulSSRC) PURE;

    
    STDMETHOD(GetCSRCList)	    (THIS_ REF(const char*) pulCSRC) PURE;
    STDMETHOD(SetCSRCList)	    (THIS_ const char* pCSRCList, UINT32 ulSize) PURE;
    STDMETHOD(GetPadding)	    (THIS_ REF(const char*) pPadding) PURE;
    STDMETHOD(SetPadding)	    (THIS_ const char* pPadding, UINT32 ulSize) PURE;
    STDMETHOD(GetExtension)	    (THIS_ REF(const char*) pExtension) PURE;
    STDMETHOD(SetExtension)	    (THIS_ const char* pExtension, UINT32 ulSize) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXValues
 *
 *  Purpose:
 *
 *  	This is an interface to a generic name-value pair facility.  This
 *	is used in various places (such as stream headers).
 *
 *  IID_IHXValues:
 *
 *	{00001302-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXValues, 0x00001302, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

/*
 *  The IHXCommonClassFactory supports creating an instance
 *  of this object.
 */
#define CLSID_IHXValues IID_IHXValues

#undef  INTERFACE
#define INTERFACE   IHXValues

DECLARE_INTERFACE_(IHXValues, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXValues methods
     */

    /*
     * Note: That strings returned as references should be copied or
     * 	     used immediately because their lifetime is only as long as the
     * 	     IHXValues's objects lifetime.
     *
     * Note: Your iterator will be reset once you give up control to the
     *	     RMA core (i.e. you exit whatever function gave you a time slice).
     */

    STDMETHOD(SetPropertyULONG32)	(THIS_
					const char*      pPropertyName,
					ULONG32          uPropertyValue) PURE;

    STDMETHOD(GetPropertyULONG32)	(THIS_
					const char*      pPropertyName,
					REF(ULONG32)     uPropertyName) PURE;

    STDMETHOD(GetFirstPropertyULONG32)	(THIS_
					REF(const char*) pPropertyName,
					REF(ULONG32)     uPropertyValue) PURE;

    STDMETHOD(GetNextPropertyULONG32)	(THIS_
					REF(const char*) pPropertyName,
					REF(ULONG32)     uPropertyValue) PURE;

    STDMETHOD(SetPropertyBuffer)	(THIS_
					const char*      pPropertyName,
					IHXBuffer*      pPropertyValue) PURE;

    STDMETHOD(GetPropertyBuffer)	(THIS_
					const char*      pPropertyName,
					REF(IHXBuffer*) pPropertyValue) PURE;
    
    STDMETHOD(GetFirstPropertyBuffer)	(THIS_
					REF(const char*) pPropertyName,
					REF(IHXBuffer*) pPropertyValue) PURE;

    STDMETHOD(GetNextPropertyBuffer)	(THIS_
					REF(const char*) pPropertyName,
					REF(IHXBuffer*) pPropertyValue) PURE;

    STDMETHOD(SetPropertyCString)	(THIS_
					const char*      pPropertyName,
					IHXBuffer*      pPropertyValue) PURE;

    STDMETHOD(GetPropertyCString)	(THIS_
					const char*      pPropertyName,
					REF(IHXBuffer*) pPropertyValue) PURE;

    STDMETHOD(GetFirstPropertyCString)	(THIS_
					REF(const char*) pPropertyName,
					REF(IHXBuffer*) pPropertyValue) PURE;

    STDMETHOD(GetNextPropertyCString)	(THIS_
					REF(const char*) pPropertyName,
					REF(IHXBuffer*) pPropertyValue) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXValuesRemove
 *
 *  Purpose:
 *
 *      This interface is to add Remove methods to a class that supports 
 *      IHXValues.  All classes that support this interface will also 
 *      support IHXValues.
 *   
 *  
 *
 *  IID_IHXValuesRemove:
 *
 *	{00001303-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXValuesRemove, 0x00001303, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

/*
 *  The IHXCommonClassFactory does not support creating an instance
 *  of this object.
 */

#undef  INTERFACE
#define INTERFACE   IHXValuesRemove

DECLARE_INTERFACE_(IHXValuesRemove, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     * IHXValuesRemove methods
     */

     /************************************************************************
     *	Method:
     *	    IHXKeyValuesRemove::Remove
     *	Purpose:
     *      Remove all items matching pKey.  (If you know what datatype you saved
     *      the key as, use the specific method.)
     */
    STDMETHOD(Remove)	     (const char* pKey) PURE;
    
     /************************************************************************
     *	Method:
     *	    IHXKeyValuesRemove::RemoveULONG32
     *	Purpose:
     *      Remove all ULONG32 items matching pKey. 
     */
    STDMETHOD(RemoveULONG32) (const char* pKey) PURE;
    
     /************************************************************************
     *	Method:
     *	    IHXKeyValuesRemove::RemoveBuffer
     *	Purpose:
     *      Remove all Buffer items matching pKey. 
     */
    STDMETHOD(RemoveBuffer)  (const char* pKey) PURE;
    
     /************************************************************************
     *	Method:
     *	    IHXKeyValuesRemove::RemoveCString
     *	Purpose:
     *      Remove all CString items matching pKey. 
     */
    STDMETHOD(RemoveCString) (const char* pKey) PURE;
};

// $Private:
DEFINE_GUID(IID_IHXClientPacket,   0x00001304, 0x0901, 0x11d1, 0x8b, 0x06, 0x0, 
				    0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXClientPacket

DECLARE_INTERFACE_(IHXClientPacket, IUnknown)
{
    /*
     *	IUnknown methods
     */

    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;
};

DEFINE_GUID(IID_IHXBroadcastDistPktExt, 0x00001305, 0x0901, 0x11d1, 0x8b, 0x06, 0x0, 
				    0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXBroadcastDistPktExt

DECLARE_INTERFACE_(IHXBroadcastDistPktExt, IUnknown)
{
    /*
     *	IUnknown methods
     */

    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    STDMETHOD_(UINT32,GetSeqNo)  (THIS) PURE;
    STDMETHOD_(UINT32,GetStreamSeqNo)  (THIS) PURE;
    STDMETHOD_(BOOL,GetIsLostRelaying)  (THIS) PURE;

    STDMETHOD(SetSeqNo)  (THIS_ UINT32 ulSeqNo) PURE;
    STDMETHOD(SetStreamSeqNo)  (THIS_ UINT32 ulStreamSeqNo) PURE;
    STDMETHOD(SetIsLostRelaying)  (THIS_ BOOL bLostRelay) PURE;
};

// $EndPrivate.

#endif /* _IHXPCKTS_H_ */

