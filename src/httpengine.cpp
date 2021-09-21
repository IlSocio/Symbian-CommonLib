/*
 ============================================================================
 Name		: HttpEngine.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Guardian-Mobile
 Description : CHttpEngine implementation
 ============================================================================
*/

#include "HttpEngine.h"

CHttpEngine::CHttpEngine(CHttpSession* session)
	{
	iSession = session;
	}

CHttpEngine* CHttpEngine::NewLC(CHttpSession* session)
	{ 
	CHttpEngine* self = new (ELeave) CHttpEngine(session);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHttpEngine* CHttpEngine::NewL(CHttpSession* session)
	{
	CHttpEngine* self = CHttpEngine::NewLC(session);
	CleanupStack::Pop(); // self;
	return self;
	}

void CHttpEngine::ConstructL()
	{
	__LOG_OPEN("CommonLib", "httpengine.txt");
	__LOG_TXT("**********"); 
    iState = EIdle;
    iFileBuf.Create(16384);
	}

CHttpEngine::~CHttpEngine()
	{
	__LOG_TXT("Destructor");
	Cancel(); 		// Cancel any request, if outstanding

	iTransaction.Close();	// Added on 28 / Giu / 2010
	iUri8.Close();

	iFileBuf.Close();
	__LOG_TXT("End Destructor");
	__LOG_CLOSE;
	}


TBool CHttpEngine::IsRunning()
	{
	return iRunning;
/*	// iReqStatus potrebbe essere stato completato da qualcun'altro... per timeout ad esempio...
	if (iReqStatus == NULL)
		return EFalse;
	return (*iReqStatus == KRequestPending);*/
	}


TBool CHttpEngine::CompleteIfRunning(TInt errCode)
	{
	if (!iRunning)
		return EFalse;
	if (iState == ETransactionInProgress)
	{
		iState = EIdle;
		iTransaction.Close();
	}
	if (iReqStatus != NULL && *iReqStatus == KRequestPending)
		User::RequestComplete(iReqStatus, errCode);
	iRunning = EFalse;
	return ETrue;
	}
 
/*
CBufBase* buffer = CBufFlat::NewL(50);
CleanupStack::PushL(buffer);
RBufWriteStream strm;
CleanupClosePushL(strm);
strm.Open(*buffer);
*/

void CHttpEngine::Prepare_GET_RequestL(const TDesC8& aUri, RWriteStream& aStream)
	{
	iFileBuf.Close();
	PrepareRequestL(HTTP::EGET, aUri, aStream);
	}

void CHttpEngine::Prepare_POST_RequestL(const TDesC8& aUri, RWriteStream& aStream, const TDesC8& aContentType, const TDesC8& aData)
	{
	iFileBuf.Close();
	iFileBuf.Create( aData.Length() );
	iFileBuf.Copy(aData);
	PrepareRequestL(HTTP::EPOST, aUri, aStream, aData);
	AddHeaderFieldL(HTTP::EContentType, aContentType);
	}

_LIT8(KBoundary, "2haO4q");
_LIT8(KDelim,"--");
_LIT8(KCrlf,"\r\n");
_LIT8(KContent,"Content-Disposition: form-data; name='userfile'; filename='");
_LIT8(KFileCompletion,"'"); 
_LIT8(KContent2,"Content-Type: ");
_LIT8(KContent3,"Content-Transfer-Encoding: binary");

void CHttpEngine::Prepare_POST_MultiPart_RequestL(const TDesC8& aUri, RWriteStream& aStream, const TDesC& aFileName, const TDesC8& aContentType, const TDesC8& aData)
	{
	iFileBuf.Close();
	iFileBuf.Create( aData.Length() + 1000 );
	
	PrepareRequestL(HTTP::EPOST, aUri, aStream, aData);
	TBuf8<50> contType = _L8("multipart/form-data; boundary=");
	contType.Append(KBoundary);
	AddHeaderFieldL(HTTP::EContentType, contType);
	// TODO: genera boundary
	iIsMultiPart = ETrue;
	iFileBuf.Append(KCrlf);
	iFileBuf.Append(KDelim);
	iFileBuf.Append(KBoundary);
	iFileBuf.Append(KCrlf);
	iFileBuf.Append(KContent);
	TParsePtrC parse(aFileName);
	iFileBuf.Append(parse.NameAndExt());
	iFileBuf.Append(KFileCompletion);
	iFileBuf.Append(KCrlf);
	iFileBuf.Append(KContent2);
	iFileBuf.Append(aContentType);
	iFileBuf.Append(KCrlf);
	iFileBuf.Append(KContent3);
	iFileBuf.Append(KCrlf);
	iFileBuf.Append(KCrlf);
	// ... <BYTES> ...
	iFileBuf.Append(aData);
	// ... <BYTES> ...
	iFileBuf.Append(KCrlf);
	iFileBuf.Append(KDelim);
	iFileBuf.Append(KBoundary);
	iFileBuf.Append(KDelim);
	iFileBuf.Append(KCrlf);
	}

void CHttpEngine::PrepareRequestL(TInt aGetPostIndex, const TDesC8& aUri, RWriteStream& aStream, const TDesC8& aData)
	{
	__LOG_TXT("PrepareRequestL");
	__LOG_DES(aUri);
	if (!iSession->IsOpened())
		User::Panic(_L("HttpEngine - Session"),555);
	Cancel();
	iGetPostIndex = aGetPostIndex;
	iUri8.Close();
	iUri8.Create(aUri);
	
	iRespStream = &aStream;

	// Si Assume che iSession sia gia' stata aperta.
	iTransaction.Close();
	iTransaction = iSession->OpenTransactionL(iUri8, *this, iGetPostIndex);

	if (iGetPostIndex == HTTP::EPOST)
		{
		iTransaction.Request().SetBody(*this);
		}
	}



void CHttpEngine::AddHeaderFieldL(const TDesC8& aField, const TDesC8& aValue)
	{
	if (iState != EIdle)
		User::Panic(_L("NotInitialized"), 2001);
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	iSession->SetHeaderL( hdr, aField, aValue );	
	}


void CHttpEngine::AddHeaderFieldL(TInt aHeaderField, const TDesC8& aValue)
	{
	// HTTP::EAuthorization	
	if (iState != EIdle)
		User::Panic(_L("NotInitialized"), 2001);
	RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
	iSession->SetHeaderL( hdr, aHeaderField, aValue );
	}


void CHttpEngine::SubmitL(TRequestStatus& aStatus)	
	{
	__LOG("SubmitL:%d", iState);
	if (iRunning)
		User::Panic(_L("Running"), 2000);
	iRunning = ETrue;
	iReqStatus = &aStatus;
	*iReqStatus = KRequestPending;
	iState = ETransactionInProgress;
	iTransaction.SubmitL();
	}


void CHttpEngine::Cancel()
	{
	CompleteIfRunning( KErrCancel );
	}




// ----------------------------------------------------------------------------
// CWebClientEngine::MHFRunL()
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ----------------------------------------------------------------------------
//
void CHttpEngine::MHFRunL( RHTTPTransaction aTransaction,
                                const THTTPEvent& aEvent )
    {
    switch ( aEvent.iStatus )	// KErrCouldNotConnect	-34
        {
        case THTTPEvent::EGotResponseHeaders:
        	{
			__LOG_TXT("GotHeaders");
            // HTTP response headers have been received. Use
            // aTransaction.Response() to get the response. However, it's not
            // necessary to do anything with the response when this event occurs.

            // Get HTTP status code from header (e.g. 200)
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();
			__LOG("Resp:%d", status);

			// Wind MMS mi da response 403
			if (status != 200)
				{
				CompleteIfRunning( -status );
				}
/*			if (status != 200)
			{
		        TBuf<KMaxStatusTextLength> statusText;
	            statusText.Copy( resp.StatusText().DesC() );
				statusText.AppendNum( status );
				iObserver.ClientErrorL(status, statusText);
				CompleteIfRunning( KErrGeneral );
			}*/
			break;
        	}

        case THTTPEvent::EGotResponseBodyData:
        	{
			__LOG_TXT("GotBodyChunk");
			MHTTPDataSupplier* body = aTransaction.Response().Body();
            TPtrC8 dataChunk;

            // GetNextDataPart() returns ETrue, if the received part is the last one.
			TBool isLast = body->GetNextDataPart( dataChunk );

			// Stream the dataChunk...
			__LOG_DES(dataChunk);
			TRAPD(err, iRespStream->WriteL( dataChunk ))
			if (err != KErrNone)
				{
				__LOG_TXT("Error Writing to Stream");
				}
			TRAP(err, iRespStream->CommitL() );
			if (err != KErrNone)
				{
				__LOG_TXT("Error Commit to Stream");
				}

			body->ReleaseData();
			if (isLast)
				{
				__LOG_TXT("Last Chunk Received!");
				CompleteIfRunning( KErrNone );
				}
			break;
        	}

        case THTTPEvent::ESucceeded:
        	{
			__LOG_TXT("GotSucceded");
			CompleteIfRunning( KErrNone );
            break;
        	}

        case THTTPEvent::EFailed:
        	{
			__LOG_TXT("GotFailed");
			CompleteIfRunning( KErrGeneral );
            break;
        	}

        case THTTPEvent::EResponseComplete:
        	{
			__LOG_TXT("GotResponseComplete");
            break;
        	}

        default:
        	{
			__LOG("GotUnknEvent:%d", aEvent.iStatus);
        	// -7361 poi passa a failed
        	if (aEvent.iStatus < KErrNone)
        		CompleteIfRunning( aEvent.iStatus );	// TODO: Maybe we have to remove this one...
            break;
        	}
        }
    }

// ----------------------------------------------------------------------------
// CWebClientEngine::MHFRunError()
// Inherited from MHTTPTransactionCallback
// Called by framework when *leave* occurs in handling of transaction event.
// These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
// ----------------------------------------------------------------------------
//
TInt CHttpEngine::MHFRunError( TInt aError,
                                    RHTTPTransaction /*aTransaction*/,
                                    const THTTPEvent& /*aEvent*/)
    {
	__LOG("MHFRunError:%d", aError);
    CompleteIfRunning( aError );
	return KErrNone;
    }




// POST Methods....

const TInt KChunk=20000;

TBool CHttpEngine::GetNextDataPart(TPtrC8& aDataPart)
{
	if (iBytesLeft <= 0)
		return ETrue;
	TInt toRead = iBytesLeft;
	if (toRead > KChunk)
		toRead = KChunk;
	aDataPart.Set( iFileBuf.RightTPtr(iBytesLeft).Ptr(), toRead ); 
	iBytesLeft -= toRead;
	
	return (iBytesLeft <= 0);
}

void CHttpEngine::ReleaseData()
{
	if (iBytesLeft == 0)
		{
		iFileBuf.Close();
		return;
		}
	
	// Notify HTTP of more data available immediately, since it's being read from file
	TRAPD(err, iTransaction.NotifyNewRequestBodyPartL());
	/*if (err != KErrNone) {
		User::Panic(KHttpExampleClientPanic, KCouldntNotifyBodyDataPart);
	}*/
}

TInt CHttpEngine::OverallDataSize()
	{	
	TInt size = 0;
	size = iFileBuf.Length();
	iBytesLeft = size;
	/*iFile.Size(size);
	TInt err = iFile.Size(size);
	  if (err < 0)
		User::Panic(KHttpExampleClientPanic, KBodyWithInvalidSize);*/

	return size;
	}

TInt CHttpEngine::Reset()
	{
	return KErrNotSupported;
	}


