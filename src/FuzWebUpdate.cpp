
#include "FuzWebUpdate.h"
#include "phone.h"
#include "httpEngine.h"
#include "asyncwaiter.h"
#include <S32MEM.H>
#include <hal.H>

#define KIMEI_LEN 20
typedef TBuf<KIMEI_LEN> TImei;
 

/*
#ifdef __WINSCW__
_LIT8(KHttpURL8, "http://localhost:4257/GuardianMobile/CheckUpdates.aspx?");
#else
_LIT8(KHttpURL8, "http://www.Fuzzy-Byte.com/CheckUpdates.aspx?");
//_LIT8(KHttpURL8, "http://www.Guardian-Mobile.com/CheckUpdates.aspx?");
#endif
*/

// Upload pagina CheckUpdates.aspx su Fuzzy-Byte.com

void CFuzWebUpdate::ConstructL(const TDesC& aServerUrl)
    {
	__LOG_OPEN("CommonLib", "webupdate.txt");
	__LOG_TXT("**********");
	User::LeaveIfError( iSocketServ.Connect() );
    iSession = CHttpSession::NewL();
    iClient = CHttpEngine::NewL(iSession);
    iServerUrl.Create( aServerUrl );
    }


CFuzWebUpdate* CFuzWebUpdate::NewL(const TDesC& aServerUrl)
    {
    CFuzWebUpdate* self = CFuzWebUpdate::NewLC(aServerUrl);
    CleanupStack::Pop( self );
    return self;
    }


CFuzWebUpdate* CFuzWebUpdate::NewLC(const TDesC& aServerUrl)
    {
    CFuzWebUpdate* self = new (ELeave) CFuzWebUpdate();
    CleanupStack::PushL( self );
    self->ConstructL(aServerUrl);
    return self;
    }


CFuzWebUpdate::CFuzWebUpdate()
    {
    }


CFuzWebUpdate::~CFuzWebUpdate()
    {
	__LOG_TXT("Destructor");
	TRAP_IGNORE( CancelDownloadL() );
	if (iClient)
	{
		delete iClient;
		iClient = NULL;
	}
    delete iSession;
    iSocketServ.Close();
    iServerUrl.Close();
	__LOG_TXT("End Destructor");
	__LOG_CLOSE;
    }


// Restituisce KErrNone se ha scaricato l'aggiornamento.
// Restituisce altro se non ha scaricato l'aggiornamento oppure se c'e' stato qualche probl. di comunicazione.
TInt CFuzWebUpdate::DownloadUpdatedVersionL(const TDesC8& downloadURL, RFs& fs, const TDesC& filename)
	{
/*	CBrowserLauncher* browser;
	MDownloadedContentHandler* downloadHandler;
	browser = CBrowserLauncher::NewLC();
#ifdef __WINSCW__
	browser->LaunchBrowserSyncEmbeddedL(_L("http://localhost:1105/GuardianMobile/"));
#else
	browser->LaunchBrowserSyncEmbeddedL(_L("http://www.Guardian-Mobile.com/"));
#endif
	CleanupStack::PopAndDestroy();
	Il problema e' che non si sa dove va a finire l'aggiornamento...
*/

	// Issue HTTP get to engine; first cancel possible existing transaction
	RFileWriteStream strm;
	User::LeaveIfError( strm.Replace(fs, filename, EFileShareReadersOrWriters|EFileWrite) );
	CleanupClosePushL(strm);

    // Issue HTTP get to engine; first cancel possible existing transaction
	TInt error = KErrNone;
	CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
	if (!iSession->IsOpened())
		{
		iSession->Open(iSocketServ, waiter->iStatus);
		waiter->StartAndWaitWithTimeout( error, 60 );
		}
	if (error == KErrNone)
		{
		iClient->Prepare_GET_RequestL(downloadURL, strm);
		iClient->SubmitL(waiter->iStatus);
		waiter->StartAndWaitWithTimeout( error, 120 );
		}

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
		iClient->Cancel();	

	CleanupStack::PopAndDestroy(waiter);
	/* 
	iClient->IssueHTTPGetL( downloadURL, strm, iWaiter->iStatus );
	iWaiter->StartAndWaitWithTimeout( error, 120 );
	iClient->CancelHTTPGet();*/

	__LOG_TXT("Pop Stream");
	CleanupStack::PopAndDestroy(); // Stream

	return (error);
	}


