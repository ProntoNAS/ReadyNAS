/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: pckunpck.cpp,v 1.1.1.1 2006/03/29 16:45:31 hagi Exp $
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

// system
#include "hlxclib/stdarg.h" /* for va_arg */
#include "safestring.h"
#ifndef _SYMBIAN
#include "hlxclib/memory.h"
#endif
#include "hlxclib/string.h"
#include "hlxclib/stdlib.h"
#include "hlxclib/time.h"
// include
#include "hxtypes.h"
#include "hxwintyp.h"
#include "hxcom.h"
#include "hxcomm.h"
// pncont
#include "hxbuffer.h"
#include "chxpckts.h"
#include "rtsputil.h" /* for Base64 utils */
#include "hxslist.h"
#include "pckunpck.h"

/*
 * constants
 */

// ascii string, buffer, pointer, uint, double, flag (BOOL), IHXValues
const char* const kFormatSpecs = "abpudfv";

// these types have implicit "parsability" so we omit the "x:" type specifier
// a string starts with '"', flag is a 'T' or 'F', uint is all decimal digits,
// and an IHXValues begins with '['
const char* const kOptTypes = "afuv";

// chars we need to escape/unescape (note they have 1:1 positioning)
const char* const kScaryChars = "\"\n\t\r\\";
const char* const kUnScaryChars = "\"ntr\\"; // these must correspond 1:1

// decimals
const char* const kDecimals = "1234567890";


/*
 * PackBuffer
 *
 * USAGE:
 *
 * IHXBuffer* pBuffer;
 * PackBuffer(pBuffer, "uu", 10, 20);
 * PackBuffer(pBuffer, "a", "foobar");
 * PackBuffer(pBuffer, "aua", 10, "foobar", 20);
 *
 * More advanced: (passing a buffer)
 * IHXBuffer* pTheBuffer;
 * PackBuffer(pBuffer, "uabu", 10, "foobar", pTheBuffer, 20);
 *
 * Still more advanced: (passing an IUnknown)
 * IHXSomeObject* pObj;
 * PackBuffer(pBuffer, "uap", 10, "foobar", (IUnknown*)pObj);
 */

HX_RESULT
PackBuffer(REF(IHXBuffer*) pBuffer, const char* pFormat, ...)
{
    va_list vargs;
    va_start(vargs, pFormat);
    return PackBufferV(pBuffer, pFormat, vargs);
}


HX_RESULT
PackBufferV(REF(IHXBuffer*) pBuffer, const char* pFormat, va_list vargs)
{
    // init the out param
    pBuffer = NULL;

    // automatically fail if we've no CCF or a bad formatter is passed
    if (!pFormat || !*pFormat || !strpbrk(pFormat, kFormatSpecs))
    {
	return HXR_FAIL;
    }

    // calculate how much space we'll need for the buffer and
    // make sure the formatter is valid
    UINT32 uBufSize = 1; // null terminator
    CHXStringList packedValuesList;
    va_list vargsOrig;
    va_copy(vargsOrig, vargs);	
    const char* pTemp = pFormat;
    while (*pTemp)
    {
	switch (*pTemp)
	{
	    case 'f':
	    {
		va_arg(vargs, BOOL);
		// XXXNH: optimization... we only put one char for a flag/bool
		uBufSize += 1;
	    }
	    break;

	    case 'd':
	    {
		va_arg(vargs, double);
		
		// to preserve precision we base64-ize the double
		uBufSize += (sizeof(double) * 4) / 3 + 10;
	    }
	    break;
	    
	    case 'u':
	    {
		UINT32 u = va_arg(vargs, UINT32);
                // Compute number of hex digits in 'u' & add to uBufSize
                if (u == 0) ++uBufSize;
                else {
                    UINT32 leadingZeroes = 0;
                    if (u <= 0x0000ffff) {leadingZeroes += 4; u <<= 16;}
                    if (u <= 0x00ffffff) {leadingZeroes += 2; u <<= 8;}
                    if (u <= 0x0fffffff) {leadingZeroes += 1; u <<= 4;}
                    uBufSize += 8 - leadingZeroes;
                }
	    }
	    break; 
	    
	    case 'a':
	    {
		const char* pStr = va_arg(vargs, const char*);
		
		int nLen = pStr ? strlen(pStr) + 2 : 2; // add on quotation marks
		while (pStr && *pStr)
		{
		    // double-count chars we'll have to escape
		    if (strchr(kScaryChars, *pStr))
			nLen++;
		    
		    pStr ++;
		}
		uBufSize += nLen;
	    }
	    break;

	    case 'b':
	    {
		IHXBuffer* pBuf = va_arg(vargs, IHXBuffer*);
		if (!pBuf)
		{
		    HX_ASSERT(FALSE);
		    return HXR_FAIL;
		}
		
		// base64 uses roughly 33% more space, plus a couple extra
		uBufSize += (pBuf->GetSize() * 4) / 3 + 10;
	    }
	    break;

	    case 'p':
	    {
		va_arg(vargs, IUnknown*);
		uBufSize += 8; // 8 digits for a hex number
	    }
	    break;

	    case 'v':
	    {
		IHXValues* pValues = va_arg(vargs, IHXValues*);
		if (!pValues)
		{
		    HX_ASSERT(FALSE);
		    return HXR_FAIL;
		}
		
		CHXString sTemp;
		if (FAILED(PackValues(sTemp, pValues)))
		{
		    HX_ASSERT(FALSE);
		    return HXR_FAIL;
		}

		packedValuesList.AddTailString(sTemp);
		uBufSize += sTemp.GetLength();
	    }
	    break;
	}

	if (strchr(kOptTypes, *pTemp))
	    uBufSize++; // just the delimiter for "optimized" types
	else
	    uBufSize += 3; // the size of type specifier and delimiter

	// next!
	pTemp++;
    }

    // now allocate our buffer
    CHXBuffer* pCHXBuffer = new CHXBuffer;
    if (!pCHXBuffer)
	return HXR_OUTOFMEMORY;
    pCHXBuffer->AddRef();
    if (SUCCEEDED(pCHXBuffer->SetSize(uBufSize)))
    {
	pBuffer = (IHXBuffer*)pCHXBuffer;
    }
    else
    {
	HX_RELEASE(pCHXBuffer);
	return HXR_OUTOFMEMORY;
    }

    // now pack the args into our buffer
    char* pBufStr = (char*)pBuffer->GetBuffer();
    
    vargs = vargsOrig;
    pTemp = pFormat;
    while (*pTemp)
    {
	// XXXNH: space optimization: omit the type & ':' for certain types
	if (strchr(kOptTypes, *pTemp) == NULL)
	{
	    // start the param
	    *pBufStr = *pTemp; // append the type
	    pBufStr++;
	    *pBufStr = ':'; // and a delimiter
	    pBufStr++;
	}

	// then format the data
	switch (*pTemp)
	{
	    case 'f':
	    {
		BOOL bFlag = va_arg(vargs, BOOL);
		*pBufStr = bFlag ? 'T' : 'F';
		pBufStr++;
	    }
	    break;

	    case 'd':
	    {
		double dVal = va_arg(vargs, double);

		//UINT32 uSize =(sizeof(double) * 4) / 3 + 10;

		int nLen = BinTo64((UCHAR*)&dVal, sizeof(double), pBufStr);
		HX_ASSERT(nLen >= 0);
		pBufStr += nLen-1; // -1 because of null terminator
	    }
	    break;
	    
	    case 'u':
	    {
		UINT32 u = va_arg(vargs, UINT32);
		char pNum[16]; /* Flawfinder: ignore */
		SafeSprintf(pNum, sizeof(pNum), "%x", u);
		*pBufStr = '\0';
		strcat(pBufStr, pNum); /* Flawfinder: ignore */
		pBufStr += strlen(pNum);
	    }
	    break; 
	    
	    case 'a':
	    {
		*pBufStr = '"'; // begin quote
		pBufStr++;
		
		const char* pStr = va_arg(vargs, const char*);
		
		while (pStr && *pStr)
		{
		    // double-count chars we'll have to escape
		    const char* pScary = strchr(kScaryChars, *pStr);
		    if (pScary)
		    {
			*pBufStr = '\\';
			pBufStr++;
			*pBufStr = kUnScaryChars[pScary - kScaryChars];
			pBufStr++;
		    }
		    else
		    {
			*pBufStr = *pStr;
			pBufStr++;
		    }
		    pStr++;
		}
		
		*pBufStr = '"'; // end quote
		pBufStr++;
	    }
	    break;

	    case 'b':
	    {
		IHXBuffer* pBuf = va_arg(vargs, IHXBuffer*);
		if (!pBuf)
		{
		    HX_ASSERT(FALSE);
		    return HXR_FAIL;
		}
		
		// base64 uses roughly 33% more space, plus a couple extra
		UINT32 uSize = pBuf->GetSize();
		int nLen = BinTo64(pBuf->GetBuffer(), uSize, pBufStr);
		HX_ASSERT(nLen >= 0);
		pBufStr += nLen-1; // -1 because of null terminator
	    }
	    break;

	    case 'p':
	    {
		IUnknown* pUnknown = va_arg(vargs, IUnknown*);
		char pPointer[9]; /* Flawfinder: ignore */
		SafeSprintf(pPointer, sizeof(pPointer), "%08x", pUnknown);
		*pBufStr = '\0';
		strcat(pBufStr, pPointer); /* Flawfinder: ignore */
		pBufStr += 8;
	    }
	    break;

	    case 'v':
	    {
		va_arg(vargs, IHXValues*);
		LISTPOSITION p = packedValuesList.GetHeadPosition();
		CHXString* psPacked = packedValuesList.GetNext(p);

		*pBufStr = '\0';
		strcat(pBufStr, *psPacked); /* Flawfinder: ignore */
		pBufStr += psPacked->GetLength();
		
		packedValuesList.RemoveHeadString();
	    }
	    break;
	}

	// end the param
	*pBufStr = ';';
	pBufStr++;

	// go to the next formatter
	pTemp++;
    }

    // null terminate!
    *pBufStr = '\0';
    pBufStr++;
    
#ifdef _DEBUG
    // XXXNH: some sanity checking
    pBufStr -= uBufSize;
    if (pBufStr > (char*)(pBuffer->GetBuffer()))
    {
	// XXXNH: we wrote past the end of the buffer!!!!!  We must have
	// miscalculated the space needed...
	HX_ASSERT(FALSE);
	HX_RELEASE(pBuffer);
	return HXR_FAIL;
    }
#endif
    
    return HXR_OK;
}


