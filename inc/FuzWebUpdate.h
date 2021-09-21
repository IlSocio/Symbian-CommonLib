

#ifndef FuzWebUpdate_H
#define FuzWebUpdate_H

#include <e32base.h>
#include <e32cons.h>
#include <e32uid.h>
#include <f32file.h>
#include <es_sock.h>
#include <FuzzyByte\FileLogger.h>
#include <FuzzyByte\HttpEngine.h>
#include <FuzzyByte\asyncwaiter.h>


// INCLUDES
//#include "WebClientObserver.h"

//class CWebClientEngine;


class CFuzWebUpdate : public CBase //, private MWebClientObserver
    {
    public:
        static CFuzWebUpdate* NewL(const TDesC& aServerUrl);
        static CFuzWebUpdate* NewLC(const TDesC& aServerUrl);
		TInt IsThereAnUpdatedVersionL(TUint32 isReg, const TDesC& version, TUint32 brandId, TDes8& url, TBool& updateAvail);
		TInt DownloadUpdatedVersionL(const TDesC8& downloadURL, RFs& fs, const TDesC& filename);
		void CancelDownloadL();

        /**
        * ~CWebClientAppUi()
        * Destructor for CWebClientAppUi.
        */
        ~CFuzWebUpdate();

    private:
		/**
        * ConstructL()
        * Perform the second phase construction of a CWebClientAppUi object.
        */
        void ConstructL(const TDesC& aServerUrl);

        CFuzWebUpdate();

		TInt GetResponseSize(const TDesC8& data);
		void GetResponseURL(const TDesC8& webData, TDes8& url);

//        void ClientEventL( const TWebClientEvents event, const TDesC& descr );
//		void ClientErrorL( const TInt error, const TDesC& aErrorData );
//        void ClientBodyReceivedL( const TDesC8& aBodyData );

    private: // Data
		RBuf				iServerUrl;
    	CHttpEngine*		iClient;
    	CHttpSession* 		iSession;
    	RSocketServ 		iSocketServ;
    	__LOG_DECLARATION_MEMBER
        };

#endif // WEBCLIENTAPPUI_H

