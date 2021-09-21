
#include "guiutils.h"
#include <CHARCONV.H>
#include <bautils.h>
#include <COEMAIN.H>
#include <eiktxlbx.h> 
#include <aknpopup.h> 
#include <aknlists.h>  
#include <BADESCA.H>
#include <akntitle.h> 
#include <eikdef.h>
#include <aknnavi.h>
#include <stringloader.h>
#include <akncontext.h> 
#include <aknnavilabel.h> 
#include <aknnavide.h>
#include <akntabgrp.h>
#include <eikapp.h>
#include <eikbtgpc.h>
#include <eikcba.h>     // link against avkon.lib, eikcoctl.lib



#define PEN_ENABLED_ORDINAL_WINS 3184
#define PEN_ENABLED_ORDINAL_ARM 4251


/*
In some phones (e.g., the Nokia E70, Nokia E90, Nokia N93, Nokia N95), the softkeys will be on the right or left side of the screen when in landscape mode.
If an application needs to adjust its UI (softkey positions) based on orientation, the CBA location can be retrieved with
    TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    enum TAknCbaLocation
        {
        EAknCbaLocationBottom, // landscape and portrait
        EAknCbaLocationRight,  // only landscape
        EAknCbaLocationLeft    // only landscape
        };
When the CBA is located at the bottom, the positive ("OK/Options") softkey is always on the left-hand side, and the negative ("Cancel/Back") on the right-hand side.
When the CBA is on either side of the screen, most devices have the positive softkey at bottom. 
As an exception, the Nokia E90 Communicator (when open) has the positive softkey on top.
There is no API for directly resolving the order of softkeys. 
However, the position of the softkey controls can be checked as follows:
*/
/*
TBool GuiUtils::IsOptionsButtonOnTop()
  {
  CEikButtonGroupContainer* cba = CEikonEnv::Static()->AppUiFactory()->Cba();
  if( !cba )
    return EFalse;
 
  CCoeControl* options = cba->ControlOrNull( EAknSoftkeyOptions );
  CCoeControl* exit = cba->ControlOrNull( EAknSoftkeyExit );
 
  if( options && exit )
    {
    if( options->Position().iY < exit->Position().iY )
      return ETrue;
    }
 
  return EFalse;
  }



void GuiUtils::SetCbaImage( CEikButtonGroupContainer* aBtGrC, CEikImage& aImage, TBool aLSK )
{
   // aBtGrC = CEikButtonGroupContainer::Current();
   TInt cmdPos = aLSK ? 0 : 2;
   TInt cmdId = aBtGrC->ButtonGroup()->CommandId( cmdPos );
   CEikCbaButton* button = (CEikCbaButton*)aBtGrC->ControlOrNull( cmdId );
   if( !button )
	   return;
   button->SetImage(aImage);
   aBtGrC->SetRect( aBtGrC->Rect() );
   aBtGrC->DrawNow();
}
*/

TBool GuiUtils::IsTouchSupportedL()
{
	TBool isPenEnabled = EFalse;

	RLibrary avkonDll;
	if ( avkonDll.Load( _L( "avkon.dll" ) ) != KErrNone)
		return EFalse;

	#ifdef __WINSCW__
	TLibraryFunction PenEnabled = avkonDll.Lookup( 3184 );
	#else
	TLibraryFunction PenEnabled = avkonDll.Lookup( 4251 );
	#endif

	if ( PenEnabled != NULL )
		isPenEnabled = PenEnabled();

	avkonDll.Close();
	return isPenEnabled;
}


TInt GuiUtils::GetFilteredIndex(CEikListBox* listBox, TInt indexOrig)
{
	//TInt Ret(-1);
	if(!listBox || indexOrig < 0)
		return -1;
	CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*, listBox->Model());
	if (!model)
		return -1;
	if (!model->Filter())
		return indexOrig;
	return model->Filter()->VisibleItemIndex(indexOrig);
}


TInt GuiUtils::GetUnFilteredIndex(CEikListBox* listBox, TInt indexFilt)
{
//	TInt Ret(-1);
	if(!listBox || indexFilt < 0)
		return -1;
	CAknFilteredTextListBoxModel* model = STATIC_CAST(CAknFilteredTextListBoxModel*, listBox->Model());
	if (!model)
		return -1;
	if (!model->Filter())
		return indexFilt;
	return model->Filter()->FilteredItemIndex(indexFilt);
}


