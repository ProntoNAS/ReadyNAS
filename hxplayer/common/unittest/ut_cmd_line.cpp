/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ut_cmd_line.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#include "ut_cmd_line.h"
#include <string.h>

UTCommandLine::UTCommandLine() :
    m_argc(0),
    m_argv(0),
    m_cmdline(0)
{}

UTCommandLine::~UTCommandLine()
{
    delete [] m_argv;
    m_argv = 0;

    delete [] m_cmdline;
    m_cmdline = 0;
}

bool UTCommandLine::Parse(const char* pCmdLine)
{
    bool ret = false;

    // Reset object state
    delete [] m_argv;
    delete [] m_cmdline;
    m_argc = 0;
    m_argv = 0;

    m_cmdline = new char[strlen(pCmdLine) + 1];

    if (m_cmdline)
    {
	const char* pSrc = pCmdLine;
	char* pDest = m_cmdline;

	while(*pSrc)
	{
	    // Skip whitespace
	    for (; *pSrc == ' '; pSrc++)
		;
	    
	    if (*pSrc)
	    {
		bool inQuotes = false;
		char* pStart = pDest;

		while(*pSrc && (inQuotes || (*pSrc != ' ')))
		{
		    if (*pSrc == '\"')
			inQuotes = !inQuotes;
		    else if (*pSrc == '\\')
			*pDest++ = *pSrc++; // copy '\\' character

		    if (*pSrc)
			*pDest++ = *pSrc++; // copy current character
		}

		*pDest++ = '\0'; // Null terminate the parameter
		
		AddParam(pStart); // Add this parameter to m_argv
	    }
	}

	if (m_argc > 0)
	    ret = true;	
    }

    return ret;
}

int UTCommandLine::Argc() const
{
    return m_argc;
}

char** UTCommandLine::Argv()
{
    return m_argv;
}

void UTCommandLine::AddParam(char* pParam)
{
    char** pNewArgv = new char*[m_argc + 1];

    // Copy the existing params
    for (int i = 0; i < m_argc; i++)
	pNewArgv[i] = m_argv[i];

    // Add the new param to the end
    pNewArgv[m_argc] = pParam;

    // Update m_argc
    m_argc++;

    // Update m_argv
    delete [] m_argv;
    m_argv = pNewArgv;
}
