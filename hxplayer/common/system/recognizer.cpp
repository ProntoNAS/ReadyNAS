#include "ihxpckts.h"
#include "hxbuffer.h"
#include "hlxclib/ctype.h" /* for isspace() */
#include "hxmarsh.h"      /* for getshort() */
#include "recognizer.h"


#define REALMEDIA_MIME_TYPE             "application/x-pn-realmedia"
#define REALAUDIO_MIME_TYPE             "audio/x-pn-realaudio"
#define FLASH_MIME_TYPE                 "application/x-shockwave-flash"
#define REALAUDIO_METAFILE_MIME_TYPE    "audio/x-pn-realaudio"
#define REALAUDIO_PLUGIN_MIME_TYPE      "audio/x-pn-realaudio-plugin"
#define SMIL_MIME_TYPE                  "application/smil"


// length of buffer read from file and passed to recognizer
#if defined(_UNIX) && !defined(_MAC_UNIX)
// we want a little more data, since we need to try to detect smil, ram and mp3
#define RECOG_LEN 4096
#else
#define RECOG_LEN 512
#endif
 

#if !defined (_SYMBIAN)
HX_RESULT CHXFileRecognizer::GetMimeType(const char* pFileName, IHXBuffer* pBuffer, REF(IHXBuffer*) pMimeType)
{
    HX_RESULT res = HXR_FAIL;
    const char* pMimeTypeStr = NULL;

#if defined(_UNIX) && !defined(_MAC_UNIX)
    do
    {
	res = GetMimeTypeFromMagic(pBuffer, pMimeTypeStr);
	if (SUCCEEDED(res))
	    break;

	res = TestSMILFile(pBuffer, pFileName, pMimeTypeStr);
	if (SUCCEEDED(res))
	    break;

	res = TestRAMFile(pBuffer, pFileName, pMimeTypeStr);
	if (SUCCEEDED(res))
	    break;

	res = TestMP3File(pBuffer, pFileName, pMimeTypeStr);
    }
    while (0);

#else // #if defined(_UNIX) && !defined(_MAC_UNIX)

    if (IsSDPFile(pBuffer))
    {
	pMimeTypeStr = "application/sdp";
	res = HXR_OK;
    }

#endif // #if defined(_UNIX) && !defined(_MAC_UNIX)

    if (pMimeTypeStr)
    {
        // convert mime type to IHXBuffer
        pMimeType = new CHXBuffer;
        if (pMimeType)
        {
            pMimeType->AddRef();

            int len = strlen(pMimeTypeStr);
	    res = pMimeType->Set((const UCHAR*)pMimeTypeStr, len + 1);
	    if (FAILED(res))
	    {
		HX_RELEASE(pMimeType);
	    }
        }
	else
	{
	    res = HXR_OUTOFMEMORY;
	}
    }

    return res;
}
#endif


#if defined(_UNIX) && !defined(_MAC_UNIX)


