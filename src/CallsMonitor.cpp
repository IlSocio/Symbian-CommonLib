/*
 ============================================================================
 Name		: CallsMonitor.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCallsMonitor implementation
 ============================================================================
 */

#include "CallsMonitor.h"


CCallsMonitor* CCallsMonitor::NewLC(MCallsObserver& aObserver)
	{
	CCallsMonitor* self = new (ELeave) CCallsMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(); 
	return self;
	}

CCallsMonitor* CCallsMonitor::NewL(MCallsObserver& aObserver)
	{
	CCallsMonitor* self = CCallsMonitor::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

CCallsMonitor::CCallsMonitor(MCallsObserver& aCallBack)
:CActive(EPriorityStandard),iCallBack(aCallBack),iCurrentStatusPckg(iCurrentStatus)
	{
	CActiveScheduler::Add(this);
	}

CCallsMonitor::~CCallsMonitor()
	{
	Cancel();
	delete iTelephony;
	}

void CCallsMonitor::ConstructL(void)
	{
	iTelephony = CTelephony::NewL();
	}

void CCallsMonitor::DoCancel()
	{
	switch (iState)
		{
		case EListen:
			iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
			break;
		}
	iState = EIdle;
	}

void CCallsMonitor::RunL()
	{
	switch (iState)
		{
		case EListen:
			CTelephony::TCallStatus prevStatus = iCurrentStatus.iStatus;
			TInt err = iStatus.Int();
			if(iStatus != KErrCancel)
				StartListening();	
			iCallBack.CallStatusChangedL(prevStatus, err);
			break;
		default:
			break;
		}
	}

void CCallsMonitor::StopListening()
	{
	Cancel();
	iState = EIdle;
	}

void CCallsMonitor::StartListening()
	{
	Cancel();
	iState = EListen;
	iCurrentStatus.iStatus = CTelephony::EStatusUnknown;
	iTelephony->NotifyChange(iStatus, CTelephony::EVoiceLineStatusChange, iCurrentStatusPckg);
	SetActive();
	}



void CCallsMonitor::GetPhoneNumber(TBool isIncoming, TDes& buff)
	{	
	CTelephony::TCallInfoV1 callInfoV1;
    CTelephony::TCallInfoV1Pckg callInfoV1Pckg( callInfoV1 );
    CTelephony::TCallSelectionV1 callSelectionV1;
    CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg( callSelectionV1 );
    CTelephony::TRemotePartyInfoV1 remotePartyInfoV1;
    CTelephony::TRemotePartyInfoV1Pckg remotePartyInfoV1Pckg( remotePartyInfoV1 );
    
    callSelectionV1.iLine = CTelephony::EVoiceLine;
    callSelectionV1.iSelect = CTelephony::EInProgressCall;
    
    iTelephony->GetCallInfo( callSelectionV1Pckg, callInfoV1Pckg, remotePartyInfoV1Pckg );
    
    buff.Zero();
    if (isIncoming)
    	{
		if( remotePartyInfoV1.iRemoteIdStatus == CTelephony::ERemoteIdentityAvailable )
			{
			buff.Copy(remotePartyInfoV1.iRemoteNumber.iTelNumber);
			}
		return;
    	}
    
    // Outgoing call number can be read from iDialledParty
   	buff.Copy(callInfoV1.iDialledParty.iTelNumber);
	}

