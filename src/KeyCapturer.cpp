/*
 ============================================================================
 Name		: KeyCapturer.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CKeyCapturer implementation
 ============================================================================
 */

#include "KeyCapturer.h"
#include <APGWGNAM.H>
#include <coedef.H>


CKeyCapturer::CKeyCapturer(MKeyCapturerObserver& obs) :
	CActive(EPriorityStandard), iObserver(obs) // Standard priority
	{
	}

CKeyCapturer* CKeyCapturer::NewLC(MKeyCapturerObserver& obs)
	{
	CKeyCapturer* self = new (ELeave) CKeyCapturer(obs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CKeyCapturer* CKeyCapturer::NewL(MKeyCapturerObserver& obs)
	{
	CKeyCapturer* self = CKeyCapturer::NewLC(obs);
	CleanupStack::Pop(); // self;
	return self;
	}

void CKeyCapturer::ConstructL()
	{
	CActiveScheduler::Add(this); // Add to scheduler
	User::LeaveIfError(iWsSession.Connect());
 
	iWg=RWindowGroup(iWsSession);
	User::LeaveIfError(iWg.Construct((TUint32)&iWg, EFalse));
 
	iWg.SetOrdinalPosition(1, ECoeWinPriorityAlwaysAtFront+2);
	iWg.EnableReceiptOfFocus(ETrue);
//	iWg.SetOrdinalPosition(-1);
//	iWg.EnableReceiptOfFocus(EFalse);
 
	CApaWindowGroupName* wn=CApaWindowGroupName::NewLC(iWsSession);
	wn->SetHidden(EFalse);
//	wn->SetHidden(ETrue);
	wn->SetWindowGroupName(iWg);
	CleanupStack::PopAndDestroy();
 
//	iHandle = iWg.CaptureKey(EKeyDevice0, 0,0);
	
	
	StartCapture();
	}

CKeyCapturer::~CKeyCapturer()
	{
	Cancel(); // Cancel any request, if outstanding
	// Delete instance variables if any
	iWg.Close();
	iWsSession.Close();	
	}

void CKeyCapturer::DoCancel()
	{
	iWsSession.EventReadyCancel();
	}

void CKeyCapturer::StartCapture()
	{
	Cancel(); 	
	iWsSession.EventReady(&iStatus);
	SetActive();
	}

void CKeyCapturer::RunL()
	{
	if (iStatus == KErrCancel) 
		return;
	
	if (iStatus != KErrNone) 
		{
		StartCapture();
		return;
		}
	
	TWsEvent e;
	iWsSession.GetEvent(e);
	TInt wgId = iWsSession.GetFocusWindowGroup();
	StartCapture();	// Issue a New Request

	switch (e.Type())
		{
		case EEventKey:	
			{
			if(iObserver.KeyReceivedL(e))
				{
				iWsSession.SendEventToWindowGroup(wgId, e);
				}
			break;
			}
		case EEventKeyUp:
		case EEventKeyDown:
			break;
		};	
	}

TInt CKeyCapturer::RunError(TInt aError)
	{
	return aError;
	}
