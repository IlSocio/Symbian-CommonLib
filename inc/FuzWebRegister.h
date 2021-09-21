
#ifndef FUZWEBREGISTER_H
#define FUZWEBREGISTER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>
#include <e32cmn.h>
#include <f32file.h>

// INCLUDES
#include <FuzzyByte\HttpSession.h>
#include <FuzzyByte\HttpEngine.h>
#include <FuzzyByte\AsyncWaiter.h>
#include <es_sock.h>


  enum TResponseID
		{
		EErrNone,
		EGenericError,						// Da Gestire... "Errore Generico"
        EInvalidIMEI,
        ESoftwareIDNotFound,
        ELicenseIDNotActivated,
        ELicenseIDHasDifferentSoftware,
        ELicenseIDNotFound,					// Da Gestire... "LicenseID non valido"
        ELicenseIDHasDifferentIMEI,			// Da Gestire... "LicenseID gia' registrato"
        EBannedFor10Minutes,				// Da Gestire... "Troppe richieste, riprovare piu' tardi"
        EUnknownDataReceived,				// Da Gestire... "Errore nella risposta del server, riprovare piu' tardi"
		ETransmissionError,					// Da Gestire... "Errore nella comunicazione, riprovare piu' tardi"
		EMaxResponseId
		};


class CFuzWebRegister : public CBase
    {
    public:
        static CFuzWebRegister* NewL(const TDesC& aServerUrl);
        static CFuzWebRegister* NewLC(const TDesC& aServerUrl);
		TInt SendRegistrationRequestL(TUint32 brandId, const TDesC& version, const TDesC& licenseID, TDes& regCode, TResponseID& resp);
		void CancelRegistrationRequestL();

        /**
        * ~CWebClientAppUi()
        * Destructor for CWebClientAppUi.
        */
        ~CFuzWebRegister();

    private:
		/**
        * ConstructL()
        * Perform the second phase construction of a CWebClientAppUi object.
        */
        void ConstructL(const TDesC& aServerUrl);

        /**
        * CWebClientAppUi()
        * First phase construction of CWebClientAppUi.
        */
        CFuzWebRegister();

		TResponseID GetResponseStatus(const TDesC8& data);
		void GetResponseRegist(const TDesC8& data, TDes& regcode);


    private: // Data
		RBuf			   iServerUrl;
		RSocketServ		   iSocketServ;
        CHttpSession*      iSession;
        CHttpEngine*       iClient;
        };

#endif
