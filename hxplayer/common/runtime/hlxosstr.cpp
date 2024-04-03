/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hlxosstr.cpp,v 1.1.1.1 2006/03/29 16:45:33 hagi Exp $
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

#include "hlxosstr.h"
#include "hlxclib/windows.h"
#include "hlxclib/assert.h"
#include "hlxclib/string.h"

#ifdef _WINCE
#  if !defined(CP_UTF8)
#     define CP_UTF8 CP_ACP
#  else //!CP_UTF8
#     if defined(_X86_) && (CP_UTF8==65001)
#       undef CP_UTF8
#       define CP_UTF8 CP_ACP
#     endif //_X86_ && (CP_UTF8==65001)
#  endif //!CP_UTF8
#endif //_WINCE


#ifndef _WINDOWS
#define CP_UTF8 65001

#if defined(_FREEBSD) || defined(_OPENBSD) || defined(_NETBSD) || \
    (defined(_MACINTOSH) && defined(_MAC_MACHO)) || defined(_MAC_UNIX) || \
    defined(_OPENWAVE_ARMULATOR)
int wcslen(const wchar_t* pStr)
{
    assert(!"wcslen() Not Implemented\n");
    return 0;
}
#elif defined(_SYMBIAN) || defined(_OPENWAVE_SIMULATOR)
// We already got wcslen in string.h
#else
#include <wchar.h> //for wcslen()
#endif

// Declare static tables needed by the UTF8 <-> Unicode functions
static const unsigned char z_byteZeroMask[] = {
    0x1F, 0x0F, 0x07, 0x03, 0x01};

static const unsigned int z_UTF8Bounds[] = {
    0, 0x80, 0x800, 0x10000, 0x200000, 0x4000000, 0x80000000};

static int UTF8toUnicode(const char* pIn, 
			 int inSize,
			 unsigned int& out)
{
    int ret = 0;
    
    if (inSize > 0)
    {
	if (pIn[0] & 0x80)
	{
	    unsigned int value = pIn[0] << 1;
	    int byteCount = 1;
		
	    // Count the number of bytes
	    while (value & 0x80)
	    {
		byteCount++;
		value <<= 1;
	    }
		
	    // Make sure the byte count is within expected values and
	    // that there are enough bytes in the input to contain
	    // the encoding of this character
	    if ((byteCount > 1) && 
		(byteCount < 7) && 
		(inSize >= byteCount))
	    {
		bool failed = false;
		value = pIn[0] & z_byteZeroMask[byteCount - 2];
		    
		for (int i = 1; i < byteCount; i++)
		{
		    if ((pIn[i] & 0xC0) == 0x80)
		    {
			value <<= 6;
			value |= pIn[i] & 0x3F;
		    }
		    else
		    {
			failed = true;
			break;
		    }
		}
		    
		// Make sure we have not failed yet and make sure
		// the value decoded is within the proper bounds for
		// that encoding method
		if ((!failed) &&
		    (value >= z_UTF8Bounds[byteCount-1]) &&
		    (value < z_UTF8Bounds[byteCount]))
		{		
		    ret = byteCount;
		    out = value;
		}
	    }
	}
	else
	{
	    // single byte
	    out = pIn[0];
	    ret = 1;
	}
    }

    return ret;
}

static int UnicodeToUTF8(unsigned int in, 
			 char* pOut, 
			 int outSize)
{
    int bytesWritten = 0;

    unsigned int i = 0;
    int bytesNeeded = -1;
    unsigned char ch;

    while (i < (sizeof(z_UTF8Bounds) / sizeof(unsigned int)))
    {
	if (in < z_UTF8Bounds[i])
	{
	    bytesNeeded = i;
	    break;
	}
	i++;
    }

    if (bytesNeeded == 1)
    {
	ch = in & 0xff;
	if (outSize > bytesWritten)
	{
	    pOut[bytesWritten] = ch;
	}
	bytesWritten++;
    }
    else if (bytesNeeded > 1)
    {
	ch = ((0xff << (8 - bytesNeeded)) | 
	      ((in >> ((bytesNeeded - 1) * 6) ) & 
	       z_byteZeroMask[bytesNeeded - 2]));
	if (outSize > bytesWritten)
	{
	    pOut[bytesWritten] = ch;
	}
	bytesWritten++;

	for (int j = bytesNeeded - 2; j >= 0; j--)
	{
	    ch = 0x80 | ((in >> (j * 6)) & 0x3F);
	    if (outSize > bytesWritten)
	    {
		pOut[bytesWritten] = ch;
	    }
	    bytesWritten++;
	}

    }
    else
    {
	bytesWritten = 0;
    }

    return bytesWritten;
}

