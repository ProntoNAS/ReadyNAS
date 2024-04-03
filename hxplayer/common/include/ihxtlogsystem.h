/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ihxtlogsystem.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef IHXTLOGSYSTEM_H
#define IHXTLOGSYSTEM_H

#include "hlxclib/stdarg.h"

enum EHXTLogCode
{
    // Producer log codes
    //
    // Messages that application end-users or
    // 3rd party SDK developers will see
    LC_APP_DIAG  = 0x000000F0, // Less important/diagnostic messages (only interesting if something goes wrong)
    LC_APP_INFO  = 0x7F000000, // Very important messages (always want to see these messages)
    LC_APP_WARN  = 0x00FF0000, // There was a problem, but it was handled and everything is probably ok
    LC_APP_ERROR = 0x0000FF00, // There was a problem -- it wasn't handled

    // Messages that SDK users will see --
    // ok to mention IHX/IHXT interfaces
    LC_SDK_DIAG  = 0x000000F2, // Less important/diagnostic messages (only interesting if something goes wrong)
    LC_SDK_INFO  = 0x7f000002, // Very important messages (always want to see these messages)
    LC_SDK_WARN  = 0x00FF0002, // There was a problem, but it was handled and everything is probably ok
    LC_SDK_ERROR = 0x0000FF02, // There was a problem -- it wasn't handled

    // Messages that are only useful with the
    // corresponding source code -- ok to mention
    // internal classnames
    LC_DEV_DIAG  = 0x000000F1, // Less important/diagnostic messages (only interesting if something goes wrong)
    LC_DEV_INFO  = 0x7f000001, // Very important messages (always want to see these messages)  
    LC_DEV_WARN  = 0x00FF0001, // There was a problem, but it was handled and everything is probably ok
    LC_DEV_ERROR = 0x0000FF01, // There was a problem -- it wasn't handled

    // Client-related log codes
    LC_CLIENT_LEVEL1 = 0x00000001,
    LC_CLIENT_LEVEL2 = 0x00000002,
    LC_CLIENT_LEVEL3 = 0x00000004,
    LC_CLIENT_LEVEL4 = 0x00000008
};

enum EHXTLogCodeFilterMask
{
    SDK_MESSAGE_MASK    = 0x00000002,
    DIAG_MESSAGE_MASK   = 0x000000F0,
    ERROR_MESSAGE_MASK  = 0x0000FF00,
    WARN_MESSAGE_MASK   = 0x00FF0000,
    INFO_MESSAGE_MASK   = 0x7F000000,
    CLIENT_MSGMASK_ALL  = 0x0000000F,
    CLIENT_MSGMASK_L1_2 = 0x00000003,
    CLIENT_MSGMASK_L1_3 = 0x00000007
};

// Note: Only add new items to the end of the
// list -- otherwise the FAs will get out of sync
// with already compiled/released code and logmessages.xml
enum EHXTLogFuncArea
{
    // Producer-related functional areas
    NONE = 0,
    ACTIVEX,
    AUDCODEC,
    AUDPREFIL,
    BCAST,
    CAPTURE,
    CMDLINE,
    FILEOUT,
    FILEREAD,
    GUI,
    JOBFILE,
    LIC,
    POSFIL,
    REMOTE,
    FA_SDK_CONFIG,  // SDK config objects (typically rmsession)
    FA_SDK_ENCODE,  // SDK encoding (typically encpipeline, streammanager)
    FA_SDK_CORE,    // RSCore, RSUtil, RSGraphManager stuff
    FA_GEN_FILTER,  // Generic filter msgs
    STATS,
    VIDCODEC,
    VIDPREFIL,
    VIDRENDR,
    MEDIASAMPLES,
    PUB,
    // Client-related functional areas
    HXLOG_GENE = 0x47454E45, // Generic
    HXLOG_TRAN = 0x5452414E, // Transport
    HXLOG_ASMX = 0x41534D58, // ASM
    HXLOG_BAND = 0x42414E44, // Bandwidth manager
    HXLOG_AUDI = 0x41554449, // RealAudio
    HXLOG_VIDE = 0x56494445, // RealVideo
    HXLOG_PIXX = 0x50495858, // RealPix
    HXLOG_JPEG = 0x4A504547, // JPEG
    HXLOG_GIFX = 0x47494658, // GIF
    HXLOG_SWFX = 0x53574658, // Flash
    HXLOG_SMIL = 0x534D494C, // SMIL
    HXLOG_TURB = 0x54555242, // TurboPlay
    HXLOG_SITE = 0x53495445, // Site
    HXLOG_AUTO = 0x4155544F, // Auto-Update
    HXLOG_RECO = 0x5245434F, // Reconnect
    HXLOG_AUTH = 0x41555448, // Authentication
    HXLOG_CORE = 0x434F5245, // Core load time
    HXLOG_RTSP = 0x52545350, // RTSP
    HXLOG_STRE = 0x53545245, // Stream source map
    HXLOG_EVEN = 0x4556454E, // RealEvents
    HXLOG_BUFF = 0x42554646, // Buffer Control
    HXLOG_MP3X = 0x4D503358  // MP3
};

