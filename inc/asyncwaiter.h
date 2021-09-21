/*
 ============================================================================
 Name        : AsyncWaiter.h
 Author      : Marco Bellino - RQV346
 Version     : 1.0
 Copyright   : Motorola
 Description : CAsyncWaiter declaration
 ============================================================================
 */

//28018 begin

#ifndef ASYNCWAITER_H
#define ASYNCWAITER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		
#include <FuzzyByte\timeouttimer.h>


class CAsyncWaiter : public CActive, public MTimeOutNotifier
	{
public:
	// Two-phased constructor.
	static CAsyncWaiter* NewL();

	// Two-phased constructor.
	static CAsyncWaiter* NewLC();

	// Cancel and destroy
	~CAsyncWaiter();

public:
	void StartAndWait(TInt& aError);
	void StartAndWaitWithTimeout(TInt& aError, TTimeIntervalSeconds secTimeout);

protected:
	virtual void TimerExpiredL(TAny* src);

private:
	// C++ constructor
	CAsyncWaiter();

	// Second-phase constructor
	void ConstructL();

private:
	void SetErrorCode(TInt errCode);
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

private:
	CActiveSchedulerWait* iASWait;
	CTimeOutTimer* iTimer;
	TInt* iError;
	};

#endif // ASYNCWAITER_H
//28018 end
