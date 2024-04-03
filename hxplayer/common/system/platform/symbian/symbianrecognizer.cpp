#include <apgcli.h> // RApaLsSession
#include <apmrec.h> // CApaDataRecognizerType
#include <apmstd.h>
#include <utf.h>

#include "hxresult.h"
#include "hxbuffer.h"

#include "recognizer.h"

static TDataType GetMimeTypeL(const TDesC& fileName, const TDesC8& fileBuf)
{
    TDataType type;

    RApaLsSession ls;
    if (ls.Connect() == KErrNone)
    {
	TDataRecognitionResult rr;
	ls.RecognizeData(fileName, fileBuf, rr);
	    
	if (rr.iConfidence != CApaDataRecognizerType::ENotRecognized)
	{
	    type = rr.iDataType;
	}

	ls.Close();
    }

    return type;
}

STDMETHODIMP CHXFileRecognizer::GetMimeType(const char* pFileName,
					    IHXBuffer* pBuffer,
					    REF(IHXBuffer*) pMimeType)
{
    HX_RESULT ret = HXR_FAILED;

    if (pBuffer && pBuffer->GetSize() > 0)
    {
	TInt err = KErrNoMemory;

        if (IsSDPFile(pBuffer))
        {
            // convert descriptor MIME type to IHXBuffer
            pMimeType = new CHXBuffer;
            if (pMimeType)
            {
                pMimeType->AddRef();

                int len = strlen("application/sdp");
	        pMimeType->Set((const UCHAR*)"application/sdp", len + 1);
	        ((char*)pMimeType->GetBuffer())[len] = '\0';
	        return HXR_OK;
            }
        }

	// convert file name to descriptor
	TPtrC8 from((TUint8*)pFileName);
	HBufC* pUCFileName = HBufC::New(from.Length());

	if (pUCFileName)
	{
	    TPtr to = pUCFileName->Des();
	    err = CnvUtfConverter::ConvertToUnicodeFromUtf8(to, from);
	}

	if (KErrNone == err)
	{
	    // convert the file buffer to a descriptor
	    TPtrC8 fileBuf((TUint8*)pBuffer->GetBuffer(), 
			   pBuffer->GetSize());
		
	    TDataType dataType;
	    TRAP(err, dataType = GetMimeTypeL(*pUCFileName, fileBuf));

	    if (KErrNone == err)
	    {
		// convert descriptor MIME type to IHXBuffer
		pMimeType = new CHXBuffer;
		pMimeType->AddRef();
		if (pMimeType)
		{
		    TPtrC8 p = dataType.Des8();
		    int len = p.Length();
		    pMimeType->Set((const UCHAR*)p.Ptr(), len + 1);
		    ((char*)pMimeType->GetBuffer())[len] = '\0';
		    ret = HXR_OK;
		}
	    }
	}
	delete pUCFileName;
    }

    return ret;
}
