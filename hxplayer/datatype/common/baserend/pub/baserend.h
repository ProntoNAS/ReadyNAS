/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: baserend.h,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#ifndef BASEREND_H
#define BASEREND_H

class CRNBaseRenderer : public CHXBaseCountingObject,
                        public IHXPlugin,
                        public IHXStatistics,
                        public IHXRenderer,
                        public IHXValues,
                        public IHXUpdateProperties
{
public:
    CRNBaseRenderer();
    virtual ~CRNBaseRenderer();

    // IUnknown methods
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void** ppvObj);
    STDMETHOD_(ULONG32,AddRef)  (THIS);
    STDMETHOD_(ULONG32,Release) (THIS);

    // IHXPlugin Methods
    STDMETHOD(GetPluginInfo)        (THIS_ REF(BOOL)         rbLoadMultiple,
                                           REF(const char *) rpszDescription,
                                           REF(const char *) rpszCopyright,
                                           REF(const char *) rpszMoreInfoURL,
                                           REF(UINT32)       rulVersionNumber);
    STDMETHOD(InitPlugin)           (THIS_ IUnknown *pContext);

    // IHXStatistics methods
    STDMETHOD(InitializeStatistics) (THIS_ UINT32 ulRegistryID);
    STDMETHOD(UpdateStatistics)     (THIS);

    // IHXRenderer methods
    STDMETHOD(GetRendererInfo)      (THIS_ REF(const char**)     pStreamMimeTypes,
                                           REF(UINT32)           rulInitialGranularity);
    STDMETHOD(StartStream)          (THIS_ IHXStream* pStream, IHXPlayer* pPlayer);
    STDMETHOD(EndStream)            (THIS);
    STDMETHOD(OnHeader)             (THIS_ IHXValues* pHeader) PURE;
    STDMETHOD(OnPacket)             (THIS_ IHXPacket* pPacket, LONG32 lTimeOffset);
    STDMETHOD(OnTimeSync)           (THIS_ ULONG32 ulTime);
    STDMETHOD(OnPreSeek)            (THIS_ ULONG32 ulOldTime, ULONG32 ulNewTime);
    STDMETHOD(OnPostSeek)           (THIS_ ULONG32 ulOldTime, ULONG32 ulNewTime);
    STDMETHOD(OnPause)              (THIS_ ULONG32 ulTime);
    STDMETHOD(OnBegin)              (THIS_ ULONG32 ulTime);
    STDMETHOD(OnBuffering)          (THIS_ ULONG32 ulFlags, UINT16 unPercentComplete);
    STDMETHOD(GetDisplayType)       (THIS_ REF(HX_DISPLAY_TYPE) rulFlags,
                                           REF(IHXBuffer*)      pBuffer);
    STDMETHOD(OnEndofPackets)       (THIS);

    // IHXValues methods
    STDMETHOD(SetPropertyULONG32)      (THIS_ const char* pName, ULONG32 ulVal);
    STDMETHOD(GetPropertyULONG32)      (THIS_ const char* pName, REF(ULONG32) rulVal);
    STDMETHOD(GetFirstPropertyULONG32) (THIS_ REF(const char*) rpName, REF(ULONG32) rulVal);
    STDMETHOD(GetNextPropertyULONG32)  (THIS_ REF(const char*) rpName, REF(ULONG32) rulVal);
    STDMETHOD(SetPropertyBuffer)       (THIS_ const char* pName, IHXBuffer* pVal);
    STDMETHOD(GetPropertyBuffer)       (THIS_ const char* pName, REF(IHXBuffer*) rpVal);
    STDMETHOD(GetFirstPropertyBuffer)  (THIS_ REF(const char*) rpName, REF(IHXBuffer*) rpVal);
    STDMETHOD(GetNextPropertyBuffer)   (THIS_ REF(const char*) rpName, REF(IHXBuffer*) rpVal);
    STDMETHOD(SetPropertyCString)      (THIS_ const char* pName, IHXBuffer* pVal);
    STDMETHOD(GetPropertyCString)      (THIS_ const char* pName, REF(IHXBuffer*) rpVal);
    STDMETHOD(GetFirstPropertyCString) (THIS_ REF(const char*) rpName, REF(IHXBuffer*) rpVal);
    STDMETHOD(GetNextPropertyCString)  (THIS_ REF(const char*) rpName, REF(IHXBuffer*) rpVal);

    // IHXUpdateProperties methods
    STDMETHOD(UpdatePacketTimeOffset)  (THIS_ INT32 lTimeOffset);
    STDMETHOD(UpdatePlayTimes)	       (THIS_
				        IHXValues* pProps);

    // Other methods required by CRNBaseRenderer
    STDMETHOD(OnPacketNoOffset)     (THIS_ IHXPacket* pPacket) PURE;
    STDMETHOD(OnTimeSyncOffset)     (THIS_ UINT32 ulTime)       PURE;
protected:
    INT32                  m_lRefCount;
    IUnknown*              m_pContext;
    IHXCommonClassFactory* m_pCommonClassFactory;
    IHXHyperNavigate*      m_pHyperNavigate;
    IHXPreferences*        m_pPreferences;
    IHXScheduler*          m_pScheduler;
    IHXStream*             m_pStream;
    IHXPlayer*             m_pPlayer;
    IHXBackChannel*        m_pBackChannel;
    IHXASMStream*          m_pASMStream;
    IHXErrorMessages*      m_pErrorMessages;
    IHXValues*             m_pValues;
    UINT32                 m_ulRegistryID;
    INT32                  m_lTimeOffset;
    UINT32                 m_ulContentVersion;
    UINT32                 m_ulStreamVersion;
    UINT32                 m_ulLastTimeSync;
    IHXBuffer*             m_pStreamSourceURL;

    STDMETHOD(GetName)                                   (THIS_ REF(const char*) rpszName) PURE;
    STDMETHOD_(BOOL,GetLoadMultiple)                     (THIS);
    STDMETHOD(GetDescription)                            (THIS_ REF(const char*) rpszDescription) PURE;
    STDMETHOD(GetCopyright)                              (THIS_ REF(const char*) rpszCopyright);
    STDMETHOD(GetMoreInfoURL)                            (THIS_ REF(const char*) rpszMoreInfoURL);
    STDMETHOD(GetMimeTypes)                              (THIS_ REF(const char**) rppszMimeType) PURE;
    STDMETHOD_(UINT32,GetPluginVersion)                  (THIS) PURE;
    STDMETHOD_(UINT32,GetInitialGranularity)             (THIS);
    STDMETHOD_(UINT32,GetDisplayFlags)                   (THIS);
    STDMETHOD_(UINT32,GetHighestSupportedContentVersion) (THIS);
    STDMETHOD_(UINT32,GetHighestSupportedStreamVersion)  (THIS);

    static const char*      const m_pszBaseCopyright;
    static const char*      const m_pszBaseMoreInfoURL;

    STDMETHOD(CheckStreamVersions)        (THIS_ IHXValues* pHeader);
    STDMETHOD(AddMimeToUpgradeCollection) (THIS_ const char* pszMimeType);
    STDMETHOD(GetPreference)              (THIS_ const char*      pszPrefName,
                                                 REF(BOOL)        rbPresent,
                                                 REF(IHXBuffer*) rpBuffer);

    virtual BOOL _IsValidRendererSurface();
    
};

#endif /* #ifdef BASEREND_H */
