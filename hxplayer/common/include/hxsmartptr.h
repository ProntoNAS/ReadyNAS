/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxsmartptr.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

/*!
  @header hxsmartptr.h
  
  @abstract Macro implementation of smart pointers to COM interfaces
            Based on pnmisc\pub\smartptr.h
 
  @discussion
 	A SmartPointer is a class that contains a pointer, but acts like it 
 	<em>is</em> the contained pointer.  When used it helps enforce correct usage 
 	of the pointer it contains.  In this case it provides the following 
 	safeguards:<ul>
 	    <li>ASSERT if the contained pointer is NULL when you attempt to 
 		use it.
 	    <li>ASSERT if you attempt to replace a valid pointer without 
 		releasing it.
 	    <li>Automatically AddRef() a pointer during assignment.
 	    <li>Automatically QI() an incoming pointer to the correct type 
 		during assignment.
 	    <li>Automatically Release() an existing pointer during assignment.
 	    <li>Automatically Release() an existing pointer during destruction.
       </ul>
 
 	SmartPointers also simplify usage. Example:
 
    <pre><code>
    HX_SMART_POINTER_INLINE( SPIHXBuffer, IHXBuffer );
    ReadDone(HX_RESULT status, IUnknown* pbufData)
    {
        SPIHXBufferPtr spBufData = pbufData;
        if(spBufData.IsValid())
        {
            cout << spBufData->GetBuffer() << endl;
        }
    }
    </code></pre>
 	This example has no memory leaks.  In the Assignment the IUnknown 
 	pointer is QI'd for the IHXBuffer.  If the QI had failed, then 
 	the IsValid test would fail too.  Even if the IsValid Test was 
 	omitted, spbufData->GetBuffer() would cause an assertion if 
 	the QI had failed.
 
   Usage Tips:
 	
 	Cannot use SmartPointers as List elements.
 
   Note that there are specific macros for creating smart pointers to 
   IUnknowns - plugging IUnknown into the standard smart pointer 
   generation macros will cause compile errors.
 
   The interface has been cut down to a minimum. In particular, the 
   automatic conversion operators have been removed to avoid the 
   possibility of unexpected conversions happening automatically. It 
   is still possible to get to the raw pointer if it is necessary.
 
 */

/*!
    @class SPIUnknown
    @description A smart pointer to an IUnknown. This will always contain the unique IUnknown 
      - i.e. it will QI any pointer passed to it (including IUnknown pointers) to make sure it gets 
      the unique IUnknown.
*/

/*!
    @class SPCIUnknown
    @description A const smart pointer to an IUnknown. This will always contain the unique IUnknown 
      - i.e. it will QI any pointer passed to it (including IUnknown pointers) to make sure it gets 
      the unique IUnknown.
*/

/*!
    @function CLASS_NAME()
*/

/*!
    @function CLASS_NAME( const CLASS_NAME& rspact )
*/

/*!
    @function CLASS_NAME(INTERFACE* pact)
*/

/*!
    @function ~CLASS_NAME()
*/

/*!
    @function CLASS_NAME(IHXCommonClassFactory* pIObjectSource, REFCLSID clsid, HX_RESULT* pResult = NULL )
    @description Create an object from a factory and attempt to assign it 
      to this smart pointer. If the created object supports the interface 
      this smart pointer refers, the smart pointer will point to that object. 
      If the created object does not support the interface, the smart pointer 
      will end up as NULL and the created object will be immediately destroyed.
    @param pIObjectSource The class factory to use
    @param clsid The clsid of the object to create
    @param pResult The result of the CreateInstance call - if this parameter 
      is NULL the result will not be returned.
*/

/*!
    @function INTERFACE* operator -> () const
*/

/*!
    @function INTERFACE& operator * () const
*/

/*!
    @function CLASS_NAME& operator =( const CLASS_NAME& rspact )
*/

/*!
    @function CLASS_NAME& operator =( INTERFACE* pNew )
*/

/*!
    @function BOOL IsValid() const
*/

/*!
    @function INTERFACE* Ptr() const
    @description Returns a non-addrefed version of the internal 
      pointer. This is designed to be used as an easy way of 
      passing the pointer to functions that take an unadorned 
      pointer. If the pointer is assigned to a local variable, 
      please bear in mind that it is not addrefed and should 
      be treated accordingly. The best way to get an AddReffed 
      pointer assigned to a local variable is to use the AsPtr 
      method.
*/

