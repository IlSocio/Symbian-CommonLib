

// CPubSubObserver.h

#ifndef __CPubSubObserver_h__
#define __CPubSubObserver_h__

#include <e32base.h>
#include <e32property.h>
#include <FuzzyByte\FileLogger.h>


class MPubSubObserver 
{
public:
	virtual void PropertyChangedL(TUid category, TUint key, TInt value) = 0;
};


class CPubSubObserver : public CActive
  {
public:
  static CPubSubObserver* NewL(MPubSubObserver& obs, TUid category, TUint key);
  ~CPubSubObserver();
  void StartMonitorProperty();
  void StopMonitorProperty();
  void Get(TInt& aValue);
  TBool Exists();
  
protected:
  void RunL();
  void DoCancel();
  virtual TInt RunError(TInt aError);

private:
  CPubSubObserver(MPubSubObserver& obs, TUid category, TUint key);
  void ConstructL();

private:
  MPubSubObserver& iObserver;
  TUid iCategory;
  TUint iKey;
  RProperty iProperty;	
  __LOG_DECLARATION_MEMBER
  };



class MPubSubNotifier
	{
public:
	virtual void PropertyReadyL(TAny* src) = 0;
	};


class CPubSubWaiter : public CBase, public MPubSubObserver
	{
public:
	static CPubSubWaiter* NewLC(MPubSubNotifier& notifier);
	static CPubSubWaiter* NewL(MPubSubNotifier& notifier);

	void WaitForPropertyL(TUid category, TUint key, TInt value);

	CPubSubWaiter(MPubSubNotifier& notifier);
	virtual ~CPubSubWaiter();

private:
	virtual void PropertyChangedL(TUid category, TUint key, TInt value);
	void ConstructL();
    
private:
	MPubSubNotifier& iNotifier;
	CPubSubObserver* iPubObs;
	TInt iExpectedValue;
	__LOG_DECLARATION_MEMBER
	};
#endif
