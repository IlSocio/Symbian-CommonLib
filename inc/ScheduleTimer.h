/*
============================================================================
 Name        : ScheduleTimer.h
 Author      : Andrea MAGNI
 Version     :
 Copyright   : BITWEEN S.R.L.
 Description : CScheduleTimer declaration
============================================================================
*/

#ifndef SCHEDULETIMER_H
#define SCHEDULETIMER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


class MScheduleTimerNotifier
	{
public:
	/**
	 * Notifies an observer about timer completion
	 * @param aError Error code
	 * @param aSelf Timer self-reference
	 */	
	virtual void ScheduleTimerCompletedL(TInt aError, TAny* aSrc) = 0;
	};

/**
*  CScheduleTimer
* 
*/
class CScheduleTimer: public CActive
	{
public: // Constructors and destructor

	/**
	 * Destructor.
	 */
	~CScheduleTimer();

	/**
	 * Two-phased constructor.
	 */
	static CScheduleTimer* NewL(MScheduleTimerNotifier& aObserver);

	/**
	 * Two-phased constructor.
	 */
	static CScheduleTimer* NewLC(MScheduleTimerNotifier& aObserver);

public: // New methods

	/**
	 * Invokes delay starting since the time
	 * @param aTime Start time in UTC
	 */
	void At(TTime aTime);

protected: // From CActive
	void RunL();
	void DoCancel();

private: // Constructors

	/**
	 * Constructor for performing 1st stage construction
	 */
	CScheduleTimer(MScheduleTimerNotifier& aObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private: // data
	MScheduleTimerNotifier&		iObserver;
	RTimer						iTimer;
	// to handle date/time changes
	TTime						iEndTime;
	};

#endif // SCHEDULETIMER_H

// End of File
