/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxurl.cpp,v 1.1.1.1 2006/03/29 16:45:31 hagi Exp $
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

#include "hlxclib/string.h"
#include "hlxclib/stdlib.h"
//#include "hlxclib/stdio.h"
#include <ctype.h>

#include "hxcom.h"
#include "hxtypes.h"
#include "hxresult.h"
#include "hxcomm.h"
#include "tparse.h"
#include "dbcs.h"
#include "protdefs.h"
#include "hxstrutl.h"
#include "hxslist.h"
#include "hxurl.h"
#include "ihxpckts.h"
#include "chxminiccf.h"

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif

/* We should really define it in a common header file */
#if defined (_WINDOWS ) || defined (WIN32) || defined(_SYMBIAN)
#define OS_SEPARATOR_CHAR	'\\'
#define OS_SEPARATOR_STRING	"\\"
#elif defined (_UNIX) || defined(_OPENWAVE)
#define OS_SEPARATOR_CHAR	'/'
#define OS_SEPARATOR_STRING	"/"
#elif defined (_MACINTOSH)
#define OS_SEPARATOR_CHAR	':'
#define OS_SEPARATOR_STRING	":"
#else
#error "undefined platform hxurl.cpp"
#endif // defined (_WINDOWS ) || defined (WIN32)

CHXURL::CHXURL (const char* pszURL)
		:m_LastError (HXR_OK)
		,m_pActualURL(NULL)
                ,m_pszURL(NULL)
                ,m_pszEscapedURL(NULL)
		,m_pszOptions (NULL)
		,m_pszHost (NULL)
		,m_pszPort (NULL)
		,m_pszUsername(NULL)
		,m_pszPassword(NULL)
		,m_unProtocol(fileProtocol)
                ,m_unDefaultPort(0)
		,m_bNetworkProtocol (FALSE)
		,m_pszResource (NULL)
		,m_pProperties (NULL)
		,m_pOptions (NULL)
                ,m_pCCF(CreateCCF())
{
    if (m_pCCF)
    {
	m_pCCF->AddRef();
    }
    
    ConstructURL(pszURL);
}

CHXURL::CHXURL (const char* pszURL, IUnknown* pContext)
		:m_LastError (HXR_OK)
		,m_pActualURL(NULL)
                ,m_pszURL(NULL)
                ,m_pszEscapedURL(NULL)
		,m_pszOptions (NULL)
		,m_pszHost (NULL)
		,m_pszPort (NULL)
		,m_pszUsername(NULL)
		,m_pszPassword(NULL)
		,m_unProtocol(fileProtocol)
                ,m_unDefaultPort(0)
		,m_bNetworkProtocol (FALSE)
		,m_pszResource (NULL)
		,m_pProperties (NULL)
		,m_pOptions (NULL)
                ,m_pCCF(0)
{
    if (pContext)
    {
	pContext->QueryInterface(IID_IHXCommonClassFactory, (void**)&m_pCCF);
    }
    
    ConstructURL(pszURL);
}