CAknSearchField* GuiUtils::CreateSearchBoxL(const CCoeControl& aParent, CEikListBox& aListBox, CAknSearchField::TSearchFieldStyle style)
    {
	CAknFilteredTextListBoxModel* model = static_cast<CAknFilteredTextListBoxModel*>(aListBox.Model());
	if (!model)
		return NULL;
    // Creates FindBox.
	CAknSearchField* findbox = CAknSearchField::NewL(aParent, style, NULL, 20);
	CleanupStack::PushL(findbox);
	if (style == CAknSearchField::EPopup)
		findbox->MakeVisible( EFalse );
	model->CreateFilterL(&aListBox, findbox);
//	CAknListBoxFilterItems* filter = model->Filter();
	CleanupStack::Pop( findbox ); // findbox
	return findbox;
    }


void GuiUtils::ListBoxAddItemL( CEikTextListBox* aListBox, const TDesC& aString )
	{
	CTextListBoxModel* model = aListBox->Model();
	CDesCArray* itemArray = static_cast< CDesCArray* > ( model->ItemTextArray() );
	itemArray->AppendL( aString );
	aListBox->HandleItemAdditionL();
	}


void GuiUtils::StatusPaneDefaultContextPictureL(CEikStatusPane* sp)
	{
	TUid contextPaneUid = TUid::Uid( EEikStatusPaneUidContext );
	CEikStatusPaneBase::TPaneCapabilities subPaneContext = sp->PaneCapabilities( contextPaneUid );
	if ( !subPaneContext.IsPresent() || !subPaneContext.IsAppOwned() )
		return;
	
	CAknContextPane* context = static_cast< CAknContextPane* > ( sp->ControlL( contextPaneUid ) );
	context->SetPictureToDefaultL();
	}


void GuiUtils::StatusPaneUpdateNaviLabelL(CEikStatusPane* sp, CAknNavigationDecorator* aNaviDecor, const TDesC& aText)
	{
	TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = sp->PaneCapabilities( naviPaneUid );
	if ( !subPaneNavi.IsPresent() || !subPaneNavi.IsAppOwned() )
		return;

	CAknNaviLabel* naviLabel = static_cast<CAknNaviLabel*>( aNaviDecor->DecoratedControl() );
	if (!naviLabel)
		return;
	naviLabel->SetTextL( aText );
	
	CAknNavigationControlContainer* naviPane = 	static_cast< CAknNavigationControlContainer* >( sp->ControlL( naviPaneUid ) );
	naviPane->DrawNow();
	aNaviDecor->DrawNow();
	}


void GuiUtils::StatusPaneUpdateNaviLabelL(CEikStatusPane* sp, CAknNavigationDecorator* aNaviDecor, TInt aTxtResource)
	{
	HBufC* labelText = StringLoader::LoadLC( aTxtResource );
	StatusPaneUpdateNaviLabelL(sp, aNaviDecor, labelText->Des());
	CleanupStack::PopAndDestroy( labelText );	
	}


CAknNavigationDecorator* GuiUtils::StatusPaneSetupNaviLabelL(CEikStatusPane* sp, const TDesC& aText)
	{
	TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = sp->PaneCapabilities( naviPaneUid );
	if ( !subPaneNavi.IsPresent() || !subPaneNavi.IsAppOwned() )
		return NULL;

	CAknNavigationControlContainer* naviPane = 	static_cast< CAknNavigationControlContainer* >( sp->ControlL( naviPaneUid ) );		
	CAknNavigationDecorator* naviDecorator = naviPane->CreateNavigationLabelL( aText );
			
	naviPane->PushL( *naviDecorator );
	return naviDecorator;
	}


CAknNavigationDecorator* GuiUtils::StatusPaneSetupNaviLabelL(CEikStatusPane* sp, TInt aTxtResource)
	{
	HBufC* labelText = StringLoader::LoadLC( aTxtResource );
	CAknNavigationDecorator* naviDecorator = StatusPaneSetupNaviLabelL(sp, labelText->Des());
	CleanupStack::PopAndDestroy( labelText );			
	return naviDecorator;
	}


void GuiUtils::StatusPaneCleanupL(CEikStatusPane* sp, CAknNavigationDecorator* naviDecor)
	{
	// reset the navi pane 
	TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = sp->PaneCapabilities( naviPaneUid );
	if ( !subPaneNavi.IsPresent() || !subPaneNavi.IsAppOwned() )
		return;
	CAknNavigationControlContainer* naviPane = static_cast< CAknNavigationControlContainer* >( sp->ControlL( naviPaneUid ) );
	naviPane->Pop( naviDecor );
	}


