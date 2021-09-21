/*
 ============================================================================
 Name		: HttpEngine.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Guardian-Mobile
 Description : CHttpEngine declaration
 ============================================================================
 */

#ifndef HTTPENGINE_H
#define HTTPENGINE_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <CommDbConnPref.h>
#include <http.h>
#include <http\mhttpauthenticationcallback.h>
#include <http\MHTTPDataSupplier.h>
#include <es_sock.h>
#include <S32FILE.H>
#include <FuzzyByte\FileLogger.h>
#include <FuzzyByte\httpsession.h>


//_LIT8(KVERSION, "1.05.0300");


class CHttpEngine : public CBase, public MHTTPTransactionCallback, public MHTTPDataSupplier
	{
public:
	// Cancel and destroy
	~CHttpEngine();

	// Two-phased constructor.
	static CHttpEngine* NewL(CHttpSession* session);

	// Two-phased constructor.
	static CHttpEngine* NewLC(CHttpSession* session);

	// Response can be stored on file or on memory.
	void Prepare_GET_RequestL(const TDesC8& aUri, RWriteStream& aResponseStream);
	void Prepare_POST_RequestL(const TDesC8& aUri, RWriteStream& aResponseStream, const TDesC8& aContentType, const TDesC8& aData = KNullDesC8);
	void Prepare_POST_MultiPart_RequestL(const TDesC8& aUri, RWriteStream& aResponseStream, const TDesC& aDescrFileName, const TDesC8& aContentType, const TDesC8& aData = KNullDesC8);
	
	void Cancel();
	TBool IsRunning();
	void AddHeaderFieldL(const TDesC8& aField, const TDesC8& aValue);
	void AddHeaderFieldL(TInt aHeaderField, const TDesC8& aValue);
	void SubmitL(TRequestStatus& aStatus);

private:
	// From MHTTPDataSupplier:
    virtual TBool GetNextDataPart(TPtrC8& aDataPart);
	virtual void ReleaseData();
	virtual TInt OverallDataSize();
	virtual TInt Reset();
	
private:
	void PrepareRequestL(TInt aGetPostIndex, const TDesC8& aUri, RWriteStream& aResponseStream, const TDesC8& aData = KNullDesC8);
	
	// C++ constructor
	CHttpEngine(CHttpSession* session);

	// Second-phase constructor
	void ConstructL();

	TBool CompleteIfRunning(TInt errCode);

    /**
    * From MHTTPSessionEventCallback
    */
    private:
        /**
        * MHFRunL()
        * Called by framework to notify about transaction events.
        * @param aTransaction: Transaction, where the event occured.
        * @param aEvent:       Occured event.
        */
        void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

        /**
        * MHFRunError()
        * Called by framework when *leave* occurs in handling of transaction event.
        * @param aError:       The leave code that occured.
        * @param aTransaction: The transaction that was being processed when leave occured.
        * @param aEvent:       The event that was being processed when leave occured.
        * @return KErrNone,    if the error was handled. Otherwise the value of aError, or
        *                      some other error value. Returning error value causes causes
        *                      HTTP-CORE 6 panic.
        */
        TInt MHFRunError( TInt aError,
                          RHTTPTransaction aTransaction,
                          const THTTPEvent& aEvent );

private:
	enum THttpEngineState
		{
		EIdle,					
		ETransactionInProgress	//
		};

private:
	TBool			iRunning;
	RBuf8			iUri8;
	RWriteStream*	iRespStream;
	TRequestStatus*	iReqStatus;		// to notify the Completion
	RHTTPTransaction iTransaction;
	TInt iState; 					// State of the active object	
	TInt iGetPostIndex;
	RBuf8			iFileBuf;
	TInt 			iBytesLeft;
	CHttpSession* iSession;
	TBool 			iIsMultiPart;
	__LOG_DECLARATION_MEMBER
	};

#endif // HTTPENGINE_H