/*!
    @function AsPtr( INTERFACE*  ) const
    @description Returns an addrefed version of the internal 
      pointer. Onlu available on non-const smart pointers (if
      this was available for a const smart pointer it would 
      have to return a const pointer which then couldn't be 
      released ).
*/

/*!
    @function void AsUnknown( IUnknown** ppIUnknown ) const
    @description Returns an addrefed version of the pointer as an 
      IUnknown*.
*/

/*!
     @function Query( IUnknown* pIUnk )
     @description If possible, set the value of this smart pointer to the
	given pointer. The value of this smart pointer will only be changed if the
	supplied pointer is non-NULL and it supports the interface corresponding to
	this smart pointer. If either of these conditions is not true the value of
	the smart pointer will be unchanged.
     @param pIUnk The pointer to try and assign to this.
     @result There are three possible results:
         1. If the pointer passed in was non-NULL and the object supported the
	    appropriate interface, a SUCCEEDED result will be returned (corresponding
	    to the result of the internal QI).
         2. If the pointer passed in was non_NULL but the object did not 
	    support the appropriate interface, a FAILED result will be returned 
	    (corresponding to the result of the internal QI).
         3. If the pointer passed in was NULL, HXR_INVALID_PARAMETER will be returned.
*/

/*!
     @function Clear()
     @description Releases the smart pointers reference to the underlying pointer and 
       set the smart pointer to NULL.
*/

/*!
     @function AsInOutParam()
     @description Clears the smart pointer, then returns a pointer to the underlying 
       pointer. This is suitable for passing to functions which pass back an AddReffed 
       pointer as output. E.g.

      void SomeFunction( IHXSomeInterface** ppOutput );

      SPIHXSomeInterface spI;

      SomeFunction( spi>AsInOutParam() );
*/


#ifndef _HXSMARTPTR_H_
#define _HXSMARTPTR_H_

#include "hxassert.h"
#include "hxcom.h"
#include "hxccf.h"

/*!
    @defined HX_SMART_POINTER_INLINE
    @abstract 
	Declare a smart pointer of name CLASS_NAME which points to an 
	interface INTERFACE. Inline definitions of all functions.
*/
#define HX_SMART_POINTER_INLINE( CLASS_NAME, INTERFACE )		\
	HX_PRIVATE_SMART_POINTER_INLINE( CLASS_NAME, INTERFACE, HX_PRIVATE_BLANK, HX_PRIVATE_NON_IUNKNOWN_FUNCTIONS( CLASS_NAME, INTERFACE, HX_PRIVATE_BLANK ) )

/*!
    @defined HX_CONST_SMART_POINTER_INLINE
    @abstract 
	Declare a const smart pointer of name CLASS_NAME which points to an 
	interface INTERFACE. Inline definitions of all functions.
*/
#define HX_CONST_SMART_POINTER_INLINE( CLASS_NAME, INTERFACE )		\
	HX_PRIVATE_SMART_POINTER_INLINE( CLASS_NAME, INTERFACE, const, HX_PRIVATE_NON_IUNKNOWN_FUNCTIONS( CLASS_NAME, INTERFACE, const ) )

/*!
    @defined HX_IUNKNOWN_SMART_POINTER_INLINE
    @abstract 
	Declare a smart pointer of name CLASS_NAME which points to an 
	IUnknown interface. Inline definitions of all functions.
*/
#define HX_IUNKNOWN_SMART_POINTER_INLINE( CLASS_NAME )		\
	HX_PRIVATE_SMART_POINTER_INLINE( CLASS_NAME, IUnknown, HX_PRIVATE_BLANK, HX_PRIVATE_BLANK )

/*!
    @defined HX_IUNKNOWN_CONST_SMART_POINTER_INLINE
    @abstract 
	Declare a const smart pointer of name CLASS_NAME which points to an 
	IUnknown interface. Inline definitions of all functions.
*/
#define HX_IUNKNOWN_CONST_SMART_POINTER_INLINE( CLASS_NAME )		\
	HX_PRIVATE_SMART_POINTER_INLINE( CLASS_NAME, IUnknown, const, HX_PRIVATE_BLANK )




