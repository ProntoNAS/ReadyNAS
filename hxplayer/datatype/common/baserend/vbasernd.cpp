/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: vbasernd.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

// include
#include "hxtypes.h"
#include "hxwintyp.h"
#include "hxcom.h"
#include "hxassert.h"

#ifdef _WINDOWS
#include <windows.h>
#include "resource.h"
#elif defined(_UNIX) && defined(USE_XWINDOWS)
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#elif defined(_MACINTOSH) || defined(_MAC_UNIX)
#include <ctype.h>
#include "platform/mac/cresload.h"
extern FSSpec g_DLLFSpec;
//CResourceLoader* CResourceLoader::zm_theResourceLoader = NULL;
#endif


#include "hxcomm.h"
#include "ihxpckts.h"
#include "hxhyper.h"
#include "hxupgrd.h"
#include "hxprefs.h"
#include "hxplugn.h"
#include "hxengin.h"
#include "hxrendr.h"
#include "hxwin.h"
#include "hxmon.h"
#include "hxcore.h"
#include "hxasm.h"
#include "hxvsurf.h"
#include "hxevent.h"
#include "hxerror.h"

// pncont
#include "hxbuffer.h"

// pnmisc
#include "unkimp.h"
#include "baseobj.h"

// baserend
#include "baserend.h"
#include "vbasernd.h"

#ifdef _WINDOWS
extern HINSTANCE g_hInstance;
#endif

CRNVisualBaseRenderer::CRNVisualBaseRenderer()
    : CRNBaseRenderer()
{
    m_cWindowSize.cx        = 0;
    m_cWindowSize.cy        = 0;
    m_pMISUS                = NULL;
    m_pSite                 = NULL;
    m_pStatusMessage        = NULL;
    m_sOldMouseX            = -1;
    m_sOldMouseY            = -1;
    m_bStatusMsgWillNeedErasing = FALSE;
    m_bSetHyperlinkCursor   = FALSE;
#if defined(_WINDOWS)
    m_hPreHyperlinkCursor   = NULL;
    m_hHyperlinkCursor      = NULL;
#elif defined(_MACINTOSH)
    const short HAND_CURSOR = 1313;
    m_eCurrentCursor        = CURSOR_ARROW;
    m_pResourceLoader = CResourceLoader::CreateInstance(g_DLLFSpec);
    m_hHyperlinkCursor = (CursHandle)m_pResourceLoader->LoadResource('CURS', HAND_CURSOR);
#elif defined(_UNIX) && defined(USE_XWINDOWS)
    m_pDisplay             = 0;
    m_Window               = 0;
    m_hHyperlinkCursor     = 0;
    m_hCurrentCursor       = 0;
#endif
}

CRNVisualBaseRenderer::~CRNVisualBaseRenderer()
{
    HX_RELEASE(m_pMISUS);
    HX_RELEASE(m_pSite);
    HX_RELEASE(m_pStatusMessage);
#if defined(_MACINTOSH)
    if (m_hHyperlinkCursor)
    {
    	m_pResourceLoader->UnloadResource((Handle)m_hHyperlinkCursor);
    	m_hHyperlinkCursor = NULL;
    	
    	HX_RELEASE(m_pResourceLoader);
    }
#endif
#if defined(_UNIX) && defined(USE_XWINDOWS)
    if (m_pDisplay && m_hHyperlinkCursor)
    {
	XLockDisplay(m_pDisplay);
	XFreeCursor(m_pDisplay, m_hHyperlinkCursor);
	XUnlockDisplay(m_pDisplay);
	m_hHyperlinkCursor = 0;
    }
#endif    
}