/*
 * UnpackBuffer
 *
 * works like PackBuffer & sscanf.
 */

int
UnpackBuffer(REF(const char*) pBuffer, const char* pFormat, ...)
{
    va_list vargs;
    va_start(vargs, pFormat);
    return UnpackBufferV(pBuffer, pFormat, vargs);
}

  
int
UnpackBufferV(REF(const char*) pBufStr, const char* pFormat, va_list vargs)
{
    // automatically fail if we've no CCF or a bad formatter is passed
    if (!pFormat || !*pFormat || !strpbrk(pFormat, kFormatSpecs))
	return -1;

    if (!pBufStr)
	return 0;

    // iterate through the args and unpack them from the buffer
    int nRead = 0;
    const char* pTemp = pFormat;
    while (*pTemp)
    {
	// XXXNH: space optimization: omit the type & ':' for certain types
	if (!strchr(kOptTypes, *pTemp))
	{
	    // make sure the types match
	    if (*pTemp != *pBufStr)
		return nRead;
	    pBufStr++;

	    // read the type/value delimiter
	    if (*pBufStr != ':')
		return nRead;
	    pBufStr++;
	}
	
	switch (*pTemp)
	{
	    case 'f':
	    {
		// XXXNH: optimization... we only put one char for a flag/bool
		BOOL* pbFlag = va_arg(vargs, BOOL*);
		if (*pBufStr == 'T')
		    *pbFlag = TRUE;
		else
		    *pbFlag = FALSE;
		pBufStr++;
	    }
	    break;

	    case 'd':
	    {
		double* pdVal = va_arg(vargs, double*);

		const char* pEnd = strchr(pBufStr, ';');
		if (pEnd)
		{
		    int nSize = pEnd - pBufStr;
                    int nLen = 0;						
		    nLen = BinFrom64(pBufStr, nSize, (UCHAR*)pdVal);
		    HX_ASSERT(nLen == sizeof(double));
		    pBufStr = pEnd;
		}
		else
		    return nRead; // couldn't parse the end of the buffer
	    }
	    break;

	    case 'u':
	    {	va_arg(vargs, UINT32*);

                // XXXSAB Untested...
                char* pEnd = NULL;
                strtoul(pBufStr, &pEnd, 16);

				// if (sscanf(pBufStr, "%x", puInt) == 1)
                if (pEnd && pEnd > pBufStr)
		{
		    // advance to the next delimiter
		    pBufStr = strchr(pBufStr, ';');
		}
		else
		    return nRead; // couldn't parse an int!
	    }
	    break;
	    
	    case 'a':
	    {
		CHXString* pTemp = va_arg(vargs, CHXString*);
		HX_ASSERT(pTemp);
		if (!pTemp)
		    return nRead; // invalid CHXString*?
		pTemp->Empty(); 

		// read the begin quote
		if (*pBufStr != '"')
		    return nRead;
		pBufStr++;

		// read till we run out of buffer OR hit the end quote
		BOOL bEscaped = FALSE;
		while (*pBufStr && (bEscaped || *pBufStr != '"'))
		{
		    if (bEscaped)
		    {
			const char* pScary = strchr(kUnScaryChars, *pBufStr);
			HX_ASSERT(pScary);
			if (pScary)
			{
			    *pTemp += kScaryChars[pScary - kUnScaryChars];
			}
		    }
		    else
		    {
			if (*pBufStr == '\\')
			{
			    bEscaped = TRUE;
			}
			else
			{
			    *pTemp += *pBufStr;
			}
		    }
		    pBufStr++;
		}

		// read the end quote!
		if (*pBufStr != '"')
		    return nRead;
		pBufStr++;
	    }
	    break;

	    case 'b':
	    {
		IHXBuffer** ppBuf = va_arg(vargs, IHXBuffer**);
		HX_ASSERT(ppBuf);
		if (!ppBuf)
		    return nRead;

		CHXBuffer* pCHXBuffer = new CHXBuffer;
		if (!pCHXBuffer)
		    return nRead;
		
		pCHXBuffer->AddRef();
		*ppBuf = (IHXBuffer*)pCHXBuffer;

		HX_RESULT res = HXR_FAIL;
		const char* pEnd = strchr(pBufStr, ';');
		if (pEnd)
		{
		    UINT32 uSize = pEnd - pBufStr;
		    CHXString sTemp(pBufStr, uSize);
		    uSize = (uSize * 3) / 4; // we need 75% the space
		    
		    res = (*ppBuf)->SetSize(uSize);
		    if (SUCCEEDED(res))
		    {
			
			UCHAR* pDecodeBuf = (*ppBuf)->GetBuffer();
			HX_ASSERT(pDecodeBuf);
			
			UINT32 nLen = BinFrom64(sTemp,
                                                sTemp.GetLength()+1,
                                                pDecodeBuf);
			HX_ASSERT(nLen <= uSize);
			(*ppBuf)->SetSize(nLen);
			
				// go the the end of the buffer
			pBufStr = pEnd;
		    }
		}
		
		if (FAILED(res))
		{
		    HX_RELEASE(*ppBuf);
		    return nRead;
		}
	    }
	    break;

	    case 'p':
	    {
		IUnknown** ppUnk = va_arg(vargs, IUnknown**);
		HX_ASSERT(ppUnk);
		if (!ppUnk)
		    return nRead;

                // XXXSAB untested...
                char* pEnd = NULL;
                char tmpBuf[9];
                SafeStrCpy(tmpBuf, pBufStr, 8);
                tmpBuf[9] = 0;
                strtoul(tmpBuf, &pEnd, 16);

		// if (sscanf(pBufStr, "%08x", ppUnk) != 1)
                if (pEnd && pEnd > tmpBuf)
		{
		    *ppUnk = NULL;
		    return nRead;
		}
		pBufStr = strchr(pBufStr, ';');
	    }
	    break;

	    case 'v':
	    {
		IHXValues** ppValues = va_arg(vargs, IHXValues**);
		if (!ppValues)
		{
		    HX_ASSERT(FALSE);
		    return nRead;
		}

		if (FAILED(UnpackValues(pBufStr, *ppValues)))
		{
		    HX_RELEASE(*ppValues);
		    return nRead;
		}
	    }
	    break;
	}

	// mark another param read
	nRead++;

	// find the next delimiter
	if (!pBufStr || *pBufStr != ';')
	    return nRead;
	pBufStr++;
	
	pTemp++;
    }
    
    return nRead;
}


