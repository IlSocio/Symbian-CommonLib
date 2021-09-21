/*
 ============================================================================
 Name        : CGPSPosition.h
 Author      : MarCo
 Version     :
 Copyright   : Just Try It
 Description : CCGPSPosition declaration
 ============================================================================
 */

#ifndef GPSPosition_H
#define GPSPosition_H

#include <FuzzyByte\FileLogger.h>
#include <FuzzyByte\TimeOutTimer.h>

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <lbs.h>

class MPositionerObserver
	{
public:
	virtual void HandleGPSModuleNotificationL(TInt error)=0;
	virtual void HandleGPSPositionL(TPosition position)=0;
	virtual void HandleGPSErrorL(TInt error)=0;
	};

class CGPSPosition : public CActive, MTimeOutNotifier
	{
public:
	CGPSPosition(MPositionerObserver& aObserver);
	~CGPSPosition();
	static CGPSPosition* NewL(MPositionerObserver& aObserver);
	static CGPSPosition* NewLC(MPositionerObserver& aObserver);

	void WaitForAvailableModuleId(TBool useAGPS = EFalse, TTimeIntervalSeconds timeOutSec = 0);
	
	void ReceiveGpsData(TTimeIntervalSeconds intervalSec, TTimeIntervalMinutes waitForFixMin = 0);

private:
	void DoReceiveGpsData(TTimeIntervalSeconds intervalSec, TTimeIntervalMinutes waitForFixMin = 0);
    virtual void TimerExpiredL(TAny* src);
    
	// From CActive
	//TBool Poll_ReceiveData(TInt intervalSec, TInt waitForFixMin = 0);
	void ConstructL();

	TPositionModuleId GetAvailableModuleId(RPositionServer& posSrv);

	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	//void RunError(TInt aError);

private:
	enum TCGPSPositionState
		{
		EUninitialized, // Uninitialized
		EInitialized, // Initalized
		EError
		// Error condition
		};

public:
	TPositionModuleId iModId;

private:
	__LOG_DECLARATION_MEMBER
	RPositionServer iPosServer;
	RPositioner iPositioner;
	TPositionInfo iPositionInfo;

	MPositionerObserver& iObserver;
	CTimeOutTimer* iTimePollModule;
	CTimeOutTimer* iTimeOutModule;
	CTimeOutTimer* iTimeReceive;
	TInt iLongIntervalSec;
	TBool iUseAGPS;
	};

#endif