void CHXURL::ConstructURL(const char* pszURL)
{
    char*   pszInputURL = NULL;
    char*   pszTemp = NULL;
    char*   pFragment = NULL;
    char*   pNewURL = NULL;
    char*   pszDollarSign = NULL;

    HX_ASSERT(pszURL != NULL);

    if (!pszURL)
    {
	m_LastError = HXR_INVALID_PATH;
	return;
    }
    
    if (!m_pCCF)
    {
	m_LastError = HXR_UNEXPECTED;
	return;
    }

    pszInputURL = new char[strlen(pszURL) + 1];
    if(!pszInputURL)
    {
        m_LastError = HXR_OUTOFMEMORY;
        return;
    }
    strcpy(pszInputURL, pszURL); /* Flawfinder: ignore */

    // Keep permanent copy of input url
    m_pszEscapedURL = new char[strlen(pszInputURL)+1];
    strcpy(m_pszEscapedURL, pszInputURL); /* Flawfinder: ignore */

    // IHXValues 
    if (m_pCCF)
    {
	m_pCCF->CreateInstance(CLSID_IHXValues, (void**)&m_pProperties);
	m_pCCF->CreateInstance(CLSID_IHXValues, (void**)&m_pOptions);
    }

    // protocol: determine whether it's network or local
    if (0 == StringNCompare(pszInputURL, "http:", 5))
    {
	m_unProtocol = httpProtocol;
	m_unDefaultPort = DEFAULT_HTTP_PORT;
    }
    else if (0 == StringNCompare(pszInputURL, "chttp:", 6))
    {
	m_unProtocol = httpProtocol;
	m_unDefaultPort = DEFAULT_HTTP_PORT;
    }
    else if (0 == StringNCompare(pszInputURL, "pnm:", 4))
    {
	m_unProtocol = pnmProtocol;
	m_bNetworkProtocol = TRUE;
	m_unDefaultPort = DEFAULT_PNA_PORT;
    }
    else if (0 == StringNCompare(pszInputURL, "rtsp:", 5))
    {
	m_unProtocol = rtspProtocol;
	m_bNetworkProtocol = TRUE;
	m_unDefaultPort = DEFAULT_RTSP_PORT;
    }
    else if (0 == StringNCompare(pszInputURL, "helix-sdp:", 10))
    {
	m_unProtocol = helixSDPProtocol;
	m_bNetworkProtocol = TRUE;
	m_unDefaultPort = DEFAULT_RTSP_PORT;
    }
    else if (0 == StringNCompare(pszInputURL, "https:", 6))
    {
	m_unProtocol = httpsProtocol;
	m_unDefaultPort = DEFAULT_HTTPS_PORT;
    }

    if (m_pProperties)
    {
	m_pProperties->SetPropertyULONG32(PROPERTY_PROTOCOL, (ULONG32)m_unProtocol);
    }
    else
    {
	m_LastError = HXR_UNEXPECTED;
	goto cleanup;
    }

    // no need to further parse helix-sdp protocol
    if (helixSDPProtocol != m_unProtocol)
    {
        // separate fragment from the URL
        if (0!=(pszTemp = (char*) ::HXFindChar(pszInputURL, '#')))
        {
	    // save fragment
	    pFragment = pszTemp + 1;
	    ::SaveStringToHeader(m_pProperties, PROPERTY_FRAGMENT, pFragment);
	    *pszTemp = '\0';
        }

        // HP - allow '$' in directory/file name
        //
        // still need to take care of that obsolete $ sign option:
        // rtsp://moe.cr.prognet.com/ambush.rm$1:00
        // time after the $ is assumed to be the start time.
        //
        // the solution is to compare the string following the $ to 
        // a properly formed time. If the string is a time and only 
        // a time, then we know its the old-style start-time option
        // otherwise, '$' is part of the directory/file name and we
        // will keep it.
        pszDollarSign = (char*) ::HXFindChar(pszInputURL, '$');
        while (pszDollarSign)
        {
	    pszTemp = pszDollarSign + 1;

	    if (::TimeParse(pszTemp))
	    {
	        *pszDollarSign = '\0';
	        INT32 lLen = (2 * strlen(pszURL)) + 8;
	        pNewURL = new char[lLen];
	        memset(pNewURL, 0, lLen);
		          
	        // upgrade to a new URL
	        SafeSprintf(pNewURL, lLen, "%s?start=%s", pszInputURL, pszTemp);

	        HX_VECTOR_DELETE(pszInputURL);
	        pszInputURL = pNewURL;
	        break;
	    }

	    pszDollarSign = (char*) ::HXFindChar(pszTemp, '$');
        }
    }

#if !defined(_MACINTOSH) && !defined(_MAC_UNIX)
    // on Mac, unescaping can put /'s and ?'s back into file and folder names, which is deadly inside URLs
    //if (0 == StringNCompare(pszInputURL, "file:", 5))
    {
	// we only unescape the URL on local source since we are
	// *responding* instead of *requesting* for the given source
	Unescape(pszInputURL);
    }
#endif

    if (!CompressURL(pszInputURL, m_pszURL))
    {
	// make a copy of the URL
	m_pszURL = new char[strlen(pszInputURL)+1];
	strcpy(m_pszURL, pszInputURL); /* Flawfinder: ignore */
    }

    m_pActualURL = new char[strlen(m_pszURL)+1];
    strcpy(m_pActualURL, m_pszURL); /* Flawfinder: ignore */

    ::SaveStringToHeader(m_pProperties, PROPERTY_URL, m_pszURL);

    // no need to further parse helix-sdp protocol
    if (helixSDPProtocol != m_unProtocol)
    {
        // separate options from the URL
        if (0 != (pszTemp = (char*) ::HXFindChar(m_pszURL, '?')))
        {
	    // options
	    m_pszOptions = pszTemp + 1;
        }
       
        // collect protocol, host, port and resource info
        ParseURL (m_pszURL);
    
        // collect other options info if it has 
        if (m_pszOptions)
        {
	    if (HXR_INCOMPLETE == CollectOptions(m_pszOptions) && m_pszResource)
	    {
    	        // bad options and remove it from the URL
    	        pszTemp = (char*) ::HXFindChar(m_pszResource, '?');
	        if (pszTemp)
	        {
		    *pszTemp = '\0';

		    ParseResource();
	        }
	    }
        }   
    }

cleanup:

    HX_VECTOR_DELETE(pszInputURL);
}

CHXURL::~CHXURL ()
{
    HX_VECTOR_DELETE(m_pActualURL);
    HX_VECTOR_DELETE(m_pszURL);
    HX_VECTOR_DELETE(m_pszEscapedURL);

    HX_RELEASE(m_pProperties);
    HX_RELEASE(m_pOptions);

    HX_RELEASE(m_pCCF);
}

CHXURL::CHXURL(const CHXURL& rhs) 		
    :m_LastError (HXR_OK)
    ,m_pActualURL(NULL)
    ,m_pszURL(NULL)
    ,m_pszOptions (NULL)
    ,m_pszHost (NULL)
    ,m_pszPort (NULL)
    ,m_pszUsername(NULL)
    ,m_pszPassword(NULL)
    ,m_unProtocol (fileProtocol)
    ,m_bNetworkProtocol (FALSE)
    ,m_pszResource (NULL)
    ,m_pProperties (NULL)
    ,m_pOptions (NULL)
    ,m_pCCF(rhs.m_pCCF)
{
    if (m_pCCF)
    {
	m_pCCF->AddRef();
    }

    ConstructURL(rhs.GetEscapedURL());
}

