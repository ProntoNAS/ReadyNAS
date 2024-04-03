/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxmangle.cpp,v 1.1.1.1 2006/03/29 16:45:31 hagi Exp $
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

//#include "hlxclib/stdio.h"
#include "hlxclib/string.h"
#include "hxmangle.h"

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif

// Given an input buffer, mangle it and return it back
// The caller has to call delete[] on the returned pointer
char*
Cipher(const char* pszBuffer)
{
    char*   pszResult = NULL;
    int	    i = 0;
    int	    nSize = 0;
    
    if (!pszBuffer)
    {
	goto cleanup;
    }

    nSize = strlen(pszBuffer);
    pszResult = new char[nSize * 2 + 1];
    memset(pszResult, 0, nSize * 2 + 1);

    for (i = 0; i < nSize; i++)
    {
	pszResult[i * 2]     = 'c' +  ((pszBuffer[i] & 0x0f) ^ (i & 0x0f) ^ 0x07);
	pszResult[i * 2 + 1] = 'f' + (((pszBuffer[i] & 0xf0) >> 4) ^ (i & 0x0f) ^ 0x03);
    }

cleanup:

    return pszResult;
}

// Given an input buffer, de-mangle it and return it back
// The caller has to call delete[] on the returned pointer
char*
DeCipher(const char* pszBuffer)
{
    char*   pszResult = NULL;
    int	    i = 0; 
    int	    nSize = 0;

    if (!pszBuffer)
    {
	goto cleanup;
    }

    nSize = strlen(pszBuffer);
    pszResult = new char[nSize / 2 + 1];
    memset(pszResult, 0, nSize / 2 + 1);

    for (i = 0; i < nSize / 2; i++)
    {
	pszResult[i] = ((pszBuffer[i * 2] - 'c') ^ (i & 0x0f) ^ 0x07) |
			(((pszBuffer[i * 2 + 1] - 'f') ^ (i & 0x0f) ^ 0x03) << 4);
    }

cleanup:

    return pszResult;
}