#include "hxtsmartpointer.h"

/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogObserver
 *
 *  Purpose:
 *
 *      IID_IHXTLogObserver:
 *
 *  This interface must be implemented by and object registering with the log
 *  system to receive log messages.
 *
 *  // {EA6ABCF4-66EB-11d4-931A-00D0B749DE42}
 *
 */

DEFINE_GUID(IID_IHXTLogObserver, 
0xea6abcf4, 0x66eb, 0x11d4, 0x93, 0x1a, 0x0, 0xd0, 0xb7, 0x49, 0xde, 0x42);

#undef INTERFACE
#define INTERFACE IHXTLogObserver

DECLARE_INTERFACE_(IHXTLogObserver, IUnknown)
{
        /*
         *      IUnknown methods
         */
        STDMETHOD(QueryInterface)               (THIS_
                                REFIID riid,
                                void** ppvObj) PURE;

        STDMETHOD_(ULONG32,AddRef)              (THIS) PURE;

        STDMETHOD_(ULONG32,Release)             (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::OnEndService
         *      Purpose:
         *          The observer will receive this call as notification that the log system
         *                      is shutting down, and that all log messages have been delivered.
         *      Parameters:
         *          NONE
         *      Returns:
         *          Ignored.
         */
  STDMETHOD(OnEndService) (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::ReceiveMsg
         *      Purpose:
         *          Method called on an observer when a log message is sent to the log 
         *                      system that passes the observers filter.
         *      Parameters:
         *          szNamespace - [in] The namespace used to qualify the functional area
         *                                      and translated messge.
         *          nCode - [in] The important level of the message.
         *          unFuncArea - [in] numeric value used in the message as the functional area
         *          szFuncArea - [in] The translated string for the numeric functional area, if
         *                                      one was found.
         *          nTimeStamp - [in] The time (in milliseconds since midnight, Jan.1, 1970) at 
         *                                      which the log message was sent to the log system.
         *          nMsg - [in] The numeric value used to identify the message for translation. 
         *          szMsg - [in] The actual message text, either translation or the text sent
         *                                      to the LogMessage method.
         *          szJobName - [in] The name of the job from which the message originated.
         *      Returns:
         *          Ignored.
         */
  STDMETHOD(ReceiveMsg) (THIS_ 
                const char*                     /*IN*/                  szNamespace, 
                EHXTLogCode                                     /*IN*/                  nCode, 
                UINT32                                  /*IN*/                  unFuncArea, 
                const char*                     /*IN*/                  szFuncArea, 
                INT32                                           /*IN*/                  nTimeStamp, 
                UINT32                                  /*IN*/                  nMsg, 
                const char*             /*IN*/                  szMsg, 
                const char*             /*IN*/                  szJobName) PURE; 
};

HXT_MAKE_SMART_PTR(IHXTLogObserver)


/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogObserver2
 *
 *  Purpose:
 *
 *      IID_IHXTLogObserver2:
 *
 *  This interface must be implemented by an object registering with the log
 *  system to receive log messages & support flusing.
 *
 *  // {68AFE313-BE30-4b46-BFAD-6F035E624C8A}
 *
 */

 // {68AFE313-BE30-4b46-BFAD-6F035E624C8A}
DEFINE_GUID(IID_IHXTLogObserver2, 
0x68afe313, 0xbe30, 0x4b46, 0xbf, 0xad, 0x6f, 0x3, 0x5e, 0x62, 0x4c, 0x8a);


#undef INTERFACE
#define INTERFACE IHXTLogObserver

DECLARE_INTERFACE_(IHXTLogObserver2, IHXTLogObserver)
{
    /************************************************************************
     *  Method:
     *      IHXTLogObserver2::Flush
     *  Purpose:
     *      Called by LogObserverManager to inform the observer to flush any internal buffers.
     *  Returns:
     *      HXR_OK - If success.
     *      HXR_FAIL - failure.
     */
    STDMETHOD(Flush)() PURE;
};

HXT_MAKE_SMART_PTR(IHXTLogObserver2)



/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTFuncAreaEnum
 *
 *  Purpose:
 *
 *  This is an enumeration interface which will enurmerate through all functional
 *  areas pre-loaded by the log system.  
 *
 *      IID_IHXTFuncAreaEnum:
 *
 *  // {938F4A21-1327-11d5-9349-00D0B749DE42}
 *
 */

DEFINE_GUID(IID_IHXTFuncAreaEnum, 
0x938f4a21, 0x1327, 0x11d5, 0x93, 0x49, 0x0, 0xd0, 0xb7, 0x49, 0xde, 0x42);

#undef INTERFACE
#define INTERFACE IHXTFuncAreaEnum

DECLARE_INTERFACE_(IHXTFuncAreaEnum, IUnknown)
{
        /*
         *      IUnknown methods
         */
        STDMETHOD(QueryInterface)               (THIS_
                                REFIID riid,
                                void** ppvObj) PURE;

        STDMETHOD_(ULONG32,AddRef)              (THIS) PURE;

        STDMETHOD_(ULONG32,Release)             (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTFuncAreaEnum::GetFirst
         *      Purpose:
         *          Returns details about the first pre-loaded functional area and resets
         *                      the enumerator to that position.
         *      Parameters:
         *          ppszNamespace - [out] Pointer to the pointer which will point to the text
         *          of the namespace for this functional area.
         *          pnNum - [out] Pointer to the integer which will be set to the numeric value
         *          for this functional area.
         *          ppszName - [out] Pointer to the pointer which will be set to the localized
         *                                      translation of the text representing the functional area.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - No functional areas preloaded by the log system.
         */
        STDMETHOD (GetFirst) (THIS_
                const char**                    /*OUT*/                 ppszNamespace, 
                UINT32*                                         /*OUT*/                 pnNum, 
                const char**                    /*OUT*/                 ppszName) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::OnEndService
         *      Purpose:
         *          Returns details about the next pre-loaded functional area.
         *      Parameters:
         *          ppszNamespace - [out] Pointer to the pointer which will point to the text
         *          of the namespace for this functional area.
         *          pnNum - [out] Pointer to the integer which will be set to the numeric value
         *          for this functional area.
         *          ppszName - [out] Pointer to the pointer which will be set to the localized
         *                                      translation of the text representing the functional area.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - No more functional areas.
         */
        STDMETHOD (GetNext) (THIS_
                const char**                    /*OUT*/                 ppszNamespace, 
                UINT32*                                         /*OUT*/                 pnNum, 
                const char**                    /*OUT*/                 ppszName) PURE;
};

HXT_MAKE_SMART_PTR(IHXTFuncAreaEnum)


/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogObserverManager
 *
 *  Purpose:
 *
 *  This interface manages the subscription of observer objects to the log
 *  system.
 *
 *      IID_IHXTLogObserverManager:
 *
 *  // {EA6ABCDC-66EB-11d4-931A-00D0B749DE42}
 *
 */

DEFINE_GUID(IID_IHXTLogObserverManager, 
0xea6abcdc, 0x66eb, 0x11d4, 0x93, 0x1a, 0x0, 0xd0, 0xb7, 0x49, 0xde, 0x42);

#undef INTERFACE
#define INTERFACE IHXTLogObserverManager

DECLARE_INTERFACE_(IHXTLogObserverManager, IUnknown)
{
        /*
         *      IUnknown methods
         */
        STDMETHOD(QueryInterface)               (THIS_
                                REFIID riid,
                                void** ppvObj) PURE;

        STDMETHOD_(ULONG32,AddRef)              (THIS) PURE;

        STDMETHOD_(ULONG32,Release)             (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::Subscribe
         *      Purpose:
         *          Adds an observer to the log system which will receive log messages, and
         *                      initializes it with the parameter values.
         *      Parameters:
         *          pUnknown - [in] IUnknown pointer to the observer object which must 
         *                                      support a QueryInterface for the IHXTLogObserver interface.
         *          szFilterStr - [in] XML string specifying an initial filter for this
         *                                      observer.
         *          szLocale - [in] Language in which the observer wishes to receive log 
         *                                      messages.  **Currently ignored**
         *          bCatchup - [in] Indicates whether the observer wishes to receive all
         *                                      log messages (up to 1000) previously delivered by the log system
         *                                      prior to this observers subscription.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - IUnknown parameter did not support the IHXTLogObserver interface.
         */
        STDMETHOD       (Subscribe) (THIS_ 
                        IUnknown*                       /*IN*/                  pUnknown, 
                        const char*             /*IN*/                  szFilterStr, 
                        const char*             /*IN*/                  szLocale, 
                        BOOL                                    /*IN*/                  bCatchUp) PURE; 

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::SetFilter
         *      Purpose:
         *          Applies the specified filter to all future log messages delivered to
         *                      the specified observer.
         *      Parameters:
         *          szFilterStr - [in] XML string specifying a filter for this
         *                                      observer.
         *          pObserver - [in] An IUnknown pointer to a previously subscribed observer
         *                                      which will have the filter applied to it.
         *      Returns:
         *          HXR_OK - Since the filter is applied asynchronously, the function will
         *               Always succeed.
         */
        STDMETHOD (SetFilter) (THIS_ 
                        const char*             /*IN*/                  szFilterStr, 
                        IUnknown*                       /*IN*/                  pObserver) PURE; 

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::Unsubscribe
         *      Purpose:
         *          Removes an observer from the log system.
         *      Parameters:
         *          pUnknown - [in] IUnknown pointer to the observer object to be 
         *                                      removed.
         *          bReceiveUnsentMessages - [in] Indicates whether the observer wishes 
         *                                      have delivered all messages which have been received by the log 
         *                                      system but not yet delivered to this observer.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - The specified observer was not subscribed to the log system.
         */
        STDMETHOD (Unsubscribe) (THIS_ 
                IUnknown*                       /*IN*/                  pObserver, 
                BOOL                                    /*IN*/                  bReceiveUnsentMessages) PURE; 

        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager::SetLanguage
         *      Purpose:
         *          Sets the language which will be used for translatable messages when
         *                      messages are delivered to the specified observer.
         *      Parameters:
         *          szLanguage - [in] Language in which the observer wishes to receive log 
         *                                      messages.  **Currently ignored**
         *          pObserver - [in] IUnknown pointer to the observer which will have its
         *                                      language value set.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - Observer was not subscribed to the log system.
         */
        STDMETHOD (SetLanguage) (THIS_ 
                const char*                     /*IN*/                  szLanguage, 
                IUnknown*                               /*IN*/                  pObserver) PURE;
};

//$ Private.
HXT_MAKE_SMART_PTR(IHXTLogObserverManager)
//$ EndPrivate.


/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogObserverManager2
 *
 *  Purpose:
 *
 *  This interface add FlushAllObservers method to IHXTLogObserverManager's methods.
 *
 *      IID_IHXTLogObserverManager2:
 *
 *  // {0E38953F-25AD-4efb-9AD4-2CBBC9D62AB0}
 *
 */

// {0E38953F-25AD-4efb-9AD4-2CBBC9D62AB0}
DEFINE_GUID(IID_IHXTLogObserverManager2, 
0xe38953f, 0x25ad, 0x4efb, 0x9a, 0xd4, 0x2c, 0xbb, 0xc9, 0xd6, 0x2a, 0xb0);

#undef INTERFACE
#define INTERFACE IHXTLogObserverManager2

DECLARE_INTERFACE_(IHXTLogObserverManager2, IHXTLogObserverManager)
{
        /************************************************************************
         *      Method:
         *          IHXTLogObserverManager2::FlushObservers
         *      Purpose:
         *          Flushes the log messages from log queue and calls Flush on all observers.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - failure.
         */
        STDMETHOD(FlushObservers) (THIS) PURE;
};

HXT_MAKE_SMART_PTR(IHXTLogObserverManager2)

/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogWriter
 *
 *  Purpose:
 *
 *  This interface is used to send log messages to the log system.
 *
 *      IID_IHXTLogWriter:
 *
 *  {EA6ABCD9-66EB-11d4-931A-00D0B749DE42}
 *
 */

DEFINE_GUID(IID_IHXTLogWriter, 
0xea6abcd9, 0x66eb, 0x11d4, 0x93, 0x1a, 0x0, 0xd0, 0xb7, 0x49, 0xde, 0x42);

#undef INTERFACE
#define INTERFACE IHXTLogWriter

DECLARE_INTERFACE_(IHXTLogWriter, IUnknown)
{
        /*
         *      IUnknown methods
         */
        STDMETHOD(QueryInterface)               (THIS_
                                REFIID riid,
                                void** ppvObj) PURE;

        STDMETHOD_(ULONG32,AddRef)              (THIS) PURE;

        STDMETHOD_(ULONG32,Release)             (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogWriter::LogMessage
         *      Purpose:
         *          Logs a message in the log system with the specified parameters
         *                      to be delivered to all observers.
         *
         *      Parameters:
         *          szNamespace - [in] Text identifier to qualify the functional area
         *                                      and numeric message parameter.
         *          nLogCode - [in] Enumerated value from rtalogconstants.h which 
         *                                      indicates the importance of the log message 
         *          nFuncArea - [in] Enumerated value from rtalogconstnats.h which 
         *                                      indicates the general area of the system where the message
         *                                      originated.
         *          nMsg - [in] Identifies the log message to be used from the translation
         *                                      xml files loaded by the log system upon startup.  To use the
         *                                      szMsg variable for the message instead, specify 0xFFFFFFFF for 
         *                                      this value.
         *          szMsg - [in] Contains the text that will be used for the log message if 
         *                                      the nMsg parameter is 0xFFFFFFFF.
         *      Returns:
         *          HXR_OK - if success
         *          HXR_FAIL - Log system is not properly initialized.
         */
        STDMETHOD(LogMessage) (THIS_ 
                                const char*                     /*IN*/          szNamespace, 
                                EHXTLogCode                                     /*IN*/          nLogCode, 
                                EHXTLogFuncArea                         /*IN*/          nFuncArea,
                                UINT32                                  /*IN*/          nMsg, 
                                const char*                     /*IN*/          szMsg
                                ) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogWriter::GetTranslatedMessage
         *      Purpose:
         *          Retrieves the translated string for the message number provided
         *                      from the log system.
         *
         *      Parameters:
         *          nMessageNumber - [in] Message number to be translated.
         *          szNamespace - [in] Namespace of the message to be translated.
         *                      szLanguage - [in] Currently unused.
         *          szMessage - [out] Translated message string.
         *      Returns:
         *          HXR_OK - if success
         */
        STDMETHOD(GetTranslatedMessage) (THIS_ 
                                UINT32                                                          /*IN*/          nMessageNumber, 
                                const char*                                             /*IN*/          szNamespace, 
                                const char*                                             /*IN*/          szLanguage,
                                const char**                                    /*OUT*/         szMessage
                                ) PURE;

};

HXT_MAKE_SMART_PTR(IHXTLogWriter)


// {E7ADC1B7-7B6E-4e54-9878-AA810ECC6DE6}
DEFINE_GUID(IID_IHXTInternalLogWriter, 
0xe7adc1b7, 0x7b6e, 0x4e54, 0x98, 0x78, 0xaa, 0x81, 0xe, 0xcc, 0x6d, 0xe6);

#undef INTERFACE
#define INTERFACE IHXTInternalLogWriter

DECLARE_INTERFACE_(IHXTInternalLogWriter, IUnknown)
{
        /*
         *      IUnknown methods
         */
        STDMETHOD(QueryInterface)               (THIS_
                                REFIID riid,
                                void** ppvObj) PURE;

        STDMETHOD_(ULONG32,AddRef)              (THIS) PURE;

        STDMETHOD_(ULONG32,Release)             (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogWriter::LogMessage
         *      Purpose:
         *          Logs a message in the log system with the specified parameters
         *                      to be delivered to all observers.
         *
         *      Parameters:
         *          szNamespace - [in] Text identifier to qualify the functional area
         *                                      and numeric message parameter.
         *          nLogCode - [in] Enumerated value from rtalogconstants.h which 
         *                                      indicates the importance of the log message 
         *          nFuncArea - [in] Enumerated value from rtalogconstnats.h which 
         *                                      indicates the general area of the system where the message
         *                                      originated.
         *          nMsg - [in] Identifies the log message to be used from the translation
         *                                      xml files loaded by the log system upon startup.  To use the
         *                                      szMsg variable for the message instead, specify 0xFFFFFFFF for 
         *                                      this value.
         *          szMsg - [in] Contains the text that will be used for the log message if 
         *                                      the nMsg parameter is 0xFFFFFFFF.
         *          args - [in] The list of variable arguments that will be substituted into
         *                                      the log message by the log system using sprintf
         *      Returns:
         *          HXR_OK - if success
         *          HXR_FAIL - Log system is not properly initialized.
         */
        STDMETHOD(LogMessage) (THIS_ 
                                const char*                     /*IN*/          szNamespace, 
                                EHXTLogCode                                     /*IN*/          nLogCode, 
                                EHXTLogFuncArea                         /*IN*/          nFuncArea,
                                UINT32                                  /*IN*/          nMsg, 
                                const char*                     /*IN*/          szMsg, 
                                va_list                                 /*IN*/          args) PURE;
};

HXT_MAKE_SMART_PTR(IHXTInternalLogWriter)


/****************************************************************************
 *
 *  Interface:
 *
 *  IHXTLogSystem
 *
 *  Purpose:
 *                      Provides access to the areas of the log system
 *  
 *      IID_IHXTLogSystem:
 *
 *  // {E50F7E51-4640-11d5-935B-00D0B749DE42}
 *
 */

DEFINE_GUID(IID_IHXTLogSystem, 
0xe50f7e51, 0x4640, 0x11d5, 0x93, 0x5b, 0x0, 0xd0, 0xb7, 0x49, 0xde, 0x42);

#undef INTERFACE
#define INTERFACE IHXTLogSystem

DECLARE_INTERFACE_(IHXTLogSystem, IUnknown)
{
        /*
         *      IUnknown methods
         */
        STDMETHOD(QueryInterface)               (THIS_
                                REFIID riid,
                                void** ppvObj) PURE;

        STDMETHOD_(ULONG32,AddRef)              (THIS) PURE;

        STDMETHOD_(ULONG32,Release)             (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogSystem::Shutdown
         *      Purpose:
         *          Properly shuts down the log system.
         *      Parameters:
         *      None.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - Log system could not shutdown properly.
         *      Notes:
         *                      Under Windows, this method should not be called from within DllMain().
         */
        STDMETHOD(Shutdown) (THIS) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogSystem::SetTranslationFileDirectory
         *      Purpose:
         *          Sets the translation file directory for the log system.
         *      Parameters:
         *          szTranslationFileDir - [in] Location of all log system translation files.  These
         *                  files will be used to translate message numbers into text strings.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - Translation file directory already set.
         */
        STDMETHOD(SetTranslationFileDirectory) (THIS_ const char* szTranslationFileDir) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogSystem::GetWriterInterface
         *      Purpose:
         *          Retrieves an interface to the log writer, used to send messages
         *                      into the log system.
         *      Parameters:
         *          ppIWriter - [out] Address of output variable that receives 
         *                  the log writer interface pointer.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - Log system not properly initialized.
         */
        STDMETHOD(GetWriterInterface) (THIS_ 
                        IHXTLogWriter**                 /*OUT*/                 ppIWriter) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogSystem::GetObserverManagerInterface
         *      Purpose:
         *          Retrieves an interface to the observer manager, used to subscribe,
         *                      manage, and unsubscribe listening observer which receive log messages.
         *      Parameters:
         *          ppILogObserverManager - [out] Address of output variable that receives 
         *                  the observer manager interface pointer.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - Log system not properly initialized.
         */
        STDMETHOD(GetObserverManagerInterface)(THIS_ 
                IHXTLogObserverManager**                        /*OUT*/                 ppILogObserverManager) PURE;

        /************************************************************************
         *      Method:
         *          IHXTLogSystem::GetFunctionalAreaEnumerator
         *      Purpose:
         *          Retrieves an interface to an enumerator which will enumerate through
         *                      all functional areas in all namespaces in the specified language  
         *                      loaded on log system initialization.
         *      Parameters:
         *          pIEnum - [out] Address of output variable that receives 
         *                  the enumerator interface pointer.
         *                      szLanguage - [in] The language of the functional areas to be enumerated.
         *      Returns:
         *          HXR_OK - If success.
         *          HXR_FAIL - Log system not properly initialized.
         */
        STDMETHOD(GetFunctionalAreaEnumerator)  (
                IHXTFuncAreaEnum**                      /*OUT*/                 pIEnum, 
                const char*                                             /*IN*/                  szLanguage) PURE;
};

/****************************************************************************
 *  Function:
 *      RMAGetLogSystemInterface
 *  Purpose:
 *      Obtains an interface pointer to the log system.  If the log system has not 
 *      yet been created, it is created and initialized.
 */
typedef HX_RESULT (STDAPICALLTYPE *FPRMAGETLOGSYSTEMINTERFACE)(IHXTLogSystem** ppLogSystem);

HXT_MAKE_SMART_PTR(IHXTLogSystem)

#endif /* #ifndef IHXTLOGSYSTEM_H */