static int ConvertUTF8ToUnicode(const char* pInBuf, 
				int iInSize,
				wchar_t* pOutWideBuf,
				int iOutSize)
{
    int used_from_input_this_round;
    int used_from_output = 0;
    unsigned int unicode = 0;
    bool failed = false;

    if (iInSize < 0)
    {
	iInSize = strlen(pInBuf);
    }

    while (iInSize > 0)
    {
	used_from_input_this_round = 
	    UTF8toUnicode(pInBuf, iInSize, unicode);
	
	if (used_from_input_this_round)
	{
	    pInBuf += used_from_input_this_round;
	    iInSize -= used_from_input_this_round;
	    if (used_from_output < iOutSize)
	    {
		pOutWideBuf[used_from_output] = unicode;
	    }
	    used_from_output++;
	}
	else
	{
	    failed = true;
	    break;
	}
    }

    // Always make sure the output is null terminated
    if (unicode != 0)
    {
	unicode = 0;
	if (used_from_output < iOutSize)
	{
	    pOutWideBuf[used_from_output] = unicode;
	}
	used_from_output++;
    }
    
    if ((failed || (used_from_output > iOutSize)) && (iOutSize != 0))
    {
	used_from_output = 0;
    }

    return used_from_output;
}

static int ConvertUnicodeToUTF8(const wchar_t* pInWideBuf,
				int iInSize,
				char* pOutBuf, 
				int iOutSize)
{
    int used_from_output_this_round;
    int used_from_output = 0;
    unsigned int unicode = 0;
    bool failed = false;

    if (iInSize < 0)
    {
	iInSize = wcslen(pInWideBuf);
    }

    while (iInSize > 0)
    {
	unicode = *pInWideBuf;
	used_from_output_this_round =
	    UnicodeToUTF8(unicode, 
			  &(pOutBuf[used_from_output]),
			  iOutSize - used_from_output);
	
	if (used_from_output_this_round)
	{
	    pInWideBuf++;
	    iInSize--;
	    used_from_output += used_from_output_this_round;
	}
	else
	{
	    failed = true;
	    break;
	}
    }

    // Always make sure the output is null terminated
    if (unicode != 0)
    {
	unicode = 0;
	used_from_output_this_round =
	    UnicodeToUTF8(unicode, 
			  &(pOutBuf[used_from_output]),
			  iOutSize - used_from_output);
	used_from_output += used_from_output_this_round;
	if (!used_from_output_this_round)
	{
	    failed = true;
	}
    }
    
    if ((failed || (used_from_output > iOutSize)) && (iOutSize != 0))
    {
	used_from_output = 0;
    }

    return used_from_output;
}

int MultiByteToWideChar(UINT8 CodePage,        // code page
			 ULONG32 dwFlags,       // character-type options
			 const char* lpMultiByteStr, // string to map
			 int cchMultiByte,      // number of bytes in string
			 wchar_t* lpWideCharStr, // wide-character buffer
			 int cchWideChar)        // size of buffer
{
    return ConvertUTF8ToUnicode(lpMultiByteStr, 
				cchMultiByte,
				lpWideCharStr,
				cchWideChar);
}

int WideCharToMultiByte(UINT8 CodePage,        // code page
			 ULONG32 dwFlags,      // performance and mapping flags
			 const wchar_t* lpWideCharStr, // wide-character string
			 int cchWideChar,       // number of characters
			 char* lpMultiByteStr, // buffer for new string
			 int cchMultiByte,      // size of buffer
			 char* lpDefaultChar,  // default for unmappable 
                                                // characters
			 BOOL* lpUsedDefaultChar) // flag set when default 
                                                 // char. used
{
    return ConvertUnicodeToUTF8(lpWideCharStr,
				cchWideChar,
				lpMultiByteStr, 
				cchMultiByte);
}

#endif

HLXOsStrW::HLXOsStrW(const char* ascii, size_t length) : 
    m_isMutable(FALSE), 
    m_toAscii(TRUE),
    m_size(0),
    m_outsize(0),
    m_uni(0),
    m_ascii(0)
{
    Init(ascii, length);
}

HLXOsStrW::HLXOsStrW(char* ascii, size_t length) : 
    m_isMutable(TRUE), 
    m_toAscii(TRUE),
    m_size(0),
    m_outsize(0),
    m_uni(0),
    m_ascii(ascii)
{ 
    Init(ascii, length);
}

