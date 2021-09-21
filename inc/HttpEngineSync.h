/*
 ============================================================================
 Name		: HttpEngineSync.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CHttpEngineSync declaration
 ============================================================================
 */

#ifndef HTTPENGINESYNC_H
#define HTTPENGINESYNC_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <FuzzyByte\httpEngine.h>
#include <FuzzyByte\httpSession.h>
#include <FuzzyByte\AsyncWaiter.h>
#include <es_sock.h>

// CLASS DECLARATION

/**
 *  CHttpEngineSync
 * 
 */
class CHttpEngineSync : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CHttpEngineSync();

	/**
	 * Two-phased constructor.
	 */
	static CHttpEngineSync* NewL(CHttpSession* aSession);

	/**
	 * Two-phased constructor.
	 */
	static CHttpEngineSync* NewLC(CHttpSession* aSession);

	TBool IsRunning();
	void AddHeaderFieldL(TInt aHeaderField, const TDesC8& aValue);
	HBufC8* IssueHTTPPostLC(RFs& aFs, const TDesC8& aUri, const TDesC8& aData, const TDesC8& aContentType, TInt& aError, TInt aTimeOut=60, TBool aMultiPart=EFalse, const TDesC8& aHeaderField=KNullDesC8(), const TDesC8& aValue=KNullDesC8());
	HBufC8* IssueHTTPPostLC(RFs& aFs, const TDesC8& aUri, const TDesC& aFileName, const TDesC8& aContentType, TInt& aError, TInt aTimeOut=60, TBool aMultiPart=EFalse, const TDesC8& aHeaderField=KNullDesC8(), const TDesC8& aValue=KNullDesC8());
	HBufC8* IssueHTTPGetLC( const TDesC8& aUri, TInt& aError, TInt aTimeOut=60, TInt aHeaderField=NULL, const TDesC8& aValue = KNullDesC8());
	void Cancel();
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CHttpEngineSync();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(CHttpSession* aSession);

private:
	CHttpEngine* iHttp;
	};

#endif // HTTPENGINESYNC_H
