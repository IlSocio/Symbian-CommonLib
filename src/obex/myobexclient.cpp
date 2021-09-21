/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES
#include <StringLoader.h>
#include <FeatDiscovery.h>
#include <Featureinfo.h>	// KFeatureIdBt
//#include <bteng.h>

//#include <GuardGUI_200131CC.rsg>		
#include "MyObexProtocolConstants.h"
#include "btobjectexchange.pan"
#include "MyObexClient.h"
#include "MyObexServiceSearcher.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CMyObexClient::NewL()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CMyObexClient* CMyObexClient::NewL( )
    {
    CMyObexClient* self = NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CMyObexClient::NewLC()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CMyObexClient* CMyObexClient::NewLC(  )
    {
    CMyObexClient* self = new ( ELeave ) CMyObexClient( );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CMyObexClient::CMyObexClient()
// Constructor.
// ----------------------------------------------------------------------------
//
CMyObexClient::CMyObexClient(  )
: CActive( CActive::EPriorityStandard ),
  iState( EWaitingToGetDevice )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CMyObexClient::~CMyObexClient()
// Destructor.
// ----------------------------------------------------------------------------
//
CMyObexClient::~CMyObexClient()
    {
//	__FLOG(_T8("Destructor"));

    Cancel();

	if (iCurrObject)
	{
		delete iCurrObject;
		iCurrObject = NULL;
	}

	if (iServiceSearcher)
	{
		delete iServiceSearcher;
		iServiceSearcher = NULL;
	}

	if (iClient)
	{
		delete iClient;
		iClient = NULL;
	}

	if (iWait)
	{
		delete iWait;
		iWait = NULL;
	}
//	__FLOG(_T8("End Destructor"));
//	__FLOG_CLOSE;
    }

// ----------------------------------------------------------------------------
// CMyObexClient::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CMyObexClient::ConstructL()
    {
//	__FLOG_OPEN("CommonLib", "OBEX.txt");
//	__FLOG(_T8("-----------------"));
    iServiceSearcher = CMyObexServiceSearcher::NewL( );
	iWait = new (ELeave) CActiveSchedulerWait();
    }

// ----------------------------------------------------------------------------
// CMyObexClient::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CMyObexClient::DoCancel()
    {
//	__FLOG(_T8("Cancel"));
	TRAP_IGNORE( StopL() );
	iError = KErrCancel;
	if ( iWait->IsStarted() ) 
		iWait->AsyncStop();
	TRAP_IGNORE( DismissWaitDialogL() );
    }


void CMyObexClient::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
			StopL();
			iError = KErrCancel;
			if ( iWait->IsStarted() ) 
				iWait->AsyncStop();
        }
    }


// TODO: Rimpiazza WaitDialog con ProgressBar
// iProgressBar = CProgressBar::NewL(this);
// TODO: Mostra avanzamento dei bytes...
// TODO: Rimpiazza iWait con classe appropriata se possibile...


void CMyObexClient::ShowWaitDialogL(TInt aResWaitDialog)
{
	if (iWaitDialog)
		return;
    iWaitDialog = new (ELeave) CAknWaitDialog(
        reinterpret_cast<CEikDialog**>( &iWaitDialog ), ETrue );
    // Set DialogDissmissed callback
    iWaitDialog->SetCallback( this ); 
    iWaitDialog->ExecuteLD( aResWaitDialog );
//    iWaitDialog->ExecuteLD( R_WEBCLIENT_DOWNLOAD_WAIT_NOTE );
}


void CMyObexClient::DismissWaitDialogL()
{
    if ( !iWaitDialog )
		return;
	iWaitDialog->ProcessFinishedL();    // deletes the dialog
	delete iWaitDialog;
	iWaitDialog = NULL;
}


#include <btserversdkcrkeys.h> // <<< Usalo per accedere direttamente al Central Repository...
#include <centralrepository.h>
#include <BtnotifierAPI.h> 


TBool CMyObexClient::BluetoothIsOnL()
    {
    CRepository* crep = CRepository::NewLC(KCRUidBluetoothPowerState);
    TInt value = 0;
    User::LeaveIfError( crep->Get(KBTPowerState, value) );
    CleanupStack::PopAndDestroy(crep);
    return (value > 0);
    }
 

TBool CMyObexClient::BluetoothTurnOnL()
    {  
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );
    TPckgBuf<TBool> dummy(ETrue);
    TPckgBuf<TBool> reply(EFalse);
    TRequestStatus status;
    notifier.StartNotifierAndGetResponse(status, KPowerModeSettingNotifierUid, dummy, reply);
    User::WaitForRequest(status);
    notifier.CancelNotifier(KPowerModeSettingNotifierUid);
    notifier.Close();
    return reply();
    }


