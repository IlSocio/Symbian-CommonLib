/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __OBJECTEXCHANGESERVICESEARCHER_H__
#define __OBJECTEXCHANGESERVICESEARCHER_H__

// INCLUDES
#include <e32base.h>

#include "BTServiceSearcher.h"
#include "MyObexProtocolConstants.h"

// CLASS DECLARATIONS

/**
* CMyObexServiceSearcher
* Searches the remote SDP database for suitable OBEX servers.
*/
class CMyObexServiceSearcher : public CBTServiceSearcher
    {
    public: // Constructors and destructor.
        
        /**
        * NewL()
        * Construct a CMyObexServiceSearcher
        * @param aLog the log to send output to
        * @return a pointer to the created instance of 
        * CMyObexServiceSearcher
        */
        static CMyObexServiceSearcher* NewL( );

        /**
        * NewLC()
        * Construct a CMyObexServiceSearcher
        * @param aLog the log to send output to
        * @return a pointer to the created instance of 
        * CMyObexServiceSearcher
        */
        static CMyObexServiceSearcher* NewLC( );

        /**
        * ~CMyObexServiceSearcher()
        * Destroy the object and release all memory objects
        */
        virtual ~CMyObexServiceSearcher();

        /**
        * Port()
        * @return the port to connect to on the remote machine
        */
        TInt Port();

    protected:  // Functions from base classes.

        /**
        * ServiceClass()
        * The service class to search for
        * @return the service class UUID
        */
        const TUUID& ServiceClass() const;

        /**
        * ProtocolList()
        * The list of Protocols required by the service.
        */
        const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const;

        /**
        * FoundElementL()
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, const CSdpAttrValue& aValue );

    private:    // Constructors
        /**
        * CMyObexServiceSearcher()
        * @param aLog the log to send output to
        * Constructs this object
        */
        CMyObexServiceSearcher();

        /**
        * ConstructL()
        * 2nd phase construction of this object
        */
        void ConstructL();

    private:    // data

        /** 
        * iServiceClass
        * the service class to search for 
        */
        TUUID iServiceClass;

        /** 
        * iPort
        * the port to connect to on the remote machine 
        */
        TInt iPort;
    };

#endif // __OBJECTEXCHANGESERVICESEARCHER_H__

// End of File
