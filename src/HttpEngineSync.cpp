/*
 ============================================================================
 Name		: HttpEngineSync.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CHttpEngineSync implementation
 ============================================================================
 */

#include "HttpEngineSync.h"
#include <s32mem.h>
#include <FuzzyByte\FileUtils.h>

CHttpEngineSync::CHttpEngineSync()
	{
	// No implementation required
	}

CHttpEngineSync::~CHttpEngineSync()
	{
	delete iHttp;
	iHttp = NULL;
	}

CHttpEngineSync* CHttpEngineSync::NewLC(CHttpSession* aSession)
	{
	CHttpEngineSync* self = new (ELeave) CHttpEngineSync();
	CleanupStack::PushL(self);
	self->ConstructL(aSession);
	return self;
	}

CHttpEngineSync* CHttpEngineSync::NewL(CHttpSession* aSession)
	{
	CHttpEngineSync* self = CHttpEngineSync::NewLC(aSession);
	CleanupStack::Pop(); // self;
	return self;
	}

void CHttpEngineSync::ConstructL(CHttpSession* aSession)
	{
	iHttp = CHttpEngine::NewL(aSession);
	}

void CHttpEngineSync::Cancel()
	{
	iHttp->Cancel();
	}

// POST...
HBufC8* CHttpEngineSync::IssueHTTPPostLC(RFs& aFs, const TDesC8& aUri,
		const TDesC8& aData, const TDesC8& aContentType, TInt& aError,
		TInt aTimeOut, TBool aMultiPart, const TDesC8& aHeaderField,
		const TDesC8& aValue)
	{
	TFileName aFileName;
	RFile aFile;
	User::LeaveIfError(FileUtils::CreateTempFileL(aFs, aFile));
	aFile.FullName(aFileName);
	aFile.Write(aData);
	aFile.Close();
	return IssueHTTPPostLC(aFs, aUri, aFileName, aContentType, aError,
			aTimeOut, aMultiPart, aHeaderField, aValue);
	}

HBufC8* CHttpEngineSync::IssueHTTPPostLC(RFs& aFs, const TDesC8& aUri,
		const TDesC& aFileName, const TDesC8& aContentType, TInt& aError,
		TInt aTimeOut, TBool aMultiPart, const TDesC8& aHeaderField,
		const TDesC8& aValue)
	{
	Cancel();
	CBufBase* buffer = CBufFlat::NewL(100);
	CleanupStack::PushL(buffer);
	RBufWriteStream strm;
	strm.PushL();
	strm.Open(*buffer);

	RBuf8 buf(FileUtils::ReadFileContentsL(aFs, aFileName));
	buf.CleanupClosePushL();

	aError = KErrNone;
	CAsyncWaiter* waiter = CAsyncWaiter::NewLC();

	if (aMultiPart)
		iHttp->Prepare_POST_MultiPart_RequestL(aUri, strm, aFileName,
				aContentType, buf);
	else
		iHttp->Prepare_POST_RequestL(aUri, strm, aContentType, buf);

	if (aHeaderField.Length() > 0)
		iHttp->AddHeaderFieldL(aHeaderField, aValue);

	iHttp->SubmitL(waiter->iStatus);
	waiter->StartAndWaitWithTimeout(aError, aTimeOut);

	// Non si puo' controllare iClient == NULL perche' in caso di cancellazione di questa istanza, viene rimosso anche iClient, quindi non punta mai a NULL
	// KErrCancel viene generato in caso di cancellazione di questa istanza, per tutti gli altri errori, posso tranquillamente chiamare la Cancel su iClient
	// Se si e' verificato un TimeOut non posso distruggere il waiter senza aver prima annullato la richiesta iClient perche' contiene iStatus
	// La chiamata iClient->Cancel() e' necessaria in caso ci sia stato un TimeOut.
	//if (error != KErrCancel && error != KErrNone)
	//	{
	//	iHttp->Cancel(); 
	//	}
	// Controllare KErrTimedOut e' la cosa piu' sicura per decidere se richiamare la Cancel() oppure no.
	// Ed il chiamante dovra' controllare KErrCancel per decidere se notificare all'observer oppure no.
	if (aError == KErrTimedOut)
		iHttp->Cancel();

	/*	if (buffer->Ptr(0).Length() <= 5) 
	 { 
	 #ifdef __WINSCW__  
	 User::InfoPrint(_L("Err Resp"));
	 #endif 
	 } */

	CleanupStack::PopAndDestroy(waiter);
	CleanupStack::PopAndDestroy(&buf);
	CleanupStack::PopAndDestroy(&strm);

	HBufC8* result = buffer->Ptr(0).AllocL();
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PushL(result);
	return result;
	}