HLXOsStrW::HLXOsStrW(const unsigned char* ascii, size_t length) : 
    m_isMutable(FALSE), 
    m_toAscii(TRUE),
    m_size(0),
    m_outsize(0),
    m_uni(0),
    m_ascii(0)
{ 
    Init((const char*) ascii, length);
}

void HLXOsStrW::Init(const char* ascii, size_t length)
{
    m_size = ((length != (size_t)-1) ? length : ((ascii) ? strlen((const char*) ascii) + 1 : 0));

    if (ascii)
    {
	m_outsize = MultiByteToWideChar(CP_UTF8, 0, (const char*) ascii, length, NULL, 0);
	if( length!=(size_t)-1 && (size_t)m_outsize<length )
        m_outsize = length;
	if (m_uni = ((wchar_t*) malloc(m_outsize * sizeof(wchar_t))))
	{
	    m_outsize = MultiByteToWideChar(CP_UTF8, 0, (const char*) ascii, length, m_uni, m_outsize);
	}
	else
	{
	    m_outsize = 0;
	}
    }
}

HLXOsStrW::HLXOsStrW(const wchar_t* uni, size_t length) : 
    m_isMutable(FALSE), 
    m_toAscii(FALSE),
    m_size((length != (size_t)-1) ? length : ((uni) ? wcslen(uni) + 1 : 0)),
    m_uni(0),
    m_ascii(0)
{ 
    if (uni)
    {
	m_outsize = WideCharToMultiByte(CP_UTF8, 0, uni, length, NULL, 0, NULL, NULL);
	if( length!=(size_t)-1 && (size_t)m_outsize<length )
        m_outsize = length;
	if (m_ascii = ((char*) malloc(m_outsize)))
	{
	    m_outsize = WideCharToMultiByte(CP_UTF8, 0, uni, length, m_ascii, m_outsize, NULL, NULL); 
	}
	else
	{
	    m_outsize = 0;
	}
    }
}

HLXOsStrW::~HLXOsStrW() 
{ 
    if (m_isMutable) 
    {
	if (m_toAscii && m_ascii && m_uni)
	{
	    WideCharToMultiByte(CP_UTF8, 0, m_uni, -1, m_ascii, m_size, NULL, NULL);
	}
    }
    if (m_toAscii) 
    {
	if (m_uni)
	{
	    free(m_uni);
	}
    } 
    else 
    {
	if (m_ascii)
	{
	    free(m_ascii);
	}
    }
}

HLXOsStrW::HLXOsStrW(const HLXOsStrW& rhs) :
    m_isMutable(FALSE),
    m_toAscii(TRUE),
    m_size(0),
    m_outsize(0),
    m_uni(0),
    m_ascii(0)
{    
    Copy(*this, rhs);
}

HLXOsStrW& HLXOsStrW::operator=(const HLXOsStrW& rhs)
{
    if (&rhs != this)
    {
	Copy(*this, rhs);
    }

    return *this;
}

void HLXOsStrW::Copy(HLXOsStrW& lhs, const HLXOsStrW& rhs)
{
    lhs.m_isMutable = rhs.m_isMutable;
    lhs.m_toAscii = rhs.m_toAscii;
    lhs.m_size = rhs.m_size;

    if (lhs.m_toAscii)
    {
	if (rhs.m_uni)
	{
	    int bufSize = rhs.m_outsize * sizeof(wchar_t);
	    
	    if (lhs.m_uni)
	    {
		free(lhs.m_uni);
	    }
	    lhs.m_uni = (wchar_t*) malloc(bufSize);
	    if (lhs.m_uni)
	    {
		lhs.m_outsize = rhs.m_outsize;
		::memcpy(lhs.m_uni, rhs.m_uni, bufSize); /* Flawfinder: ignore */
	    }
	}
	lhs.m_ascii = rhs.m_ascii;
    }
    else
    {
	if (rhs.m_ascii)
	{
	    if (lhs.m_ascii)
	    {
		free(lhs.m_ascii);
	    }
	    lhs.m_ascii = (char*) malloc(rhs.m_outsize);
	    if (lhs.m_ascii)
	    {
		lhs.m_outsize = rhs.m_outsize;
		::memcpy(lhs.m_ascii, rhs.m_ascii, lhs.m_outsize); /* Flawfinder: ignore */
	    }
	}
	lhs.m_uni = rhs.m_uni;
    }
}
