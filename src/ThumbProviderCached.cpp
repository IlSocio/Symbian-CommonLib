/*
 ============================================================================
 Name		: ThumbProviderCached.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CThumbProviderCached implementation
 ============================================================================
 */

#include "ThumbProviderCached.h"
#include "FileUtils.h" 
#include "ASyncWaiter.h"

CThumbProviderCached::CThumbProviderCached(MThumbProviderCachedCallBack& aObserver) : iObserver(aObserver)
	{
	// No implementation required
	}

CThumbProviderCached::~CThumbProviderCached()
	{
	Cancel(); 
	delete iThumbProvider;
	delete iBitmap;
	iBitmap = NULL;
	delete iCache;
	}

CThumbProviderCached* CThumbProviderCached::NewLC(const TDesC& aCachePath, MThumbProviderCachedCallBack& aObserver, CHttpSession* aSession)
	{
	CThumbProviderCached* self = new (ELeave) CThumbProviderCached(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aCachePath, aSession);
	return self;
	}

CThumbProviderCached* CThumbProviderCached::NewL(const TDesC& aCachePath, MThumbProviderCachedCallBack& aObserver, CHttpSession* aSession)
	{
	CThumbProviderCached* self = CThumbProviderCached::NewLC(aCachePath, aObserver, aSession);
	CleanupStack::Pop(); // self;
	return self;
	}

void CThumbProviderCached::ConstructL(const TDesC& aCachePath, CHttpSession* aSession)
	{
	iCache = CBitmapsCache::NewL(aCachePath);
	iThumbProvider = CThumbProvider::NewL(*this, aSession);
	}

 
void CThumbProviderCached::ThumbReadyL(const TInt& aError)
	{
	if (aError == KErrNone)
		{
		iBitmap = iThumbProvider->GetBitmap();
		if (iBitmap != NULL)
			{
			iCache->PutImageL(CCoeEnv::Static()->FsSession(), iLastImgPath, *iBitmap);
			}
		}
	iObserver.ThumbReadyL(aError);
	}


CFbsBitmap* CThumbProviderCached::GetBitmap()
	{
	CFbsBitmap* res = iBitmap;
	iBitmap = NULL;
	return res;
	}


TBool CThumbProviderCached::ProcessImageL(const TDesC& aImgPath, TSize aNewSize)
	{
	Cancel();
	delete iBitmap;
	iBitmap = NULL;
	iLastImgPath = aImgPath;
	iLastImgPath.AppendNum(aNewSize.iWidth);
	iLastImgPath.Append(_L("_"));
	iLastImgPath.AppendNum(aNewSize.iHeight);
	if (iCache->GetImageL(CCoeEnv::Static()->FsSession(), iLastImgPath, iBitmap) == KErrNone)
		{ 
		iObserver.ThumbReadyL(KErrNone);
		return EFalse;
		}
	iThumbProvider->ProcessImageL(aImgPath, aNewSize);
	return ETrue;
	}


void CThumbProviderCached::Cancel()
	{
	iThumbProvider->Cancel();
	}


void CThumbProviderCached::ClearCacheL()
	{
	iCache->ClearCacheL(CCoeEnv::Static()->FsSession());
	}
