/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxcomm.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef _HXCOMM_H_
#define _HXCOMM_H_

#include "hxengin.h" /* For HXTimeval */
#include "hxccf.h" /* For IHXCommonClassFactory. Formerly declared in this file. */

/*
 * Forward declarations of some interfaces defined here-in.
 */

typedef _INTERFACE	IHXStatistics			IHXStatistics;
typedef _INTERFACE	IHXRegistryID			IHXRegistryID;
typedef _INTERFACE	IHXServerFork			IHXServerFork;
typedef _INTERFACE	IHXServerControl		IHXServerControl;
typedef _INTERFACE	IHXReconfigServerResponse	IHXReconfigServerResponse;
typedef _INTERFACE	IHXBuffer			IHXBuffer;
typedef _INTERFACE	IHXWantServerReconfigNotification
						IHXWantServerReconfigNotification;  
typedef _INTERFACE	IHXFastAlloc			IHXFastAlloc;


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXStatistics
 * 
 *  Purpose:
 * 
 *	This interface allows update of the client statistics.
 * 
 *  IID_IHXStatistics:
 * 
 *	{00000001-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXStatistics, 0x00000001, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
	    0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXStatistics

DECLARE_INTERFACE_(IHXStatistics, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXStatistics methods
     */

    /************************************************************************
     *	Method:
     *	    IHXStatistics::Init
     *	Purpose:
     *	    Pass registry ID to the caller
     *
     */
    STDMETHOD(InitializeStatistics)	(THIS_
					UINT32	/*IN*/  ulRegistryID) PURE;

    /************************************************************************
     *	Method:
     *	    IHXStatistics::Update
     *	Purpose:
     *	    Notify the client to update its statistics stored in the registry
     *
     */
    STDMETHOD(UpdateStatistics)		(THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXRegistryID
 * 
 *  Purpose:
 * 
 *	This interface is implemented by IHXPlayer, IHXStreamSource,
 *	and IHXStream.  It allows the user to get the registry Base ID,
 *	for an object that you have a pointer to.
 * 
 *  IID_IHXRegistryID:
 * 
 *	{00000002-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXRegistryID, 0x00000002, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
	    0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXRegistryID

DECLARE_INTERFACE_(IHXRegistryID, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXRegistryID methods
     */

    /************************************************************************
     *	Method:
     *	    IHXRegistryID::GetID
     *	Purpose:
     *	    Get the registry ID of the object.
     *
     */
    STDMETHOD(GetID)	(THIS_
			REF(UINT32)	/*OUT*/  ulRegistryID) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXServerFork
 * 
 *  Purpose:
 * 
 *	This interface is implemented by the server context on Unix
 *	platforms.  This interface allows your plugin to fork off a
 *	process.  Note that the process that is forked off cannot use
 *	any RMA APIs.  The fork() system call is prohibited from within
 *	a RMA plugin.
 *
 *  IID_IHXServerFork:
 * 
 *	{00000003-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXServerFork, 0x00000003, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
	    0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXServerFork

DECLARE_INTERFACE_(IHXServerFork, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXServerFork methods
     */

    /************************************************************************
     *	Method:
     *	    IHXServerFork::Fork
     *	Purpose:
     *	    Fork off a child process.  The child process cannot use any RMA
     *	    APIs.  Upon successful completion, Fork returns 0 to the child
     *	    process and the PID of the child to the parent.  A return value
     *	    of -1 indicates an error.
     *
     *	    Note:  The child process should *NOT* Release any interfaces.
     *		The cleanup of the IHXServerFork() interface and other
     *		RMA interfaces is done by the parent.
     *
     */
    STDMETHOD_(INT32, Fork)	(THIS) PURE;
};

/*
 * 
 *  Interface:
 *
 *	IHXServerControl
 *
 *  Purpose:
 *
 *	This inteface provides access to the RealMedia server's controls
 *      for shutting down (for now).
 *
 *	Note:  This registry is not related to the Windows system registry.
 *
 *  IID_IHXServerControl:
 *
 *	{00000004-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXServerControl, 0x00000004, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);

#define CLSID_IHXServerControl	IID_IHXServerControl

#undef  INTERFACE
#define INTERFACE   IHXServerControl

DECLARE_INTERFACE_(IHXServerControl, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXServerControl methods
     */

    /************************************************************************
     *  Method:
     *      IHXServerControl::ShutdownServer
     *  Purpose:
     *      Shutdown the server.
     */
    STDMETHOD(ShutdownServer)		(THIS_
					UINT32 status) PURE;
};

/*
 * 
 *  Interface:
 *
 *	IHXServerControl2
 *
 *  Purpose:
 *
 *	Interface for extended server control methods.
 *
 *
 *  IID_IHXServerControl2:
 *
 *	{00000005-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXServerControl2, 0x00000005, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);


#undef  INTERFACE
#define INTERFACE   IHXServerControl2

DECLARE_INTERFACE_(IHXServerControl2, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *	IHXServerControl2 methods
     */

    /************************************************************************
     * IHXServerControl2::RestartServer
     *
     * Purpose:
     *
     *	    Completely shutdown the server, then restart.  Mainly used to
     * cause not hot setting config var changes to take effect.
     */
    STDMETHOD(RestartServer) (THIS) PURE;

    /************************************************************************
     * IHXServerControl2::ReconfigServer
     *
     * Purpose:
     *
     *	    Used to cause the server to re-read in config from file or registry
     * (however it was started) and attempt to use the values.
     */
    STDMETHOD(ReconfigServer)	(THIS_ IHXReconfigServerResponse* pResp) PURE;

};

/*
 * 
 *  Interface:
 *
 *	IHXReconfigServerResponse
 *
 *  Purpose:
 *
 *	Response interface for IHXServerControl2::ReconfigServer
 *
 *
 *  IID_IHXReconfigServerResponse:
 *
 *	{00000006-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXReconfigServerResponse, 0x00000006, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);


#undef  INTERFACE
#define INTERFACE   IHXReconfigServerResponse

DECLARE_INTERFACE_(IHXReconfigServerResponse, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /************************************************************************
     * IHXReconfigServerResponse::ReconfigServerDone
     *
     * Purpose:
     *
     *	    Notification that reconfiguring the server is done.
     */
    STDMETHOD(ReconfigServerDone)   (THIS_
				    HX_RESULT res,
				    IHXBuffer** pInfo,
				    UINT32 ulNumInfo) PURE;
};

/*
 * 
 *  Interface:
 *
 *	IHXServerReconfigNotification
 *
 *  Purpose:
 *
 *	Register with the server that you want notification when a reconfig
 *  request comes in and want/need to take part in the reconfiguration.  This
 *  is used when you have configuration info outside the server config file
 *  which needs to be re-initialized.
 *
 *
 *  IID_IHXServerReconfigNotification:
 *
 *	{00000007-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXServerReconfigNotification, 0x00000007, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);


#undef  INTERFACE
#define INTERFACE   IHXServerReconfigNotification

DECLARE_INTERFACE_(IHXServerReconfigNotification, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /************************************************************************
     * IHXServerReconfigNotification::WantReconfigNotification
     *
     * Purpose:
     *
     *	    Tell the server that you want reconfig notification.
     */
    STDMETHOD(WantReconfigNotification)	(THIS_
		IHXWantServerReconfigNotification* pResponse) PURE;
    
    /************************************************************************
     * IHXServerReconfigNotification::CancelReconfigNotification
     *
     * Purpose:
     *
     *	    Tell the server that you no longer want reconfig notification.
     */
    STDMETHOD(CancelReconfigNotification)   (THIS_
		IHXWantServerReconfigNotification* pResponse) PURE;

};

/*
 * 
 *  Interface:
 *
 *	IHXWantServerReconfigNotification
 *
 *  Purpose:
 *
 *	Tell user that the server got a reconfig request and it is time to
 *  do your reconfiguration.  NOTE: You should not need this if all of your
 *  configuration is stored in the config file; that is taken care of through
 *  IHXActiveRegistry.
 *
 *  IID_IHXWantServerReconfigNotification:
 *
 *	{00000008-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXWantServerReconfigNotification, 0x00000008, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);


#undef  INTERFACE
#define INTERFACE   IHXWantServerReconfigNotification

DECLARE_INTERFACE_(IHXWantServerReconfigNotification, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /************************************************************************
     * IHXWantServerReconfigNotification::ServerReconfig
     *
     * Purpose:
     *
     *	    Notify user that a server reconfig request had come in and it
     * is now your turn to do external (not server config) reconfiguration.*
     */
    STDMETHOD(ServerReconfig)	(THIS_
	IHXReconfigServerResponse* pResponse) PURE;

};

// $Private:

DEFINE_GUID(IID_IHXResolverExec, 0x00000009, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);


#undef  INTERFACE
#define INTERFACE   IHXResolverExec

DECLARE_INTERFACE_(IHXResolverExec, IUnknown)
{
    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    STDMETHOD(ResolverExec)	(THIS_ int readfd, int writefd) PURE;

};


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXFastAlloc
 *
 *  Purpose:
 *
 *	Basic memory management interface.
 *
 *  IID_IHXFastAlloc:
 *
 *	{0000000a-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXFastAlloc, 0x0000000a, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXFastAlloc

DECLARE_INTERFACE_(IHXFastAlloc, IUnknown)
{
    /*
     * IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     * IHXFastAlloc methods
     */
    STDMETHOD_(void*,FastAlloc)	    (THIS_
				    UINT32  /*IN*/ count) PURE;

    STDMETHOD_(void,FastFree)	    (THIS_
				    void*   /*IN*/ pMem) PURE;
};

#define FAST_CACHE_MEM\
    void* operator new(size_t size, IHXFastAlloc* pMalloc = NULL)\
    {\
        void* pMem;\
        if (pMalloc)\
        {\
            pMem = pMalloc->FastAlloc(size + sizeof(IHXFastAlloc*));\
        }\
        else\
        {\
            pMem = (void *)::new char [size + sizeof(IHXFastAlloc*)];\
        }\
        *(IHXFastAlloc**)pMem = pMalloc;\
        return ((unsigned char*)pMem + sizeof(IHXFastAlloc*));\
    }\
\
    void operator delete(void* pMem)\
    {\
        pMem = (unsigned char*)pMem - sizeof(IHXFastAlloc*);\
        IHXFastAlloc* pMalloc = *(IHXFastAlloc**)pMem;\
        if (pMalloc)\
        {\
            pMalloc->FastFree((char *)pMem);\
        }\
        else\
        {\
            delete[] (char *)pMem;\
        }\
    }\


/****************************************************************************
 * 
 *  Interface:
 *
 *	IHXAccurateClock
 *
 *  Purpose:
 *
 *	High Accuracy, Cheap (no system-call) gettimeofday()
 *      [ Only available on some Unix platforms, except QI can fail!! ]
 *
 *  IID_IHXAccurateClock:
 *
 *	{0000000b-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXAccurateClock, 0x0000000b, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
            0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXAccurateClock

DECLARE_INTERFACE_(IHXAccurateClock, IUnknown)
{
    /*
     * IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     * IHXAccurateClock methods
     */
    STDMETHOD_(HXTimeval,GetTimeOfDay)      (THIS) PURE;
};

// $EndPrivate.

#endif /*_HXCOMM_H_*/
