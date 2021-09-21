// LIBRARY ezip.lib ws32.lib charconv.lib euser.lib efsrv.lib bafl.lib apgrfx.lib msgs.lib 


#ifndef PROCESSES_H
#define PROCESSES_H

#include <FuzzyByte\TimeOutTimer.h>
#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		
#include <e32cmn.h>
#include <f32file.h>
#include <W32STD.H>

class Processes
	{
public:
	static TInt Launch(const TDesC& aImage, TInt uid3, TBool rendez = ETrue);
	static TBool KillL(const TDesC& procName);
	static TBool RenameIfNotRunning(const TDesC& name);
	static TBool IsRunning(const TDesC& exeName);
	};


class MProcessNotifier
	{
public:
	virtual void ProcessRunningL(TAny* src) = 0;
	};


class CProcessWaiter : public CBase, public MTimeOutNotifier
	{
public:
	static CProcessWaiter* NewLC(MProcessNotifier& notifier);
	static CProcessWaiter* NewL(MProcessNotifier& notifier);

	void WaitForProcess(const TDesC& exeName, const TTimeIntervalSeconds interval);

	CProcessWaiter(MProcessNotifier& notifier);
	virtual ~CProcessWaiter();

private:
	void ConstructL();
    virtual void TimerExpiredL(TAny* src);
    
private:
    MProcessNotifier& iNotifier;
    RBuf iExeName;
    CTimeOutTimer* iTimer;
    TTimeIntervalSeconds iInterval;
	};
#endif
