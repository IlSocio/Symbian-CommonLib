/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES
#include <stringloader.h>

#include "BTServiceSearcher.h"
#include "BTServiceSearcher.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBTServiceSearcher::CBTServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CBTServiceSearcher::CBTServiceSearcher()
:  iIsDeviceSelectorConnected( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::~CBTServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CBTServiceSearcher::~CBTServiceSearcher()
    {
	Close();
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;
    }


void CBTServiceSearcher::Close()
	{
    if ( iIsDeviceSelectorConnected )
        {
        iDeviceSelector.CancelNotifier( KDeviceSelectionNotifierUid );
        iDeviceSelector.Close();
        iIsDeviceSelectorConnected = EFalse;
        }	
	}
	
// ----------------------------------------------------------------------------
// CBTServiceSearcher::SelectDeviceByDiscoveryL()
// Select a device.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::SelectDeviceByDiscoveryL( TRequestStatus& 
                                                  aObserverRequestStatus )
    {
    if ( !iIsDeviceSelectorConnected )
        {
        User::LeaveIfError( iDeviceSelector.Connect() );
        iIsDeviceSelectorConnected = ETrue;
        }

    //  Request a device selection 
    TBTDeviceSelectionParamsPckg selectionFilter;
    selectionFilter().SetUUID( ServiceClass() );

    iDeviceSelector.StartNotifierAndGetResponse( 
        aObserverRequestStatus, 
        KDeviceSelectionNotifierUid, 
        selectionFilter, 
        iResponse );
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::FindServiceL()
// Find a service on the specified device.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::FindServiceL( TRequestStatus& aObserverRequestStatus )
    {
    if ( !iResponse().IsValidBDAddr() )
        {
        User::Leave( KErrNotFound );
        }
    iHasFoundService = EFalse;

    // delete any existing agent and search pattern
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;

    iAgent = CSdpAgent::NewL( *this, BTDevAddr() );

    iSdpSearchPattern = CSdpSearchPattern::NewL();
   
    iSdpSearchPattern->AddL( ServiceClass() );
    // return code is the position in the list that the UUID is inserted at 
    // and is intentionally ignored
  
    iAgent->SetRecordFilterL( *iSdpSearchPattern );

    iStatusObserver = &aObserverRequestStatus;

    iAgent->NextRecordRequestL();
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::NextRecordRequestComplete()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::NextRecordRequestComplete( 
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount )
    {
    TRAPD( error,
        NextRecordRequestCompleteL( aError, aHandle, aTotalRecordsCount );
    );

    if ( error != KErrNone ) 
        {
        Panic( EBTServiceSearcherNextRecordRequestComplete );
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::NextRecordRequestCompleteL()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::NextRecordRequestCompleteL( 
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount )
    {
    if ( aError == KErrEof )
        {
        Finished();
        return;
        }

    if ( aError != KErrNone )
        {
        // iLog.LogL( KErrNRRCErr, aError );
        Finished( aError );
        return;
        }

    if ( aTotalRecordsCount == 0 )
        {
		// iLog.LogL( _L("No Records") );
        Finished( KErrNotFound );
        return;
        }

    //  Request its attributes
    iAgent->AttributeRequestL( aHandle, KSdpAttrIdProtocolDescriptorList );
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestResult()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestResult( 
    TSdpServRecordHandle aHandle, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue )
    {
    TRAPD( error,
        AttributeRequestResultL( aHandle, aAttrID, aAttrValue );
    );
    if ( error != KErrNone )
        {
        Panic( EBTServiceSearcherAttributeRequestResult );
        }

    // Ownership has been transferred
    delete aAttrValue;
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestResultL()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestResultL( 
    TSdpServRecordHandle,
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue )
    {
    __ASSERT_ALWAYS( aAttrID == KSdpAttrIdProtocolDescriptorList, 
                                User::Leave( KErrNotFound ) );

    TSdpAttributeParser parser( ProtocolList(), *this );

    // Validate the attribute value, and extract the RFCOMM channel
    aAttrValue->AcceptVisitorL( parser );

    if ( parser.HasFinished() )
        {
        // Found a suitable record so change state
        iHasFoundService = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestComplete()
// Process the arrtibute request completion.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestComplete( TSdpServRecordHandle aHandle,
                                                   TInt aError )
    {
    TRAPD( error, AttributeRequestCompleteL( aHandle, aError );
    );
    if ( error != KErrNone )
        {
        Panic( EBTServiceSearcherAttributeRequestComplete );
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::AttributeRequestCompleteL()
// Process the arrtibute request completion.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::AttributeRequestCompleteL( TSdpServRecordHandle, 
                                                    TInt aError )
    {
    if ( aError != KErrNone )
        {
        //iLog.LogL( _L("Can't Get Attribute"), aError );
        }
    else if ( !HasFinishedSearching() )
        {
        // have not found a suitable record so request another
        iAgent->NextRecordRequestL();
        }
    else
        {
        // iLog.LogL( _L("attr req com") );
        Finished();
        }
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::Finished()
// The search has finished and notify the observer 
// that the process is complete.
// ----------------------------------------------------------------------------
//
void CBTServiceSearcher::Finished( TInt aError )
    {
    if ( aError == KErrNone && !HasFoundService() )
        {
        User::RequestComplete( iStatusObserver, KErrNotFound );
        }
	else 
		{
        User::RequestComplete( iStatusObserver, aError );
		}
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::HasFinishedSearching()
// Is the instance still wanting to search.
// ----------------------------------------------------------------------------
//
TBool CBTServiceSearcher::HasFinishedSearching() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::BTDevAddr()
// Returns the bluetooth device address.
// ----------------------------------------------------------------------------
//
const TBTDevAddr& CBTServiceSearcher::BTDevAddr()
    {
    return iResponse().BDAddr();
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::ResponseParams()
// Returns information about the device selected by the user.
// ----------------------------------------------------------------------------
//
const TBTDeviceResponseParams& CBTServiceSearcher::ResponseParams()
    {
    return iResponse();
    }

// ----------------------------------------------------------------------------
// CBTServiceSearcher::HasFoundService()
// True if a service has been found.
// ----------------------------------------------------------------------------
//
TBool CBTServiceSearcher::HasFoundService() const
    {
    return iHasFoundService;
    }

// End of File
