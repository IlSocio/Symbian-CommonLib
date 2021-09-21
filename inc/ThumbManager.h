/*
 ============================================================================
 Name		: ThumbManager.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CThumbManager declaration
 ============================================================================
 */

#ifndef THUMBMANAGER_H
#define THUMBMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <eiklbx.h>

#include <FuzzyByte\ThumbProviderCached.h>
#include <FuzzyByte\LongRunTask.h>
#include <FuzzyByte\HttpSession.h>
#include <FuzzyByte\FileLogger.h>
// CLASS DECLARATION


class MThumbManagerCallBack
	{
	public:
		virtual void ThumbReadyL(const TInt& aError, TInt aElemIndex) = 0;
	};
 

/**
 *  CThumbManager
 * 
 */
class CThumbManager : public CBase, public MThumbProviderCachedCallBack, public MLongTaskCallBack
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CThumbManager();

	/**
	 * Two-phased constructor.
	 */
	static CThumbManager* NewL(MThumbManagerCallBack& aObserver, CHttpSession* aSession=NULL);

	/**
	 * Two-phased constructor.
	 */
	static CThumbManager* NewLC(MThumbManagerCallBack& aObserver, CHttpSession* aSession=NULL);

	void StartProcessThumbsL(RPointerArray<HBufC>& aImgArray, TSize aSize);
	void Cancel();
	CFbsBitmap* GetBitmap();
	void ClearCacheL();
	
private:
	virtual void DoOneRoundL();
	virtual void ThumbReadyL(const TInt& aError);

	/**
	 * Constructor for performing 1st stage construction
	 */
	CThumbManager(MThumbManagerCallBack& aObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(CHttpSession* aSession);

private:
	CThumbProviderCached* iThumbProvider;
    TInt iElemCounter;
    TSize iSize;
	RPointerArray<HBufC>* iImgArray;
    CLongTaskAO* iCallBack;
    MThumbManagerCallBack& iObserver;
	__LOG_DECLARATION_MEMBER;
	};

#endif // THUMBMANAGER_H
