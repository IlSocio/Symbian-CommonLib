/*
 ============================================================================
 Name		: ImageReader.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CImageReader declaration
 ============================================================================
 */

// LIBRARY bautils.lib imageconversion.lib


#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <e32base.h>
#include <coecntrl.h>
#include <w32std.h>
#include <cntdef.h>
#include <cntdb.h> 
#include <ImageConversion.h>
#include <FuzzyByte\ImageResizer.h>
#include <FuzzyByte\FileLogger.h>
 

class CFbsBitmap;


class MImageReaderCallBack
	{
	public:
		virtual void ImageReadyL(const TInt& aError) = 0;
	};
 

class CImageReader : public CActive, public MImageResizerCallBack
	{
public:
	// Cancel and destroy
	~CImageReader();

	// Two-phased constructor.
	static CImageReader* NewL(MImageReaderCallBack& aObserver);

	// Two-phased constructor.
	static CImageReader* NewLC(MImageReaderCallBack& aObserver);

public:
	void LoadImageL(RFs& aFs, const TDesC& aFileName, TSize aNewSize=TSize(-1, -1), TBool aKeepAspectRatio=ETrue);
	CFbsBitmap* GetBitmap();
    
private:
//	void DoLoadImageL(RFs& aFs, const TDesC& aFileName, TSize aNewSize=TSize(-1, -1), TBool aKeepAspectRatio=ETrue);
	// MImageResizerCallBack:
	virtual void ResizeCompleteL(const TInt& aError);
	
private:
	// C++ constructor
	CImageReader(MImageReaderCallBack& aObserver);

	// Second-phase constructor
	void ConstructL();

private:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	TInt RunError(TInt aError);

private:
	MImageReaderCallBack&	iNotify;
	CImageDecoder*			iImageDecoder;
	CFbsBitmap*				iFrame;
//	TFileName				iImageName;
	CImageResizer* 			iResizer;
	TSize					iNewSize;
	TBool					iKeepAspectRatio;
	TInt 					iInt;
	__LOG_DECLARATION_MEMBER;
	};

#endif // IMAGEREADER_H