HX_RESULT
PackValues(REF(CHXString) sBuffer, IHXValues* pValues)
{
    if (!pValues)
	return HXR_FAIL;
    
    sBuffer = '[';
    
    ULONG32 uVal = 0;
    const char* pName = NULL;
    HX_RESULT res = pValues->GetFirstPropertyULONG32(pName, uVal);
    while (SUCCEEDED(res))
    {
	sBuffer += pName;
	sBuffer += '=';
	sBuffer.AppendULONG(uVal);
	sBuffer += ',';
	
	res = pValues->GetNextPropertyULONG32(pName, uVal);
    }

    IHXBuffer* pValBuf = NULL;
    res = pValues->GetFirstPropertyCString(pName, pValBuf);
    while (SUCCEEDED(res))
    {
	sBuffer += pName;
	sBuffer += "=\"";

	const char* pStr = (const char*)pValBuf->GetBuffer();
	while (*pStr)
	{
	    // double-count chars we'll have to escape
	    const char* pScary = strchr(kScaryChars, *pStr);
	    if (pScary)
	    {
		sBuffer += '\\';
		sBuffer += kUnScaryChars[pScary - kScaryChars];
	    }
	    else
	    {
		sBuffer += *pStr;
	    }
	    pStr++;
	}
	sBuffer += "\",";

	HX_RELEASE(pValBuf);
	res = pValues->GetNextPropertyCString(pName, pValBuf);
    }

    res = pValues->GetFirstPropertyBuffer(pName, pValBuf);
    while (SUCCEEDED(res))
    {
	sBuffer += pName;
	sBuffer += '=';

	CHXString sTemp;
	UINT32 uSize = (pValBuf->GetSize() * 4) / 3 + 10;
	char* pTemp = (char*)sTemp.GetBuffer(uSize);
	if (pTemp)
	{
	    int	nLen = 0;												
            nLen = BinTo64(pValBuf->GetBuffer(), pValBuf->GetSize(), pTemp);
	    sTemp.ReleaseBuffer();
	    HX_ASSERT(nLen >= 0);			// It's used
	}
	
	HX_RELEASE(pValBuf);
	
	sBuffer += sTemp;
	sBuffer += ',';

	res = pValues->GetNextPropertyBuffer(pName, pValBuf);
    }

    UINT32 uSize = sBuffer.GetLength();
    sBuffer.SetAt(uSize-1, ']');

    return HXR_OK;
}



HX_RESULT
Bufferize(REF(IHXBuffer*) pBuffer, void* pData, UINT32 uSize)
{
    CHXBuffer* pCHXBuffer = new CHXBuffer;
    if (!pCHXBuffer)
	return HXR_OUTOFMEMORY;

    pCHXBuffer->AddRef();
    HX_RESULT res = pCHXBuffer->Set((UCHAR*)pData, uSize);
    if (SUCCEEDED(res))
    {
	pBuffer = (IHXBuffer*)pCHXBuffer;
    }
    else
    {
	HX_RELEASE(pCHXBuffer);
    }
    
    return res;
}