CHXURL& CHXURL::operator=(const CHXURL& rhs)
{
    if (&rhs != this)
    {
	// Clean out old data
	HX_VECTOR_DELETE(m_pActualURL);
	HX_VECTOR_DELETE(m_pszURL);
        HX_VECTOR_DELETE(m_pszEscapedURL);
	HX_RELEASE(m_pProperties);
	HX_RELEASE(m_pOptions);
	HX_RELEASE(m_pCCF);
	m_LastError = HXR_OK;
	m_pszOptions = NULL;
	m_pszHost = NULL;
	m_pszPort = NULL;
	m_pszUsername = NULL;
	m_pszPassword = NULL;
	m_unProtocol = fileProtocol;
	m_bNetworkProtocol = FALSE;
	m_pszResource = NULL;

	ConstructURL(rhs.GetEscapedURL());
	m_pCCF = rhs.m_pCCF;

	if (m_pCCF)
	{
	    m_pCCF->AddRef();
	}
    }

    return *this;
}

HX_RESULT CHXURL::ParseURL (char* pszURL)
{
    char* pszOption = NULL;
    char* pszTemp = NULL;
    char* pszSlash = NULL;
    char* pszCursor = NULL;    

    if (HXR_OK != m_LastError)
    {
	goto cleanup;
    }

    pszCursor = pszURL;
    
    //
    // let's start
    //

    // find the scheme - note that ParseURL only succeeds
    // for absolute URLs
    m_LastError = HXR_INVALID_PROTOCOL;	// assume the worst
    pszTemp = (char *) FindURLSchemeEnd(pszURL);
    if (pszTemp)
    {
	char* pScheme = new_string(pszURL, pszTemp - pszURL);

	m_LastError = HXR_OK;
	::SaveStringToHeader(m_pProperties, PROPERTY_SCHEME, pScheme);

	delete[] pScheme;
    }

    if(HXR_OK != m_LastError)
    {
	goto cleanup;
    }

    pszCursor = pszTemp + 1;
  
    if(strncmp(pszCursor, "//", 2) == 0)
    {
	pszCursor += 2;	// skip '//'
    }
    else if(strncmp(pszCursor, "/", 1) == 0)
    {
	pszCursor += 1;	// skip '/' (ill-formed url?)
    }

    if (fileProtocol == m_unProtocol)
    {
	// resource
	if (*(m_pszResource = pszCursor) == '\0')
	{
	    m_LastError = HXR_INVALID_URL_PATH;
	    goto cleanup;
	}
    }
    // network URL + http
    else	
    {
	// First check for optional username and password parameters.
	// The colon is also optional if either username or password
	// is not given Form in context:
	// protocol://username:password@host:port/resource (From RFC
	// 1738)
	pszTemp = (char*) ::HXFindChar(pszCursor, ':');
	pszSlash = (char*) ::HXFindChar(pszCursor, '/');
	pszOption = (char*) ::HXFindChar(pszCursor, '@');

	// There is a username or password if we see a '@' character
	// according to RFC 1738 this is a reserved character
	if (pszOption && pszOption < pszSlash)
	{
	    // Username
	    if (*(m_pszUsername = pszCursor) == '\0')
	    {
		m_LastError = HXR_INVALID_URL_HOST;
		goto cleanup;
	    }

	    // If the is a ':' and it is before the '@' then we have a
	    // password, so zero terminate the username and move the
	    // cursor to the password
	    if (pszTemp && (pszTemp < pszOption))
	    {
		*pszTemp = '\0';
		pszCursor = pszTemp+1;
	    }
	    // There is no ':' so move the cursor to the '@' character
	    // so it will be a zero terminated empty string
	    else
		pszCursor = pszOption;

	    // Password
	    if (*(m_pszPassword = pszCursor) == '\0')
	    {
		m_LastError = HXR_INVALID_URL_HOST;
		goto cleanup;
	    }

	    // Zero terminate password and move the cursor to the hostname
	    *pszOption = '\0';

	    if (m_pszUsername)
	    {
		::SaveStringToHeader(m_pProperties, PROPERTY_USERNAME, m_pszUsername);
	    }

	    if (m_pszPassword)
	    {
		::SaveStringToHeader(m_pProperties, PROPERTY_PASSWORD, m_pszPassword);
	    }

	    pszCursor = pszOption+1;
	}

	// host
	if (*(m_pszHost = pszCursor) == '\0')
	{
	    m_LastError = HXR_INVALID_URL_HOST;
	    goto cleanup;
	}

	// port
	pszTemp = (char*) ::HXFindChar(pszCursor, '/');
	if (pszTemp)
	{
	    *pszTemp = '\0';
	    pszCursor = pszTemp+1;
	}
	else
	{
	    // it's legal to have RTSP or HTTP URLs with no resource.

            // The correct behavior for RTSP would be not to flag this as
            // an error but the client core treats the lack of a resource
            // as a failure. The correct change would be to have the client
            // core check explicitly for a resource but legacy code rears its
            // ugly head... so we flag the error (client core is happy) but
            // go ahead and parse the rest of the headers (so you can still 
            // get the host:port if you need to). */
	    if (m_unProtocol != httpProtocol && m_unProtocol != rtspProtocol)
	    {
		m_LastError = HXR_INVALID_URL_PATH;                
	    }
	    pszCursor = NULL;
	}


	// port
	pszTemp = (char*) ::HXFindChar(m_pszHost, ':');

	if (pszTemp)
	{
	    *pszTemp = '\0';
	  
	    if (*(m_pszPort = pszTemp+1) == '\0')
	    {
		m_LastError = HXR_INVALID_URL_HOST;
		goto cleanup;
	    }
	}

	if (m_pszHost)
	{
	    ::SaveStringToHeader(m_pProperties, PROPERTY_HOST, m_pszHost);
	}

	if (m_pszPort)
	{
	    m_pProperties->SetPropertyULONG32(PROPERTY_PORT, (ULONG32)atol(m_pszPort));
	}
	else if (m_unDefaultPort > 0)
	{
	    m_pProperties->SetPropertyULONG32(PROPERTY_PORT, (ULONG32)m_unDefaultPort);
	}

	if (pszCursor && (*(m_pszResource = pszCursor) == '\0' && m_unProtocol != httpProtocol && m_unProtocol != rtspProtocol))
	{
	    m_LastError = HXR_INVALID_URL_PATH;
	    goto cleanup;
	}
    }

    //
    // other options?
    //

    /*
    // 1.0 player
    if (pszTemp = (char*) ::HXFindChar(pszCursor, '$'))
    {
	*pszTemp = '\0';
	pszCursor = pszTemp+1;

	if (*(m_pszStartTime = pszCursor) == '\0')
	{
	    m_LastError = HXR_FAILED;
	    goto cleanup;
	}
    }
    */

cleanup:

    ParseResource();

    return m_LastError;
}

