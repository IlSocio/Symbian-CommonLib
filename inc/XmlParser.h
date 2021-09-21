/*
 ============================================================================
 Name		: XmlHandler.h
 Author	  : Marco Bellino
 Version	 : 1.0
 Copyright   : FuzzyByte
 Description : CXmlParser declaration
 ============================================================================
 */

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <f32file.h>  //Link against efsrv.lib 
#include <xml\contenthandler.h> // for MContentHandler
#include <xml\parser.h> // for CParser


using namespace Xml;
 
class MXmlNotifier
    {
    public: // New functions
        virtual void ParsingCompleteL(TAny* src) = 0;
    };

 
class CXmlParser : public CActive
	{
public:
	// Cancel and destroy
	~CXmlParser();

	// Two-phased constructor.
	static CXmlParser* NewL(MXmlNotifier& notif, MContentHandler& aHandler);

	// Two-phased constructor.
	static CXmlParser* NewLC(MXmlNotifier& notif, MContentHandler& aHandler);

public:
    void StartParsingWithAoL(RFs& aFs, const TDesC& aFileName );
    
private:
	// C++ constructor
	CXmlParser(MXmlNotifier& notif);

	// Second-phase constructor
	void ConstructL(MContentHandler& aHandler);

private:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	TInt RunError(TInt aError);

private:
	MXmlNotifier&	 	 iNotifier;
    CParser*             iParser;
    HBufC8*              iBuffer;
    RFile                iFile;
	};

#endif // XMLHANDLER_H
