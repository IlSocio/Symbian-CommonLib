#include "GPSPosition.h"

// TODO: Usa HPositionGenericInfo invece di TPositionInfo

// CONSTANTS

//Seconds and Minutes
const TInt64 KSecond = 1000000;
const TInt64 KMinute = KSecond * 60;

//MaxAge
const TInt KMaxAge = 5000000;

//The name of the requestor
_LIT(KRequestor,"GPSPosition");



CGPSPosition::CGPSPosition(MPositionerObserver& aObserver) :
	CActive(EPriorityStandard), iObserver(aObserver)
{
	 CActiveScheduler::Add( this );
}

CGPSPosition* CGPSPosition::NewL(MPositionerObserver& aObserver)
		{
	CGPSPosition* self = CGPSPosition::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
}

CGPSPosition* CGPSPosition::NewLC(MPositionerObserver& aObserver)
{
	CGPSPosition* self = new (ELeave) CGPSPosition(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

void CGPSPosition::ConstructL()
{
    // Connect to the position server
	__LOG_OPEN_ID("CommonLib", "NewGPS.txt")
			
	TInt err = iPosServer.Connect();
	__LOG("Connect Result:%d", err);

	User::LeaveIfError( err );
	__LOG_TXT("Connected");
	
	iTimePollModule = CTimeOutTimer::NewL(*this);
	iTimeOutModule = CTimeOutTimer::NewL(*this);
	iTimeReceive =  CTimeOutTimer::NewL(*this);
}


CGPSPosition::~CGPSPosition()
{
	__LOG_ENTER
	Cancel(); // Cancel any request, if outstanding
	delete iTimeReceive;
	delete iTimeOutModule;
	delete iTimePollModule;
	iPositioner.Close(); 
	iPosServer.Close();
	// Delete instance variables if any
	
	__LOG_EXIT
	__LOG_CLOSE
}


TPositionModuleId CGPSPosition::GetAvailableModuleId(RPositionServer& posSrv)
{
	TUint qta=0;
	posSrv.GetNumModules(qta);
	__LOG("NumModules:%d", qta);

	// Prende il primo modulo avail che e' esterno, oppure interno...
	TPositionModuleId modId;
	modId.iUid = KNullUidValue;
	for (TUint i=0; i<qta; i++)
	{
		TPositionModuleInfo modInfo;
		TInt error = posSrv.GetModuleInfoByIndex(i, modInfo);
		if (error)
			continue;

		TBuf<50> name;
		modInfo.GetModuleName(name);
		__LOG_TXT("Current Module: ");
		__LOG_DES(name);
		if (!modInfo.IsAvailable())
		{
			__LOG_TXT("Skip NotAvailable");
			continue;									// Questa impostazione viene modificata dal menu' impostaz. del cell...
		}

		TPositionModuleStatus status;
		posSrv.GetModuleStatus(status, modInfo.ModuleId());
		if (status.DeviceStatus() < TPositionModuleStatus::EDeviceInactive)
		{
			__LOG("Skip DeviceStatus:%d", status.DeviceStatus());
			continue;	 // Unknown, Error, Disabled	// Questa impostazione viene modificata dal menu' impostaz. del cell...
		}

		TInt capab = static_cast<TInt> (modInfo.Capabilities());
		if ((capab & TPositionModuleInfo::ECapabilityHorizontal) == 0)
		{
			__LOG("Skip CapabHorizontal:%d", capab);
			continue;
		}

		__LOG("DeviceStatus:%d", status.DeviceStatus());
		__LOG("DataQuality:%d", status.DataQualityStatus());

		// Se il modulo riceve gia' la posizione lo restituisce come risultato...
		if ((status.DeviceStatus() == TPositionModuleStatus::EDeviceActive) &&
	        (status.DataQualityStatus() >= TPositionModuleStatus::EDataQualityPartial))
		{
			__LOG_TXT("Good Module!");
			return modInfo.ModuleId();
		}

		// Se non e' interno e non e' ancora stato restituito allora significa che e' disconnesso
		if (modInfo.DeviceLocation() != TPositionModuleInfo::EDeviceInternal)
		{
			__LOG_TXT("Skip NotInternal");
			continue;
		}

		if ((modInfo.TechnologyType() & TPositionModuleInfo::ETechnologyAssisted) != 0)
		{
			// return modInfo.ModuleId();
			// GPS Assistito...
			// COSA SUCCEDE SE SI USA e c'e' una connessione internet errata?
			// Usando il GPS Assistito viene presentato il box all'utente...
/*			if (iUseAGPS)
				{
				__FLOG(_T8("Use AssistedGPS") );
				return modInfo.ModuleId();
				}*/
			__LOG_TXT("Skip AssistedGPS" );
			continue;
		}

		/*
		TPositionQuality quality;
	       modInfo.GetPositionQuality(quality);
	       // In this example, check for horizontal accuracy better than 10 metres
	       if (!Math::IsNaN(quality.HorizontalAccuracy()) && quality.HorizontalAccuracy() < 10 )
	         {
	         // This module has all the required characteristics!
	         modId = modInfo.ModuleId();
	         }
	         */
	       
		// GPS Integrato nel terminale...
		if ((modInfo.TechnologyType() & TPositionModuleInfo::ETechnologyTerminal) != 0)
		{
			__LOG_TXT("Marked Integrated GPS" );
			modId = modInfo.ModuleId();
		}

		// Anche quello assistito e' integrato...
	}
	return modId; // KNullUidValue oppure un valore
}


void CGPSPosition::WaitForAvailableModuleId(TBool useAGPS, TTimeIntervalSeconds timeOutSec)
	{
	__LOG_ENTER
	iUseAGPS = useAGPS;
	if (timeOutSec.Int() > 0)
		iTimeOutModule->AfterTime(timeOutSec);
	iTimePollModule->AfterTime(TTimeIntervalMicroSeconds32(1000000));
	iLongIntervalSec = 0;
	__LOG_EXIT
	}


void CGPSPosition::TimerExpiredL(TAny* src)
	{
	__LOG_TXT("TimerExpiredL");
	if (src == iTimeOutModule)
		{
		iObserver.HandleGPSModuleNotificationL(KErrTimedOut);
		return;
		}
	if (src == iTimePollModule)
		{
		if (iUseAGPS)
			{
			iObserver.HandleGPSModuleNotificationL(KErrNone);
			return;
			}
		iModId = GetAvailableModuleId(iPosServer);
		if (iModId.iUid == KNullUidValue)
			{
			iTimePollModule->AfterTime(TTimeIntervalSeconds(5));
			return;
			}
		// != NULL;
		iObserver.HandleGPSModuleNotificationL(KErrNone);
		return;
		}
	if (src == iTimeReceive)
		{
		iTimeReceive->AfterTime(TTimeIntervalSeconds(iLongIntervalSec));
		DoReceiveGpsData(60, 30);
		return;
		}
	}


void CGPSPosition::ReceiveGpsData(TTimeIntervalSeconds intervalSec, TTimeIntervalMinutes waitForFixMin)
	{
	__LOG_TXT("ReceiveGpsData");
	// Gestisce il caso in cui l'intervallo e' superiore ai 30 minuti... cosa fare ad esempio se imposto 100minuti di intervallo?
	// Faccio partire il timer, allo scadere richiamo il DoReceiveGpsData() con intervalli di 1 minuto
	// Alla prima complete con successo, chiamo iPositioner.Close() e Faccio partire di nuovo il timer.

	iLongIntervalSec = 0;
	if (intervalSec.Int() > 31*60)
		{
		iLongIntervalSec = intervalSec.Int();
		DoReceiveGpsData(60, 30);
		// iTimeReceive->AfterSeconds(iLongIntervalSec);
		return;
		}
	DoReceiveGpsData(intervalSec, waitForFixMin);
	}

	
void CGPSPosition::DoReceiveGpsData(TTimeIntervalSeconds intervalSec, TTimeIntervalMinutes waitForFixMin)
{
	__LOG_TXT("DoReceiveGpsData");
	if (!iUseAGPS)		
		ASSERT_PANIC(iModId.iUid != KNullUidValue, 666);
//	ASSERT_PANIC(waitForFixMin.Int() <= 30, 777);
//	iModId.iUid = KNullUidValue;
//	return EFalse;
/*	iModId = GetAvailableModuleId(iPosServer);
	__FLOG_1(_T8("Get Avail Module:%d"), iModId.iUid);
	if (iModId.iUid == KNullUidValue)
		return EFalse;*/
	
    // Open subsession to the position server
	Cancel();
	iPositioner.Close();
	
    TInt error = KErrNone;
    
    if (iUseAGPS)
		error = iPositioner.Open(iPosServer);
	else
		error = iPositioner.Open(iPosServer, iModId);
	if (error != KErrNone)
		{
		__LOG_TXT("Error Open");
		SetActive();
		TRequestStatus* stat=&iStatus;
		User::RequestComplete(stat, error);
		return;
		}

    // Set position requestor
	// (Allow multiple instances)
	TBuf<100> req = KRequestor();
	TLinAddr addr = (TLinAddr)(this);
	req.AppendNum(addr, EHex);
	
//	error = iPositioner.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication , req );
	error = iPositioner.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication , KRequestor() );
	if (error != KErrNone)
		{
		__LOG_TXT("Error SetRequestor");
		SetActive();
		TRequestStatus* stat=&iStatus;
		User::RequestComplete(stat, error);
		return;
		}

   // Set update interval to one second to receive one position data per second
	TPositionUpdateOptions upOpt;
	TInt64 ms = intervalSec.Int();
	ms *= (KSecond * 60);
    upOpt.SetUpdateInterval(TTimeIntervalMicroSeconds(ms));
//    upOpt.SetUpdateInterval(TTimeIntervalMicroSeconds(KUpdateInterval));

// Di default c'e' un timeout illimitato da rispettare per ottenre un Fix...
	if (waitForFixMin.Int() != 0)
		{
		ms = waitForFixMin.Int();
		ms *= KMinute;
		upOpt.SetUpdateTimeOut(TTimeIntervalMicroSeconds(ms));
		}

    // Positions which have time stamp below KMaxAge can be reused
    upOpt.SetMaxUpdateAge(TTimeIntervalMicroSeconds(KMaxAge));

    // Enables location framework to send partial position data    
    upOpt.SetAcceptPartialUpdates(iUseAGPS);
    // upOpt.SetAcceptPartialUpdates(/*ETrue*/EFalse);
 
    // KIS000850: Known Issue Set update options
    error =  iPositioner.SetUpdateOptions( upOpt );
	if (error != KErrNone)
		{
		__LOG_TXT("Error SetUpdateOptions");
		SetActive();
		TRequestStatus* stat=&iStatus;
		User::RequestComplete(stat, error);
		return;
		}

	__LOG_TXT("Initial NotifyPositionUpdate");
    iPositioner.NotifyPositionUpdate(iPositionInfo, iStatus);
	SetActive();
}


void CGPSPosition::DoCancel()
{
	__LOG_ENTER;
	iPositioner.CancelRequest(EPositionerNotifyPositionUpdate );
	iTimeReceive->Cancel();
	iTimeOutModule->Cancel();
	iTimePollModule->Cancel();
	__LOG_EXIT;
}

void CGPSPosition::RunL()
{
	__LOG("RunL:%d", iStatus.Int());
	iTimeReceive->Cancel();
	
	if (iStatus.Int() != KErrNone)
	{
		// KErrTimedOut
		// KPositionQualityLoss
		// KPositionPartialUpdate
		iPositioner.Close();
		if (iStatus.Int() == KErrServerTerminated)
			{
			TInt err = iPosServer.Connect();
			__LOG("Restart GPS Server:%d", err);
			}
		iObserver.HandleGPSErrorL(iStatus.Int());
		return;
	}
/*        // The fix has only partially valid information.
        // It is guaranteed to only have a valid timestamp
        case KPositionPartialUpdate:*/

/*				if (iPositionInfo.ModuleId() != iModId)
				{
				Rileva un cambio di PSY
				}*/

	TPosition pos;
  	iPositionInfo.GetPosition(pos);
	// Velocita', Direzione

  	// Se l'intervallo e' lungo non schedula una nuova richiesta...
	if (iLongIntervalSec > 0)
		{
		iPositioner.Close();
		iTimeReceive->AfterTime(TTimeIntervalSeconds(iLongIntervalSec));
		iObserver.HandleGPSPositionL(pos);
		return;
		}
	
	// Request next position
//	__FLOG(_T8("Next NotifyPositionUpdate"));
    iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );

   	// Set this object active
   	SetActive();

	// Richiama l'observer per ultimo, cosi' l'observer ha la facolta' di annullare la richiesta...
	iObserver.HandleGPSPositionL(pos);
}
