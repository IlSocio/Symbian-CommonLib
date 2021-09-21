/*
 ============================================================================
 Name		: ThumbProvider.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CThumbProvider declaration
 ============================================================================
 */

#ifndef THUMBPROVIDER_H
#define THUMBPROVIDER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
#include <FuzzyByte\ImageReader.h>
#include <FuzzyByte\HttpEngine.h>
#include <FuzzyByte\HttpSession.h>


class MThumbProviderCallBack
	{
	public:
		virtual void ThumbReadyL(const TInt& aError) = 0;
	};
 


/**
 *  CThumbProvider
 * 
 */
class CThumbProvider : public CBase, public MImageReaderCallBack
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CThumbProvider();

	/**
	 * Two-phased constructor.
	 */
	static CThumbProvider* NewL(MThumbProviderCallBack& aObserver, CHttpSession* aSession=NULL);

	/**
	 * Two-phased constructor.
	 */
	static CThumbProvider* NewLC(MThumbProviderCallBack& aObserver, CHttpSession* aSession=NULL);

	void ProcessImageL(const TDesC& aImgPath, TSize aNewSize);
	CFbsBitmap* GetBitmap();
	void Cancel();

private:
	TInt DownloadImageL(const TDesC& aUrl, const TDesC& aImgPath);
	void ClearTempPathL(RFs& aFs);
	void GetTempFileNameL(const TDesC& aUri, TDes& fname);
	virtual void ImageReadyL(const TInt& aError);

	/**
	 * Constructor for performing 1st stage construction
	 */
	CThumbProvider(MThumbProviderCallBack& aObserver);

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(CHttpSession* iSession);

private:
    CImageReader* iImageReader;
    TSize iSize;
    MThumbProviderCallBack& iObserver;
    TFileName iTempPath;
	CHttpEngine* iHttp;
	};

#endif // THUMBPROVIDER_H
