/*
 ============================================================================
 Name		: SendAsSms.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : FuzzyByte
 Description : CSendAsSms implementation
 ============================================================================
 */

#include "SendAsSms.h"
#include <smut.h>

CSendAsSms::CSendAsSms(MSendAsSmsNotifier& aNotifier) :
	CActive(EPriorityStandard), iNotifier(aNotifier) // Standard priority
	{
	}

CSendAsSms* CSendAsSms::NewLC(MSendAsSmsNotifier& aNotifier)
	{
	CSendAsSms* self = new (ELeave) CSendAsSms(aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSendAsSms* CSendAsSms::NewL(MSendAsSmsNotifier& aNotifier)
	{
	CSendAsSms* self = CSendAsSms::NewLC(aNotifier);
	CleanupStack::Pop(); // self;
	return self;
	}

void CSendAsSms::ConstructL()
	{
	CActiveScheduler::Add(this); // Add to scheduler
	User::LeaveIfError(iSendAs.Connect());
	}

CSendAsSms::~CSendAsSms()
	{
	Cancel(); // Cancel any request, if outstanding
	iSendAsMessage.Close();
	iSendAs.Close();
	}


void CSendAsSms::SendSmsL(const TDesC& aRecipients, const TDesC& aMessage)
	{
	Cancel();
	iSendAsMessage.Close();
	iSendAsMessage.CreateL(iSendAs, KUidMsgTypeSMS);
 
	iSendAsMessage.AddRecipientL(aRecipients, RSendAsMessage::ESendAsRecipientTo);
	iSendAsMessage.SetBodyTextL(aMessage);
 
	iSendAsMessage.SendMessage(iStatus);
	SetActive();	
	}


void CSendAsSms::DoCancel()
	{
	iSendAsMessage.Cancel();
	}


void CSendAsSms::RunL()
	{ 
	TInt err = iStatus.Int();
	iNotifier.MessageSentL(err);
/*	if (err != KErrNone) 
		{
		iNotifier.MessageSentL(err);
		return;
		}
	TSendAsProgress progr;
	iSendAsMessage.ProgressL(progr);
	if (progr.iError != KErrNone)
		{
		iNotifier.MessageSentL(progr.iError);
		return;
		}
	if (progr.iState != CMsvSendOperation::ESendStateDone)
		{
		iNotifier.MessageSentL(KErrGeneral);
		return;		
		}
	iNotifier.MessageSentL(KErrNone);/**/
	}

TInt CSendAsSms::RunError(TInt aError)
	{
	return aError;
	}


