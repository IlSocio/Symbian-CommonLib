/*
 ============================================================================
 Name		: HttpEngineAdv.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CHttpEngineAdv declaration
 ============================================================================
 */

#ifndef HTTPENGINEADV_H
#define HTTPENGINEADV_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <FuzzyByte\FileLogger.h>
#include <FuzzyByte\httpsession.h>
#include <FuzzyByte\httpEngine.h>

// CLASS DECLARATION

/**
 *  CHttpEngineAdv
 * 
 */
class CHttpEngineAdv : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CHttpEngineAdv();

	/**
	 * Two-phased constructor.
	 */
	static CHttpEngineAdv* NewL(CHttpSession* session);

	/**
	 * Two-phased constructor.
	 */
	static CHttpEngineAdv* NewLC(CHttpSession* session);

	
	void Prepare_GET_RequestL(const TDesC8& aUri);
	void Prepare_GET_RequestToFileL(RFs& fs, const TDesC8& aUri, const TDesC& aFileName);
	void Prepare_POST_RequestL(const TDesC8& aUri, const TDesC8& aContentType, const TDesC8& aData = KNullDesC8);
	void Prepare_POST_MultiPart_RequestL(const TDesC8& aUri, const TDesC& aDescrFileName, const TDesC8& aContentType, const TDesC8& aData = KNullDesC8);

	TPtrC GetResponse();
	
	void Cancel();
	TBool IsRunning();
	void AddHeaderFieldL(const TDesC8& aField, const TDesC8& aValue);
	void AddHeaderFieldL(TInt aHeaderField, const TDesC8& aValue);
	void SubmitL(TRequestStatus& aStatus);
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CHttpEngineAdv();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(CHttpSession* session);

private:
	CBufBase* iBuffer;
	CHttpEngine* iHttp;
	RFileWriteStream iFileStream;
	};

#endif // HTTPENGINEADV_H
