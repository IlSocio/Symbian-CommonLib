/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __SDP_ATTRIBUTE_PARSER_PAN__
#define __SDP_ATTRIBUTE_PARSER_PAN__

/** BTPointToPoint application panic codes */
enum TSdpAttributeParserPanics 
    {
    ESdpAttributeParserInvalidCommand = 1,
    ESdpAttributeParserNoValue,
    ESdpAttributeParserValueIsList,
    ESdpAttributeParserValueTypeUnsupported
    };


/** Panic Category */
_LIT( KPanicSAP, "SAP" );


inline void Panic( TSdpAttributeParserPanics aReason )
    { 
    User::Panic( KPanicSAP, aReason );
    }


#endif // __SDP_ATTRIBUTE_PARSER_PAN__

// End of File
