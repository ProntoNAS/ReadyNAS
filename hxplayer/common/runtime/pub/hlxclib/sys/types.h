/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: types.h,v 1.1.1.1 2006/03/29 16:45:35 hagi Exp $
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

#ifndef HLXSYS_SYS_TYPES_H
#define HLXSYS_SYS_TYPES_H

#if !defined(WIN32_PLATFORM_PSPC) && !defined(_MACINTOSH) && !defined(_OPENWAVE)
#include <sys/types.h>
#endif /* !defined(WIN32_PLATFORM_PSPC) */

#if defined (_WINDOWS) || (defined (_MACINTOSH) && !defined(_MAC_MACHO))
typedef long off_t;
#endif /* defined (_WINDOWS) || defined (_MACINTOSH) */

#if defined(_OPENWAVE)
#include "hlxclib/time.h"
#include "platform/openwave/hx_op_fs.h"
typedef unsigned short mode_t;
typedef OpFsSize off_t;         // XXXSAB???

#ifndef SEEK_SET
#define SEEK_SET kOpFsSeekSet
#define SEEK_CUR kOpFsSeekCur
#define SEEK_END kOpFsSeekEnd
#endif

struct stat
{
    // XXXSAB fill this in...
    mode_t st_mode;
    off_t st_size;
    time_t st_atime;
    time_t st_ctime;
    time_t st_mtime;
    short st_nlink;
};

#elif defined(WIN32_PLATFORM_PSPC)
#include "hlxclib/windows.h"

typedef unsigned short mode_t;

#define S_IFDIR 0040000

struct stat {
    mode_t st_mode;
    off_t st_size;
    time_t st_atime;
    time_t st_ctime;
    time_t st_mtime;
    short st_nlink;
};

#endif /* defined(WIN32_PLATFORM_PSPC) */

#endif /* HLXSYS_SYS_TYPES_H */
