#include "Phone.h"
#include <MTCLREG.H> 
#include <smsclnt.h>
#include <smut.h>
#include <smutset.h>
#include <smutsimparam.h>
//#include "mmlist.h"
#include <e32math.h>


CPhone::CPhone() : CActive(EPriorityStandard)
	{
	iFunc = ENoneFunc;
	}

CPhone* CPhone::NewL() 
	{
	CPhone* self = new (ELeave) CPhone();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPhone::ConstructL()
	{
	CActiveScheduler::Add(this);
	__LOG_OPEN_ID("CommonLib", "Phone.txt");

	// Nell'emulatore UIQ CTelephony non e' supportata...
#ifndef __WINSCW__
	iTelephony = CTelephony::NewL();
#endif

	iWait = new (ELeave) CActiveSchedulerWait();
	iAbort = EFalse;
	}

void CPhone::SetObserver(MPhoneObserver* Observer)
	{
	iObserver = Observer;
	}

CPhone::~CPhone()
	{
	__LOG_TXT("Destructor");
	if ( IsActive() ) 
	{
		//__LOG_TXT("IsActive"));
		Cancel();
	}

	if (iTelephony) 
	{
		//__LOG_TXT("delete Telephony"));
		delete iTelephony; 
		iTelephony = NULL;
	}

	if ( iWait ) 
	{
		delete iWait;
		iWait = NULL;
	}

/*	delete iOp; iOp=NULL;
	delete iSmsMtm; iSmsMtm=NULL;
	delete iMtmReg; iMtmReg=NULL;
	delete iMsvSession; iMsvSession=NULL;*/
	__LOG_TXT("End Destructor");
	__LOG_CLOSE;
	}


void CPhone::DoCancel()
	{
	if ( iWait->IsStarted() )	// ***
		iWait->AsyncStop();		// ***

	iAbort = ETrue;
	//__LOG("DoCancel:%d"), iFunc);
	switch (iFunc) 
		{
		case EImei:
			iTelephony->CancelAsync( CTelephony::EGetPhoneIdCancel );
		break;
		case EImsi:
			iTelephony->CancelAsync( CTelephony::EGetSubscriberIdCancel );
		break;
		case ENetStatus:
			iTelephony->CancelAsync( CTelephony::EGetNetworkRegistrationStatusCancel );
		break;
		case ECellID:
            iTelephony->CancelAsync( CTelephony::EGetCurrentNetworkInfoCancel );
		break;
		case ENotifyNetworkStatusChange:
			iTelephony->CancelAsync( CTelephony::ENetworkRegistrationStatusChangeCancel );
		break;
		case ENetName:
			iTelephony->CancelAsync( CTelephony::EGetCurrentNetworkNameCancel );
		break;
		case ECharger:
		    iTelephony->CancelAsync(CTelephony::EGetIndicatorCancel); 
		break;
		case EBatteryInfo:
		    iTelephony->CancelAsync(CTelephony::EGetBatteryInfoCancel); 
		break;
		default:
		break;
		}
	iFunc = ENoneFunc;
	}

TInt CPhone::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


void CPhone::GetImei(TDes& aImei)
	{
	if ( IsActive() )
		{
		Cancel();
		}

#ifdef __WINSCW__
    // prova a scrivere in iPhoneId tramite una nuda e cruda iPhoneId.iSerialNumer = _L("123123123123");
	// poi prova a rileggere il codice IMEI
	// aImei = iPhoneId.iSerialNumber.Left(15);
	// Per poter individuare 1) i dati che bisogna scrivere 2) le istruzioni che bisogna usare
	aImei = _L("356962014461702");
	return;
#endif

	CTelephony::TPhoneIdV1Pckg phoneIdPckg( iPhoneId );
    iTelephony->GetPhoneId( iStatus, phoneIdPckg );
    
	iFunc = EImei;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;

	iPhoneId.iSerialNumber.Trim();
	aImei = iPhoneId.iSerialNumber.Left(15);
	}


void CPhone::GetImsi(TDes& aImsi)
	{
	if ( IsActive() )
		{
		Cancel();
		}

#ifdef __WINSCW__
	aImsi = _L("123456789012345");
	return;
#endif
	CTelephony::TSubscriberIdV1Pckg subIdPckg( iSubId );
	iTelephony->GetSubscriberId( iStatus, subIdPckg );

	iFunc = EImsi;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;

	aImsi = iSubId.iSubscriberId;
	}


void CPhone::GetNetworkName(TDes& aNetworkName)
	{
	if ( IsActive() )
		{
		Cancel();
		}

#ifdef __WINSCW__
	aNetworkName = _L("FakeNet");
	return;
#endif
	 CTelephony::TNetworkNameV1Pckg netPckg( iNetwork );
	iTelephony->GetCurrentNetworkName( iStatus, netPckg );

	iFunc = ENetName;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;

	aNetworkName = iNetwork.iNetworkName;
	__LOG_TXT("NetworkName:");
	__LOG_DES(aNetworkName);
	}


void CPhone::GetNetworkStatus(CTelephony::TRegistrationStatus& regStatus)
	{
	if ( IsActive() )
		{
		Cancel();
		}

#ifdef __WINSCW__
	regStatus = CTelephony::ERegisteredOnHomeNetwork;
	return;
#endif

	CTelephony::TNetworkRegistrationV1Pckg netStatPckg(iNetStatus);
	iNetStatus.iRegStatus = CTelephony::ERegistrationUnknown;
	iTelephony->GetNetworkRegistrationStatus(iStatus, netStatPckg);

	iFunc = ENetStatus;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;

	regStatus = iNetStatus.iRegStatus;
	}


void CPhone::NotifyNetworkStatusChange(TDes8& pckgNet)
{
	if ( IsActive() )
		{
		Cancel();
		}

#ifdef __WINSCW__
	return;
#endif

	iTelephony->NotifyChange(iStatus, CTelephony::ENetworkRegistrationStatusChange, pckgNet);
	iFunc = ENotifyNetworkStatusChange;
	SetActive();
}


//	iPhone.NotifyNetworkRegistrationStatusChange(iStatus, iRegStatus);

void CPhone::GetCellID(TUint& aCellId, TUint& aLocationAreaCode, TDes& aNetworkId, TDes& aCountryCode, TDes& aOperName)
	{
	__LOG_TXT("CetCellID");
	if ( IsActive() )
		{
		__LOG_TXT("IsActive");
		Cancel();
		}

#ifdef __WINSCW__
	aCellId = 1000;
	aLocationAreaCode = 2000;
	aCountryCode = _L("11");
	aNetworkId = _L("22");
	aOperName = _L("name");
	return;
#endif
	CTelephony::TNetworkInfoV1Pckg netInfoPckg(iNetInfo);
	iTelephony->GetCurrentNetworkInfo(iStatus, netInfoPckg);

	iFunc = ECellID;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;
	
//	if (iNetInfo.iAreaKnown) 
//	{
    __LOG_TXT("CellId");
		aCellId = iNetInfo.iCellId;
	    __LOG_TXT("LAC");
		aLocationAreaCode = iNetInfo.iLocationAreaCode;
	    __LOG_TXT("CC");
		aCountryCode = iNetInfo.iCountryCode;
		aCountryCode.Trim();
	    __LOG_TXT("OPER");
		aOperName = iNetInfo.iLongName;
		aOperName.Trim();
	    __LOG_TXT("NetId");
		aNetworkId = iNetInfo.iNetworkId;
		aNetworkId.Trim();
//	}
	__LOG_TXT("NetId:");
	__LOG_DES(aNetworkId);
	__LOG_TXT("OperLongName:");
	__LOG_DES(iNetInfo.iLongName);
	__LOG_TXT("OperShortName:");
	__LOG_DES(iNetInfo.iShortName);
	}
	
CTelephony::TBatteryStatus CPhone::GetBatteryInfo()
	{
	CTelephony::TBatteryInfoV1Pckg batteryInfoV1Pckg(iBatteryInfoV1);
	
	iTelephony->GetBatteryInfo(iStatus, batteryInfoV1Pckg);
	iFunc = EBatteryInfo;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;

	return iBatteryInfoV1.iStatus;
	}

TChargerStatus CPhone::IsChargerConnected()
	{
	CTelephony::TIndicatorV1Pckg indicatorV1Pckg(iIndicatorV1);

	iTelephony->GetIndicator(iStatus, indicatorV1Pckg);
	iFunc = ECharger;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;
    
    if(iIndicatorV1.iCapabilities & CTelephony::KIndChargerConnected)
       {
       // We can detect when a charger is connected
       if(iIndicatorV1.iIndicator & CTelephony::KIndChargerConnected)
          {
    	  return EConnected;
          } // Charger is connected
       else
          {
    	  return EDisconnected;
          } // Charger is not connected
       }
    return EUnknown;
	}


/*// BETA1
// +3205858500
void CPhone::GetSCA(TDes& aSCA)
	{
	CMsvSession* aMsvSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(aMsvSession);

	CClientMtmRegistry* aMtmReg = CClientMtmRegistry::NewL(*aMsvSession);
	CleanupStack::PushL(aMtmReg);

	CSmsClientMtm* aSmsMtm = static_cast <CSmsClientMtm*> ( aMtmReg->NewMtmL(KUidMsgTypeSMS) );
	CleanupStack::PushL(aSmsMtm);
	CSmsSettings& serviceSettings = aSmsMtm->ServiceSettings();

	if ( !serviceSettings.ServiceCenterCount() ) 
	{
		aSCA = _L("");
		return;
	}
	CSmsServiceCenter& num = serviceSettings.GetServiceCenter(serviceSettings.DefaultServiceCenter());
	aSCA.Copy(num.Address());
	CleanupStack::PopAndDestroy(3);
	}*/


/*void CPhone::GetSCA(TDes& aSCA)
	{
	if ( IsActive() )
		{
		Cancel();
		}

	iMsvSession = CMsvSession::OpenSyncL(*this);
//	CleanupStack::PushL(aMsvSession);

	iMtmReg = CClientMtmRegistry::NewL(*iMsvSession);
//	CleanupStack::PushL(aMtmReg);

	iSmsMtm = static_cast <CSmsClientMtm*> ( iMtmReg->NewMtmL(KUidMsgTypeSMS) );
//	CleanupStack::PushL(aSmsMtm);
//	CSmsSettings& serviceSettings = aSmsMtm->ServiceSettings();

	//__LOG_TXT("ReadSim1"));
	iOp = iSmsMtm->ReadSimParamsL(iStatus);
	//__LOG_TXT("ReadSim2"));

	iFunc = ESca;
	SetActive();
    StartWait();
	iFunc = ENoneFunc;

	//__LOG_TXT("ReadSim3"));
	const CMobilePhoneSmspList* list = &iOp->SmspList();
	TMobileSmspEntryV1 entry1 = list->GetEntryL(0);
	aSCA.Copy( entry1.iServiceCentre.iTelNumber );

	delete iOp; iOp=NULL;
	delete iSmsMtm; iSmsMtm=NULL;
	delete iMtmReg; iMtmReg=NULL;
	delete iMsvSession; iMsvSession=NULL;

// CODICE BETA2
// MANCANO TMobileSmspEntryV1 e CMobilePhoneSmspList 
// NON SI PUO' USARE... MA MAGARI PRENDENDO I SORGENTI...
	}*/


/*
void CPhone::GetSCAddress(TDes& aNumber) {
	CMsvSession* aMsvSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(aMsvSession);

	CClientMtmRegistry* aMtmReg = CClientMtmRegistry::NewL(*aMsvSession);
	CleanupStack::PushL(aMtmReg);

	CSmsClientMtm* aSmsMtm = static_cast <CSmsClientMtm*> ( aMtmReg->NewMtmL(KUidMsgTypeSMS) );
	CleanupStack::PushL(aSmsMtm);

	CSmsSettings& serviceSettings = aSmsMtm->ServiceSettings();
	CSmsNumber& num = serviceSettings.SCAddress(serviceSettings.DefaultSC());
	iSca.Copy(num.Address());
	aNumber.Copy(num.Address());
	CleanupStack::PopAndDestroy(3);
	}
*/


void CPhone::RunL()
	{
	__LOG("RunL:%d",iFunc);
	if ( iWait->IsStarted() ) 
		iWait->AsyncStop();

	TPhoneFunctions completed = iFunc;
	// NOTA: Impostando iFunc su None e' possibile richiamare Cancel senza problemi...
	// Inoltre, bisogna impostarlo prima di chiamare la CallBack altrim.
	// Una successiva chiamata Asincrona fatta dalla CallBack non servita' a nulla
	// perche' subito dopo viene impostato iFunc a None.
	iFunc = ENoneFunc; 

	if(iStatus.Int() != KErrNone) // ERRORE
	{
		//__LOG("Func:"),completed);
		__LOG("RunLError:",iStatus.Int());
		return;
	}

	switch (completed) 
		{
		case EImei: 
			{
			break;
			}
		case EImsi:
			{
			break;
			}
		case ENotifyNetworkStatusChange:
			{
			if (iObserver)
				iObserver->HandlePhoneEventL(completed);
			// CallBack
			break;
			}
		default:
		break;
		}
	}


void CPhone::StartWait( )
    {
	iAbort = EFalse;
	if ( iWait->IsStarted() != (TInt)ETrue )
        {
        __LOG_TXT("StartWait");
        iWait->Start();
        }
    }