HX_RESULT
UnpackValues(REF(const char*) pBuffer, REF(IHXValues*) pValues,
             BOOL bCreateValues)
{
    HX_ASSERT(pBuffer);
    if (!pBuffer)
	return HXR_POINTER;
    
    if (*pBuffer != '[')
	return HXR_FAIL;

    if (bCreateValues)
    {
        CHXHeader* pCHXHeader = new CHXHeader;
        if (!pCHXHeader)
	    return HXR_OUTOFMEMORY;
        pCHXHeader->AddRef();
        pValues = (IHXValues*)pCHXHeader;
    }
    else
    {
        // The input said to NOT create the IHXValues,
        // but use the passed-in one, so if we don't
        // *have* a passed-in one, then that's an error.
        if (!pValues)
        {
            return HXR_FAIL;
        }
    }

    // eat the '['
    pBuffer++;

    HX_RESULT res = HXR_FAIL;
    while (*pBuffer)
    {
	res = HXR_FAIL;
	
	// find the end of the property name
	const char* pEnd = strchr(pBuffer, '=');
	HX_ASSERT(pEnd);
	if (!pEnd)
	    break;

	// parse the name
	CHXString sName(pBuffer, pEnd - pBuffer);
	pBuffer = pEnd+1;

	if (*pBuffer == '"')
	{
	    pBuffer++;
	    // looks like a string
	    CHXString sValue;
	    
	    BOOL bEscaped = FALSE;
	    while (*pBuffer != '"' && !bEscaped && *pBuffer)
	    {
		if (bEscaped)
		{
		    const char* pScary = strchr(kUnScaryChars, *pBuffer);
		    HX_ASSERT(pScary);
		    if (pScary)
		    {
			sValue += kScaryChars[pScary - kUnScaryChars];
		    }
		}
		else
		{
		    if (*pBuffer == '\\')
		    {
			bEscaped = TRUE;
		    }
		    else
		    {
			sValue += *pBuffer;
		    }
		}
		pBuffer++;
	    }
	    
	    // read the end quote!
	    if (*pBuffer == '"')
	    {
		pBuffer++;
		
		IHXBuffer* pStrBuf = NULL;
		res = Bufferize(pStrBuf,
				(void*)(const char*)sValue,
				sValue.GetLength()+1);
		if (SUCCEEDED(res))
		{
		    res = pValues->SetPropertyCString(sName, pStrBuf);
		    HX_RELEASE(pStrBuf);
		}
	    }
	}
	else
	{
	    // is it an int or base64 buffer?

	    // how many digits do we have?
	    size_t sz = strspn(pBuffer, kDecimals);
	    
	    // where does the next property begin?
	    pEnd = strpbrk(pBuffer, ",]");
	    HX_ASSERT(pEnd);
	    if (!pEnd)
		break;

	    // so were there *only* decimal digits?
	    if (pBuffer + sz == pEnd)
	    {
		CHXString sNumber(pBuffer, sz);
                ULONG32 uValue = strtoul((const char*) sNumber, NULL, 10);

		res = pValues->SetPropertyULONG32(sName, uValue);
	    }
	    else
	    {
		// nope-- looks like a base64 buffer
		UINT32 uSize = pEnd - pBuffer;
		UINT32 uBinSize = (uSize * 3) / 4 + 10;

		res = HXR_OUTOFMEMORY;
		CHXBuffer* pTempBuf = new CHXBuffer;
		if (!pTempBuf)
		    break;
		pTempBuf->AddRef();
		res = pTempBuf->SetSize(uBinSize);
		if (SUCCEEDED(res))
		{
		    UCHAR* pTemp = pTempBuf->GetBuffer();
		    UINT32 nLen = BinFrom64(pBuffer, uSize, pTemp);
		    HX_ASSERT(nLen <= uBinSize && nLen > 0);
		    pTempBuf->SetSize(nLen);
		    res = pValues->SetPropertyBuffer(sName, pTempBuf);
		}
		HX_RELEASE(pTempBuf);
	    }

	    // advance to the end
	    pBuffer = pEnd;
	}

	// did we fail during the property parsing?
	if (FAILED(res))
	    break;
	
	// are we done?
	if (*pBuffer == ']')
	{
	    pBuffer++;
	    break;
	}

	// ready for next?
	if (*pBuffer != ',')
	{
	    res = HXR_FAIL;
	    break;
	}
	pBuffer++;
    }

    if (FAILED(res))
	HX_RELEASE(pValues);

    return res;
}

#ifdef _DEBUG

void
TestBufferPacking()
{
    IHXBuffer* pParams = NULL;
    HX_RESULT res = PackBuffer(pParams, "uuuuuu", 0, 1, 2, 3, 4, 5);
    HX_ASSERT(SUCCEEDED(res));
    if (SUCCEEDED(res))
    {
	int nums[6];
	const char* pTemp = (const char*)pParams->GetBuffer();
	int nCount = UnpackBuffer(pTemp, "uuuuuu", nums, nums+1, nums+2, nums+3, nums+4, nums+5);
	HX_ASSERT(nCount == 6);
	HX_ASSERT(*pTemp == '\0');

	for (int i = 0; i < 6; i++)
	{
	    HX_ASSERT(nums[i] == i);
	}

	HX_RELEASE(pParams);
    }

    res = PackBuffer(pParams, "uauau", 10, "foo", 20, "bar", 50);
    HX_ASSERT(SUCCEEDED(res));
    if (SUCCEEDED(res))
    {
	CHXString sArg2, sArg4;
	UINT32 nArg1, nArg3, nArg5;
	const char* pTemp = (const char*)pParams->GetBuffer();
	int nCount = UnpackBuffer(pTemp, "uauau", &nArg1, &sArg2, &nArg3, &sArg4, &nArg5);
	HX_ASSERT(nCount == 5);
	HX_ASSERT(*pTemp == '\0');

	HX_ASSERT(nArg1 == 10 && nArg3 == 20 && nArg5 == 50);
	HX_ASSERT(sArg2 == "foo" && sArg4 == "bar");

	HX_RELEASE(pParams);
    }

    res = PackBuffer(pParams, "fuda", TRUE, 666, 12.3456789, "foobar");
    HX_ASSERT(SUCCEEDED(res));
    if (SUCCEEDED(res))
    {
	BOOL bArg1;
	UINT32 nArg2;
	double dArg3;
	CHXString sArg4;
	
	const char* pTemp = (const char*)pParams->GetBuffer();
	int nCount = UnpackBuffer(pTemp, "fuda", &bArg1, &nArg2, &dArg3, &sArg4);
	HX_ASSERT(nCount == 4);
	HX_ASSERT(*pTemp == '\0');

	HX_ASSERT(bArg1 == TRUE && nArg2 == 666 && dArg3 == 12.3456789);
	HX_ASSERT(sArg4 == "foobar");
	
	HX_RELEASE(pParams);
    }

    CHXBuffer* pTempBuf = new CHXBuffer;
    HX_ASSERT(pTempBuf);
    if (pTempBuf)
    {
	HX_ADDREF(pTempBuf);
	res = pTempBuf->SetSize(4096);
	HX_ASSERT(SUCCEEDED(res));
	if (SUCCEEDED(res))
	{
	    // fill up the buffer
	    UCHAR* pData = pTempBuf->GetBuffer();
	    int i;
	    for (i = 0; i < 4096; i++)
	    {
		pData[i] = i & 0xff;
	    }
	    
	    res = PackBuffer(pParams, "ufbau", 69, FALSE, pTempBuf, "foobar", 666);
	    HX_ASSERT(SUCCEEDED(res));
	    if (SUCCEEDED(res))
	    {
		UINT32 nArg1, nArg5;
		BOOL bArg2;
		IHXBuffer* pArg3;
		CHXString sArg4;
		
		const char* pTemp = (const char*)pParams->GetBuffer();
		int nCount = UnpackBuffer(pTemp, "ufbau", &nArg1, &bArg2, &pArg3, &sArg4, &nArg5);
		HX_ASSERT(nCount == 5);
		HX_ASSERT(*pTemp == '\0');
		
		HX_ASSERT(nArg1 == 69 && bArg2 == FALSE && nArg5 == 666);
		HX_ASSERT(sArg4 == "foobar");
		HX_ASSERT(pArg3 && pArg3->GetSize() == 4096);
		if (pArg3 && pArg3->GetSize() == 4096)
		{
		    pData = pArg3->GetBuffer();
		    for (i = 0; i < 4096; i++)
		    {
			HX_ASSERT(pData[i] == (i & 0xff));
		    }
		}
		HX_RELEASE(pArg3);
	    }
	}
	
	HX_RELEASE(pTempBuf);
    }
}
#endif

