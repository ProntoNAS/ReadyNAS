/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: riffres.h,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#ifndef __RIFFRES_H__
#define __RIFFRES_H__

class CRIFFResponse
{
public:
    STDMETHOD(RIFFOpenDone)(HX_RESULT) PURE;

    STDMETHOD(RIFFCloseDone)(HX_RESULT) PURE;

    /* RIFFFindChunkDone is called when a FindChunk completes.
     * len is the length of the data in the chunk (i.e. Read(len)
     * would read the whole chunk)
     */
    STDMETHOD(RIFFFindChunkDone)(HX_RESULT status, UINT32 len) PURE;

    /* Called after a Descend completes */
    STDMETHOD(RIFFDescendDone)(HX_RESULT) PURE;

    /* Called after an Ascend completes */
    STDMETHOD(RIFFAscendDone)(HX_RESULT) PURE;

    /* Called when a read completes.  IHXBuffer contains the data read
     */
    STDMETHOD(RIFFReadDone)(HX_RESULT, IHXBuffer* pBuffer) PURE;

    /* Called when a seek completes */
    STDMETHOD(RIFFSeekDone)(HX_RESULT) PURE;

    /* Called with the data from a GetChunk request */
    STDMETHOD(RIFFGetChunkDone)(HX_RESULT, UINT32, IHXBuffer*) PURE;

    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG32,AddRef) (THIS) PURE;
    STDMETHOD_(ULONG32,Release) (THIS) PURE;
};

#endif  /* __RIFFRES_H_ */
