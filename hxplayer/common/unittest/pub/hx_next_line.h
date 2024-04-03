/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hx_next_line.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef HLX_NEXT_LINE_H
#define HLX_NEXT_LINE_H

#include <stdio.h>

#include "ut_string.h"

class HLXNextLine
{
public:
    HLXNextLine();
    ~HLXNextLine();

    bool Open(const UTString& name="");	 // Open file
    void Close();		// Close file
    bool IsOpen() const;	// Return true if open
    bool Reset();		// If file rewindable seek to beginning
    bool GetLine(UTString& line); // Get next line
    bool End() const;		// return true if at end of file

    int LineNum() const;        // Current line number

protected:
				// replacement for system fgets that
				// looks for \r in addition to \n
    char* FGetS(char*& pBuf, int& size, FILE* fp);

private:
    HLXNextLine(const HLXNextLine& rhs); // Dont implement
    HLXNextLine& operator=(const HLXNextLine&);

    UTString m_name;	// File name: if "" then use stdin
    FILE* m_pFile;		// Use stdio for now
    char* m_pBuf;		// Buffer for reading lines
    int m_bufSize;              // Size of read buffer
    int m_lineNum;              // Current line number
};

inline
int HLXNextLine::LineNum() const
{
    return m_lineNum;
}

#endif // HLX_NEXT_LINE_H