HX_RESULT
CHXFileRecognizer::GetMimeTypeFromMagic(IHXBuffer* pBuffer,
					REF(const char*) pMimeType)
{
    HX_RESULT res = HXR_FAIL;
    pMimeType = NULL;
    HX_ASSERT(pBuffer);

    if (pBuffer && pBuffer->GetSize())
    {
	UCHAR* pData = pBuffer->GetBuffer();
	UINT32 uLength = pBuffer->GetSize();

	// map magic data to a mime type
	typedef struct
	{
	    UINT32 uOffset;
	    const char* pMagic;
	    UINT32 uLength;
	    const char* pMimeType;
	} magic_data;

	// these are the magic tricks we know
	const magic_data kMagicMap[] =
	    {
		{ 0, "v=",               2, "application/sdp" },
		{ 0, ".RA ",             4, REALAUDIO_MIME_TYPE },
		{ 0, ".RMF",             4, REALMEDIA_MIME_TYPE },
		{ 0, ".RMS",             4, REALMEDIA_MIME_TYPE },
		{ 0, "OggS",             4, "audio/x-ogg", },
		{ 0, "#!AMR",            5, "audio/amr" },
		{ 0, "WAVE",             4, "audio/x-wav" },
		{ 0, "GIF89a",           6, "image/gif" },
		{ 0, "GIF87a",           6, "image/gif" },
		{ 0, "\211PNG",          4, "image/png" },
		{ 0, "\377\330\377\340", 4, "image/jpeg" },
		{ 0, "BM",               2, "image/bmp" },
		{ 0, "\355\253\356\333", 4, "application/x-rpm" },
		{ 0, "FWS",              3, FLASH_MIME_TYPE },
		{ 0, "CWS",              3, FLASH_MIME_TYPE },
		{ 0, "#EXTM3U",          7, "audio/scpls" },
		{ 0, NULL, 0, NULL } // null terminator
	    };

	// find a matching magic string
	for (const magic_data* pCursor = kMagicMap; pCursor->pMagic; ++pCursor)
	{
	    if (pCursor->uOffset + pCursor->uLength < uLength &&
		memcmp(pData + pCursor->uOffset, 
		       pCursor->pMagic, pCursor->uLength) == 0)
	    {
		pMimeType = pCursor->pMimeType;
		res = HXR_OK;
		break;
	    }
	}
    }

    return res;
}

HX_RESULT
CHXFileRecognizer::TestMP3File(IHXBuffer* pBuffer,
			       const char* pFileName,
			       REF(const char*) pMimeType)
{
    HX_RESULT res = HXR_FAIL;
    HX_ASSERT(pBuffer);

    BOOL bMPA = FALSE;
    BOOL bID3 = FALSE;

    if (pBuffer->GetSize() >= 4)
    {
	const char* pBegin = (const char*)pBuffer->GetBuffer();

	// XXXNH: I stole this from /usr/share/misc/magic and it seems to
	// do a good job of guessing mp3
	UINT16 uShort = getshort((UINT8*)pBegin);

	if ((uShort & 0xffe0) == 0xffe0 &&
	    (uShort & 0x0018) != 0x0008 &&
	    (uShort & 0x0006) != 0x0000)
	{
	    bMPA = TRUE;
	}
	
	else if (memcmp(pBegin, "ID3\003", 4) == 0)
	{
	    bID3 = TRUE;
	}
    }

    if (bMPA || bID3)
    {
	res = HXR_OK;
	pMimeType = "audio/mp3";
    }

    return res;
}


HX_RESULT
CHXFileRecognizer::TestSMILFile(IHXBuffer* pBuffer,
				const char* pFileName,
				REF(const char*) pMimeType)
{
    HX_RESULT res = HXR_FAIL;
    HX_ASSERT(pBuffer);

    const char* pExt = strrchr(pFileName, '.');
    BOOL bValidExt = pExt && (strcasecmp(pExt, ".smi") == 0 ||
			      strcasecmp(pExt, ".smil") == 0);

    const char* pBegin = (const char*)pBuffer->GetBuffer();
    UINT32 uLen = pBuffer->GetSize();
    const char* pEnd = (const char*)((UINT32)pBegin + uLen);

    BOOL bSMILFound = FALSE;
    BOOL bBodyFound = FALSE;
    for (const char* pCursor = pBegin; 
	 (!bSMILFound || !bBodyFound) && pCursor && pCursor < pEnd; 
	 pCursor = strpbrk(pCursor+1, "\n\r")) // next line
    {
	// eat whitespace
	while (isspace(*pCursor) && pCursor < pEnd)
	    ++pCursor;

	const char* pTag = strchr(pCursor, '<');
	while (pTag && (!bSMILFound || !bBodyFound))
	{
	    if (!bSMILFound && strncasecmp(pTag, "<smil", 5) == 0)
	    {
		bSMILFound = TRUE;
	    }
	    else if (!bBodyFound && strncasecmp(pTag, "<body", 5) == 0)
	    {
		bBodyFound = TRUE;
	    }
	    pTag = strchr(pTag+1, '<');
	}
    }

    if (bSMILFound && (bBodyFound || bValidExt))
    {
	// very likely a SMIL file
	pMimeType = SMIL_MIME_TYPE;
	res = HXR_OK;
    }

    return res;
}


