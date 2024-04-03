/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ut_string.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#include "ut_string.h"

#include <string.h>

UTString::UTString() :
    m_pStr(new char[1])
{
    m_pStr[0] = '\0';
}

UTString::UTString(const char* pStr) :
    m_pStr(0)
{
    if (pStr)
    {
	m_pStr = new char[strlen(pStr) + 1];
	strcpy(m_pStr, pStr); /* Flawfinder: ignore */
    }
    else
    {
	m_pStr = new char[1];
	m_pStr[0] = '\0';
    }
}

UTString::UTString(const char* pStr, int length) :
    m_pStr(0)
{
    if (pStr)
    {
	m_pStr = new char[length + 1];
	strncpy(m_pStr, pStr, length); /* Flawfinder: ignore */
	m_pStr[length] = '\0';
    }
    else
    {
	m_pStr = new char[1];
	m_pStr[0] = '\0';
    }
}

UTString::UTString(const UTString& rhs) :
    m_pStr(new char[rhs.length() + 1])
{
    ::strcpy(m_pStr, rhs.m_pStr); /* Flawfinder: ignore */
}

UTString::~UTString()
{
    delete [] m_pStr;
    m_pStr = 0;
}

UTString::operator const char*() const
{
    return m_pStr;
}

UTString& UTString::operator=(const UTString& rhs)
{
    if (&rhs != this)
    {
	delete [] m_pStr;

	m_pStr = new char[rhs.length() + 1];
	::strcpy(m_pStr, rhs.m_pStr); /* Flawfinder: ignore */
    }
    
    return *this;
}

bool UTString::operator==(const char* pStr) const
{
    return (strcmp(m_pStr, pStr) == 0);
}

bool UTString::operator==(const UTString& rhs) const
{
    return (strcmp(m_pStr, rhs.m_pStr) == 0);
}

bool UTString::operator!=(const char* pStr) const
{
    return (strcmp(m_pStr, pStr) != 0);
}

bool UTString::operator!=(const UTString& rhs) const
{
    return (strcmp(m_pStr, rhs.m_pStr) != 0);
}

int UTString::length() const
{
    return strlen(m_pStr);
}

