/*
 ============================================================================
 Name        : AsyncWaiter.cpp
 Author      : Marco Bellino -
 Version     : 1.0
 Copyright   :
 Description : CAsyncWaiter implementation
 ============================================================================
 */

//28018 begin

#include "AsyncWaiter.h"


CAsyncWaiter::CAsyncWaiter() : 
	CActive(EPriorityStandard) // Standard priority
	{
	CActiveScheduler::Add( this); // Add to scheduler
	} 

CAsyncWaiter* CAsyncWaiter::NewLC()
	{
	CAsyncWaiter* self = new ( ELeave ) CAsyncWaiter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CAsyncWaiter* CAsyncWaiter::NewL()
	{
	CAsyncWaiter* self = CAsyncWaiter::NewLC();
	CleanupStack::Pop(self); // self;
	return self;
	}

void CAsyncWaiter::ConstructL()
	{
	iASWait = new (ELeave) CActiveSchedulerWait();
	iTimer = CTimeOutTimer::NewL(*this);
	iError = NULL;
	}

CAsyncWaiter::~CAsyncWaiter()
	{
	Cancel(); // Cancel any request, if outstanding --> Will Call the DoCancel()
	SetErrorCode(KErrCancel); // If there aren't any outstanding requests will not call the DoCancel()
	if (iTimer)
		{
		delete iTimer;
		iTimer = NULL;
		}
	if (iASWait)
		{
		delete iASWait;
		iASWait = NULL;
		}
	iError = NULL;
	}

void CAsyncWaiter::DoCancel()
	{
	SetErrorCode(KErrCancel);
	if (iASWait->IsStarted())
		{
		iASWait->AsyncStop();
		}
	iTimer->Cancel();
	}


/**
 Indicates that the active object has issued a request
 and that it is now outstanding .
 */
void CAsyncWaiter::StartAndWait(TInt& aError)
	{
	Cancel();
	// The iError pointer is needed because when the CAsyncWaiter is deleted the iError member doesn't exists anymore
	iError = &aError;
	SetActive();
	iASWait->Start();
	}


void CAsyncWaiter::StartAndWaitWithTimeout(TInt& aError, TTimeIntervalSeconds secTimeout)
	{
	//Cancel();
	//iTimer->Cancel();
	// Non metto queste cancel perche' cosi' ottengo subito un Kern-Exec 15 o CBase46
	// Significa che ho utilizzato male CAsyncWaiter!
//	iTimer->After(TTimeIntervalMicroSeconds32(secTimeout * 1000000) );
	iTimer->AfterTime(secTimeout);
	iError = &aError;
	SetActive();
	iASWait->Start();
	}


void CAsyncWaiter::TimerExpiredL(TAny* /*src*/)
	{
	if (!iASWait->IsStarted())
		return;
	SetErrorCode(KErrTimedOut);
	iASWait->AsyncStop();
	}


/**
 Handles an active object’s request completion event.
 It sets to run the desired task.
 */
void CAsyncWaiter::RunL()
	{
	SetErrorCode( iStatus.Int() );
	if (iASWait->IsStarted())
		{
		iASWait->AsyncStop();
		}
	iTimer->Cancel();
	}


void CAsyncWaiter::SetErrorCode(TInt errCode)
	{
	if (iError == NULL)
		return;
	*iError = errCode;
	iError = NULL;
	// Pending Request has been dispatched.
	// Some asyncronous request may call the RunL() on cancellation, we force iError = NULL to avoid the override of the errorCode.
	}
