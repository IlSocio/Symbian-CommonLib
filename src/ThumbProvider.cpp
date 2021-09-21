/*
 ============================================================================
 Name		: ThumbProvider.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CThumbProvider implementation
 ============================================================================
 */

#include "ThumbProvider.h"
#include "FileUtils.h" 
#include "ASyncWaiter.h"

CThumbProvider::CThumbProvider(MThumbProviderCallBack& aObserver) : iObserver(aObserver)
	{
	// No implementation required
	}

CThumbProvider::~CThumbProvider()
	{
	Cancel(); 
	delete iImageReader; 
	delete iHttp; 
	iHttp = NULL;	
	// necessario perche' si usa asyncwaiter e viene controllato in caso di KErrCancel
	// No, non e' vero perche' tutte le variabili di istanza vengono rimosse a causa della distruzione di questo oggetto e non sara' piu possibile accedere a iHttp per verificare che e' NULL
	}

CThumbProvider* CThumbProvider::NewLC(MThumbProviderCallBack& aObserver, CHttpSession* aSession)
	{
	CThumbProvider* self = new (ELeave) CThumbProvider(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSession);
	return self;
	}

CThumbProvider* CThumbProvider::NewL(MThumbProviderCallBack& aObserver, CHttpSession* aSession)
	{
	CThumbProvider* self = CThumbProvider::NewLC(aObserver, aSession);
	CleanupStack::Pop(); // self;
	return self;
	}

void CThumbProvider::ConstructL(CHttpSession* aSession)
	{
	iImageReader = CImageReader::NewL(*this);
	if (aSession != NULL)
		iHttp = CHttpEngine::NewL(aSession);
		
	RFs& fs = CCoeEnv::Static()->FsSession();
	iTempPath = _L("ThumbProviderThumb\\");
	FileUtils::CompleteWithPrivatePathL(fs, iTempPath);
	ClearTempPathL(fs);
	fs.MkDirAll(iTempPath);
	}


void CThumbProvider::ClearTempPathL(RFs& aFs)
	{
	CFileMan* fileMan = CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	TFullName dirName = iTempPath;
	dirName.Append(_L("*"));
	fileMan->RmDir(dirName);
	fileMan->RmDir(iTempPath);
	CleanupStack::PopAndDestroy(fileMan);	
	}


void CThumbProvider::GetTempFileNameL(const TDesC& aUri, TDes& fname)
	{
	RFs& fs = CCoeEnv::Static()->FsSession();
	RFile file;
	TFileName newFileName;
	file.Temp(fs, iTempPath, newFileName, EFileShareReadersOrWriters | EFileStream | EFileWrite);
	file.Close();
	fname = newFileName;
	TParsePtrC parse(aUri);
    fname.Append(parse.Ext());
	}


void CThumbProvider::ImageReadyL(const TInt& aError)
	{
	iObserver.ThumbReadyL(aError);
	}

CFbsBitmap* CThumbProvider::GetBitmap()
	{
	return iImageReader->GetBitmap();
	}

void CThumbProvider::ProcessImageL(const TDesC& aImgPath, TSize aNewSize)
	{
	RFs& fs = CCoeEnv::Static()->FsSession();
	TBool isURL = (aImgPath.Find(_L("//")) >= 0);

	if (!isURL)
		{
		// Se e' locale la processa ed esce.
		iImageReader->LoadImageL(fs, aImgPath, aNewSize);
		return;
		}
	
	// Se e' remota fa il download poi parte il process dell'immagine locale
	TFileName imgFileName;
	GetTempFileNameL(aImgPath, imgFileName);
	TInt res = DownloadImageL(aImgPath, imgFileName);
	if (res == KErrCancel)
		{
		// Nel caso in cui venga distrutto mentre questa operazione e' in corso, viene restuito error = KErrCancel
		// Poi pero' non si puo' richiamare nulla di questo oggetto perche' e' stato distrutto!!!
		return;
		}
	if (res != KErrNone)
		{ 
		ImageReadyL(res);
		return;
		}
	ProcessImageL(imgFileName, aNewSize);
	}


TInt CThumbProvider::DownloadImageL(const TDesC& aUrl, const TDesC& aImgPath)
	{
	if (iHttp == NULL)
		{
		// E' stato richiesto di processare una immagine remota ma non si e' fornita una session...
		return KErrArgument;
		}
	RFs& fs = CCoeEnv::Static()->FsSession();

	RFileWriteStream file;
	CleanupClosePushL(file);
	TInt err = file.Replace(fs, aImgPath, EFileShareReadersOrWriters | EFileStream | EFileWrite);
	if (err != KErrNone)
		{
		User::LeaveIfError(err);
		}

	TInt error = KErrNone;
	TBuf8<500> url8;
	url8.Copy(aUrl);
	// Non va bene mettere waiter come variabile di istanza perche' 2 richieste consecutive andranno a collidere sul medesimo iStatus...
	// La Cancel() infatti non fa in tempo a segnalare ad iHttp che deve annullare la richiesta precedente.
	CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
	iHttp->Prepare_GET_RequestL(url8, file);
	iHttp->SubmitL(waiter->iStatus);
	waiter->StartAndWaitWithTimeout( error, 60 );

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
	if (error == KErrTimedOut)
		iHttp->Cancel();	

	CleanupStack::PopAndDestroy(waiter); 
	CleanupStack::PopAndDestroy(&file); 
	return error;
	}


void CThumbProvider::Cancel()
	{
	if (iHttp)
		iHttp->Cancel();
	iImageReader->Cancel();
	}
