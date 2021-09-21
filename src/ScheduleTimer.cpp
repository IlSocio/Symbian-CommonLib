/*
============================================================================
 Name        : ScheduleTimer.cpp
 Author      : Andrea MAGNI
 Version     : Version 1.00
 Copyright   : BITWEEN S.R.L.
 Description : CScheduleTimer implementation
============================================================================
*/

#include "ScheduleTimer.h"

// -----------------------------------------------------------------------------
// CScheduleTimer::CScheduleTimer(MScheduleTimerObserver& aObserver)
// Constructor for performing 1st stage construction
// -----------------------------------------------------------------------------
//
CScheduleTimer::CScheduleTimer(MScheduleTimerNotifier& aObserver) : 
CActive( CActive::EPriorityStandard ),
iObserver( aObserver )
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CScheduleTimer::~CScheduleTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
CScheduleTimer::~CScheduleTimer()
	{
	Cancel();
	iTimer.Close();
	}

// -----------------------------------------------------------------------------
// CScheduleTimer* CScheduleTimer::NewLC(MScheduleTimerObserver& aObserver)
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CScheduleTimer* CScheduleTimer::NewLC(MScheduleTimerNotifier& aObserver)
	{
	CScheduleTimer* self = new (ELeave) CScheduleTimer( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CScheduleTimer* CScheduleTimer::NewL(MScheduleTimerObserver& aObserver)
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CScheduleTimer* CScheduleTimer::NewL(MScheduleTimerNotifier& aObserver)
	{
	CScheduleTimer* self = CScheduleTimer::NewLC( aObserver );
	CleanupStack::Pop(); // self;
	return self;
	}

// -----------------------------------------------------------------------------
// void CScheduleTimer::ConstructL()
// EPOC default constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CScheduleTimer::ConstructL()
	{
	CActiveScheduler::Add( this );
	User::LeaveIfError( iTimer.CreateLocal() );
	}

// -----------------------------------------------------------------------------
// void CScheduleTimer::RunL()
// From CActive
// -----------------------------------------------------------------------------
//
void CScheduleTimer::RunL()
	{
	TInt errorCode = iStatus.Int();
	switch ( errorCode )
		{
		case KErrAbort:
			{
			At(iEndTime);
			return;
			}
		default:
			break;
		}
	iObserver.ScheduleTimerCompletedL( errorCode, this );
	}
// -----------------------------------------------------------------------------
// void CScheduleTimer::DoCancel()
// From CActive
// -----------------------------------------------------------------------------
//
void CScheduleTimer::DoCancel()
	{
	iTimer.Cancel();
	}


// -----------------------------------------------------------------------------
// void CScheduleTimer::AfterSince(TTime aTimeUtc, TTimeIntervalMicroSeconds aDelay)
// Invokes delay starting since the time
// -----------------------------------------------------------------------------
//
void CScheduleTimer::At(TTime aTime)
	{
	Cancel();
	// store start time
	iEndTime = aTime;
	iTimer.At( iStatus, iEndTime );
	SetActive();	
	}

