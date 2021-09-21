
// INCLUDE FILES
#include "TimeOutTimer.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CTimeOutTimer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeOutTimer* CTimeOutTimer::NewL(MTimeOutNotifier& aTimeOutNotify, const TInt aPriority )
    {
    CTimeOutTimer* self = CTimeOutTimer::NewLC(aTimeOutNotify, aPriority );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeOutTimer* CTimeOutTimer::NewLC( MTimeOutNotifier& aTimeOutNotify, const TInt aPriority )
    {
    CTimeOutTimer* self = new ( ELeave ) CTimeOutTimer( aTimeOutNotify, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::CTimeOutTimer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CTimeOutTimer::CTimeOutTimer( MTimeOutNotifier& aTimeOutNotify, const TInt aPriority )
: CTimer( aPriority ), iNotify( aTimeOutNotify )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTimeOutTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::~CTimeOutTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
CTimeOutTimer::~CTimeOutTimer()
    {
	Cancel();
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::RunL()
// Called when operation completes.
// -----------------------------------------------------------------------------
//
void CTimeOutTimer::RunL()
    {
	if (i30MinutesCounter > 0)
		{
		CTimer::After(30*60*1000000);	// 30 minutes
		i30MinutesCounter--;
		return;
		}
	if (iInterval.Int() >= 0)
		{
		CTimer::After(iInterval);
		iInterval = -1;
		return;
		}
    // Timer request has completed, so notify the timer's owner
    // User::LeaveIfError( iStatus.Int() );	
    iNotify.TimerExpiredL( this );
    }

void CTimeOutTimer::AfterInactivity(TTimeIntervalSeconds aSeconds)
	{
	Cancel();
	CTimer::Inactivity(aSeconds);
	}

void CTimeOutTimer::AfterTime(TTimeIntervalHours aInterval)
	{
	TTimeIntervalSeconds sec = aInterval.Int() * 60 * 60;
	AfterTime(sec);
	}


void CTimeOutTimer::AfterTime(TTimeIntervalMinutes aInterval)
	{
	TTimeIntervalSeconds sec = aInterval.Int() * 60;
	AfterTime(sec);
	}

void CTimeOutTimer::AfterTime(TTimeIntervalSeconds aInterval)
	{
	Cancel();
	if (aInterval.Int() == 0)
		{
		CTimer::After(0);
		return;
		}
	i30MinutesCounter = aInterval.Int() / 1800;
	TInt rem_sec = aInterval.Int() % 1800;
	iInterval = rem_sec * 1000000;

	SetActive();
	TRequestStatus* stat=&iStatus;
	User::RequestComplete(stat, KErrNone);
	}


void CTimeOutTimer::AfterTime(TTimeIntervalMicroSeconds32 aInterval)
	{
	Cancel();
	if (aInterval.Int() == 0)
		{
		CTimer::After(0);
		return;
		}
	i30MinutesCounter = 0;
	iInterval = aInterval;

	SetActive();
	TRequestStatus* stat=&iStatus;
	User::RequestComplete(stat, KErrNone);
	}

// End of File
