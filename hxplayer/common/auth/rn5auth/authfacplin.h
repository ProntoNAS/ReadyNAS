/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: authfacplin.h, 2004/27/07 
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
#ifndef AUTHFACPLIN_H
#define AUTHFACPLIN_H

/****************************************************************************
 * 
 *  Function:
 * 
 *	HXCreateInstance()
 * 
 *  Purpose:
 * 
 *	Function implemented by all plugin DLL's to create an instance of 
 *	any of the objects supported by the DLL. This method is similar to 
 *	Window's CoCreateInstance() in its purpose, except that it only 
 *	creates objects from this plugin DLL.
 *
 *	NOTE: Aggregation is never used. Therefore an outer unknown is
 *	not passed to this function, and you do not need to code for this
 *	situation.
 * 
 */
STDAPI HXCreateInstance(IUnknown**  /*OUT*/	ppIUnknown);

/****************************************************************************
 * 
 *  Function:
 * 
 *	HXShutdown()
 * 
 *  Purpose:
 *  
 *	Function implemented by all plugin DLL's to free any *global* 
 *	resources. This method is called just before the DLL is unloaded.
 *
 */
STDAPI HXShutdown(void);

class CAuthPluginFactory : public IHXPluginFactory
{
public:
    CAuthPluginFactory();
    virtual ~CAuthPluginFactory();

    // IUnknown methods
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void** ppvObj);
    STDMETHOD_(UINT32,AddRef)  (THIS);
    STDMETHOD_(UINT32,Release) (THIS);

    // IHXPluginFactory methods
    STDMETHOD_(UINT16, GetNumPlugins) (THIS);
    STDMETHOD(GetPlugin)              (THIS_ UINT16 usIndex, IUnknown** ppPlugin);

protected:
    INT32  m_lRefCount;
    UINT16 m_usNumPlugins;

    static HX_RESULT (STDAPICALLTYPE* m_fpHXCreateInstanceArray[])(IUnknown**);
};

#endif