STDMETHODIMP CRNVisualBaseRenderer::QueryInterface(REFIID riid, void** ppvObj)
{
    HX_RESULT retVal = HXR_OK;

    if (ppvObj)
    {
        // Set default
        *ppvObj = NULL;
        // Check for IID type
        if (IsEqualIID(riid, IID_IHXSiteUser))
        {
            AddRef();
            *ppvObj = (IHXSiteUser*) this;
        }
        else if (IsEqualIID(riid, IID_IHXSiteUserSupplier))
        {
            if (m_pMISUS)
            {
                return m_pMISUS->QueryInterface(IID_IHXSiteUserSupplier, ppvObj);
            }
            else
            {
                retVal = HXR_UNEXPECTED;
            }
        }
        else
        {
            // If we don't support it, then QI our
            // base class for this interface
            retVal = CRNBaseRenderer::QueryInterface(riid, ppvObj);
        }
    }
    else
    {
        retVal = HXR_FAIL;
    }

    return retVal;
}

STDMETHODIMP_(UINT32) CRNVisualBaseRenderer::AddRef()
{
    return CRNBaseRenderer::AddRef();
}


STDMETHODIMP_(UINT32) CRNVisualBaseRenderer::Release()
{
    return CRNBaseRenderer::Release();
}

STDMETHODIMP CRNVisualBaseRenderer::InitPlugin(IUnknown *pContext)
{
    HX_RESULT retVal = CRNBaseRenderer::InitPlugin(pContext);
    if (SUCCEEDED(retVal))
    {
        // Now we need to get IHXStatusMessage - OK if TLC doesn't support it
        HX_RELEASE(m_pStatusMessage);
        pContext->QueryInterface(IID_IHXStatusMessage, (void**) &m_pStatusMessage);
    }
    return retVal;
}

STDMETHODIMP CRNVisualBaseRenderer::StartStream(IHXStream* pStream, IHXPlayer* pPlayer)
{
    HX_RESULT retVal   = HXR_OK;

    // Call the super-class's StartStream()
    retVal = CRNBaseRenderer::StartStream(pStream, pPlayer);
#if defined (HELIX_FEATURE_MISU)
    if (SUCCEEDED(retVal))
    {
        // Create a IHXMultiInstanceSiteUserSupplier interface
        HX_RELEASE(m_pMISUS);
        retVal = m_pCommonClassFactory->CreateInstance(CLSID_IHXMultiInstanceSiteUserSupplier,
                                                       (void**) &m_pMISUS);
        if (SUCCEEDED(retVal))
        {
            // Register ourselves as a site user
            retVal = m_pMISUS->SetSingleSiteUser((IUnknown*) (IHXSiteUser*) this);
        }
    }
#endif

    // Clean up the state if there was an error
    if (FAILED(retVal))
    {
        HX_RELEASE(m_pStream);
        HX_RELEASE(m_pPlayer);
        HX_RELEASE(m_pBackChannel);
        HX_RELEASE(m_pASMStream);
        HX_RELEASE(m_pMISUS);
    }

    return retVal;
}

STDMETHODIMP CRNVisualBaseRenderer::AttachSite(IHXSite *pSite)
{
    HX_RESULT retVal = HXR_OK;

    if (pSite)
    {
        // Check to see if we alredy have a site interface
        if (!m_pSite)
        {
            // Save a copy of the IHXSite interface
            m_pSite = pSite;
            m_pSite->AddRef();

            // Get the window size from the sub-class
            retVal = GetWindowSize(m_cWindowSize);
            if (SUCCEEDED(retVal))
            {
                // Inform the site of our size
                retVal = m_pSite->SetSize(m_cWindowSize);
            }
            //Give sub class a change to set up site.
            _AttachSite();
        }
        else
        {
            retVal = HXR_UNEXPECTED;
        }
    }
    else
    {
        retVal = HXR_FAIL;
    }

    if (FAILED(retVal))
    {
        HX_RELEASE(m_pSite);
    }

    return retVal;
}

STDMETHODIMP CRNVisualBaseRenderer::DetachSite()
{
    // Release our IHXSite interface
    HX_RELEASE(m_pSite);

    // Now we're done with the MISUS
    if (m_pMISUS)
    {
        m_pMISUS->ReleaseSingleSiteUser();
    }
    HX_RELEASE(m_pMISUS);

    return HXR_OK;
}


