/*
 ============================================================================
 Name		: SessionEngine.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Guardian-Mobile
 Description : CSessionEngine implementation
 ============================================================================
 */

#include "HttpSession.h"

CHttpSession::CHttpSession() :
	CActive(EPriorityStandard) // Standard priority
	{
	}

CHttpSession* CHttpSession::NewLC()
	{
	CHttpSession* self = new (ELeave) CHttpSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHttpSession* CHttpSession::NewL()
	{
	CHttpSession* self = CHttpSession::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CHttpSession::ConstructL()
	{
	CActiveScheduler::Add(this); // Add to scheduler
	__LOG_OPEN("CommonLib", "httpsession.txt");
	__LOG_TXT("**********");
	}

CHttpSession::~CHttpSession()
	{
	__LOG_TXT("Destructor");
	Cancel(); // Cancel any request, if outstanding
	Close();
	__LOG_TXT("End Destructor");
	__LOG_CLOSE;
	}


void CHttpSession::Close()
	{	
	iConnection.Close();
	iHttpSession.Close();
	iState = EUninitialized;
	}


TBool CHttpSession::IsOpened()
	{
	return (iState == EInitialized);
	}


// Callback
void CHttpSession::Open(RSocketServ& socketServ, TRequestStatus& aStatus)
	{
	__LOG_TXT("Open");
	iReqStatus = &aStatus;
	*iReqStatus = KRequestPending;
	
	if (iState == EInitialized)
		{
		SetActive();
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, KErrNone);
		return;
		}
	Cancel();
	Close();
	TRAPD(err, iConnection.Open( socketServ ) );
	if (err != KErrNone)
		{
		__LOG("Err Conn.Open:%d", err);
		iState = EUninitialized;
		SetActive();
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, err); 
		return;
		}
	
	/* HTTP API for WAP:	 
	//First when you create http session you nee to call the Open method with WSP parameter. (It tells the http stack to use WSP protocolinstead of HTTP)
		–_LIT8(KWspProtString,"WSP/WSP");
		–iSession.OpenL(KWspProtString)
	•After this you need to call the Connect Method
		–iSession.ConnectL();
	•Rest is the same as with HTTP
	*/
	
	TRAPD(err2, iHttpSession.OpenL() );
	if (err2 != KErrNone)
		{
		__LOG("Err Sess.Open:%d", err2);
		iState = EUninitialized;
		SetActive();
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, err2);
		return; 
		}

	iSrvHandle = THTTPHdrVal( socketServ.Handle() );
	iState = EConnectionStart;
	 
	TCommDbConnPref connectPref; 
	connectPref.SetDialogPreference( ECommDbDialogPrefPrompt );
	connectPref.SetBearerSet( ECommDbBearerGPRS | ECommDbBearerWLAN );
	connectPref.SetDirection( ECommDbConnectionDirectionOutgoing );
	
	iConnection.Start( connectPref, iStatus );	// Genera una Leave -11 (KErrAlreadyExists)
	SetActive();
	// A quanto pare non e' possibile cancellare la RConnection.Start()
	}


void CHttpSession::SetHeaderL( RHTTPHeaders aHeaders, const TDesC8& aHdrField, const TDesC8& aHdrValue )
    {
    RStringF valStr = iHttpSession.StringPool().OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    
    RStringF field = iHttpSession.StringPool().OpenFStringL( aHdrField );
    CleanupClosePushL( field );
    
    aHeaders.SetFieldL(field, val);

    CleanupStack::PopAndDestroy(&field);    	
    CleanupStack::PopAndDestroy(&valStr);
    }


void CHttpSession::SetHeaderL( RHTTPHeaders aHeaders,
                                   TInt aHdrField,
                                   const TDesC8& aHdrValue )
    {
    RStringF valStr = iHttpSession.StringPool().OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    
    RStringF fieldStr = iHttpSession.StringPool().StringF( aHdrField, RHTTPSession::GetTable() );
    CleanupClosePushL( fieldStr );
    
    aHeaders.SetFieldL(fieldStr, val );
    
    CleanupStack::PopAndDestroy(&fieldStr);
    CleanupStack::PopAndDestroy(&valStr);
    }

