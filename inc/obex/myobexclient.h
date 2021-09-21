
// LIBRARY 		sdpagent.lib sdpdatabase.lib bluetooth.lib btextnotifiers.lib irobex.lib irda.lib


#ifndef __OBJECTEXCHANGECLIENT_H__
#define __OBJECTEXCHANGECLIENT_H__

// INCLUDES
#include <e32base.h>
#include <obex.h>

// CONSTANTS
//#include <bteng.h>
#include <aknwaitdialog.h> 

// FORWARD DECLARATIONS
class CMyObexServiceSearcher;


// CLASS DECLARATIONS
/**
* CMyObexClient
* Handles the client interactions with the remote server, namely
* the connection, sending of messages and disconnection.
*/
class CMyObexClient : public CActive, public MProgressDialogCallback
    {
    public: // Constructors and destructor.
        /**
        * NewL
        * Construct a CMyObexClient
        * @param aLog the log to send output to
        * @return a pointer to the created instance of
        * CMyObexClient
        */
        static CMyObexClient* NewL( );

        /**
        * NewLC
        * Construct a CMyObexClient
        * @param aLog the log to send output to
        * @return a pointer to the created instance
        * of CMyObexClient
        */
        static CMyObexClient* NewLC( );

        /**
        * ~CMyObexClient
        * Destroy the object and release all memory objects.
        * Close any open sockets
        */
        virtual ~CMyObexClient();

		void ShowQueryAndSendFileL(const TDesC& filename, TInt resWaitDialog, TBool bluetooth=ETrue, TInt size=0);

        /**
        * ConnectL
        * Connect to an OBEX service on a remote machine
        */
        void BluetoothConnectL();

        /**
        * DisconnectL
        * Disconnect from the remote machine, by sending an
        * OBEX disconnect, and closing the transport on (and
        * regardless of) response from the server.
        */
        void DisconnectL();

        /**
        * SendObjectL
        * Send a file to a service on a remote machine
        */
        void SendObjectL(const TDesC& aName, TInt size=0);


        /**
        * StopL
        * Send the OBEX aborts command to the remote machine
        */
        void StopL();

        /**
        * IsConnected
        * @return ETrue if the client is connected
        */
        TBool IsConnected();

        /**
        * IsBusy
        * @return ETrue if the client is performing some operation.
        */
        TBool IsBusy();

		TInt BytesSent();

    protected:    // from CActive
		TBool DoConnectL(TBool bluetooth=ETrue);
		TBool DoSendFileL(const TDesC& filename, TInt size=0);

        /**
        * DoCancel
        * Cancel any outstanding requests
        */
        void DoCancel();

        /**
        * RunL
        * Respond to an event
        */
        void RunL();

    private:    // Constructors

		TBool BluetoothIsOnL();
		TBool BluetoothTurnOnL();

        /**
        * CMessageClient
        * @param aLog the log to send output to
        */
        CMyObexClient( );

        /**
        * ConstructL
        * Perform second phase construction of this object
        */
        void ConstructL();

        /**
        * ConnectToServerL
        * Connect to the server
        */
        void BluetoothConnectToServerL();
		void InfraredConnectToServerL();

		void DialogDismissedL( TInt aButtonId );
		void ShowWaitDialogL( TInt aResWaitDialog );
		void DismissWaitDialogL();
    private:    // data
		void StartWait();
        /**
        * TState
        * The state of the active object, determines behaviour
        * within the RunL method.
        * EWaitingToGetDevice waiting for the user to select a device
        * EGettingDevice searching for a device
        * EGettingService searching for a service
        * EGettingConnection connecting to a service on a remote machine
        * EWaitingToSend sending a message to the remote machine
        * EDisconnecting disconnecting the server
        */
        enum TState
            {
                EWaitingToGetDevice,
                EGettingDevice,
                EGettingService,
                EGettingConnection,
                EWaitingToSend,
                EDisconnecting
            };

        /**
        * iState the state of the active object,
        * determines behaviour within the RunL method.
        */
        TState iState;

        /**
        * iServiceSearcher searches for service this
        * client can connect to.
        * Owned by CMyObexClient
        */
        CMyObexServiceSearcher* iServiceSearcher;

        /**
        * iClient manages the OBEX client connection
        * Owned by CMyObexClient
        */
        CObexClient* iClient;

        /**
        * iCurrObject the OBEX object to transfer
        * Owned by CMyObexClient
        */
        CObexBaseObject* iCurrObject;

		CActiveSchedulerWait* iWait;
		TInt iError;
        CAknWaitDialog*         iWaitDialog;
//		__FLOG_DECLARATION_MEMBER;
    };

#endif // __OBJECTEXCHANGECLIENT_H__

// End of File