void
CHXURL::TrimOffSpaces(char*& pszString)
{
    if( NULL == pszString ) 
        return;
    
    char* pszValue = pszString;
    char* pszCursor = pszString;
    
    // trim off the leading spaces 
    while (*pszCursor == ' ')
    {
        pszCursor++;
    }

    pszValue = pszCursor;

    // trim off the tailing spaces
    if( strlen(pszCursor) != 0 )
    {
        pszCursor = pszCursor + strlen(pszCursor) - 1;
        
        while (*pszCursor == ' ' )
        {
            pszCursor--;
        }
        ++pszCursor;
        if( *pszCursor != '\0' )
            *pszCursor = '\0';
    }
    
    pszString = pszValue;
}
	
HX_RESULT
CHXURL::CollectOptions (char* pszOptions)
{
    HX_RESULT	hr = HXR_OK;
    char*   pszCursor = NULL;
    char*   pszKey = NULL;
    char*   pszValue = NULL;
    char*   pszTemp = NULL;
    BOOL    bValueQuoted = FALSE;
  
    if (HXR_OK != m_LastError)
    {
	return m_LastError;
    }

    pszCursor = pszOptions;

    char* pszEndOptions = pszOptions + strlen(pszOptions);

    // let's start parsing
    while (pszCursor < pszEndOptions)
    {
	//
	// collect one value pair at a time
	//

	// <key>="<value>" or <key>=<value> 
	pszKey = pszCursor;

	if (!(pszTemp = (char*) ::HXFindChar(pszCursor, '=')))
	{
	    hr = HXR_FAILED;
	    goto cleanup;
	}
	*pszTemp = '\0';

	pszCursor = pszTemp + 1;

	// remove all the spaces between '=' and actual value
	while (*pszCursor == ' ')
	{
	    pszCursor++;
	}

	// "<value>"
	if (*pszCursor == '"')
	{
	    bValueQuoted = TRUE;
	    pszCursor += 1;
	}

	pszValue = pszCursor;

	if (bValueQuoted)
	{
	    if (!(pszTemp = (char*) ::HXFindChar(pszCursor, '"')))
	    {
		hr = HXR_INCOMPLETE;
		goto cleanup;
	    }
	    *pszTemp = '\0';

	    pszCursor = pszTemp + 1;
	}

	if ((pszTemp = (char*) ::HXFindChar(pszCursor, '&')) != 0)
	{
	    *pszTemp = '\0';
	    
	    // move cursor to the next pair
	    pszCursor = pszTemp + 1;
	}
	else
	{
	    // move cursor to the end of this URL
	    pszCursor += strlen(pszValue);
	}

	// trim off leading/tailing spaces
	TrimOffSpaces(pszKey);
	TrimOffSpaces(pszValue);

        // decode each value (option) since it may be URL-encoded.
        CHXString strUnescapedOptionValue;
        CHXURL::decodeURL(pszValue, strUnescapedOptionValue);
        pszValue = (char *)(const char *)strUnescapedOptionValue;

	// save to the header
	if (!strcasecmp("Start", pszKey)
	    || !strcasecmp("End", pszKey)
	    || !strcasecmp("Delay", pszKey)
	    || !strcasecmp("Duration", pszKey))
	{
	    m_pOptions->SetPropertyULONG32(pszKey, (ULONG32) ::TimeParse(pszValue) * 100);
	}
	else if (bValueQuoted || !IsNumber(pszValue))
	{
	    IHXBuffer*	pBuffer = NULL;

	    if (m_pCCF)
	    {
		m_pCCF->CreateInstance(CLSID_IHXBuffer, (void**)&pBuffer);
	    }

	    if (!pBuffer)
	    {
		hr = HXR_OUTOFMEMORY;
		goto cleanup;
	    }
	    pBuffer->Set((UCHAR*)pszValue, strlen(pszValue)+1);

	    m_pOptions->SetPropertyBuffer(pszKey, (IHXBuffer*) pBuffer);

	    pBuffer->Release();
	}
	else
	{
	    m_pOptions->SetPropertyULONG32(pszKey, (ULONG32) atol(pszValue));
	}
	
	bValueQuoted = FALSE;
	pszKey = NULL;
	pszValue = NULL;
    }
    
cleanup:

    return hr;
}

