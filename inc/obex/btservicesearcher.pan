/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __BTSERVICE_SEARCHER_PAN__
#define __BTSERVICE_SEARCHER_PAN__

// INCLUDES
#include <e32std.h>

/** BTServiceSearcher application panic codes */
enum TBTServiceSearcherPanics 
    {
    EBTServiceSearcherNextRecordRequestComplete = 1,
    EBTServiceSearcherAttributeRequestResult,
    EBTServiceSearcherAttributeRequestComplete,
    EBTServiceSearcherInvalidControlIndex,
    EBTServiceSearcherProtocolRead,
    EBTServiceSearcherAttributeRequest,
    EBTServiceSearcherSdpRecordDelete
    };


/** Panic Category */
_LIT( KPanicServiceSearcher, "ServiceSearcher" );
_LIT( KPanicServiceAdvertiser, "BTServiceAdvertiser");


inline void Panic( TBTServiceSearcherPanics aReason )
    { 
    User::Panic( KPanicServiceSearcher, aReason );
    }

#endif // __BTSERVICE_SEARCHER_PAN__

// End of File
