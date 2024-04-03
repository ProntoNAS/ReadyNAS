/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: pckunpck.h,v 1.1.1.1 2006/03/29 16:45:32 hagi Exp $
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

#ifndef PCKUNPCK_H
#define PCKUNPCK_H

/* Unfortunate but necessary includes */
#include "hxtypes.h"
#include "hxwintyp.h"
#include "hxcom.h"
#include "hxstring.h"
#include <stdarg.h> /* for va_arg */

/*
 * forward decls
 */
typedef _INTERFACE IHXBuffer IHXBuffer;
typedef _INTERFACE IHXValues IHXValues;

/*
 * packing/unpacking functions
 * NOTE: this are Nick Hart's original methods imported
 * directly from chinembed/unix/bufferutils.cpp
 */

HX_RESULT PackBuffer(REF(IHXBuffer*) pBuffer,
		     const char*      pFormat,
		     ...);
HX_RESULT PackBufferV(REF(IHXBuffer*) pBuffer,
		      const char*      pFormat,
		      va_list vargs);
int UnpackBuffer(REF(const char*) pBuffer,
		 const char*      pFormat,
		 ...);
int UnpackBufferV(REF(const char*) pBuffer,
		  const char*      pFormat,
		  va_list          vargs);
HX_RESULT PackValues(REF(CHXString) sBuffer,
                     IHXValues*    pValues);
HX_RESULT UnpackValues(REF(const char*) pBuffer,
                       REF(IHXValues*) pValues,
                       BOOL             bCreateValues = TRUE);
HX_RESULT Bufferize(REF(IHXBuffer*) pBuffer,
                    void*            pData,
                    UINT32           uSize);
#ifdef _DEBUG
void TestBufferPacking();
#endif

/*
 * NOTE: these are methods Eric Hyche added specifically
 * for packing and unpacking IHXValues. They also add two
 * additional options:
 * a) binary packing (as opposed to Nick's text string packing); and 
 * b) if you pass in an IUnknown context, then all your IHXBuffer's
 *    and IHXValues will be created by the common class factory.
 *    If you leave out the IUnknown, then IHXBuffer's will be 
 *    directly created from CHXBuffer's and IHXValues will be created
 *    directly from CHXHeader's.
 */
// GetBinaryPackedSize() returns the number of
// bytes required to binary pack the IHXValues
// that you pass in
UINT32 GetBinaryPackedSize(IHXValues* pValues);
// PackValuesBinary() packs the IHXValues you provide
// in the IHXBuffer you provide in binary form. It assumes
// that pBuffer is at least as big as the the
// number of bytes returned by GetBinaryPackedSize(). If
// pBuffer is not big enough, it will return HXR_FAIL.
HX_RESULT PackValuesBinary(IHXBuffer* pBuffer,
                           IHXValues* pValues);
// CreateBuffer() creates an IHXBuffer. If you pass in
// an IUnknown context, it will QI that context for
// IHXCommonClassFactory and use IHXCommonClassFactory::CreateInstance()
// to create the IHXBuffer. If you don't pass in an IUnknown context, then
// it will do a "new CHXBuffer()" to create the IHXBuffer.
HX_RESULT CreateBuffer(REF(IHXBuffer*) rpBuffer,
                       IUnknown*        pContext = NULL);
// CreateValues() creates an IHXValues. If you pass in
// an IUnknown context, it will QI that context for
// IHXCommonClassFactory and use IHXCommonClassFactory::CreateInstance()
// to create the IHXValues. If you don't pass in an IUnknown context, then
// it will do a "new CHXHeader()" to create the IHXValues.
HX_RESULT CreateValues(REF(IHXValues*) rpValues,
                       IUnknown*        pContext = NULL);
// CreateStringBuffer() creates an IHXBuffer with the
// string pszStr in it. The length of rpBuffer will
// be strlen(pszStr) + 1. CreateStringBuffer() calls
// CreateBuffer() to create the IHXBuffer.
HX_RESULT CreateStringBuffer(REF(IHXBuffer*) rpBuffer,
                             const char*      pszStr,
                             IUnknown*        pContext = NULL);
// SetCStringProperty() sets a CString property in
// pValues where pszName is the property name
// and the property value is pszValue. SetCStringProperty()
// calls CreateStringBuffer() to create the IHXBuffer
// which holds the property value string.
HX_RESULT SetCStringProperty(IHXValues* pValues,
                             const char* pszName,
                             const char* pszValue,
                             IUnknown*   pContext = NULL,
                             BOOL        bSetAsBufferProp = FALSE);
// SetCStringPropertyWithNullTerm() sets a CString property in
// pValues where pszName is the property name
// and the property value is in pBuf and ulLen. It's assumed
// that pBuf and ulLen hold a string, but the string is
// not NULL-terminated. Therefore, SetCStringPropertyWithNT()
// will NULL-terminate the buffer in pBuf (making the
// IHXBuffer it creates ulLen + 1).
HX_RESULT SetCStringPropertyWithNullTerm(IHXValues*  pValues,
                                         const char* pszName,
                                         BYTE*       pBuf,
                                         UINT32      ulLen,
                                         IUnknown*   pContext = NULL,
                                         BOOL        bSetAsBufferProp = FALSE);
// CreateNullTermBuffer takes pBuf and ulLen, creates a
// buffer of length ulLen+1, copies ulLen bytes
// of pBuf into this new buffer, and then NULL-terminates
// it. It then returns this buffer as an out parameter.
HX_RESULT CreateNullTermBuffer(BYTE*  pBuf,
                               UINT32 ulLen,
                               char** ppNTBuf);
