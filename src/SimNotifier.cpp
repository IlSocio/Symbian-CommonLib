
#include "SimNotifier.h"

#include <FuzzyByte\Utils.h>

#include <PSVariables.h>
#include <SimUtils.h>
#include <StartupDomainPSKeys.h>
#include <sacls.h>   


CSimNotifier::CSimNotifier(MSimNotifier& notifier) : CBase(), iNotifier(notifier)
	{
	}

CSimNotifier::~CSimNotifier()
	{
	__LOG_CLOSE
	delete iSimStatusObs;
	delete i3rd_SimPresentObs;
	delete i3rd_SimReadyObs;
	}

CSimNotifier* CSimNotifier::NewL(MSimNotifier& notifier)
	{
	CSimNotifier* self = CSimNotifier::NewLC(notifier);
	CleanupStack::Pop(self);	
	return self;	
	}

CSimNotifier* CSimNotifier::NewLC(MSimNotifier& notifier)
	{
	CSimNotifier* self = new (ELeave) CSimNotifier(notifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

void CSimNotifier::ConstructL()
	{	
	__LOG_OPEN_ID("CommonLib", "SimNotifier.txt");	
	} 
 

void CSimNotifier::WaitForSimL(RFs& aFs)
	{
	__LOG_TXT("WaitForSimL");

	delete iSimStatusObs;
	delete i3rd_SimPresentObs;
	delete i3rd_SimReadyObs;

	TUint s60Major=0;
	TUint s60Minor=0;
	Utils::GetS60PlatformL(aFs, s60Major, s60Minor);
	if (s60Major + s60Minor <= 4) // 3rd e 3rd FP1
//	if (s60Major + s60Minor <= 5) // 5th
		{
		__LOG_TXT("3rd - 3rd FP1");
		// 3rd Edition
		i3rd_SimPresentObs = CPubSubObserver::NewL(*this, KUidSystemCategory, KPSUidSimPresentValue);		
		i3rd_SimReadyObs = CPubSubObserver::NewL(*this, KUidSystemCategory, KPSSimReadyStatusValue);		

		TInt simPresValue = EPSSimPresentInitWait;		
 		__LOG_TXT("SimPresent Get()");
 		i3rd_SimPresentObs->Get(simPresValue);
		i3rd_SimPresentObs->StartMonitorProperty();
		
		TInt simReadyValue = EPSSimReadyStatusUninitialized;
 		__LOG_TXT("SimReady Get()");
 		i3rd_SimReadyObs->Get(simReadyValue);
		i3rd_SimReadyObs->StartMonitorProperty();
		
		// Richiama il PropertyChangedL per ultimo perchè la callback di notifica potrebbe provocare la cancellazione di questo oggetto.
		PropertyChangedL(KUidSystemCategory, KPSUidSimPresentValue, simPresValue);
		PropertyChangedL(KUidSystemCategory, KPSSimReadyStatusValue, simReadyValue);
		return;
		}
	
	__LOG_TXT("3rd FP2 - 5th - Anna - Belle");
	// Questa chiave non è presente su N95 8Gb... 3rd FP1
	iSimStatusObs = CPubSubObserver::NewL(*this, KPSUidStartup, KPSSimStatus);			
	TInt simValue = ESimStatusUninitialized;
	iSimStatusObs->Get(simValue);
	iSimStatusObs->StartMonitorProperty();
	
	// Richiama il PropertyChangedL per ultimo perchè la callback di notifica potrebbe provocare la cancellazione di questo oggetto.
	PropertyChangedL(KPSUidStartup, KPSSimStatus, simValue);		
	}

void CSimNotifier::PropertyChangedL(TUid category, TUint key, TInt value)
	{	
	__LOG("PropertyChangedL: %x", value);
	// Per i cell. 3rd Edition, es: E61
	if (category == KUidSystemCategory && key == KPSUidSimPresentValue)
		{
		__LOG("PS_SIM Present Change: %d", value);
		switch (value)
			{
			case EPSSimPresentInitWait:
			case EPSSimPresentTrue:				// 1
				break;
			case EPSSimPresentFalse:			// 2
				{
				// La Sim non è presente... e siamo su 3rd
				iNotifier.SimNotifyL( EFalse );

				// CheckAndLockPhoneIfNeededL();
				break;
				}
			default:
				break;
			}		
		return;
		}
	
	// Per i cell. 3rd Edition, es: E61
	if (category == KUidSystemCategory && key == KPSSimReadyStatusValue)
		{
		__LOG("PS_SIM Status Change: %d", value);
		switch (value)
			{
			case EPSSimReadyStatusUninitialized:	// 0
			case EPSSimNotReady:					// 1	(senza sim ottengo NotReady)
				break;
			case EPSSimReady:						// 2
				{
				// La Sim è presente... e siamo su 3rd
				iNotifier.SimNotifyL( ETrue );
				break;
				}
			default:
				break;
			}	
		return;
		}	

	// Per i cell. 3rd FP1, 3rd FP2, 5th, Anna, Belle, Belle FP1
	if (category == KPSUidStartup && key == KPSSimStatus)
		{
		__LOG("PS_SIM Status Change: %d", value);
		switch (value)
			{
			case ESimStatusUninitialized:	// 100
				break;
			case ESimNotReady:				// 103
				break;
			case ESimReadable:				// 102	Non è ancora pronto perchè restituisce Invalid IMSI
				break;
			case ESimNotSupported:			// 105
				break;
			case ESimNotPresent:			// 104
				iNotifier.SimNotifyL( EFalse );
				break;
			case ESimUsable:				// 101
				iNotifier.SimNotifyL( ETrue );
				break;
/*				{
				CheckAndLockPhoneIfNeededL();
				
				// Serve per verificare se il cell. si trova gia' connesso alla rete
				iPhone->GetNetworkStatus(iNetStatus.iRegStatus);
				// Attende la rete...
				HandlePhoneEventL(ENotifyNetworkStatusChange);			
				break;
				}*/
			default:
				break;
			}
		// __LOG_TXT("PS_SIM END");	// Non va bene loggare qui perchè il notifier può cancellare questa istanza (come infatti fa...)
		return;
		}
	}

