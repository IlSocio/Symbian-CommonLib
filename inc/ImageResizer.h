/*
 ============================================================================
 Name		: ImageResizer.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CImageResizer declaration
 ============================================================================
 */

// LIBRARY fbscli.lib bitmaptransforms.lib


#ifndef IMAGERESIZER_H
#define IMAGERESIZER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		
#include <FBS.H>
#include <BitmapTransforms.h>


class CFbsBitmap;
 
class MImageResizerCallBack
	{
	public:
		virtual void ResizeCompleteL(const TInt& aError) = 0;
	};


class CImageResizer : public CActive
	{
public:
	// Cancel and destroy
	~CImageResizer();

	// Two-phased constructor.
	static CImageResizer* NewL(MImageResizerCallBack& aObserver);

	// Two-phased constructor.
	static CImageResizer* NewLC(MImageResizerCallBack& aObserver);

public:
	void ResizeImage(CFbsBitmap& aBmp, const TSize& aNewSize, TBool aKeepAspectRatio=ETrue);
	CFbsBitmap* GetBitmap();
	
private:
	// C++ constructor
	CImageResizer(MImageResizerCallBack& aObserver);

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
	MImageResizerCallBack& iNotify;
	CBitmapScaler* iBitmapScaler;
	//CFbsBitmap* iBitmap;
	};

#endif // IMAGERESIZER_H
