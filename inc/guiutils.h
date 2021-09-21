
//LIBRARY aknskins.lib aknskinsrv.lib aknswallpaperutils.lib eikcoctl.lib avkon.lib bitgdi.lib aknicon.lib fbscli.lib egul.lib commonengine.lib bafl.lib

#ifndef GUIUTILS_H
#define GUIUTILS_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		
#include <e32cmn.h>
#include <f32file.h>
#include <eikmenup.h> 
#include <aknsfld.h>
#include <EIKLBX.H>
#include <EIKIMAGE.H>
#include <EIKTXLBX.H>
#include <AKNICONUTILS.H>

class CEikStatusPane;
class CAknNavigationDecorator;

class GuiUtils 
	{
public:
	//static void SetCbaImage( CEikButtonGroupContainer* aBtGrC, CEikImage& aImage, TBool aLSK );
	//static TBool IsOptionsButtonOnTop();
	static void DrawOverBitmapL(CFbsBitmap* destBmp, TPoint destPoint, const CGulIcon* icon);
	static void DrawOverBitmapL(CFbsBitmap* destBmp, TPoint destPoint, const CFbsBitmap *srcBmp, const CFbsBitmap* srcBmpMask=NULL);
	static TBool IsTouchSupportedL();
	static TInt GetFilteredIndex(CEikListBox* listBox, TInt indexOrig);
	static TInt GetUnFilteredIndex(CEikListBox* listBox, TInt indexFilt);
	static void ListBoxAddItemL( CEikTextListBox* aListBox, const TDesC& aString );
	static CAknSearchField* CreateSearchBoxL(const CCoeControl& aParent, CEikListBox& aListBox, CAknSearchField::TSearchFieldStyle style);

	// Transfer Ownership
	static TInt ShowItemPopupL(MDesCArray* aItemsArray, const TDesC& aTitle=KNullDesC());
	static TInt ShowItemPopupL(TInt aArrayResource);
	static void DeleteMenuItem(CEikMenuPane* menuPane, TInt aCommand);
	static void DeleteMenuItemIfEmpty(CEikMenuPane* menuPane, TInt aCommand);
	static CGulIcon* GetApplicationIconL(const TUid& aAppUID);
	static CGulIcon* LoadAndScaleIconL(const TDesC& aFileName, TInt aBitmapId, TInt aMaskId=0, TScaleMode aScaleMode=EAspectRatioPreservedAndUnusedSpaceRemoved, TSize* aSize = NULL);
	
	static void StatusPaneSetupTitleL(CEikStatusPane* sp, TInt aTitlePaneResource);
	static CAknNavigationDecorator* StatusPaneSetupNaviLabelL(CEikStatusPane* sp, const TDesC& aText);
	static CAknNavigationDecorator* StatusPaneSetupNaviLabelL(CEikStatusPane* sp, TInt aTxtResource);
	static void StatusPaneUpdateNaviLabelL(CEikStatusPane* sp, CAknNavigationDecorator* aNaviDecor, const TDesC& aText);
	static void StatusPaneUpdateNaviLabelL(CEikStatusPane* sp, CAknNavigationDecorator* aNaviDecor, TInt aTxtResource);
	static void StatusPaneDefaultContextPictureL(CEikStatusPane* sp);
	static void StatusPaneCleanupL(CEikStatusPane* sp, CAknNavigationDecorator* naviDecor);
	static void StatusPaneReplaceNaviTabIconL(CEikStatusPane* sp, CAknNavigationDecorator* naviDecor, TInt tabId, const TDesC& mifFilename, TInt svgId );
	
	// Transfers the bitmap ownership
	static void ReplaceImageL(CEikListBox* aListBox, CFbsBitmap* aBitmap, TInt aIndex);
	};

#endif