/*
void CDemoFilter::GetHeaderL(const RHTTPSession& aSession, const RHTTPHeaders& aHeaders, TInt aHdrField, THTTPHdrVal &aHeaderValue ) const
{
    RStringF fieldStr = aSession.StringPool().StringF( aHdrField, RHTTPSession::GetTable() );
    CleanupClosePushL( fieldStr );
	
	RStringPool stringPool = aSession.StringPool();	
	RStringF field = stringPool.OpenFStringL( fieldStr.DesC() );
	aHeaders.GetField(field, 0, aHeaderValue);	
	field.Close();
	
    CleanupStack::PopAndDestroy(&fieldStr);
}


void CDemoFilter::GetHeaderL(const RHTTPSession& aSession, const RHTTPHeaders& aHeaders, const TDesC8& aHdrField, THTTPHdrVal &aHeaderValue ) const
{
	RStringPool stringPool = aSession.StringPool();	
	RStringF field = stringPool.OpenFStringL( aHdrField );
	aHeaders.GetField(field, 0, aHeaderValue);	
	field.Close();
}


void CDemoFilter::RemoveHeaderL(const RHTTPSession& aSession, RHTTPHeaders aHeaders, TInt aHdrField)
    {
    RStringF fieldStr = aSession.StringPool().StringF( aHdrField, RHTTPSession::GetTable() );
    CleanupClosePushL( fieldStr );
	
    RStringF field = aSession.StringPool().OpenFStringL(fieldStr.DesC());	
    TInt err = aHeaders.RemoveField(field);
	field.Close();
	
    CleanupStack::PopAndDestroy(&fieldStr);	
    }
*/

_LIT8( KUserAgent, "MyClient 1.0" );
_LIT8( KAccept, "*/*" );

RHTTPTransaction CHttpSession::OpenTransactionL(const TDesC8& aUri, MHTTPTransactionCallback& callBack, TInt index)
	{
	__LOG_TXT("Open Transaction");
    TUriParser8 uri;
    uri.Parse( aUri );
    RStringF method = iHttpSession.StringPool().StringF( index, RHTTPSession::GetTable());
	RHTTPTransaction trans = iHttpSession.OpenTransactionL(uri, callBack, method);
    CleanupClosePushL( trans );
    RHTTPHeaders hdr = trans.Request().GetHeaderCollection();
    SetHeaderL( hdr, HTTP::EUserAgent, KUserAgent );
    SetHeaderL( hdr, HTTP::EAccept, KAccept );
    // EContentType
    CleanupStack::Pop(); // trans
    return trans;
	}

void CHttpSession::DoCancel()
	{
	__LOG_TXT("DoCancel");
/*	if (iState == EConnectionStart)
		{
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, KErrNone);
		}
	__LOG_TXT("Cancel"));*/
	
	// A quanto pare non e' possibile cancellare la RConnection.Start()
	Close();
	}


void CHttpSession::RunL()
	{
	__LOG("RunL:%d", iStatus.Int());
	TInt errCode = iStatus.Int();
	if (iReqStatus != NULL && *iReqStatus == KRequestPending)
		User::RequestComplete(iReqStatus, errCode);
	
	if (errCode != KErrNone) 
		{
		// iCallBack.OpenedL( iStatus.Int() );
		return;
		}
	switch (iState)
		{
		case EConnectionStart:
			{
		    RStringPool strPool = iHttpSession.StringPool();
		    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();

		    //to use our socket server and connection
		    connInfo.SetPropertyL(
	    		strPool.StringF(HTTP::EHttpSocketServ, RHTTPSession::GetTable()),
	    		iSrvHandle );

		    connInfo.SetPropertyL (
		    	strPool.StringF(HTTP::EHttpSocketConnection, RHTTPSession::GetTable()),
		    	THTTPHdrVal(REINTERPRET_CAST(TInt, &(iConnection))) );

		    iState = EInitialized;
			break; 
			}
		default: 
			{
			break;
			}
		}
	}

TInt CHttpSession::RunError(TInt aError)
	{
	return aError;
	}