HX_RESULT CRNVisualBaseRenderer::RMASurfaceUpdate2(IHXSubRectVideoSurface* pSurface,
                                                   HXxRect*                 pExtents,
                                                   HXxBoxRegion*              pDirtyRegion)
{
    //Base impl. Should never be called except by those renderers that
    //subscribe to the sub rect messages and implemnet this themselves.
    HX_ASSERT( "Should Never be called."==NULL );
    return HXR_OK;
}

STDMETHODIMP CRNVisualBaseRenderer::HandleEvent(HXxEvent *pEvent)
{
    // Check for input error
    if (!pEvent)
    {
        return HXR_FAIL;
    }

    // Set the defaults
    pEvent->handled = FALSE;
    pEvent->result  = 0;

    switch (pEvent->event)
    {
       case HX_SURFACE_UPDATE2:
       {
           
           HXxExposeInfo* pExpose = (HXxExposeInfo*)pEvent->param2;
           IHXSubRectVideoSurface *pSurface = (IHXSubRectVideoSurface*) (pEvent->param1);
           if (pSurface)
           {
               pSurface->AddRef();
               RMASurfaceUpdate2(pSurface, &pExpose->extents, pExpose->pRegion);
               HX_RELEASE(pSurface);
           }

           pEvent->handled = TRUE;           
       }
       break;
           
       case HX_SURFACE_UPDATE:
       {
            IHXVideoSurface *pSurface = (IHXVideoSurface *) (pEvent->param1);
            if (pSurface)
            {
                pSurface->AddRef();
                RMASurfaceUpdate(pSurface);
                HX_RELEASE(pSurface);
            }

#if defined(_UNIX) && defined(USE_XWINDOWS)
	    //
	    // Create a "hand" cursor for hyperlinks
	    //
	    {
		//
		// free previously allocated cursor
		//
		if (m_pDisplay && m_hHyperlinkCursor)
		{
		    XLockDisplay(m_pDisplay);
		    XFreeCursor(m_pDisplay, m_hHyperlinkCursor);
		    XUnlockDisplay(m_pDisplay);
		    m_hHyperlinkCursor = 0;
		}

		// 
		// get new display/window parameters and 
		// allocate a new cursor
		//
		HXxWindow *pWnd = (HXxWindow*)pEvent->param2;
		m_pDisplay = (Display*)pWnd->display;
		m_Window = (Window)pWnd->window;
		if (m_pDisplay)
		    m_hHyperlinkCursor = XCreateFontCursor(m_pDisplay, XC_hand2);
	    }	    
#endif

            pEvent->handled = TRUE;
        }
        break;

        case HX_MOUSE_ENTER:
        case HX_MOUSE_LEAVE:
        case HX_MOUSE_MOVE:
        {
            HXxPoint* mousePt = (HXxPoint*) pEvent->param1;
            OnMouseMove(0, (INT16) mousePt->x, (INT16) mousePt->y);
            pEvent->handled = TRUE;
        }
        break;

        case HX_PRIMARY_BUTTON_UP:
        {
            HXxPoint* mousePt = (HXxPoint*) pEvent->param1;
            HandleClick((INT16) mousePt->x, (INT16) mousePt->y);
            pEvent->handled = TRUE;
        }
        break;

#ifdef _WINDOWS
        case WM_SETCURSOR:
        {
            if(m_bSetHyperlinkCursor)
            {
                pEvent->handled       = TRUE;
                m_hPreHyperlinkCursor = SetCursor(m_hHyperlinkCursor);
            }
            else
            {
                // pngui will handle the setting of the cursor (back to arrow cursor)
                pEvent->handled       = FALSE;
            }
        }
        break;
#endif
        default:
            break;
    }

    return HXR_OK;
}

STDMETHODIMP_(BOOL) CRNVisualBaseRenderer::NeedsWindowedSites()
{
    return FALSE;
}