UINT32 GetBinaryPackedSize(IHXValues* pValues)
{
    UINT32 ulRet = 0;

    if (pValues)
    {
        // Run through the ULONG32 properties
        const char* pszName = NULL;
        UINT32      ulValue = 0;
        HX_RESULT rv = pValues->GetFirstPropertyULONG32(pszName, ulValue);
        while (SUCCEEDED(rv))
        {
            // Add size of name/value type character: 'u'
            ulRet += 1;
            // Add size of name string plus NULL terminator
            ulRet += (UINT32) strlen(pszName) + 1;
            // Add 4 bytes for packing ULONG32
            ulRet += 4;
            // Get next ULONG32 prop
            rv = pValues->GetNextPropertyULONG32(pszName, ulValue);
        }
        // Run through the CString properties
        IHXBuffer* pValue = NULL;
        rv = pValues->GetFirstPropertyCString(pszName, pValue);
        while (SUCCEEDED(rv))
        {
            // Add size of name/value type character: 'c'
            ulRet += 1;
            // Add size of name string plus NULL terminator
            ulRet += (UINT32) strlen(pszName) + 1;
            // Add size of value string plus NULL terminator
            ulRet += strlen((const char*) pValue->GetBuffer()) + 1;
            // Get next CString prop
            HX_RELEASE(pValue);
            rv = pValues->GetNextPropertyCString(pszName, pValue);
        }
        // Run through the Buffer properties
        rv = pValues->GetFirstPropertyBuffer(pszName, pValue);
        while (SUCCEEDED(rv))
        {
            // Add size of name/value type character: 'b'
            ulRet += 1;
            // Add size of name string plus NULL terminator
            ulRet += (UINT32) strlen(pszName) + 1;
            // Add size of value buffer size (4 bytes)
            ulRet += 4;
            // Add size of value buffer
            ulRet += pValue->GetSize();
            // Get next Buffer prop
            HX_RELEASE(pValue);
            rv = pValues->GetNextPropertyBuffer(pszName, pValue);
        }
    }

    return ulRet;
}

HX_RESULT PackValuesBinary(IHXBuffer* pBuffer,
                           IHXValues* pValues)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && pBuffer)
    {
        // Make sure the buffer is big enough
        UINT32 ulMinSize = GetBinaryPackedSize(pValues);
        // Is our buffer big enough?
        if (ulMinSize && pBuffer->GetSize() >= ulMinSize)
        {
            // Ok, pack that puppy
            BYTE* pBuf = pBuffer->GetBuffer();
            if (pBuf)
            {
                // Clear the return value
                retVal = HXR_OK;
                // Run through the ULONG32 properties
                const char* pszName = NULL;
                UINT32      ulValue = 0;
                HX_RESULT rv = pValues->GetFirstPropertyULONG32(pszName, ulValue);
                while (SUCCEEDED(rv))
                {
                    // Copy name/value type character: 'u'
                    *pBuf++ = (BYTE) 'u';
                    // Copy name string plus NULL terminator
                    UINT32 ulBytes = (UINT32) strlen(pszName) + 1;
                    memcpy(pBuf, pszName, ulBytes);
                    pBuf += ulBytes;
                    // Copy ULONG32 in big-endian format (high byte first)
                    *pBuf++ = (BYTE) ((ulValue >> 24) & 0x000000FF);
                    *pBuf++ = (BYTE) ((ulValue >> 16) & 0x000000FF);
                    *pBuf++ = (BYTE) ((ulValue >>  8) & 0x000000FF);
                    *pBuf++ = (BYTE) ( ulValue        & 0x000000FF);
                    // Get next ULONG32 prop
                    rv = pValues->GetNextPropertyULONG32(pszName, ulValue);
                }
                // Run through the CString properties
                IHXBuffer* pValue = NULL;
                rv = pValues->GetFirstPropertyCString(pszName, pValue);
                while (SUCCEEDED(rv))
                {
                    // Copy name/value type character: 'c'
                    *pBuf++ = (BYTE) 'c';
                    // Copy name string plus NULL terminator
                    UINT32 ulBytes = (UINT32) strlen(pszName) + 1;
                    memcpy(pBuf, pszName, ulBytes);
                    pBuf += ulBytes;
                    // Copy value string plus NULL terminator
                    const char* pszValue = (const char*) pValue->GetBuffer();
                    ulBytes = (UINT32) strlen(pszValue) + 1;
                    memcpy(pBuf, pszValue, ulBytes);
                    pBuf += ulBytes;
                    // Get next CString prop
                    HX_RELEASE(pValue);
                    rv = pValues->GetNextPropertyCString(pszName, pValue);
                }
                // Run through the Buffer properties
                rv = pValues->GetFirstPropertyBuffer(pszName, pValue);
                while (SUCCEEDED(rv))
                {
                    // Copy name/value type character: 'b'
                    *pBuf++ = (BYTE) 'b';
                    // Copy name string plus NULL terminator
                    UINT32 ulBytes = (UINT32) strlen(pszName) + 1;
                    memcpy(pBuf, pszName, ulBytes);
                    pBuf += ulBytes;
                    // Copy value buffer size in big-endian format
                    UINT32 ulSize = pValue->GetSize();
                    *pBuf++ = (BYTE) ((ulSize >> 24) & 0x000000FF);
                    *pBuf++ = (BYTE) ((ulSize >> 16) & 0x000000FF);
                    *pBuf++ = (BYTE) ((ulSize >>  8) & 0x000000FF);
                    *pBuf++ = (BYTE) ( ulSize        & 0x000000FF);
                    // Copy value buffer
                    memcpy(pBuf, pValue->GetBuffer(), ulSize);
                    pBuf += ulSize;
                    // Get next Buffer prop
                    HX_RELEASE(pValue);
                    rv = pValues->GetNextPropertyBuffer(pszName, pValue);
                }
            }
        }
    }

    return retVal;
}

HX_RESULT CreateBuffer(REF(IHXBuffer*) rpBuffer,
                       IUnknown*        pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    // Were we given a context?
    if (pContext)
    {
        // Yes, we have a context, use it to create the buffer
        //
        // Get the CCF
        IHXCommonClassFactory* pCCF = NULL;
        retVal = pContext->QueryInterface(IID_IHXCommonClassFactory,
                                          (void**) &pCCF);
        if (SUCCEEDED(retVal))
        {
            // Create a buffer
            HX_RELEASE(rpBuffer);
            retVal = pCCF->CreateInstance(CLSID_IHXBuffer,
                                          (void**) &rpBuffer);
        }
        HX_RELEASE(pCCF);
    }
    else
    {
        // No, we don't have a context, so use CHXBuffer
        CHXBuffer* pBuffer = new CHXBuffer();
        if (pBuffer)
        {
            // Assign the out parameter
            HX_RELEASE(rpBuffer);
            rpBuffer = (IHXBuffer*) pBuffer;
            rpBuffer->AddRef();
            // Clear the return value
            retVal = HXR_OK;
        }
    }

    return retVal;
}

