/*
============================================================================
 Name        : Installer.h
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : CInboxAttachment declaration
============================================================================
*/

#ifndef INSTALLER_H
#define INSTALLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <MSVAPI.H>

// CLASS DECLARATION



/**
*  CInboxAttachment
*
*/
class CInboxAttachment : public CBase, public MMsvSessionObserver
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CInboxAttachment();

        /**
        * Two-phased constructor.
        */
	static CInboxAttachment* NewL();

        /**
        * Two-phased constructor.
        */
	static CInboxAttachment* NewLC();

	TInt BT_IRDA_SaveFileToInboxL(RFs& fs, const TDesC& details, const TDesC& aFileName);	
	TBool BT_IRDA_GetFileFromInboxL(RFs& fs, TDes& path);
	
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

private:
	TInt AddAttachmentL(CMsvSession& aMsvSession, RFile& aFile, TMsvId aParent);
	TMsvId CreateMessageL( const TDesC& aFromDetails, const TDesC& aFileName, CMsvEntry* aInbox, const TUid& aMtmID) const;
	void MakeValidFileName(const RFs& aFs, TDes& aFileName) const;

	/**
        * Constructor for performing 1st stage construction
        */
	CInboxAttachment();

	void ConstructL();

};

#endif // INSTALLER_H
