/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxengin.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef _HXENGIN_H_
#define _HXENGIN_H_

#include "hxcom.h"

/*
 * Forward declarations of some interfaces used here-in.
 */

typedef _INTERFACE	IHXBuffer			IHXBuffer;
typedef _INTERFACE	IHXCallback			IHXCallback;
typedef _INTERFACE	IHXScheduler			IHXScheduler;
typedef _INTERFACE	IHXTCPResponse			IHXTCPResponse;
typedef _INTERFACE	IHXLBoundTCPSocket		IHXLBoundTCPSocket;
typedef _INTERFACE	IHXTCPSocket			IHXTCPSocket;
typedef _INTERFACE	IHXListenResponse		IHXListenResponse;
typedef _INTERFACE	IHXListenSocket		IHXListenSocket;
typedef _INTERFACE	IHXNetworkServices		IHXNetworkServices;
typedef _INTERFACE	IHXNetworkServices2		IHXNetworkServices2;
typedef _INTERFACE	IHXUDPResponse		    	IHXUDPResponse;
typedef _INTERFACE	IHXUDPSocket			IHXUDPSocket;
typedef _INTERFACE	IHXResolver			IHXResolver;
typedef _INTERFACE	IHXResolverResponse		IHXResolverResponse;
typedef _INTERFACE	IHXInterruptSafe		IHXInterruptSafe;
typedef _INTERFACE	IHXAsyncIOSelection		IHXAsyncIOSelection;
typedef _INTERFACE	IHXUDPMulticastInit		IHXUDPMulticastInit;
typedef _INTERFACE	IHXInterruptState		IHXInterruptState;
typedef _INTERFACE	IHXOptimizedScheduler		IHXOptimizedScheduler;
// $Private:
typedef _INTERFACE	IHXThreadSafeScheduler		IHXThreadSafeScheduler;
typedef _INTERFACE	IHXBufferedSocket		IHXBufferedSocket;
typedef _INTERFACE	IHXNetInterfaces		IHXNetInterfaces;
typedef _INTERFACE	IHXNetInterfacesAdviseSink	IHXNetInterfacesAdviseSink;
// $EndPrivate.
typedef _INTERFACE	IHXNetworkInterfaceEnumerator	IHXNetworkInterfaceEnumerator;
typedef _INTERFACE	IHXUDPConnectedSocket           IHXUDPConnectedSocket;


/*
 * Address flags starting with PNR are depricated.
 */
#define HXR_INADDR_ANY	(UINT32)0x00000000  //THIS FLAG IS DEPRICATED
#define HX_INADDR_ANY	(UINT32)0x00000000

/*
 * 255.255.255.254
 *
 * Bind to all ports in IPBindings list from
 * server config.
 */
#define HXR_INADDR_IPBINDINGS (UINT32)0xfffffffe    //THIS FLAG IS DEPRICATED
#define HX_INADDR_IPBINDINGS (UINT32)0xfffffffe


/* Async IO Selection Type (Unix Only) */