void CMyObexClient::ShowQueryAndSendFileL(const TDesC& filename, TInt aResWaitDialog, TBool bluetooth, TInt size)
{
#ifndef __WINSCW__
	Cancel();						// Cancel any request, just to be sure
	iError = KErrNone;

	TBool hasBT = CFeatureDiscovery::IsFeatureSupportedL(KFeatureIdBt);
	//	__FLOG_1(_T8("HasBT:%d"), hasBT);
	// Rilevamento Stato BT
	TBool wasOn = EFalse;
	if (hasBT && bluetooth)
	{
		wasOn = BluetoothIsOnL();
		if (!wasOn)
		{
			// if ( !BluetoothTurnOnL() )
			//	return;
		}
	}

/*	BINARY BREAK SU Feature Pack 2
CBTMCMSettings* btSettings = CBTMCMSettings::NewLC();
//	__FLOG(_T8("BtSettings"));
	TBool wasOn = EFalse;
	if (hasBT && bluetooth)
	{
//		__FLOG(_T8("GetPowerState"));
		btSettings->GetPowerState(wasOn);
		// Attivazione BT
//		__FLOG_1(_T8("BT Is On:%d"), wasOn);
		if (!wasOn)
			btSettings->SetPowerState(ETrue);
	}
*/

	// Show ProgressBar
	// TODO: Show Progress invece del WaitDialog...
	ShowWaitDialogL(aResWaitDialog);

	if ( DoConnectL(bluetooth) ) 
	{
		// (opt.) Se viene pigiata progress bar richiama iBTClient->Stop()
		TRAPD(err, DoSendFileL(filename, size));
	}

	// Hide ProgressBar
	DismissWaitDialogL();

	// Ripristino Stato BT
//	__FLOG(_T8("RestoreBT"));
//	if (hasBT && !wasOn && bluetooth)
//		SWitchOffBTL();
//	CleanupStack::PopAndDestroy(btSettings);			// btSettings
#endif
}


TBool CMyObexClient::DoConnectL(TBool bluetooth)
{
	if (!bluetooth)
	{
//		__FLOG(_T8("Do_IR_ConnectL"));
		do 
		{
			InfraredConnectToServerL();
			StartWait();
//			__FLOG(_T8("Try Again..."));
		} while (iError == -5501);
//		__FLOG(_T8("Connected!"));
		if (iError)
			return EFalse;
		return ETrue;
	}

//	__FLOG(_T8("Do_BT_ConnectL"));
	BluetoothConnectL();
	// Attesa della fine Connect
//	__FLOG(_T8("Wait Connect"));
	StartWait();
//	__FLOG(_T8("Connected!"));

	if (iError)
		return EFalse;

//	__FLOG(_T8("Wait Search"));
	// Attesa della fine Search Service
	StartWait();
//	__FLOG(_T8("Searched!"));
	if (iError)
		return EFalse;

	// Attesa Connect to Server
//	__FLOG(_T8("Wait For Server"));
    StartWait();
//	__FLOG(_T8("Server!"));
	if (iError)
		return EFalse;
//	__FLOG(_T8("End DoConnectL"));

	return ETrue;
}


TBool CMyObexClient::DoSendFileL(const TDesC& filename, TInt size)
{
	// Invio del file
	SendObjectL(filename, size);

	// Attesa della fine SendObjectL
    StartWait();
	if (iError)
		return EFalse;

	DisconnectL();
	// Attesa della fine DisconnectL
    StartWait();
	if (iError)
		return EFalse;

	return ETrue;
}


void CMyObexClient::StartWait( )
    {
//	__FLOG(_T8("StartWait"));
	if ( iWait->IsStarted() != (TInt)ETrue )
        {
        iWait->Start();
        }
    }



// ----------------------------------------------------------------------------
// CMyObexClient::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CMyObexClient::RunL()
    {
//	__FLOG_2(_T8("RunL:%d:%d"), iStatus.Int(), iState);
	iError = iStatus.Int();
	if ( iWait->IsStarted() ) 
		iWait->AsyncStop();

//	__FLOG(_T8("AsyncStopped"));
    if ( iStatus != KErrNone )
        {    	
        switch ( iState )
            {
			case EWaitingToGetDevice:
			//	__FLOG(_T8("ERR WaitingGettingDevice"));
				break;
            case EGettingDevice:
			//	__FLOG(_T8("Err GettingDevice"));
                if ( iStatus == KErrCancel )
                    {
                    //iLog.LogL( _L("No Device") );
                    }
                iState = EWaitingToGetDevice;
                break;
            case EGettingService:
            case EGettingConnection:
            case EDisconnecting:
                //iLog.LogL( _L("Connection Error") );
                iState = EWaitingToGetDevice;
                break;
            case EWaitingToSend:
                //iLog.LogL( _L("Send Error") );
                iState = EWaitingToGetDevice;
                break;
            default:
                Panic( EBTObjectExchangeUnexpectedLogicState );
                break;
            }
		return;
        }

    switch ( iState )
        {
        case EGettingDevice:
            // found a device now search for a suitable service
		//	__FLOG(_T8("Device Found"));
            //iLog.LogL( iServiceSearcher->ResponseParams().DeviceName() );
            iState = EGettingService;
            iStatus = KRequestPending; 
            // this means that the RunL can not be called until
            // this program does something to iStatus
		//	__FLOG(_T8("Search Service"));
            iServiceSearcher->FindServiceL( iStatus );
            SetActive();
            break;

        case EGettingService:
            //iLog.LogL( _L("Found Service") );
            iState = EGettingConnection;
            BluetoothConnectToServerL();
            break;

        case EGettingConnection:
            //iLog.LogL( _L("Connected") );
            iState = EWaitingToSend;
            break;

        case EWaitingToSend:
            //iLog.LogL( _L("Sent") );
            break;

        case EDisconnecting:
            //iLog.LogL( _L("Disconnected") );
            iState = EWaitingToGetDevice;
            break;

        default:
            Panic( EBTObjectExchangeSdpRecordDelete );
            break;
        };
    }