HX_RESULT
CHXFileRecognizer::TestRAMFile(IHXBuffer* pBuffer,
			       const char* pFileName,
			       REF(const char*) pMimeType)
{
    HX_RESULT res = HXR_FAIL;
    HX_ASSERT(pBuffer);

    const char* pBegin = (const char*)pBuffer->GetBuffer();
    UINT32 uLen = pBuffer->GetSize();
    const char* pEnd = (const char*)((UINT32)pBegin + uLen);

    BOOL bProtocolFound = FALSE;
    BOOL bHTML = FALSE;
    for (const char* pCursor = pBegin; pCursor && pCursor < pEnd; 
	 pCursor = strpbrk(pCursor+1, "\n\r")) // next line
    {
	// eat whitespace
	while (isspace(*pCursor) && pCursor < pEnd)
	    ++pCursor;

	if (pCursor && pCursor < pEnd && *pCursor != '#') // skip comments
	{
	    // do we have an html tag?
	    const char* pTag = strchr(pCursor, '<');
	    if (pTag && strncasecmp(pCursor, "<html", 5) == 0)
	    {
		bHTML = TRUE;
		break;
	    }

	    // find a protocol?
	    if (strncasecmp(pCursor, "rtsp://", 7) == 0 ||
		strncasecmp(pCursor, "http://", 7) == 0 ||
		strncasecmp(pCursor, "file://", 7) == 0 ||
		strncasecmp(pCursor, "pnm://", 6) == 0)
	    {
		bProtocolFound = TRUE;
		break;
	    }
	}
    }

    if (!bHTML && bProtocolFound)
    {
	// this is probably a RAM file
	const char* pExt = strrchr(pFileName, '.');
	if (pExt && strcasecmp(pExt, ".rpm") == 0)
	{
	    pMimeType = REALAUDIO_PLUGIN_MIME_TYPE;
	}
	else
	{
	    pMimeType = REALAUDIO_METAFILE_MIME_TYPE;
	}

	res = HXR_OK;
    }

    return res;
}

#else // #if defined(_UNIX) && !defined(_MAC_UNIX)



BOOL
CHXFileRecognizer::IsSDPFile(IHXBuffer* pBuffer)
{
    BOOL bResult = FALSE;

    if (pBuffer && pBuffer->GetSize())
    {
        if (0 == strncmp((const char*)pBuffer->GetBuffer(), "v=", 2))
        {
            bResult = TRUE;
        }
    }

    return bResult;
}


#endif // #if defined(_UNIX) && !defined(_MAC_UNIX)


CHXFileRecognizer::CHXFileRecognizer()
    : m_lRefCount(0),
      m_pResponse(NULL),
      m_pFile(NULL),
      m_pBuffer(NULL),
      m_bGetMimeTypeDone(FALSE)
{
}

CHXFileRecognizer::~CHXFileRecognizer()
{
    HX_RELEASE(m_pBuffer);
    HX_RELEASE(m_pResponse);
    HX_RELEASE(m_pFile);
}

STDMETHODIMP CHXFileRecognizer::QueryInterface(REFIID riid, void** ppvObj)
{
    HX_RESULT ret = HXR_OK;

    if (IsEqualIID(riid, IID_IHXFileRecognizer))
    {
	AddRef();
	*ppvObj = (IHXFileRecognizer*)this;
    }
    else if (IsEqualIID(riid, IID_IHXFileResponse))
    {
	AddRef();
	*ppvObj = (IHXFileResponse*)this;
    }
    else if (IsEqualIID(riid, IID_IUnknown))
    {
	AddRef();
	*ppvObj = this;
    }
    else
    {
	*ppvObj = NULL;
	ret = HXR_NOINTERFACE;
    }

    return ret;
}

