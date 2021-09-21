

#ifndef __CSIMNOTIFIER_H__
#define __CSIMNOTIFIER_H__

#include <e32base.h>
#include <e32property.h>
#include <FuzzyByte\FileLogger.h>
#include <FuzzyByte\PubSubObserver.h>



class MSimNotifier
	{
public:
	virtual void SimNotifyL(TBool isPresentAndReady) = 0;
	};


class CSimNotifier : public CBase, public MPubSubObserver
	{
public:
	static CSimNotifier* NewLC(MSimNotifier& notifier);
	static CSimNotifier* NewL(MSimNotifier& notifier);

	void WaitForSimL(RFs& aFs);

	virtual ~CSimNotifier();

private:
	CSimNotifier(MSimNotifier& notifier);
	void ConstructL();
	
private:
	virtual void PropertyChangedL(TUid category, TUint key, TInt value);
    
private:
	MSimNotifier& iNotifier;
	CPubSubObserver* iSimStatusObs;
	CPubSubObserver* i3rd_SimReadyObs;
	CPubSubObserver* i3rd_SimPresentObs;
	__LOG_DECLARATION_MEMBER
	};
#endif
