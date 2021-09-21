/*
 ============================================================================
 Name		: ThumbManager.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CThumbManager implementation
 ============================================================================
 */

#include "ThumbManager.h"
#include "FileUtils.h"
#include <eikdef.h>
#include <eikenv.h>
#include <coemain.h>


CThumbManager::CThumbManager(MThumbManagerCallBack& aObserver) : iObserver(aObserver)
	{
	// No implementation required
	}

CThumbManager::~CThumbManager()
	{
	Cancel();
	delete iCallBack;
	delete iThumbProvider;
	__LOG_CLOSE;
	}

CThumbManager* CThumbManager::NewLC(MThumbManagerCallBack& aObserver, CHttpSession* aSession)
	{
	CThumbManager* self = new (ELeave) CThumbManager(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSession);
	return self;
	}

CThumbManager* CThumbManager::NewL(MThumbManagerCallBack& aObserver, CHttpSession* aSession)
	{
	CThumbManager* self = CThumbManager::NewLC(aObserver, aSession);
	CleanupStack::Pop(); // self;
	return self;
	}

void CThumbManager::ConstructL(CHttpSession* aSession)
	{
	__LOG_OPEN("CommonLib", "ThumbManager.txt");
	RFs& fs = CCoeEnv::Static()->FsSession();
	TFileName cacheDir = _L("Thumbs\\");
	FileUtils::CompleteWithPrivatePathL(fs, cacheDir);
	
	iThumbProvider = CThumbProviderCached::NewL(cacheDir, *this, aSession);
	iCallBack = CLongTaskAO::NewL(*this);
	}


void CThumbManager::StartProcessThumbsL(RPointerArray<HBufC>& aImgArray, TSize aSize)
	{
	__LOG("StartProcessThumbs: %d", aImgArray.Count() );
	Cancel();
	iImgArray = &aImgArray;
	// ...Useful for testing purpose only...
	// iThumbProvider->ClearCacheL();

	iElemCounter = -1;
	iSize = aSize;
	iCallBack->NextRound();
	}


void CThumbManager::DoOneRoundL()
	{
	if (!iImgArray)
		return;
	__LOG("DoOneRoundL: %d", iElemCounter );
	RFs& fs = CCoeEnv::Static()->FsSession();
	iElemCounter++;
	if (iElemCounter >= iImgArray->Count())
		{
		iImgArray = NULL;
		return;
		}
		
	TBuf<300> thumbPath = *((*iImgArray)[iElemCounter]);

	__LOG_TXT("ProcessImage:");
	__LOG_TXT("thumbPath");
	iThumbProvider->ProcessImageL(thumbPath, iSize);
	}



CFbsBitmap* CThumbManager::GetBitmap()
	{
	__LOG_TXT("GetBitmap");
	return iThumbProvider->GetBitmap();
	}


void CThumbManager::ThumbReadyL(const TInt& aError)
	{
	__LOG("ThumbReadyL: %d", iElemCounter );
	if (iElemCounter < 0)
		{
		return;
		}
	iObserver.ThumbReadyL(aError, iElemCounter);
	iCallBack->NextRound();
	}


void CThumbManager::Cancel()
	{
	__LOG_TXT("Cancel");
	iCallBack->Cancel();
	iThumbProvider->Cancel();
	}


void CThumbManager::ClearCacheL()
	{
	iThumbProvider->ClearCacheL();
	}