// ---------------------------------------------------------------------
// The macros below here should not be used directly. They might 
// change in future implementations
// ---------------------------------------------------------------------


// The IUnknown functions macros have been included 
// to reduce code duplication. If we just attempt to use the normal 
// macros for creating an IUnknown smart pointer we end up with compile 
// errors because the copy constructor and assignment ops are duplicated.
// The HX_PRIVATE_NON_IUNKNOWN_FUNCTIONS macro separates out a copy 
// constructor / assignment op that are needed in the non-IUnknown versions 
// but would cause errors in the IUnknown versions.

#define HX_PRIVATE_NON_IUNKNOWN_FUNCTIONS( CLASS_NAME, INTERFACE, QUALIFIERS )		\
    CLASS_NAME(INTERFACE QUALIFIERS* pact)						\
    : pActual(NULL)									\
    {											\
	if( pact )									\
	{										\
	    pActual = (INTERFACE*) pact;						\
	    pActual->AddRef();								\
	}										\
    }											\
    CLASS_NAME& operator =( INTERFACE QUALIFIERS* pNew )				\
    {											\
	if( pNew != pActual )								\
	{										\
	    INTERFACE* pTemp = pActual;							\
	    pActual = (INTERFACE*) pNew;						\
	    if( pActual )								\
	    {										\
		pActual->AddRef();							\
	    }										\
	    HX_RELEASE( pTemp );							\
	}										\
	return *this;									\
    }											\


#define HX_PRIVATE_SMART_POINTER_INLINE( CLASS_NAME, INTERFACE, QUALIFIERS, NON_IUNKNOWN_DEFINITIONS )	\
											\
