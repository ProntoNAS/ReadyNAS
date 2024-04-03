/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hx_next_line.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#include "hx_next_line.h"
#include <string.h>

static const int InitialLineBufSize = 320;

HLXNextLine::HLXNextLine() : 
    m_name(""),
    m_pFile(0),
    m_pBuf(new char[InitialLineBufSize]),
    m_bufSize(InitialLineBufSize),
    m_lineNum(0)
{}

HLXNextLine::~HLXNextLine()
{
    Close();

    delete [] m_pBuf;
    m_pBuf = 0;
}

bool HLXNextLine::Open(const UTString& name)    
{
    m_name = name;

    m_pFile = (m_name == "") ? stdin : fopen(m_name, "r");

    return m_pFile != 0;
}

void HLXNextLine::Close()
{
    if (m_name != "" && m_pFile)
	fclose(m_pFile);
    m_pFile = 0;
}

bool HLXNextLine::IsOpen() const
{
    return m_pFile != 0;
}

bool HLXNextLine::Reset()
{
    bool ret = m_pFile && fseek(m_pFile, (long) 0, SEEK_SET) != -1;

    if (ret)
	m_lineNum = 0;

    return ret;
}

bool HLXNextLine::GetLine(UTString& line)
{
    int len = 0;
    if (m_pFile)
    {
	if (FGetS(m_pBuf, m_bufSize, m_pFile))
	{
	    line = m_pBuf;
	    len = line.length();
	    m_lineNum++;
	}
    }
    return len > 0;
}

bool HLXNextLine::End() const
{
    return m_pFile == 0 || feof(m_pFile);
}

				// replacement for system fgets that
				// looks for \r or \r\n in addtion to \n
char* HLXNextLine::FGetS(char*& pBuf, int& bufSize, FILE* fp)
{
    bool eol = false;
    int i = 0;

    for (i = 0; !eol; ++i)
    {
	int c = fgetc(fp);

	if (c == EOF)
	    break;

	// Make sure the buffer is large enough
	if ((i + 2) > bufSize)
	{
	    int newSize = bufSize * 2;
	    char* pNewBuf = new char[newSize];
	    ::memcpy(pNewBuf, pBuf, bufSize); /* Flawfinder: ignore */

	    delete [] pBuf;
	    bufSize = newSize;
	    pBuf = pNewBuf;
	}

	pBuf[i] = c;
	
	if (c == '\n')
	    eol = true;
	else if (c == '\r')
	{
	    eol = true;

	    // check for \n and eat it 
	    if ((c = fgetc(fp)) == '\n')
		pBuf[++i] = c;
	    else if (c != EOF)
		ungetc(c, fp);
	    else
		break;
	}
    }

    if (bufSize > 0)
	pBuf[i] = '\0';

    return (i == 0 ? 0 : pBuf);
}