void GuiUtils::StatusPaneSetupTitleL(CEikStatusPane* sp, TInt aTitlePaneResource)
	{
	// TODO:
	//return;  
	// setup the title pane
	TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = sp->PaneCapabilities( titlePaneUid );
	if ( !subPaneTitle.IsPresent() || !subPaneTitle.IsAppOwned() )
		return;
	CAknTitlePane* title = static_cast< CAknTitlePane* >( sp->ControlL( titlePaneUid ) );

	/*HBufC* titleText = StringLoader::LoadLC( aTitlePaneResource );
	title->SetText(titleText);
	CleanupStack::PopAndDestroy(titleText);/**/
	
	TResourceReader reader;
	CCoeEnv::Static()->CreateResourceReaderLC( reader, aTitlePaneResource );
	title->SetFromResourceL( reader );
	CleanupStack::PopAndDestroy();/**/
	}


void GuiUtils::StatusPaneReplaceNaviTabIconL(CEikStatusPane* sp, CAknNavigationDecorator* aNaviDecor, TInt tabId, const TDesC& mifFilename, TInt svgId )
	{
	TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi = sp->PaneCapabilities( naviPaneUid );
	if ( !subPaneNavi.IsPresent() || !subPaneNavi.IsAppOwned() )
		return;

	CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( aNaviDecor->DecoratedControl() );
	if (!tabGroup)
		return;
	
//	_LIT(KMifFile,"\\resource\\apps\\GuardSvg_200131CC.mif");
/*    TParsePtrC parse((CEikonEnv::Static()->EikAppUi()->Application())->AppFullName());
	TFileName filePath;
	filePath.Append(parse.Drive());
	filePath.Append(mifFilename);/**/

	// AknsUtils: Skinned Icons
	CFbsBitmap* bmp(NULL);
	CFbsBitmap* mask(NULL);
	AknIconUtils::CreateIconL(bmp, mask, mifFilename, svgId, svgId);
	AknIconUtils::SetSize(bmp, TSize(5, 5));
	AknIconUtils::SetSize(mask, TSize(5, 5));
	tabGroup->ReplaceTabL(tabId, bmp, mask);
	}

	
void GuiUtils::DeleteMenuItem(CEikMenuPane* menuPane, TInt aCommand)
{
	TInt pos=0;
	TBool exists = menuPane->MenuItemExists(aCommand, pos);
	if (!exists)
		return;
	menuPane->DeleteMenuItem(aCommand);
}


void GuiUtils::DeleteMenuItemIfEmpty(CEikMenuPane* menuPane, TInt aCommand)
{
	TInt pos=0;
	TBool exists = menuPane->MenuItemExists(aCommand, pos);
	if (!exists)
		return;
	CEikMenuPaneItem::SData& data = menuPane->ItemData(aCommand);
	if (data.iText.Length() <= 0)
		menuPane->DeleteMenuItem(aCommand);
}


CGulIcon* GuiUtils::GetApplicationIconL(const TUid& aAppUID)
{
	CFbsBitmap*	AppIcon(NULL);
	CFbsBitmap*	AppIconMsk(NULL);

	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
// EAknsAppIconTypeList // EAknsAppIconTypeContext
	AknsUtils::CreateAppIconLC(skin, aAppUID,  EAknsAppIconTypeList, AppIcon, AppIconMsk);
//	__FLOG(_T8("AknIconUtils"));
//	AknIconUtils::CreateAppIconLC(skin, aAppUID,  EAknsAppIconTypeList, AppIcon, AppIconMsk);

	CGulIcon* ris = CGulIcon::NewL(AppIcon, AppIconMsk);

	CleanupStack::Pop(2);
	return ris;
}


TInt GuiUtils::ShowItemPopupL(MDesCArray* aItemsArray, const TDesC& aTitle)
	{
    // Create CEikTextListBox instance, list
    CEikTextListBox* alist = new( ELeave ) CAknSinglePopupMenuStyleListBox;

    // Push list'pointer to CleanupStack.
    CleanupStack::PushL( alist );

    // Create CAknPopupList instance, popupList
    CAknPopupList* popupList = CAknPopupList::NewL( alist, R_AVKON_SOFTKEYS_SELECT_CANCEL, AknPopupLayouts::EMenuWindow );
    // Push popupList'pointer to CleanupStack.
    CleanupStack::PushL( popupList );

    // Initialize listbox.
    alist->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    alist->CreateScrollBarFrameL( ETrue );
    alist->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    // Set listitems.
    CTextListBoxModel* model = alist->Model();
    model->SetOwnershipType( ELbmOwnsItemArray );
    model->SetItemTextArray( aItemsArray );

    // Show popup list and then show return value.
    TInt res = -1;
    if (aTitle.Length() > 0)
    	popupList->SetTitleL(aTitle);
    
	CleanupStack::Pop(popupList);			// popupList
    TInt popupOk = popupList->ExecuteLD();
	if (popupOk)
		res = alist->CurrentItemIndex();
	CleanupStack::PopAndDestroy(alist);		// aList
	return res;
	}


