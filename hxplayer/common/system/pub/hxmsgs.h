/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxmsgs.h,v 1.1.1.1 2006/03/29 16:45:36 hagi Exp $
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

#ifndef _HXMSGS_
#define _HXMSGS_

#ifdef _WIN32
#define HXMSG_ASYNC_TIMER	WM_TIMER	/*Async Timer Notification */
#define HXMSG_ASYNC_CALLBACK	(WM_USER + 100) /*Async DNS Notification*/
#define HXMSG_ASYNC_DNS	(WM_USER + 101) /*Async DNS Notification*/
#define HXMSG_ASYNC_CONNECT	(WM_USER + 102) /*Async Connect Notification*/
#define HXMSG_ASYNC_READ	(WM_USER + 103) /*Async Read Notification*/
#define HXMSG_ASYNC_WRITE	(WM_USER + 104)	/*Async Write Notification*/
#define HXMSG_ASYNC_DETACH	(WM_USER + 105)	/*Async Detach Notification*/
#define HXMSG_ASYNC_NETWORKIO	(WM_USER + 106)	/*Read/Write*/
#define HXMSG_ASYNC_RESUME	(WM_USER + 107)	/* Resume network thread timer*/
#define HXMSG_ASYNC_STOP	(WM_USER + 108)	/* Stop network thread timer*/
#define HXMSG_ASYNC_ACCEPT	(WM_USER + 109)	/*Async Accept Notification */
#define HXMSG_ASYNC_SETREADER_CONNECTION	(WM_USER + 111)	/*local loopback reader accept completed */
#define HXMSG_ASYNC_START_READERWRITER	(WM_USER + 112)	/*local loopback starting */


#define HXMSG_QUIT		(WM_USER + 200)	/* Exit from the thread */

#else /*********** not _WIN32 ***************/

#define HXMSG_ASYNC_CALLBACK	100		/*Async DNS Notification*/
#define HXMSG_ASYNC_DNS	101		/*Async DNS Notification*/
#define HXMSG_ASYNC_CONNECT	102		/*Async Connect Notification*/
#define HXMSG_ASYNC_READ	103		/*Async Read Notification*/
#define HXMSG_ASYNC_WRITE	104		/*Async Write Notification*/
#define HXMSG_ASYNC_DETACH	105		/*Async Detach Notification*/
#define HXMSG_ASYNC_NETWORKIO	106		/*Read/Write*/
#define HXMSG_ASYNC_RESUME	107		/* Resume network thread timer*/
#define HXMSG_ASYNC_STOP	108		/* Stop network thread timer*/
#define HXMSG_ASYNC_ACCEPT	109		/*Async Accept Notification */
#define HXMSG_ASYNC_TIMER	110		/*Async Timer Notification */

#define HXMSG_QUIT		200	    	/* Exit from the thread */


#endif /* _WIN32 */

#endif
