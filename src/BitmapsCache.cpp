/*
 ============================================================================
 Name		: BitmapsCache.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CBitmapsCache implementation
 ============================================================================
 */

// TODO: Cambia per utilizzare Cache...

#include "BitmapsCache.h"
#include <bautils.h>
#include <f32file.h>


CBitmapsCache::CBitmapsCache(const TDesC& aPath) :
	iCachePath(aPath)
	{
	// No implementation required
	}

CBitmapsCache::~CBitmapsCache()
	{
	delete iMd5;
	}

CBitmapsCache* CBitmapsCache::NewLC(const TDesC& aPath)
	{
	CBitmapsCache* self = new (ELeave) CBitmapsCache(aPath);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBitmapsCache* CBitmapsCache::NewL(const TDesC& aPath)
	{
	CBitmapsCache* self = CBitmapsCache::NewLC(aPath);
	CleanupStack::Pop(); // self;
	return self;
	}

void CBitmapsCache::ConstructL()
	{
	iMd5 = CMD5::NewL();	
	}


HBufC* CBitmapsCache::FileNameToLocalLC(const TDesC& aKey)
	{
	TBuf8<400> fname;
	fname.Copy(aKey);		
	HBufC* hash = ComputeHashLC(fname);
	TFullName newFile = iCachePath;
	newFile.Append(*hash);
	CleanupStack::PopAndDestroy(hash);
	return newFile.AllocLC();
	}


TInt CBitmapsCache::PutImageL(RFs& aFs, const TDesC& aKey, CFbsBitmap& bmp)
	{
	HBufC* localFile = FileNameToLocalLC(aKey);
	aFs.MkDirAll(*localFile);
	TInt res = bmp.Save(*localFile);
	CleanupStack::PopAndDestroy(localFile);
	return res;
	}


TInt CBitmapsCache::GetImageL(RFs& aFs, const TDesC& aKey, CFbsBitmap*& bmp)
	{
	HBufC* localFile = FileNameToLocalLC(aKey);
	if (!BaflUtils::FileExists(aFs, *localFile))
		{
		bmp = NULL;
		CleanupStack::PopAndDestroy(localFile);
		return KErrNotFound;
		}
	bmp = new (ELeave) CFbsBitmap;
	TInt res = bmp->Load(*localFile);
	CleanupStack::PopAndDestroy(localFile);
	return res;
	}


TInt CBitmapsCache::RemoveImageL(RFs& aFs, const TDesC& aKey)
	{	
	HBufC* localFile = FileNameToLocalLC(aKey);
	TInt res = aFs.Delete(*localFile);
	CleanupStack::PopAndDestroy(localFile);
	return res;
	}


void CBitmapsCache::ClearCacheL(RFs& aFs)
	{
	CFileMan* fileMan = CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	TFullName dirName = iCachePath;
	dirName.Append(_L("*"));			
	fileMan->RmDir(dirName);
	fileMan->RmDir(iCachePath);
	CleanupStack::PopAndDestroy(fileMan);
	}


HBufC* CBitmapsCache::ComputeHashLC(const TDesC8& aData)
	{
	iMd5->Reset();
	TPtrC8 hashedSig(iMd5->Hash(aData));
	HBufC* buf = HBufC::NewLC(hashedSig.Length() * 2);
	TPtr bufPtr = buf->Des();
	for (TInt i = 0; i < hashedSig.Length(); i++)
		{
		bufPtr.AppendFormat(_L("%+02x"), hashedSig[i]);
		}
	return buf;
	}