STDMETHODIMP CRNVisualBaseRenderer::OnMouseMove(INT16 fwKeys, INT16 xPos, INT16 yPos)
{
    // Make sure we're up and running
    if (!m_pPlayer)
    {
        return HXR_OK;
    }

    // Don't do anything if the x/y coordinates have changed from the
    // last call to OnMouseMove - this is needed because the call to
    // IHXStatusMessage::SetStatus() results in a WM_MOUSEMOVE event
    if(xPos == m_sOldMouseX && yPos == m_sOldMouseY)
    {
        return HXR_OK;
    }
    m_sOldMouseX = xPos;
    m_sOldMouseY = yPos;


#if defined(_WINDOWS)
    HCURSOR hCurrentCursor = GetCursor();
#endif

    // Find out from the sub-class if we're over an active hyperlink
    IHXBuffer* pStatusStr     = NULL;
    BOOL        bOverHyperlink = FALSE;
    HX_RESULT   retVal         = IsMouseOverActiveLink(xPos, yPos, bOverHyperlink, pStatusStr);
    if (FAILED(retVal))
    {
        return retVal;
    }

    if (bOverHyperlink)
    {
        // Set the status bar
        if (m_pStatusMessage)
        {
	    m_bStatusMsgWillNeedErasing = TRUE;
            m_pStatusMessage->SetStatus((const char*) pStatusStr->GetBuffer());
        }
        HX_RELEASE(pStatusStr);

        // Set the cursor
#if defined(_WINDOWS)
        if(!m_hHyperlinkCursor)
        {
            m_hHyperlinkCursor = LoadCursor(g_hInstance, MAKEINTRESOURCE(HANDCURSOR));
            if(!m_hHyperlinkCursor)
            {
                m_hHyperlinkCursor = LoadCursor(NULL, IDC_UPARROW);
            }
        }

        if(m_hHyperlinkCursor && hCurrentCursor != m_hHyperlinkCursor)
        {
            // We're over a link and the cursor is NOT already the hyperlink cursor,
            // so change it. This will happen when we get a WM_SETCURSOR event
            m_bSetHyperlinkCursor = TRUE;
        }
#elif defined(_MACINTOSH)
        if (m_hHyperlinkCursor)
        {
            ::SetCursor(*m_hHyperlinkCursor);
            m_eCurrentCursor = CURSOR_HYPERLINK;
        }
#elif defined(_UNIX) && defined(USE_XWINDOWS)
	if (m_pDisplay && m_hCurrentCursor != m_hHyperlinkCursor)
	{
	    XLockDisplay(m_pDisplay);
	    XDefineCursor(m_pDisplay, m_Window, m_hHyperlinkCursor);
	    XUnlockDisplay(m_pDisplay);
	    m_hCurrentCursor = m_hHyperlinkCursor;
	}
#endif
    }
    else // if (bOverHyperlink)
    {
        // Clear the status bar
	if (m_pStatusMessage  &&
		// /Fixes PR 65008 (JPG, PNG versions): only set this to NULL
		// if we have recently set the status message, otherwise we
		// may cause SMIL's setting of the status message to be
		// overwritten with NULL, i.e., erased:
		m_bStatusMsgWillNeedErasing)
	{
	    m_bStatusMsgWillNeedErasing = FALSE;
            m_pStatusMessage->SetStatus(NULL);
        }

        // Reset the cursor
#if defined(_WINDOWS)
        if(hCurrentCursor == m_hHyperlinkCursor)
        {
            // We are not over a hyperlink and out cursor IS the hyperlink cursor,
            // so we need to change it back. This will happen when we get a WM_SETCURSOR event
            m_bSetHyperlinkCursor = FALSE;
        }
#elif defined(_MACINTOSH)
        if (m_eCurrentCursor == CURSOR_HYPERLINK)
        {
            ::InitCursor();
            m_eCurrentCursor = CURSOR_ARROW;
        }
#elif defined(_UNIX) && defined(USE_XWINDOWS)
	if (m_pDisplay && m_hCurrentCursor == m_hHyperlinkCursor)
	{
	    XLockDisplay(m_pDisplay);
	    XUndefineCursor(m_pDisplay, m_Window);
	    XUnlockDisplay(m_pDisplay);
	    m_hCurrentCursor = 0;
	}
#endif
    }

    return HXR_OK;
}


BOOL CRNVisualBaseRenderer::_IsValidRendererSurface()
{
    return m_pSite!=NULL;
}

void CRNVisualBaseRenderer::_AttachSite()
{
    //Empty base impl.
}