HX_RESULT CreateValues(REF(IHXValues*) rpValues,
                       IUnknown*        pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    // Were we given a context?
    if (pContext)
    {
        // Yes, we have a context, use it to create the IHXValues
        //
        // Get the CCF
        IHXCommonClassFactory* pCCF = NULL;
        retVal = pContext->QueryInterface(IID_IHXCommonClassFactory,
                                          (void**) &pCCF);
        if (SUCCEEDED(retVal))
        {
            // Create an IHXValues
            HX_RELEASE(rpValues);
            retVal = pCCF->CreateInstance(CLSID_IHXValues,
                                          (void**) &rpValues);
        }
        HX_RELEASE(pCCF);
    }
    else
    {
        // No, we don't have a context, so use CHXBuffer
        CHXHeader* pValues = new CHXHeader();
        if (pValues)
        {
            // Assign the out parameter
            HX_RELEASE(rpValues);
            rpValues = (IHXValues*) pValues;
            rpValues->AddRef();
            // Clear the return value
            retVal = HXR_OK;
        }
    }

    return retVal;
}

HX_RESULT CreateStringBuffer(REF(IHXBuffer*) rpBuffer,
                             const char*      pszStr,
                             IUnknown*        pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pszStr)
    {
        // Create a buffer
        IHXBuffer* pBuffer = NULL;
        retVal = CreateBuffer(pBuffer, pContext);
        if (SUCCEEDED(retVal))
        {
            // Set the string into the buffer
            retVal = pBuffer->Set((const UCHAR*) pszStr, strlen(pszStr) + 1);
            if (SUCCEEDED(retVal))
            {
                // Assign the out parameters
                HX_RELEASE(rpBuffer);
                rpBuffer = pBuffer;
                rpBuffer->AddRef();
            }
        }
        HX_RELEASE(pBuffer);
    }

    return retVal;
}

HX_RESULT SetCStringProperty(IHXValues* pValues,
                             const char* pszName,
                             const char* pszValue,
                             IUnknown*   pContext,
                             BOOL        bSetAsBufferProp)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && pszName && pszValue)
    {
        // Create value buffer
        IHXBuffer* pValue = NULL;
        retVal = CreateStringBuffer(pValue, pszValue, pContext);
        if (SUCCEEDED(retVal))
        {
            // Set the property
            if (bSetAsBufferProp)
            {
                retVal = pValues->SetPropertyBuffer(pszName, pValue);
            }
            else
            {
                retVal = pValues->SetPropertyCString(pszName, pValue);
            }
        }
        HX_RELEASE(pValue);
    }

    return retVal;
}

HX_RESULT SetCStringPropertyWithNullTerm(IHXValues*  pValues,
                                         const char* pszName,
                                         BYTE*       pBuf,
                                         UINT32      ulLen,
                                         IUnknown*   pContext,
                                         BOOL        bSetAsBufferProp)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && pszName && pBuf && ulLen)
    {
        // Create value buffer
        IHXBuffer* pValue = NULL;
        retVal = CreateBuffer(pValue, pContext);
        if (SUCCEEDED(retVal))
        {
            // Set the buffer size
            retVal = pValue->SetSize(ulLen + 1);
            if (SUCCEEDED(retVal))
            {
                // Get the buffer pointer
                BYTE* pValBuf = pValue->GetBuffer();
                if (pValBuf)
                {
                    // memcpy the buffer
                    memcpy(pValBuf, pBuf, ulLen); /* Flawfinder: ignore */
                    // Add the NULL-terminator
                    pValBuf[ulLen] = (BYTE) '\0';
                    // Are we setting as a buffer property?
                    if (bSetAsBufferProp)
                    {
                        retVal = pValues->SetPropertyBuffer(pszName, pValue);
                    }
                    else
                    {
                        retVal = pValues->SetPropertyCString(pszName, pValue);
                    }
                }
                else
                {
                    retVal = HXR_OUTOFMEMORY;
                }
            }
        }
        HX_RELEASE(pValue);
    }

    return retVal;
}

HX_RESULT CreateNullTermBuffer(BYTE*  pBuf,
                               UINT32 ulLen,
                               char** ppNTBuf)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pBuf && ulLen && ppNTBuf)
    {
        char* pTmp = new char [ulLen+1];
        if (pTmp)
        {
            // Copy pBuf into the new buf
            memcpy(pTmp, pBuf, ulLen); /* Flawfinder: ignore */
            // NULL-terminate the new buf
            pTmp[ulLen] = '\0';
            // Set the out parameter
            *ppNTBuf = pTmp;
            // Clear the return value
            retVal = HXR_OK;
        }
    }

    return retVal;
}

HX_RESULT SetBufferProperty(IHXValues* pValues,
                            const char* pszName,
                            BYTE*       pBuf,
                            UINT32      ulLen,
                            IUnknown*   pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && pszName && pBuf && ulLen)
    {
        // Create value buffer
        IHXBuffer* pValue = NULL;
        retVal = CreateBuffer(pValue, pContext);
        if (SUCCEEDED(retVal))
        {
            // Set the buffer
            retVal = pValue->Set(pBuf, ulLen);
            if (SUCCEEDED(retVal))
            {
                // Set the property
                retVal = pValues->SetPropertyBuffer(pszName, pValue);
            }
        }
        HX_RELEASE(pValue);
    }

    return retVal;
}

HX_RESULT UnpackPropertyULONG32(IHXValues* pValues,
                                REF(BYTE*)  rpBuf,
                                BYTE*       pLimit,
                                IUnknown*   pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && rpBuf && pLimit &&
        rpBuf < pLimit && rpBuf[0] == 'u')
    {
        // Skip the 'u' type character
        rpBuf++;
        // Save the beginning of the string
        const char* pszName = (const char*) rpBuf;
        // Search until we find either a NULL or 
        // the end of the buffer
        while (rpBuf < pLimit && *rpBuf != 0) ++rpBuf;
        // Did we get a NULL terminator?
        if (rpBuf < pLimit && *rpBuf == 0)
        {
            // We know now that pszName is a valid string
            //
            // Skip the NULL terminator
            ++rpBuf;
            // Now get the ULONG32
            if (rpBuf + 4 <= pLimit)
            {
                // Unpack in big-endian form
                UINT32 ulValue = ((rpBuf[0] << 24) & 0xFF000000) |
                                 ((rpBuf[1] << 16) & 0x00FF0000) |
                                 ((rpBuf[2] <<  8) & 0x0000FF00) |
                                 ( rpBuf[3]        & 0x000000FF);
                // Skip the ULONG32
                rpBuf += 4;
                // Set the property
                retVal = pValues->SetPropertyULONG32(pszName, ulValue);
            }
        }
    }

    return retVal;
}

