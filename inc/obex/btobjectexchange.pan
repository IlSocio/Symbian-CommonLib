

#ifndef __BTOBJECTEXCHANGE_PAN__
#define __BTOBJECTEXCHANGE_PAN__

// INCLUDES
#include <e32std.h>

/** BTObjectExchange application panic codes */
enum TBTObjectExchangePanics 
    {
    EBTObjectExchangeBasicUi = 1,
    EBTObjectExchangeReceiverInvalidState,
    EBTObjectExchangeSenderExists,
    EBTObjectExchangeReceiverExists,
    EBTObjectExchangeSenderInvalidState,
    EBTObjectExchangeNoSender,
    EBTObjectExchangeAddMessage,
    EBTObjectExchangeNextRecordRequestComplete,
    EBTObjectExchangeAttributeRequestResult,
    EBTObjectExchangeAttributeRequestComplete,
    EBTObjectExchangeInvalidControlIndex,
    EBTObjectExchangeProtocolRead,
    EBTObjectExchangeAttributeRequest,
    EBTObjectExchangeSdpRecordDelete,
    EBTObjectExchangeUnexpectedState,
    EBTObjectExchangeUnexpectedLogicState
    };


/** Panic Category */
_LIT( KPanicBTOBEX, "BTOBEX" );


inline void Panic( TBTObjectExchangePanics aReason )
    { 
    User::Panic( KPanicBTOBEX, aReason );
    }

#endif // __BTOBJECTEXCHANGE_PAN__

// End of File