STDMETHODIMP_(ULONG32) CHXFileRecognizer::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG32) CHXFileRecognizer::Release()
{
    if (InterlockedDecrement(&m_lRefCount) > 0)
    {
        return m_lRefCount;
    }

    delete this;
    return 0;
}

STDMETHODIMP CHXFileRecognizer::GetMimeType(IHXFileObject* /*IN*/ pFile, 
	    IHXFileRecognizerResponse* /*IN*/ pRecognizerResponse)
{
    HX_RESULT ret = HXR_FAIL;

    if (pRecognizerResponse)
    {
	m_pResponse = pRecognizerResponse;
	m_pResponse->AddRef();
    }
    
    // get our own IHXFileResponse interface
    IHXFileResponse* pFileResponse = NULL;
    ret = QueryInterface(IID_IHXFileResponse, (void**) &pFileResponse);

    if (SUCCEEDED(ret) && pFileResponse && pFile != NULL)
    {
	m_pFile = pFile;
	m_pFile->AddRef();
    
        ret = m_pFile->Init(HX_FILE_READ | HX_FILE_BINARY, pFileResponse);
    }

    if (FAILED(ret) && m_pResponse)
    {
	m_pResponse->GetMimeTypeDone(ret, NULL);
    }
    
    HX_RELEASE(pFileResponse);

    return ret;
}

STDMETHODIMP CHXFileRecognizer::InitDone(HX_RESULT status)
{
    if (SUCCEEDED(status))
    {
	status = m_pFile->Read(RECOG_LEN);
    }

    if (FAILED(status) && m_pResponse)
    {
	m_pResponse->GetMimeTypeDone(status, NULL);
    }

    return status;
}

STDMETHODIMP CHXFileRecognizer::SeekDone(HX_RESULT /* status */)
{
    if (!m_bGetMimeTypeDone)
    {
        DoFileRecognize();
        HX_RELEASE(m_pBuffer);
    }

    return HXR_OK;
}

STDMETHODIMP CHXFileRecognizer::ReadDone(HX_RESULT status,
					 IHXBuffer* pBuffer)
{
    if (FAILED(status) && m_pResponse)
    {
	m_pResponse->GetMimeTypeDone(HXR_FAIL, NULL);
    }
    else
    {
        HX_ASSERT(!m_pBuffer);

        m_pBuffer = pBuffer;
        HX_ADDREF(m_pBuffer);
 
         // seek back to the beginning of the file
         // since the same file object will be passed to the file format
        if (HXR_OK != m_pFile->Seek(0, FALSE) &&
            !m_bGetMimeTypeDone)
        {
            DoFileRecognize();
            HX_RELEASE(m_pBuffer);
        }
    }

    return HXR_OK;
}

STDMETHODIMP CHXFileRecognizer::WriteDone(HX_RESULT /* status */)
{
    return HXR_OK;
}

STDMETHODIMP CHXFileRecognizer::CloseDone(HX_RESULT /* status */)
{
    return HXR_OK;
}

void
CHXFileRecognizer::DoFileRecognize(void)
{
    HX_RESULT   ret = HXR_OK;
    IHXBuffer*  pMimeType = NULL;
    const char* pFileName = NULL;

    m_bGetMimeTypeDone = TRUE;

    if (m_pFile)
	m_pFile->GetFilename(pFileName);

    ret = GetMimeType(pFileName, m_pBuffer, pMimeType);

    if (SUCCEEDED(ret) && 
	pMimeType && 
	pMimeType->GetSize() > 0 && 
	m_pResponse)
    {
	m_pResponse->GetMimeTypeDone(HXR_OK, pMimeType);
    }
    else
    {
	m_pResponse->GetMimeTypeDone(HXR_FAIL, NULL);
    }

    HX_RELEASE(pMimeType);
}