BOOL
CHXURL::IsTimeValue (char* pszValue)
{
    int	    i = 0;
    char*   pszData = NULL;
    char*   pszTemp = NULL;

    // time value format: hh:mm:ss
    if (isdigit(*pszValue)	&&
	isdigit(*(pszValue+1))	&&
	*(pszValue+2) == ':'	&&
	isdigit(*(pszValue+3))	&&
	isdigit(*(pszValue+4))	&&
	*(pszValue+5) == ':'	&&
	isdigit(*(pszValue+6))	&&
	isdigit(*(pszValue+7)))
    {
	for (i = 0; i < 3; i++)
	{
	    pszData = pszValue;

	    if (i < 2)
	    {
		pszTemp = (char*) ::HXFindChar(pszValue, ':');
		pszTemp = '\0';
	    }

	    switch (i)
	    {
	    case 0: // hh
		if (atoi(pszData) >= 24)
		{
		    return FALSE;
		}
		break;
	    case 1: // mm
		if (atoi(pszData) >= 60)
		{
		    return FALSE;
		}
		break;
	    case 2: // ss
		if (atoi(pszData) >= 60)
		{
		    return FALSE;
		}
		break;
	    default:
		break;
	    }

	    pszValue = pszTemp + 1;
	}

	return TRUE;
    }

    return FALSE;
}    

BOOL
CHXURL::IsNumber(char* pszValue)
{
    char* pszCursor = pszValue;

    while (*pszCursor != '\0')
    {
	if (!isdigit(*pszCursor))
	{
	    return FALSE;
	}

	pszCursor++;
    }

    return TRUE;
}

// case insensitive compare
int
CHXURL::StringNCompare (const char* pszStr1, const char* pszStr2, size_t nChars)
{
#ifdef _WINDOWS
    return strnicmp (pszStr1, pszStr2, nChars);
#elif defined(_MACINTOSH)
    return strnicmp (pszStr1, pszStr2, nChars);
#elif defined(_UNIX) || defined(_OPENWAVE)
    return strncasecmp (pszStr1, pszStr2, nChars);
#elif defined(_SYMBIAN)
    return strnicmp(pszStr1, pszStr2, nChars);
#elif
#   error "undefined platform....."    
#else
    return -1;
#endif
}

void	
CHXURL::AddOption(char* pKey, char* pValue)
{
    // trim off leading/tailing spaces
    TrimOffSpaces(pKey);
    TrimOffSpaces(pValue);
    
    // save to the header
    if (IsNumber(pValue))
    {
	m_pOptions->SetPropertyULONG32(pKey, (ULONG32) atol(pValue));
    }
    else
    {
	IHXBuffer*  pBuffer = NULL;

	if (m_pCCF)
	{
	    m_pCCF->CreateInstance(CLSID_IHXBuffer, (void**)&pBuffer);
	}

	if (pBuffer)
	{
	    pBuffer->Set((UCHAR*)pValue, strlen(pValue)+1);

	    m_pOptions->SetPropertyBuffer(pKey, (IHXBuffer*) pBuffer);

	    pBuffer->Release();
	}
    }
}

void	
CHXURL::AddOption(char* pKey, UINT32 ulValue)
{
    TrimOffSpaces(pKey);
    m_pOptions->SetPropertyULONG32(pKey, ulValue);
}

IHXValues*
CHXURL::GetProperties(void)
{
    if (m_pProperties)
    {
	m_pProperties->AddRef();
    }

    return m_pProperties;
}

IHXValues*
CHXURL::GetOptions(void)
{
    if (m_pOptions)
    {
	m_pOptions->AddRef();
    }

    return m_pOptions;
}

char*
CHXURL::GetAltURL(BOOL& bDefault)
{
    IHXBuffer*	pValue = NULL;
    char*	pAltURL = NULL;
    char*	pURL = NULL;
    char*	pCursor1 = NULL;
    char*	pCursor2 = NULL;

    bDefault = FALSE;

    if (HXR_OK != m_LastError)
    {
	goto cleanup;
    }

    // retrieve Alt-URL if it exists in the option list
    if (HXR_OK == m_pOptions->GetPropertyBuffer("altURL", pValue) && pValue)
    {	    
	// allocate mem. for m_pszAltURL
	pAltURL = new char[pValue->GetSize()];
	SafeStrCpy(pAltURL, (const char*)pValue->GetBuffer(), pValue->GetSize());
    }
    else if (HXR_OK == m_pProperties->GetPropertyBuffer("url", pValue) && pValue)
    {	
	if (m_unProtocol == pnmProtocol ||
	    m_unProtocol == rtspProtocol)
	{
	    bDefault = TRUE;
	    
	    // The +1 is NOT for the NULL terminator since the size already has space for it.  The +1 is actually because
	    // if the URL contains a pnm:// the code below will potentially add a http:// which is one more character than
	    // a pnm URL.  A rtsp:// URL will work correctly 
            INT32 lSize = pValue->GetSize() + 1;
	    pAltURL = new char[lSize];
	    memset(pAltURL, 0, lSize);

	    pURL = (char*)pValue->GetBuffer();
	    
	    switch (m_unProtocol)
	    {
	    case pnmProtocol:				
		SafeSprintf(pAltURL, lSize, "http:%s", pURL + 4);
		break;
	    case rtspProtocol:		
		SafeSprintf(pAltURL, lSize, "http:%s", pURL + 5);
		break;
	    default:
		break;
	    }

	    // exclude the port from the URL
	    if (m_pszPort)
	    {
		pCursor1 = strstr(pAltURL, m_pszPort);
                pCursor2 = pCursor1 + strlen(m_pszPort);
                pCursor1--; // back up over the ':'
                
                while(*pCursor2)
                {
                    *pCursor1++ = *pCursor2++;
                }

                *pCursor1 = '\0';
	    }
	}
    }

cleanup:

    HX_RELEASE(pValue);

    return pAltURL;
}