class CLASS_NAME									\
{											\
public:											\
    CLASS_NAME()									\
	: pActual(NULL)									\
    {}											\
											\
    CLASS_NAME( const CLASS_NAME& rspact )						\
    : pActual(NULL)									\
    {											\
	if( rspact.pActual )								\
	{										\
	    pActual = rspact.pActual;							\
	    pActual->AddRef();								\
	}										\
    }											\
											\
    CLASS_NAME(IUnknown QUALIFIERS* punk)						\
    : pActual(NULL)									\
    {											\
        if(punk)									\
	    ((IUnknown*) punk)->QueryInterface( IID_##INTERFACE, ( void** )( &pActual ) );		\
    }											\
    CLASS_NAME& operator =( IUnknown QUALIFIERS* punkNew )				\
    {											\
	if( punkNew != pActual )							\
	{										\
	    INTERFACE* pTemp = pActual;							\
	    if(punkNew)									\
	    {										\
		((IUnknown*) punkNew)->QueryInterface( IID_##INTERFACE, ( void** )( &pActual ) );	\
	    }										\
	    else									\
	    {										\
		pActual = NULL;								\
	    }										\
	    HX_RELEASE( pTemp );							\
	}										\
	return *this;									\
    }											\
											\
    CLASS_NAME( IHXCommonClassFactory* pIObjectSource, REFCLSID clsid, HX_RESULT* pResult = NULL ) \
    : pActual(NULL)									\
    {											\
	HX_ASSERT(pIObjectSource);							\
	IUnknown* pIUnk = NULL;								\
	HX_RESULT result = pIObjectSource->CreateInstance( clsid, (void**)&pIUnk );	\
	if (SUCCEEDED(result))								\
	{										\
	    HX_ASSERT(pIUnk);								\
	    result = pIUnk->QueryInterface( IID_##INTERFACE, ( void** )( &pActual ) );	\
	    pIUnk->Release();								\
	}										\
	if( pResult )									\
	{										\
	    *pResult = result;								\
	}										\
    }											\
											\
    NON_IUNKNOWN_DEFINITIONS								\
											\
    ~CLASS_NAME()									\
    { HX_RELEASE (pActual); }								\
											\
    class INTERFACE##_InternalSP : public INTERFACE					\
    {											\
    private:										\
	virtual ULONG32 STDMETHODCALLTYPE AddRef () PURE;				\
	virtual ULONG32 STDMETHODCALLTYPE Release () PURE;				\
    };											\
											\
    INTERFACE##_InternalSP QUALIFIERS* operator -> () const				\
    {HX_ASSERT(pActual);return (INTERFACE##_InternalSP*) pActual;}			\
											\
    INTERFACE##_InternalSP QUALIFIERS& operator * () const				\
    {HX_ASSERT(pActual);return *((INTERFACE##_InternalSP*) pActual);}			\
											\
    CLASS_NAME& operator =( const CLASS_NAME& rspact )					\
    {*this = rspact.pActual; return *this;}						\
											\
											\
										\
    HX_RESULT Query(IUnknown QUALIFIERS* punkNew)					\
    {											\
	HX_RESULT result = HXR_INVALID_PARAMETER;					\
	if( punkNew )									\
	{										\
	    result = HXR_OK;								\
	    if( punkNew != pActual )							\
	    {										\
		INTERFACE* pTemp = NULL;						\
		result = ((IUnknown*) punkNew)->QueryInterface( IID_##INTERFACE, ( void** )( &pTemp ) );	\
		if( SUCCEEDED( result ) )						\
		{									\
		    HX_RELEASE( pActual );						\
		    pActual = pTemp;							\
		}									\
	    }										\
	}										\
	return result;									\
    }											\
											\
    BOOL IsValid() const								\
    {return pActual!=NULL;}								\
											\
    INTERFACE** AsInOutParam () 							\
    { HX_RELEASE (pActual); return &pActual; }						\
											\
    INTERFACE QUALIFIERS* Ptr() const							\
    {return pActual;}									\
											\
    void AsPtr( INTERFACE QUALIFIERS** ppActual ) const					\
    {											\
	HX_ASSERT(pActual);								\
	pActual->AddRef ();								\
	*ppActual = pActual;								\
    }											\
											\
    void AsUnknown( IUnknown QUALIFIERS** ppIUnknown ) const				\
    {											\
	HX_ASSERT(pActual);								\
	pActual->QueryInterface								\
	(										\
	    IID_IUnknown,								\
	    ( void** )ppIUnknown							\
	);										\
    }											\
											\
    void Clear ()									\
    { HX_RELEASE (pActual); }								\
											\
private:										\
    INTERFACE*	    pActual;								\
}

#define HX_PRIVATE_BLANK

HX_IUNKNOWN_SMART_POINTER_INLINE( SPIUnknown );
HX_IUNKNOWN_CONST_SMART_POINTER_INLINE( SPCIUnknown );

// Since we are guaranteed that SPIUnknown and SPCIUnknown will be storing 
// the unique IUnknown pointer we can just compare the results of the Ptr function.

inline BOOL operator ==( const SPIUnknown& sp1, const SPIUnknown& sp2 )
{
    return sp1.Ptr() == sp2.Ptr();
}

inline BOOL operator ==( const SPCIUnknown& spc1, const SPCIUnknown& spc2 )
{
    return spc1.Ptr() == spc2.Ptr();
}

/* These cannot be activated without letting everybody know. Some code might
 fail to compile: 
 	IRCAClickable* p = 0;
 	if (p == SPIUnknown(p))  //is ambigous
 	{ ... }
 	
 	
inline BOOL operator ==( const SPIUnknown& sp1, const SPCIUnknown& spc2 )
{
    return sp1.Ptr() == spc2.Ptr();
}

inline BOOL operator ==( const SPCIUnknown& spc1, const SPIUnknown& sp2 )
{
    return spc1.Ptr() == sp2.Ptr();
}
*/

inline BOOL operator !=( const SPIUnknown& sp1, const SPIUnknown& sp2 )
{
    return ! ( sp1 == sp2 );
}

inline BOOL operator !=( const SPCIUnknown& spc1, const SPCIUnknown& spc2 )
{
    return ! ( spc1 == spc2 );
}

/*
inline BOOL operator !=( const SPIUnknown& sp1, const SPCIUnknown& spc2 )
{
    return ! ( sp1 == spc2 );
}

inline BOOL operator !=( const SPCIUnknown& spc1, const SPIUnknown& sp2 )
{
    return ! ( spc1 == sp2 );
}
*/
#endif // _HXSMARTPTR_H_
