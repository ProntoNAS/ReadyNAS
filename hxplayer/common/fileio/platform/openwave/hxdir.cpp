/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxdir.cpp,v 1.1.1.1 2006/03/29 16:45:37 hagi Exp $
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

#include "hlxclib/sys/stat.h"
#include "hlxclib/errno.h"

//#include <sys/dir.h>

#include "findfile.h"
#include "hxdir.h"
#include "hxstrutl.h"

#include "hxheap.h"
#ifdef _DEBUG
#undef HX_THIS_FILE		
static const char HX_THIS_FILE[] = __FILE__;
#endif

CHXDirectory::CHXDirectory()
    : m_pFileFinder(NULL)
{
}

CHXDirectory::~CHXDirectory()
{
    HX_DELETE(m_pFileFinder);
}

// Locate a writable spot for a temp directory.
// This is done by checking a number of possible locations
// in the following order of preference:
//
// 1) the current working directory
// 2) "/tmp"

static BOOL
isWritable(const char* szPath)
{
	return FALSE;
}

BOOL
CHXDirectory::SetTempPath(HXXHANDLE /* hpsHandle */, const char* szRelPath)
{
    return FALSE;
}

/* Creates directory. */
BOOL 
CHXDirectory::Create()
{
    return FALSE;
}

/* Checks if directory exists. */    
BOOL 
CHXDirectory::IsValid()
{
    return FALSE;
}

/* Deletes empty directory. */
BOOL 
CHXDirectory::DeleteDirectory()
{
    return FALSE;
}

/* Starts enumeration process. */
CHXDirectory::FSOBJ 
CHXDirectory::FindFirst(const char* szPattern, char* szPath, UINT16 nSize)
{
    FSOBJ RetVal = FSOBJ_NOTVALID;
    return RetVal;
}

/* Continues enumeration process. */
CHXDirectory::FSOBJ 
CHXDirectory::FindNext(char* szPath, UINT16 nSize)
{
    FSOBJ RetVal = FSOBJ_NOTVALID;
    return RetVal;
}

BOOL 
CHXDirectory::DeleteFile(const char* szRelPath)
{
    BOOL RetVal = FALSE;
    return RetVal;
}

/* Sets itself to current directory. */
BOOL 
CHXDirectory::SetCurrentDir()
{
    BOOL bRetVal = FALSE;
    return bRetVal;
}

/* Makes itself a current directory. */
BOOL 
CHXDirectory::MakeCurrentDir()
{
    return FALSE;
}


UINT32 
CHXDirectory::Rename(const char* szOldName, const char* szNewName)
{
    return (UINT32)HXR_FAIL;
}

BOOL 
CHXDirectory::IsValidFileDirName(const char* szPath)
{
   return FALSE;
}
