/*
 * ============================================================================
 *  Name        : CProgressBar from ProgressBar.h
 *  Part of     : GeoPicture
 *  Created     : 26.11.2008 by BITWEEN S.R.L.
 *  Description : CProgressBar declaration.
 *  Version     : 1.0
 *  Copyright   : BITWEEN S.R.L.
 * ============================================================================
 */

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

// INCLUDES 
#include <e32base.h>
#include <e32std.h>
#include <aknprogressdialog.h>          // MProgressDialogCallback
#include <FuzzyByte\TimeOutTimer.h> 

// LIBS
// commonengine.lib( StringLoader ), eikcdlg.lib( CAknWaitDialog ), 
// eikctl.lib( CAknProgressDialog )

// CAPS
// None

// FORWARD DECLARATIONS
class CAknWaitDialog;



class MProgressBarObserver
    {
public:
	virtual void RequestCancelled() = 0;
    };


// CLASS DECLARATION
/**
 * CProgressBar class.
 * CProgressBar declaration.
 * 
 */
class CProgressBar: public CBase, public MProgressDialogCallback, public MTimeOutNotifier

    {      
    public: // Constructors and destructor

        /**
         * NewL()
         * Creates new CProgressBar object.
         * @param Pointer to the MProgressBarObserver object.
         * If NULL, Cancel button will be hide.
         * @return Pointer to the created instance of CProgressBar.
         */
        static CProgressBar* NewL( MProgressBarObserver* aObserver );

        /**
         * NewLC()
         * Creates new CProgressBar object.
         * @param Pointer to the MProgressBarObserver object.
         * If NULL, Cancel button will be hide.
         * @return Pointer to the created instance of CProgressBar.
         */
        static CProgressBar* NewLC( MProgressBarObserver* aObserver );

        /**
         * ~CProgressBar()
         * Destructor.
         */
        ~CProgressBar();

    public: // New functions
        
        /** 
         * FinishRequestL()
         * Finishes current request state.
         * @return Last state.
         */
        void FinishRequestL();

        /**
         * StartRequestL()
         * Starts new request state.
         * @param aState State to start.
         * @param aProgressTick Progress length count.
         */
        void StartRequestL( TInt aResDialog, TTimeIntervalSeconds aTimeOutInSec, TInt aFinalProgressTick=-1 );

        /**
         * IncreaseProgressL()
         * Increases the progress bar.
         * @param aTickLength Amount fo progress to increase.
         */
        TBool IncreaseProgressL( TInt aTickLength );

        /**
         * UpdateRequestL()
         * Updates request with some message.
         * @param aMessage Message to be set up.
         */
        void UpdateTextL( const TDesC& aMessage );
                
        CEikProgressInfo* GetProgressInfoL();

    protected: // From MProgressDialogCallback

        /**
         * DialogDismissedL()
         * Called when wait dialog is cancelled.
         * @param aButtonId Button Id.
         */
        void DialogDismissedL( TInt aButtonId );

    private: // Constructors
        virtual void TimerExpiredL(TAny* src);

        /**
         * CProgressBar()
         * Default C++ constructor.
         * @param Pointer to the MProgressBarObserver object.
         */
        CProgressBar( MProgressBarObserver* aObserver );

        /**
         * ConstructL()
         * Default EPOC constructor.
         */
        void ConstructL();

    private: // data

        MProgressBarObserver*           iObserver;
  
        CAknProgressDialog*				iDialog;
        
        TTimeIntervalSeconds iTimeOut;
        CTimeOutTimer* iTimer;
//        CAknWaitDialog*                 iWaitDialog;
//        CAknProgressDialog*             iProgressDialog;
   };

#endif // PROGRESSBAR_H

// End of File