void
CHXURL::ParseResource(void)
{
    char* pszTemp = NULL;
    char* pResource = NULL;
    char* pPath = NULL;
    char* pFullPath = NULL;

    if (m_pszResource && (*m_pszResource != '\0'))
    {
	::SaveStringToHeader(m_pProperties, PROPERTY_RESOURCE, m_pszResource);

	::StrAllocCopy(pResource, m_pszResource);
	
	pszTemp = ::HXFindChar(pResource, '?');
	if (pszTemp)
	{
	    *pszTemp = '\0';
	}

        pFullPath = new char[strlen(pResource) + 2];
	SafeSprintf(pFullPath, strlen(pResource)+2, "/%s", pResource);
	::SaveStringToHeader(m_pProperties, PROPERTY_FULLPATH, pFullPath);

	pszTemp = ::HXReverseFindChar(pResource, '/');
	if (pszTemp)
	{
	    *pszTemp = '\0';
	    
	    pPath = new char[strlen(pResource)+2];
	    SafeSprintf(pPath, strlen(pResource)+2, "/%s", pResource);

	    ::SaveStringToHeader(m_pProperties, PROPERTY_PATH, pPath);
	}
	else
	{
	    ::SaveStringToHeader(m_pProperties, PROPERTY_PATH, "/");
	}

	HX_VECTOR_DELETE(pFullPath);
	HX_VECTOR_DELETE(pPath);
	HX_VECTOR_DELETE(pResource);
    }
    else if (m_unProtocol == rtspProtocol)
    {
	::SaveStringToHeader(m_pProperties, PROPERTY_RESOURCE, "\0");
	::SaveStringToHeader(m_pProperties, PROPERTY_FULLPATH, "\0");
	::SaveStringToHeader(m_pProperties, PROPERTY_PATH, "\0");
    }
}


HX_RESULT
CHXURL::GeneratePrefixRootFragment(const char* pURL, CHXString& urlPrefix, 
				   CHXString& urlRoot, char*& pURLFragment)
{
    BOOL bHasHost = FALSE;

    CHXURL urlObj(pURL);
    IHXValues* pHeader = urlObj.GetProperties();
    if(!pHeader)
    {
	return HXR_FAIL;
    }

    IHXBuffer* pBuffer = 0;
    ULONG32 ulTemp;

    if(HXR_OK == pHeader->GetPropertyBuffer(PROPERTY_SCHEME, pBuffer))
    {
	urlPrefix = (const char*)pBuffer->GetBuffer();
	urlPrefix += "://";
	pBuffer->Release();
    }
    if(HXR_OK == pHeader->GetPropertyBuffer(PROPERTY_HOST, pBuffer))
    {
	urlPrefix += (const char*)pBuffer->GetBuffer();
	pBuffer->Release();
	bHasHost = TRUE;
    }
    if(HXR_OK == pHeader->GetPropertyULONG32(PROPERTY_PORT, ulTemp))
    {
 	char szTemp[10]; /* Flawfinder: ignore */
	SafeSprintf(szTemp, sizeof(szTemp), ":%d", (UINT16)ulTemp);
	urlPrefix += szTemp;
    }

    // set root
    urlRoot = urlPrefix;

    if(bHasHost)
    {
	urlPrefix += "/";
    }

	if(HXR_OK == pHeader->GetPropertyBuffer(PROPERTY_RESOURCE, pBuffer))
    {
	const char* pResource = (const char*)pBuffer->GetBuffer();
	const char  cDelimiter1  = '/';
	const char  cDelimiter2  = '\\';
	const char  cOSDelimiter = OS_SEPARATOR_CHAR;
	
	CHXString   strURLWork = pResource; 

	char* pFirstChar    = strURLWork.GetBuffer(strURLWork.GetLength());
	char* pLastChar	    = NULL;
	char* pOptions	    = NULL;

	pOptions = strchr(pFirstChar, '?');

	if (pOptions)
	{
	    pLastChar = pOptions -1;
	}
	else
	{
	    pLastChar = pFirstChar + strlen(pFirstChar)-1;
	}

	while ((pLastChar > pFirstChar) && 
	   (*pLastChar != cDelimiter1) && (*pLastChar != cDelimiter2) &&
	   (*pLastChar != cOSDelimiter))
	{
		pLastChar--;
	}
	// If we hit a delimiter before hitting the end, back up one character!
	if(pLastChar > pFirstChar) 
	{
	    *(++pLastChar) = '\0';
	    
	    urlPrefix += pFirstChar;
	}
	pBuffer->Release();
    }

    if(HXR_OK == pHeader->GetPropertyBuffer(PROPERTY_FRAGMENT, pBuffer))
    {
	const char* pFragment = (const char*)pBuffer->GetBuffer();
	pURLFragment = new_string(pFragment);
	pBuffer->Release();
    }

    HX_RELEASE(pHeader);

    return HXR_OK;
}

