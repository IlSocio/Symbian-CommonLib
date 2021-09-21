

#include "FuzWebRegister.h"
#include <S32MEM.H>
#include "Phone.h"
//#include <e32hashtab.h>

// Schemes for given uris
//_LIT8(KHttpURL8, "http://localhost:1035/website1/signature.bin");
//_LIT8(KHttpURL8, "http://www.navisluni.com/Notification.aspx?LicenseID=1111-2222-4444-2045&IMEI=123456789012345&SoftwareID=10203040&Test=TRUE&ApplicationData=VABFAFMAVABPAEQASQBQAFIATwBWAEEA");
//_LIT8(KHttpURL8, "http://www.navisluni.com/Notification.aspx?Test=TRUE");
//_LIT8(KHttpURL8, "http://www.symbian-toys.com/LicSrv/LicRequest.aspx?Test=TRUE");
//_LIT8(KHttpURL8, "http://www.symbian-toys.com/default.aspx?Test=TRUE");

#define KIMEI_LEN 20
typedef TBuf<KIMEI_LEN> TImei;


/*
#ifdef __WINSCW__
_LIT8(KHttpURL8, "http://localhost:1105/GuardianMobile/LicRequest.aspx?");
#else
_LIT8(KHttpURL8, "http://www.Fuzzy-Byte.com/LicRequest.aspx?");
#endif
*/

void CFuzWebRegister::ConstructL(const TDesC& aServerUrl)
    {
	iServerUrl.CreateL( aServerUrl );
	User::LeaveIfError( iSocketServ.Connect() );
    iSession = CHttpSession::NewL();
    iClient = CHttpEngine::NewL(iSession);
    // TInt ris = GetResponseStatus(_L8("Status:0"));
    }


CFuzWebRegister* CFuzWebRegister::NewL(const TDesC& aServerUrl)
    {
    CFuzWebRegister* self = CFuzWebRegister::NewLC(aServerUrl);
    CleanupStack::Pop( self );
    return self;
    }


CFuzWebRegister* CFuzWebRegister::NewLC(const TDesC& aServerUrl)
    {
    CFuzWebRegister* self = new (ELeave) CFuzWebRegister();
    CleanupStack::PushL( self );
    self->ConstructL(aServerUrl);
    return self;
    }


CFuzWebRegister::CFuzWebRegister()
    {
    }


CFuzWebRegister::~CFuzWebRegister()
    {
    if ( iClient )
        {
        delete iClient;
        iClient = NULL;
        }
    delete iSession;
    iSocketServ.Close();
    iServerUrl.Close();
    }



TInt CFuzWebRegister::SendRegistrationRequestL(TUint32 brandId, const TDesC& version, const TDesC& licenseID, TDes& regCode, TResponseID& resp)
    {
	TImei imei;
	CPhone* info = CPhone::NewL();
	CleanupStack::PushL(info);
	info->GetImei(imei);
	CleanupStack::PopAndDestroy(info);	// info

	// Rileva UID dell'applicazione che sta inviando la richiesta...
	TUint32 id=RProcess().SecureId().iId;

	RBuf8 url8;
	url8.CleanupClosePushL();
	url8.CreateL(300);
	url8.Append( iServerUrl );
	url8.Append(_L8("BrandId="));
	url8.AppendNum(brandId);
	url8.Append(_L8("&Version="));
	url8.Append(version);
	url8.Append(_L8("&LicenseID="));
	url8.Append(licenseID);
//	url8.Append(_L8("&TEST=TRUE"));
	url8.Append(_L8("&IMEI="));
	url8.Append(imei);
	url8.Append(_L8("&SoftwareID="));
	url8.AppendNum(id, EHex);

/*	// Converte customApplicationData in Base64
	RBuf8 inBase64;
	inBase64.CleanupClosePushL();
	inBase64.CreateL(100);
	CBase64CodecBase* base64 = new (ELeave) CBase64CodecBase();
	CleanupStack::PushL(base64);
	base64->Encode( customApplicationData, inBase64 );
	CleanupStack::PopAndDestroy(base64); // base64
	url8.Append(_L8("&ApplicationData="));
	url8.Append(inBase64);
	CleanupStack::PopAndDestroy(); // inBase64*/

	TBuf8<200> buff;
	RDesWriteStream strm(buff);
	CleanupClosePushL(strm);

    // Start transaction
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
	
	// Non si puo' neppure controllare iClient perche' questa istanza e' stata rimossa ed anche iHttp, quindi non punta mai a NULL
	// if (iClient)
	// 	iClient->Cancel();
	CleanupStack::PopAndDestroy(waiter);
	/*
	iClient->IssueHTTPGetL( url8, strm, iWaiter->iStatus );
	iWaiter->StartAndWaitWithTimeout( error, 60 );
	iClient->CancelHTTPGet();*/

	CleanupStack::PopAndDestroy(&strm); // Stream
	CleanupStack::PopAndDestroy(&url8); // url8

	resp = EErrNone;
	if (error == KErrCancel)
	{
		resp = EErrNone;
		return error;
	}
	if (error == KErrGeneral)
	{
		resp = ETransmissionError;
		return error;
	}

	// TODO: Parse di buff e popola array con Chiave, Valore...
	// TODO: Poi si cerchera' nell'array se ci sono le keywords...
	// TODO: Usare una RHashMap?
	//  RHashMap<TDesC16, TDesC16> hashMapDes16();

	
	resp = GetResponseStatus( buff );
	if (resp != EErrNone)
	{
		return error;
	}

//	TBuf<10> regCode;
	GetResponseRegist(buff, regCode);
	if (regCode.Length() != 8)
	{
		resp = EUnknownDataReceived;
	}
//	iObserver.GotRegCodeL( regCode );
	return error;
    }


void CFuzWebRegister::CancelRegistrationRequestL()
	{
	iClient->Cancel();
	}


TResponseID CFuzWebRegister::GetResponseStatus(const TDesC8& data)
{
	if (data.Length() < 7)
		return EUnknownDataReceived;
	if (data.Left(7).MatchC(_L8("Status:")) != 0)
		return EUnknownDataReceived;
	TInt i=7;
	while (i<data.Length() && data[i] >= '0' && data[i] <= '9')
		i++;
	if (i == 7)
		return EUnknownDataReceived;
	TInt ris;
	TPtrC8 ptrNum = data.Mid(7, i-7);
	TLex8 val(ptrNum);
	val.Val(ris);
	if (ris >= EMaxResponseId || ris < 0)
		return EUnknownDataReceived;
	return (TResponseID) ris;
}


void CFuzWebRegister::GetResponseRegist(const TDesC8& data, TDes& regcode)
{
	regcode.Zero();
	TInt pos = data.Find(_L8("Code:"));
	if (pos < 0) return;
	if (pos + 5 + 8 > data.Length()) return;
	TPtrC8 ptrCode = data.Mid(pos + 5, 8);
	regcode.Copy(ptrCode);
	regcode.TrimAll();
}
