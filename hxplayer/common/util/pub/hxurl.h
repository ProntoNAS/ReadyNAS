/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxurl.h,v 1.1.1.1 2006/03/29 16:45:32 hagi Exp $
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

/*******************************************************************************************
    CHXURL.H
  
    A class to parse and store the contents of an URL

    Usage:
	
    - The constructor parses the url.
    - The various components of the url may be accessed via their
      respective access methods or via get_event, for events
    - Use send_client_events to stuff all the client events in the 
      outgoing event queue

*******************************************************************************************/


#ifdef __MWERKS__
#pragma once
#endif

#ifndef _CHXURL_H_
#define	_CHXURL_H_

#include "hxcom.h"

// option signal/separator
#define OPTION_SIGNAL		'?'
#define OPTION_SEPARATOR	'&'
#define OPTION_ASSIGNMENT	'='

// URL properties
#define PROPERTY_URL		"url"
#define PROPERTY_PROTOCOL	"protocol"
#define PROPERTY_SCHEME		"scheme"
#define PROPERTY_HOST		"host"
#define PROPERTY_PORT		"port"
#define PROPERTY_PATH		"path"
#define PROPERTY_FULLPATH       "fullpath"
#define PROPERTY_RESOURCE	"resource"
#define PROPERTY_FRAGMENT	"fragment"
#define PROPERTY_USERNAME	"username"
#define PROPERTY_PASSWORD	"password"

enum 
{
    unknownProtocol = 0,
    httpProtocol,
    pnmProtocol,
    rtspProtocol,
    fileProtocol,
    fnordProtocol,
    httpsProtocol,
    helixSDPProtocol
};

typedef _INTERFACE IHXCommonClassFactory IHXCommonClassFactory;
typedef _INTERFACE IHXValues IHXValues;

class CHXURL
{
public:
    CHXURL (const char* /* IN */ pszURL);
    CHXURL (const char* /* IN */ pszURL, IUnknown* pContext);
    ~CHXURL ();

    CHXURL(const CHXURL& rhs);
    CHXURL& operator=(const CHXURL& rhs);


    // called to retrieve URL properties/options
    IHXValues*	GetProperties(void);
    IHXValues*	GetOptions(void);

    // determine if it needs network protocol
    BOOL	IsNetworkProtocol() {return m_bNetworkProtocol;}

    UINT16	GetProtocol() const { return m_unProtocol; };

    HX_RESULT	GetLastError() {return m_LastError;}
    
    void	AddOption(char* pKey, char* pValue);
    void	AddOption(char* pKey, UINT32 ulValue);

    char*	GetAltURL(BOOL& bDefault);
    const char*	GetURL() const {return (const char*) m_pActualURL;};
    const char*	GetEscapedURL() const {return (const char*) m_pszEscapedURL;};

    static HX_RESULT GeneratePrefixRootFragment(
				const char* pURL,   CHXString& urlPrefix, 
				CHXString& urlRoot, char*& pURLFragment);

    BOOL	     CompressURL(const char* pURL, char*& pCompressURL);

    static HX_RESULT encodeURL(const char* pURL, CHXString& encodedURL);    
    static HX_RESULT decodeURL(const char* pURL, CHXString& unencodedURL);

    static const char* FindURLSchemeEnd(const char *pszURL);
    static BOOL IsURLRelative(const char *pszURL)
    {
	return (FindURLSchemeEnd(pszURL) == NULL);
    }

    static void	     TestCompressURL();

private:
    void ConstructURL(const char* pszURL);

    static IHXCommonClassFactory* CreateCCF();

    HX_RESULT	m_LastError;

    // URL options/values
    char*	m_pActualURL;
    char*       m_pszURL;
    char*	m_pszEscapedURL;
    char*	m_pszOptions;

    char*	m_pszHost;
    char*	m_pszPort;
    char*	m_pszUsername;
    char*	m_pszPassword;
    UINT16	m_unProtocol;
    UINT16      m_unDefaultPort;
    HX_BITFIELD	m_bNetworkProtocol : 1;
    char*	m_pszResource;

    IHXValues*	m_pProperties;
    IHXValues*	m_pOptions;
    IHXCommonClassFactory* m_pCCF;

    // help functions
    void	TrimOffSpaces (char*& /* IN OUT */ pszString);

    int		StringNCompare (const char* /* IN */ pszStr1, 
			        const char* /* IN */ pszStr2,
				size_t	    /* IN */ nChars);
    
    HX_RESULT	ParseURL (char* /* IN */ pszURL);

    HX_RESULT	CollectOptions (char* /* IN */ pszOptions);


    BOOL	IsTimeValue (char* /* IN */ pszValue);
    BOOL	IsNumber (char* /* IN */    pszValue);

    void	ParseResource(void);

    void	Unescape(char* s);
    int		Unhex(char c);
};

#endif	// _CHXURL_H_