BOOL 
CHXURL::CompressURL(const char* pURL, char*& pCompressURL)
{
    HX_ASSERT(pURL != NULL);

    if (!pURL)
    {
	return FALSE;
    }

    pCompressURL	    = NULL;

    BOOL bNeedToCompress    = FALSE; 
    char separator1	    = '\\';
    char separator2	    = '/';
    char* pWalker	    = (char*) pURL;

    while (*pWalker)
    {
	/* 
	 *   /./ || /.\ || \./ || \.\ || 
	 *   /../ || /..\ || \../ || \..\
	 */
	if ((*pWalker	    == separator1 || *pWalker == separator2) &&
	    (*(pWalker+1)   == '.') &&
	    ((*(pWalker+2)   == separator1 || *(pWalker+2) == separator2) ||
	     ((*(pWalker+2)   == '.') &&
	     (*(pWalker+3)   == separator1 || *(pWalker+3) == separator2))))
	{
	    // we need to commpress it
	    bNeedToCompress = TRUE;
	    break;
	}
	
	/* Do not process options in the URL (stuff after ?) */
	if (*pWalker == '?')
	{
	    break;
	}

	pWalker++;
    }

    if (!bNeedToCompress)
    {
	return FALSE;
    }

    UINT32 ulURLLength;
    char* pTempURL;
    char* pOptions;

    ulURLLength = strlen(pURL) + 1;
    pTempURL = new char[ulURLLength];
    ::strcpy(pTempURL, pURL); /* Flawfinder: ignore */

    pOptions = strchr(pTempURL, '?');

    /* We will only compress till before the options and then paste the options
     * at the end
     */
    if (pOptions)
    {
	*pOptions = '\0';
    }

    CHXSimpleList* pList;
    char* pToken;
    UINT16 uNumToBeDiscarded;
    CHXSimpleList* pNewList;
    
    pList    = new CHXSimpleList;
    pNewList = new CHXSimpleList;
    uNumToBeDiscarded = 0;

    pWalker = pToken = pTempURL;    
    while (*pWalker)
    {
	if (*pWalker == '/' || *pWalker == '\\')
	{
	    *pWalker = '\0';
	    pList->AddTail(pToken);
	    pToken = pWalker+1;
	}
	pWalker++;
    }

    pList->AddTail(pToken);

    while (pList->GetCount() > 0)
    {
	pToken = (char*) pList->RemoveTail();
	if (::strcmp(pToken, ".") == 0)
	{
	    /* ignore it */    
	}
	else if (::strcmp(pToken, "..") == 0)
	{
	    uNumToBeDiscarded++;
	}
	else if (uNumToBeDiscarded > 0)
	{
	    uNumToBeDiscarded--;
	}
	else
	{
	    pNewList->AddTail(pToken);
	}
    }

// /Valid content that starts with "../../" will trigger this, so turn it off
// unless someone wants to refine it:
#if defined(ALLOW_IRRITATING_ASSERTS_FOR_VALID_CONTENT)
    // This will trigger with malformed urls with two additional ellipses(..)
    HX_ASSERT(uNumToBeDiscarded == 0 && pNewList->GetCount() > 0);
#endif

    if (uNumToBeDiscarded > 0 || pNewList->GetCount() == 0)
    {
	bNeedToCompress = FALSE;
	goto exit;
    }

    pCompressURL = new char[ulURLLength];
    *pCompressURL = '\0';
    while (pNewList->GetCount() > 0)
    {
	pToken = (char*) pNewList->RemoveTail();	
	SafeStrCat(pCompressURL, (const char*) pToken, ulURLLength);

	if (!pNewList->IsEmpty())
	{
	    SafeStrCat(pCompressURL, "/", ulURLLength);
	}
    }

    if (pOptions)
    {
	SafeStrCat(pCompressURL, "?", ulURLLength);
	SafeStrCat(pCompressURL, (const char*) (pOptions+1), ulURLLength);
    }

exit:
    HX_VECTOR_DELETE(pTempURL);
    HX_DELETE(pList);
    HX_DELETE(pNewList);
    return bNeedToCompress;
}

HX_RESULT
CHXURL::encodeURL(const char* pURL, CHXString& encodedURL)
{
    HX_RESULT rc = HXR_OK;

    char    hexBuf[3] = {0}; /* Flawfinder: ignore */
    char*   pEncodeBuf = new char[(strlen(pURL)+1)*3];    // overkill
    char*   pEncodePtr = pEncodeBuf;
    const   char* pURLPtr = pURL;

    while(*pURLPtr)
    {
    	// according to the URL encoding spec. from
	// http://www.isi.edu/in-notes/rfc1738.txt
	if (*pURLPtr <= 0x1f		||
	    *pURLPtr >= 0x7f		||
	    HXIsEqual(pURLPtr, ' ')	||
//	    HXIsEqual(pURLPtr, '<')	||
//	    HXIsEqual(pURLPtr, '>')	||
//	    HXIsEqual(pURLPtr, '"')	||
//	    HXIsEqual(pURLPtr, '#')	||
//	    HXIsEqual(pURLPtr, '%')	||
	    HXIsEqual(pURLPtr, '{')	||
	    HXIsEqual(pURLPtr, '}')	||
	    HXIsEqual(pURLPtr, '|')	||
	    HXIsEqual(pURLPtr, '\\')	||
	    HXIsEqual(pURLPtr, '^')	||
	    HXIsEqual(pURLPtr, '~')	||
	    HXIsEqual(pURLPtr, '[')	||
	    HXIsEqual(pURLPtr, ']')	||
	    HXIsEqual(pURLPtr, '`')	||
	    HXIsEqual(pURLPtr, ',')	||
	    HXIsEqual(pURLPtr, ';'))
	{	    
	    SafeSprintf(hexBuf, sizeof(hexBuf), "%02x", (UCHAR)*pURLPtr);
	    *pEncodePtr++ = '%';
	    *pEncodePtr++ = hexBuf[0];
	    *pEncodePtr++ = hexBuf[1];
	    
	    if (HXIsLeadByte(*pURLPtr))
	    {
		SafeSprintf(hexBuf, sizeof(hexBuf), "%02x", (UCHAR)*(pURLPtr+1));
		*pEncodePtr++ = '%';
		*pEncodePtr++ = hexBuf[0];
		*pEncodePtr++ = hexBuf[1];
	    }	    
	}
	else
	{
	    *pEncodePtr++ = *pURLPtr;
	}

	pURLPtr = HXGetNextChar(pURLPtr) ;
    }
    *pEncodePtr = '\0';

    encodedURL = pEncodeBuf;
    delete[] pEncodeBuf;

    return rc;
}