// ----------------------------------------------------------------------------
// CMyObexClient::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CMyObexClient::BluetoothConnectL()
    {
//	__FLOG(_T8("ConnectL"));
    if ( iState == EWaitingToGetDevice && !IsActive() )
        {
	//	__FLOG(_T8("ConnectL Ok"));
        iServiceSearcher->Close();
        iServiceSearcher->SelectDeviceByDiscoveryL( iStatus );
        iState = EGettingDevice;
        SetActive();
        }
    else
        {
		//__FLOG(_T8("ConnectL Err"));
		//iLog.LogL( _L("Client Busy") );
        User::Leave( KErrInUse );
        }
    }

// ----------------------------------------------------------------------------
// CMyObexClient::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CMyObexClient::BluetoothConnectToServerL()
    {
    if (IsActive())
    	return;
//	__FLOG(_T8("ConnectToServerL"));
    TObexBluetoothProtocolInfo protocolInfo;

    protocolInfo.iTransport.Copy( KServerTransportName );
    protocolInfo.iAddr.SetBTAddr( iServiceSearcher->BTDevAddr() );
    protocolInfo.iAddr.SetPort( iServiceSearcher->Port() );

    if ( iClient )
        {
        delete iClient;
        iClient = NULL;
        }
    iClient = CObexClient::NewL( protocolInfo );
    
    iState = EGettingConnection;
    iClient->Connect( iStatus );
    SetActive();
    }


void CMyObexClient::InfraredConnectToServerL()
    {
    if (IsActive())
    	return;
//	__FLOG(_T8("IR_ConnectToServerL"));
	TObexIrProtocolInfo info;
	_LIT8(KObexClass, "OBEX");
	_LIT8(KTinyTpLsapSel, "IrDA:TinyTP:LsapSel");

	info.iTransport = KObexIrTTPProtocol;
	info.iClassName = KObexClass;
	info.iAttributeName = KTinyTpLsapSel;

    if ( iClient )
        {
        delete iClient;
        iClient = NULL;
        }
	iClient = CObexClient::NewL (info);
    
    iState = EGettingConnection;
    iClient->Connect( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CMyObexClient::SendObjectL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CMyObexClient::SendObjectL(const TDesC& aName, TInt size)
    {
	//__FLOG(_T8("SendObjectL"));
    if ( iState != EWaitingToSend )
        {
        User::Leave( KErrDisconnected );
        }
    else if ( IsActive() ) 
        {
        User::Leave( KErrInUse );
        }

    delete iCurrObject;
    iCurrObject = NULL;
    iCurrObject = CObexFileObject::NewL(aName);
    
    TParsePtrC parse(aName);
    TPtrC ptr = parse.NameAndExt();

	if (size > 0 )
		iCurrObject->SetLengthL( size );
    iCurrObject->SetNameL( ptr );
        
    iClient->Put( *iCurrObject, iStatus );
    SetActive();
    }


TInt CMyObexClient::BytesSent()
	{
	if (!iCurrObject) 
		return 0;
	return iCurrObject->BytesSent();
	}

// ----------------------------------------------------------------------------
// CMyObexClient::StopL()
// Aborts command.
// ----------------------------------------------------------------------------
//
void CMyObexClient::StopL()
    {
//	__FLOG(_T8("StopL"));
//    if ( iState != EWaitingToGetDevice && iClient )
    if ( iClient && iClient->IsConnected() )
        {
		//__FLOG(_T8("Abort"));
        iClient->Abort();
        iState = EWaitingToGetDevice;
        }
    }

// ----------------------------------------------------------------------------
// CMyObexClient::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CMyObexClient::DisconnectL()
    {
   	Cancel();
    iServiceSearcher->Close();

	//__FLOG(_T8("DisconnectL"));
    if ( iState == EWaitingToGetDevice )
        {
        return;
        }
    if ( iState == EWaitingToSend )
        {
        //iLog.LogL( _L("Disconnecting") );
        iState = EDisconnecting;
        iClient->Disconnect( iStatus );
        SetActive();
        }
    else 
        {
        User::Leave( KErrInUse );
        }
    }

// ----------------------------------------------------------------------------
// CMyObexClient::IsBusy()
// True, if the client is performing some operation..
// ----------------------------------------------------------------------------
//
TBool CMyObexClient::IsBusy()
    {
    return IsActive();
    }

// ----------------------------------------------------------------------------
// CMyObexClient::IsConnected()
// True, if the client is performing some operation..
// ----------------------------------------------------------------------------
//
TBool CMyObexClient::IsConnected()
    {
    return iState == EWaitingToSend;
    }

// End of File
