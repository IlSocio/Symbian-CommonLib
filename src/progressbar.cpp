/*
 * ============================================================================
 *  Name        : CProgressBar from ProgressBar.cpp
 *  Description : CProgressBar implementation.
 *  Version     : 1.0
 * ============================================================================
 */


// http://wiki.forum.nokia.com/index.php/How_to_use_the_CAknWaitDialog

// INCLUDE FILES 
#include "ProgressBar.h"                // CProgressBar

#include <aknwaitdialog.h>              // CAknWaitDialog
#include <eikprogi.h>                   // CEikProgressInfo 
#include <stringloader.h>               // StringLoader

#include <avkon.rsg>

// ================= MEMBER FUNCTIONS ========================================
//
// ---------------------------------------------------------------------------
// CProgressBar::CProgressBar()
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CProgressBar::CProgressBar( MProgressBarObserver* aObserver ) : iObserver(
        aObserver )
    {
    }

// ---------------------------------------------------------------------------
// CProgressBar::~CProgressBar()
// Destructor.
// ---------------------------------------------------------------------------
//
CProgressBar::~CProgressBar()
    {
	iObserver = NULL;
    if ( iDialog )
        {
        // Modifica del 20/Luglio/2010 Start
		delete iDialog;
        // TRAP_IGNORE( iDialog->ProcessFinishedL() );
        // Modifica del 20/Luglio/2010 End

        iDialog = NULL;
        }
	delete iTimer;
    }