/*
 HBufC8* CHttpEngineSync::IssueHTTPPostPictureLC(RFs& aFs, const TDesC8& aUri, TInt aProfileID, const TDesC& aFileName, TInt& aError)
 {
 Cancel();
 CBufBase* buffer = CBufFlat::NewL(100);
 CleanupStack::PushL(buffer);
 RBufWriteStream strm; 
 strm.PushL();
 strm.Open(*buffer);

 RBuf8 buf( FileUtils::ReadFileContentsL(aFs, aFileName) );
 buf.CleanupClosePushL();
 
 aError = KErrNone;
 CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
 iHttp->Prepare_POST_RequestL(aUri, strm, _L8("image/jpeg"), buf);
 
 TBuf8<20> profId;
 profId.AppendNum(aProfileID); 
 iHttp->AddHeaderFieldL(_L8("profileID"), profId);

 TBuf8<30> fname8;
 TParsePtrC parse(aFileName);
 fname8.Copy( parse.NameAndExt() );
 iHttp->AddHeaderFieldL(_L8("userfile"), fname8);
 
 iHttp->SubmitL(waiter->iStatus);
 waiter->StartAndWaitWithTimeout( aError, 60 );
 
 // Non si puo' controllare iClient == NULL perche' in caso di cancellazione di questa istanza, viene rimosso anche iClient, quindi non punta mai a NULL
 // KErrCancel viene generato in caso di cancellazione di questa istanza, per tutti gli altri errori, posso tranquillamente chiamare la Cancel su iClient
 // Se si e' verificato un TimeOut non posso distruggere il waiter senza aver prima annullato la richiesta iClient perche' contiene iStatus
 // La chiamata iClient->Cancel() e' necessaria in caso ci sia stato un TimeOut.
 //if (error != KErrCancel && error != KErrNone)
 //	{
 //	iHttp->Cancel(); 
 //	}
 // Controllare KErrTimedOut e' la cosa piu' sicura per decidere se richiamare la Cancel() oppure no.
 // Ed il chiamante dovra' controllare KErrCancel per decidere se notificare all'observer oppure no.
 if (aError == KErrTimedOut)
 iHttp->Cancel();	
 
 if (buffer->Ptr(0).Length() <= 5) 
 { 
 #ifdef __WINSCW__  
 User::InfoPrint(_L("Err Resp"));
 #endif 
 } 
 
 CleanupStack::PopAndDestroy(waiter); 	
 CleanupStack::PopAndDestroy(&buf);  
 CleanupStack::PopAndDestroy(&strm); 

 HBufC8* result = buffer->Ptr(0).AllocL();
 CleanupStack::PopAndDestroy(buffer);
 CleanupStack::PushL(result);
 return result;
 }*/

TBool CHttpEngineSync::IsRunning()
	{
	return iHttp->IsRunning();
	}

HBufC8* CHttpEngineSync::IssueHTTPGetLC(const TDesC8& aUri, TInt& aError,
		TInt aTimeOut, TInt aHeaderField, const TDesC8& aValue)
	{
	Cancel();
	CBufBase* buffer = CBufFlat::NewL(100);
	CleanupStack::PushL(buffer);
	RBufWriteStream strm;
	strm.PushL();
	strm.Open(*buffer);
	strm.CommitL();

	aError = KErrNone;
	// Non va bene mettere waiter come variabile di istanza perche' 2 richieste consecutive andranno a collidere sul medesimo iStatus...
	// La Cancel() infatti non fa in tempo a segnalare ad iHttp che deve annullare la richiesta precedente.
	CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
	iHttp->Prepare_GET_RequestL(aUri, strm);

	if (aHeaderField > 0)
		iHttp->AddHeaderFieldL(aHeaderField, aValue);

	iHttp->SubmitL(waiter->iStatus);
	waiter->StartAndWaitWithTimeout(aError, aTimeOut);
	// Non si puo' controllare iHttp == NULL perche' in caso di cancellazione di questa istanza, viene rimosso anche iHttp, quindi non punta mai a NULL
	// KErrCancel viene generato in caso di cancellazione di questa istanza, per tutti gli altri errori, posso tranquillamente chiamare la Cancel su iHttp
	// Se si e' verificato un TimeOut non posso distruggere il waiter senza aver prima annullato la richiesta iClient perche' contiene iStatus
	// La chiamata iHttp->Cancel() e' necessaria in caso ci sia stato un TimeOut.
	//if (error != KErrCancel && error != KErrNone)
	//	{
	//	iHttp->Cancel(); 
	//	}
	// Controllare KErrTimedOut e' la cosa piu' sicura per decidere se richiamare la Cancel() oppure no.
	// Ed il chiamante dovra' controllare KErrCancel per decidere se notificare all'observer oppure no.
	if (aError == KErrTimedOut)
		iHttp->Cancel();

	CleanupStack::PopAndDestroy(waiter);
	CleanupStack::PopAndDestroy(&strm);

	/*	if (buffer->Ptr(0).Length() <= 5)
	 {
	 #ifdef __WINSCW__
	 User::InfoPrint(_L("Err Resp"));
	 #endif
	 }*/

	HBufC8* result = buffer->Ptr(0).AllocL();
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PushL(result);
	return result;
	}