HX_RESULT UnpackPropertyCString(IHXValues* pValues,
                                REF(BYTE*)  rpBuf,
                                BYTE*       pLimit,
                                IUnknown*   pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && rpBuf && pLimit &&
        rpBuf < pLimit && rpBuf[0] == 'c')
    {
        // Skip the 'c' type character
        rpBuf++;
        // Save the beginning of the string
        const char* pszName = (const char*) rpBuf;
        // Search until we find either a NULL or 
        // the end of the buffer
        while (rpBuf < pLimit && *rpBuf != 0) ++rpBuf;
        // Did we get a NULL terminator?
        if (rpBuf < pLimit && *rpBuf == 0)
        {
            // We know now that pszName is a valid string
            //
            // Skip the NULL terminator
            ++rpBuf;
            // Save the beginning of the value string
            const char* pszValue = (const char*) rpBuf;
            // Search until we find either a NULL or 
            // the end of the buffer
            while (rpBuf < pLimit && *rpBuf != 0) ++rpBuf;
            // Did we get a NULL terminator?
            if (rpBuf < pLimit && *rpBuf == 0)
            {
                // Skip the NULL terminator
                ++rpBuf;
                // We know now that pszValue is a valid string, so
                // make an IHXBuffer out of it
                IHXBuffer* pValue = NULL;
                retVal = CreateStringBuffer(pValue, pszValue, pContext);
                if (SUCCEEDED(retVal))
                {
                    // Set the property
                    retVal = pValues->SetPropertyCString(pszName, pValue);
                }
                HX_RELEASE(pValue);
            }
        }
    }

    return retVal;
}

HX_RESULT UnpackPropertyBuffer(IHXValues* pValues,
                               REF(BYTE*)  rpBuf,
                               BYTE*       pLimit,
                               IUnknown*   pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && rpBuf && pLimit &&
        rpBuf < pLimit && rpBuf[0] == 'b')
    {
        // Skip the 'b' type character
        rpBuf++;
        // Save the beginning of the string
        const char* pszName = (const char*) rpBuf;
        // Search until we find either a NULL or 
        // the end of the buffer
        while (rpBuf < pLimit && *rpBuf != 0) ++rpBuf;
        // Did we get a NULL terminator?
        if (rpBuf < pLimit && *rpBuf == 0)
        {
            // We know now that pszName is a valid string
            //
            // Skip the NULL terminator
            ++rpBuf;
            // Do we have enough bytes to get the buffer length?
            if (rpBuf + 4 <= pLimit)
            {
                // Unpack the buffer length in big-endian form
                UINT32 ulLen = ((rpBuf[0] << 24) & 0xFF000000) |
                               ((rpBuf[1] << 16) & 0x00FF0000) |
                               ((rpBuf[2] <<  8) & 0x0000FF00) |
                               ( rpBuf[3]        & 0x000000FF);
                // Skip the buffer length
                rpBuf += 4;
                // Do we have enough bytes to get the buffer?
                if (rpBuf + ulLen <= pLimit)
                {
                    // Create a buffer
                    IHXBuffer* pValue = NULL;
                    retVal = CreateBuffer(pValue, pContext);
                    if (SUCCEEDED(retVal))
                    {
                        // Set the buffer
                        retVal = pValue->Set(rpBuf, ulLen);
                        if (SUCCEEDED(retVal))
                        {
                            // Skip the buffer bytes
                            rpBuf += ulLen;
                            // Set the property
                            retVal = pValues->SetPropertyBuffer(pszName,
                                                                pValue);
                        }
                    }
                    HX_RELEASE(pValue);
                }
            }
        }
    }

    return retVal;
}

HX_RESULT UnpackValuesBinary(IHXValues* pValues,
                             IHXBuffer* pBuffer,
                             IUnknown*   pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues && pBuffer)
    {
        retVal = UnpackValuesBinary(pValues,
                                    pBuffer->GetBuffer(),
                                    pBuffer->GetSize(),
                                    pContext);
    }

    return retVal;
}

HX_RESULT UnpackValuesBinary(IHXValues* pValues,
                             BYTE*       pBuf,
                             UINT32      ulLen,
                             IUnknown*   pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pBuf && ulLen)
    {
        // Get the pointer limit
        BYTE* pLimit = pBuf + ulLen;
        // Make sure this is not string format
        if (pBuf[0] != '[')
        {
            // Clear the return value
            retVal = HXR_OK;
            // Loop through the buffer, unpacking name/value pairs
            while (pBuf < pLimit && SUCCEEDED(retVal))
            {
                // Get a property type code
                char c = (char) pBuf[0];
                // Save the buffer pointer
                BYTE* pCur = pBuf;
                // Switch based on type
                switch (c)
                {
                    case 'u':
                        retVal = UnpackPropertyULONG32(pValues, pBuf,
                                                       pLimit, pContext);
                        break;
                    case 'c':
                        retVal = UnpackPropertyCString(pValues, pBuf,
                                                       pLimit, pContext);
                        break;
                    case 'b':
                        retVal = UnpackPropertyBuffer(pValues, pBuf,
                                                      pLimit, pContext);
                        break;
                    default:
                        retVal = HXR_FAIL;
                        break;
                }
                // Do a sanity check: if we succeeded,
                // then we must have advanced the pointer.
                // This will prevent an infinite loop.
                if (SUCCEEDED(retVal) && pCur == pBuf)
                {
                    retVal = HXR_FAIL;
                }
            }
        }
    }

    return retVal;
}

HX_RESULT PackValues(REF(IHXBuffer*) rpBuffer,
                     IHXValues*      pValues,
                     BOOL             bPackBinary,
                     IUnknown*        pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pValues)
    {
        // Create an output IHXBuffer
        IHXBuffer* pBuffer = NULL;
        retVal = CreateBuffer(pBuffer, pContext);
        if (SUCCEEDED(retVal))
        {
            // Are we supposed to pack this in binary form?
            if (bPackBinary)
            {
                // Pack in binary form
                //
                // First get the size necessary to pack it in binary form
                UINT32 ulBinPackSize = GetBinaryPackedSize(pValues);
                if (ulBinPackSize)
                {
                    // Make the buffer this size
                    retVal = pBuffer->SetSize(ulBinPackSize);
                    if (SUCCEEDED(retVal))
                    {
                        // Binary pack the buffer
                        retVal = PackValuesBinary(pBuffer, pValues);
                        if (SUCCEEDED(retVal))
                        {
                            // Copy the out parameter
                            HX_RELEASE(rpBuffer);
                            rpBuffer = pBuffer;
                            rpBuffer->AddRef();
                        }
                    }
                }
                else
                {
                    retVal = HXR_FAIL;
                }
            }
            else
            {
                // Pack in a string
                CHXString cTmp;
                retVal = PackValues(cTmp, pValues);
                if (SUCCEEDED(retVal))
                {
                    // Now just copy the string into the IHXBuffer
                    retVal = pBuffer->Set((const UCHAR*) (const char*) cTmp,
                                          cTmp.GetLength() + 1);
                    if (SUCCEEDED(retVal))
                    {
                        // Set the out parameter
                        HX_RELEASE(rpBuffer);
                        rpBuffer = pBuffer;
                        rpBuffer->AddRef();
                    }
                }
            }
        }
        HX_RELEASE(pBuffer);
    }

    return retVal;
}

HX_RESULT UnpackValues(REF(IHXValues*) rpValues,
                       IHXBuffer*      pBuffer,
                       IUnknown*        pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pBuffer)
    {
        retVal = UnpackValues(rpValues,
                              pBuffer->GetBuffer(),
                              pBuffer->GetSize(),
                              pContext);
    }

    return retVal;
}