// ---------------------------------------------------------------------------
// CProgressBar::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CProgressBar* CProgressBar::NewL( MProgressBarObserver* aObserver )
    {
    CProgressBar* self = CProgressBar::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CProgressBar::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CProgressBar* CProgressBar::NewLC( MProgressBarObserver* aObserver )
    {
    CProgressBar* self = new ( ELeave ) CProgressBar( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CProgressBar::ConstructL()
// Default EPOC constructor.
// ---------------------------------------------------------------------------
//
void CProgressBar::ConstructL()
    {
	iTimer = CTimeOutTimer::NewL(*this);
    }

// ---------------------------------------------------------------------------
// CProgressBar::DialogDismissedL()
// Called when wait dialog is cancelled.
// ---------------------------------------------------------------------------
// 
void CProgressBar::DialogDismissedL( TInt aButtonId )
    {
    // notify observer about cancelation
    if ( aButtonId == EAknSoftkeyCancel )
        {
        if ( iObserver ) 
            {
            iObserver->RequestCancelled();
            }
        }

    iDialog = NULL;	// Just in case
    }

// ---------------------------------------------------------------------------
// CProgressBar::FinishRequestL()
// Finishes current request state.
// ---------------------------------------------------------------------------
// 
void CProgressBar::FinishRequestL()
    {
    // dismiss wait dialog
    if (iDialog)
    	{
        // Modifica del 20/Luglio/2010 Start
		delete iDialog;
    	// iDialog->ProcessFinishedL();
        // Modifica del 20/Luglio/2010 End
    	iDialog = NULL;
    	}
    iTimer->Cancel();
    }

// ---------------------------------------------------------------------------
// CProgressBar::StartRequestL()
// Starts new request state.
// ---------------------------------------------------------------------------
// 
void CProgressBar::StartRequestL( TInt aResDialog, TTimeIntervalSeconds aTimeOutInSec, TInt aFinalProgressTick )
    {
	iTimeOut = aTimeOutInSec;
    iTimer->Cancel();
	if (iTimeOut.Int() > 0)
		iTimer->AfterTime(iTimeOut);
    /*
    // For the wait dialog
    iWaitDialog = new (ELeave) CAknWaitDialog(
        REINTERPRET_CAST(CEikDialog**, &iWaitDialog)); 
    iWaitDialog->SetCallback(this);
    iWaitDialog->SetTextL(aLabel);
    iWaitDialog->ExecuteLD(1);//R_TEST_WAITNOTE);
    */
    if(iDialog)
        {
        // Forse richiamare FinishRequest invece della delete...
        // Basta fare una prova chiamando 2 volte la StartRequestL();
        // Sembra andare bene cosi'.
        delete iDialog;
        iDialog = NULL;
        }
    
    if (aFinalProgressTick > 0)
		{
		// create progress dialog
		iDialog = new (ELeave) CAknProgressDialog(
				reinterpret_cast<CEikDialog**> (&iDialog), ETrue);
		}
	else
		{
		// create wait dialog
		iDialog = new (ELeave) CAknWaitDialog(
				reinterpret_cast<CEikDialog**> (&iDialog), ETrue);
		}
    
	iDialog->SetCallback(this);
	iDialog->PrepareLC(aResDialog);
	if (!iObserver)
		{
		iDialog->ButtonGroupContainer().SetCommandSetL(	R_AVKON_SOFTKEYS_EMPTY );
		}
	iDialog->RunLD();
	
	if (aFinalProgressTick > 0)
		iDialog->GetProgressInfoL()->SetFinalValue(aFinalProgressTick);
	
	iDialog->DrawNow();
    	        
/*    if ( aFinalProgressTick == -1 )
        {
        // create wait dialog
        iWaitDialog = new ( ELeave ) CAknWaitDialog(
                reinterpret_cast<CEikDialog**>( &iWaitDialog ), ETrue );
        iWaitDialog->SetCallback( this );
        iWaitDialog->PrepareLC( aResDialog );
//        iWaitDialog->PrepareLC( R_SKEBBYGUI_WAIT_DIALOG );
        if ( !iObserver )
            {
            iWaitDialog->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_EMPTY );
            }
        iWaitDialog->RunLD(); 
        }    
    else
        {
        // create progress dialog
        iProgressDialog = new ( ELeave ) CAknProgressDialog(
                reinterpret_cast<CEikDialog**>( &iProgressDialog ), ETrue );
//        iProgressDialog->DrawNow();
        iProgressDialog->SetCallback( this );
        iProgressDialog->PrepareLC( aResDialog );
        if ( !iObserver )
            {
            iProgressDialog->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_EMPTY );
            }
        
        iProgressDialog->RunLD();
        iProgressDialog->GetProgressInfoL()->SetFinalValue( aProgressTick );
        }*/

    // laod dialog text
/*    HBufC* text = NULL;
    switch ( aState )
        {
        case EDeleteMessages:
            {
            text = StringLoader::LoadLC( R_SKEBBYGUI_PROGRESS_DIALOG_DELETING_MESSAGES_TEXT );
            break;
            }
        case EProcessing:
            {
            text = StringLoader::LoadLC( R_PROCESSING_TEXT );
            break;
            }
        case ESending:
            {
            text = StringLoader::LoadLC( R_SKEBBYGUI_SENDING_TEXT, aProgressTick );
            break;
            }
        default:
            {
            text = KNullDesC().AllocLC();
            break;
            }
        }

    // set dialog text
    UpdateRequestL( aState, *text );
    CleanupStack::PopAndDestroy( text );*/
    }

/*TInt CProgressBar::ProgressValueL()
	{
	if (!iDialog)
		return -1;
    CEikProgressInfo* info = iDialog->GetProgressInfoL();
	return info->CurrentValue();
	}*/

CEikProgressInfo* CProgressBar::GetProgressInfoL()
	{
	if (!iDialog)
		return NULL;
	return iDialog->GetProgressInfoL();
	}

// ---------------------------------------------------------------------------
// CProgressBar::IncreaseProgressL()
// Increases the progress bar.
// ---------------------------------------------------------------------------
// 
TBool CProgressBar::IncreaseProgressL( TInt aTickLength )
    {
    if ( !iDialog )
        return EFalse;
    
    iTimer->Cancel();
	if (iTimeOut.Int() > 0)
		iTimer->AfterTime(iTimeOut);
	
    CEikProgressInfo* info = iDialog->GetProgressInfoL();
    if (info->CurrentValue() + aTickLength <= info->Info().iFinalValue)
    	info->IncrementAndDraw( aTickLength );
    
    if ( info->CurrentValue() >= info->Info().iFinalValue )
        {
      	FinishRequestL();
      	return EFalse;
        }
    return ETrue;
    }


void CProgressBar::TimerExpiredL(TAny* src)
	{
	this->FinishRequestL();
	}

// ---------------------------------------------------------------------------
// CProgressBar::UpdateRequestL()
// Updates request with some message.
// ---------------------------------------------------------------------------
// 
void CProgressBar::UpdateTextL( const TDesC& aMessage )
    {
    if (!iDialog)
    	return;
    
    iDialog->SetTextL( aMessage );
/*    if ( iWaitDialog )
        {
        iWaitDialog->SetTextL( aMessage );
        }
    else if ( iProgressDialog )
        {
        iProgressDialog->SetTextL( aMessage );
        }*/
    }

// End of File