TInt CFuzWebUpdate::IsThereAnUpdatedVersionL(TUint32 isReg, const TDesC& version, TUint32 brandId, TDes8& url, TBool& updateAvail)
	{
	updateAvail = EFalse;
	url.Zero();

	TInt intMac;
	HAL::Get(HALData::EMachineUid, intMac);

	TImei imei;
	CPhone* info = CPhone::NewL();
	CleanupStack::PushL(info);
	info->GetImei(imei);
	CleanupStack::PopAndDestroy(info);	// info
	
	// Rileva UID dell'applicazione che sta inviando la richiesta...
	//TInt id=RProcess().SecureId().iId;
	TLanguage lang = User::Language();
	
	// Rileva UID dell'applicazione che sta inviando la richiesta...
	TUint32 id=RProcess().SecureId().iId;
	
	RBuf8 url8;
	url8.CleanupClosePushL();
	url8.CreateL(300);
	url8.Append( iServerUrl );
	url8.Append(_L8("Imei="));
	url8.Append(imei);
	url8.Append(_L8("&reg="));
	url8.AppendNum(isReg);
	url8.Append(_L8("&m_lang="));
	url8.AppendNum(lang);
	url8.Append(_L8("&Mac="));
	url8.AppendNum(intMac, EHex);
	url8.Append(_L8("&BrandId="));
	url8.AppendNum(brandId);
	url8.Append(_L8("&Version="));
	url8.Append(version);
	url8.Append(_L8("&SoftwareID="));
	url8.AppendNum(id, EHex);

	TBuf8<200> buff;
	RDesWriteStream strm(buff);
	CleanupClosePushL(strm);

    // Issue HTTP get to engine; first cancel possible existing transaction
	__LOG_TXT("StartIssueHTTPGet");
	TInt error = KErrNone;
	CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
	if (!iSession->IsOpened())
		{
		iSession->Open(iSocketServ, waiter->iStatus);
		waiter->StartAndWaitWithTimeout( error, 60 );
		}
	if (error == KErrNone)
		{
		iClient->Prepare_GET_RequestL(url8, strm);
		iClient->SubmitL(waiter->iStatus);
		waiter->StartAndWaitWithTimeout( error, 60 );
		}
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
		iClient->Cancel();	
	
	CleanupStack::PopAndDestroy(waiter); 
	/* 
	iClient->IssueHTTPGetL( url8, strm, iWaiter->iStatus );
	iWaiter->StartAndWaitWithTimeout( error, 60 );
	iClient->CancelHTTPGet();*/
	//__LOG("EndIssueHTTPGet:%d"), iWaiter->iStatus);

	__LOG_TXT("Pop Stream");
	CleanupStack::PopAndDestroy(&strm); 
	CleanupStack::PopAndDestroy(&url8); 

	__LOG("Get Result:%d", error);
	if (error != KErrNone)
		return error;

	__LOG_DES(buff);
	GetResponseURL(buff, url);
	__LOG_DES(url);
	if (url.Length() > 0)
		updateAvail = ETrue;
	return error;
	}


void CFuzWebUpdate::GetResponseURL(const TDesC8& webData, TDes8& url)
{
	__LOG_TXT("GetResponseURL");
	url.Zero();
	TInt posFile = webData.Find(_L8("file="));
	__LOG("PosFile:%d", posFile);
	if (posFile < 0)
		return;
	TPtrC8 fileData = webData.Mid(posFile + 5);
	__LOG_DES(fileData);
	TInt posCrLf = fileData.Find(_L8("\n"));
	__LOG("PosCrLf:%d", posCrLf);
	if (posCrLf <= 0)
		return;
	url = fileData.Mid(0, posCrLf);
}


void CFuzWebUpdate::CancelDownloadL()
	{
	// Cancella la richiesta attualmente in corso...
	__LOG_TXT("CancelDownloadL");
	iClient->Cancel();
	}