// SetBufferPropety() sets a Buffer property in pValues.
// First calls CreateBuffer() (see above) to create the
// IHXBuffer, then calls IHXBuffer::Set() with the
// provided pBuf and ulLen, and then sets this
// property into the IHXValues.
HX_RESULT SetBufferProperty(IHXValues* pValues,
                            const char* pszName,
                            BYTE*       pBuf,
                            UINT32      ulLen,
                            IUnknown*   pContext = NULL);
// UnpackPropertyULONG32() unpacks a ULONG32 property
// from binary form, and then sets that ULONG32 property
// into the provided IHXValues. Initially rpBuf should
// be set to the beginning of the packed ULONG32 and 
// pLimit should be set to the end of the entire packing
// buffer (this provides protection against corrupt data
// buffer overruns). Upon return, rpBuf will be set to
// the first byte past the end of the packed ULONG32.
// UnpackPropertyULONG32() is called by UnpackValuesBinary().
HX_RESULT UnpackPropertyULONG32(IHXValues* pValues,
                                REF(BYTE*)  rpBuf,
                                BYTE*       pLimit,
                                IUnknown*   pContext = NULL);
// UnpackPropertyCString() unpacks a CString property
// from binary form, and then sets that CString property
// into the provided IHXValues. Initially rpBuf should
// be set to the beginning of the packed CString and 
// pLimit should be set to the end of the entire packing
// buffer (this provides protection against corrupt data
// buffer overruns). Upon return, rpBuf will be set to
// the first byte past the end of the packed CString.
// UnpackPropertyCString() is called by UnpackValuesBinary().
HX_RESULT UnpackPropertyCString(IHXValues* pValues,
                                REF(BYTE*)  rpBuf,
                                BYTE*       pLimit,
                                IUnknown*   pContext = NULL);
// UnpackPropertyBuffer() unpacks a Buffer property
// from binary form, and then sets that Buffer property
// into the provided IHXValues. Initially rpBuf should
// be set to the beginning of the packed CString and 
// pLimit should be set to the end of the entire packing
// buffer (this provides protection against corrupt data
// buffer overruns). Upon return, rpBuf will be set to
// the first byte past the end of the packed Buffer.
// UnpackPropertyBuffer() is called by UnpackValuesBinary().
HX_RESULT UnpackPropertyBuffer(IHXValues* pValues,
                               REF(BYTE*)  rpBuf,
                               BYTE*       pLimit,
                               IUnknown*   pContext = NULL);
// UnpackValuesBinary() unpacks a binary-packed pBuffer into
// the provided pValues. If pBuffer is not binary-packed, then
// it will return HXR_FAIL. If you pass in an IUnknown context,
// then it will use the common class factory to create the
// necessary IHXBuffer's. Otherwise, it will use "new CHXBuffer()".
HX_RESULT UnpackValuesBinary(IHXValues* pValues,
                             IHXBuffer* pBuffer,
                             IUnknown*   pContext = NULL);
// UnpackValuesBinary() unpacks a binary-packed pBuffer into
// the provided pValues. If pBuffer is not binary-packed, then
// it will return HXR_FAIL. If you pass in an IUnknown context,
// then it will use the common class factory to create the
// necessary IHXBuffer's. Otherwise, it will use "new CHXBuffer()".
HX_RESULT UnpackValuesBinary(IHXValues* pValues,
                             BYTE*       pBuf,
                             UINT32      ulLen,
                             IUnknown*   pContext = NULL);
// PackValues() packs pValues into an IHXBuffer and returns
// the packed buffer into rpBuffer. If bPackBinary is TRUE,
// it will pack the IHXValues as binary; otherwise, it
// will pack the IHXValues as text. If you pass in an IUnknown context,
// then it will use the common class factory to create the
// necessary IHXBuffer's. Otherwise, it will use "new CHXBuffer()".
HX_RESULT PackValues(REF(IHXBuffer*) rpBuffer,
                     IHXValues*      pValues,
                     BOOL             bPackBinary = FALSE,
                     IUnknown*        pContext = NULL);
// UnpackValues() unpacks an IHXBuffer into rpValues. It 
// automatically detects whether pBuffer is binary-packed
// or text-packed (as long as PackValues() was used to pack
// the IHXValues).
HX_RESULT UnpackValues(REF(IHXValues*) rpValues,
                       IHXBuffer*      pBuffer,
                       IUnknown*        pContext = NULL);
// UnpackValues() unpacks an IHXBuffer into rpValues. It 
// automatically detects whether pBuffer is binary-packed
// or text-packed (as long as PackValues() was used to pack
// the IHXValues).
HX_RESULT UnpackValues(REF(IHXValues*) rpValues,
                       BYTE*            pBuf,
                       UINT32           ulLen,
                       IUnknown*        pContext = NULL);
// AreValuesInclusiveIdentical() checks whether all the
// properties in pValues1 are:
// a) in pValues2; AND
// b) identical to the corresponding properties in pValues2.
BOOL AreValuesInclusiveIdentical(IHXValues* pValues1,
                                 IHXValues* pValues2);
// AreValuesIdentical() returns
// AreValuesInclusiveIdentical(pValues1, pValues2) &&
// AreValuesInclusiveIdentical(pValues1, pValues2).
// By checking inclusive identity in both directions, this
// absolutely establishes identity between the contents
// of pValues1 and pValues2.
BOOL AreValuesIdentical(IHXValues* pValues1,
                        IHXValues* pValues2);
#ifdef _DEBUG
// This tests text and binary packing of IHXValues.
// It is also a good place to look for examples of how
// to call PackValues() and UnpackValues().
HX_RESULT TestValuesPacking(IUnknown* pContext = NULL);
#endif

#endif
