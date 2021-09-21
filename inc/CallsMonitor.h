/*
 ============================================================================
 Name		: CallsMonitor.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCallsMonitor declaration
 ============================================================================
 */

#ifndef CALLSMONITOR_H
#define CALLSMONITOR_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <Etel3rdParty.h> 

class MCallsObserver
	{
	public:
		virtual void CallStatusChangedL(CTelephony::TCallStatus& aStatus, TInt aError)=0;
	};

class CCallsMonitor : public CActive
	{
	public:
		~CCallsMonitor();
		static CCallsMonitor* NewLC(MCallsObserver& aObserver);
		static CCallsMonitor* NewL(MCallsObserver& aObserver);
		void GetPhoneNumber(TBool isIncoming, TDes& buff);
		void StartListening();
		void StopListening();
	private:
		CCallsMonitor(MCallsObserver& aCallBack);
		void ConstructL();
	protected:
		void DoCancel();
		void RunL();
		
	private:
		enum TState
			{
		    EIdle,
			EListen
			};
	private:
		MCallsObserver&               iCallBack;
		CTelephony::TCallStatusV1     iCurrentStatus;
		CTelephony::TCallStatusV1Pckg iCurrentStatusPckg;
		CTelephony*                   iTelephony;		
		TState						  iState;
	};

#endif // CALLSMONITOR_H
