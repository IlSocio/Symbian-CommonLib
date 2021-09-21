/*
 ============================================================================
 Name		: KeyCapturer.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CKeyCapturer declaration
 ============================================================================
 */

#ifndef KEYCAPTURER_H
#define KEYCAPTURER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <w32std.h>

class MKeyCapturerObserver
{
public:
	// Must return ETrue if this key must be forwarded to the other applications.
	virtual TBool KeyReceivedL(TWsEvent aEvent) = 0;
};
 

class CKeyCapturer : public CActive
	{
public:
	// Cancel and destroy
	~CKeyCapturer();

	// Two-phased constructor.
	static CKeyCapturer* NewL(MKeyCapturerObserver& obs);

	// Two-phased constructor.
	static CKeyCapturer* NewLC(MKeyCapturerObserver& obs);

public:
	void StartCapture();

private:
	// C++ constructor
	CKeyCapturer(MKeyCapturerObserver& obs);

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
	MKeyCapturerObserver& 	iObserver;
	RWsSession     	iWsSession;
	RWindowGroup    iWg; 
	};

#endif // KEYCAPTURER_H
