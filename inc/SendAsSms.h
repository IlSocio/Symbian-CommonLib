/*
 ============================================================================
 Name		: SendAsSms.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : FuzzyByte
 Description : CSendAsSms declaration
 ============================================================================
 */
//LIBRARY    sendas2.lib

#ifndef SENDASSMS_H
#define SENDASSMS_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <F32FILE.H>
#include <BADESCA.H>
 
#include <rsendasmessage.h>
#include <rsendas.h>
#include <MTCLREG.H>
 
class MSendAsSmsNotifier
    {
	public:
	    virtual void MessageSentL(TInt aError) = 0;
	};

class CSendAsSms : public CActive
	{
public:
	// Cancel and destroy
	~CSendAsSms();

	// Two-phased constructor.
	static CSendAsSms* NewL(MSendAsSmsNotifier& notifier);

	// Two-phased constructor.
	static CSendAsSms* NewLC(MSendAsSmsNotifier& notifier);

	void SendSmsL(const TDesC& aRecipients, const TDesC& aMessage);
private:
	// C++ constructor
	CSendAsSms(MSendAsSmsNotifier& notifier);

	// Second-phase constructor
	void ConstructL();

private:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	TInt RunError(TInt aError);

private:
	MSendAsSmsNotifier& iNotifier;
	RSendAs 			iSendAs;
	RSendAsMessage 		iSendAsMessage;
	};

#endif // SENDASSMS_H
