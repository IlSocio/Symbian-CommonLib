/*
 ============================================================================
 Name		: ThumbProviderCached.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CThumbProviderCached declaration
 ============================================================================
 */

#ifndef ThumbProviderCached_H
#define ThumbProviderCached_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
#include <FuzzyByte\ThumbProvider.h>
#include <FuzzyByte\BitmapsCache.h>


class MThumbProviderCachedCallBack
	{
	public:
		virtual void ThumbReadyL(const TInt& aError) = 0;
	};
 


/**
 *  CThumbProviderCached
 * 
 */
class CThumbProviderCached : public CBase, public MThumbProviderCallBack
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CThumbProviderCached();

	/**
	 * Two-phased constructor.
	 */
	static CThumbProviderCached* NewL(const TDesC& aCachePath, MThumbProviderCachedCallBack& aObserver, CHttpSession* aSession=NULL);

	/**
	 * Two-phased constructor.
	 */
	static CThumbProviderCached* NewLC(const TDesC& aCachePath, MThumbProviderCachedCallBack& aObserver, CHttpSession* aSession=NULL);

	// Return ETrue if the image is ready because it has been get from the cache...
	// Otherwise, return EFalse and the image will be nofied using ThumbReady()
	TBool ProcessImageL(const TDesC& aImgPath, TSize aNewSize);
	CFbsBitmap* GetBitmap();
	void Cancel();
	void ClearCacheL();
	
private:
	virtual void ThumbReadyL(const TInt& aError);

	/**
	 * Constructor for performing 1st stage construction
	 */
	CThumbProviderCached(MThumbProviderCachedCallBack& aObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(const TDesC& aCachePath, CHttpSession* iSession);

private:
    MThumbProviderCachedCallBack& iObserver;
    CThumbProvider* iThumbProvider;
    CBitmapsCache* iCache;
    CFbsBitmap* iBitmap;
    TFullName iLastImgPath;
	};

#endif // ThumbProviderCached_H
