/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: cwin32file.h,v 1.1.1.1 2006/03/29 16:45:38 hagi Exp $
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

#ifndef _CWIN32FILE
#define _CWIN32FILE

#include "hxtypes.h"
#include "hxresult.h"
#include "chxdataf.h"

class CWin32File : public CHXDataFile {

public:

    CWin32File                      (void);
    ~CWin32File                      (void);

    virtual BOOL GetTemporaryFileName(const char *tag, char* name, UINT32 ulBufLen);

/*      Create creates a file using the specified mode
        returns HXR_OK or -1 if an error occurred */

    virtual HX_RESULT Create(const char *filename, UINT16 mode, BOOL textflag = 0);

/*      Open will open a file with the specified mode
        returns HXR_OK or -1 if an error occurred */

    virtual HX_RESULT Open(const char *filename, UINT16 mode, BOOL textflag = 0);

/* 	Open will open a file for sharing with the specified permissions 
	returns HXR_OK or -1 if an error occurred */
    virtual HX_RESULT OpenShared(const char *filename, UINT16 mode, 
				 UINT16 sharedmode, BOOL textflag = 0);

/*      Close closes a file */

    virtual HX_RESULT Close(void);

/*      Seek moves the current file position to the offset from the
        fromWhere specifier returns HXR_OK or -1 if an error occurred */
    
    virtual HX_RESULT Seek(ULONG32 offset, UINT16 fromWhere);

/*      Tell returns the current file position in the file */

    virtual ULONG32 Tell(void);

/*      Read reads up to count bytes of data into buf.
        returns the number of bytes read, EOF, or -1 if the read failed */

    virtual ULONG32 Read(char *buf, ULONG32 count);

/*      Write writes up to count bytes of data from buf.
        returns the number of bytes written, or -1 if the write failed */

    virtual ULONG32 Write(const char *buf, ULONG32 count);

/*      Rewind sets the file position to the start of file
        returns HXR_OK or -1 if an error occurred */

    virtual HX_RESULT Rewind(void);
    
/*	Returns the size of the file in bytes */
    virtual ULONG32 GetSize (void);

/*      Changes the size of the file to the newSize */
    virtual HX_RESULT ChangeSize (ULONG32 newSize) ;


/*      Return the file descriptor                      */
    
    virtual INT16 GetFd(void) { return (INT16)mFD; };

/*      Delete deletes a file. */

    virtual HX_RESULT Delete(const char *filename);

private:

    HANDLE mFD;
};

#endif  // _CWIN32FILE