HX_RESULT UnpackValues(REF(IHXValues*) rpValues,
                       BYTE*            pBuf,
                       UINT32           ulLen,
                       IUnknown*        pContext)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pBuf && ulLen)
    {
        // Create an output IHXValues
        IHXValues* pValues = NULL;
        retVal = CreateValues(pValues, pContext);
        if (SUCCEEDED(retVal))
        {
            // Get the string
            const char* pszBuffer = (const char*) pBuf;
            // Is this packed in binary or text? If it's in text
            // form, then it will have a '[' as the first character
            if (pszBuffer[0] != '[')
            {
                // The buffer is binary packed
                retVal = UnpackValuesBinary(pValues, pBuf, ulLen, pContext);
            }
            else
            {
                // The buffer is text packed
                retVal = UnpackValues(pszBuffer, pValues, FALSE);
            }
            if (SUCCEEDED(retVal))
            {
                // Assign the out parameter
                HX_RELEASE(rpValues);
                rpValues = pValues;
                rpValues->AddRef();
            }
        }
        HX_RELEASE(pValues);
    }

    return retVal;
}

// This method checks to see if all the properties
// in pValues1 are in pValues2 and that the value
// of these properties is indentical. pValues2,
// however, could still have properties that pValues1
// doesn't have. AreValuesIdentical() calls
// AreValuesInclusiveIdentical() in both directions,
// which establishes absolute equality if both
// are true.
BOOL AreValuesInclusiveIdentical(IHXValues* pValues1,
                                 IHXValues* pValues2)
{
    BOOL bRet = FALSE;

    if (pValues1 && pValues2)
    {
        // Assume that they are equal, and breakout
        // at the first inequality
        bRet = TRUE;
        // Check the ULONG32 properties
        const char* pszName  = NULL;
        UINT32      ulValue1 = 0;
        HX_RESULT rv = pValues1->GetFirstPropertyULONG32(pszName, ulValue1);
        while (SUCCEEDED(rv) && bRet)
        {
            // Lookup this property in pValues2
            UINT32 ulValue2 = 0;
            HX_RESULT rv2 = pValues2->GetPropertyULONG32(pszName, ulValue2);
            // Check for a match
            if (FAILED(rv2) || ulValue1 != ulValue2)
            {
                bRet = FALSE;
            }
            // Get next ULONG32 property
            rv = pValues1->GetNextPropertyULONG32(pszName, ulValue1);
        }
        if (bRet)
        {
            // Check the CString properties
            IHXBuffer* pValue1 = NULL;
            rv = pValues1->GetFirstPropertyCString(pszName, pValue1);
            while (SUCCEEDED(rv) && bRet)
            {
                // Lookup this property in pValues2
                IHXBuffer* pValue2 = NULL;
                HX_RESULT rv2 = pValues2->GetPropertyCString(pszName, pValue2);
                if (FAILED(rv2) ||
                    strcmp((const char*) pValue1->GetBuffer(),
                           (const char*) pValue2->GetBuffer()) != 0)
                {
                    bRet = FALSE;
                }
                HX_RELEASE(pValue2);
                // Get next CString prop
                HX_RELEASE(pValue1);
                rv = pValues1->GetNextPropertyCString(pszName, pValue1);
            }
            if (bRet)
            {
                // Check the buffer properties
                rv = pValues1->GetFirstPropertyBuffer(pszName, pValue1);
                while (SUCCEEDED(rv) && bRet)
                {
                    // Lookup this property in pValues2
                    IHXBuffer* pValue2 = NULL;
                    HX_RESULT rv2 = pValues2->GetPropertyBuffer(pszName, pValue2);
                    if (FAILED(rv2)                              ||
                        pValue1->GetSize() != pValue2->GetSize() ||
                        memcmp((const void*) pValue1->GetBuffer(),
                               (const void*) pValue2->GetBuffer(),
                               pValue1->GetSize()) != 0)
                    {
                        bRet = FALSE;
                    }
                    HX_RELEASE(pValue2);
                    // Get next Buffer prop
                    HX_RELEASE(pValue1);
                    rv = pValues1->GetNextPropertyBuffer(pszName, pValue1);
                }
            }
        }
    }

    return bRet;
}

BOOL AreValuesIdentical(IHXValues* pValues1,
                        IHXValues* pValues2)
{
    BOOL bRet = FALSE;

    // Check if all the properties in pValues1 are
    // in pValues2 and are identical
    bRet = AreValuesInclusiveIdentical(pValues1, pValues2);
    if (bRet)
    {
        // Check if all the properties in pValues2 are
        // in pValues1 and are identical
        bRet = AreValuesInclusiveIdentical(pValues2, pValues1);
    }

    return bRet;
}

#ifdef _DEBUG

HX_RESULT TestValuesPacking(IUnknown* pContext)
{
    HX_RESULT retVal = HXR_OK;

    // Create an IHXValues
    IHXValues* pValues = NULL;
    retVal = CreateValues(pValues, pContext);
    if (SUCCEEDED(retVal))
    {
        // Populate this IHXValues
        pValues->SetPropertyULONG32("ulong1", 42);
        pValues->SetPropertyULONG32("ulong2", 0xbaadf00d);
        SetCStringProperty(pValues, "cstring1",
                           "Rock the Casbah", pContext);
        SetCStringProperty(pValues, "cstring2",
                           "Sandinista", pContext);
        UINT32 ulBuf1Len = 128;
        BYTE*  pBuf1     = new BYTE [ulBuf1Len];
        if (pBuf1)
        {
            // Fill the buffer with random byte values
            srand(time(NULL));
            for (UINT32 i = 0; i < ulBuf1Len; i++)
            {
                UINT32 ulVal = (UINT32) rand();
                pBuf1[i]     = (BYTE) (ulVal & 0x000000FF);
            }
            SetBufferProperty(pValues, "buffer1", pBuf1, ulBuf1Len, pContext);
            // Pack it as text
            IHXBuffer* pBufferText = NULL;
            retVal = PackValues(pBufferText, pValues, FALSE, pContext);
            if (SUCCEEDED(retVal))
            {
                // Now unpack it
                IHXValues* pValuesTextOut = NULL;
                retVal = UnpackValues(pValuesTextOut, pBufferText, pContext);
                if (SUCCEEDED(retVal))
                {
                    // Compare them
                    if (AreValuesIdentical(pValues, pValuesTextOut))
                    {
                        // Now pack as binary
                        IHXBuffer* pBufferBinary = NULL;
                        retVal = PackValues(pBufferBinary, pValues,
                                            TRUE, pContext);
                        if (SUCCEEDED(retVal))
                        {
                            // Now unpack it
                            IHXValues* pValuesBinaryOut = NULL;
                            retVal = UnpackValues(pValuesBinaryOut,
                                                  pBufferBinary, pContext);
                            if (SUCCEEDED(retVal))
                            {
                                // Compare them
                                if (!AreValuesIdentical(pValues,
                                                        pValuesBinaryOut))
                                {
                                    // Oops - they are not the same
                                    retVal = HXR_FAIL;
                                }
                            }
                            HX_RELEASE(pValuesBinaryOut);
                        }
                        HX_RELEASE(pBufferBinary);
                    }
                    else
                    {
                        // Oops - they are not the same
                        retVal = HXR_FAIL;
                    }
                }
                HX_RELEASE(pValuesTextOut);
            }
            HX_RELEASE(pBufferText);
        }
        HX_VECTOR_DELETE(pBuf1);
    }
    HX_RELEASE(pValues);

    return retVal;
}

#endif