TInt GuiUtils::ShowItemPopupL(TInt resource)
	{
    // Create CDesCArrayFlat instance, items
    CDesCArrayFlat* items = CCoeEnv::Static()->ReadDesCArrayResourceL( resource );
    CleanupStack::PushL( items );
    TInt res = ShowItemPopupL( items );
    CleanupStack::Pop( items );
    return res;
	}


/**
 *  This routine loads and scales a bitmap or icon.
 *
 *  @param aFileName the MBM or MIF filename
 *  @param aBitmapId the bitmap id
 *  @param aMaskId the mask id or -1 for none
 *  @param aSize the TSize for the icon, or NULL to use real size
 *  @param aScaleMode one of the EAspectRatio* enums when scaling
 *
 */
CGulIcon* GuiUtils::LoadAndScaleIconL( 
    const TDesC& aFileName,
    TInt aBitmapId, 
    TInt aMaskId,
    TScaleMode aScaleMode,
    TSize* aSize) 
  {
  CFbsBitmap* bitmap; 
  CFbsBitmap* mask = NULL;
  if (aMaskId > 0)
	  AknIconUtils::CreateIconL( bitmap, mask, aFileName, aBitmapId, aMaskId );
  else
	  bitmap = AknIconUtils::CreateIconL( aFileName, aBitmapId );
  
  TSize newSize;
  if ( aSize == NULL)
    {
    // Use size from the image header.  In case of SVG,
    // we preserve the image data for a while longer, since ordinarily
    // it is disposed at the first GetContentDimensions() or SetSize() call.
    AknIconUtils::PreserveIconData( bitmap );
    AknIconUtils::GetContentDimensions( bitmap, newSize );
    }
  else
    {
	newSize = *aSize;
    }
  
  AknIconUtils::SetSize( bitmap, newSize, aScaleMode );
  AknIconUtils::SetSize( mask, newSize, aScaleMode );
  
  if ( aSize == NULL)
    {
    AknIconUtils::DestroyIconData( bitmap );
    }
  
  return CGulIcon::NewL( bitmap, mask );
  }




void GuiUtils::ReplaceImageL(CEikListBox* aListBox, CFbsBitmap* aBitmap, TInt aIndex)
	{
	// ***
	CTextListBoxModel* model = (CTextListBoxModel*) aListBox->Model();
	MDesCArray* textArray = model->ItemTextArray();		
	CDesCArray* itemList = static_cast<CDesCArray*> (textArray);
	if (aIndex >= itemList->MdcaCount())
		return;
	
	// Aggiunge l'icona all'array di icone
	CGulIcon* icon = NULL;
	icon = CGulIcon::NewL(aBitmap, NULL);	
		
	// ***
	CFormattedCellListBoxItemDrawer* itemDrawer = (CFormattedCellListBoxItemDrawer *) aListBox->View()->ItemDrawer();
	CArrayPtr<CGulIcon>* iconArray = itemDrawer->ColumnData()->IconArray(); 
	iconArray->AppendL(icon);
	TInt iconIndex = iconArray->Count()-1;
	
	// Rimpiazza l'icona esistente con la nuova appena prelevata.
	TPtrC ptr = itemList->MdcaPoint(aIndex);	
	TBuf<200> newName = ptr;
	TInt posTab = newName.Find(_L("\t"));
	newName.Delete(0, posTab);
	
	TBuf<5> newIcon;
	newIcon.AppendNum(iconIndex);
	newName.Insert(0, newIcon);

	itemList->Delete(aIndex);
	itemList->InsertL(aIndex, newName);
	
	aListBox->View()->DrawItem(aIndex);	
	}


void GuiUtils::DrawOverBitmapL(CFbsBitmap* destBmp, TPoint destPoint, const CFbsBitmap* srcBmp, const CFbsBitmap* srcBmpMask)
	{
    CFbsBitmapDevice* bitDev = CFbsBitmapDevice::NewL(destBmp);
    CleanupStack::PushL(bitDev);
    CBitmapContext* bgc = NULL;
    User::LeaveIfError(bitDev->CreateBitmapContext(bgc));
	TRect destRect(destPoint, srcBmp->SizeInPixels());
	TRect srcRect(TPoint(0, 0), srcBmp->SizeInPixels());
	if (srcBmpMask != NULL)
		bgc->DrawBitmapMasked(destRect, srcBmp, srcRect, srcBmpMask, ETrue );
	else 
		bgc->DrawBitmap(destRect, srcBmp);
    delete bgc;
    CleanupStack::PopAndDestroy(bitDev);
	}


void GuiUtils::DrawOverBitmapL(CFbsBitmap* destBmp, TPoint destPoint, const CGulIcon* icon)
	{
	DrawOverBitmapL(destBmp, destPoint, icon->Bitmap(), icon->Mask());
	}
