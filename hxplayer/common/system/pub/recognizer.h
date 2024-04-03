#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include "hxcom.h"
#include "hxfiles.h"
#include "hxfilerecognizer.h"


class CHXFileRecognizer : public IHXFileRecognizer
			, public IHXFileResponse 
{
public:
    CHXFileRecognizer();
    virtual ~CHXFileRecognizer();

    /*
     *	IUnknown methods
     */
    STDMETHOD(QueryInterface)	(THIS_
				REFIID riid,
				 void** ppvObj);

    STDMETHOD_(ULONG32,AddRef)	(THIS);

    STDMETHOD_(ULONG32,Release)	(THIS);


    /*
     *	IHXFileRecognizer methods
     */

    /************************************************************************
     *	Method:
     *	    IHXFileRecognizer::GetMimeType
     *	Purpose:
     */
    STDMETHOD(GetMimeType) (THIS_
			    IHXFileObject* /*IN*/ pFile, 
			    IHXFileRecognizerResponse* /*IN*/ pFileRecognizerResponse
	);

    // IHXFileResponse methods
    STDMETHOD(InitDone)  (THIS_ HX_RESULT status);
    STDMETHOD(SeekDone)  (THIS_ HX_RESULT status);
    STDMETHOD(ReadDone)  (THIS_ HX_RESULT status, IHXBuffer *pBuffer);
    STDMETHOD(WriteDone) (THIS_ HX_RESULT status);
    STDMETHOD(CloseDone) (THIS_ HX_RESULT status);

private:
    STDMETHOD(GetMimeType) (THIS_ const char* /* IN */pFileName,
			    IHXBuffer* /* IN */ pBuffer,
			    REF(IHXBuffer*) /* OUT*/ pMimeType);

#if defined(_UNIX) && !defined(_MAC_UNIX)

    HX_RESULT GetMimeTypeFromMagic(IHXBuffer* pBuffer,
				   REF(const char*) pMimeType);
    HX_RESULT TestMP3File(IHXBuffer* pBuffer,
			  const char* pFileName,
			  REF(const char*) pMimeType);
    HX_RESULT TestSMILFile(IHXBuffer* pBuffer,
			   const char* pFileName,
			   REF(const char*) pMimeType);
    HX_RESULT TestRAMFile(IHXBuffer* pBuffer,
			  const char* pFileName,
			  REF(const char*) pMimeType);

#else // #if defined(_UNIX) && !defined(_MAC_UNIX)

    BOOL      IsSDPFile(IHXBuffer* pBuffer);

#endif // #if defined(_UNIX) && !defined(_MAC_UNIX)

    void      DoFileRecognize(void);

private:
    LONG32 m_lRefCount;
    IHXFileRecognizerResponse* m_pResponse;
    IHXFileObject* m_pFile;
    IHXBuffer* m_pBuffer;
    BOOL m_bGetMimeTypeDone;
};

#endif // _RECOGNIZER_H_
