/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hx_cmd_parser.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#include <ctype.h>
#include <stdio.h>

#include "hx_cmd_parser.h"
#include "hx_char_stack.h"

HLXCommandParser::HLXCommandParser()
{}

inline
static
UTString Trim(const UTString& line)
{
    const char* p = line;
    bool inQuote = false;

    for(; *p && (inQuote || (*p != '#')); p++)
    {
	// If the current char is a '\', we
	// are in a quoted string, and the escaped
	// character is not the null terminator
	// we want to skip over the '\'
	if ((*p == '\\') && inQuote && p[1])
	    p++; // Skip the '\'
	else if (*p == '"')
	    inQuote = !inQuote;
    }

    // Attempt to remove trailing whitespace
    if (p != (const char*)line)
    {
	// Back up one character since p is
	// either the end of the line or it
	// points to the first '#'
	p--;

	// Search back for the last non-whitespace character
	for (; p != (const char*)line && isspace(*p); p--);

	// If the current character is not whitespace,
	// we need to move p forward one so the length
	// computation comes out right
	if (!isspace(*p))
	    p++;
    }
    
    return UTString((const char*) line, p - (const char*)line);
}

bool HLXCommandParser::Parse(const UTString& line)
{
    HLXCharStack tok;
    UTString trimmed = Trim(line);

    m_tokens.Resize(0);
    int ntok = 0;
    const char *p = trimmed;
    while (*p)
    {
	// Consume whitespace
	for (; *p && isspace(*p); p++);
	
	tok.Reset();
	
	if (*p == '"')
	{
	    // Handle quoted strings
	    p++; // skip '"'

	    for (; *p && (*p != '"'); tok++)
	    {
		if (*p == '\\')
		{
		    // Handles escaped characters
		    p++; // skip '\'

		    if (*p)
		    {
			switch (*p) {
			case '0' :
			    *tok = '\0';
			    break;
			case 'n':
			    *tok = '\n';
			    break;
			case 'r':
			    *tok = '\r';
			    break;
			case 't':
			    *tok = '\t';
			    break;
			default:
			    *tok = *p;
			}

			p++;
		    }
		    else
			*tok = '\\';
		}
		else
		    *tok = *p++;
	    }
	    
	    if (*p == '"')
		p++;
	}
	
	for(; *p && !isspace(*p); tok++)
	    *tok = *p++;

	m_tokens.Resize(ntok+1);
	m_tokens[ntok++] = tok.Finish();
    }

    return ntok > 0;
}

bool HLXCommandParser::Valid() const
{
    return m_tokens.Nelements() > 0;
}

void HLXCommandParser::Reset()
{
    m_tokens.Resize(0);
}

const UTString& HLXCommandParser::operator[](int i) const
{
    return m_tokens[i];
}

int HLXCommandParser::Count() const
{
    return m_tokens.Nelements();
}
