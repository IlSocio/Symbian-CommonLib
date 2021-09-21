/*
 ============================================================================
 Name		: SessionEngine.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Guardian-Mobile
 Description : CSessionEngine declaration
 ============================================================================
 */

#ifndef HTTPSESSION_H
#define HTTPSESSION_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>
#include <CommDbConnPref.h>
#include <ecom\ecom.h>
#include <http.h>
#include <http\mhttpauthenticationcallback.h>
#include <es_sock.h>
#include <FuzzyByte\FileLogger.h>
#include <S32FILE.H>



class CHttpSession : public CActive
	{
public:
	// Cancel and destroy
	~CHttpSession();

	// Two-phased constructor.
	static CHttpSession* NewL();

	// Two-phased constructor.
	static CHttpSession* NewLC();

public:
	TBool IsOpened();
	void Close(); 

	// CallBack
	void Open(RSocketServ& socketServ, TRequestStatus& aStatus);

	// index = HTTP::EGET
	// index = HTTP::EPOST
	RHTTPTransaction OpenTransactionL(const TDesC8& aUri, MHTTPTransactionCallback& callBack, TInt index);
	void SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue );
	void SetHeaderL( RHTTPHeaders aHeaders, const TDesC8& aHdrField, const TDesC8& aHdrValue );

private:
	// C++ constructor
	CHttpSession();

	// Second-phase constructor
	void ConstructL();
	TBool CompleteIfPending(TInt errCode);

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
	enum TSessionEngineState
		{
		EUninitialized, 	// Uninitialized
		EConnectionStart,	//
		EInitialized		// Initialized
		};

private:
	TInt iState; // State of the active object
	TRequestStatus*	iReqStatus;		// to notify the Completion
	RConnection	iConnection;
	RHTTPSession iHttpSession;		// one for each RConnection
	THTTPHdrVal iSrvHandle;
	__LOG_DECLARATION_MEMBER
	};

#endif // SESSIONENGINE_H
