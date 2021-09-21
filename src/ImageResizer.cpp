/*
 ============================================================================
 Name		: ImageResizer.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CImageResizer implementation
 ============================================================================
 */

#include "ImageResizer.h"

CImageResizer::CImageResizer(MImageResizerCallBack& aObserver) :
	CActive(EPriorityStandard), iNotify(aObserver) // Standard priority
	{
	}

CImageResizer* CImageResizer::NewLC(MImageResizerCallBack& aObserver)
	{
	CImageResizer* self = new (ELeave) CImageResizer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImageResizer* CImageResizer::NewL(MImageResizerCallBack& aObserver)
	{
	CImageResizer* self = CImageResizer::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CImageResizer::ConstructL()
	{
	CActiveScheduler::Add(this); // Add to scheduler
	iBitmapScaler = CBitmapScaler::NewL();
	}

CImageResizer::~CImageResizer()
	{
	Cancel(); // Cancel any request, if outstanding
	delete iBitmapScaler;
	}

void CImageResizer::DoCancel()
	{
	iBitmapScaler->Cancel();
	}

void CImageResizer::RunL()
	{
	TInt result = iStatus.Int();
	iNotify.ResizeCompleteL( result );
	}

TInt CImageResizer::RunError(TInt aError)
	{
	return aError;
	}
   

void CImageResizer::ResizeImage(CFbsBitmap& aBmp, const TSize& aNewSize, TBool aKeepAspectRatio)
	{
//	iBitmap = &aBmp;
	iBitmapScaler->Scale(&iStatus, aBmp, aNewSize, aKeepAspectRatio);
	SetActive();
	}

