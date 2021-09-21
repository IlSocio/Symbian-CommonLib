/*
 ============================================================================
 Name		: Cache.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCache implementation
 ============================================================================
 */

#include "Cache.h"
#include <BaUtils.h>
#include <FuzzyByte\FileUtils.h>



CCache::CCache(const TDesC& aPath) :
		iCachePath(aPath)
	{
	// No implementation required
	}

CCache::~CCache()
	{
	delete iMd5;
	}

CCache* CCache::NewLC(const TDesC& aPath)
	{
	CCache* self = new (ELeave) CCache(aPath);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCache* CCache::NewL(const TDesC& aPath)
	{
	CCache* self = CCache::NewLC(aPath);
	CleanupStack::Pop(); // self;
	return self;
	}

void CCache::ConstructL()
	{
	iMd5 = CMD5::NewL();	
	}


void CCache::ClearL(RFs& aFs)
	{
	CFileMan* fileMan = CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	TFullName dirName = iCachePath;
	dirName.Append(_L("*"));			
	fileMan->RmDir(dirName);
	fileMan->RmDir(iCachePath);
	CleanupStack::PopAndDestroy(fileMan);
	}


HBufC* CCache::ComputeHashL(const TDesC& aKey)
	{
	RBuf8 key8;
	key8.CleanupClosePushL();
	key8.Create(aKey.Length()*2);
	key8.Copy(aKey);

	iMd5->Reset();
	TPtrC8 hashedSig(iMd5->Hash(key8));
	CleanupStack::PopAndDestroy(&key8);
	
	HBufC* buf = HBufC::NewL(hashedSig.Length() * 2);
	TPtr bufPtr = buf->Des();
	for (TInt i = 0; i < hashedSig.Length(); i++)
		{
		bufPtr.AppendFormat(_L("%+02x"), hashedSig[i]);
		}
	return buf;
	}


HBufC* CCache::GetFileNameFromKeyLC(const TDesC& aKey)
	{
    TFileName fname = iCachePath;
    RBuf hash( ComputeHashL(aKey) );
    hash.CleanupClosePushL();
    fname.Append(hash);
	CleanupStack::PopAndDestroy(&hash);
	return fname.AllocLC();
	}


TBool CCache::ExistsL(RFs& aFs, const TDesC& aKey)
	{
    HBufC* fname = GetFileNameFromKeyLC(aKey);
    TBool exists = BaflUtils::FileExists(aFs, *fname);
	CleanupStack::PopAndDestroy(fname);
	return exists;
	}

HBufC8* CCache::GetL(RFs& aFs, const TDesC& aKey)
	{
    HBufC* fname = GetFileNameFromKeyLC(aKey);
	HBufC8* res = FileUtils::ReadFileContentsL(aFs, *fname);
	CleanupStack::PopAndDestroy(fname);
	return res;
	}

void CCache::InsertL(RFs& aFs, const TDesC& aKey, const TDesC8& aData)
	{
    HBufC* fname = GetFileNameFromKeyLC(aKey);
	FileUtils::DumpToFileL(aFs, *fname, aData, EFalse);
	CleanupStack::PopAndDestroy(fname);
	}
