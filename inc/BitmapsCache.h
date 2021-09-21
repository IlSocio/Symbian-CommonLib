/*
 ============================================================================
 Name		: BitmapsCache.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CBitmapsCache declaration
 ============================================================================
 */

// LIBRARY Hash.lib

#ifndef BITMAPSCACHE_H
#define BITMAPSCACHE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <hash.h>
#include <f32file.h>
#include <fbs.h>

// CLASS DECLARATION


/**
 *  CBitmapsCache
 * 
 */
class CBitmapsCache : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CBitmapsCache();

	/**
	 * Two-phased constructor.
	 */
	static CBitmapsCache* NewL(const TDesC& aPath);

	/**
	 * Two-phased constructor.
	 */
	static CBitmapsCache* NewLC(const TDesC& aPath);

	TInt PutImageL(RFs& aFs, const TDesC& aKey, CFbsBitmap& bmp);
	TInt GetImageL(RFs& aFs, const TDesC& aKey, CFbsBitmap*& bmp);
	TInt RemoveImageL(RFs& aFs, const TDesC& aKey);
	void ClearCacheL(RFs& aFs);

private:
	HBufC* FileNameToLocalLC(const TDesC& aKey);
	HBufC* ComputeHashLC(const TDesC8& data);

	/**
	 * Constructor for performing 1st stage construction
	 */
	CBitmapsCache(const TDesC& aPath);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	CMD5* iMd5;
	TFullName iCachePath;
	};

#endif // BITMAPSCACHE_H
