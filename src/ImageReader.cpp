/*
 ============================================================================
 Name		: ImageReader.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CImageReader implementation
 ============================================================================
 */

#include "ImageReader.h"
#include <FuzzyByte\FileUtils.h>
#include <bautils.h>


CImageReader::CImageReader(MImageReaderCallBack& aObserver) :
	CActive(EPriorityStandard), iNotify(aObserver) // Standard priority
	{ 
	}

CImageReader* CImageReader::NewLC(MImageReaderCallBack& aObserver)
	{
	CImageReader* self = new (ELeave) CImageReader(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImageReader* CImageReader::NewL(MImageReaderCallBack& aObserver)
	{
	CImageReader* self = CImageReader::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CImageReader::ConstructL()
	{
	__LOG_OPEN("CommonLib", "ImageReader.txt");
	CActiveScheduler::Add(this); // Add to scheduler
	iResizer = CImageResizer::NewL(*this);
	}

CImageReader::~CImageReader()
	{
	Cancel(); // Cancel any request, if outstanding
    delete iResizer;
    delete iImageDecoder;
	delete iFrame;
	__LOG_CLOSE;
	}

void CImageReader::DoCancel()
	{	
	if (iImageDecoder)
		iImageDecoder->Cancel();
	iResizer->Cancel();
	delete iFrame;
	iFrame = NULL;
	}

void CImageReader::RunL()
	{
	delete iImageDecoder;
	iImageDecoder = NULL;
	
	if (iStatus != KErrNone)
		{
		iNotify.ImageReadyL( iStatus.Int() );
		return;
		}
	if (iNewSize.iWidth<0 || iNewSize.iWidth<0)
		{
		iNotify.ImageReadyL( KErrNone );
		return;
		}
	// Resize the image... 
	iResizer->ResizeImage(*iFrame, iNewSize, iKeepAspectRatio);
	}

void CImageReader::ResizeCompleteL(const TInt& aError)
	{
	iNotify.ImageReadyL( aError );	
	}

TInt CImageReader::RunError(TInt aError)
	{
	return aError;
	}



void CImageReader::LoadImageL(RFs& aFs, const TDesC& aFileName, TSize aNewSize, TBool aKeepAspectRatio)
	{	
	iKeepAspectRatio = aKeepAspectRatio;
	iNewSize = aNewSize;
	delete iFrame;
	iFrame = NULL;

	// Check if file exists.
	TInt size = FileUtils::GetFileSize(aFs, aFileName);
	if (size <= 0)
		{
		SetActive();
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNotFound);
		return;
		} 
	
 	TBuf8<255> imageType;
	FileUtils::GetMimeTypeL(aFs, aFileName, imageType);
	
	// Check not supported.
	if (imageType.Length() <= 0)
		{
		SetActive();
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNotSupported);
		return;
		}
	
	// Log di iData e di imageType
	__LOG_DES(aFileName);
	__LOG_DES(imageType);
	
	delete iImageDecoder;
	iImageDecoder = NULL;
//	TRAPD(err, iImageDecoder = CImageDecoder::DataNewL(aFs, iData, imageType));
	TRAPD(err, iImageDecoder = CImageDecoder::FileNewL(aFs, aFileName, imageType));
	if (err != KErrNone)
		{
		__LOG_TXT("Error");
		delete iImageDecoder; 
		iImageDecoder = NULL;
		SetActive();
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrGeneral);
		return;
		}
	__LOG_TXT("Ok");

	iFrame = new(ELeave)CFbsBitmap();
	TInt res = iFrame->Create(iImageDecoder->FrameInfo(0).iOverallSizeInPixels, iImageDecoder->FrameInfo(0).iFrameDisplayMode);
	if (res != KErrNone)
		{
		SetActive();
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, res);
		return;
		}

	iImageDecoder->Convert(&iStatus, *iFrame, 0);
	SetActive();
	}


CFbsBitmap* CImageReader::GetBitmap()
	{
	CFbsBitmap* result = iFrame;
	iFrame = NULL;
	return result;
	}

