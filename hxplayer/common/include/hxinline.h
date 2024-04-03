/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxinline.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef _HXINLINE_H_
#define _HXINLINE_H_

/*
 * Define HX_INLINE_ALWAYS to be the keyword that triggers inlining,
 * independent of language (C/C++)
 */

#ifdef __cplusplus
#define HX_INLINE_ALWAYS inline
#else

#if defined(_MSC_VER) || defined(__GNUC__) || defined(ARM_ADS)
#define HX_INLINE_ALWAYS __inline
#elif defined(__MWERKS__)
#define HX_INLINE_ALWAYS inline
#else
#define HX_INLINE_ALWAYS
#endif
#endif /* __cplusplus */

/*
 * Include this file in header files that contain configurable inline methods.
 * For configurable inline functions do the following:
 * 1. Replace inline with HX_INLINE
 * 2. Add function implementation in header file inside of
 *    #if defined (_DEFINE_INLINE) block
 * 3. In one module add #define _DEFINE_INLINE before including the 
 *    header file with the configurable functions
 *
 * ex a.h
 * 
 * Class A
 * {
 *      void F1();
 *      void F2();
 * }
 *
 * // standard inline function
 * inline void A::F1() {some code;}
 *
 * configurable inline function
 *
 * #if defined (_DEFINE_INLINE)
 * HX_INLINE void A::F2() {some code;}
 * #endif
 *
 * a.cpp
 *
 * #define _DEFINE_INLINE
 * #include "a.h"
 */
#if !defined (HX_INLINE)
    #if defined (HELIX_FEATURE_NOINLINE)
        #define HX_INLINE
    #else
        #define _DEFINE_INLINE
        #define HX_INLINE HX_INLINE_ALWAYS
    #endif /* HELIX_FEATURE_NOINLINE */
#endif /* HX_INLINE */


#endif /* _HXINLINE_H_ */
