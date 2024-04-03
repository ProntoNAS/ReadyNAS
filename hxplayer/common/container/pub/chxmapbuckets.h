/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: chxmapbuckets.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef _CHLXMAPBUCKETS_H_
#define _CHLXMAPBUCKETS_H_

#include "hxmaputils.h"

class CHlxMapBuckets
{
public:
    typedef HlxMap::IntVec_t ITEM;

    CHlxMapBuckets() : m_items(0), m_size(0)
    {
    }

    CHlxMapBuckets(UINT16 num) : m_items(0), m_size(0)
    {
        // Nasty new in constructor makes it hard to detect OOM errors,
        // but I don't think this constructor is actually used by anybody.
        m_items = new ITEM[num];
        m_size = num;
    }

    ~CHlxMapBuckets() { HX_VECTOR_DELETE(m_items); }

    inline ITEM& operator[] (int idx)
    {
        return m_items[idx];
    }

    inline const ITEM& operator[] (int idx) const
    {
        return m_items[idx];
    }

    inline bool empty () const { return !m_items; }

    inline UINT16 size() const { return m_size; }

    inline HX_RESULT Init (UINT16 num)
    {
        HX_VECTOR_DELETE(m_items);
        m_items = new ITEM[num];
        if( !m_items )
        {
            return HXR_OUTOFMEMORY;
        }
        m_size = num;
        return HXR_OK;
    }

private:
    ITEM*       m_items;
    UINT16      m_size;
};

#endif // _CHLXMAPBUCKETS_H_
