/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxmime.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

/////
//
// Mime Types definition...
//
/////

#ifndef _HX_MIME_TYPES
#define _HX_MIME_TYPES

#define REALMEDIA_MIME_TYPE		"application/x-pn-realmedia"
#define REALAUDIO_MIME_TYPE		"audio/x-pn-realaudio"
#define REALAUDIO_ENCRYPTED_MIME_TYPE	"audio/x-pn-realaudio-encrypted"
#define REALAUDIO_PD_MIME_TYPE		"audio/x-pn-realaudio-pd"
#define REALVIDEO_MIME_TYPE		"video/x-pn-realvideo"
#define REALVIDEO_ENCRYPTED_MIME_TYPE	"video/x-pn-realvideo-encrypted"
#define IMAGEMAP_MIME_TYPE		"image_map/x-pn-realvideo"
#define IMAGEMAP_ENCRYPTED_MIME_TYPE	"image_map/x-pn-realvideo-encrypted"
#define SYNCMM_MIME_TYPE		"syncMM/x-pn-realvideo"
#define SYNCMM_ENCRYPTED_MIME_TYPE	"syncMM/x-pn-realvideo-encrypted"
#define REALAD_MIME_TYPE		"application/x-pn-realad"
#define REALAD_ENCRYPTED_MIME_TYPE	"application/x-pn-realad-encrypted"
#define AVI_MIME_TYPE			"video/avi"
#define TEXT_MIME_TYPE			"video/text"

#define H263_2000_MIME_TYPE             "video/h263-2000"
#define MP4_V_ES_MIME_TYPE              "video/mp4v-es"
#define MP4_A_LATM_MIME_TYPE            "audio/mp4a-latm"
#define AMR_MIME_TYPE                   "audio/amr"
#define AMR_WB_MIME_TYPE                "audio/amr-wb"

#define REALEVENT_MIME_TYPE		"application/x-pn-realevent"
#define REALEVENT_ENCRYPTED_MIME_TYPE	"application/x-pn-realevent-encrypted"
#define REALIMAGEMAP_MIME_TYPE	"application/x-pn-imagemap"
#define REALIMAGEMAP_ENCRYPTED_MIME_TYPE	"application/x-pn-imagemap-encrypted"

//
// New MIME types used for Fiji stream switching
//
#define REALAUDIO_MULTIRATE_MIME_TYPE	"audio/x-pn-multirate-realaudio"
#define REALAUDIO_ENCRYPTED_MULTIRATE_MIME_TYPE	"audio/x-pn-multirate-realaudio-encrypted"
#define REALAUDIO_MULTIRATE_LIVE_MIME_TYPE	"audio/x-pn-multirate-realaudio-live"
#define REALAUDIO_ENCRYPTED_MULTIRATE_LIVE_MIME_TYPE	"audio/x-pn-multirate-realaudio-live-encrypted"
#define REALVIDEO_MULTIRATE_MIME_TYPE	"video/x-pn-multirate-realvideo"
#define REALVIDEO_ENCRYPTED_MULTIRATE_MIME_TYPE	"video/x-pn-multirate-realvideo-encrypted"
#define REALEVENT_MULTIRATE_MIME_TYPE		"application/x-pn-multirate-realevent"
#define REALEVENT_ENCRYPTED_MULTIRATE_MIME_TYPE		"application/x-pn-multirate-realevent-encrypted"
#define REALIMAGEMAP_MULTIRATE_MIME_TYPE	"application/x-pn-multirate-imagemap"
#define REALIMAGEMAP_ENCRYPTED_MULTIRATE_MIME_TYPE	"application/x-pn-multirate-imagemap-encrypted"

#define MULTIRATE_MIME_TYPE				"multirate-"
#define LOGICAL_MIME_TYPE_PREFIX		"logical-"
#define FILE_INFO_MIME_TYPE				"logical-fileinfo"
#endif //_HX_MIME_TYPES
