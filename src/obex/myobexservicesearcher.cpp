/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES
#include <bt_sock.h>

#include "MyObexServiceSearcher.h"
#include "BTObjectExchange.pan"
#include "MyObexProtocolConstants.h"

// ----------------------------------------------------------------------------
// TSdpAttributeParser::SSdpAttributeNode gObexProtocolListData[]
// Construct a TSdpAttributeParser
// ----------------------------------------------------------------------------
//
const TSdpAttributeParser::SSdpAttributeNode gObexProtocolListData[] = 
    {
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KL2CAP },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KRFCOMM },
        { TSdpAttributeParser::EReadValue, ETypeUint, KRfcommChannel },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KBtProtocolIdOBEX },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::EFinished }
    };

// ----------------------------------------------------------------------------
// TStaticArrayC
// Provides a type and size, safe method of using static arrays.
// ----------------------------------------------------------------------------
//
const TStaticArrayC<TSdpAttributeParser::SSdpAttributeNode> gObexProtocolList =
CONSTRUCT_STATIC_ARRAY_C( gObexProtocolListData );

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMyObexServiceSearcher* CMyObexServiceSearcher
::NewL( )
    {
    CMyObexServiceSearcher* self = CMyObexServiceSearcher
    ::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMyObexServiceSearcher* CMyObexServiceSearcher
::NewLC( )
    {
    CMyObexServiceSearcher* self = new ( 
        ELeave ) CMyObexServiceSearcher( );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::CMyObexServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CMyObexServiceSearcher::CMyObexServiceSearcher()
: CBTServiceSearcher(),
  iServiceClass( KServiceClass ),
  iPort( -1 )
    {
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::~CMyObexServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CMyObexServiceSearcher::~CMyObexServiceSearcher()
    {
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMyObexServiceSearcher::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CMyObexServiceSearcher::ServiceClass() const
    {
    return iServiceClass;
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::ProtocolList()
// The list of Protocols required by the service
// ----------------------------------------------------------------------------
//
const TSdpAttributeParser::TSdpAttributeList& CMyObexServiceSearcher
::ProtocolList() const
    {
    return gObexProtocolList;
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CMyObexServiceSearcher::FoundElementL( TInt aKey, 
                                                    const CSdpAttrValue& aValue )
    {
    __ASSERT_ALWAYS( aKey == static_cast<TInt>( KRfcommChannel ), 
        Panic( EBTObjectExchangeProtocolRead ) );
    iPort = aValue.Uint();
    }

// ----------------------------------------------------------------------------
// CMyObexServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CMyObexServiceSearcher::Port()
    {
    return iPort;
    }

// End of File
