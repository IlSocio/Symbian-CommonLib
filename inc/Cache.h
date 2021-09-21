// LIBRARY Hash.lib

/*
 ============================================================================
 Name		: Cache.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCache declaration
 ============================================================================
 */

#ifndef CACHE_H
#define CACHE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <hash.h>

// CLASS DECLARATION

/**
 *  CCache
 * 
 */
class CCache : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CCache();

	/**
	 * Two-phased constructor.
	 */
	static CCache* NewL(const TDesC& aPath);

	/**
	 * Two-phased constructor.
	 */
	static CCache* NewLC(const TDesC& aPath);

	TBool ExistsL(RFs& aFs, const TDesC& aKey);
	HBufC8* GetL(RFs& aFs, const TDesC& aKey);
	void InsertL(RFs& aFs, const TDesC& aKey, const TDesC8& aData);
	
	void ClearL(RFs& aFs);
	
private:
	HBufC* ComputeHashL(const TDesC& aKey);
	HBufC* GetFileNameFromKeyLC(const TDesC& aKey);

	/**
	 * Constructor for performing 1st stage construction
	 */
	CCache(const TDesC& aPath);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	CMD5* iMd5;
	TFullName iCachePath;
	};

#endif // CACHE_H