#define PNAIO_READ 1
#define PNAIO_WRITE 2
#define PNAIO_EXCEPTION 4

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXCallback
 * 
 *  Purpose:
 * 
 *	This interface defines a simple callback which will be used in
 *	various interfaces such as IHXScheduler.
 * 
 *  IID_IHXCallback:
 * 
 *	{00000100-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXCallback, 0x00000100, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXCallback

DECLARE_INTERFACE_(IHXCallback, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *  IHXCallback methods
     */

    /************************************************************************
     *	Method:
     *	    IHXCallback::Func
     *	Purpose:
     *	    This is the function that will be called when a callback is
     *	    to be executed.
     */
    STDMETHOD(Func)		(THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXScheduler
 * 
 *  Purpose:
 * 
 *	This interface provides the user with a way of scheduling callbacks
 *	that will be executed at some time in the future.
 * 
 *  IID_IHXScheduler:
 * 
 *	{00000101-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXScheduler, 0x00000101, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXScheduler

typedef ULONG32 CallbackHandle;

typedef struct _HXTimeval
{
    UINT32 tv_sec;
    UINT32 tv_usec;
} HXTimeval;

DECLARE_INTERFACE_(IHXScheduler, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXScheduler methods
     */

    /************************************************************************
     *	Method:
     *	    IHXScheduler::RelativeEnter
     *	Purpose:
     *	    Schedule a callback to be executed "ms" milliseconds from now
     *	    This function is less percise then AbsoluteEnter and should only
     *	    be used when accurate timing is not critical.
     */
    STDMETHOD_(CallbackHandle,RelativeEnter)	(THIS_
						IHXCallback* pCallback,
						UINT32 ms) PURE;

    /************************************************************************
     *	Method:
     *	    IHXScheduler::AbsoluteEnter
     *	Purpose:
     *	    Schedule a callback to be executed at time "tVal".
     */
    STDMETHOD_(CallbackHandle,AbsoluteEnter)	(THIS_
						IHXCallback* pCallback,
						HXTimeval tVal) PURE;

    /************************************************************************
     *	Method:
     *	    IHXScheduler::Remove
     *	Purpose:
     *	    Remove a callback from the scheduler.
     */
    STDMETHOD(Remove)		(THIS_
			    	CallbackHandle Handle) PURE;

    /************************************************************************
     *	Method:
     *	    IHXScheduler::GetCurrentSchedulerTime
     *	Purpose:
     *	    Gives the current time (in the timeline of the scheduler).
     */
    STDMETHOD_(HXTimeval,GetCurrentSchedulerTime)	(THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXTCPResponse
 * 
 *  Purpose:
 * 
 *	This is the response interface for the asynchronous TCP networking 
 *	interface.
 * 
 *  IID_IHXTCPResponse:
 * 
 *	{00000102-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXTCPResponse, 0x00000102, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXTCPResponse

DECLARE_INTERFACE_(IHXTCPResponse, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXTCPResponse methods
     */

    /************************************************************************
     *	Method:
     *	    IHXTCPResponse::ConnectDone
     *	Purpose:
     *	    A Connect operation has been completed or an error has occurred.
     */
    STDMETHOD(ConnectDone)	(THIS_
				HX_RESULT		status) PURE;

    /************************************************************************
     *	Method:
     *	    IHXTCPResponse::ReadDone
     *	Purpose:
     *	    A Read operation has been completed or an error has occurred.
     *	    The data is returned in the IHXBuffer.
     */
    STDMETHOD(ReadDone)		(THIS_
				HX_RESULT		status,
				IHXBuffer*		pBuffer) PURE;

    /************************************************************************
     *	Method:
     *	    IHXTCPResponse::WriteReady
     *	Purpose:
     *	    This is the response method for WantWrite.
     *	    If HX_RESULT is ok, then the TCP channel is ok to Write to.
     */
    STDMETHOD(WriteReady)	(THIS_
    				HX_RESULT		status) PURE;

    /************************************************************************
     *	Method:
     *	    IHXTCPResponse::Closed
     *	Purpose:
     *	    This method is called to inform you that the TCP channel has
     *	    been closed by the peer or closed due to error.
     */
    STDMETHOD(Closed)		(THIS_
				HX_RESULT		status) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXTCPSocket
 * 
 *  Purpose:
 * 
 *	Provides the user with an asynchronous TCP networking interface.
 * 
 *  IID_IHXTCPSocket:
 * 
 *	{00000103-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXTCPSocket, 0x00000103, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXTCPSocket

DECLARE_INTERFACE_(IHXTCPSocket, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXTCPSocket methods
     *
     *  Network addresses and ports are in native byte order
     *  
     */

    STDMETHOD(Init)		(THIS_
				IHXTCPResponse*    /*IN*/  pTCPResponse) PURE;

    STDMETHOD(SetResponse)	(THIS_
    				IHXTCPResponse*	    pTCPResponse) PURE;

    STDMETHOD(Bind)		(THIS_
				UINT32			    ulLocalAddr,
				UINT16 			    nPort) PURE;

    /*
     * pDestination is a string containing host name or dotted-ip notation
     */
    STDMETHOD(Connect)		(THIS_
				const char*		    pDestination,
				UINT16 			    nPort) PURE;

    STDMETHOD(Read)		(THIS_
				UINT16			    Size) PURE;

    STDMETHOD(Write)		(THIS_
				IHXBuffer*		    pBuffer) PURE;

    /************************************************************************
     *	Method:
     *	    IHXTCPSocket::WantWrite
     *	Purpose:
     *	    This method is called when you wish to write a large amount of
     *	    data.  If you are only writing small amounts of data, you can
     *	    just call Write (all data not ready to be transmitted will be
     *	    buffered on your behalf).  When the TCP channel is ready to be
     *	    written to, the response interfaces WriteReady method will be 
     *	    called.
     */
    STDMETHOD(WantWrite)	(THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXTCPSocket::GetForeignAddress
     *	Purpose:
     *	    Returns the address of the other end of the TCP socket as a
     *	    ULONG32 in local host order
     */
    STDMETHOD(GetForeignAddress)	(THIS_
    					REF(ULONG32) lAddress) PURE;

    STDMETHOD(GetLocalAddress)		(THIS_
    					REF(ULONG32) lAddress) PURE;

    /************************************************************************
     *	Method:
     *	    IHXTCPSocket::GetForeignPort
     *	Purpose:
     *	    Returns the port of the other end of the TCP socket in local
     *      host order.
     */
    STDMETHOD(GetForeignPort)		(THIS_
    					REF(UINT16) port) PURE;

    STDMETHOD(GetLocalPort)		(THIS_
    					REF(UINT16) port) PURE;
};

// $Private:
/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXTCPSecureSocket
 * 
 *  Purpose:
 * 
 *	When an IHXTCPSocket also supports this interface,
 *	it allows you to say it's secure so it tries to use
 *	SSL.
 * 
 */
DEFINE_GUID(IID_IHXTCPSecureSocket, 0x00000203, 0x911, 0x21d1, 0x8c, 0x4, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x54);

#undef  INTERFACE
#define INTERFACE   IHXTCPSecureSocket

DECLARE_INTERFACE_(IHXTCPSecureSocket, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *  IHXTCPSecureSocket method
     */
    STDMETHOD(SetSecure)		(THIS_
    					BOOL bSecure) PURE;
};
// $EndPrivate.



// $Private:
/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXSSL
 * 
 *  Purpose:
 * 
 *	This is the interface to an SSL library.
 * 
 */
DEFINE_GUID(IID_IHXSSL, 0x34e171d4, 0xa8f0,
           0x4832, 0xbc, 0x7d, 0x06, 0xdf, 0xe3, 0xae, 0x58, 0xfd);

DECLARE_INTERFACE_(IHXSSL, IUnknown)
{
	/*
	 * IUnknown methods
	 */
	
	STDMETHOD(QueryInterface)	(THIS_
					REFIID riid,
					void** ppvObj) PURE;
	
	STDMETHOD_(ULONG32, AddRef)	(THIS) PURE;
	
	STDMETHOD_(ULONG32, Release)	(THIS) PURE;
	
	/*
	 *
	 */
	
	STDMETHOD(Initialize)		(THIS) PURE;
	
	STDMETHOD(Shutdown)		(THIS) PURE;
	
	STDMETHOD(PostConnect)		(THIS_
					LONG32 nSocketNumber) PURE;
	
	STDMETHOD_(LONG32, Read)	(THIS_
					LONG32 nSocketNumber,
					void* buff,
					LONG32 buffLen) PURE;
	
	STDMETHOD_(LONG32, Write)	(THIS_
					LONG32 nSocketNumber,
					void* buff,
					LONG32 buffLen) PURE;
	
	STDMETHOD(Close)		(THIS_
					LONG32 nSocketNumber) PURE;
					
	STDMETHOD(SetCallbacks)		(THIS_
					void* readCallback,
					void* writeCallback,
					void* closeCallback) PURE;
	
	
};
// $EndPrivate.





// $Private:
/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXBufferedSocket
 * 
 *  Purpose:
 * 
 *	This provdies a method for doing for doing more optimal
 *      TCP delivery using desired packet size and writev.  
 * 
 *  IID_IHXTCPSocket:
 * 
 *	{00001402-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXBufferedSocket, 
    0x00001402, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXBufferedSocket

DECLARE_INTERFACE_(IHXBufferedSocket, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXTCPSocket methods
     */

    STDMETHOD(BufferedWrite)		(THIS_
					IHXBuffer*	pBuffer) PURE;

    STDMETHOD(FlushWrite)		(THIS) PURE;

    STDMETHOD(SetDesiredPacketSize)     (THIS_
					UINT32		ulPacketSize) PURE;

};
// $EndPrivate.


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXListenResponse
 * 
 *  Purpose:
 * 
 *	This is the response interface for the asynchronous TCP listening
 *	socket interface.
 * 
 *  IID_IHXListenResponse:
 * 
 *	{00000104-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXListenResponse, 0x00000104, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXListenResponse

DECLARE_INTERFACE_(IHXListenResponse, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXListenResponse methods
     */

    STDMETHOD(NewConnection)	(THIS_
				HX_RESULT		status,
				IHXTCPSocket*		pTCPSocket) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXListenSocket
 * 
 *  Purpose:
 * 
 *	This interfaces allows you to asynchronously listen on a port for
 *	TCP connections.
 * 
 *  IID_IHXListenSocket:
 * 
 *	{00000105-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXListenSocket, 0x00000105, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXListenSocket

DECLARE_INTERFACE_(IHXListenSocket, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXListenSocket methods
     */

    STDMETHOD(Init)		(THIS_
				UINT32				ulLocalAddr,
				UINT16				port,
				IHXListenResponse*    /*IN*/	pListenResponse
				) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXNetworkServices
 * 
 *  Purpose:
 * 
 *	This is a factory interface for the various types of networking
 *	interfaces described above.
 * 
 *  IID_IHXNetworkServices:
 * 
 *	{00000106-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXNetworkServices, 0x00000106, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXNetworkServices

DECLARE_INTERFACE_(IHXNetworkServices, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXNetworkServices methods
     */

    /************************************************************************
     *	Method:
     *	    IHXNetworkServices::CreateTCPSocket
     *	Purpose:
     *	    Create a new TCP socket.
     */
    STDMETHOD(CreateTCPSocket)	(THIS_
				IHXTCPSocket**    /*OUT*/  ppTCPSocket) PURE;

    /************************************************************************
     *	Method:
     *	    IHXNetworkServices::CreateUDPSocket
     *	Purpose:
     *	    Create a new UDP socket.
     */
    STDMETHOD(CreateUDPSocket)	(THIS_
				IHXUDPSocket**    /*OUT*/  ppUDPSocket) PURE;

    /************************************************************************
     *	Method:
     *	    IHXNetworkServices::CreateListenSocket
     *	Purpose:
     *	    Create a new TCP socket that will listen for connections on a
     *	    particular port.
     */
    STDMETHOD(CreateListenSocket)   (THIS_
				    IHXListenSocket** /*OUT*/ ppListenSocket
				    ) PURE;

    /************************************************************************
     *	Method:
     *	    IHXNetworkServices::CreateResolver
     *	Purpose:
     *	    Create a new resolver that can lookup host names
     */
    STDMETHOD(CreateResolver)  	(THIS_
			    	IHXResolver**    /*OUT*/     ppResolver) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXNetworkServices2
 * 
 *  Purpose:
 * 
 *	This is a factory interface for the various types of networking
 *	interfaces described above.
 * 
 *  IID_IHXNetworkServices:
 * 
 *	{17951551-5683-11d3-B6BA-00C0F031C237}
 * 
 */

// {17951551-5683-11d3-B6BA-00C0F031C237}
DEFINE_GUID(IID_IHXNetworkServices2, 0x17951551, 0x5683, 0x11d3, 0xb6, 0xba, 0x0, 0xc0, 0xf0, 0x31, 0xc2, 0x37);

#undef  INTERFACE
#define INTERFACE   IHXNetworkServices2

DECLARE_INTERFACE_(IHXNetworkServices2, IHXNetworkServices)
{
    /************************************************************************
     *	Method:
     *	    IHXNetworkServices2::CreateLBoundTCPSocket
     *	Purpose:
     *	    Create a new local bound TCP socket.
     */
    STDMETHOD(CreateLBoundTCPSocket)	(THIS_
				IHXTCPSocket**    /*OUT*/  ppTCPSocket) PURE;
};



/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXUDPResponse
 * 
 *  Purpose:
 * 
 *	This is the response interface for the asynchronous UDP networking 
 *	interface.
 * 
 *  IID_IHXUDPResponse:
 * 
 *	{00000107-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXUDPResponse, 0x00000107, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXUDPResponse

DECLARE_INTERFACE_(IHXUDPResponse, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXUDPResponse methods
     */

    STDMETHOD(ReadDone)		(THIS_
				HX_RESULT		status,
				IHXBuffer*		pBuffer,
				ULONG32			ulAddr,
				UINT16			nPort) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXUDPSocket
 * 
 *  Purpose:
 * 
 *	Provides the user with an asynchronous UDP networking interface.
 * 
 *  IID_IHXUDPSocket:
 * 
 *	{00000108-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXUDPSocket, 0x00000108, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXUDPSocket

DECLARE_INTERFACE_(IHXUDPSocket, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXUDPSocket methods
     *
     *  Network addresses and ports are in native byte order
     */

    STDMETHOD(Init)		(THIS_
				ULONG32			ulAddr,
				UINT16			nPort,
				IHXUDPResponse*	pUDPResponse) PURE;

    STDMETHOD(Bind)		(THIS_
				UINT32			    ulLocalAddr,
				UINT16 			    nPort) PURE;

    STDMETHOD(Read)		(THIS_
				UINT16			Size) PURE;

    STDMETHOD(Write)		(THIS_
				IHXBuffer*		pBuffer) PURE;

    STDMETHOD(WriteTo)		(THIS_
    				ULONG32			ulAddr,
				UINT16			nPort,
				IHXBuffer*		pBuffer) PURE;

    STDMETHOD(GetLocalPort)	(THIS_
    				REF(UINT16)		port) PURE;

    STDMETHOD(JoinMulticastGroup)	(THIS_
    					ULONG32	    ulMulticastAddr,
    					ULONG32	    ulInterfaceAddr) PURE;
    
    STDMETHOD(LeaveMulticastGroup)	(THIS_
    					ULONG32	    ulMulticastAddr,
    					ULONG32	    ulInterfaceAddr) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXResolver
 * 
 *  Purpose:
 * 
 *	This interface allows you to asynchronously resolve hostnames.
 * 
 *  IID_IHXResolver:
 * 
 *	{00000109-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXResolver, 0x00000109, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXResolver

DECLARE_INTERFACE_(IHXResolver, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXResolver methods
     */

    STDMETHOD(Init)			(THIS_
					IHXResolverResponse*  pResponse) PURE;

    STDMETHOD(GetHostByName)		(THIS_
					const char* pHostName) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXResolverResponse
 * 
 *  Purpose:
 * 
 *	This is the response interface for the asynchronous DNS hostname
 *	resolver.
 * 
 *  IID_IHXResolverResponse:
 * 
 *	{0000010A-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXResolverResponse, 0x0000010A, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXResolverResponse

DECLARE_INTERFACE_(IHXResolverResponse, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXResolverResponse methods
     */

    STDMETHOD(GetHostByNameDone)	(THIS_
					HX_RESULT status,
					ULONG32 ulAddr) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXInterruptSafe
 * 
 *  Purpose:
 * 
 *	This interface is used in Macintosh implementations of callback 
 *	functions, renderers, etc... to determine if interrupt time execution  
 *	is supported. If this interface is not implemented then it is assumed
 *	that interrupt time execution is NOT supported. There are restrictions 
 *	on what may be executed at interrupt time; please consult the Macintosh
 *	Deferred Task Manager tech notes from Apple.
 * 
 *  IID_IHXInterruptSafe:
 * 
 *	{0000010B-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXInterruptSafe, 0x0000010B, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXInterruptSafe

DECLARE_INTERFACE_(IHXInterruptSafe, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *  IHXInterruptSafe methods
     */

    /************************************************************************
     *	Method:
     *	    IHXInterruptSafe::IsInterruptSafe
     *	Purpose:
     *	    This is the function that will be called to determine if
     *	    interrupt time execution is supported.
     */
    STDMETHOD_(BOOL,IsInterruptSafe)		(THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXAsyncIOSelection
 * 
 *  Purpose:
 * 
 *      This interface is implemented by the server/player context on Unix
 *      platforms.  This interface allows your plugin to get callbacks based
 *      I/O events that are normally handled by select().  This interface
 *	allows you to setup callbacks which will be executed when a file
 *	descriptor is ready for reading, writing, or has an exception.
 * 
 *  IID_IHXAsyncIOSelection:
 * 
 *	{0000010C-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXAsyncIOSelection, 0x0000010C, 0x901, 0x11d1, 0x8b, 0x6, 0x0,
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXAsyncIOSelection

DECLARE_INTERFACE_(IHXAsyncIOSelection, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXAsyncIOSelection methods
     */

    /************************************************************************
     *	Method:
     *	    IHXAsyncIOSelection::Add
     *	Purpose:
     *	    This function will allow you to receive a callback when the
     *	    given descriptor is ready for read, write, or has an
     *	    exception.  This function is only available on Unix, and is
     *	    intended to replace the functionality of select().
     */
    STDMETHOD(Add)			(THIS_
					IHXCallback*	pCallback,
					INT32		lFileDescriptor,
					UINT32		ulType) PURE;

    /************************************************************************
     *	Method:
     *	    IHXAsyncIOSelection::Remove
     *	Purpose:
     *	    This function will allow you remove the callback associated
     *	    with the given descriptor from the event handler.
     *	    This function is only available on Unix, and is intended to
     *	    replace the functionality of select().
     */
    STDMETHOD(Remove)                   (THIS_
                                        INT32           lFileDescriptor,
					UINT32		ulType) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXUDPMulticastInit
 * 
 *  Purpose:
 * 
 *	Provides the user with a way to set the TTL for outgoing multicast
 *	UDP packets.  Usually shared with IHXUDPSocket.
 * 
 *  IID_IHXUDPMulticastInit:
 * 
 *	{0000010D-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXUDPMulticastInit, 0x0000010D, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXUDPMulticastInit

DECLARE_INTERFACE_(IHXUDPMulticastInit, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXUDPMulticastInit methods
     *
     */

     /************************************************************************
     *	Method:
     *	    IHXUDPMulticastInit::InitMulticast
     *	Purpose:
     *	    This function will set the TTL (time to live) for the UDP socket
     *      so it can be used as a multicast socket, sending packets across
     *      the number of routers specified in the ulTTL parameter.  
     */

    STDMETHOD(InitMulticast)		(THIS_
    					UINT8	    chTTL) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXInterruptState
 * 
 *  Purpose:
 * 
 *	This interface is used in Macintosh implementations to inform the
 *	the client engine when entering & leaving an interupt task. It is
 *	also used to determine if it is currently at interrupt time.
 *	Please consult the Macintosh Deferred Task Manager tech notes from Apple
 *	for information on interrupt tasks.
 * 
 *  IID_IHXInterruptState:
 * 
 *	{0000010E-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXInterruptState, 0x0000010E, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXInterruptState

DECLARE_INTERFACE_(IHXInterruptState, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)	(THIS) PURE;

    STDMETHOD_(ULONG32,Release)	(THIS) PURE;

    /*
     *  IHXInterruptState methods
     */

    /************************************************************************
     *	Method:
     *	    IHXInterruptState::AtInterruptTime
     *	Purpose:
     *	    This function is called to determine if we are currently at
     *	    interrupt task time.
     */
        STDMETHOD_(BOOL,AtInterruptTime)		(THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXInterruptState::EnterInterruptState
     *	Purpose:
     *	    This function is called when starting a deferred/interrupt task
     */
    STDMETHOD(EnterInterruptState)	(THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXInterruptState::LeaveInterruptState
     *	Purpose:
     *	    This function is called when leaving a deferred/interrupt task
     */
    STDMETHOD(LeaveInterruptState)	(THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXInterruptState::EnableInterrupt
     *	Purpose:
     *	    This function can be called to enable/disable interrupt time 
     *	    processsing
     */
    STDMETHOD(EnableInterrupt)	(THIS_
				BOOL	bEnable) PURE;
    
    /************************************************************************
     *	Method:
     *	    IHXInterruptState::IsInterruptEnabled
     *	Purpose:
     *	    This function can be called to find if the core is currently
     *	    interrupt enabled.
     */
    STDMETHOD_(BOOL, IsInterruptEnabled)   (THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXOptimizedScheduler
 * 
 *  Purpose:
 * 
 *	This interface provides the user with a way of scheduling callbacks
 *	that will be executed at some time in the future.
 *
 *	This interface should ONLY be used if you need accurately timed 
 *	callbacks. These callbacks should be efficient and should not consume 
 *	much time/CPU. This is not a thread safe interface. The user has to 
 *	take care of synchronization in their callbacks.
 * 
 *  IID_IHXOptimizedScheduler:
 * 
 *	{0000010F-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXOptimizedScheduler, 0x0000010F, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXOptimizedScheduler

DECLARE_INTERFACE_(IHXOptimizedScheduler, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXOptimizedScheduler methods
     */

    /************************************************************************
     *	Method:
     *	    IHXOptimizedScheduler::RelativeEnter
     *	Purpose:
     *	    Schedule a callback to be executed "ms" milliseconds from now
     *	    This function is less percise then AbsoluteEnter and should only
     *	    be used when accurate timing is not critical.
     */
    STDMETHOD_(CallbackHandle,RelativeEnter)	(THIS_
						IHXCallback* pCallback,
						UINT32 ms) PURE;

    /************************************************************************
     *	Method:
     *	    IHXOptimizedScheduler::AbsoluteEnter
     *	Purpose:
     *	    Schedule a callback to be executed at time "tVal".
     */
    STDMETHOD_(CallbackHandle,AbsoluteEnter)	(THIS_
						IHXCallback* pCallback,
						HXTimeval tVal) PURE;

    /************************************************************************
     *	Method:
     *	    IHXOptimizedScheduler::Remove
     *	Purpose:
     *	    Remove a callback from the scheduler.
     */
    STDMETHOD(Remove)		(THIS_
			    	CallbackHandle Handle) PURE;

    /************************************************************************
     *	Method:
     *	    IHXOptimizedScheduler::GetCurrentSchedulerTime
     *	Purpose:
     *	    Gives the current time (in the timeline of the scheduler).
     */
    STDMETHOD_(HXTimeval,GetCurrentSchedulerTime)	(THIS) PURE;
};

/****************************************************************************
 *
 *  Interface:
 *
 *     IHXThreadSafeScheduler
 *
 *  Purpose:
 *
 *     This interface provides the user with a way of scheduling callbacks
 *     that will be executed at some time in the future.  This is identical
 *      to IHXScheduler except the scheduler events are considered thread-safe.
 *
 *  IID_IHXThreadSafeScheduler:
 *
 *     {00000120-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXThreadSafeScheduler, 0x00000120, 0x901, 0x11d1, 0x8b, 0x6, 0x0,
                       0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXThreadSafeScheduler

DECLARE_INTERFACE_(IHXThreadSafeScheduler, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)          (THIS_
                                       REFIID riid,
                                       void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)         (THIS) PURE;

    STDMETHOD_(ULONG32,Release)                (THIS) PURE;

    /*
     * IHXThreadSafeScheduler methods
     */

    /************************************************************************
     * Method:
     *     IHXThreadSafeScheduler::RelativeEnter
     * Purpose:

     *     Schedule a callback to be executed "ms" milliseconds from now
     *     This function is less percise then AbsoluteEnter and should only
     *     be used when accurate timing is not critical.
     */
    STDMETHOD_(CallbackHandle,RelativeEnter)   (THIS_
                                               IHXCallback* pCallback,
                                               UINT32 ms) PURE;

    /************************************************************************
     * Method:
     *     IHXThreadSafeScheduler::AbsoluteEnter
     * Purpose:
     *     Schedule a callback to be executed at time "tVal".
     */
    STDMETHOD_(CallbackHandle,AbsoluteEnter)   (THIS_
                                               IHXCallback* pCallback,
                                               HXTimeval tVal) PURE;

    /************************************************************************
     * Method:
     *     IHXThreadSafeScheduler::Remove
     * Purpose:
     *     Remove a callback from the scheduler.
     */
    STDMETHOD(Remove)          (THIS_
                               CallbackHandle Handle) PURE;
};

/****************************************************************************
 *
 *  Interface:
 *
 *     IHXProcessEntryPoint
 *
 *  Purpose:
 *
 *     This interface is the entry point for an IHXProcess
 *
 *  IID_IHXProcessEntryPoint
 *
 *     {00000123-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXProcessEntryPoint, 0x00000123, 0x901, 0x11d1, 0x8b, 0x6,
            0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXProcessEntryPoint

DECLARE_INTERFACE_(IHXProcessEntryPoint, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)          (THIS_
                                       REFIID riid,
                                       void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)         (THIS) PURE;

    STDMETHOD_(ULONG32,Release)        (THIS) PURE;

    STDMETHOD(Func)                   (THIS_
                                       IUnknown* pContext) PURE;
};

/****************************************************************************
 *
 *  Interface:
 *
 *     IHXProcess
 *
 *  Purpose:
 *
 *     This interface allows you to create new server processes and specify
 *  an entry point.  It is queried off the context.
 *
 *  IID_IHXProcess
 *
 *     {00000122-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXProcess, 0x00000122, 0x901, 0x11d1, 0x8b, 0x6, 0x0,
                       0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXProcess

DECLARE_INTERFACE_(IHXProcess, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)          (THIS_
                                       REFIID riid,
                                       void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)         (THIS) PURE;

    STDMETHOD_(ULONG32,Release)        (THIS) PURE;

    STDMETHOD(Start)                   (THIS_
                                       const char* pProcessName,
                                       IHXProcessEntryPoint* pEntryPoint) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXLoadBalancedListen
 * 
 *  Purpose:
 * 
 *	This interface is queried off of IHXListenSocket.  It allows
 *	a plugin to specify that it wants the server to load balance
 *	multiple instances of itself.  The server will instantiate multiple
 *	instances of the plugin as needed based on socket / descriptor limits.
 *	Each plugin instance should attempt to listen on the same port as
 *	other instances (they will share the port).
 * 
 *  IID_IHXLoadBalancedListen:
 * 
 *	{00000110-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXLoadBalancedListen, 0x00000110, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXLoadBalancedListen

DECLARE_INTERFACE_(IHXLoadBalancedListen, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXLoadBalancedListen methods
     */

    /************************************************************************
     *	Method:
     *	    IHXLoadBalancedListen::SetID
     *	Purpose:
     *	    This function set's the unique ID for this listen socket.  This
     *	    ID is used to determine whether or not different instances of
     *	    a plugin trying to listen on a single port are actually the
     *	    same plugin.  Without this function, it would be possible for
     *	    two completely different plugins to listen on the same port using
     *	    the load balanced listener.
     */
    STDMETHOD(SetID)		(THIS_
			    	REFIID ID) PURE;

    /************************************************************************
     *	Method:
     *	    IHXLoadBalancedListen::SetReserveLimit
     *	Purpose:
     *	    Sets the reserve limit for descriptors / sockets.  If less
     *	    than reserve limit descriptors / sockets are left then a new
     *	    instance of the plugin will be created.
     */
    STDMETHOD(SetReserveLimit)	(THIS_
			    	UINT32		ulDescriptors,
				UINT32		ulSockets) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXOverrideDefaultServices
 * 
 *  Purpose:
 * 
 *	This interface is queried off of the context.  It allows
 *	a plugin to override any default services provided by the G2 system.
 *	Currently, it is supported only on the client side. 
 *	You may currently override IHXNetworkServices using this interface
 *	You can use the same interface to later restore back the overriden services.
 *	This is done by calling the same OverrideServices() function with the 
 *	original service QIed before the initial override.
 * 
 *  IID_IHXOverrideDefaultServices:
 * 
 *	{00000111-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXOverrideDefaultServices, 0x00000111, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXOverrideDefaultServices

DECLARE_INTERFACE_(IHXOverrideDefaultServices, IUnknown)
{
    /*
     * IUnknown methods
     */
    STDMETHOD(QueryInterface)   (THIS_
                                REFIID riid,
				void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)  (THIS) PURE;

    STDMETHOD_(ULONG32,Release) (THIS) PURE;

   /*
    * IHXOverrideDefaultServices methods
    */

   /************************************************************************
    *  Method:
    *      IHXOverrideDefaultServices::OverrideServices
    *  Purpose:
    *      Override default services provided by the G2 system.
    *
    */
    STDMETHOD(OverrideServices)         (THIS_
				IUnknown* pContext) PURE;
};

typedef enum _HX_SOCKET_OPTION
{
    HX_SOCKOPT_REUSE_ADDR,
    HX_SOCKOPT_REUSE_PORT,
    HX_SOCKOPT_BROADCAST,
    HX_SOCKOPT_SET_RECVBUF_SIZE,
    HX_SOCKOPT_SET_SENDBUF_SIZE,
    HX_SOCKOPT_MULTICAST_IF,
    HX_SOCKOPT_IP_TOS
} HX_SOCKET_OPTION;

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXSetSocketOption
 * 
 *  Purpose:
 * 
 *	Set sockt option
 * 
 *  IID_IHXSetSocketOption:
 * 
 *	IID_IHXSetSocketOption:    {00000114-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXSetSocketOption,	
    0x00000114, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXSetSocketOption
DECLARE_INTERFACE_(IHXSetSocketOption, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXListenSocket methods
     */

    STDMETHOD(SetOption)		(THIS_ 
					 HX_SOCKET_OPTION option,
					 UINT32 ulValue) PURE;					 
};

#define HX_THREADSAFE_METHOD_FF_GETPACKET		0x00000001
/*
 * FileFormat::GetPacket() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     FS->Read(), FS->Close(), FS->Seek(),
 *     FFR->PacketReady(), FFR->StreamDone()
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */

#define HX_THREADSAFE_METHOD_FS_READ			0x00000002
/*
 * FileSystem::Read()/Seek()/Close() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     FS->Read(), FS->Close(), FS->Seek(),
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */
#define HX_THREADSAFE_METHOD_FSR_READDONE		0x00000004
/*
 * FileFormat::ReadDone()/SeekDone()/CloseDone() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     FS->Read(), FS->Close(), FS->Seek(),
 *     FFR->PacketReady(), FFR->StreamDone()
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */
#define HX_THREADSAFE_METHOD_CACHE_FILE		0x00000008
/*
 * FileSystem::Read()/Seek()/Close() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     FS->Read(), FS->Close(), FS->Seek(),
 *     IHXCacheFile->*, IHXCacheFileResponse->*,
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */
#define HX_THREADSAFE_METHOD_CACHE_FILE_RESPONSE	0x00000010
/*
 * FileSystem::Read()/Seek()/Close() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     FS->Read(), FS->Close(), FS->Seek(),
 *     IHXCacheFile->*, IHXCacheFileResponse->*,
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */

/*
 * Thread Safe flags for IHXDataConvert
 */
#define HX_THREADSAFE_METHOD_CONVERT_HEADERS		0x00000020
/*
 * IHXDataConvert::ConvertXXX()/CtrlBufferReady() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     IHXDataConvertResponse->*
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */
#define HX_THREADSAFE_METHOD_CONVERT_DATA		0x00000040
/*
 * IHXDataConvert::ConvertXXX()/CtrlBufferReady() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     IHXDataConvertResponse->*
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */
#define HX_THREADSAFE_METHOD_CONVERT_CTRL_BUFFER_READY	0x00000080
/*
 * IHXDataConvert::ConvertXXX()/CtrlBufferReady() only calls:
 *     CCF->CI(Buffer), CCF->CI(Packet), CCF->CI(Values), *Alloc, *Free, 
 *     IHXDataConvertResponse->*
 *     Context->Scheduler->*,
 *     CCF->CI(Mutex), Mutex->*
 *     Context->ErrorMessages
 *
 * XXXSMPNOW
 */
#define HX_THREADSAFE_METHOD_SOCKET_READDONE 		0x00000100

#define HX_THREADSAFE_METHOD_ALL 			(~0)

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXThreadSafeMethods
 * 
 *  Purpose:
 * 
 *	XXXSMPNOW
 * 
 *  IID_IHXThreadSafeMethods:
 * 
 *	{00000115-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXThreadSafeMethods, 0x00000115, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXThreadSafeMethods

DECLARE_INTERFACE_(IHXThreadSafeMethods, IUnknown)
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
     *	IHXThreadSafeMethods methods
     */

    /************************************************************************
     *	Method:
     *	    IHXThreadSafeMethods::IsThreadSafe
     *	Purpose:
     *	    XXXSMPNOW
     */
    STDMETHOD_(UINT32,IsThreadSafe)	    (THIS) PURE;
};


/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXMutex
 * 
 *  Purpose:
 * 
 *	XXXSMPNOW
 * 
 *  IID_IHXMutex:
 * 
 *	{00000116-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXMutex, 0x00000116, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXMutex

/*
 *  The IHXCommonClassFactory supports creating an instance
 *  of this object.
 */
#define CLSID_IHXMutex IID_IHXMutex

DECLARE_INTERFACE_(IHXMutex, IUnknown)
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
     *	IHXMutex methods
     */

     /* XXXSMPNOW Comments */
    STDMETHOD(Lock)	    (THIS) PURE;

    STDMETHOD(TryLock)	    (THIS) PURE;

    STDMETHOD(Unlock)	    (THIS) PURE;
};

// $Private:
/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXFastPathNetWrite
 * 
 *  Purpose:
 * 
 *	Private interface for high speed UDP output.
 * 
 *  IID_IHXFastPathNetWrite:
 * 
 *	{00000117-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXFastPathNetWrite, 0x00000117, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXFastPathNetWrite

DECLARE_INTERFACE_(IHXFastPathNetWrite, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXFastPathNetWrite methods
     *
     */

    STDMETHOD(FastWrite)	(THIS_
				const UINT8* pBuffer, UINT32 ulLen) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXWouldBlockResponse
 * 
 *  Purpose:
 * 
 *	Get notifications of EWOULDBLOCK conditions.
 * 
 *  IID_IHXWouldBlockResponse:
 * 
 *	{00000118-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXWouldBlockResponse, 0x00000118, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXWouldBlockResponse

DECLARE_INTERFACE_(IHXWouldBlockResponse, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXWouldBlockResponse methods
     *
     */
    
    
    /*
     * WouldBlock
     *
     * Return HXR_OK to go into blocked mode, causing a future
     * WouldBlockCleared call.  HXR_anythingelse to ignore.
     */
    
    STDMETHOD(WouldBlock)	(THIS_ UINT32 id) PURE;
    STDMETHOD(WouldBlockCleared)(THIS_ UINT32 id) PURE;

};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXWouldBlock
 * 
 *  Purpose:
 * 
 *	Notifier for EWOULDBLOCK conditions.
 * 
 *  IID_IHXWouldBlock:
 * 
 *	{00000119-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXWouldBlock, 0x00000119, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXWouldBlock

DECLARE_INTERFACE_(IHXWouldBlock, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXWouldBlock methods
     *
     */
    
    STDMETHOD(WantWouldBlock)	(THIS_
	    			IHXWouldBlockResponse*, UINT32 id) PURE;
};

/****************************************************************************
 *
 *  Interface:
 *
 *     IHXSharedUDPServices
 *
 *  Purpose:
 *
 *     Private interface for tying a UDP socket (via IHXUDPSocketContext) to
 *  a shared UDP resend port.  Used to send UDP packet resend requests to one
 *  shared UDP port per streamer.
 *
 *  IID_IHXSharedUDPServices
 *
 *     {00000123-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXSharedUDPServices, 0x00000124, 0x901, 0x11d1, 0x8b, 0x6, 0x0,
                       0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXSharedUDPServices

DECLARE_INTERFACE_(IHXSharedUDPServices, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)          (THIS_
                                       REFIID riid,
                                       void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)         (THIS) PURE;

    STDMETHOD_(ULONG32,Release)        (THIS) PURE;

    /*
     * IHXSharedUDPServices methods
     */
    STDMETHOD(RegisterSharedResponse)	(THIS_
					 IHXUDPResponse* response,
					 UINT16 sPortEnum) PURE;

    STDMETHOD(UnregisterSharedResponse)	(THIS) PURE;

    STDMETHOD_(UINT16, GetSharedPort)	(THIS) PURE;

};

/****************************************************************************
 *
 *  Interface:
 *
 *     IHXThreadLocal
 *
 *  Purpose:
 *
 *     Thread-local information, namely the procnum.
 *
 *  IID_IHXThreadLocal
 *
 *     {00000125-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXThreadLocal, 0x00000125, 0x901, 0x11d1, 0x8b, 0x6, 0x0,
                       0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXThreadLocal

#define CLSID_IHXThreadLocal IID_IHXThreadLocal

DECLARE_INTERFACE_(IHXThreadLocal, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)          (THIS_
                                       REFIID riid,
                                       void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)         (THIS) PURE;

    STDMETHOD_(ULONG32,Release)        (THIS) PURE;

    /*
     * IHXThreadLocal methods
     */

    /*
     *  IHXThreadLocal::GetMaxThreads()
     *
     *  Maximum number of threads on the system (MAX_THREADS on server)
     */
    STDMETHOD_(int, GetMaxThreads)	(THIS) PURE;

    /*
     *  IHXThreadLocal::GetThreadNumber()
     *
     *  This thread's number (< MAX_THREADS)
     */
    STDMETHOD_(int, GetThreadNumber)	(THIS) PURE;
};

/****************************************************************************
 *
 *  Interface:
 *
 *     IHXMemoryServices
 *
 *  Purpose:
 *
 *     Exposes server memory functions
 *
 *  IID_IHXMemoryServices
 *
 *     {00000126-0901-11d1-8B06-00A024406D59}
 *
 */
DEFINE_GUID(IID_IHXMemoryServices, 0x00000126, 0x901, 0x11d1, 0x8b, 0x6, 0x0,
                       0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXMemoryServices

#define CLSID_IHXMemoryServices IID_IHXMemoryServices

DECLARE_INTERFACE_(IHXMemoryServices, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)          (THIS_
                                       REFIID riid,
                                       void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)         (THIS) PURE;

    STDMETHOD_(ULONG32,Release)        (THIS) PURE;

    /*
     * IHXMemoryServices methods
     */

    /*
     *  IHXMemoryServices::ValidateMemory()
     *
     *  Make consistency checks on the server shared memory space.
     *
     * lStartPage, lPages
     *
     * Specifies a page range to allow smaller searches, if you have a 
     * repro case and want to fail as soon as possible after the scribble.
     * Use lPages == 0 to check to the last page.  (Pages in the shared
     * space are numbered starting from zero.  You can decide on a range
     * based on the printout of previous errors.)
     *
     * ulFlags
     *
     * 0x00000001	fail -- abort() -- on finding an error.
     * 0x00000002	do rudimentary checks on internal SharedMemory data arrays.
     *
     */
    STDMETHOD(ValidateMemory)		(THIS_
					 INT32 lStartPage, 
					 INT32 lPages, 
					 UINT32 ulFlags) PURE;
};


typedef enum _HX_PRIVATE_SOCKET_OPTION
{
    HX_PRIVATE_SOCKOPT_IGNORE_WSAECONNRESET
} HX_PRIVATE_SOCKET_OPTION;

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXSetPrivateSocketOption
 * 
 *  Purpose:
 * 
 *	Set private sockt option
 * 
 *  IID_IHXSetPrivateSocketOption:
 *
 *     {00000127-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXSetPrivateSocketOption, 
    0x00000127, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);
 
#undef  INTERFACE
#define INTERFACE   IHXSetPrivateSocketOption
DECLARE_INTERFACE_(IHXSetPrivateSocketOption, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXSetPrivateSocketOption methods
     */

    STDMETHOD(SetOption)		(THIS_ 
					 HX_PRIVATE_SOCKET_OPTION option,
					 UINT32 ulValue) PURE;					 
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXNetInterfaces
 * 
 *  Purpose:
 * 
 *	Network Interfaces
 * 
 *  IID_IHXNetInterfaces:
 *
 *     {00000128-0901-11d1-8B06-00A024406D59}
 * 
 */
typedef enum
{
    UNKNOWN,
    LOOPBACK,
    POINTTOPOINT,
    BROADCAST
} NIType;

typedef struct _NIInfo
{
    NIType	type;
    BOOL	bActive;
    UINT32	ulNetAddress;
    UINT32	ulNetMask;
} NIInfo;

DEFINE_GUID(IID_IHXNetInterfaces, 
    0x00000128, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);
 
#undef  INTERFACE
#define INTERFACE   IHXNetInterfaces
DECLARE_INTERFACE_(IHXNetInterfaces, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXNetworkInterfaces methods
     */
    STDMETHOD(UpdateNetInterfaces)		(THIS) PURE;

    STDMETHOD_(UINT32, GetNumOfNetInterfaces)	(THIS) PURE;

    STDMETHOD(GetNetInterfaces)			(THIS_
						UINT16		lIndex,
						REF(NIInfo*)	pNIInfo) PURE;

    STDMETHOD(AddAdviseSink)			(THIS_
						IHXNetInterfacesAdviseSink* pSink) PURE;

    STDMETHOD(RemoveAdviseSink)			(THIS_
						IHXNetInterfacesAdviseSink* pSink) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXNetInterfacesAdviseSink
 * 
 *  Purpose:
 * 
 *	Network Interfaces Advise Sink
 * 
 *  IID_IHXNetInterfaces:
 *
 *     {00000129-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXNetInterfacesAdviseSink, 
    0x00000129, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 0xa0, 0x24, 0x40, 0x6d, 0x59);
 
#undef  INTERFACE
#define INTERFACE   IHXNetInterfacesAdviseSink
DECLARE_INTERFACE_(IHXNetInterfacesAdviseSink, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;

    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;

    STDMETHOD_(ULONG32,Release)		(THIS) PURE;

    /*
     *	IHXNetInterfacesAdviseSink methods
     */
    STDMETHOD(NetInterfacesUpdated)	(THIS) PURE;
};
// $EndPrivate.

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXNetworkInterfaceEnumerator
 * 
 *  Purpose:
 * 
 *	Enumerate interfaces on a box.
 * 
 *  IID_IHXNetworkInterfaceEnumerator;
 * 
 *	{00000121-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXNetworkInterfaceEnumerator, 0x00000121, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXNetworkInterfaceEnumerator

DECLARE_INTERFACE_(IHXNetworkInterfaceEnumerator, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;
    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG32,Release)		(THIS) PURE;


    /************************************************************************
     *	Method:
     *	    IHXNetworkInterfaceEnumerator::EnumerateInterfaces
     *	Purpose:
     *	    returns a list of local interfaces
     *	Usage:
     *	    If a buffer passed in is too small, it will return 
     *	    HXR_BUFFERTOOSMALL with ulNumInterfaces updated.
     */
    STDMETHOD(EnumerateInterfaces)	(THIS_
	REF(UINT32*) pulInterfaces, REF(UINT32) ulNumInterfaces) PURE;
};

/****************************************************************************
 * 
 *  Interface:
 * 
 *	IHXUDPConnectedSocket
 * 
 *  Purpose:
 * 
 *	Connect and disconnect a UDP socket
 * 
 *  IID_IHXUDPConnectedSocket;
 * 
 *	{0000012A-0901-11d1-8B06-00A024406D59}
 * 
 */
DEFINE_GUID(IID_IHXUDPConnectedSocket, 0x0000012a, 0x901, 0x11d1, 0x8b, 0x6, 0x0, 
			0xa0, 0x24, 0x40, 0x6d, 0x59);

#undef  INTERFACE
#define INTERFACE   IHXUDPConnectedSocket

DECLARE_INTERFACE_(IHXUDPConnectedSocket, IUnknown)
{
    /*
     *  IUnknown methods
     */
    STDMETHOD(QueryInterface)		(THIS_
					REFIID riid,
					void** ppvObj) PURE;
    STDMETHOD_(ULONG32,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG32,Release)		(THIS) PURE;


    /************************************************************************
     *	Method:
     *	    IHXUDPConnectedSocket::UDPConnect
     *	Purpose:
     *	    Connect the udp socket
     *	Usage:
     *	    Connect to the foreign addr and port the socket already knows
     *      about; this is implementation-dependent.
     */
    STDMETHOD(UDPConnect)	(THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXUDPConnectedSocket::UDPConnect(ULONG32 ulAddr, UINT16 nPort);
     *	Purpose:
     *	    Connect the udp socket
     *	Usage:
     *	    Specify the host-ordered foreign addr and port to connect to.
     */
    STDMETHOD(UDPConnect)	(THIS_	ULONG32 ulAddr, UINT16 nPort) PURE;

    /************************************************************************
     *	Method:
     *	    IHXUDPConnectedSocket::UDPDisconnect
     *	Purpose:
     *	    Disconnect the udp socket
     */
    STDMETHOD(UDPDisconnect) (THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXUDPConnectedSocket::IsUDPConnected
     *	Purpose:
     *	    Return whether the socket is connected.
     */
    STDMETHOD_(BOOL, IsUDPConnected) (THIS) PURE;

    /************************************************************************
     *	Method:
     *	    IHXUDPConnectedSocket::IsUDPConnected(REF(ULONG32) ulAddr, 
     *                                            REF(UINT16) nPort)
     *	Purpose:
     *	    Return whether the socket is connected, and the connected addr/port.
     *	Usage:
     *      Return the foreign addr/port the socket knows about, regardless of
     *      whether it's connected.  This is the foreign addr that is connected
     *      to (if TRUE) or that will be used if Connect(void) called (if FALSE).
     */
    STDMETHOD_(BOOL, IsUDPConnected) (THIS_ REF(ULONG32) ulAddr, 
                                            REF(UINT16) nPort) PURE;
};

#endif /* _HXENGIN_H_ */
