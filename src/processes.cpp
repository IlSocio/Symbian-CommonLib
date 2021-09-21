#include "processes.h"

TBool Processes::RenameIfNotRunning(const TDesC& newName)
	{
	RMutex mutex;
	TInt result = mutex.CreateGlobal(newName);
	// This check already grants the mutual exclusion from the critical section
	// However, we call the mutex.Wait() just in case...
	if (result != KErrNone)
		{
		return EFalse;
		}
	mutex.Wait();

	// Critical Section
	TBool renamed = EFalse; 
	if (!Processes::IsRunning(newName))
		{
		User::RenameProcess(newName);
		renamed = ETrue;
		}
	// Critical Section

	mutex.Signal();
	mutex.Close();
	return renamed;
	}


TBool Processes::IsRunning(const TDesC& exeName)
	{
	TFileName procName;
	procName.Copy(exeName);
	procName.Trim();
	procName.Append(_L("*"));

	TFindProcess processFinder(procName);
	TFullName fullName;
	if (processFinder.Next(fullName) == KErrNone)
		{
		return ETrue;
		}
	return EFalse;
	}


TInt Processes::Launch(const TDesC& aImage, TInt uid3, TBool rendez) 
	{
//	_LIT(KIconImage, "IconHider_20024264");
	const TUid KUid3={uid3};
	
	const TUidType procUid(KNullUid, KNullUid, KUid3);
	RProcess process;			
	TInt r = process.Create(aImage, KNullDesC, procUid);
	if (r != KErrNone)
		return r;
	if (!rendez) 
		{
		process.Resume();
		process.Close();
		return KErrNone;
		}
	TRequestStatus stat;
	process.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		process.Kill(0);		// abort startup
		}
	else
		{
		//process.SetParameter(1, 0);	// Avvio da Autostart.
		process.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	if (process.ExitType()==EExitPanic)
		r = KErrGeneral;
	else 
		r = stat.Int();
	process.Close();
	return r;
	}


TBool Processes::KillL(const TDesC& procName) 
{
	TFindProcess finder(procName);
	TFullName fullName;
	TBool killed;

	killed = EFalse;

	while (finder.Next(fullName) == KErrNone)
	{
		RProcess process;
		User::LeaveIfError( process.Open(finder, EOwnerThread) );
		CleanupClosePushL(process);

		//TUint id = (TUint) process.Id();
		TExitType type = process.ExitType();
		if (type == EExitPending ) 
		{
			//TInt reason = process.ExitReason();
			killed = ETrue;
			TRequestStatus status;
			process.Logon(status);
			process.Kill(KErrCancel);
			User::WaitForRequest(status);
		}
		CleanupStack::PopAndDestroy(&process);
	}
	return killed;
}


CProcessWaiter* CProcessWaiter::NewL(MProcessNotifier& notifier)
	{
	CProcessWaiter* proc = CProcessWaiter::NewLC(notifier);
	CleanupStack::Pop(proc);
	return proc;
	}

CProcessWaiter* CProcessWaiter::NewLC(MProcessNotifier& notifier)
	{
	CProcessWaiter* proc = new(ELeave) CProcessWaiter(notifier);
	CleanupStack::PushL(proc);
	proc->ConstructL();
	return proc;
	}

CProcessWaiter::CProcessWaiter(MProcessNotifier& notifier) : CBase(), iNotifier(notifier)
	{	
	}

CProcessWaiter::~CProcessWaiter()
	{
	delete iTimer;
	iExeName.Close();
	}

void CProcessWaiter::ConstructL()
	{
	iTimer = CTimeOutTimer::NewL(*this);
	}


void CProcessWaiter::WaitForProcess(const TDesC& exeName, const TTimeIntervalSeconds interval)
	{
	iTimer->Cancel();
	iExeName.Close();
	iExeName.Create( exeName ); 
	iInterval = interval;

	iTimer->AfterTime( TTimeIntervalSeconds(0) );
	}

void CProcessWaiter::TimerExpiredL(TAny* src)
	{
	TBool running = Processes::IsRunning( iExeName );
	if (running)
		iNotifier.ProcessRunningL( this );
	else
		iTimer->AfterTime( iInterval );
	}

