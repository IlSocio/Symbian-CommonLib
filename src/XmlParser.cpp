/*
 ============================================================================
 Name		: XmlHandler.cpp
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : FuzzyByte
 Description : CXmlParser implementation
 ============================================================================
 */

// TODO: Usa CLineReader e CLongRunTask

#include "XmlParser.h" 
const TInt KFileBufferSize = 1024; // buffer size for file reading
_LIT8( KXmlMimeType, "text/xml" );

CXmlParser::CXmlParser(MXmlNotifier& notif) :
	CActive(EPriorityStandard), iNotifier(notif) // Standard priority
	{
	}

CXmlParser* CXmlParser::NewLC(MXmlNotifier& notif, MContentHandler& aHandler)
	{
	CXmlParser* self = new (ELeave) CXmlParser(notif);
	CleanupStack::PushL(self);
	self->ConstructL(aHandler);
	return self;
	}

CXmlParser* CXmlParser::NewL(MXmlNotifier& notif, MContentHandler& aHandler)
	{
	CXmlParser* self = CXmlParser::NewLC(notif, aHandler);
	CleanupStack::Pop(); // self;
	return self;
	}

void CXmlParser::ConstructL(MContentHandler& aHandler)
	{
	CActiveScheduler::Add(this); // Add to scheduler
    iParser = CParser::NewL( KXmlMimeType, aHandler );
	}

CXmlParser::~CXmlParser()
	{
	Cancel(); // Cancel any request, if outstanding
    iFile.Close();    
    delete iParser;
    delete iBuffer;
	}

void CXmlParser::DoCancel()
    {
    iParser->ParseEndL();
    iFile.Close();
    delete iBuffer;
    iBuffer = NULL;
    }
 
void CXmlParser::StartParsingWithAoL(RFs& aFs, const TDesC& aFileName )
    {
     // Remember to cancel any outstanding request first.
    if ( IsActive() )
        {
        Cancel();
        }   
    TInt err = iFile.Open( aFs, aFileName, EFileRead );
    if (err != KErrNone)
    	{
    	iNotifier.ParsingCompleteL(this);
    	return;
    	}
 
    // Create a buffer to store the file content.
    // Note that this method uses active object to read the file.
    // So we have to call SetActive() at the end. Then we call CParser::ParseL()
    // in RunL() method.
    delete iBuffer;
    iBuffer = HBufC8::NewL( KFileBufferSize );
    TPtr8 bufferPtr( iBuffer->Des() );
    iFile.Read( bufferPtr, KFileBufferSize, iStatus );
    SetActive();
 
    // Tell the parser that we are about to parse a XML document.    
    iParser->ParseBeginL();
    }


void CXmlParser::RunL()
	{
    if ( KErrNone != iStatus.Int() )
    	return;
	// If the buffer length is zero, it means if we have reached
	// the end of the file.
	if ( iBuffer->Length() == 0) 
		{
		iParser->ParseEndL();
		iFile.Close();
		delete iBuffer;
		iBuffer = 0;
		// Notify the caller
		iNotifier.ParsingCompleteL(this);
		} 
	// Otherwise, we continue reading the next chunk of the XML file.
	else
		{
		// Parse the next "part" of the XML document.
		iParser->ParseL( *iBuffer );

		// Read the next chunk of the file.
		TPtr8 bufferPtr( iBuffer->Des() );
		iFile.Read( bufferPtr, KFileBufferSize, iStatus );

		// Don't forget to call this... :)
		SetActive();
		}
	}


TInt CXmlParser::RunError(TInt aError)
	{
	return aError;
	}