HX_RESULT
CHXURL::decodeURL(const char* pURL, CHXString& decodedURL)
{
    HX_RESULT rc = HXR_OK;

    //XXXBAB - reimplement using CHXString::GetBuffer()/SetBuffer()
    // to avoid memcpy

    char* pDecodeBuf = new char[strlen(pURL)+1];
    char* pDecodePtr = pDecodeBuf;
    const char* pURLPtr = pURL;

    while(*pURLPtr)
    {
	switch(*pURLPtr)
	{
	    case '%':
	    {
		char hexBuf[3]; /* Flawfinder: ignore */
		if(pURLPtr[1] &&    // check for overbound condition
		   pURLPtr[2])
		{
		    pURLPtr++;  // walk past '%'
		    hexBuf[0] = *pURLPtr++;
		    hexBuf[1] = *pURLPtr;
		    hexBuf[2] = '\0';
		    *pDecodePtr++ = (char)strtol(hexBuf, NULL, 16);
		}
	    }
	    break;

	    default:
	    {
		*pDecodePtr++ = *pURLPtr;
	    }
	    break;
	}
	pURLPtr++;
    }
    *pDecodePtr = '\0';

    decodedURL = pDecodeBuf;
    delete[] pDecodeBuf;

    return rc;
}

const char* CHXURL::FindURLSchemeEnd(const char *pszURL)
{
    const char *pszTemp;

    for (pszTemp = pszURL; *pszTemp; pszTemp++)
    {
	if(*pszTemp == ':')
	{
	    return pszTemp;
	}
	else if(*pszTemp == '$' ||
		*pszTemp == '#' ||
		*pszTemp == '?' ||
		*pszTemp == '/' ||
		*pszTemp == '\\')
	{
	    return NULL;
	}
    }
    
    return NULL;
}

void
CHXURL::Unescape(char* s)
{
/*
 * Remove URL hex escapes from s... done in place.  The basic concept for
 * this routine is borrowed from the WWW library HTUnEscape() routine.
 */
    char* p = NULL;
    BOOL bProcessingOptionsPastQuestionMark = FALSE;

    for (p = s; *s != '\0'; ++s)
    {
        if ( (!bProcessingOptionsPastQuestionMark) && (*s == '%') )
	{
	    if (*++s != '\0') 
	    {
		*p = Unhex( *s ) << 4;
	    }

	    if (*++s != '\0') 
	    {
		*p++ += Unhex( *s );
	    }
	} 
	else 
	{
            if (*s == '?')
            {
                bProcessingOptionsPastQuestionMark = TRUE;
            }
	    *p++ = *s;
	}
    }

    *p = '\0';
}

int
CHXURL::Unhex(char c)
{
    return (c >= '0' && c <= '9' ? c - '0'
	    : c >= 'A' && c <= 'F' ? c - 'A' + 10
	    : c - 'a' + 10 );
}

void 
CHXURL::TestCompressURL()
{
#ifdef _DEBUG
    /* Brad made me do it...:( */
    char* pTestCompressURL = NULL;
    BOOL bTestCompress = FALSE;
    CHXURL url("blah");
    
    HX_VERIFY(url.CompressURL("http://blah.real.com/test/../foo.smi", pTestCompressURL) == TRUE &&
	      ::strcmp(pTestCompressURL, "http://blah.real.com/foo.smi") == 0);
    HX_VECTOR_DELETE(pTestCompressURL);

    HX_VERIFY(url.CompressURL("http://blah.real.com/test/foo.smi?foo/../.", pTestCompressURL) == FALSE);
    HX_VECTOR_DELETE(pTestCompressURL);

    HX_VERIFY(url.CompressURL("http://blah.real.com/test/./foo.smi", pTestCompressURL) == TRUE &&
	      ::strcmp(pTestCompressURL, "http://blah.real.com/test/foo.smi") == 0);
    HX_VECTOR_DELETE(pTestCompressURL);

    HX_VERIFY(url.CompressURL("http://blah.real.com/test/.blah/foo.smi", pTestCompressURL) == FALSE);
    HX_VECTOR_DELETE(pTestCompressURL);

    HX_VERIFY(url.CompressURL("http://blah.real.com/test/..foo.smi", pTestCompressURL) == FALSE);
    HX_VECTOR_DELETE(pTestCompressURL);

    HX_VERIFY(url.CompressURL("rtsp://blah.real.com/test/blah/../../foo.smi?end=./../blah", pTestCompressURL)  == TRUE &&
	      ::strcmp(pTestCompressURL, "rtsp://blah.real.com/foo.smi?end=./../blah") == 0);
    HX_VECTOR_DELETE(pTestCompressURL);
#endif
}

IHXCommonClassFactory* CHXURL::CreateCCF()
{
    return  new CHXMiniCCF();
}
