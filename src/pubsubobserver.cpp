// PubSubObserver.cpp

#include "PubSubObserver.h"

CPubSubObserver::CPubSubObserver(MPubSubObserver& obs, TUid category, TUint key) :
	CActive(EPriorityStandard), iObserver(obs), iCategory(category), iKey(key)
	{
	// adds CPubSubObserver to the active scheduler
	CActiveScheduler::Add(this);
	}

CPubSubObserver::~CPubSubObserver()
	{
	__LOG_CLOSE
	// cancels any request still pending
	Cancel();

	// closing the handle to the property
	iProperty.Close();
	}

CPubSubObserver* CPubSubObserver::NewL(MPubSubObserver& obs, TUid category, TUint key)
	{
	// two phase construction
	CPubSubObserver* self = new (ELeave) CPubSubObserver(obs, category, key);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPubSubObserver::Get(TInt& aValue)
	{
	TBool monitorRunning = IsActive();
	if (monitorRunning) 
		StopMonitorProperty();
	
	// ASSERT_PANIC( !IsActive(), 1002);	// Su Nokia 3250, se c'e' il monitor in corso non è possibile fare la Get()
	__LOG_TXT("Get...");
	TInt res = iProperty.Get(aValue);
	__LOG("Get Res: %d    Value: %d", res, aValue);

	if (monitorRunning)
		StartMonitorProperty();
	}

void CPubSubObserver::RunL()
	{
	TInt val = 0;
	TInt res = iProperty.Get(val);
	__LOG("RunL: %d    Value: %d", res, val);

	// Invia una nuova richiesta in modo da non perdere nessuna notifica...
	StartMonitorProperty();

	iObserver.PropertyChangedL(iCategory, iKey, val);
	}

void CPubSubObserver::DoCancel()
	{
	iProperty.Cancel();
	}

TInt CPubSubObserver::RunError(TInt aError)
	{
	__LOG("RunError: %d", aError);
	return KErrNone; 
	}

TBool CPubSubObserver::Exists()
	{
	ASSERT_PANIC( !IsActive(), 1001);	// Su Nokia 3250, se c'e' il monitor in corso non è possibile fare la Get()
	
	// TODO: Verifica se funziona
	TInt value;
	TInt err = RProperty::Get(iCategory, iKey, value);
	__LOG("Exists Err: %d", err);
	if (err == KErrNotFound)
		return EFalse;
	return ETrue;
/*	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	TInt err = RProperty::Define(iCategory, iKey, RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	if (err == KErrNone) 
		{
		RProperty::Delete(iCategory, iKey);
		}
	return (err == KErrAlreadyExists);*/
	}

void CPubSubObserver::StartMonitorProperty()
	{
	__LOG_TXT("BEGIN StartMonitorProperty");
	if (!IsActive())
		{
		// now subscribe 
		iProperty.Subscribe(iStatus);
		SetActive();
		}
	__LOG_TXT("END StartMonitorProperty");
	}

void CPubSubObserver::StopMonitorProperty()
	{
	Cancel();
	}

void CPubSubObserver::ConstructL()
	{
	__LOG_OPEN_ID("CommonLib", "PubSubObserver.txt");
	// attach to the property
	TInt res = iProperty.Attach(iCategory, iKey, EOwnerThread);
	__LOG("Attach %X:%X  Res: %d", iCategory.iUid, iKey, res);
	
	}




CPubSubWaiter::CPubSubWaiter(MPubSubNotifier& notifier) : CBase(), iNotifier(notifier)
	{
	}

CPubSubWaiter::~CPubSubWaiter()
	{
	__LOG_CLOSE
	delete iPubObs;
	}

CPubSubWaiter* CPubSubWaiter::NewL(MPubSubNotifier& notifier)
	{
	CPubSubWaiter* self = CPubSubWaiter::NewLC(notifier);
	CleanupStack::Pop(self);	
	return self;	
	}

CPubSubWaiter* CPubSubWaiter::NewLC(MPubSubNotifier& notifier)
	{
	CPubSubWaiter* self = new (ELeave) CPubSubWaiter(notifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

void CPubSubWaiter::ConstructL()
	{	
	__LOG_OPEN_ID("CommonLib", "PubSubWaiter.txt");
	} 


void CPubSubWaiter::WaitForPropertyL(TUid category, TUint key, TInt value)
	{
	__LOG("WaitForPropertyL: %x", value);
	iExpectedValue = value;
	delete iPubObs;
	iPubObs = CPubSubObserver::NewL(*this, category, key);
	
	TInt currValue = -1;
	iPubObs->Get(currValue);
	iPubObs->StartMonitorProperty();
	
	__LOG("CurrValue: %x", currValue);
	PropertyChangedL(category, key, currValue);
	}

void CPubSubWaiter::PropertyChangedL(TUid category, TUint key, TInt value)
	{	
	__LOG("PropertyChangedL: %x", value);
	if (value == iExpectedValue)
		{
		iNotifier.PropertyReadyL( this );
		delete iPubObs;
		iPubObs = NULL;
		}
	}

